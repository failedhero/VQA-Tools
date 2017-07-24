#ifndef INPUTYUV_HPP
#define INPUTYUV_HPP

#include <string>
#include <fstream>
#include <iostream>
#include <memory>
#include <io.h>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

enum ChromaSubSampling
{
	CHROMA_SUBSAMP_400 = 0,
	CHROMA_SUBSAMP_411 = 1,
	CHROMA_SUBSAMP_420 = 2,
	CHROMA_SUBSAMP_422 = 3,
	CHROMA_SUBSAMP_444 = 4	
};

class yuv;

class InputYUV
{
public:
	InputYUV(const std::string s, const int h, const int w, const int c);
	~InputYUV() = default;

	std::shared_ptr<cv::Mat> readOneFrame();
	void close();

	unsigned int nframes = 0;
private:
	void chooseChromaFormat(const int chromaFormat);

	std::string inputPath;
	std::shared_ptr<std::vector<std::shared_ptr<cv::Mat>>> data;
	std::shared_ptr<yuv> link;

	int height = 0, width = 0;
};

class yuv
{
	friend class InputYUV;
public:
	yuv(const std::string s, const int h, const int w, const int c);
	~yuv() = default;
	virtual void generateFrame() = 0;
	std::shared_ptr<cv::Mat> yuv2RGB();

	float nframes = 0.0;
protected:
	bool checkInputPath(const std::string inputPath);
	bool checkChromaFormat();
	virtual void readFile();
	void copyYMatrix();
	void close() { file.close(); }

	int height = 0, width = 0, chromaFormat = 0;
	float fwidth = 0.0, fheight = 0.0;
	float width_r = 0.0, height_r = 0.0;
	std::ifstream file;

	std::shared_ptr<cv::Mat> YMatrix, UMatrix, VMatrix;
	std::shared_ptr<cv::Mat> frame;
};

class yuv400 : public yuv
{
public:
	yuv400(const std::string s, const int h, const int w, const int c) : yuv(s, h, w, c) { }
	void generateFrame();
protected:
	void readFile();
};

class yuv411 : public yuv
{
public:
	yuv411(const std::string s, const int h, const int w, const int c) : yuv(s, h, w, c) { }
	void generateFrame();
};

class yuv420 : public yuv
{
public:
	yuv420(const std::string s, const int h, const int w, const int c) : yuv(s, h, w, c) { }
	void generateFrame();
};

class yuv422 : public yuv
{
public:
	yuv422(const std::string s, const int h, const int w, const int c) : yuv(s, h, w, c) { }
	void generateFrame();
};

class yuv444 : public yuv
{
public:
	yuv444(const std::string s, const int h, const int w, const int c) : yuv(s, h, w, c) { }
	void generateFrame();
};

inline void InputYUV::close()
{
	link->close();
}

#endif