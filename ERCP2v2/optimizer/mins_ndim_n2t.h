template <class T>
struct F1dim {
	const VecDoub &p;
	const VecDoub &xi;
	Int n;
	T &func;
	VecDoub xt;
	F1dim(VecDoub_I &pp, VecDoub_I &xii, T &funcc) : p(pp),
		xi(xii), n(pp.size()), func(funcc), xt(n) {}
	Doub operator() (const Doub x)
	{
		for (Int j=0;j<n;j++)
			xt[j]=p[j]+x*xi[j];
		return func(xt);
	}	
};
template <class T>
struct Linemethod {
	int currentVariable;// p[currentVariable];
	VecDoub p;
	VecDoub xi;
	T &func;
	Int n;
	Linemethod(T &funcc) : func(funcc) {}
	Doub linmin()
	{
		Doub ax,xx,xmin;
		n=p.size();
		F1dim<T> f1dim(p,xi,func);
		//qDebug("Linemethod limin p = [%f,%f,%f,%f,%f,%f], xi=[%f,%f,%f,%f%f,%f]",p[0],p[1],p[2],p[3],p[4],p[5],xi[0],xi[1],xi[2],xi[3],xi[4],xi[5]);
		ax=0.0;
		xx=1.0;
		Brent brent;
		brent.bracket(ax,xx,f1dim);
		//qDebug("After bracket: ax = %f, bx = %f, cx = %f, fa =%f, fb = %f, fc = %f",brent.ax,brent.bx,brent.cx,brent.fa,brent.fb,brent.fc);

		xmin=brent.minimize(f1dim);
		for (Int j=0;j<n;j++) {
			xi[j] *= xmin;
			p[j] += xi[j];
		}
		return brent.fmin;
	}
};
template <class T>
struct Df1dim {
	const VecDoub &p;
	const VecDoub &xi;
	Int n;
	T &funcd;
	VecDoub xt;
	VecDoub dft;
	Df1dim(VecDoub_I &pp, VecDoub_I &xii, T &funcdd) : p(pp),
		xi(xii), n(pp.size()), funcd(funcdd), xt(n), dft(n) {}
	Doub operator()(const Doub x)
	{
		for (Int j=0;j<n;j++)
			xt[j]=p[j]+x*xi[j];
		return funcd(xt);
	}
	Doub df(const Doub x)
	{
		Doub df1=0.0;
		funcd.df(xt,dft);
		for (Int j=0;j<n;j++)
			df1 += dft[j]*xi[j];
		return df1;
	}
};
template <class T>
struct Dlinemethod {
	VecDoub p;
	VecDoub xi;
	T &func;
	Int n;
	Dlinemethod(T &funcc) : func(funcc) {}
	Doub linmin()
	{
		Doub ax,xx,xmin;
		n=p.size();
		Df1dim<T> df1dim(p,xi,func);
		ax=0.0;
		xx=1.0;
		Dbrent dbrent;
		dbrent.bracket(ax,xx,df1dim);
		xmin=dbrent.minimize(df1dim);
		for (Int j=0;j<n;j++) {
			xi[j] *= xmin;
			p[j] += xi[j];
		}
		return dbrent.fmin;
	}
};
template <class T>
struct Powell : Linemethod<T> {
	Int iter;  // hold the number of iteration
	Doub fret; // hold the result of function optimized
	using Linemethod<T>::func;   // the function
	using Linemethod<T>::linmin; // the limin method
	using Linemethod<T>::p;		 // variables array
	using Linemethod<T>::xi;	 // matrix of directions	
	const Doub ftol;
	Powell(T &func, const Doub ftoll=3.0e-8) : Linemethod<T>(func),
		ftol(ftoll) {}
	VecDoub minimize(VecDoub_I &pp)
	{
		Int n=pp.size();
		MatDoub ximat(n,n,0.0);
		for (Int i=0;i<n;i++) ximat[i][i]=1.0;
		return minimize(pp,ximat);
	}
	VecDoub minimize(VecDoub_I &pp, MatDoub_IO &ximat)
	{
		const Int ITMAX=200;		// maximum allowed iterations
		const Doub TINY=1.0e-25;	// a small number
		Doub fptt;					// temporary value of function
		Int n=pp.size();
		p=pp;						// temporary values of variables
		VecDoub pt(n),ptt(n);		
		xi.resize(n);
		fret=func(p);
		for (Int j=0;j<n;j++) pt[j]=p[j];	// save the initial point
		for (iter=0;;++iter) {
			Doub fp=fret;
			Int ibig=0;
			Doub del=0.0;					// Will be the biggest function decrease
			for (Int i=0;i<n;i++) {			// In each iteration, loop over all direction in the set
				for (Int j=0;j<n;j++) xi[j]=ximat[j][i];	// Copy the direction to xi from the matrix of direction ximat
				fptt=fret;					
				fret=linmin();								// minimize function along the direction (just copied from ximat)
				if (fptt-fret > del) {						
					del=fptt-fret;							// update the largest decrease of the function	
					ibig=i+1;								// update the index of the direction for the biggest decrease of the function		
				}
			}
			if (2.0*(fp-fret) <= ftol*(abs(fp)+abs(fret))+TINY) { // check if the decrease rate is small -> end
				return p;
			}
			if (iter == ITMAX) throw("powell exceeding maximum iterations.");  
			for (Int j=0;j<n;j++) {							// construct extrapolated point 		
				ptt[j]=2.0*p[j]-pt[j];						// and the average direction to move		
				xi[j]=p[j]-pt[j];							// save the old starting point
				pt[j]=p[j];
			}
			fptt=func(ptt);
			if (fptt < fp) {
				Doub t=2.0*(fp-2.0*fret+fptt)*SQR(fp-fret-del)-del*SQR(fp-fptt);
				if (t < 0.0) {
					fret=linmin();
					for (Int j=0;j<n;j++) {
						ximat[j][ibig-1]=ximat[j][n-1];
						ximat[j][n-1]=xi[j];
					}
				}
			}
		}
	}
};

