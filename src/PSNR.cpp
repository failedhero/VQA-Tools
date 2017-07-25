#include "PSNR.hpp"

PSNR::PSNR(std::shared_ptr<cv::Mat> s, std::shared_ptr<cv::Mat> d) : Metric(s, d)
{
	psnrIdx = std::make_shared<cv::Mat>();
}

float PSNR::computeScore()
{
	cv::subtract(*src, *dst, *psnrIdx);
	cv::multiply(*psnrIdx, *psnrIdx, *psnrIdx);
	auto tmp = cv::mean(*psnrIdx).val[0];
	float val = 10 * log10(255 * 255 / tmp);
	std::cout << val << std::ends;
	return val;
}

void PSNR::reset(std::shared_ptr<cv::Mat> s, std::shared_ptr<cv::Mat> d)
{
	Metric::reset(s, d);
	if (!psnrIdx)
	{
		psnrIdx = std::make_shared<cv::Mat>();
	}
}