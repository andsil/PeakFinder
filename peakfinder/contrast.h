#ifndef CONTRAST_H
#define	CONTRAST_H

#include <stdio.h>//fprintf
#include <stdlib.h>//malloc
#include <math.h>//exp2, round
#include <string.h>//strcat, strlen, etc
//#include <unistd.h>//usleep

#include "auxFunc.h"
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

/*****************************************************************
########################  END PROTOTYPES    ######################
*****************************************************************/



#ifdef	__cplusplus
}
#endif

#endif	/* CONTRAST_H */

