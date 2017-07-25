#ifndef MSSIM_hpp
#define MSSIM_hpp

#include "SSIM.hpp"

class MSSIM : public SSIM {
public:
	MSSIM() : SSIM() { }
	MSSIM(std::shared_ptr<cv::Mat> s, std::shared_ptr<cv::Mat> d);
	// Compute the SSIM and MS-SSIM indexes of the processed image
	// Return the MS-SSIM index
	float computeScore();
	// Return the SSIM index only
	// compute() needs to be called before getSSIM()
	float getSSIM();
	void reset(std::shared_ptr<cv::Mat> s, std::shared_ptr<cv::Mat> d);
private:

	double ssim = 0.0, msssim = 0.0;
	static const int NLEVS = 5;
	static const double WEIGHT[];
	std::shared_ptr<std::vector<std::shared_ptr<cv::Mat>>> mssimIdx, mcsIdx;
};

#endif