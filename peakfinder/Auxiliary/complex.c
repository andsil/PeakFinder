//REF: https://github.com/ginrou/ginrou-private/blob/1542f56865b55f10b1c17dbd825b859be20b5a67/src/
#include "complex.h"

#define REAL    0
#define IMG     1

Complex compAdd(Complex a, Complex b) {
  Complex r;

  r.Re = a.Re + b.Re;
  r.Im = a.Im + b.Im;

  return r;
}

Complex compSub(Complex a, Complex b) {
  Complex r;

  r.Re = a.Re - b.Re;
  r.Im = a.Im - b.Im;

  return r;
}

Complex compMul(Complex a, Complex b) {
  Complex r;

  r.Re = a.Re * b.Re - a.Im * b.Im;
  r.Im = a.Im * b.Re + a.Re * b.Im;

  return r;
}

Complex compDiv(Complex a, Complex b) {
  double x;
  Complex r;

  x = b.Re * b.Re + b.Im * b.Im;
  r.Re = (a.Re * b.Re + a.Im * b.Im) / x;
  r.Im = (a.Im * b.Re - a.Re * b.Im) / x;

  return r;
}

double compAbs(Complex a) {
  return sqrt(a.Re * a.Re + a.Im * a.Im);
}

double compAbs2(Complex a) {
  return a.Re * a.Re + a.Im * a.Im;
}

void compDisp(Complex a) {
  if(a.Im >= 0) {
    printf("%lf+%lfi", a.Re, a.Im);
  }
  else {
    printf("%lf%lfi", a.Re, a.Im);
  }
}

////////////////////////////////////////////

void fftw_compAdd(fftw_complex a, fftw_complex b, fftw_complex r) {

  r[REAL] = a[REAL] + b[REAL];
  r[IMG] = a[IMG] + b[IMG];
}

void fftw_compSub(fftw_complex a, fftw_complex b, fftw_complex r) {

  r[REAL] = a[REAL] - b[REAL];
  r[IMG] = a[IMG] - b[IMG];

}

void fftw_compMul(fftw_complex a, fftw_complex b, fftw_complex r) {

  r[REAL] = a[REAL] * b[REAL] - a[IMG] * b[IMG];
  r[IMG] = a[IMG] * b[REAL] + a[REAL] * b[IMG];

}

void fftw_compDiv(fftw_complex a, fftw_complex b, fftw_complex r) {
  double x;

  x = b[REAL] * b[REAL] + b[IMG] * b[IMG];
  r[REAL] = (a[REAL] * b[REAL] + a[IMG] * b[IMG]) / x;
  r[IMG] = (a[IMG] * b[REAL] - a[REAL] * b[IMG]) / x;

}

double fftw_compAbs(fftw_complex a) {
  return sqrt(a[REAL] * a[REAL] + a[IMG] * a[IMG]);
}

double fftw_compAbs2(fftw_complex a) {
  return a[REAL] * a[REAL] + a[IMG] * a[IMG];
}

void fftw_compDisp(fftw_complex a) {
  if(a[IMG] >= 0) {
    printf("%lf+%lfi", a[REAL], a[IMG]);
  }
  else {
    printf("%lf%lfi", a[REAL], a[IMG]);
  }
}