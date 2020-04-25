#include "Model.h"

#include <iomanip>
#include <unordered_map>
#include "Assert.h"
#include "TextureConversion.h"

void Vertex::print()
{
	std::ios::sync_with_stdio(false);
	std::cin.tie(NULL);

	std::cout << std::fixed << std::setprecision(2)
		<< std::setw(6) << this->x << " "
		<< std::right << std::setw(6) << this->y << " "
		<< std::right << std::setw(6) << this->z << " "
		<< std::right << std::setw(6) << this->unknown1 << " "
		<< std::right << std::setw(6) << this->a << " "
		<< std::right << std::setw(6) << this->b << " "
		<< std::right << std::setw(6) << this->c << " "
		<< std::right << std::setw(6) << this->d << " "
		<< std::right << std::setw(6) << this->u << " "
		<< std::right << std::setw(6) << this->v << " "
		<< std::right << std::setw(6) << this->i1 << " "
		<< std::right << std::setw(6) << this->i2 << " "
		<< std::right << std::setw(6) << this->i3 << " "
		<< std::right << std::setw(6) << this->i4 << " "
		<< std::right << std::setw(6) << this->i5 << " "
		<< std::right << std::setw(6) << this->i6 << " "
		<< "\n";
}

bool Model::Load(std::string& filename, const MaterialDictionary* dictionary)
{
	std::ifstream file(filename, std::ios_base::binary);

	if (file.is_open() == false)
	{
		std::cout << "Unable to open model file: " << filename << "\n";
		return false;
	}
	
	std::cout << "Loading model file: " << filename << "\n";

	this->filename = filename;
	this->dictionary = dictionary;
	char count;
	
	this->kerbLeader.resize(4);
	file.read(&this->kerbLeader[0], 4);

	file.read(reinterpret_cast<char*>(&this->kerbSerialNumber), sizeof(this->kerbSerialNumber));
	
	count = file.get();
	this->kerbType.resize(count);
	file.read(&this->kerbType[0], count);

	file.read(reinterpret_cast<char*>(&this->nodeCount), sizeof(this->nodeCount));

	for (int i = 0; i < this->nodeCount; ++i)
	{
		int value;
		file.read(reinterpret_cast<char*>(&value), sizeof(value));
		this->nodeParents.push_back(value);
	}

	for (int i = 0; i < this->nodeCount; ++i)
	{
		float data[16];		
		file.read(reinterpret_cast<char*>(&data[0]), sizeof(data));
		
		glm::mat4 matrix = glm::make_mat4(&data[0]); // Do we need to transpose? Row or Column order?
		glm::mat4 transposedMatrix = glm::transpose(matrix);

		this->nodeMatrixes.push_back(std::move(transposedMatrix));
	}

	for (int i = 0; i < this->nodeCount; ++i)
	{
		std::string nodeName;
		count = file.get();

		nodeName.resize(count);

		file.read(&nodeName[0], count);
		this->nodeNames.push_back(std::move(nodeName));
	}

	//////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////
	// Should be okay up to here
	//////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////

	// After loading node names and parent layout, 
	// 1. Given a number of "texture objects" Each object can have multiple textures
	// 2. Given the parent node number of each texture object
	// 
	//	For each texture object
	// 1. Read Indexes (given in bytes)
	// 2. Read Vertexes (given in bytes)
	//
	// For each texture in the texture object
	// 3. Read Texture size and file offset (given in bytes)
	// 4. Read Texture name
	//
	// 5. Read texture footer


	int nodeObjectCount;
	file.read(reinterpret_cast<char*>(&nodeObjectCount), sizeof(nodeObjectCount));

	for (int i = 0; i < nodeObjectCount; ++i)
	{
		int index; // TODO: Store this somewhere but what is it?!
		file.read(reinterpret_cast<char*>(&index), sizeof(index));
		this->nodeObjectParents.push_back(index);
	}

	for (int i = 0; i < nodeObjectCount; ++i)
	{	
		NodeObject nodeObject;

		int nodeIndexesFileBytes;
		file.read(reinterpret_cast<char*>(&nodeIndexesFileBytes), sizeof(nodeIndexesFileBytes));

		for (int i = 0; i < nodeIndexesFileBytes; i+=sizeof(int))
		{
			int index; 
			file.read(reinterpret_cast<char*>(&index), sizeof(index));
			nodeObject.nodeIndexes.push_back(index);
		}

		int nodeVertsFileBytes;
		file.read(reinterpret_cast<char*>(&nodeVertsFileBytes), sizeof(nodeVertsFileBytes));
		
		for (int i = 0; i < nodeVertsFileBytes; i+=sizeof(Vertex))
		{
			Vertex vertex; // TODO: Decypher this data layout
			file.read(reinterpret_cast<char*>(&vertex), sizeof(Vertex));

			//if(vertex.unknown1 != 0.0f || vertex.a != 1.0f || vertex.b != 1.0f || vertex.c != 1.0f || vertex.d != 1.0f || vertex.i4 != 0 || vertex.i5 != 0 || vertex.i6 != 0) vertex.print();

			/*
			Assert(vertex.unknown1 == 0.0f, "vertex.unknown1 not equal to 0.0f as expected");
			Assert(vertex.a == 1.0f, "vertex.a not equal to 1.0f as expected");
			Assert(vertex.b == 1.0f, "vertex.b not equal to 1.0f as expected");
			Assert(vertex.c == 1.0f, "vertex.c not equal to 1.0f as expected");
			Assert(vertex.d == 1.0f, "vertex.d not equal to 1.0f as expected");
			Assert(vertex.i4 == 0, "vertex.i4 not equal to 0 as expected");
			Assert(vertex.i5 == 0, "vertex.i5 not equal to 0 as expected");
			Assert(vertex.i6 == 0, "vertex.i6 not equal to 0 as expected");
			*/

			nodeObject.nodeVertexes.push_back(std::move(vertex));
		}

		int textureCount;
		file.read(reinterpret_cast<char*>(&textureCount), sizeof(textureCount));

		for (int j = 0; j < textureCount; ++j)
		{
			TextureInfo texture;
			file.read(reinterpret_cast<char*>(&texture.unknown), sizeof(texture.unknown));
			file.read(reinterpret_cast<char*>(&texture.size), sizeof(texture.size));
			file.read(reinterpret_cast<char*>(&texture.fileOffset), sizeof(texture.fileOffset));

			count = file.get();
			texture.name.resize(count);
			file.read(&texture.name[0], count);

			nodeObject.textures.push_back(std::move(texture));
		}

		// Mystery data at end of texture names. Seems like (approx):
		// -xx.xx -xx.xx -xx.xx  (max/min values?)
		//  xx.xx  xx.xx  xx.xx
		//  x1.x1 y1.y1			 (normals? uv?)
		//  x2.x2 y2.y2	
		file.read(reinterpret_cast<char*>(&nodeObject.x1), sizeof(nodeObject.x1));
		file.read(reinterpret_cast<char*>(&nodeObject.y1), sizeof(nodeObject.y1));
		file.read(reinterpret_cast<char*>(&nodeObject.z1), sizeof(nodeObject.z1));
		file.read(reinterpret_cast<char*>(&nodeObject.x2), sizeof(nodeObject.x2));
		file.read(reinterpret_cast<char*>(&nodeObject.y2), sizeof(nodeObject.y2));
		file.read(reinterpret_cast<char*>(&nodeObject.z2), sizeof(nodeObject.z2));
		file.read(reinterpret_cast<char*>(&nodeObject.u1), sizeof(nodeObject.u1));
		file.read(reinterpret_cast<char*>(&nodeObject.v1), sizeof(nodeObject.v1));
		file.read(reinterpret_cast<char*>(&nodeObject.u2), sizeof(nodeObject.u2));
		file.read(reinterpret_cast<char*>(&nodeObject.v2), sizeof(nodeObject.v2));

		this->nodeObjects.push_back(std::move(nodeObject));
	}

	// Seems to be a trailer of 12 char(0)s at end of model
	for (int i = 0; i < 3; ++i)
	{
		int shouldBeZero;
		file.read(reinterpret_cast<char*>(&shouldBeZero), sizeof(shouldBeZero));
		Assert(shouldBeZero == 0, "This model has additional animation data");
	}

	// Followed by a single char that seems to be 0 if no additional data otherwise 1
	// If 1, remaining data is what, animation?
	count = file.get();

	for (int i = 0; i < count; ++i)
	{
		// do animations (??)
		// Looks like a bunch of vertices and matrixes - probably animation/movement data
	}
	
	file.close();
	return true;
}

