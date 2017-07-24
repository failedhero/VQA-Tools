#include "Metric.hpp"

Metric::Metric(std::shared_ptr<cv::Mat> s, std::shared_ptr<cv::Mat> d) : src(s), dst(d)
{
	check();
}

void Metric::check()
{
	if (src->empty() || dst->empty() || src->size() != dst->size())
	{
		std::cerr << "The size of two images are not matched." << std::endl;
	}else{
		height = src->rows;
		width = src->cols;
	}
}

void Metric::reset(std::shared_ptr<cv::Mat> s, std::shared_ptr<cv::Mat> d)
{
	src = s;
	dst = d;
	check();
}

void Metric::applyGaussianBlur(cv::Mat& src, cv::Mat& dst, int ksize, double sigma)
{
	int invalid = (ksize-1)/2;
	cv::Mat tmp(src.rows, src.cols, CV_32F);
	cv::GaussianBlur(src, tmp, cv::Size(ksize,ksize), sigma);
	tmp(cv::Range(invalid, tmp.rows-invalid), cv::Range(invalid, tmp.cols-invalid)).copyTo(dst);
}