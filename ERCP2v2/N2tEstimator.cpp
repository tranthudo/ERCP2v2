#include "N2tEstimator.h"
 #include <QDebug>
#include <stdio.h>
#include <assert.h>

const int N_MIN = 15;
const int N_MAX = 30;
const double lambda = 0.5;
const double lambda_max = 3.5;
N2tEstimator::N2tEstimator()
{
	double opts[LM_OPTS_SZ], info[LM_INFO_SZ]; 
	opts[0] = LM_INIT_MU;// tau = LM_INIT_MU = 1E-3;
	opts[1] = 1E-15;
	opts[2] = 1E-15;
	opts[3] = 1E-20;
	opts[4] = LM_DIFF_DELTA;

	// tukey function
	int m = 6;
	int n = 300;
	//n = 1;// if 1 measurement	
	work = new double[(LM_DIF_WORKSZ(m, n)+m*m)*sizeof(double)];
	covar = work +LM_DIF_WORKSZ(m,n);
}

N2tEstimator::~N2tEstimator()
{

}

void N2tEstimator::estimate( cv::Mat& objPoints, cv::Mat& imgPoints, cv::Mat& camera_intrinsic, cv::Mat& distCoeffs,cv::Mat& rvec,cv::Mat& tvec, int mode, bool jac, int number_first_matches )
{
	assert(imgPoints.cols*imgPoints.rows<=1000);
	cv::Mat errPoints;
	cv::projectPoints(objPoints,rvec,tvec,camera_intrinsic,distCoeffs,errPoints);
	errPoints = errPoints - imgPoints;
	/*for (int j = 0; j<imgPoints.cols;j++)
	{
		float* data = imgPoints.ptr<float>(j);
		for (int i = 0; i<imgPoints.rows; i++)
		{
			x[j*imgPoints.cols+i]  = (double)data[i];
		}
	}*/
	double x[1000] = {0};
	int m = 6;
	int n = imgPoints.cols*imgPoints.rows;
	double opts[LM_OPTS_SZ], info[LM_INFO_SZ]; 
	opts[0] = LM_INIT_MU;// tau = LM_INIT_MU = 1E-3;
	opts[1] = 1E-15;
	opts[2] = 1E-15;
	opts[3] = 1E-20;
	opts[4] = LM_DIFF_DELTA;

	N2tEstimatorData* n2tData = new N2tEstimatorData();
	objPoints.copyTo(n2tData->ObjPoints);
	imgPoints.copyTo(n2tData->ImgPoints);
	camera_intrinsic.copyTo(n2tData->camera_intrinsic);
	distCoeffs.copyTo(n2tData->distCoeffs);
	n2tData->number_of_first_matches = number_first_matches;
	
	n2tData->distCoeffs = distCoeffs;
	cv::Mat _rvec;
	cv::Mat _tvec;
	rvec.copyTo(_rvec);
	tvec.copyTo(_tvec);
	double *_r = _rvec.ptr<double>();
	double *_t = _tvec.ptr<double>();	
	double p[6] = {_r[0], _r[1], _r[2],_t[0], _t[1], _t[2]};
	
	double ret;	
	switch (mode) {
	case N2T_LEAST_SQUARE:		
		if (jac == N2T_NOT_USE_JACOBIAN)
			ret = dlevmar_dif(leastSquare,p,x,m,n,100,opts,info,work, covar,n2tData);
		else 
			ret = dlevmar_der(leastSquare,jaLeastSquare,p,x,m,n,100,opts,info,work,covar,n2tData);
		break;
	case N2T_TUKEY:
		if (jac == N2T_NOT_USE_JACOBIAN)
			ret = dlevmar_dif(tukey,p,x,m,n,100,opts,info,work, covar,n2tData);
		else 
			ret = dlevmar_der(tukey,jaTukey,p,x,m,n,100,opts,info,work,covar,n2tData);
		break;	
	case N2T_HUBER:
		if (jac == N2T_NOT_USE_JACOBIAN)
			ret = dlevmar_dif(huber,p,x,m,n,100,opts,info,work, covar,n2tData);
		else 
			ret = dlevmar_der(huber,jaHuber,p,x,m,n,100,opts,info,work,covar,n2tData);
		break;	
	case N2T_CAUCHY:
		if (jac == N2T_NOT_USE_JACOBIAN)
			ret = dlevmar_dif(huber,p,x,m,n,100,opts,info,work, covar,n2tData);
		else 
			ret = dlevmar_der(huber,jaHuber,p,x,m,n,100,opts,info,work,covar,n2tData);
		break;	

	default:
		break;
	}
	
	qDebug()<<"Number of Iterations = "<<info[5];
	qDebug()<<"Reason for terminating = " << info[6];
	
	_rvec = cv::Mat(3,1,CV_64F,p);
	_tvec = cv::Mat(3,1,CV_64F,p+3);
	_rvec.copyTo(rvec);
	_tvec.copyTo(tvec);	
}