bool Model::SaveAsOBJ()
{
	std::ios::sync_with_stdio(false);
	std::cin.tie(NULL);

	if (this->filename.empty())
	{
		return false;
	}

	std::string objFilename = this->filename.substr(0, this->filename.find_last_of('.')) + ".obj";
	std::string mtlFilename = this->filename.substr(0, this->filename.find_last_of('.')) + ".mtl";

	if (this->dictionary != nullptr)
	{		
		std::ofstream mtlFile{ mtlFilename };
		if (mtlFile.is_open() == false)
		{
			return false;
		}

		std::cout << "Writing MTL file: " << mtlFilename << "\n";

		for (const auto& material : this->dictionary->materials)
		{
			if (material.materialType == "standard")
			{
				mtlFile << "newmtl " << material.materialName << "\n"
					<< "Ka 1.000 1.000 1.000\n"
					<< "Kd 1.000 1.000 1.000\n"
					<< "Ks 1.000 1.000 1.000\n"
					<< "d 1.0\n"
					<< "illum 2\n";

				std::string texturePath;
				if (material.settings.find("DiffuseTexture") != material.settings.end()
					&& SOTS2TextureToDDS(material.settings.at("DiffuseTexture"), texturePath))
				{
					mtlFile << "map_Ka " << texturePath << "\n";
					mtlFile << "map_Kd " << texturePath << "\n";
				}
				if (material.settings.find("SpecularTexture") != material.settings.end()
					&& SOTS2TextureToDDS(material.settings.at("SpecularTexture"), texturePath))
				{
					mtlFile << "map_Ks " << texturePath << "\n";
				}
					//<< "map_Ns " << material.settings.at("DiffuseTexture") << "\n"
					//<< "map_d " << material.settings.at("DiffuseTexture") << "\n"
					//<< "map_bump " << material.settings.at("DiffuseTexture") << "\n"
					mtlFile << "\n";
			}			
		}

		mtlFile.close();
	}
	
		
	std::ofstream file{ objFilename };

	if (file.is_open() == false)
	{
		return false;
	}

	std::cout << "Writing OBJ file: " << objFilename << "\n";

	if (this->dictionary != nullptr)
	{
		file << "mtllib " << mtlFilename << "\n";
	}

	unsigned int numPriorVertexes = 0;

	for (size_t i = 0; i < this->nodeObjects.size(); ++i)
	{
		file << "o " << this->nodeNames[this->nodeObjectParents[i]] << "\n";

		for (size_t j = 0; j < this->nodeObjects[i].nodeVertexes.size(); ++j)
		{
			file << std::fixed << std::setprecision(6) << "v " 
				<< this->nodeObjects[i].nodeVertexes[j].x << " "
				<< this->nodeObjects[i].nodeVertexes[j].y << " "
				<< this->nodeObjects[i].nodeVertexes[j].z << "\n";
		}

		for (size_t j = 0; j < this->nodeObjects[i].nodeVertexes.size(); ++j)
		{
			file << std::fixed << std::setprecision(6) << "vt " 
				<< this->nodeObjects[i].nodeVertexes[j].u << " "
				<< this->nodeObjects[i].nodeVertexes[j].v << "\n";
		}

		if (this->dictionary != nullptr)
		{
			file << "usemtl " << this->nodeObjects[i].textures[0].name << "\n";

			if(this->nodeObjects[i].textures.size() > 1)
			{
			//	std::cout << "Multiple Textures: " << this->nodeObjects[i].textures[0].name << "\n";
			}
		}		

		file << "s off\n";

		for (size_t j = 0; j < this->nodeObjects[i].nodeIndexes.size(); j+=3)
		{
			file << "f " << this->nodeObjects[i].nodeIndexes[j] + 1 + numPriorVertexes << "/" << this->nodeObjects[i].nodeIndexes[j] + 1 + numPriorVertexes << " "
				<< this->nodeObjects[i].nodeIndexes[j+1] + 1 + numPriorVertexes << "/" << this->nodeObjects[i].nodeIndexes[j+1] + 1 + numPriorVertexes << " "
				<< this->nodeObjects[i].nodeIndexes[j+2] + 1 + numPriorVertexes << "/" << this->nodeObjects[i].nodeIndexes[j+2] + 1 + numPriorVertexes << "\n";
		}

		numPriorVertexes = numPriorVertexes + this->nodeObjects[i].nodeVertexes.size();
	}
		
	file.close();
	return true;
}

