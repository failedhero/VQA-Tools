#ifndef PSNR_HPP
#define PSNR_HPP

#include "Metric.hpp"

class PSNR : public Metric
{
public:
	PSNR() : Metric() { }
	PSNR(std::shared_ptr<cv::Mat> s, std::shared_ptr<cv::Mat> d);
	void reset(std::shared_ptr<cv::Mat> s, std::shared_ptr<cv::Mat> d);
	float computeScore();
private:
	std::shared_ptr<cv::Mat> psnrIdx;
};

#endif