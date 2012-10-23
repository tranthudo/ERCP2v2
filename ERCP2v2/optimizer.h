#include <opencv2/opencv.hpp>
#include <glm/glm.hpp>
#include <fstream>
#include <cmath>
#include <complex>
#include <iostream>
#include <iomanip>
#include <vector>
#include <limits>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>

using namespace std;
// basic type names (redefine if your bit lengths don't match)

typedef int Int; // 32 bit integer
typedef unsigned int Uint;

#ifdef _MSC_VER
typedef __int64 Llong; // 64 bit integer
typedef unsigned __int64 Ullong;
#else
typedef long long int Llong; // 64 bit integer
typedef unsigned long long int Ullong;
#endif

typedef char Char; // 8 bit integer
typedef unsigned char Uchar;

typedef double Doub; // default floating type
typedef long double Ldoub;

typedef complex<double> Complex; // default complex type

typedef bool Bool;

template<class T>
inline T SQR(const T a) {return a*a;}

template<class T>
inline const T &MAX(const T &a, const T &b)
{return b > a ? (b) : (a);}
//
//inline float MAX(const double &a, const float &b)
//{return b > a ? (b) : float(a);}
//
//inline float MAX(const float &a, const double &b)
//{return b > a ? float(b) : (a);}
//
//template<class T>
//inline const T &MIN(const T &a, const T &b)
//{return b < a ? (b) : (a);}
//
//inline float MIN(const double &a, const float &b)
//{return b < a ? (b) : float(a);}
//
//inline float MIN(const float &a, const double &b)
//{return b < a ? float(b) : (a);}

//template<class T>
//inline T SIGN(const T &a, const T &b)
//{return b >= 0 ? (a >= 0 ? a : -a) : (a >= 0 ? -a : a);}
//
//inline float SIGN(const float &a, const double &b)
//{return b >= 0 ? (a >= 0 ? a : -a) : (a >= 0 ? -a : a);}
//
//inline float SIGN(const double &a, const float &b)
//{return (float)(b >= 0 ? (a >= 0 ? a : -a) : (a >= 0 ? -a : a));}
//

//template<class T>
//inline void SWAP(T &a, T &b)
//{T dum=a; a=b; b=dum;}
//
//template<class T>
//inline void helloWorld(T &a)
//{
//	printf("Hello %f",a);
//}

// exception handling

//#ifdef _USESTDVECTOR_
//#define NRvector vector
//#else

template <class T>
class NRvector {
private:
	int nn;	// size of array. upper index is nn-1
	T *v;
public:
	NRvector();
	explicit NRvector(int n);		// Zero-based array
	NRvector(int n, const T &a);	//initialize to constant value
	NRvector(int n, const T *a);	// Initialize to array
	NRvector(const NRvector &rhs);	// Copy constructor
	NRvector & operator=(const NRvector &rhs);	//assignment
	typedef T value_type; // make T available externally
	inline T & operator[](const int i);	//i'th element
	inline const T & operator[](const int i) const;
	inline int size() const;
	void resize(int newn); // resize (contents not preserved)
	void assign(int newn, const T &a); // resize and assign a constant value
	~NRvector();
};

// NRvector definitions

template <class T>
NRvector<T>::NRvector() : nn(0), v(NULL) {}

template <class T>
NRvector<T>::NRvector(int n) : nn(n), v(n>0 ? new T[n] : NULL) {}

template <class T>
NRvector<T>::NRvector(int n, const T& a) : nn(n), v(n>0 ? new T[n] : NULL)
{
	for(int i=0; i<n; i++) v[i] = a;
}

template <class T>
NRvector<T>::NRvector(int n, const T *a) : nn(n), v(n>0 ? new T[n] : NULL)
{
	for(int i=0; i<n; i++) v[i] = *a++;
}

template <class T>
NRvector<T>::NRvector(const NRvector<T> &rhs) : nn(rhs.nn), v(nn>0 ? new T[nn] : NULL)
{
	for(int i=0; i<nn; i++) v[i] = rhs[i];
}

