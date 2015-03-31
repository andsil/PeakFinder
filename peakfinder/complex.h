//REF: https://github.com/ginrou/ginrou-private/blob/1542f56865b55f10b1c17dbd825b859be20b5a67/src/
#ifndef COMPLEX_H
#define	COMPLEX_H

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
double compAbs2(Complex a);
void compDisp(Complex a);


#ifdef	__cplusplus
}
#endif

#endif	/* COMPLEX_H */

