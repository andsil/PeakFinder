#ifndef TRANSFORMATIONS_H
#define	TRANSFORMATIONS_H

#include <tiff.h>//uint8

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
 * Morphologic Operator - Dilatation
 */
TiffImage dilation(TiffImage img);

/**
 * Morphologic Operator - Erasion
 */
TiffImage erosion(TiffImage img);

/**
 * Morphologic Operator - Opening
 */
TiffImage opening(TiffImage img);

/**
 * Morphologic Operator - Closing
 */
TiffImage closing(TiffImage img);

/*****************************************************************
########################  END PROTOTYPES    ######################
 *****************************************************************/
#ifdef	__cplusplus
}
#endif

#endif	/* TRANSFORMATIONS_H */