template <class T>
NRvector<T> & NRvector<T>::operator=(const NRvector<T> &rhs)
	// postcondition: normal assignment via copying has been performed;
	//		if vector and rhs were different sizes, vector
	//		has been resized to match the size of rhs
{
	if (this != &rhs)
	{
		if (nn != rhs.nn) {
			if (v != NULL) delete [] (v);
			nn=rhs.nn;
			v= nn>0 ? new T[nn] : NULL;
		}
		for (int i=0; i<nn; i++)
			v[i]=rhs[i];
	}
	return *this;
}

template <class T>
inline T & NRvector<T>::operator[](const int i)	//subscripting
{
#ifdef _CHECKBOUNDS_
	if (i<0 || i>=nn) {
		throw("NRvector subscript out of bounds");
	}
#endif
	return v[i];
}

template <class T>
inline const T & NRvector<T>::operator[](const int i) const	//subscripting
{
#ifdef _CHECKBOUNDS_
	if (i<0 || i>=nn) {
		throw("NRvector subscript out of bounds");
	}
#endif
	return v[i];
}

template <class T>
inline int NRvector<T>::size() const
{
	return nn;
}

template <class T>
void NRvector<T>::resize(int newn)
{
	if (newn != nn) {
		if (v != NULL) delete[] (v);
		nn = newn;
		v = nn > 0 ? new T[nn] : NULL;
	}
}

template <class T>
void NRvector<T>::assign(int newn, const T& a)
{
	if (newn != nn) {
		if (v != NULL) delete[] (v);
		nn = newn;
		v = nn > 0 ? new T[nn] : NULL;
	}
	for (int i=0;i<nn;i++) v[i] = a;
}

template <class T>
NRvector<T>::~NRvector()
{
	if (v != NULL) delete[] (v);
}

// end of NRvector definitions

//#endif //ifdef _USESTDVECTOR_

// vector types

typedef const NRvector<Int> VecInt_I;
typedef NRvector<Int> VecInt, VecInt_O, VecInt_IO;

typedef const NRvector<Uint> VecUint_I;
typedef NRvector<Uint> VecUint, VecUint_O, VecUint_IO;

typedef const NRvector<Llong> VecLlong_I;
typedef NRvector<Llong> VecLlong, VecLlong_O, VecLlong_IO;

typedef const NRvector<Ullong> VecUllong_I;
typedef NRvector<Ullong> VecUllong, VecUllong_O, VecUllong_IO;

typedef const NRvector<Char> VecChar_I;
typedef NRvector<Char> VecChar, VecChar_O, VecChar_IO;

typedef const NRvector<Char*> VecCharp_I;
typedef NRvector<Char*> VecCharp, VecCharp_O, VecCharp_IO;

typedef const NRvector<Uchar> VecUchar_I;
typedef NRvector<Uchar> VecUchar, VecUchar_O, VecUchar_IO;

typedef const NRvector<Doub> VecDoub_I;
typedef NRvector<Doub> VecDoub, VecDoub_O, VecDoub_IO;

typedef const NRvector<Doub*> VecDoubp_I;
typedef NRvector<Doub*> VecDoubp, VecDoubp_O, VecDoubp_IO;

typedef const NRvector<Complex> VecComplex_I;
typedef NRvector<Complex> VecComplex, VecComplex_O, VecComplex_IO;

typedef const NRvector<Bool> VecBool_I;
typedef NRvector<Bool> VecBool, VecBool_O, VecBool_IO;




