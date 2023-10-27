/*	MdlFile.cpp

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

#include "MdlFile.h"

#include <cassert>
#include <cstring>

MdlFile::MdlFile(molecular::util::WriteStorage& storage) :
	mStorage(storage)
{

}

void MdlFile::WriteHeader(const Header& header)
{
	assert(mCurrentSection == HEADER);
	mStorage.Write(&header, sizeof(Header));
	mHeader = header;
	mCurrentSection = SKINS;
}

void MdlFile::WriteSkin(const uint8_t* skin)
{
	assert(mCurrentSection == SKINS);
	const uint32_t group = 0;
	mStorage.Write(&group, 4);
	mStorage.Write(skin, mHeader.skinHeight * mHeader.skinWidth);
}

void MdlFile::WriteSkinGroup(const std::vector<float>& times, const std::vector<const uint8_t*>& skins)
{
	assert(mCurrentSection == SKINS);
	assert(times.size() == skins.size());
	const uint32_t group = 1;
	mStorage.Write(&group, 4);
	const uint32_t nb = times.size();
	mStorage.Write(&nb, 4);
	mStorage.Write(times.data(), times.size() * 4);
	for(auto skin: skins)
		mStorage.Write(skin, mHeader.skinHeight * mHeader.skinWidth);
}

void MdlFile::WriteSkinGroup(const std::vector<float>& times, const std::vector<std::vector<uint8_t>>& skins)
{
	std::vector<const uint8_t*> skinPointers;
	skinPointers.reserve(skins.size());
	for(auto& skin: skins)
		skinPointers.push_back(skin.data());
	WriteSkinGroup(times, skinPointers);
}

void MdlFile::WriteStVertex(bool onSeam, uint32_t s, uint32_t t)
{
	assert(mCurrentSection == SKINS || mCurrentSection == ST_VERTICES);
	const uint32_t onseam = onSeam ? 0x20 : 0;
	mStorage.Write(&onseam, 4);
	mStorage.Write(&s, 4);
	mStorage.Write(&t, 4);
	mCurrentSection = ST_VERTICES;
}

void MdlFile::WriteTriangle(const uint32_t vertices[], bool facesFront)
{
	assert(mCurrentSection == ST_VERTICES || mCurrentSection == TRIANGLES);
	const uint32_t front = facesFront ? 1 : 0;
	mStorage.Write(&front, 4);
	mStorage.Write(vertices, 12);
	mCurrentSection = TRIANGLES;
}

static void WriteSimpleFrame(molecular::util::WriteStorage& storage, const MdlFile::SimpleFrame& frame)
{
	storage.Write(&frame.min, sizeof(MdlFile::TriangleVertex));
	storage.Write(&frame.max, sizeof(MdlFile::TriangleVertex));
	char name[16];
	std::strncpy(name, frame.name.c_str(), 16);
	storage.Write(name, 16);
	storage.Write(frame.vertices.data(), frame.vertices.size() * sizeof(MdlFile::TriangleVertex));
}

void MdlFile::WriteSingleFrame(const SimpleFrame& frame)
{
	assert(mCurrentSection == TRIANGLES || mCurrentSection == FRAMES);
	const uint32_t group = 0;
	mStorage.Write(&group, 4);
	WriteSimpleFrame(mStorage, frame);
	mCurrentSection = FRAMES;
}

void MdlFile::WriteFrameGroup(const TriangleVertex& min, const TriangleVertex& max, const std::vector<float>& times, const std::vector<SimpleFrame>& frames)
{
	assert(mCurrentSection == TRIANGLES || mCurrentSection == FRAMES);
	assert(times.size() == frames.size());
	const uint32_t group = 1;
	mStorage.Write(&group, 4);
	const uint32_t num = times.size();
	mStorage.Write(&num, 4);
	mStorage.Write(&min, sizeof(TriangleVertex));
	mStorage.Write(&max, sizeof(TriangleVertex));
	mStorage.Write(times.data(), times.size() * 4);
	for(auto& frame: frames)
		WriteSimpleFrame(mStorage, frame);

	mCurrentSection = FRAMES;
}