void NodeObject::PrintIndexedVertices()
{
	for (size_t i = 0; i < this->nodeIndexes.size(); ++i)
	{
		std::ios::sync_with_stdio(false);
		std::cin.tie(NULL);

		std::cout << std::fixed << std::setprecision(2)
			<< std::setw(6) << this->nodeVertexes[this->nodeIndexes[i]].x << " "
			<< std::right << std::setw(6) << this->nodeVertexes[this->nodeIndexes[i]].y << " "
			<< std::right << std::setw(6) << this->nodeVertexes[this->nodeIndexes[i]].z << " "
			<< std::right << std::setw(6) << this->nodeVertexes[this->nodeIndexes[i]].unknown1 << " "
			<< std::right << std::setw(6) << this->nodeVertexes[this->nodeIndexes[i]].a << " "
			<< std::right << std::setw(6) << this->nodeVertexes[this->nodeIndexes[i]].b << " "
			<< std::right << std::setw(6) << this->nodeVertexes[this->nodeIndexes[i]].c << " "
			<< std::right << std::setw(6) << this->nodeVertexes[this->nodeIndexes[i]].d << " "
			<< std::right << std::setw(6) << this->nodeVertexes[this->nodeIndexes[i]].u << " "
			<< std::right << std::setw(6) << this->nodeVertexes[this->nodeIndexes[i]].v << " "
			<< std::right << std::setw(6) << this->nodeVertexes[this->nodeIndexes[i]].i1 << " "
			<< std::right << std::setw(6) << this->nodeVertexes[this->nodeIndexes[i]].i2 << " "
			<< std::right << std::setw(6) << this->nodeVertexes[this->nodeIndexes[i]].i3 << " "
			<< std::right << std::setw(6) << this->nodeVertexes[this->nodeIndexes[i]].i4 << " "
			<< std::right << std::setw(6) << this->nodeVertexes[this->nodeIndexes[i]].i5 << " "
			<< std::right << std::setw(6) << this->nodeVertexes[this->nodeIndexes[i]].i6 << " "
			<< "\n";
	}
}
