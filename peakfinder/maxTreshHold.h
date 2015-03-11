#ifndef MAXTRESHLEVEL_H
#define	MAXTRESHLEVEL_H

#include <stdio.h>  //fprintf
#include <tiff.h>   //uint8

#include "auxFunc.h" //isInside
#include "tiffFile.h"
#include "RegionLL.h" //RegionLL
#include "binary.h" //binImage8bitStatic

#ifdef	__cplusplus
extern "C" {
#endif

/*****************************************************************
########################  DATA STRUCTURES #######################
 *****************************************************************/

//Useful definitions
//QUEUE STATUS
#define NOTVISITED  0
#define QUEUED      1
#define VISITED     2

//COLOR VALUES
#define WHITE       255
#define BLACK       0
    
/*****************************************************************
######################  END DATA STRUCTURES #####################
 *****************************************************************/

/*****************************************************************
########################    PROTOTYPES    #######################
 *****************************************************************/
/**
 * Go through the image pixels and aggregates them in Regions
 */
RegionLL findRegions(TiffImage img);

/**
 * Checks if the point (j,i) -> (x,y) is white. If it is add it to the queue.
 * Returns the new stack pointer if there were changes.  is.
 */
int addQueue(uint8** img, char** mark, PointCoord* queueStack, int sp, int i, int j);

/**
 * VERY VERY Heavy calculations!!! Should be avoided!
 * Lower start ThreshHold lower performance -> higher calculation times
 */
TiffImage imageBinarization(TiffImage img, float startThreshHold, int maxIt);

/*****************************************************************
########################  END PROTOTYPES    ######################
 *****************************************************************/


#ifdef	__cplusplus
}
#endif

#endif	/* MAXTRESHLEVEL_H */