template <class T>
struct N2tPowell : Powell<T> {
	N2tPowell(T &func, const Doub ftoll=3.0e-8) : Powell<T>(func,ftoll){}

	VecDoub minimize(VecDoub_I &pp, bool mode)
	{
		Int n=pp.size();
		MatDoub ximat(n,n,0.0);
		for (Int i=0;i<n;i++) ximat[i][i]=1.0;		
		return minimize(pp,ximat,mode);			
	}
	VecDoub minimize(VecDoub_I &pp, MatDoub_IO &ximat, bool mode)
	{
		
		const Int ITMAX=100;		// maximum allowed iterations
		const Doub TINY=1.0e-25;	// a small number
		Doub fptt;					// temporary value of function
		Int n=pp.size();
		p=pp;						// temporary values of variables
		VecDoub pt(n),ptt(n);		// pt save the last iteration pt, ptt is just a temp point
		xi.resize(n);
		fret=func(p);				// calculate initial value of function;
		qDebug("Initial mutual information value = %f",fret);
		for (Int j=0;j<n;j++) pt[j]=p[j];	// save the initial point
		for (iter=0;;++iter) {
			if (mode){						// option 1 : reinitialize the direction ui
				for (Int i=0;i<n;i++){		
						
					for (Int j = 0; j<n; j++)
						ximat[j][i] = (j==i?1.0:0.0);
				}
			}
			qDebug("Iteration %d",iter);
			qDebug("Initial value tvec = (%f, %f, %f); rvec = (%f, %f, %f)\n", p[0],p[1],p[3],p[4],p[5],p[2]);
			Doub fp=fret;					// previous iteration result of function
			Int ibig=0;
			Doub del=0.0;					// Will be the biggest function decrease
			for (Int i=0;i<n;i++) {			// In each iteration, loop over all direction in the set
				for (Int j=0;j<n;j++) xi[j]=ximat[j][i];	// Copy the direction to xi from the matrix of direction ximat				
				fptt=fret;					
				fret=linmin();								// minimize function along the direction (just copied from ximat)
				if (fptt-fret > del) {						
					del=fptt-fret;							// update the largest decrease of the function	
					ibig=i+1;								// update the index of the direction for the biggest decrease of the function		
				}
				// For debug only

				switch(i)
				{
				case 0:					
					qDebug("After search along tx (%f, %f, %f); rvec = (%f, %f, %f) fret = %f", p[0],p[1],p[3],p[4],p[5],p[2],fret);					
					break;
				case 1:					
					qDebug("After search along ty (%f, %f, %f); rvec = (%f, %f, %f) fret = %f", p[0],p[1],p[3],p[4],p[5],p[2], fret);					
					break;
				case 2:					
					qDebug("After search along rz (%f, %f, %f); rvec = (%f, %f, %f), fret = %f", p[0],p[1],p[3],p[4],p[5],p[2], fret);					
					break;
				case 3:
					qDebug("After search along tz (%f, %f, %f); rvec = (%f, %f, %f) fret = %f", p[0],p[1],p[3],p[4],p[5],p[2], fret);
					break;
				case 4:
					qDebug("After search along rx (%f, %f, %f); rvec = (%f, %f, %f) fret = %f", p[0],p[1],p[3],p[4],p[5],p[2], fret);
					break;
				case 5:
					qDebug("After search along ry (%f, %f, %f); rvec = (%f, %f, %f) fret = %f", p[0],p[1],p[3],p[4],p[5],p[2], fret);
					break;
				default:
					break;
				}
				// End for debug only
			}
			if (2.0*(fp-fret) <= ftol*(abs(fp)+abs(fret))+TINY) { // check if the decrease rate is small -> end
				qDebug()<<"Optimization done!\n";
				return p;
			}
			if (iter == ITMAX) throw("powell exceeding maximum iterations.");  
			
			
			for (Int j=0;j<n;j++) {							// construct extrapolated point 		
				ptt[j]=2.0*p[j]-pt[j];						// and the average direction to move		
				xi[j]=p[j]-pt[j];							// save the old starting point
				pt[j]=p[j];									// remember that p is changed in limin function not here
			}
			
			//printf("iteration %5dth; p   = [%f,%f];pt  = [%f,%f]; ptt = [%f,%f]\n",iter,p[0],p[1],pt[0],pt[1],ptt[0],ptt[1]);

			fptt=func(ptt);									// Move to the minimum of the new direction
			if (fptt < fp ) {								// and save the new direction
																// alway save current direction if mode = true
				Doub t=2.0*(fp-2.0*fret+fptt)*SQR(fp-fret-del)-del*SQR(fp-fptt);
				if (t < 0.0) {								// check if function is decrease
					fret=linmin();							
					if (!mode)
						for (Int j=0;j<n;j++) {
							ximat[j][ibig-1]=ximat[j][n-1];
							ximat[j][n-1]=xi[j];
						}
				}
			}
			
		}
	}
	
};
template <class T>
struct Frprmn : Linemethod<T> {
	Int iter;
	Doub fret;
	using Linemethod<T>::func;
	using Linemethod<T>::linmin;
	using Linemethod<T>::p;
	using Linemethod<T>::xi;
	const Doub ftol;
	Frprmn(T &funcd, const Doub ftoll=3.0e-8) : Linemethod<T>(funcd),
		ftol(ftoll) {}
	VecDoub minimize(VecDoub_I &pp)
	{
		const Int ITMAX=200;
		const Doub EPS=1.0e-18;
		const Doub GTOL=1.0e-8;
		Doub gg,dgg;
		Int n=pp.size();
		p=pp;
		VecDoub g(n),h(n);
		xi.resize(n);
		Doub fp=func(p);
		func.df(p,xi);
		for (Int j=0;j<n;j++) {
			g[j] = -xi[j];
			xi[j]=h[j]=g[j];
		}
		for (Int its=0;its<ITMAX;its++) {
			iter=its;
			fret=linmin();
			if (2.0*abs(fret-fp) <= ftol*(abs(fret)+abs(fp)+EPS))
				return p;
			fp=fret;
			func.df(p,xi);
			Doub test=0.0;
			Doub den=MAX(fp,1.0);
			for (Int j=0;j<n;j++) {
				Doub temp=abs(xi[j])*MAX(abs(p[j]),1.0)/den;
				if (temp > test) test=temp;
			}
			if (test < GTOL) return p;
			dgg=gg=0.0;
			for (Int j=0;j<n;j++) {
				gg += g[j]*g[j];
//			  dgg += xi[j]*xi[j];
				dgg += (xi[j]+g[j])*xi[j];
			}
			if (gg == 0.0)
				return p;
			Doub gam=dgg/gg;
			for (Int j=0;j<n;j++) {
				g[j] = -xi[j];
				xi[j]=h[j]=g[j]+gam*h[j];
			}
		}
		throw("Too many iterations in frprmn");
	}
};
