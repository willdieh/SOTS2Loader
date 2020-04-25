#pragma once

#include <string>

//Data Definitions

#define DDPF_ALPHAPIXELS    0x1         //uncompressed texture contains alpha data and a valid RGB Alpha Bit Mask
#define DDPF_ALPHA          0x2         //alpha only uncompressed, valid RGB Bit Count and Alpha Bit Mask values present
#define DDPF_FOURCC         0x4         //Compressed RGB data, valid FourCC compression type value present
#define DDPF_RGB            0x40        //Uncompressed RGB data, valid RGB Bitcount and RGB masks present
#define DDPF_YUV            0x200       //YUV uncompressed data, valid RGB bitcount and rgb masks (for YUV values)
#define DDPF_LUMINANCE      0x20000     //single color uncompressed, valid RGB bitcount and Red Bitmask (for lum vals)

//Definitions for DDS flags [used in DDS header]
#define DDSD_CAPS           0x1         //Required in every .dds file
#define DDSD_HEIGHT         0x2         //Required in every .dds file
#define DDSD_WIDTH          0x4         //Required in every .dds file
#define DDSD_PITCH          0x8         //Required when pitch is provided for uncompressed texture
#define DDSD_PIXELFORMAT    0x1000      //Required in every .dds file
#define DDSD_MIPMAPCOUNT    0x20000     //Required in a mipmapped texture
#define DDSD_LINEARSIZE     0x80000     //Required when pitch is provided for a compressed texture
#define DDSD_DEPTH          0x800000    //Required in a depth texture

//Definitions for DDS capabilities [used in DDS header]
#define DDSCAPS_COMPLEX     0x8         //Must be used on any file that has multiple surfaces/mipmaps
#define DDSCAPS_MIPMAP      0x400000    //Must be used if mipmap present
#define DDSCAPS_TEXTURE     0x1000      //Required for all files


//Block Compression 3 (BC3) data structure
struct BC3_struct {
    char alpha_0;			//1 byte alpha_0
    char alpha_1;			//1 byte alpha_1
    char alpha_map[6];		//6 bytes alpha bitmap

    char color_0[2];		//2 bytes color_0
    char color_1[2];		//2 bytes color_1
    char color_map[4];		//4 bytes color bitmap
};

//SOTS2 Header
struct SOTS2_Header_struct {

    // Kerberos specific header stuff (seems useless)
    std::string						kerbLeader;			// "SHYL"
    int								kerbSerialNumber;	// INT? UINT? Two CHARs?
    std::string						kerbType;			// "Texture2D", etc

    unsigned int type_code;             //Texture type (DirectX specific) See DXGI_FORMAT https://msdn.microsoft.com/en-us/library/windows/desktop/bb173059(v=vs.85).aspx
    unsigned int width;                 //width
    unsigned int height;                //height
    unsigned int MIP_map_count;         //number of MIP Maps in file
    unsigned int flag1;                 //Unknown flag/value 1
    unsigned int flag2;                 //Unknown flag/value 2
    unsigned int main_size;             //Total number of data bytes in main image (used as pitch for compressed images)
    unsigned int total_size;            //Total number of data bytes in file
    unsigned int pitch;                 //Pitch (bytes per line) used for uncompressed images
};

//Pixel Format Data Structure
struct DDS_Pixel_Format_struct {
    unsigned int DDSsize = 32;  //Size of structure. Always 32, per Microsoft
    unsigned int flags;         //what type of data is in the surface
    unsigned char fourCC[4];    //four char codes for specifying compression types (DXT1, DXT2, DXT3, DXT4, DXT5). DXT10 reqs extended header
    unsigned int RGBbitcount;   //number of bits in uncompressed RGB format (including any alpha bits)
    unsigned int Rbitmask;      //Red bitmask of the rgb bits
    unsigned int Gbitmask;      //Green bitmask of the rgb bits
    unsigned int Bbitmask;      //Blue bitmask of the rgb bits
    unsigned int Abitmask;      //Alpha bitmask of the rgb bits
};

//Direct Draw Surface (DDS) Header
struct DDS_Header_struct {
    unsigned int magic_string = 0x20534444;   //"DDS " in hex, reverse ordering (little endian)
    unsigned int structure_size = 124;        //Size of structure. Always 124, per Microsoft
    unsigned int flags;                       //Flags to indicate which members contain valid data
    unsigned int height;                      //Height, in pixels
    unsigned int width;                       //Width, in pixels
    unsigned int pitch;                       //#of bytes per line in uncompressed, total bytes in compressed texture
    unsigned int depth;                       //Depth in pixels of a volume texture or otherwise unused
    unsigned int MIP_map_count;               //Number of mipmap levels, otherwise unused
    unsigned int reserved1[11] = { 0 };       //Unused
    DDS_Pixel_Format_struct pf;               //Pixel Format data
    unsigned int caps;                        //Specifies complexity of the surfaces stored (ie, are there mipmaps?)
    unsigned int caps2 = 0;                   //More boring info if the texture is a cube or volume map, otherwise 0
    unsigned int caps3 = 0;                   //unused
    unsigned int caps4 = 0;                   //unused
    unsigned int reserved2 = 0;               //unused
};

bool SOTS2TextureToDDS(const std::string& inputFilename, std::string& outputFilename);

