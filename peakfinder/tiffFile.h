#ifndef TIFFFILE_H
#define	TIFFFILE_H

#include <stdio.h>//fprintf
#include <stdlib.h>//malloc
#include <string.h>//strcpy
#include <math.h>//exp2
#include <tiff.h>//uint8/16/32
#include <tiffio.h>

#include "RegionLL.h"//tdir_t

#ifdef	__cplusplus
extern "C" {
#endif

/*****************************************************************
########################  DATA STRUCTURES #######################
*****************************************************************/
typedef struct sTiffImage{
  //Image details
  tdir_t  ndirs;              //Number of directories (num pages)
  char*   fileName;           //Name of source file
  uint8** image;              //Matrix of intensity values
  uint32  width;              //Width of the image
  uint32  height;             //Height of the image
  int     config;             //
  int     fillOrder;          //
  uint16  nSamples;           //1-Grey, 3-RGB
  uint16  depth;              //Image depth in bits
  uint16  photometric;        //
  uint16  resUnit;            //Metric or inches
  float   xRes;               //ppi in X
  float   yRes;               //ppi in Y
  // Useful Statistics
  uint8   maximum;            //Maximum intensity in image
  uint8   minimum;            //Minimum intensity in image
  int*    instensityCounter;  //Counter of pixels with index intensity
  uint8   median;             //Median of intensity
  uint8   average;            //Average intensity
  //Regions
  RegionLL listRegions;       //List of Regions in the image
  uint    pointCount;         //Number of points (entries in listRegion)
} *TiffImage;

/*****************************************************************
######################  END DATA STRUCTURES #####################
*****************************************************************/

/*****************************************************************
########################    PROTOTYPES    #######################
*****************************************************************/
/**
 * Allocates space and initializes all values on the structure to 0.
 */
TiffImage initTiffImage();

/**
 * Free memory allocated to structure and return NULL;
 */
void* destroyTiffImage(TiffImage obj);

/**
 * Clone the obj structure completely.
 */
TiffImage cloneTiffImage(TiffImage obj);

/**
 * Get the maximum, minimum and update intensity counter array.
 */
void createStatistics(uint8* row, uint32 width, uint8* max, uint8* min, int* intensity);

/**
 * From an array with levels length and Size pixels get the median value.
 */
uint8 getMedian(int* intensity, int levels, int size);

/**
 * From an array with levels length and Size pixels get the average value.
 */
uint8 getAverage(int* intensity, int levels, int size);

/**
 * Auxiliary function that prints to file in CSV format.
 */
void showHistogram(char* fileName, int* histogram, uint32 levels);

/**
 * Disable the Tiff library warnings.
 */
void  setTiffWarningsOff();


/*****************************************************************
########################  END PROTOTYPES    ######################
*****************************************************************/

#ifdef	__cplusplus
}
#endif

#endif	/* TIFFFILE_H */

