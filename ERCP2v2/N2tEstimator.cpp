#include "N2tEstimator.h"



void N2tEstimator::estimate( cv::Mat& objPoints, cv::Mat& imgPoints, cv::Mat& camera_intrinsic, cv::Mat& distCoeffs,cv::Mat& rvec,cv::Mat& tvec, int mode, bool jac )
{
	N2tEstimatorData* n2tData = new N2tEstimatorData();
	objPoints.copyTo(n2tData->ObjPoints);
	imgPoints.copyTo(n2tData->ImgPoints);
	camera_intrinsic.copyTo(n2tData->camera_intrinsic);
	distCoeffs.copyTo(n2tData->distCoeffs);
	
	n2tData->distCoeffs = distCoeffs;
	double *_r = rvec.ptr<double>();
	double *_t = tvec.ptr<double>();	
	double p[6] = {_r[0], _r[1], _r[2],_t[0], _t[1], _t[2]};
	double x[1];
	cv::Mat errPoints;
	cv::projectPoints(objPoints,rvec,tvec,camera_intrinsic,distCoeffs,errPoints);
	errPoints = errPoints - imgPoints;
	x[0] = cv::norm(errPoints);	
	int m, n;
	double opts[LM_OPTS_SZ], info[LM_INFO_SZ]; 
	opts[0] = LM_INIT_MU;// tau = LM_INIT_MU = 1E-3;
	opts[1] = 1E-15;
	opts[2] = 1E-15;
	opts[3] = 1E-20;
	opts[4] = LM_DIFF_DELTA;

	// tukey function
	m = 6;
	n = 1;

	double *work = new double;
	double *covar = new double;
	double ret;	
	switch (mode) {
	case N2T_LEAST_SQUARE:
		break;
		ret = dlevmar_dif(leastSquare,p,x,m,n,100,opts,info,work, covar,n2tData);
	case N2T_TUKEY:
		ret = dlevmar_dif(tukey,p,x,m,n,100,opts,info,work, covar,n2tData);
		break;
	default:
		break;
	}

	
	cv::Mat __rvec(3,1,CV_64F,p);
	cv::Mat __tvec(3,1,CV_64F,p+3);
	__rvec.copyTo(rvec);
	__tvec.copyTo(tvec);
}

N2tEstimator::N2tEstimator()
{

}

N2tEstimator::~N2tEstimator()
{

}


void leastSquare( double *p, double *x, int m, int n, void *adata )
{
	N2tEstimatorData* n2tData = (N2tEstimatorData*)adata;
		
	cv::Mat rvec(3,1,CV_64F,p);
	cv::Mat tvec(3,1,CV_64F,p+3);
	cv::Mat errPoints;
	cv::projectPoints(n2tData->ObjPoints,rvec,tvec,n2tData->camera_intrinsic,n2tData->distCoeffs,errPoints);	
	errPoints = errPoints-n2tData->ImgPoints;
	x[0] = cv::norm(errPoints);		
}

void tukey( double *p, double *x, int m, int n, void*adata )
{
	N2tEstimatorData* n2tData = (N2tEstimatorData*)adata;
	x[0] = 0;
	cv::Mat rvec(3,1,CV_64F,p);
	cv::Mat tvec(3,1,CV_64F,p+3);
	cv::Mat errPoints;
	cv::projectPoints(n2tData->ObjPoints,rvec,tvec,n2tData->camera_intrinsic,n2tData->distCoeffs,errPoints);	
	errPoints = errPoints-n2tData->ImgPoints;
	for (int j = 0; j<errPoints.cols;j++)
	{
		float* data = errPoints.ptr<float>(j);
		double c = 2.0;
		for (int i = 0; i<errPoints.rows;i++)
		{
			double err =  data[i];
			if (err>c || err<-c){
				x[0] += c*c;
			}
			else x[0] +=err*err;
		}

	}	
}
