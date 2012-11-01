#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "opencv2/opencv.hpp"
#include "optimizer/levmar/levmar.h"

#pragma once



#define N2T_LEAST_SQUARE 0
#define N2T_TUKEY 1
#define N2T_LEAST_SQUARE_JACOBIAN 2
#define N2T_TUKEY_JACOBIAN 3	


#define N2T_USE_JACOBIAN true
#define N2T_NOT_USE_JACOBIAN false

void leastSquare(double *p, double *x, int m, int n, void *adata);
void tukey(double *p, double *jac, int m, int n, void*adata);

struct N2tEstimatorData 
{
	cv::Mat ObjPoints;
	cv::Mat ImgPoints;
	cv::Mat distCoeffs;
	cv::Mat camera_intrinsic;	
};

class N2tEstimator
{
public:
	N2tEstimator();
	//N2tEstimator(std::vector<cv::Point3f>* oPoints, std::vector<cv::Point2f>* iPoints);
	~N2tEstimator();
	void estimate(cv::Mat& objPoints, cv::Mat& imgPoints, cv::Mat& camera_intrinsic, cv::Mat& distCoeffs,cv::Mat& rvec,cv::Mat& tvec, int mode = 0, bool jac=false);

private:	
	std::vector<cv::Point3f> objPoints;
	std::vector<cv::Point2f> imgPoints;
	
};

