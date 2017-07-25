#include "InputYUV.hpp"

InputYUV::InputYUV(const std::string s, const int h, const int w, const int c) : inputPath(s), height(h), width(w)
{
	// data = std::make_shared<std::vector<std::shared_ptr<cv::Mat>>>();
	chooseChromaFormat(c);
	nframes = link->nframes;
}

std::shared_ptr<cv::Mat> InputYUV::readOneFrame()
{
	link->generateFrame();
	// data->push_back(link->yuv2RGB());
	// return std::make_shared<cv::Mat>(*(data->back()));
	return link->yuv2RGB();
}

void InputYUV::chooseChromaFormat(const int chromaFormat)
{
	switch (chromaFormat)
	{
		case CHROMA_SUBSAMP_400:
			link = std::make_shared<yuv400>(inputPath,height,width,chromaFormat);
			break;
		case CHROMA_SUBSAMP_411:
			link = std::make_shared<yuv411>(inputPath,height,width,chromaFormat);
			break;
		case CHROMA_SUBSAMP_420:
			link = std::make_shared<yuv420>(inputPath,height,width,chromaFormat);
			break;
		case CHROMA_SUBSAMP_422:
			link = std::make_shared<yuv422>(inputPath,height,width,chromaFormat);
			break;
		case CHROMA_SUBSAMP_444:
			link = std::make_shared<yuv444>(inputPath,height,width,chromaFormat);
			break;
		default:
			std::cerr << "Please check the YUV chromaFormat" << std::endl;
			link = nullptr;
			break;
	}
}

yuv::yuv(const std::string s, const int h, const int w, const int c) : height(h), width(w), chromaFormat(c)
{
	if (checkChromaFormat() && checkInputPath(s))
	{
		file.open(s, std::ifstream::binary);
		width_r = width*fwidth;
		height_r = height*fheight;
		frame = std::make_shared<cv::Mat>(height, width, CV_8UC3, cv::Scalar::all(0));
	}else{
		std::cerr << "Initial Failed." << std::endl;
		//exit(EXIT_FAILURE);
	}
}

bool yuv::checkInputPath(const std::string inputPath)
{
	if (_access(inputPath.c_str(), 4))
	{
		std::cerr << "Access the input file failed!" << std::endl;
		return false;
	}
	struct _stat buf;
	int ans;
	if ((ans = _stat(inputPath.c_str(), &buf)) != 0)
	{
		std::cerr << "Read file information failed!" << std::endl;
		return false;
	}
	auto yuv_size = buf.st_size;
	nframes = yuv_size / (width*height*(1.0 + 2.0*fheight*fwidth));
	if (((int)nframes - nframes) != 0)
	{
		std::cout << "Wrong resolution, chromaFormat, frame_step or file size." << std::endl;
		return false;
	}else{
		return true;
	}
}

bool yuv::checkChromaFormat()
{
	switch (chromaFormat)
	{
		case CHROMA_SUBSAMP_400:
			fwidth = 0;
			fheight = 0;
			break;
		case CHROMA_SUBSAMP_411:
			fwidth = 0.25;
			fheight = 1;
			break;
		case CHROMA_SUBSAMP_420:
			fwidth = 0.5;
			fheight = 0.5;
			break;
		case CHROMA_SUBSAMP_422:
			fwidth = 0.5;
			fheight = 1;
			break;
		case CHROMA_SUBSAMP_444:
			fwidth = 1;
			fheight = 1;
			break;
		default:
			std::cerr << "Please check the YUV chromaFormat" << std::endl;
			return false;
			break;
	}
	return true;
}

