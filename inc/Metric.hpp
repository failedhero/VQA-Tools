#ifndef METRIC_HPP
#define METRIC_HPP

#include <cmath>
#include <iostream>
#include <memory>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

class Metric
{
public:
	Metric() : src(nullptr), dst(nullptr) { }
	Metric(std::shared_ptr<cv::Mat> s, std::shared_ptr<cv::Mat> d);
	~Metric() = default;

	virtual void reset(std::shared_ptr<cv::Mat> s, std::shared_ptr<cv::Mat> d);
	virtual float computeScore() = 0;
	virtual float getSSIM() { return 0.0;}
protected:
	void applyGaussianBlur(cv::Mat &src, cv::Mat &dst, int ksize, double sigma);
	void check();
	int height = 0, width = 0;
	std::shared_ptr<cv::Mat> src, dst;
};

#endif