void meyer(double*p, double*x, int m, int n, void *data)
{
	register int i;
	double ui;
	for (i = 0; i<n; i++)
	{
		ui = 0.45+0.05*i;
		x[i] = p[0]*exp(10.0*p[1]/(ui+p[2]-13.0));
	}
}

void leastSquare( double *p, double *x, int m, int n, void *adata )
{
	N2tEstimatorData* n2tData = (N2tEstimatorData*)adata;	
	cv::Mat rvec(3,1,CV_64F,p);
	cv::Mat tvec(3,1,CV_64F,p+3);
	cv::Mat errPoints;	
	cv::projectPoints(n2tData->ObjPoints,rvec,tvec,n2tData->camera_intrinsic,n2tData->distCoeffs,errPoints);	
	errPoints = errPoints-n2tData->ImgPoints;
		
	for (int j = 0; j<errPoints.rows;j++) {
		float* data = errPoints.ptr<float>(j);
		for (int i = 0; i<errPoints.cols; i++) {
			x[j*errPoints.cols+i]  = data[i];
		}
	}
	
}
void jaLeastSquare( double *p, double *jac, int m, int n, void *adata )
{
	N2tEstimatorData* n2tData = (N2tEstimatorData*)adata;

	cv::Mat rvec(3,1,CV_64F,p);
	cv::Mat tvec(3,1,CV_64F,p+3);
	cv::Mat errPoints;	
	cv::Mat jacobian;
	cv::projectPoints(n2tData->ObjPoints,rvec,tvec,n2tData->camera_intrinsic,n2tData->distCoeffs,errPoints,jacobian);	
	//errPoints = errPoints-n2tData->ImgPoints;	

	//qDebug()<<"m = "<<m<<"; n= "<<n<<"; size of jacobian=("<<jacobian.rows<<","<<jacobian.cols<<")";
	for (int j = 0; j<m; j++){
		double* data= jacobian.ptr<double>(j);		
		for (int i = 0; i <n; i++){
			jac[n*j+i] = data[i];
		}		
	}
	//qDebug()<<"number of jacobian element"<<m*n;
}
void tukey( double *p, double *x, int m, int n, void*adata )
{
	N2tEstimatorData* n2tData = (N2tEstimatorData*)adata;

	cv::Mat rvec(3,1,CV_64F,p);
	cv::Mat tvec(3,1,CV_64F,p+3);
	cv::Mat errPoints;
	cv::projectPoints(n2tData->ObjPoints,rvec,tvec,n2tData->camera_intrinsic,n2tData->distCoeffs,errPoints);	
	errPoints = errPoints-n2tData->ImgPoints;	
	double c = 2.0;
	double w1;
	if (n2tData->number_of_first_matches<N_MIN){
		double dd = (N_MIN-n2tData->number_of_first_matches)/N_MAX;
		w1 = lambda-0.1*dd*dd;
	}	else if (n2tData->number_of_first_matches<N_MAX){
		double dd = (n2tData->number_of_first_matches-N_MIN)/N_MAX;
		w1 = lambda+dd*dd;
	}
	else {
		double dd = (N_MAX-N_MIN)/N_MAX;
		w1 = lambda+dd*dd;
	}
	/*if (n2tData->number_of_first_matches<N_MIN){
		w1 = lambda;
	}	else if (n2tData->number_of_first_matches<N_MAX){
		w1 = (lambda_max-lambda)*(n2tData->number_of_first_matches-N_MIN)/(N_MAX-N_MIN);
	}
	else {
		w1=lambda_max;
	}*/

		

	for (int j = 0; j<errPoints.rows;j++)
	{
		float* data = errPoints.ptr<float>(j);
		for (int i = 0; i<errPoints.cols; i++)
		{
			if (data[i]<-c)
				x[j*errPoints.cols+i] = -c;
			else if (data[i]>c) 
				x[j*errPoints.cols+i] = c;
			else 
				x[j*errPoints.cols+i] = data[i];	
			if (i<(n2tData->number_of_first_matches*2))
				x[j*errPoints.cols+i] *=w1;


			/*else{
				double dd2 ;
				if (n2tData->number_of_first_matches>N_MIN)
					dd2= 1-(n2tData->number_of_first_matches-N_MIN)/N_MAX;
				else  dd2 = 1;
				x[j*errPoints.cols+i] *=dd2;
			}*/

				
		}
	}
}