void yuv::readFile()
{
	if (!file.is_open())
	{
		std::cerr << "File is not open." << std::endl;
	}
	YMatrix = std::make_shared<cv::Mat>(height, width, CV_8U, cv::Scalar::all(0));
	UMatrix = std::make_shared<cv::Mat>(height_r, width_r, CV_8U, cv::Scalar::all(0));
	VMatrix = std::make_shared<cv::Mat>(height_r, width_r, CV_8U, cv::Scalar::all(0));

	file.read((char*)YMatrix->ptr<uchar>(0), sizeof(uchar)*width*height);
	file.read((char*)UMatrix->ptr<uchar>(0), sizeof(uchar)*width_r*height_r);
	file.read((char*)VMatrix->ptr<uchar>(0), sizeof(uchar)*width_r*height_r);
	
	copyYMatrix();
}

void yuv::copyYMatrix()
{
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			frame->at<cv::Vec3b>(i, j)[0] = YMatrix->at<uchar>(i, j);
		}
	}
}

std::shared_ptr<cv::Mat> yuv::yuv2RGB()
{
	auto rgb = std::make_shared<cv::Mat>(height, width, CV_8UC3, cv::Scalar::all(0));
	//auto rgb = std::make_shared<cv::Mat>(height, width, CV_8U, cv::Scalar::all(0));
	for(int i = 0; i < height; ++i)
	{
		for(int j = 0; j < width; ++j)
		{
			rgb->at<cv::Vec3b>(i,j)[2] = cv::saturate_cast<uchar>(frame->at<cv::Vec3b>(i,j)[0] + (frame->at<cv::Vec3b>(i,j)[2] - 128)*1.4075);
			rgb->at<cv::Vec3b>(i,j)[1] = cv::saturate_cast<uchar>(frame->at<cv::Vec3b>(i,j)[0] - (frame->at<cv::Vec3b>(i,j)[1] - 128)*0.3455 - (frame->at<cv::Vec3b>(i,j)[2] - 128)*0.7169);
			rgb->at<cv::Vec3b>(i,j)[0] = cv::saturate_cast<uchar>(frame->at<cv::Vec3b>(i,j)[0] + (frame->at<cv::Vec3b>(i,j)[1] - 128)*1.779);
			//rgb->at<uchar>(i, j) = cv::saturate_cast<uchar>(frame->at<cv::Vec3b>(i, j)[0] + (frame->at<cv::Vec3b>(i, j)[1] - 128)*1.779);
		}
	}
	cv::cvtColor(*rgb, *rgb, CV_RGB2GRAY);
	rgb->convertTo(*rgb, CV_32F);
	return rgb;
}

void yuv400::readFile()
{
	YMatrix = std::make_shared<cv::Mat>(height, width, CV_8U, cv::Scalar::all(0));
	UMatrix = std::make_shared<cv::Mat>(height_r, width_r, CV_8U, cv::Scalar::all(0));
	VMatrix = std::make_shared<cv::Mat>(height_r, width_r, CV_8U, cv::Scalar::all(0));

	file.read((char*)YMatrix->ptr<uchar>(0), sizeof(uchar)*width*height);
}

void yuv400::generateFrame()
{
	yuv400::readFile();
	for (int i = 0; i < height; ++i)
		for (int j = 0; j < width; ++j)
		{
			frame->at<cv::Vec3b>(i,j)[0] = YMatrix->at<uchar>(i,j);
			frame->at<cv::Vec3b>(i,j)[1] = 127;
			frame->at<cv::Vec3b>(i,j)[2] = 127;
		}
}

void yuv411::generateFrame()
{
	readFile();
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width/4.0; ++j)
		{
			frame->at<cv::Vec3b>(i,4*j)[1] = UMatrix->at<uchar>(i,j);
			frame->at<cv::Vec3b>(i,4*j)[2] = VMatrix->at<uchar>(i,j);
			if ((4*j+1) < width)
			{
				frame->at<cv::Vec3b>(i,4*j+1)[1] = UMatrix->at<uchar>(i,j);
				frame->at<cv::Vec3b>(i,4*j+1)[2] = VMatrix->at<uchar>(i,j);
			}
			if ((4*j+2) < width)
			{
				frame->at<cv::Vec3b>(i,4*j+2)[1] = UMatrix->at<uchar>(i,j);
				frame->at<cv::Vec3b>(i,4*j+2)[2] = VMatrix->at<uchar>(i,j);
			}
			if ((4 * j + 3) < width)
			{
				frame->at<cv::Vec3b>(i, 4 * j + 3)[1] = UMatrix->at<uchar>(i, j);
				frame->at<cv::Vec3b>(i, 4 * j + 3)[2] = VMatrix->at<uchar>(i, j);
			}
		}
	}
}

