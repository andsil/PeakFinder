#include "binary.h"

TiffImage binImage8bitStatic(TiffImage img, int threshold) {
    //variables
    uint8 trgLevel = threshold % 255;
    return binImage8bit(img, trgLevel);
}

/* REF: https://github.com/MPS-UPB/10Team/blob/3846a66e28a956c9bb8f784a6851b3fb400d4627/BAM1/binarization.cpp
 * calculate a global threshold for the image using Otsu algorithm
 * params
 * @histData: histogram of the image
 * @y0, y1: Oy coordinates of the image
 * @x0, x1: Ox coordinates of the image
 * @return: global threshold for the image
*/
int getOtsuThreshold(int *histData, int y0, int y1, int x0, int x1) {
    int height = y1 - y0;
    int width  = x1 - x0;

    // Total number of pixels
    int total = height * width;

    float sum = 0;
    int t;
    for (t = 0; t < 256 ; t++) 
        sum += t * histData[t];

    float sumB = 0;
    int wB = 0;
    int wF = 0;

    float varMax = 0;
    int threshold = 0;

    for (t = 0 ; t < 256 ; t++) {
        wB += histData[t];              // Weight Background
        if (wB == 0) continue;

        wF = total - wB;                 // Weight Foreground
        if (wF == 0) break;

        sumB += (float) (t * histData[t]);

        float mB = sumB / wB;            // Mean Background
        float mF = (sum - sumB) / wF;    // Mean Foreground

        // Calculate Between Class Variance
        float varBetween = (float)wB * (float)wF * (mB - mF) * (mB - mF);

        // Check if new maximum found
        if (varBetween > varMax) {
            varMax = varBetween;
            threshold = t;
        }
    }

    return threshold;
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
        if (!(res[i] = (uint8*) calloc(img->width, sizeof(uint8)))) {
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
TiffImage binary_sigmaFilter(TiffImage img, int sigma) {
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
TiffImage binary_gaussianFilter(TiffImage img) {
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
    int i,y,x,x1, y1;

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
void createFilter(double gKernel[][5])
{
    int x,y,i,j;
    // set standard deviation to 1.0
    double sigma = 1.0;
    double r, s = 2.0 * sigma * sigma;
 
    // sum is for normalization
    double sum = 0.0;
 
    // generate 5x5 kernel
    for (x = -2; x <= 2; x++)
    {
        for(y = -2; y <= 2; y++)
        {
            r = sqrt(x*x + y*y);
            gKernel[x + 2][y + 2] = (exp(-(r*r)/s))/(M_PI * s);
            sum += gKernel[x + 2][y + 2];
        }
    }
 
    // normalize the Kernel
    for(i = 0; i < 5; ++i)
        for(j = 0; j < 5; ++j)
            gKernel[i][j] /= sum;
 
}

// reflected indexing for border processing
int reflect(int M, int x)
{
    if(x < 0)
    {
        return -x - 1;
    }
    if(x >= M)
    {
        return 2*M - x - 1;
    }
    
    return x;
}