void jaTukey( double *p, double *jac, int m, int n, void* adata )
{
	N2tEstimatorData* n2tData = (N2tEstimatorData*)adata;

	cv::Mat rvec(3,1,CV_64F,p);
	cv::Mat tvec(3,1,CV_64F,p+3);
	cv::Mat errPoints;	
	cv::Mat jacobian;
	cv::projectPoints(n2tData->ObjPoints,rvec,tvec,n2tData->camera_intrinsic,n2tData->distCoeffs,errPoints,jacobian);	
	errPoints = errPoints-n2tData->ImgPoints;		
	//qDebug()<<"m = "<<m<<"; n= "<<n<<"; size of jacobian=("<<jacobian.rows<<","<<jacobian.cols<<")";	
	double w1;
	if (n2tData->number_of_first_matches<N_MIN){
		double dd = (N_MIN-n2tData->number_of_first_matches)/N_MAX;
		w1 = lambda-0.1*dd*dd;
	}	else if (n2tData->number_of_first_matches<N_MAX){
		double dd = (n2tData->number_of_first_matches-N_MIN)/N_MAX;
		w1 = lambda+dd*dd;
	}
	else {
		double dd = (N_MAX-N_MIN)/N_MAX;
		w1 = lambda+dd*dd;
	}
	float c = 2.0;
	for (int j = 0; j<m; j++){
		float* data1 = errPoints.ptr<float>(j);
		double* data= jacobian.ptr<double>(j);		
		for (int i = 0; i <n; i++){
			if (abs(data1[i])<c)	{
				double w3 = (data1[i])/c;
				double w2 = (1-w3*w3);
				jac[n*j+i] = w2*w2*data[i];
			}
						
			else jac[n*j+i] = 0;
			if (i<(n2tData->number_of_first_matches*2))
				jac[n*j+i] *=w1;
		}		
	}

}

void huber( double *p, double *x, int m, int n, void*adata )
{
	N2tEstimatorData* n2tData = (N2tEstimatorData*)adata;

	cv::Mat rvec(3,1,CV_64F,p);
	cv::Mat tvec(3,1,CV_64F,p+3);
	cv::Mat errPoints;
	cv::projectPoints(n2tData->ObjPoints,rvec,tvec,n2tData->camera_intrinsic,n2tData->distCoeffs,errPoints);	
	errPoints = errPoints-n2tData->ImgPoints;	
	double c = 2.0;
	double w1;
	if (n2tData->number_of_first_matches<N_MIN){
		double dd = (N_MIN-n2tData->number_of_first_matches)/N_MAX;
		w1 = lambda-0.1*dd*dd;
	}	else if (n2tData->number_of_first_matches<N_MAX){
		double dd = (n2tData->number_of_first_matches-N_MIN)/N_MAX;
		w1 = lambda+dd*dd;
	}
	else {
		double dd = (N_MAX-N_MIN)/N_MAX;
		w1 = lambda+dd*dd;
	}
	



	/*if (n2tData->number_of_first_matches<N_MIN){
		w1 = lambda;
	}	else if (n2tData->number_of_first_matches<N_MAX){
		w1 = (lambda_max-lambda)*(n2tData->number_of_first_matches-N_MIN)/(N_MAX-N_MIN);
	}
	else {
		w1=lambda_max;
	}*/

		

	for (int j = 0; j<errPoints.rows;j++)
	{
		float* data = errPoints.ptr<float>(j);
		for (int i = 0; i<errPoints.cols; i++)
		{
			if (data[i]<-c)
				x[j*errPoints.cols+i] = c*(abs(data[i])-c/2);
			else if (data[i]>c) 
				x[j*errPoints.cols+i] = c*(abs(data[i])-c/2);
			else 
				x[j*errPoints.cols+i] = (data[i])*(data[i])/2;	
			if (i<(n2tData->number_of_first_matches*2))
				x[j*errPoints.cols+i] *=w1;


			/*else{
				double dd2 ;
				if (n2tData->number_of_first_matches>N_MIN)
					dd2= 1-(n2tData->number_of_first_matches-N_MIN)/N_MAX;
				else  dd2 = 1;
				x[j*errPoints.cols+i] *=dd2;
			}*/

				
		}
	}
}

void jaHuber( double *p, double *jac, int m, int n, void* adata )
{
	N2tEstimatorData* n2tData = (N2tEstimatorData*)adata;

	cv::Mat rvec(3,1,CV_64F,p);
	cv::Mat tvec(3,1,CV_64F,p+3);
	cv::Mat errPoints;	
	cv::Mat jacobian;
	cv::projectPoints(n2tData->ObjPoints,rvec,tvec,n2tData->camera_intrinsic,n2tData->distCoeffs,errPoints,jacobian);	
	errPoints = errPoints-n2tData->ImgPoints;		
	//qDebug()<<"m = "<<m<<"; n= "<<n<<"; size of jacobian=("<<jacobian.rows<<","<<jacobian.cols<<")";	
	double w1;
	if (n2tData->number_of_first_matches<N_MIN){
		double dd = (N_MIN-n2tData->number_of_first_matches)/N_MAX;
		w1 = lambda-0.1*dd*dd;
	}	else if (n2tData->number_of_first_matches<N_MAX){
		double dd = (n2tData->number_of_first_matches-N_MIN)/N_MAX;
		w1 = lambda+dd*dd;
	}
	else {
		double dd = (N_MAX-N_MIN)/N_MAX;
		w1 = lambda+dd*dd;
	}
	float c = 2.0;
	for (int j = 0; j<m; j++){
		float* data1 = errPoints.ptr<float>(j);
		double* data= jacobian.ptr<double>(j);		
		for (int i = 0; i <n; i++){
			if (abs(data1[i])<c)				
				jac[n*j+i] = data[i];			
			else jac[n*j+i] = c/abs(data1[i])*data[i];
			if (i<(n2tData->number_of_first_matches*2))
				jac[n*j+i] *=w1;
		}		
	}
}