void yuv420::generateFrame()
{
	readFile();
	cv::Mat UMatrix1(height_r,width,CV_8U,cv::Scalar::all(0));
	cv::Mat VMatrix1(height_r,width,CV_8U,cv::Scalar::all(0));

	for(int i = 0; i < height_r; ++i)
	{
		for(int j = 0; j < width/2; ++j)
		{
			UMatrix1.at<uchar>(i,2*j) = UMatrix->at<uchar>(i,j);
			VMatrix1.at<uchar>(i,2*j) = VMatrix->at<uchar>(i,j);
			if (2*j+1 < width)
			{
				UMatrix1.at<uchar>(i,2*j+1) = UMatrix->at<uchar>(i,j);
				VMatrix1.at<uchar>(i,2*j+1) = VMatrix->at<uchar>(i,j);
			}					
		}
	}
	for(int i = 0; i < height/2; ++i)
	{
		for(int j = 0; j < width; ++j)
		{
			frame->at<cv::Vec3b>(2*i,j)[1] = UMatrix1.at<uchar>(i,j);
			frame->at<cv::Vec3b>(2*i,j)[2] = VMatrix1.at<uchar>(i,j);
			if(2*i+1 < height)
			{
				frame->at<cv::Vec3b>(2*i+1,j)[1] = UMatrix1.at<uchar>(i,j);
				frame->at<cv::Vec3b>(2*i+1,j)[2] = VMatrix1.at<uchar>(i,j);
			}
		}
	}
}

void yuv422::generateFrame()
{
	readFile();
	cv::Mat UMatrix1(height_r,width,CV_8U,cv::Scalar::all(0));
	cv::Mat VMatrix1(height_r,width,CV_8U,cv::Scalar::all(0));

	for(int i = 0; i < height_r; i++)
	{
		for(int j = 0; j < width/2; j++)
		{
			UMatrix1.at<uchar>(i,2*j) = UMatrix->at<uchar>(i,j);
			VMatrix1.at<uchar>(i,2*j) = VMatrix->at<uchar>(i,j);
			if (2*j+1 < width)
			{
				UMatrix1.at<uchar>(i,2*j+1) = UMatrix->at<uchar>(i,j);
				VMatrix1.at<uchar>(i,2*j+1) = VMatrix->at<uchar>(i,j);
			}					
		}
	}

	for(int i = 0; i < height/2; i++)
	{
		for(int j = 0; j < width; j ++)
		{
			frame->at<cv::Vec3b>(2*i,j)[1] = UMatrix1.at<uchar>(i,j);
			frame->at<cv::Vec3b>(2*i,j)[2] = VMatrix1.at<uchar>(i,j);
			if(2*i+1 < height)
			{
				frame->at<cv::Vec3b>(2*i+1,j)[1] = UMatrix1.at<uchar>(i,j);
				frame->at<cv::Vec3b>(2*i+1,j)[2] = VMatrix1.at<uchar>(i,j);
			}
		}
	}
}

void yuv444::generateFrame()
{
	readFile();
	for (int i = 0; i < height; ++i)
		for (int j = 0; j < width; ++j)
		{
			frame->at<cv::Vec3b>(i,j)[0] = YMatrix->at<uchar>(i,j);
			frame->at<cv::Vec3b>(i,j)[1] = UMatrix->at<uchar>(i,j);
			frame->at<cv::Vec3b>(i,j)[2] = VMatrix->at<uchar>(i,j);
		}
}