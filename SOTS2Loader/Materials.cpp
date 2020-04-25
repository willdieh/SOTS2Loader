#include "Materials.h"
#include <fstream>
#include <iostream>

bool MaterialDictionary::Load(std::string& filename, std::string& baseInstallDir)
{	
	std::ifstream file{ filename, std::ios_base::binary };

	if (file.is_open() == false)
	{
		std::cout << "Unable to open materials file: " << filename << "\nTextures will be unavailable\n";
		return false;
	}

	std::cout << "Parsing material dictionary: " << filename << "\n";

	char count;
	
	this->kerbLeader.resize(4);
	file.read(&this->kerbLeader[0], 4);

	file.read(reinterpret_cast<char*>(&this->kerbSerialNumber), sizeof(this->kerbSerialNumber));

	count = file.get();
	this->kerbType.resize(count);
	file.read(&this->kerbType[0], count);
		
	int materialCount;
	file.read(reinterpret_cast<char*>(&materialCount), sizeof(materialCount));

	for (int i = 0; i < materialCount; ++i)
	{
		Material material;

		count = file.get();
		
		material.materialName.resize(count);
		file.read(reinterpret_cast<char*>(&material.materialName[0]), count);
		
		count = file.get();
		material.materialType.resize(count);
		file.read(reinterpret_cast<char*>(&material.materialType[0]), count);
		
		int materialSettingsCount;
		file.read(reinterpret_cast<char*>(&materialSettingsCount), sizeof(materialSettingsCount));

		for (int j = 0; j < materialSettingsCount; ++j)
		{	
			std::string param, value;

			count = file.get();
			param.resize(count);
			file.read(reinterpret_cast<char*>(&param[0]), count);

			count = file.get();
			value.resize(count);
			file.read(reinterpret_cast<char*>(&value[0]), count);

			if (param.size() >= 7 && param.compare(param.size() - 7, 7, "Texture") == 0
				&& value.size() >= 1 && value[0] == '\\'
				&& value.size() >= 4 && (value.compare(value.size() - 4, 4, ".tga") == 0 || value.compare(value.size() - 4, 4, ".Tga") == 0 || value.compare(value.size() - 4, 4, ".TGA") == 0))
			{
				value = baseInstallDir + "\\assets" + value.substr(0, value.size() - 4) + ".tga~";
			}
			else if (param.size() >= 7 && param.compare(param.size() - 7, 7, "Texture") == 0
				&& value.size() >= 1 && value[0] == '\\'
				&& value.size() >= 4 && (value.compare(value.size() - 4, 4, ".bmp") == 0 || value.compare(value.size() - 4, 4, ".Bmp") == 0 || value.compare(value.size() - 4, 4, ".BMP") == 0))
			{
				value = baseInstallDir + "\\assets" + value.substr(0, value.size() - 4) + ".bmp~";
			}

			material.settings.emplace(param, value);
		}

		this->materials.push_back(std::move(material));
	}

	file.close();
	return true;
}

void MaterialDictionary::Print()
{
	std::ios::sync_with_stdio(false);
	std::cin.tie(NULL);

	for (size_t i = 0; i < this->materials.size(); ++i)
	{
		std::cout << this->materials[i].materialName << " (" << this->materials[i].materialType << ")\n";
		
		for (const auto& setting : this->materials[i].settings)
		{
			std::cout << " " << setting.first << ": " << setting.second << "\n";
		}
	}
}