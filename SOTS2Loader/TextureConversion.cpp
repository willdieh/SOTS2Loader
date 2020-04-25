#include "TextureConversion.h"

#include <fstream>
#include <string>
#include <iostream>

bool SOTS2TextureToDDS(const std::string& inputFilename, std::string& outputFilename)
{
	std::string customInputFilename = inputFilename;

	std::ifstream inputfile;
	inputfile.open(customInputFilename, std::ios_base::binary);

	if (inputfile.is_open() == false)
	{
		// Try again using the _assetbin development directory (used by Kerb for debug mode)
		size_t location = inputFilename.find("\\base\\", 0, 6);
		if (location != inputFilename.npos)
		{
			customInputFilename = inputFilename.substr(0, location + 6) + "_assetbin\\" + inputFilename.substr(location + 6, inputFilename.size() - location + 6);
			inputfile.open(customInputFilename, std::ios_base::binary);
			if (inputfile.is_open() == false)
			{
				std::cout << "Unable to open input texture file: " << customInputFilename << "\n";
				return false;
			}
		}
		else 
		{
			std::cout << "Unable to open input texture file: " << customInputFilename << "\n";
			return false;
		}		
	}

	//Create a structure to hold our header data
	SOTS2_Header_struct sots2;

	//Create a DDS structure to hold our data
	DDS_Header_struct dds;

	//Create a BC3 structure to hold (compressed) image data
	BC3_struct bc3;

	std::cout << "Converting texture file: " << customInputFilename << "\n";

	char count;

	sots2.kerbLeader.resize(4);
	inputfile.read(&sots2.kerbLeader[0], 4);

	inputfile.read(reinterpret_cast<char*>(&sots2.kerbSerialNumber), sizeof(sots2.kerbSerialNumber));

	count = inputfile.get();
	sots2.kerbType.resize(count);
	inputfile.read(&sots2.kerbType[0], count);

	//Get the file type code
	inputfile.read(reinterpret_cast<char*>(&sots2.type_code), sizeof(sots2.type_code));
	
	//Get the width and height
	inputfile.read(reinterpret_cast<char*>(&sots2.width), sizeof(sots2.width));
	inputfile.read(reinterpret_cast<char*>(&sots2.height), sizeof(sots2.height));
	
	//Get the number of mipmaps
	inputfile.read(reinterpret_cast<char*>(&sots2.MIP_map_count), sizeof(sots2.MIP_map_count));
	
	//read two unknown flag values
	inputfile.read(reinterpret_cast<char*>(&sots2.flag1), sizeof(sots2.flag1));
	inputfile.read(reinterpret_cast<char*>(&sots2.flag2), sizeof(sots2.flag2));

	//parse through the MIP Map info
	for (unsigned int i = 1; i <= sots2.MIP_map_count; i++)
	{
		if (i == 1) //First MIP definition holds pitch (bytes per line) of main image. Pick up the main image size too.
		{
			int data; //Throw away first word? Hmmm, my original logic is lost during this refactoring
			inputfile.read(reinterpret_cast<char*>(&data), sizeof(data));

			// Read Pitch and Main Image Size
			inputfile.read(reinterpret_cast<char*>(&sots2.pitch), sizeof(sots2.pitch));
			inputfile.read(reinterpret_cast<char*>(&sots2.main_size), sizeof(sots2.main_size));

			if (sots2.MIP_map_count == 1)
			{
				sots2.total_size = sots2.main_size; //If there's only one MIP, main_size == total_size				
			}
		}
		else if (i == sots2.MIP_map_count) //Pull the total file size from last MIP (only get here if more than one MIP)
		{
			int data; //Throw away first two words? Hmmm, my original logic is lost during this refactoring
			inputfile.read(reinterpret_cast<char*>(&data), sizeof(data));
			inputfile.read(reinterpret_cast<char*>(&data), sizeof(data));

			// Read total size
			inputfile.read(reinterpret_cast<char*>(&sots2.total_size), sizeof(sots2.total_size));
		}
		else //Otherwise just pull in the three DWORD data for each MIP
		{
			int data; //Throw away three words? Hmmm, my original logic is lost during this refactoring
			inputfile.read(reinterpret_cast<char*>(&data), sizeof(data));
			inputfile.read(reinterpret_cast<char*>(&data), sizeof(data));
			inputfile.read(reinterpret_cast<char*>(&data), sizeof(data));
		}
	}

	//Write out to the appropriate file type
	//Type Code: 77 - DirectDraw Surface (DDS) in Block Compression 3 (BC3/DXT5) format
	if (sots2.type_code == 77)
	{
		//Initialize our DDS structure
		//We could have just written straight to the output file from the values in the SOTS2 header, but this is clearer to understand [maybe]
		if (sots2.MIP_map_count > 1)
		{
			dds.flags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_MIPMAPCOUNT | DDSD_LINEARSIZE;
		}
		else
		{
			dds.flags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_LINEARSIZE;
		}

		dds.width = sots2.width;        //width
		dds.height = sots2.height;      //height

		dds.pitch = sots2.total_size;    //pitch for compressed images is the total image size
		dds.depth = 0;                  //not a depth texture, so zero
		dds.MIP_map_count = sots2.MIP_map_count;    //Number of MIP Maps

													//Define pixel format info
		dds.pf.flags = DDPF_FOURCC;         //Set the DDS Pixel Format flags. Assumes this is a compressed format. todo: logic for uncompressed images
		dds.pf.fourCC[0] = 'D';             //Set the 4 char code compression type
		dds.pf.fourCC[1] = 'X';
		dds.pf.fourCC[2] = 'T';
		dds.pf.fourCC[3] = '5';
		dds.pf.RGBbitcount = 0;             //Not an uncompressed RGB image. todo: logic for uncompressed images
		dds.pf.Rbitmask = 0;                //Not an uncompressed RGB image. todo: logic for uncompressed images
		dds.pf.Gbitmask = 0;                //Not an uncompressed RGB image. todo: logic for uncompressed images
		dds.pf.Bbitmask = 0;                //Not an uncompressed RGB image. todo: logic for uncompressed images
		dds.pf.Abitmask = 0;                //Not an uncompressed RGB image. todo: logic for uncompressed images

											//set capabilities flags
		if (sots2.MIP_map_count > 0)
		{
			dds.caps = DDSCAPS_COMPLEX | DDSCAPS_MIPMAP | DDSCAPS_TEXTURE;   //has mipmaps
		}
		else
		{
			dds.caps = DDSCAPS_TEXTURE; //no mipmaps
		}
	}
	//Type Code: 28 - DirectDraw Surface (DDS) in Uncompressed 4 byte RGBA format
	else if (sots2.type_code == 28) 
	{
		//Initialize our DDS structure
		//We could have just written straight to the output file from the values in the SOTS2 header, but this is clearer to understand [maybe]

		if (sots2.MIP_map_count > 1)
		{
			dds.flags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PITCH | DDSD_PIXELFORMAT | DDSD_MIPMAPCOUNT;
		}
		else
		{
			dds.flags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PITCH | DDSD_PIXELFORMAT;
		}

		dds.width = sots2.width;        //width
		dds.height = sots2.height;      //height

		dds.pitch = sots2.pitch;        //pitch for uncompressed images is the bytes per line
		dds.depth = 0;                  //not a depth texture, so zero
		dds.MIP_map_count = sots2.MIP_map_count;    //Number of MIP Maps

													//Define pixel format info
		dds.pf.flags = DDPF_ALPHAPIXELS | DDPF_RGB;         //Set the DDS Pixel Format flags. Assumes this is a compressed format. todo: logic for uncompressed images
		dds.pf.fourCC[0] = 0;             //Set the 4 char code compression type (N/A for uncompressed RGBA files)
		dds.pf.fourCC[1] = 0;
		dds.pf.fourCC[2] = 0;
		dds.pf.fourCC[3] = 0;
		dds.pf.RGBbitcount = 32;            //Number of bits in an RGB format
		dds.pf.Rbitmask = 0x000000ff;       //Red Color Bitmask (little endian)
		dds.pf.Gbitmask = 0x0000ff00;       //Green Color Bitmask (little endian)
		dds.pf.Bbitmask = 0x00ff0000;       //Blue Color Bitmask (little endian)
		dds.pf.Abitmask = 0xff000000;       //Alpha Color Bitmask (little endian)

											//set capabilities flags
		if (sots2.MIP_map_count > 1)
		{
			dds.caps = DDSCAPS_COMPLEX | DDSCAPS_MIPMAP | DDSCAPS_TEXTURE;   //has mipmaps
		}
		else
		{
			dds.caps = DDSCAPS_TEXTURE; //no mipmaps
		}
	}
		
	if (customInputFilename.size() > 5 && (customInputFilename.compare(customInputFilename.size() - 5, 5, ".tga~") == 0 || customInputFilename.compare(customInputFilename.size() - 5, 5, ".bmp~")))
	{
		outputFilename = customInputFilename.substr(0, customInputFilename.size() - 5) + ".dds";
	}

	std::ofstream outputfile{ outputFilename, std::ios_base::binary };

	if (outputfile.is_open() == false)
	{
		std::cout << "Unable to open output texture file : " << outputFilename << "\n";
		return false;
	}

	std::cout << "Converted: " << outputFilename << "\n";

	//Write out the DDS header
	outputfile.write(reinterpret_cast<char*>(&dds), sizeof(dds));

	//Read in and Write out the DDS image data
	for (unsigned int i = 0; i < sots2.total_size && !inputfile.eof(); i += sizeof(bc3))
	{
		inputfile.read(reinterpret_cast<char*>(&bc3), sizeof(bc3));
		outputfile.write(reinterpret_cast<char*>(&bc3), sizeof(bc3));
	}

	inputfile.close();
	outputfile.close();
	return true;
}

