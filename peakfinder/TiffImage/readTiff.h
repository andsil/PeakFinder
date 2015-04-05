#ifndef READTIFF_H
#define	READTIFF_H

#include <stdio.h>//printf
#include <stdlib.h>//malloc, calloc, etc
#include <tiffio.h>//libtiff
#include <math.h>//exp2
#include <string.h>//strcpy

#include "tiffFile.h"

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
 * Read Tiff image into TiffSampleImage structure
 */
TiffImage readTiffImage(char* inputFileName);

/*****************************************************************
########################  END PROTOTYPES    ######################
*****************************************************************/

#ifdef	__cplusplus
}
#endif

#endif	/* READTIFF_H */

