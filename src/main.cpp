#include <map>
#include "Metric.hpp"
#include "PSNR.hpp"
#include "SSIM.hpp"
#include "MSSIM.hpp"
#include "VIFP.hpp"
#include "InputYUV.hpp"

#define DEBUG

enum Params
{
	PARAM_ORIGINAL = 1,
	PARAM_PROCESSED,
	PARAM_HEIGHT,
	PARAM_WIDTH,
	PARAM_CHROMA,
	PARAM_RESULTS,
	PARAM_METRICS,
	PARAM_SIZE
};

enum Metrics {
	METRIC_PSNR = 0,
	METRIC_SSIM,
	METRIC_MSSIM,
	METRIC_VIFP,
	METRIC_SIZE
};

int main(int argc, char const *argv[])
{
	#ifndef DEBUG
	if (argc < PARAM_SIZE)
	{
		std::cerr << "Check software usage : at least " << PARAM_SIZE << " parameters are required." << std::endl;
		exit(EXIT_FAILURE);
	}

	std::string srcPath(argv[PARAM_ORIGINAL]);
	std::string dstPath(argv[PARAM_PROCESSED]);
	int height = std::stoi(argv[PARAM_HEIGHT]);
	int width = std::stoi(argv[PARAM_WIDTH]);
	int chromaFormat = std::stoi(argv[PARAM_CHROMA]);
	#else
	std::string srcPath("G:/Database/LIVE/bs/bs01_25fps.yuv");
	std::string dstPath("G:/Database/LIVE/bs/bs02_25fps.yuv");
	int height = std::stoi("432");
	int width = std::stoi("768");
	int chromaFormat = std::stoi("2");
	#endif // DEBUG


	if (srcPath.empty() || dstPath.empty() || height <= 0 || width <= 0 || chromaFormat < 0 || chromaFormat > 5)
	{
		std::cerr << "Invalid Argumnets" << std::endl;
		exit(EXIT_FAILURE);
	}

	InputYUV src(srcPath, height, width, chromaFormat), dst(dstPath, height, width, chromaFormat);

	int nframes = src.nframes;

	std::shared_ptr<std::vector<std::map<std::string, std::vector<float>>>> result = std::make_shared<std::vector<std::map<std::string, std::vector<float>>>>();

	SSIM ssim;
	float score;

	for (size_t i = 0; i < nframes; i++)
	{
		auto srcIMG =  src.readOneFrame();
		auto dstIMG = dst.readOneFrame();

		ssim.reset(srcIMG, dstIMG);
		std::cout << ssim.computeScore() << std::endl;
	}
	src.close();
	dst.close();
	system("pause");
}