#ifndef SSIM_HPP
#define SSIM_HPP

#include "Metric.hpp"

class SSIM : public Metric
{
public:
	SSIM() : Metric() { }
	SSIM(std::shared_ptr<cv::Mat> s, std::shared_ptr<cv::Mat> d);
	void reset(std::shared_ptr<cv::Mat> s, std::shared_ptr<cv::Mat> d);
	float computeScore();
protected:
	std::pair<double, double> computeSSIM();

	std::shared_ptr<cv::Mat> ssimIdx, csIdx;

	static const float C1;
	static const float C2;
};

#endif