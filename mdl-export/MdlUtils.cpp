#include "MdlUtils.h"
#include "QuakeNormal.h"


#include <molecular/util/FileStreamStorage.h>
#include <molecular/util/TextStream.h>
#include <molecular/util/ObjFile.h>
#include <molecular/util/ObjFileUtils.h>
#include <molecular/util/Vector3.h>

#include <vector>

using namespace molecular;
using namespace molecular::util;

std::pair<Vector3, Vector3> GetMinMax(const std::vector<Vector3>& positions)
{
	Vector3 min(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	Vector3 max(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min());
	for(auto& pos: positions)
	{
		for(int i = 0; i < 3; ++i)
		{
			min[i] = std::min(min[i], pos[i]);
			max[i] = std::max(max[i], pos[i]);
		}
	}
	return std::make_pair(min, max);
}

std::pair<MdlFile::TriangleVertex, MdlFile::TriangleVertex> GetMinMax(const std::vector<MdlFile::TriangleVertex>& vertices)
{
	MdlFile::TriangleVertex min = {{255, 255, 255}, 0};
	MdlFile::TriangleVertex max = {{0, 0, 0}, 0};
	for(auto& pos: vertices)
	{
		for(int i = 0; i < 3; ++i)
		{
			min.packedPositions[i] = std::min(min.packedPositions[i], pos.packedPositions[i]);
			max.packedPositions[i] = std::max(max.packedPositions[i], pos.packedPositions[i]);
		}
	}
	return std::make_pair(min, max);
}

float CalculateBoundingRadius(const Vector3& min, const Vector3& max)
{
	Vector3 rsqr;
	for (size_t i = 0; i < 3; ++i)
		rsqr[i] = std::pow(std::max(std::abs(min[i]), std::abs(max[i])), 2);

	// Calculate bounding radius:
	return std::sqrt(rsqr[0] + rsqr[1] + rsqr[2]);
}

float CalculateAverageTriangleArea(
		const std::vector<uint32_t>& indices,
		const std::vector<Vector3>& positions)
{
	float totalArea = 0.0f;

	for (size_t i = 0; i < indices.size(); i += 3)
	{
		Vector3 A = positions[indices[i]];
		Vector3 B = positions[indices[i + 1]];
		Vector3 C = positions[indices[i + 2]];

		Vector3 AB = B - A;
		Vector3 AC = C - A;

		float area = 0.5f * AB.CrossProduct(AC).Length();
		totalArea += area;
	}

	const size_t numTriangles = indices.size() / 3;
	return totalArea / numTriangles;
}

std::vector<MdlFile::TriangleVertex> ToTriangleVertices(const std::vector<Vector3>& positions, const std::vector<Vector3>& normals, const Vector3& origin, const Vector3& scale)
{
	assert(positions.size() == normals.size());

	std::vector<MdlFile::TriangleVertex> out;

	for(size_t i = 0; i < positions.size(); ++i)
	{
		MdlFile::TriangleVertex vert;
		Vector3 pos = (positions[i] - origin) / scale;
		vert.packedPositions[0] = pos[0];
		vert.packedPositions[1] = pos[1];
		vert.packedPositions[2] = pos[2];
		vert.lightNormalIndex = QuakeNormal(normals[i]);
		out.push_back(vert);
	}

	return out;
}

void ReadObj(const std::string& fileName,
			std::vector<uint32_t>& indices,
			std::vector<Vector3>& positions,
			std::vector<Vector3>& normals,
			std::vector<Vector2>& uvs)
{
	FileReadStorage inFile(fileName);
	TextReadStream<FileReadStorage> trs(inFile);
	ObjFile objFile(trs);

	for(auto& vg: objFile.GetVertexGroups())
	{
		if(vg.numQuads == 0 && vg.numTriangles == 0)
			continue;

		ObjFileUtils::ObjVertexGroupBuffers(objFile, vg, indices, positions, normals, uvs);
		size_t numVertices = positions.size();
		assert(normals.empty() || normals.size() == numVertices);
		assert(uvs.empty() || uvs.size() == numVertices);
	}

	// Convert to quake units:
	for(auto& pos: positions)
		pos *= 64.0 / 1.7;
}
