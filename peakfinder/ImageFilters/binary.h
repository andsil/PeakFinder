#ifndef BINARY_H
#define	BINARY_H

#include <tiff.h>//uint8
#include <math.h>//exp,...

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
 * Given an image and the thresh level rewrites the image values
 * to 0 (black) and 1 (white) if below or above the target
 * intensity level, respectively.
 */
TiffImage binImage8bitStatic(TiffImage img, int threshold);

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
 * Construct a binary image. If the pixel is below trgLevel its
 * black(0), otherwise is white(1)
 */
TiffImage binImage8bit(TiffImage img, uint8 trgLevel);

/**
 * Morphologic Operator - Dilatation
 */
TiffImage binary_dilation(TiffImage img);

/**
 * Morphologic Operator - Erasion
 */
TiffImage binary_erosion(TiffImage img);

/**
 * Morphologic Operator - Opening
 */
TiffImage binary_opening(TiffImage img);

/**
 * Morphologic Operator - Closing
 */
TiffImage binary_closing(TiffImage img);

/**
 * Gets the mean of the surrounding pixels
 */
TiffImage binary_meanFilter(TiffImage img);

/**
 * Gets the median of the surrounding pixels
 */
TiffImage binary_medianFilter(TiffImage img);

/**
 * Sigma filter
 */
TiffImage binary_sigmaFilter(TiffImage img, int sigma);

/**
 * gaussian filter
 * sigma=1
 */
TiffImage binary_gaussianFilter(TiffImage img);

// reflected indexing for border processing
int reflect(int M, int x);
/*****************************************************************
########################  END PROTOTYPES    ######################
 *****************************************************************/
#ifdef	__cplusplus
}
#endif

#endif	/* BINARY_H */

