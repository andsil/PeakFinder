#ifndef AUXFUNC_H
#define	AUXFUNC_H

#include <stdlib.h>//malloc
#include <stdarg.h>//va_*
#include <string.h>//strcpy
#include <tiff.h>//uint8

#ifdef	__cplusplus
extern "C" {
#endif

/*****************************************************************
########################  DATA STRUCTURES #######################
 *****************************************************************/

//AUX DEFINITIONS
#define FALSE       0
#define TRUE        1
    

/*****************************************************************
######################  END DATA STRUCTURES #####################
 *****************************************************************/

/*****************************************************************
########################    PROTOTYPES    #######################
 *****************************************************************/
/**
 * remove_ext: removes the "extension" from a file spec.
 *    mystr is the string to process.
 *   dot is the extension separator.
 *   sep is the path separator (0 means to ignore).
 * Returns an allocated string identical to the original but
 *   with the extension removed. It must be freed when you're
 *   finished with it.
 * If you pass in NULL or the new string can't be allocated,
 *   it returns NULL.
 */
char *remove_ext (char* mystr, char dot, char sep);

/**
 * int main(int argc, char* argv[]){
 *     char *str;
 *     str = concat(0);             println(str); free(str);
 *     str = concat(1,"a");         println(str); free(str);
 *     str = concat(2,"a","b");     println(str); free(str);
 *     str = concat(3,"a","b","c"); println(str); free(str);
 *     return 0;
 * }
 * Output:
 *   // Empty line
 * a
 * ab
 * abc
 */
char* concat(int count, ...);

/**
 * Auxiliary functions that returns if the current Point (row, column) is
 * inside the image area or not.
 */
char isInside(int startY, int startX, int sizeY, int sizeX);

/**
 * sort the window using insertion sort
 * insertion sort is best for this sorting
 */
void insertionSort(uint8 window[]);

/*****************************************************************
########################  END PROTOTYPES    ######################
 *****************************************************************/


#ifdef	__cplusplus
}
#endif

#endif	/* AUXFUNC_H */