void cauchy( double *p, double *x, int m, int n, void *adata )
{
	N2tEstimatorData* n2tData = (N2tEstimatorData*)adata;

	cv::Mat rvec(3,1,CV_64F,p);
	cv::Mat tvec(3,1,CV_64F,p+3);
	cv::Mat errPoints;
	cv::projectPoints(n2tData->ObjPoints,rvec,tvec,n2tData->camera_intrinsic,n2tData->distCoeffs,errPoints);	
	errPoints = errPoints-n2tData->ImgPoints;	
	double c = 2.0;
	double w1;
	if (n2tData->number_of_first_matches<N_MIN){
		double dd = (N_MIN-n2tData->number_of_first_matches)/N_MAX;
		w1 = lambda-0.1*dd*dd;
	}	else if (n2tData->number_of_first_matches<N_MAX){
		double dd = (n2tData->number_of_first_matches-N_MIN)/N_MAX;
		w1 = lambda+dd*dd;
	}
	else {
		double dd = (N_MAX-N_MIN)/N_MAX;
		w1 = lambda+dd*dd;
	}
	



	/*if (n2tData->number_of_first_matches<N_MIN){
		w1 = lambda;
	}	else if (n2tData->number_of_first_matches<N_MAX){
		w1 = (lambda_max-lambda)*(n2tData->number_of_first_matches-N_MIN)/(N_MAX-N_MIN);
	}
	else {
		w1=lambda_max;
	}*/

		

	for (int j = 0; j<errPoints.rows;j++)
	{
		float* data = errPoints.ptr<float>(j);
		for (int i = 0; i<errPoints.cols; i++)
		{			
			double err = (data[i])/c;
			x[j*errPoints.cols+i] = c*c/2*log(1+err*err);			
			/*if (i<(n2tData->number_of_first_matches*2))
				x[j*errPoints.cols+i] *=w1;*/


			/*else{
				double dd2 ;
				if (n2tData->number_of_first_matches>N_MIN)
					dd2= 1-(n2tData->number_of_first_matches-N_MIN)/N_MAX;
				else  dd2 = 1;
				x[j*errPoints.cols+i] *=dd2;
			}*/

				
		}
	}
}

void jaCauchy( double *p, double *jac, int m, int n, void* adata )
{
	N2tEstimatorData* n2tData = (N2tEstimatorData*)adata;

	cv::Mat rvec(3,1,CV_64F,p);
	cv::Mat tvec(3,1,CV_64F,p+3);
	cv::Mat errPoints;	
	cv::Mat jacobian;
	cv::projectPoints(n2tData->ObjPoints,rvec,tvec,n2tData->camera_intrinsic,n2tData->distCoeffs,errPoints,jacobian);	
	errPoints = errPoints-n2tData->ImgPoints;		
	//qDebug()<<"m = "<<m<<"; n= "<<n<<"; size of jacobian=("<<jacobian.rows<<","<<jacobian.cols<<")";	
	double w1;
	if (n2tData->number_of_first_matches<N_MIN){
		double dd = (N_MIN-n2tData->number_of_first_matches)/N_MAX;
		w1 = lambda-0.1*dd*dd;
	}	else if (n2tData->number_of_first_matches<N_MAX){
		double dd = (n2tData->number_of_first_matches-N_MIN)/N_MAX;
		w1 = lambda+dd*dd;
	}
	else {
		double dd = (N_MAX-N_MIN)/N_MAX;
		w1 = lambda+dd*dd;
	}
	float c = 2.0;
	for (int j = 0; j<m; j++){
		float* data1 = errPoints.ptr<float>(j);
		double* data= jacobian.ptr<double>(j);		
		for (int i = 0; i <n; i++){
			double err = (data1[i])/c;
			err = 1/(1+err*err);
			jac[n*j+i] = err*data[i];
			
			/*if (i<(n2tData->number_of_first_matches*2))
				jac[n*j+i] *=w1;*/
		}		
	}
}





