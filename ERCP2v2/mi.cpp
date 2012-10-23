#include "mi.h"

Options ReadOptions(int argc, char *argv[])
{
	Options opts = {BINS, BUFFER_LEN, 0, 0};
	for (int i = 1; i < argc; i++)
	{
		if (strnicmp(argv[i], "-h", 2) == 0)
		{
			printf("Usage:\n");
			printf("mi_example [-b<number of bins>] [-l<data length>] [-k<number of GPU blocks>] [-t<threads per block>]\n");
			exit(0);
		}
		else if (strnicmp(argv[i], "-b", 2) == 0)
			opts.bins = strtol(argv[i] + 2, 0, 10); 
		else if (strnicmp(argv[i], "-l", 2) == 0)
			opts.len = strtol(argv[i] + 2, 0, 10); 
		else if (strnicmp(argv[i], "-t", 2) == 0)
			opts.threads = strtol(argv[i] + 2, 0, 10); 
		else if (strnicmp(argv[i], "-k", 2) == 0)
			opts.blocks = strtol(argv[i] + 2, 0, 10);
		else
		{
			printf("Invalid input argument. Use -h for help.");
			exit(1);
		}
	}

	return opts;
}

void cpuHist2D(float *src1, float *src2, unsigned int *hist, int length, int xbins, int ybins)
{
	for (int i = 0; i < length; i++)
	{
		unsigned int x = src1[i] * (xbins - 1) + 0.5f;
		unsigned int y = src2[i] * (ybins - 1) + 0.5f;
		hist[x + xbins * y]++;
	}
}

float cpuMI(float *src1, float *src2, int length, int xbins, int ybins, double &time)
{
	time = 0;
	unsigned int hTimer;
	CUT_SAFE_CALL(cutCreateTimer(&hTimer));

	CUT_SAFE_CALL(cutStartTimer(hTimer));								

	unsigned int *hist = new unsigned int [xbins * ybins];
	memset(hist, 0, sizeof(float) * xbins * ybins);

	cpuHist2D(src1, src2, hist, length, xbins, ybins);

	// Marginal and joint entropies
	float Hxy = 0, Hx = 0, Hy = 0;
	unsigned int *p_hist = hist;
	for (int y = 0; y < ybins; y++)
	{
		float Px = 0;
		for (int x = 0; x < xbins; x++, p_hist++)
		{
			if (*p_hist != 0)
			{
				float p = (float)*p_hist / length;
				Hxy -= p * log(p);
				Px += p;
			}
		}

		if (Px != 0)
			Hx -= Px * log(Px);
	}

	for (int x = 0; x < xbins; x++)
	{
		p_hist = hist + x;
		float Py = 0;
		for (int y = 0; y < ybins; y++, p_hist += xbins)
			Py += *p_hist;
		Py = Py / length;

		if (Py != 0)
			Hy -= Py * log(Py);
	}

	float MI = (Hx + Hy)/Hxy;
	delete []hist;

	CUT_SAFE_CALL(cutStopTimer(hTimer));
	time = cutGetTimerValue(hTimer);
	CUT_SAFE_CALL(cutDeleteTimer(hTimer));		

	return MI;
}