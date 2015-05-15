//REF: https://github.com/ginrou/ginrou-private/blob/1542f56865b55f10b1c17dbd825b859be20b5a67/src/
#ifndef COMPLEX_H
#define	COMPLEX_H

#include <stdio.h>
#include <math.h>
#include <fftw3.h>

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct {
  double Re;
  double Im;
} Complex;

Complex compAdd(Complex a, Complex b);
Complex compSub(Complex a, Complex b);
Complex compMul(Complex a, Complex b);
Complex compDiv(Complex a, Complex b);
double compAbs(Complex a);
double compAbsFFTW(fftw_complex a);
double compAbs2(Complex a);
void compDisp(Complex a);

//FFTW3 (temporary)
void fftw_compAdd(fftw_complex a, fftw_complex b, fftw_complex r);
void fftw_compSub(fftw_complex a, fftw_complex b, fftw_complex r);
void fftw_compMul(fftw_complex a, fftw_complex b, fftw_complex r);
void fftw_compDiv(fftw_complex a, fftw_complex b, fftw_complex r);
double fftw_compAbs(fftw_complex a);
double fftw_compAbs2(fftw_complex a);
void fftw_compDisp(fftw_complex a);

#ifdef	__cplusplus
}
#endif

#endif	/* COMPLEX_H */

