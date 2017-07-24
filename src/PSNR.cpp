#include "PSNR.hpp"

PSNR::PSNR(std::shared_ptr<cv::Mat> s, std::shared_ptr<cv::Mat> d) : Metric(s, d)
{
	psnrIdx = std::make_shared<cv::Mat>();
}

float PSNR::computeScore()
{
	cv::subtract(*src, *dst, *psnrIdx);
	return float(10*log10(255*255/cv::mean(*psnrIdx).val[0]));
}

void PSNR::reset(std::shared_ptr<cv::Mat> s, std::shared_ptr<cv::Mat> d)
{
	Metric::reset(s, d);
	if (!psnrIdx)
	{
		psnrIdx = std::make_shared<cv::Mat>();
	}
}