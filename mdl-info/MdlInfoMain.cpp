#include <molecular/util/FileStreamStorage.h>
#include <molecular/util/LittleEndianStream.h>
#include <molecular/util/Vector3.h>

#include <cstdint>

using namespace molecular::util;

struct Header
{
	uint32_t id;
	uint32_t version;
	Vector3 scale;
	Vector3 origin;
	float radius;
	Vector3 offsets;
	uint32_t numSkins;
	uint32_t skinWidth;
	uint32_t skinHeight;
	uint32_t numVerts;
	uint32_t numTris;
	uint32_t numFrames;
	uint32_t synctype;
	uint32_t flags;
	float size;
};

struct TriangleVertex
{
	uint8_t packedPositions[3];
	uint8_t lightNormalIndex;
};

/** Can be either a single frame or part of a frame group. */
struct SimpleFrame
{
	TriangleVertex min;
	TriangleVertex max;

	char name[16];

	TriangleVertex vertices[0];
};

int main(int argc, char** argv)
{
	if(argc != 2)
	{
		std::cerr << "Usage: mdl-info <MDL file>" << std::endl;
		return EXIT_FAILURE;
	}

	FileReadStorage file(argv[1]);
	LittleEndianReadStream<FileReadStorage> stream(file);
	Header header;
	file.Read(&header, sizeof(Header));

	if(header.id != 0x4F504449)
	{
		std::cerr << "Not an MDL file" << std::endl;
		return EXIT_FAILURE;
	}

	if(header.version != 6)
	{
		std::cerr << "Unsupported MDL version" << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << "scale: " << header.scale << std::endl;
	std::cout << "origin: " << header.origin << std::endl;
	std::cout << "radius: " << header.radius << std::endl;
	std::cout << "offsets: " << header.offsets << std::endl;
	std::cout << "numSkins: " << header.numSkins << std::endl;
	std::cout << "skinWidth: " << header.skinWidth << std::endl;
	std::cout << "skinHeight: " << header.skinHeight << std::endl;
	std::cout << "numVerts: " << header.numVerts << std::endl;
	std::cout << "numTris: " << header.numTris << std::endl;
	std::cout << "numFrames: " << header.numFrames << std::endl;
	std::cout << "synctype: " << header.synctype << std::endl;
	std::cout << "flags: " << header.flags << std::endl;
	std::cout << "size: " << header.size << "\n---\n";

	for(unsigned int i = 0; i < header.numSkins; ++i)
	{
		std::cout << "Skin " << i << ": ";
		uint32_t group;
		stream.Read(group);
		if(group == 0)
		{
			std::cout << "simple skin\n";
			file.Skip(header.skinWidth * header.skinHeight);
		}
		else
		{
			uint32_t numberOfSkins;
			stream.Read(numberOfSkins);
			std::cout << "skin group (" << numberOfSkins << " skins). times: [";
			for(unsigned int s = 0; s < numberOfSkins; ++s)
			{
				float time;
				stream.Read(time);
				std::cout << time << ", ";
			}
			std::cout << "]\n";
			file.Skip(numberOfSkins * header.skinWidth * header.skinHeight);
		}
	}

	file.Skip(header.numVerts * 12); // Skip STs
	file.Skip(header.numTris * 16); // Skip triangles

	for(unsigned int i = 0; i < header.numFrames; ++i)
	{
		std::cout << "Frame " << i << ": ";

		uint32_t group;
		stream.Read(group);
		if(group == 0)
		{
			std::cout << "simple frame \"";
			file.Skip(8); // Skip min/max
			char name[17] = {0};
			file.Read(name, 16);
			std::cout << name << "\"\n";
			file.Skip(header.numVerts * 4); // Skip vertices
		}
		else
		{
			uint32_t numberOfFrames;
			stream.Read(numberOfFrames);
			std::cout << "frame group (" << numberOfFrames << " frames). times: [";
			for(unsigned int s = 0; s < numberOfFrames; ++s)
			{
				float time;
				stream.Read(time);
				std::cout << time << ", ";
			}
			std::cout << "], names: [";
			for(unsigned int f = 0; f < numberOfFrames; ++f)
			{
				file.Skip(8); // Skip min/max
				char name[17] = {0};
				file.Read(name, 16);
				std::cout << name << ", ";
				file.Skip(header.numVerts * 4); // Skip vertices
			}
			std::cout << "]\n";
		}
	}

	return EXIT_SUCCESS;
}
