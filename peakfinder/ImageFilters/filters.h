#ifndef FILTERS_H
#define	FILTERS_H

#include <tiff.h>//uint8
#include <math.h>//exp,...

#include "../TiffImage/tiffFile.h"//TiffImage

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
 * Gets the mean of the surrounding pixels
 */
TiffImage meanFilter(TiffImage img);

/**
 * Gets the median of the surrounding pixels
 */
TiffImage medianFilter(TiffImage img);

/**
 * Sigma filter
 */
TiffImage sigmaFilter(TiffImage img, int sigma);

/**
 * gaussian filter
 * sigma=1
 */
TiffImage gaussianFilter(TiffImage img);

// reflected indexing for border processing
int reflect(int M, int x);
/*****************************************************************
########################  END PROTOTYPES    ######################
 *****************************************************************/
#ifdef	__cplusplus
}
#endif

#endif	/* FILTERS_H */

