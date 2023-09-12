/*	MdlFile.h

MIT License

Copyright (c) 2023 Fabian Herb

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef MDLFILE_H
#define MDLFILE_H

#include <molecular/util/Vector3.h>
#include <molecular/util/StreamStorage.h>

#include <cstdint>
#include <vector>

class MdlFile
{
public:

	struct Header
	{
		uint32_t id = 0x4F504449;
		uint32_t version = 6;
		molecular::util::Vector3 scale;
		molecular::util::Vector3 origin;
		float radius;
		molecular::util::Vector3 offsets;
		uint32_t numSkins;
		uint32_t skinWidth;
		uint32_t skinHeight;
		uint32_t numVerts;
		uint32_t numTris;
		uint32_t numFrames;
		uint32_t synctype = 0;
		uint32_t flags = 0;
		float size;
	};

	struct TriangleVertex
	{
		uint8_t packedPositions[3];
		uint8_t lightNormalIndex;
	};

	struct SimpleFrame
	{
		TriangleVertex min;
		TriangleVertex max;
		std::string name;
		std::vector<TriangleVertex> vertices;
	};

	MdlFile(molecular::util::WriteStorage& storage);

	void WriteHeader(const Header& header);
	void WriteSkin(const uint8_t* skin);
	void WriteSkinGroup(std::vector<float> times, const std::vector<uint8_t*>& skins);
	void WriteStVertex(bool onSeam, uint32_t s, uint32_t t);
	void WriteTriangle(bool facesFront, const uint32_t vertices[3]);
	void WriteSingleFrame(const SimpleFrame& frame);
	void WriteFrameGroup(const TriangleVertex& min, const TriangleVertex& max, const std::vector<float>& times, const std::vector<SimpleFrame>& frames);

private:
	enum Section
	{
		HEADER,
		SKINS,
		ST_VERTICES,
		TRIANGLES,
		FRAMES
	};

	Section mCurrentSection = HEADER;
	molecular::util::WriteStorage& mStorage;
	Header mHeader;
};

#endif // MDLFILE_H
