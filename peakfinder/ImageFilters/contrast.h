#ifndef CONTRAST_H
#define	CONTRAST_H

#include <stdio.h>//fprintf
#include <stdlib.h>//malloc
#include <math.h>//exp2, round
#include <string.h>//strcat, strlen, etc
#include <omp.h>//pragma omp
#include <tiff.h>//uint8

#include "../Auxiliary/auxFunc.h"
#include "../TiffImage/tiffFile.h"//TiffImage

#ifdef	__cplusplus
extern "C" {
#endif

/*****************************************************************
########################  DATA STRUCTURES #######################
*****************************************************************/
    
#define BLACK 0
#define WHITE 255
    
/*****************************************************************
######################  END DATA STRUCTURES #####################
*****************************************************************/
/*****************************************************************
########################    PROTOTYPES    #######################
*****************************************************************/
/**
 * Return a normalized image with increased contrast.
 */
TiffImage histogramEqualization(TiffImage img);

/**
 * Casts a signed integer to unsigned integer of 8bits.
 */
uint8 saturate_cast_uint8(int x);

/**
 * Generate cumulative frequency histogram.
 */
void cumhist(int histogram[], int cumhistogram[]);


/* REF: https://github.com/MPS-UPB/10Team/blob/3846a66e28a956c9bb8f784a6851b3fb400d4627/BAM1/binarization.cpp
 * calculate a global threshold for the image using Otsu algorithm
 * params
 * @histData: histogram of the image
 * @y0, y1: Oy coordinates of the image
 * @x0, x1: Ox coordinates of the image
 * @return: global threshold for the image
*/
int getOtsuThreshold(int *histData, int y0, int y1, int x0, int x1);

/**
 * Construct a binary image. If the pixel is below threshold its
 * black(0), otherwise is white(1)
 */
TiffImage binImage8bit(TiffImage img, uint8 threshold);

/*****************************************************************
########################  END PROTOTYPES    ######################
*****************************************************************/

#ifdef	__cplusplus
}
#endif

#endif	/* CONTRAST_H */