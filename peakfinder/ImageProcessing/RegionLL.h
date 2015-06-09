#ifndef REGIONLL_H
#define	REGIONLL_H

#include <stdio.h>  //fprintf
#include <stdlib.h> //NULL

#include "tiff.h"   //uint8
#include "PointLL.h"//PointLL

#ifdef	__cplusplus
extern "C" {
#endif

/*****************************************************************
########################  DATA STRUCTURES #######################
 *****************************************************************/
/**
 * Region - Includes a list of Points and some characteristics
 * of the Points included.
 */
typedef struct sRegion{
    int        id;         //Region ID
    float      coordXBeg;  //Minimum X coordinate of this region
    float      coordYBeg;  //Minimum Y coordinate of this region
    float      coordXEnd;  //Maximum X coordinate of this region
    float      coordYEnd;  //Maximum Y coordinate of this region
    uint8      minValue;   //Minimum Intensity value of this region
    uint8      maxValue;   //Maximum Intensity value of this region
    int        pointCount; //Number of points in this Region
    PointCoord centroid;   //Centroid of the region
    PointLL    pointList;  //Pointer to first Point of the list
}Region;

/**
 * List of Regions
 */
typedef struct sRegionLL{
    int lenght;                     //Current occupied indexes
    int size;                       //Allocated array size
    Region *regions;                //Array of regions
}*RegionLL;

/*****************************************************************
######################  END DATA STRUCTURES #####################
 *****************************************************************/

/*****************************************************************
########################    PROTOTYPES    #######################
 *****************************************************************/

/*############## REGIONLL ##############*/

/** CONSTRUCTOR **/

/**
 * Creates new list of Regions
 */
RegionLL createNewRegionLL(Region *region);

/**
 * Reallocates the Regions array to double the size
 */
RegionLL realocRegionLL(RegionLL list);

/** INSERT **/

/**
 * Add a Region to a list of Regions
 */
RegionLL addRegionLLEntry(RegionLL list, Region *addRegion);

/** REMOVE **/

/**
 * Removes a Region identified by its ID from a list of Regions
 */
RegionLL remRegionLLEntry(RegionLL entry, int remId);

/**
 * Removes all Regions
 */
void remAllRegionLL(RegionLL list);

/** GETS **/

/**
 * Count how many region the list have
 */
int regionCount(RegionLL list);

/*############## REGION ##############*/

/** CONSTRUCTOR **/

/**
 * Create a new Region from a List of Points
 * NOTE: Borders regions (within 5 pixels) are deleted!
 */
Region* createNewRegion(PointLL pointList, uint32 width, uint32 height);

/** REMOVE **/

/**
 * Removes the indicated Region (free memory)
 */
void remRegion(Region *region);

/** COPY **/

/**
 * Do a shadow copy
 */
void cloneRegion(Region *src, Region *dst);

/*****************************************************************
########################  END PROTOTYPES    ######################
 *****************************************************************/


#ifdef	__cplusplus
}
#endif

#endif	/* REGIONLL_H */

