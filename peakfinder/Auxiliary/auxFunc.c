#include "auxFunc.h"

/* LOCAL PROTOTYPES */
int partition_mod( double value[], int position[], int l, int r);

/**
 * Read line from stdin
 */
char* readline(FILE *file){
    size_t size  = 80;
    size_t curr  = 0;
    char *buffer = malloc(size);
    while(fgets(buffer + curr, size - curr, file)) {
        if(strchr(buffer + curr, '\n')){
            return buffer; // success
        }
        curr = size - 1;
        size *= 2;
        char *tmp = realloc(buffer, size);
        if(tmp == NULL){//handle error
            goto error;
        }
        buffer = tmp;
    }
    return buffer;

error:
    fprintf(stderr,"[MAIN]No space left to allocate\n");
    return NULL;
}

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

int compare( const void* a, const void* b){
     int double_a = * ( (double*) a );
     int double_b = * ( (double*) b );

     if ( double_a == double_b ) return 0;
     else if ( double_a < double_b ) return -1;
     else return 1;
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

/**
 * Adds the extension array to the original filename
 * @return new pointer or NULL if an error occurred
 */
char* addExtension(char* original, char extension[]){
    char *aux_FileName = remove_ext(original, '.', '/');
    if(!(original = (char*)realloc(original, strlen(aux_FileName)+strlen(extension)+1))){
        return NULL;
    }
    original = concat(2, aux_FileName, extension);
    free(aux_FileName);
    
    return original;
}

/**
 * Quick Sort modified to swap position in two arrays based on the values of the first one
 * @param value Histogram of values
 * @param position Histogram of position
 * @param l start
 * @param r end
 */
void quickSort_mod( double value[], int position[], int l, int r){
    int j;

    if( l < r ) {
   	// divide and conquer
        j = partition_mod( value, position, l, r);
        quickSort_mod( value, position, l, j-1);
        quickSort_mod( value, position, j+1, r);
    }	
}

int partition_mod( double value[], int position[], int l, int r) {
   int i, j;
   double pivot, t_d; int t;
   pivot = value[l];
   i = l; j = r+1;
		
   while(1){
   	do ++i; while( value[i] >= pivot && i <= r );
   	do --j; while( value[j] < pivot );
   	if( i >= j ) break;
   	t_d = value[i]; value[i] = value[j]; value[j] = t_d;
        t = position[i]; position[i] = position[j]; position[j] = t;
   }
   t_d = value[l]; value[l] = value[j]; value[j] = t_d;
   t = position[l]; position[l] = position[j]; position[j] = t;
   return j;
}