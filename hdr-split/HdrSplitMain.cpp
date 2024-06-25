#include <StbHdrImage.h>
#include <WriteImage.h>

#include <molecular/util/CommandLineParser.h>
#include <stb_image.h>

#include <cmath>
#include <iostream>

using namespace molecular::util;

int Main(int argc, char** argv)
{
	CommandLineParser cmd;
	CommandLineParser::PositionalArg<std::string> inFileName(cmd, "input file", "Input HDR image");
	CommandLineParser::PositionalArg<std::string> outFile1Name(cmd, "lower output file", "Output SDR file 0..100%");
	CommandLineParser::PositionalArg<std::string> outFile2Name(cmd, "upper output file", "Output SDR file 100..200%");

	CommandLineParser::HelpFlag help(cmd);

	try
	{
		cmd.Parse(argc, argv);
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		cmd.PrintHelp();
		return EXIT_FAILURE;
	}

	if(!stbi_is_hdr(inFileName->c_str()))
		throw std::runtime_error("Input is not an HDR image");
	StbHdrImage hdrImage(inFileName->c_str(), 3);
	const float* inData = hdrImage.Data();

	const size_t numValues = hdrImage.GetWidth() * hdrImage.GetHeight() * 3;
	std::vector<uint8_t> outData(numValues);

	const float scale = 1.0f;
	const float inverseGamma = 1.f/2.2f;
	for(size_t i = 0; i < numValues; ++i)
	{
		float z = std::pow(inData[i] * scale, inverseGamma) * 255 + 0.5f;
		if (z < 0)
			z = 0;
		if (z > 255)
			z = 255;
		outData[i] = z;
	}
	WriteRgbImage(outFile1Name->c_str(), outData.data(), hdrImage.GetWidth(), hdrImage.GetHeight());

	for(size_t i = 0; i < numValues; ++i)
	{
		float z = std::pow((inData[i] * scale) - 1.f, inverseGamma) * 255 + 0.5f;
		if (z < 0)
			z = 0;
		if (z > 255)
			z = 255;
		outData[i] = z;
	}
	WriteRgbImage(outFile2Name->c_str(), outData.data(), hdrImage.GetWidth(), hdrImage.GetHeight());
	return EXIT_SUCCESS;
}

int main(int argc, char** argv)
{
	try
	{
		return Main(argc, argv);
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}
