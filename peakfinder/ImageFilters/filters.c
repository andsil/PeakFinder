#include "filters.h"

/**
 * Gets the mean of the surrounding pixels
 */
TiffImage meanFilter(TiffImage img) {
    //variables
    //float sum;

    //validation
    if (!img) {
        goto error;
    }

    //new image
    TiffImage res = cloneTiffImage(img);
    if (!res) {
        goto error;
    }

    int sum, x, xx, xxx, y, yy, yyy;
    for (y = 1; y < img->height - 1; y++) //for each row    except the first and last
    {
        for (x = 1; x < img->width - 1; x++) //for each column except the first and last
        {
            sum = 0;
            for (yy = -1; yy <= 1; yy++) //upper, mid and lower indicees
            {
                yyy = y + yy;
                for (xx = -1; xx <= 1; xx++)//left,  mid and right indicees
                {
                    xxx = x + xx;
                    sum += img->image[yyy][xxx]; //add them up
                }
            }
            res->image[y][x] = ((float) sum) / ((float) 9.0); //divide by 9 and round
        }
    }
    /*
        //convolution operation
        for (i = 1; i < img->height - 1; i++) {
            for (j = 1; j < img->width - 1; j++) {
                sum = 0.0;
                for (k = -1; k <= 1; k++) {
                    for (l = -1; l <= 1; l++) {
                        sum += img->image[i + l][l + k];
                    }
                }
                res->image[i][j] = ((float) sum) / 9.0;
            }
        }
     */

    return res;
error:
    fprintf(stderr, "[BINIMG]An error occurred\n");
    return NULL;
}

/**
 * Gets the median of the surrounding pixels
 */
TiffImage medianFilter(TiffImage img) {
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

    for (i = 1; i < img->height - 1; i++) {
        for (j = 1; j < img->width - 1; j++) {

            // Pick up window element
            window[0] = img->image[i - 1][j - 1];
            window[1] = img->image[i - 1][j];
            window[2] = img->image[i - 1][j + 1];
            window[3] = img->image[i][j - 1];
            window[4] = img->image[i][j];
            window[5] = img->image[i][j + 1];
            window[6] = img->image[i + 1][j - 1];
            window[7] = img->image[i + 1][j];
            window[8] = img->image[i + 1][j + 1];

            // sort the window to find median
            insertionSort(window);

            // assign the median to centered element of the matrix
            res->image[i][j] = window[4];
        }
    }

    return res;
error:
    fprintf(stderr, "[BINIMG]An error occurred\n");
    return NULL;
}

/**
 * Sigma filter
 */
TiffImage sigmaFilter(TiffImage img, int sigma) {
    //variables

    //validation
    if (!img) {
        goto error;
    }

    //new image
    TiffImage res = cloneTiffImage(img);
    if (!res) {
        goto error;
    }

    //----------
    int midPixel, anyPixel, sum, no, x, xx, xxx, y, yy, yyy;
    for (y = 0; y < img->height; y++) //=============
    {
        for (x = 0; x < img->width; x++) //===========
        {
            midPixel = img->image[y][x]; //input is the noisy image
            sum = no = 0;
            for (yy = -1; yy <= 1; yy++) //==========
            {
                yyy = y + yy;
                if (yyy < 0 || yyy >= img->height)
                    continue; //beyond the border
                for (xx = -1; xx <= 1; xx++)//=========
                {
                    xxx = xx + x;
                    if (xxx < 0 || xxx >= img->width)
                        continue; //beyond the border
                    anyPixel = img->image[yyy][xxx];
                    if (abs(midPixel - anyPixel) < sigma) {
                        sum += anyPixel;
                        no++;
                    }
                } //====== end for (int xx... ================
            } //======== end for (int yy... ================
            if (no > 0)
                res->image[y][x] = ((float) sum) / no;
            else
                res->image[y][x] = midPixel; //just copy
        } //============ end for (int x... =====================
    }
    //----------

    return res;
error:
    fprintf(stderr, "[BINIMG]An error occurred\n");
    return NULL;
}

/**
 * gaussian filter
 * sigma=1
 */
TiffImage gaussianFilter(TiffImage img) {
    //variables


    //validation
    if (!img) {
        goto error;
    }

    //new image
    TiffImage res = cloneTiffImage(img);
    TiffImage temp = cloneTiffImage(img);
    if (!res || !temp) {
        goto error;
    }

    //----------
    float sum;
    int i, y, x, x1, y1;

    // coefficients of 1D gaussian kernel with sigma = 1
    double coeffs[] = {0.0545, 0.2442, 0.4026, 0.2442, 0.0545};

    // along y - direction
    for (y = 0; y < img->height; y++) {
        for (x = 0; x < img->width; x++) {
            sum = 0.0;
            for (i = -2; i <= 2; i++) {
                y1 = reflect(img->height, y - i);
                sum = sum + coeffs[i + 2] * img->image[y1][x];
            }
            temp->image[y][x] = sum;
        }
    }

    // along x - direction
    for (y = 0; y < img->height; y++) {
        for (x = 0; x < img->width; x++) {
            sum = 0.0;
            for (i = -2; i <= 2; i++) {
                x1 = reflect(img->width, x - i);
                sum = sum + coeffs[i + 2] * temp->image[y][x1];
            }
            res->image[y][x] = sum;
        }
    }
    //----------

    //free previous image
    destroyTiffImage(temp);

    return res;
error:
    fprintf(stderr, "[BINIMG]An error occurred\n");
    return NULL;
}

/**
 * 5x5 kernel
 */
void createFilter(double gKernel[][5]) {
    int x, y, i, j;
    // set standard deviation to 1.0
    double sigma = 1.0;
    double r, s = 2.0 * sigma * sigma;

    // sum is for normalization
    double sum = 0.0;

    // generate 5x5 kernel
    for (x = -2; x <= 2; x++) {
        for (y = -2; y <= 2; y++) {
            r = sqrt(x * x + y * y);
            gKernel[x + 2][y + 2] = (exp(-(r * r) / s)) / (M_PI * s);
            sum += gKernel[x + 2][y + 2];
        }
    }

    // normalize the Kernel
    for (i = 0; i < 5; ++i)
        for (j = 0; j < 5; ++j)
            gKernel[i][j] /= sum;

}

// reflected indexing for border processing

int reflect(int M, int x) {
    if (x < 0) {
        return -x - 1;
    }
    if (x >= M) {
        return 2 * M - x - 1;
    }

    return x;
}