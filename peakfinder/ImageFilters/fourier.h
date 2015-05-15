//REF: https://github.com/ginrou/ginrou-private/blob/1542f56865b55f10b1c17dbd825b859be20b5a67/src/complex.c
#ifndef FOURIER_H
#define	FOURIER_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>//pow ...
#include <unistd.h>//usleep
#include <tiff.h> //uint8
#include <fftw3.h>//fftw_complex
#include "../Auxiliary/complex.h"

#ifdef	__cplusplus
extern "C" {
#endif
    
void fourier(Complex** out, uint8** in, int FFT_SIZE);
void fourierFFTW(fftw_complex* out, uint8** in, int FFT_SIZE);
void inverseFourier(uint8** out, Complex** in, int FFT_SIZE);
void inverseFourierFFTW(uint8** out, fftw_complex* in, int FFT_SIZE);
void fourierSpectrumImage(uint8** out, Complex** in, int FFT_SIZE);
int  fourierSpectrumImageFFTW(uint8** out, fftw_complex* in, int FFT_SIZE);

int max_exp_value(fftw_complex* in, int FFT_SIZE);
void fourier1D(Complex out[], double in[], int FFT_SIZE);
void inverseFourier1D(double out[], Complex in[], int FFT_SIZE);


#ifdef	__cplusplus
}
#endif

#endif	/* FOURIER_H */

