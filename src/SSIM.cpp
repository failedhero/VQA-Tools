#include "SSIM.hpp"

const float SSIM::C1 = 6.5025f;
const float SSIM::C2 = 58.5225f;

SSIM::SSIM(std::shared_ptr<cv::Mat> s, std::shared_ptr<cv::Mat> d) : Metric(s, d)
{
	ssimIdx = std::make_shared<cv::Mat>();
	csIdx = std::make_shared<cv::Mat>();
}

float SSIM::computeScore()
{
	auto res = computeSSIM();
	return float(res.first);
}

std::pair<double, double> SSIM::computeSSIM()
{
	int w = width - 10;
	int h = height - 10;

	cv::Mat mu1(h,w,CV_32F), mu2(h,w,CV_32F);
	cv::Mat mu1_sq(h,w,CV_32F), mu2_sq(h,w,CV_32F), mu1_mu2(h,w,CV_32F);
	cv::Mat src_sq(height,width,CV_32F), dst_sq(height, width,CV_32F), src_dst(height, width,CV_32F);
	cv::Mat sigma1_sq(h,w,CV_32F), sigma2_sq(h,w,CV_32F), sigma12(h,w,CV_32F);
	cv::Mat tmp1(h,w,CV_32F), tmp2(h,w,CV_32F), tmp3(h,w,CV_32F);
	cv::Mat ssim_map(h,w,CV_32F), cs_map(h,w,CV_32F);


	applyGaussianBlur(*src, mu1, 11, 1.5);
	applyGaussianBlur(*dst, mu2, 11, 1.5);

	cv::multiply(mu1, mu1, mu1_sq);
	cv::multiply(mu2, mu2, mu2_sq);
	cv::multiply(mu1, mu2, mu1_mu2);

	cv::multiply(*src, *src, src_sq);
	cv::multiply(*dst, *dst, dst_sq);
	cv::multiply(*src, *dst, src_dst);

	applyGaussianBlur(src_sq, sigma1_sq, 11, 1.5);
	sigma1_sq -= mu1_sq;

	applyGaussianBlur(dst_sq, sigma2_sq, 11, 1.5);
	sigma2_sq -= mu2_sq;

	applyGaussianBlur(src_dst, sigma12, 11, 1.5);
	sigma12 -= mu1_mu2;

	tmp1 = 2*sigma12 + C2;
	tmp2 = sigma1_sq + sigma2_sq + C2;
	cv::divide(tmp1, tmp2, *csIdx);

	tmp3 = 2*mu1_mu2 + C1;
	cv::multiply(tmp1, tmp3, tmp1);
	tmp3 = mu1_sq + mu2_sq + C1;
	cv::multiply(tmp2, tmp3, tmp2);
	cv::divide(tmp1, tmp2, *ssimIdx);

	double mssim = cv::mean(*ssimIdx).val[0];
	double mcs = cv::mean(*csIdx).val[0];

	return std::make_pair(mssim, mcs);
}

void SSIM::reset(std::shared_ptr<cv::Mat> s, std::shared_ptr<cv::Mat> d)
{
	Metric::reset(s, d);
	if (!ssimIdx)
	{
		ssimIdx = std::make_shared<cv::Mat>();
	}
	if (!csIdx)
	{
		csIdx = std::make_shared<cv::Mat>();
	}
}