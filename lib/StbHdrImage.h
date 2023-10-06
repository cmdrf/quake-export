#ifndef STBHDRIMAGE_H
#define STBHDRIMAGE_H


/// Minimal C++ wrapper around an stb_image HDR image
class StbHdrImage
{
public:
	StbHdrImage(char const *filename, int desired_channels);

	StbHdrImage(const StbHdrImage&) = delete;
	StbHdrImage(StbHdrImage&& other);
	~StbHdrImage();

	StbHdrImage& operator=(const StbHdrImage&) = delete;
	StbHdrImage& operator=(StbHdrImage&& other);

	const float* Data() const {return mData;}
	int GetWidth() const {return mWidth;}
	int GetHeight() const {return mHeight;}
	int GetChannelsInFile() const {return mChannelsInFile;}

private:
	float* mData = nullptr;
	int mWidth = 0;
	int mHeight = 0;
	int mChannelsInFile = 0;
};

#endif // STBHDRIMAGE_H
