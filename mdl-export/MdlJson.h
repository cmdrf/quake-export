#ifndef MDLJSON_H
#define MDLJSON_H

#include "TextureImage.h"
#include <molecular/util/Vector3.h>

#include <vector>
#include <variant>

namespace MdlJson
{

/// Single skin
using SimpleSkin = TextureImage;

/// Group of skins for animation
struct SkinGroup
{
	std::vector<float> times;
	std::vector<SimpleSkin> skins;
};

/// A skin can be either a SimpleSkin or a SkinGroup
using Skin = std::variant<SimpleSkin, SkinGroup>;

/// Single frame
struct SimpleFrame
{
	std::string name;
	std::vector<molecular::util::Vector3> positions;
	std::vector<molecular::util::Vector3> normals;
};

/// Group of frames for animation
struct FrameGroup
{
	std::vector<float> times;
	std::vector<SimpleFrame> frames;
};

using Frame = std::variant<SimpleFrame, FrameGroup>;

/// Data read from a JSON file and the referenced OBJ and image files
struct Data
{
	std::vector<uint32_t> mainIndices;
	std::vector<molecular::util::Vector3> mainPositions;
	std::vector<molecular::util::Vector3> mainNormals;
	std::vector<molecular::util::Vector2> mainUvs;

	std::vector<Skin> skins;
	std::vector<Frame> frames;

	/// Get all positions from the main mesh and from all frames
	/** For min/max calculation. */
	std::vector<molecular::util::Vector3> CollectAllPositions();

	/** Throws if not all skins have the same width and height. */
	std::pair<unsigned int, unsigned int> GetSkinWidthHeight();
};

/// Read data from a JSON file and the referenced OBJ and image files
Data Read(const std::string& filename);

}

#endif // MDLJSON_H