struct Bracketmethod {
	double ax,bx,cx,fa,fb,fc;
	template <class T>
	void bracket(const double a, const double b, T &func)
	{
		const double GOLD=1.618034,GLIMIT=100.0,TINY=1.0e-20;
		ax=a; bx=b;
		double fu;
		fa=func(ax);
		fb=func(bx);
		if (fb > fa) {
			SWAP(ax,bx);
			SWAP(fb,fa);
		}
		cx=bx+GOLD*(bx-ax);
		fc=func(cx);
		while (fb > fc) {
			double r=(bx-ax)*(fb-fc);
			double q=(bx-cx)*(fb-fa);
			double u=bx-((bx-cx)*q-(bx-ax)*r)/
				(2.0*SIGN(MAX(abs(q-r),TINY),q-r));
			double ulim=bx+GLIMIT*(cx-bx);
			if ((bx-u)*(u-cx) > 0.0) {
				fu=func(u);
				if (fu < fc) {
					ax=bx;
					bx=u;
					fa=fb;
					fb=fu;
					return;
				} else if (fu > fb) {
					cx=u;
					fc=fu;
					return;
				}
				u=cx+GOLD*(cx-bx);
				fu=func(u);
			} else if ((cx-u)*(u-ulim) > 0.0) {
				fu=func(u);
				if (fu < fc) {
					shft3(bx,cx,u,u+GOLD*(u-cx));
					shft3(fb,fc,fu,func(u));
				}
			} else if ((u-ulim)*(ulim-cx) >= 0.0) {
				u=ulim;
				fu=func(u);
			} else {
				u=cx+GOLD*(cx-bx);
				fu=func(u);
			}
			shft3(ax,bx,cx,u);
			shft3(fa,fb,fc,fu);
		}
	}
	inline void shft2(double &a, double &b, const double c)
	{
		a=b;
		b=c;
	}
	inline void shft3(double &a, double &b, double &c, const double d)
	{
		a=b;
		b=c;
		c=d;
	}
	inline void mov3(double &a, double &b, double &c, const double d, const double e,
		const double f)
	{
		a=d; b=e; c=f;
	}
};

struct Brent : Bracketmethod {
	double xmin,fmin;
	const double tol;
	Brent(const double toll=3.0e-8) : tol(toll) {}
	template <class T>
	double minimize(T &func)
	{
		const Int ITMAX=100;
		const double CGOLD=0.3819660;
		const double ZEPS=numeric_limits<double>::epsilon()*1.0e-3;
		double a,b,d=0.0,etemp,fu,fv,fw,fx;
		double p,q,r,tol1,tol2,u,v,w,x,xm;
		double e=0.0;

		a=(ax < cx ? ax : cx);
		b=(ax > cx ? ax : cx);
		x=w=v=bx;
		fw=fv=fx=func(x);
		for (Int iter=0;iter<ITMAX;iter++) {
			xm=0.5*(a+b);
			tol2=2.0*(tol1=tol*abs(x)+ZEPS);
			if (abs(x-xm) <= (tol2-0.5*(b-a))) {
				fmin=fx;
				return xmin=x;
			}
			if (abs(e) > tol1) {
				r=(x-w)*(fx-fv);
				q=(x-v)*(fx-fw);
				p=(x-v)*q-(x-w)*r;
				q=2.0*(q-r);
				if (q > 0.0) p = -p;
				q=abs(q);
				etemp=e;
				e=d;
				if (abs(p) >= abs(0.5*q*etemp) || p <= q*(a-x)
					|| p >= q*(b-x))
					d=CGOLD*(e=(x >= xm ? a-x : b-x));
				else {
					d=p/q;
					u=x+d;
					if (u-a < tol2 || b-u < tol2)
						d=SIGN(tol1,xm-x);
				}
			} else {
				d=CGOLD*(e=(x >= xm ? a-x : b-x));
			}
			u=(abs(d) >= tol1 ? x+d : x+SIGN(tol1,d));
			fu=func(u);
			if (fu <= fx) {
				if (u >= x) a=x; else b=x;
				shft3(v,w,x,u);
				shft3(fv,fw,fx,fu);
			} else {
				if (u < x) a=u; else b=u;
				if (fu <= fw || w == x) {
					v=w;
					w=u;
					fv=fw;
					fw=fu;
				} else if (fu <= fv || v == x || v == w) {
					v=u;
					fv=fu;
				}
			}
		}
		throw("Too many iterations in brent");
	}
};

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
		ax=0.0;
		xx=1.0;
		Brent brent;
		brent.bracket(ax,xx,f1dim);
		xmin=brent.minimize(f1dim);
		for (Int j=0;j<n;j++) {
			xi[j] *= xmin;
			p[j] += xi[j];
		}
		return brent.fmin;
	}
};