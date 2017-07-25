#ifndef VIFP_HPP
#define VIFP_HPP

#include "Metric.hpp"

class VIFP : public Metric {
public:
	VIFP() : Metric() { }
	VIFP(std::shared_ptr<cv::Mat> s, std::shared_ptr<cv::Mat> d);
	// Compute the VIFp index of the processed image
	float computeScore();
	void reset(std::shared_ptr<cv::Mat> s, std::shared_ptr<cv::Mat> d);
private:
	static const int NLEVS = 4;
	static const float SIGMA_NSQ;
	// Compute the coefficients of the VIFp index at a particular subband
	void computeVIFP(int N, double& num, double& den, int scale);

	std::shared_ptr<std::vector<std::shared_ptr<cv::Mat>>> vifpnumidx, vifpdenidx;
};

#endif