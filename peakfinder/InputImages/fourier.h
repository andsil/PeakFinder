//REF: https://github.com/ginrou/ginrou-private/blob/1542f56865b55f10b1c17dbd825b859be20b5a67/src/complex.c
#ifndef FOURIER_H
#define	FOURIER_H

#include "complex.h"

#ifdef	__cplusplus
extern "C" {
#endif

#define FFT_SIZE 64
    
void fourier(Complex out[][FFT_SIZE],  double in[][FFT_SIZE] );
void inverseFourier( double out[][FFT_SIZE], Complex in[][FFT_SIZE]);
void fourierSpectrumImage(double out[][FFT_SIZE], Complex in[][FFT_SIZE]);

void fourier1D(Complex out[], double in[]);
void inverseFourier1D(double out[], Complex in[]);


#ifdef	__cplusplus
}
#endif

#endif	/* FOURIER_H */

