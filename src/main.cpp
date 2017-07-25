#include <map>
#include <set>
#include <utility>
#include <direct.h>
#include "Metric.hpp"
#include "PSNR.hpp"
#include "SSIM.hpp"
#include "MSSIM.hpp"
#include "VIFP.hpp"
#include "InputYUV.hpp"

//please comment the following sentences if release the program
#define DEBUG

enum Params
{
	PARAM_ORIGINAL = 1,
	PARAM_PROCESSED,
	PARAM_HEIGHT,
	PARAM_WIDTH,
	PARAM_CHROMA,
	PARAM_RESULTS,
	PARAM_METRICS,
	PARAM_SIZE
};

static const std::set<std::string> METRIC{"PSNR", "SSIM", "MSSIM", "VIFP"};
typedef std::map<std::string, std::vector<float>> RESULT;

bool checkDirPath(std::string &s)
{
	if (s[s.size() - 1] != '/')
		s.append("/");
	if (_access(s.c_str(), 4))
	{
		auto pos = find(s.cbegin(), s.cend()-1, '/');
		if (!_access(s.substr(0, pos - s.cbegin() + 1).c_str(), 4))
		{
			_mkdir(s.c_str());
			if (_access(s.c_str(), 4))
			{
				std::cerr << "Create Output Directory Failed: \"" << s << "\"." << std::endl;
				return false;
			}else{
				return true;
			}
		}else{
			std::cerr << "Please Create the root Output Directory menually \"" << s.substr(0, pos - s.cbegin() + 1) << "\"." << std::endl;
			return false;
		}
	}else{
		return true;
	}
}

void saveResult(std::string &outDir, std::shared_ptr<RESULT> result)
{
	std::string ans;
	while(!checkDirPath(outDir))
	{
		std::cout << "Re-print the Output Dir, or quit with print \"q\", Warning: all date will be missing." << std::endl;
		std::cin >> ans;
		if (!ans.empty() && ans == "q" || ans == "Q")
		{
			break;
		}
	}
	if (ans == "q" || ans == "Q")
		return;
	for (auto elem : *result)
	{
		std::string m(outDir + elem.first + ".txt");
		std::ofstream out(m);
		if (!out.is_open())
		{
			std::cerr << "Permission Asserted. Create Output Text File Failed: \"" << m << "\"." << std::endl;
			return;
		}
		float mean = 0.0;
		int idx = 0;
		for(auto c : elem.second)
		{
			mean += c;
			++idx;
			out << c << std::endl;
		}
		out << "The final Score: " << mean/idx << " (" << idx << " frame)";
		out.close();
	}
}

int main(int argc, char const *argv[])
{
	int height = 0, width = 0, chromaFormat = 0;
	std::string srcPath, dstPath, outDir;
	std::set<std::string> metricSEQ;

	#ifndef DEBUG
	if (argc < PARAM_SIZE)
	{
		std::cerr << "Check software usage : at least " << PARAM_SIZE << " parameters are required." << std::endl;
		exit(EXIT_FAILURE);
	}

	srcPath.assign(argv[PARAM_ORIGINAL]);
	dstPath.assign(argv[PARAM_PROCESSED]);
	outDir.assign(argv[PARAM_RESULTS]);
	height = std::stoi(argv[PARAM_HEIGHT]);
	width = std::stoi(argv[PARAM_WIDTH]);
	chromaFormat = std::stoi(argv[PARAM_CHROMA]);

	// metricSEQ.insert(argv + PARAM_METRICS, argv + argc);
	for (int i = PARAM_METRICS; i < argc; ++i)
		if (METRIC.find(argv[i]))
			metricSEQ.insert(argv[i]);
		else
			std::cerr << "Metric - " << argv[i] << " is not found" << std::endl;
	#else
	srcPath.assign("G:/Database/LIVE/bs/bs01_25fps.yuv");
	dstPath.assign("G:/Database/LIVE/bs/bs02_25fps.yuv");
	outDir.assign("G:/Database/LIVE/");
	height = std::stoi("432");
	width = std::stoi("768");
	chromaFormat = std::stoi("2");

	metricSEQ.insert({ "SSIM", "MSSIM"});
	#endif // DEBUG

	if (srcPath.empty() || dstPath.empty() || height <= 0 || width <= 0 || chromaFormat < 0 || chromaFormat > 5 || metricSEQ.empty())
	{
		std::cerr << "Invalid Argumnets" << std::endl;
		exit(EXIT_FAILURE);
	}

	InputYUV src(srcPath, height, width, chromaFormat), dst(dstPath, height, width, chromaFormat);

	std::shared_ptr<RESULT> result = std::make_shared<RESULT>();
	std::map<std::string, std::shared_ptr<Metric>> method;

	for(auto cb = metricSEQ.cbegin(); cb != metricSEQ.cend(); ++cb)
	{
		if (*cb == "PSNR")
		{
			method.insert(std::make_pair("PSNR", std::make_shared<PSNR>()));
			result->insert(std::make_pair("PSNR", std::vector<float>()));
		}else if(*cb == "SSIM"){
			method.insert(std::make_pair("SSIM", std::make_shared<SSIM>()));
			result->insert(std::make_pair("SSIM", std::vector<float>()));
		}else if (*cb == "MSSIM") {
			method.insert(std::make_pair("MSSIM", std::make_shared<MSSIM>()));
			result->insert(std::make_pair("MSSIM", std::vector<float>()));
		}else if (*cb == "VIFP") {
			method.insert(std::make_pair("VIFP", std::make_shared<VIFP>()));
			result->insert(std::make_pair("VIFP", std::vector<float>()));
		}else {
			std::cerr << "No Metric been choosed." << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	const int nframes = src.nframes;
	for (size_t i = 0; i < nframes; i++)
	{
		std::cout << i+1 << ":";
		auto srcIMG = src.readOneFrame();
		auto dstIMG = dst.readOneFrame();
		for(auto cb = metricSEQ.cbegin(); cb != metricSEQ.cend(); ++cb)
		{
			auto m = method.find(*cb);
			m->second->reset(srcIMG, dstIMG);
			if (*cb == "SSIM")
			{
				if (method.find("MSSIM") == method.end())
				{
					std::cout << *cb << " ";
					result->find(*cb)->second.push_back(m->second->computeScore());
				}
			}else if (*cb == "MSSIM"){
				std::cout << *cb << " ";
				result->find(*cb)->second.push_back(m->second->computeScore());
				if (method.find("SSIM") != method.end())
				{
					std::cout << "SSIM ";
					result->find("SSIM")->second.push_back(m->second->getSSIM());
				}
			}else{
				std::cout << *cb << " ";
				result->find(*cb)->second.push_back(m->second->computeScore());
			}			
		}
		std::cout << std::endl;
	}

	saveResult(outDir, result);

	src.close();
	dst.close();
	system("pause");
}