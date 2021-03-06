#ifndef WRITETIFF_H
#define	WRITETIFF_H

#include <stdlib.h>//fprintf
#include <tiffio.h>//libtiff
#include <unistd.h>//usleep

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
 * Write Tiff image into TiffSampleImage structure
 */
int writeTiffImage(char* outputFileName, TiffImage img);

/*****************************************************************
########################  END PROTOTYPES    ######################
*****************************************************************/

#ifdef	__cplusplus
}
#endif

#endif	/* WRITETIFF_H */

