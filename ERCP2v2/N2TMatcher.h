
#pragma once

#include "opencv2/opencv.hpp"

class N2TMatcher
{
private:	
	float ratio;			// max ratio between 1st and 2NN 


public:
	N2TMatcher(void);
	~N2TMatcher(void);
	int ratioTest( std::vector<std::vector<cv::DMatch>>& matches);
	void symmetryTest( const std::vector<std::vector<cv::DMatch>>& matches1, const std::vector<std::vector<cv::DMatch>>& matches2, std::vector<cv::DMatch>& symMatches );	
};

