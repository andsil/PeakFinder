#ifndef POINTLL_H
#define	POINTLL_H

#include <stdio.h>  //fprintf
#include <stdlib.h> //NULL
#include <float.h>  //FLT_MAX

#include "tiff.h"   //uint8
#include "../Auxiliary/auxFunc.h" //isInside

#ifdef	__cplusplus
extern "C" {
#endif

/*****************************************************************
########################  DATA STRUCTURES #######################
*****************************************************************/
typedef struct sPointCoord{
    float x;
    float y;
} PointCoord;           //size: 
    
/**
 * Point - Describes a Point in its 2D coordinates and the
 * respective value.
 */
typedef struct sPoint{
    PointCoord coordinates; //2D Coordinates
    uint8      value;       //Intensity value (unsigned 8-bit)
}Point;

/**
 * List of Points - Includes a Point, identified by an ID and a
 * pointer for the following and previous Point.
 */
typedef struct sPointLL{
    int lenght;                     //Current occupied indexes
    int size;                       //Allocated array size
    Point *points;                  //Array of points
}*PointLL;

/*****************************************************************
######################  END DATA STRUCTURES #####################
 *****************************************************************/

/*****************************************************************
########################    PROTOTYPES    #######################
 *****************************************************************/
 
 
/*############## POINT_LL ##############*/

/** CONSTRUCTOR **/

/**
 * Creates a new list of Points with the indicated Point.
 */
PointLL createNewPointLL(Point* point);

/**
 * Reallocates the PointLL array to double the size
 */
PointLL realocPointLL(PointLL list);

/** INSERT **/

/**
 * Inserts a new Point at the end of the list.
 */
PointLL addPointLLEntry(PointLL list, Point* addPoint);

/** REMOVE **/

/**
 * Removes a Point identified by its ID from the list.
 */
PointLL remPointLLEntry(PointLL entry, int remId);

/**
 * Removes all Points from the list.
 */
void remAllPointLL(PointLL list);

/** GETS **/

/**
 * From the given list of Points gets the maximum and minimum 2D coordinates
 * and intensity values, as well as the number of points in the list.
 */
void getPointLLParameters(PointLL list, float *coordXBeg, float *coordYBeg,
        float *coordXEnd, float *coordYEnd, uint8 *minValue, uint8 *maxValue,
        int *pointCount);

/**
 * Given a list of Points the start of X and Y Coordinates and their size
 * returns a Point of the geometric centroid.
 */
PointCoord* compute2DPolygonCentroid(PointLL list, PointCoord* centroid, int startX, int startY, int sizeX, int sizeY);

/**
 * Counts the number of Points in the given list.
 */
int pointCount(PointLL list);

/** OTHER **/

/**
 * Updates the Points ID following the indicated Point by its increment value.
 */
void updateIDNextPointLL(PointLL point, int increment);

/*############## POINT ##############*/

/** CONSTRUCTOR **/

/**
 * Creates a new Point
 */
Point* createNewPoint(float coordX, float coordY, uint8 value);

/** REMOVE **/

/**
 * Removes the Point (free memory).
 */
void remPoint(Point* point);

/*****************************************************************
########################  END PROTOTYPES    ######################
 *****************************************************************/


#ifdef	__cplusplus
}
#endif

#endif	/* POINTLL_H */

