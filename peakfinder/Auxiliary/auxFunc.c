#include "auxFunc.h"

/**
 * remove_ext: removes the "extension" from a file spec.
 * @param mystr is the string to process.
 * @param dot is the extension separator.
 * @param sep is the path separator (0 means to ignore).
 * @return an allocated string identical to the original but
 *   with the extension removed. It must be freed when you're
 *   finished with it.
 *   If you pass in NULL or the new string can't be allocated,
 *   it returns NULL.
 */
char *remove_ext (char* mystr, char dot, char sep) {
    char *retstr, *lastdot, *lastsep;

    // Error checks and allocate string.

    if (mystr == NULL)
        return NULL;
    if ((retstr = (char*)malloc(sizeof(char) * strlen (mystr) + 1)) == NULL)
        return NULL;

    // Make a copy and find the relevant characters.

    strcpy (retstr, mystr);
    lastdot = strrchr (retstr, dot);
    lastsep = (sep == 0) ? NULL : strrchr (retstr, sep);

    // If it has an extension separator.

    if (lastdot != NULL) {
        // and it's before the extenstion separator.

        if (lastsep != NULL) {
            if (lastsep < lastdot) {
                // then remove it.

                *lastdot = '\0';
            }
        } else {
            // Has extension separator with no path separator.

            *lastdot = '\0';
        }
    }

    // Return the modified string.

    return retstr;
}

/**
 * int main(int argc, char* argv[]) {
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
char* concat(int count, ...) {
    va_list ap;
    int i;

    // Find required length to store merged string
    int len = 1; // room for NULL
    va_start(ap, count);
    for(i=0 ; i<count ; i++)
        len += strlen(va_arg(ap, char*));
    va_end(ap);

    // Allocate memory to concat strings
    char *merged;
    if(!(merged= calloc(len, sizeof(char))))
        return NULL;;
    int null_pos = 0;

    // Actually concatenate strings
    va_start(ap, count);
    for(i=0 ; i<count ; i++)
    {
        char *s = va_arg(ap, char*);
        strcpy(merged+null_pos, s);
        null_pos += strlen(s);
    }
    va_end(ap);

    return merged;
}

/**
 * Auxiliary functions that returns if the current Point (row, column) is
 * inside the image area or not.
 */
char isInside(int startY, int startX, int sizeY, int sizeX){
    if(startX<0 || startY<0 || startY>=sizeY || startX>=sizeX){
        return FALSE;
    }
    return TRUE;
}

/**
 * sort the window using insertion sort
 * insertion sort is best for this sorting
 */
void insertionSort(uint8 window[]) {
    int i, j;
    uint8 temp;
    
    for(i = 0; i < 9; i++){
        temp = window[i];
        for(j = i-1; j >= 0 && temp < window[j]; j--){
            window[j+1] = window[j];
        }
        window[j+1] = temp;
    }
}