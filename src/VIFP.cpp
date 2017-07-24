#include "VIFP.hpp"

const float VIFP::SIGMA_NSQ = 2.0f;

VIFP::VIFP(std::shared_ptr<cv::Mat> s, std::shared_ptr<cv::Mat> d) : Metric(s, d)
{
	vifpnumidx = std::make_shared<std::vector<std::shared_ptr<cv::Mat>>>();
	vifpdenidx = std::make_shared<std::vector<std::shared_ptr<cv::Mat>>>();
}

float VIFP::computeScore()
{
	double num = 0.0;
	double den = 0.0;
	
	for (int scale=0; scale<NLEVS; scale++)
	{
		// N=2^(4-scale+1)+1;
		int N = (2 << (NLEVS-scale-1)) + 1;
		
		if (scale != 0)
		{
			applyGaussianBlur(*src, *src, N, N/5.0);
			applyGaussianBlur(*dst, *dst, N, N/5.0);

			width = (width-(N-1)) / 2;
			height = (height-(N-1)) / 2;
			
			cv::resize(*src, *src, cv::Size(width,height), 0, 0, cv::INTER_NEAREST);
			cv::resize(*dst, *dst, cv::Size(width,height), 0, 0, cv::INTER_NEAREST);
		}		
		computeVIFP(N, num, den, scale);
	}
	
	return float(num/den);
}

void VIFP::computeVIFP(int N, double& num, double& den, int scale)
{
	int w = width - (N-1);
	int h = height - (N-1);
	
	cv::Mat tmp(h,w,CV_32F), mu1(h,w,CV_32F), mu2(h,w,CV_32F), mu1_sq(h,w,CV_32F), mu2_sq(h,w,CV_32F), mu1_mu2(h,w,CV_32F), sigma1_sq(h,w,CV_32F), sigma2_sq(h,w,CV_32F), sigma12(h,w,CV_32F), g(h,w,CV_32F), sv_sq(h,w,CV_32F);
	cv::Mat sigma1_sq_th, sigma2_sq_th, g_th;
	
	applyGaussianBlur(*src, mu1, N, N/5.0);
	applyGaussianBlur(*dst, mu2, N, N/5.0);
	
	const float EPSILON = 1e-10f;

	cv::multiply(mu1, mu1, mu1_sq);
	cv::multiply(mu2, mu2, mu2_sq);
	cv::multiply(mu1, mu2, mu1_mu2);		
	
	cv::multiply(*src, *src, tmp);
	applyGaussianBlur(tmp, sigma1_sq, N, N/5.0);
	sigma1_sq -= mu1_sq;
	cv::multiply(*dst, *dst, tmp);
	applyGaussianBlur(tmp, sigma2_sq, N, N/5.0);
	sigma2_sq -= mu2_sq;
	cv::multiply(*src, *dst, tmp);
	applyGaussianBlur(tmp, sigma12, N, N/5.0);
	sigma12 -= mu1_mu2;
	
	cv::max(sigma1_sq, 0.0f, sigma1_sq);
	cv::max(sigma2_sq, 0.0f, sigma2_sq);
	
	tmp = sigma1_sq + EPSILON;
	cv::divide(sigma12, tmp, g);	
	cv::multiply(g, sigma12, tmp);
	sv_sq = sigma2_sq - tmp;
	
	cv::threshold(sigma1_sq, sigma1_sq_th, EPSILON, 1.0f, cv::THRESH_BINARY);

	cv::multiply(g, sigma1_sq_th, g);
	cv::multiply(sv_sq, sigma1_sq_th, sv_sq);
	cv::multiply(sigma2_sq, 1.0f - sigma1_sq_th, tmp);
	sv_sq += tmp;
	
	cv::threshold(sigma1_sq, sigma1_sq, EPSILON, 1.0f, cv::THRESH_TOZERO);	
	cv::threshold(sigma2_sq, sigma2_sq_th, EPSILON, 1.0f, cv::THRESH_BINARY);

	cv::multiply(g, sigma2_sq_th, g);
	cv::multiply(sv_sq, sigma2_sq_th, sv_sq);
	
	cv::threshold(g, g_th, 0.0f, 1.0f, cv::THRESH_BINARY);
	
	cv::multiply(sv_sq, g_th, sv_sq);
	cv::multiply(sigma2_sq, 1.0f - g_th, tmp);
	cv::add(sv_sq, tmp, sv_sq);
	
	cv::max(g, 0.0f, g);
	
	cv::max(sv_sq, EPSILON, sv_sq);
	
	sv_sq += SIGMA_NSQ;
	cv::multiply(g, g, g);
	cv::multiply(g, sigma1_sq, g);
	cv::divide(g, sv_sq, tmp);
	tmp += 1.0;
	auto depth = tmp.depth();
	cv::log(tmp, tmp);
	vifpnumidx->push_back(std::make_shared<cv::Mat>(tmp));
	num += cv::sum(tmp)[0] / log(10.0f);
	
	tmp = 1.0f + sigma1_sq / SIGMA_NSQ;
	cv::log(tmp, tmp);	
	vifpdenidx->push_back(std::make_shared<cv::Mat>(tmp));
	den += cv::sum(tmp)[0] / log(10.0f);
}

void VIFP::reset(std::shared_ptr<cv::Mat> s, std::shared_ptr<cv::Mat> d)
{
	Metric::reset(s, d);
	if (!vifpnumidx)
		vifpnumidx = std::make_shared<std::vector<std::shared_ptr<cv::Mat>>>();
	else
		vifpnumidx->clear();
	if (!vifpdenidx)
		vifpdenidx = std::make_shared<std::vector<std::shared_ptr<cv::Mat>>>();
	else
		vifpdenidx->clear();
}