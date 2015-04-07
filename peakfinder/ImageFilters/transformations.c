#include "transformations.h"

/**
 * Morphologic Operator - Dilatation
 */
TiffImage dilation(TiffImage img) {
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
    fprintf(stderr, "[TRANSFORMATION]An error occurred\n");
    return NULL;
}

/**
 * Morphologic Operator - Erasion
 */
TiffImage erosion(TiffImage img) {
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
    fprintf(stderr, "[TRANSFORMATION]An error occurred\n");
    return NULL;
}

/**
 * Morphologic Operator - Opening
 */
TiffImage opening(TiffImage img) {
    //validation
    if (!img) {
        return NULL;
    }

    return dilation(erosion(img));
}

/**
 * Morphologic Operator - Closing
 */
TiffImage closing(TiffImage img) {
    //validation
    if (!img) {
        return NULL;
    }

    return erosion(dilation(img));
}
