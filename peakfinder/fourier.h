//REF: https://github.com/ginrou/ginrou-private/blob/1542f56865b55f10b1c17dbd825b859be20b5a67/src/complex.c
#ifndef FOURIER_H
#define	FOURIER_H

#include <tiff.h> //uint8
#include "complex.h"

#ifdef	__cplusplus
extern "C" {
#endif

#define FFT_SIZE 1024
    
void fourier(Complex** out,  uint8** in );
void inverseFourier( double out[][FFT_SIZE], Complex in[][FFT_SIZE]);
void fourierSpectrumImage(uint8** out, Complex** in);

void fourier1D(Complex out[], double in[]);
void inverseFourier1D(double out[], Complex in[]);


#ifdef	__cplusplus
}
#endif

#endif	/* FOURIER_H */

