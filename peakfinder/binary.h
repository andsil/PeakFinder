#ifndef BINARY_H
#define	BINARY_H

#include <tiff.h>//uint8

#include "tiffFile.h"//TiffImage

#ifdef	__cplusplus
extern "C" {
#endif
/*****************************************************************
########################  DATA STRUCTURES #######################
 *****************************************************************/
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
TiffImage binImage8bitStatic(TiffImage img, float threshlevel);

/**
 * Construct a binary image separating pixels at 127
 */
TiffImage binImage8bitStaticHalf(TiffImage img);

/**
 * Construct a binary image with the median intensity value
 */
TiffImage binImage8bitAutoMedian(TiffImage img);

/**
 * Construct a binary image with the average intensity value
 */
TiffImage binImage8bitAutoAverage(TiffImage img);

/**
 * Construct a binary image with the thresh level value applied to
 * the range of variation (maximum - minimum)
 */
TiffImage binImage8bitDynamic(TiffImage img, float threshlevel);

/**
 * Construct a binary image separating pixels at (max-min)/2
 */
TiffImage binImage8bitDynamicHalf(TiffImage img);

/**
 * Construct a binary image. If the pixel is below trgLevel its
 * black(0), otherwise is white(1)
 */
TiffImage binImage8bit(TiffImage img, uint8 trgLevel);
/*****************************************************************
########################  END PROTOTYPES    ######################
 *****************************************************************/
#ifdef	__cplusplus
}
#endif

#endif	/* BINARY_H */

