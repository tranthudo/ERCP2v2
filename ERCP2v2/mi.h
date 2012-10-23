#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <cutil.h>


#include "cuda\cuda_basics.h"
#include "cuda\cuda_hist.h"
#include "cuda\cuda_mi.h"

#pragma once

#define BUFFER_LEN			141376
#define	BINS				80



struct Options 
{
	int bins;
	unsigned int len;
	int threads, blocks;
};

Options ReadOptions(int argc, char *argv[]);

void cpuHist2D(float *src1, float *src2, unsigned int *hist, int length, int xbins, int ybins);

float cpuMI(float *src1, float *src2, int length, int xbins, int ybins, double &time);
