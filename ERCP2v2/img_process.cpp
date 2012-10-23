#include "img_process.h"

cv::Mat floatingGray(cv::Mat img){
	cv::Mat floatImg = cv::Mat(img.rows,img.cols,CV_32FC1);
	for (int j = 0; j<img.rows;j++)
	{
		uchar* data = img.ptr<uchar>(j);
		float* floatData = floatImg.ptr<float>(j);
		for (int i = 0; i<img.cols;i++)
		{
			*floatData++ = static_cast<float>(*data++)/255.0f;
		}
	}
	return floatImg;
}