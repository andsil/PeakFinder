#include "binary.h"

TiffImage binImage8bitStatic(TiffImage img, float threshlevel) {
    //variables
    uint8 trgLevel = ((int) (255 * threshlevel)) % 255;
    return binImage8bit(img, trgLevel);
}

TiffImage binImage8bitStaticHalf(TiffImage img) {
    //variables
    uint8 trgLevel = 127;
    return binImage8bit(img, trgLevel);
}

TiffImage binImage8bitAutoMedian(TiffImage img) {
    //variables
    uint8 trgLevel = img->median;
    return binImage8bit(img, trgLevel);
}

TiffImage binImage8bitAutoAverage(TiffImage img) {
    //variables
    uint8 trgLevel = img->average;
    return binImage8bit(img, trgLevel);
}

TiffImage binImage8bitDynamic(TiffImage img, float threshlevel) {
    uint8 trgLevel = ((int) ((img->maximum - img->minimum) * threshlevel)) % 255;
    return binImage8bit(img, trgLevel);
}

TiffImage binImage8bitDynamicHalf(TiffImage img) {
    uint8 trgLevel = (img->maximum - img->minimum) / 2;
    return binImage8bit(img, trgLevel);
}

TiffImage binImage8bit(TiffImage img, uint8 trgLevel) {
    //variables
    int i, j;
    uint8** res = NULL;

    //validation
    if (!img) {
        goto error;
    }

    //allocate memory for bin image
    if (!(res = (uint8**) malloc(sizeof (uint8*) * img->height))) {
        goto error;
    }
    for (i = 0; i < img->height; i++) {
        //put the value to 0
        if (!(res[i] = (uint8*) calloc(sizeof (uint8) * img->width, sizeof (uint8)))) {
            goto error;
        }
    }

    //construct bin image
    for (i = 0; i < img->height; i++) {
        for (j = 0; j < img->width; j++) {
            if (img->image[i][j] > trgLevel)
                res[i][j] = WHITE;
        }
        //free line to be replaced
        free(img->image[i]);
    }

    //free array of pointers
    free(img->image);

    //register image pointer to binary matrix created
    img->image = res;

    return img;

error:
    fprintf(stderr, "[BINIMG]An error occurred\n");
    if (res) {
        for (i = 0; i < img->height; i++) {
            if (res[i]) free(res[i]);
        }
        free(res);
    }
    return NULL;
}

/**
 * Morphologic Operator - Dilatation
 */
TiffImage binary_dilation(TiffImage img) {
    if (!img) {
        goto error;
    }
    TiffImage res = cloneTiffImage(img);
    if (!res) {
        goto error;
    }

    int width = img->width;
    int height = img->height;
    int i, j;
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            //if the pixel is black
            if (img->image[i][j] == BLACK) {
                res->image[i][j] = BLACK;
                if (i - 1 > 0) res->image[i - 1][j] = BLACK;
                if (j - 1 > 0) res->image[i][j - 1] = BLACK;
                if (i + 1 < height) res->image[i + 1][j] = BLACK;
                if (j + 1 < width) res->image[i][j + 1] = BLACK;
            }
        }
    }
    destroyTiffImage(img);

    return res;
error:
    fprintf(stderr, "[BINIMG]An error occurred\n");
    return NULL;
}

/**
 * Morphologic Operator - Erasion
 */
TiffImage binary_erosion(TiffImage img) {
    if (!img) {
        goto error;
    }
    TiffImage res = cloneTiffImage(img);
    if (!res) {
        goto error;
    }

    int width = img->width;
    int height = img->height;
    int i, j;
    int white;

    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            white = FALSE;
            //if the pixel is black
            if (img->image[i][j] == BLACK) {
                //if one of the surrounding pixels is white change to white
                if ((i - 1 > 0 && img->image[i - 1][j] == WHITE) ||
                        (j - 1 > 0 && img->image[i][j - 1] == WHITE) ||
                        (i + 1 < height && img->image[i + 1][j] == WHITE) ||
                        (j + 1 < width && img->image[i][j + 1] == WHITE)) {
                    white = TRUE;
                }
                if (white) {
                    res->image[i][j] = WHITE;
                } else {
                    res->image[i][j] = BLACK;
                }
            }
        }
    }
    destroyTiffImage(img);

    return res;
error:
    fprintf(stderr, "[BINIMG]An error occurred\n");
    return NULL;
}

/**
 * Morphologic Operator - Opening
 */
TiffImage binary_opening(TiffImage img) {
    //validation
    if (!img) {
        return NULL;
    }

    return binary_dilation(binary_erosion(img));
}

/**
 * Morphologic Operator - Closing
 */
TiffImage binary_closing(TiffImage img) {
    //validation
    if (!img) {
        return NULL;
    }

    return binary_erosion(binary_dilation(img));
}

/**
 * Gets the mean of the surrounding pixels
 */
TiffImage binary_meanFilter(TiffImage img) {
    //variables
    float sum;
    int i, j, k, l;

    //validation
    if (!img) {
        goto error;
    }

    //new image
    TiffImage res = cloneTiffImage(img);
    if (!res) {
        goto error;
    }

    // define the kernel
    float Kernel[3][3] = {
        {1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0},
        {1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0},
        {1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0}
    };

    //init at 0-> unnecessary
    for (i = 0; i < img->height; i++)
        for (j = 0; j < img->width; j++)
            res->image[i][j] = 0.0;

    //convolution operation
    for (i = 1; i < img->height - 1; i++) {
        for (j = 1; j < img->width - 1; j++) {
            sum = 0.0;
            for (k = -1; k <= 1; k++) {
                for (l = -1; l <= 1; l++) {
                    sum = sum + Kernel[l + 1][k + 1] * img->image[i - l][l - k];
                }
            }
            res->image[i][j] = sum;
        }
    }

    //free previous image
    destroyTiffImage(img);

    return res;
error:
    fprintf(stderr, "[BINIMG]An error occurred\n");
    return NULL;
}

/**
 * Gets the median of the surrounding pixels
 */
TiffImage binary_medianFilter(TiffImage img) {
    //variables
    int i, j;
    
    //create a sliding window of size 9
    uint8 window[9];

    //validation
    if (!img) {
        goto error;
    }

    //new image
    TiffImage res = cloneTiffImage(img);
    if (!res) {
        goto error;
    }

    //init at 0 -> unnecessary
    for (i = 0; i < img->height; i++)
        for (j = 0; j < img->width; j++)
            res->image[i][j] = 0.0;

    for (i = 1; i < img->height - 1; i++) {
        for (j = 1; j < img->width - 1; j++) {

            // Pick up window element
            window[0] = img->image[i-1][j-1];
            window[1] = img->image[i-1][j];
            window[2] = img->image[i-1][j+1];
            window[3] = img->image[i][j-1];
            window[4] = img->image[i][j];
            window[5] = img->image[i][j+1];
            window[6] = img->image[i+1][j-1];
            window[7] = img->image[i+1][j];
            window[8] = img->image[i+1][j+1];

            // sort the window to find median
            insertionSort(window);

            // assign the median to centered element of the matrix
            res->image[i][j] = (window[3]+window[4])/2;
        }
    }

    //free previous image
    destroyTiffImage(img);

    return res;
error:
    fprintf(stderr, "[BINIMG]An error occurred\n");
    return NULL;
}

