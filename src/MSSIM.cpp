#include "MSSIM.hpp"

const double MSSIM::WEIGHT[] = { 0.0448, 0.2856, 0.3001, 0.2363, 0.1333 };

MSSIM::MSSIM(std::shared_ptr<cv::Mat> s, std::shared_ptr<cv::Mat> d) : SSIM(s, d)
{
	mssimIdx = std::make_shared<std::vector<std::shared_ptr<cv::Mat>>>();
	mcsIdx = std::make_shared<std::vector<std::shared_ptr<cv::Mat>>>();
}

float MSSIM::computeScore()
{
	double mssim[NLEVS];
	double mcs[NLEVS];
	
	for (int l=0; l<NLEVS; l++) {
		auto res = SSIM::computeSSIM();
		mssimIdx->push_back(std::make_shared<cv::Mat>(*ssimIdx));
		mcsIdx->push_back(std::make_shared<cv::Mat>(*csIdx));

		mssim[l] = res.first;
		mcs[l] = res.second;

		if (l < NLEVS-1) {
			width /= 2;
			height /= 2;

			cv::resize(*src, *src, cv::Size(width,height), 0, 0, cv::INTER_LINEAR);
			cv::resize(*dst, *dst, cv::Size(width, height), 0, 0, cv::INTER_LINEAR);
		}
	}

	ssim = mssim[0];

	msssim = mssim[NLEVS-1];
	for (int l=0; l<NLEVS-1; l++)
		msssim *= pow(mcs[l], WEIGHT[l]);

	return float(msssim);
}

void MSSIM::reset(std::shared_ptr<cv::Mat> s, std::shared_ptr<cv::Mat> d)
{
	SSIM::reset(s, d);
	if (!mssimIdx)
		mssimIdx = std::make_shared<std::vector<std::shared_ptr<cv::Mat>>>();
	else
		mssimIdx->clear();
	if (!mcsIdx)
		mcsIdx = std::make_shared<std::vector<std::shared_ptr<cv::Mat>>>();
	else
		mcsIdx->clear();
}

float MSSIM::getSSIM()
{
	return float(ssim);
}

float MSSIM::getMSSIM()
{
	return float(msssim);
}