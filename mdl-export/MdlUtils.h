#ifndef MDLUTILS_H
#define MDLUTILS_H

#include "MdlFile.h"

#include <molecular/util/Vector3.h>

#include <vector>

/// Get minimum and maximum float vertex
std::pair<molecular::util::Vector3, molecular::util::Vector3> GetMinMax(const std::vector<molecular::util::Vector3>& positions);

/// Get minimum and maximum packed vertex
std::pair<MdlFile::TriangleVertex, MdlFile::TriangleVertex> GetMinMax(const std::vector<MdlFile::TriangleVertex>& vertices);

/// Calculate bounding sphere from bounding box
float CalculateBoundingRadius(const molecular::util::Vector3& min, const molecular::util::Vector3& max);

/// Calculate average triangle area
/** Not sure what this is used for, but it's in the header of MDL files. */
float CalculateAverageTriangleArea(const std::vector<uint32_t>& indices, const std::vector<molecular::util::Vector3>& positions);

/// Float to packed vertices
std::vector<MdlFile::TriangleVertex> ToTriangleVertices(const std::vector<molecular::util::Vector3>& positions, const std::vector<molecular::util::Vector3>& normals, const molecular::util::Vector3& origin, const molecular::util::Vector3& scale);

void ReadObj(const std::string& fileName,
			std::vector<uint32_t>& indices,
			 std::vector<molecular::util::Vector3>& positions,
			 std::vector<molecular::util::Vector3>& normals,
			 std::vector<molecular::util::Vector2>& uvs);

#endif // MDLUTILS_H
