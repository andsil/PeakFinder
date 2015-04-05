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
}*Region;

/**
 * List of Regions - Includes a pointer to some Region and
 * two pointers to the following a previous entry of the list.
 */
typedef struct sRegionLL{
    int id;             //Region ID
    Region region;      //Pointer to Region
    struct sRegionLL*   nextRegion;  //Pointer to next Region
    struct sRegionLL*   prevRegion;  //Pointer to previous Region
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
RegionLL createNewRegionLL(Region region);

/** INSERT **/

/**
 * Add a Region to a list of Regions
 */
RegionLL addRegionLLEntry(RegionLL list, Region addRegion);

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
 * Get a Region from a list of Regions by its ID
 */
RegionLL getRegionEntry(RegionLL list, int regionID);

/**
 * Get the last Region from the list
 */
RegionLL getLastRegionEntry(RegionLL list);

/**
 * Get the first Region from the list
 */
RegionLL getFirstRegionEntry(RegionLL list);

/** OTHER **/

/**
 * Updates the ID of the Regions following the pointed Region by the
 * specified increment value
 */
void updateIDNextRegionLL(RegionLL region, int increment);

/**
 * Count how many region the list have
 */
int regionCount(RegionLL list);

/*############## REGION ##############*/

/** CONSTRUCTOR **/

/**
 * Create a new Region from a List of Points
 */
Region createNewRegion(PointLL pointList);

/** REMOVE **/

/**
 * Removes the indicated Region (free memory)
 */
void remRegion(Region region);

/*****************************************************************
########################  END PROTOTYPES    ######################
 *****************************************************************/


#ifdef	__cplusplus
}
#endif

#endif	/* REGIONLL_H */

