#include "MdlJson.h"
#include "MdlUtils.h"

#include <nlohmann/json.hpp>

#include <fstream>
#include <stdexcept>

using namespace molecular;
using namespace molecular::util;
using json = nlohmann::json;

namespace MdlJson
{

static SimpleSkin ReadSkin(const json& skin)
{
	std::string image = skin.at("image");
	SimpleSkin out {
		TextureImage(image.c_str())
	};
	return out;
}

static SimpleFrame ReadFrame(const json& frame)
{
	SimpleFrame out;
	out.name = frame.at("name");

	std::vector<uint32_t> indices;
	std::vector<Vector2> uvs;

	ReadObj(frame.at("mesh"), indices, out.positions, out.normals, uvs);

	return out;
}

Data Read(const std::string& filename)
{
	Data out;

	std::ifstream i(filename);
	if(!i.good())
		throw std::runtime_error("Error opening " + filename);
	nlohmann::json j;
	i >> j;

	ReadObj(j.at("mesh"), out.mainIndices, out.mainPositions, out.mainNormals, out.mainUvs);

	// Process skins
	for (const auto &skin : j.at("skins"))
	{
		if (skin.is_object())
			out.skins.push_back(ReadSkin(skin));
		else if (skin.is_array())
		{
			SkinGroup group;
			// Read times:
			for (const auto &inner_skin : skin)
				group.times.push_back(inner_skin.at("time"));

			// Read images:
			for (const auto &inner_skin : skin)
				group.skins.push_back(ReadSkin(inner_skin));
			out.skins.push_back(std::move(group));
		}
		else
			throw std::runtime_error("Unexpected skin element");
	}

	// Process frames
	for (const auto &frame : j["frames"])
	{
		if (frame.is_object())
			out.frames.push_back(ReadFrame(frame));
		else if (frame.is_array())
		{
			FrameGroup group;

			// Read times:
			for (const auto &inner_frame : frame)
				group.times.push_back(inner_frame.at("time"));

			// Read images:
			for (const auto &inner_frame : frame)
				group.frames.push_back(ReadFrame(inner_frame));
			out.frames.push_back(group);
		}
	}

	return out;
}

std::vector<Vector3> Data::CollectAllPositions()
{
	std::vector<Vector3> allPositions = mainPositions;

	// Collect all frame vertices:
	for(auto& frame: frames)
		std::visit([&](auto&& arg)
		{
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, MdlJson::SimpleFrame>)
			   allPositions.insert(allPositions.end(), arg.positions.begin(), arg.positions.end());
			else if constexpr (std::is_same_v<T, MdlJson::FrameGroup>)
			{
				for(auto& frame: arg.frames)
					allPositions.insert(allPositions.end(), frame.positions.begin(), frame.positions.end());
			}
		}, frame);
	return allPositions;
}

std::pair<unsigned int, unsigned int> Data::GetSkinWidthHeight()
{
	unsigned int skinWidth = 0;
	unsigned int skinHeight = 0;

	// Check skin sizes:
	for(auto& skin: skins)
		std::visit([&](auto&& arg)
		{
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, MdlJson::SimpleSkin>)
			{
				if(skinWidth != 0 && skinWidth != arg.GetWidth())
					throw std::runtime_error("Skin widths don't match");
				if(skinHeight != 0 && skinHeight != arg.GetHeight())
					throw std::runtime_error("Skin heights don't match");
				skinWidth = arg.GetWidth();
				skinHeight = arg.GetHeight();
			}
			else if constexpr (std::is_same_v<T, MdlJson::SkinGroup>)
			{
				for(auto& skin: arg.skins)
				{
					if(skinWidth != 0 && skinWidth != skin.GetWidth())
						throw std::runtime_error("Skin widths don't match");
					if(skinHeight != 0 && skinHeight != skin.GetHeight())
						throw std::runtime_error("Skin heights don't match");
					skinWidth = skin.GetWidth();
					skinHeight = skin.GetHeight();
				}
			}
		}, skin);
	return std::make_pair(skinWidth, skinHeight);
}

}
