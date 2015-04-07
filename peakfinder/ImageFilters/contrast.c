#include "contrast.h"

/**
 * Generate cumulative frequency histogram
 */
void cumhist(int histogram[], int cumhistogram[]){//size missing
    int i;
    
    cumhistogram[0] = histogram[0];
    for(i = 1; i < 256; i++){
        cumhistogram[i] = histogram[i] + cumhistogram[i-1];
    }
}

uint8 saturate_cast_uint8(int x){
    uint8 res;
    if(x<0)     return (res=0);
    if(x>255)   return (res=255);
    return (res=x);
}

TiffImage histogramEqualization(TiffImage img){
    //variables
    int i,x,y;
    TiffImage res = NULL;
    
    if(!img){
        goto error;
    }
    
    if(!(res = cloneTiffImage(img))){
        goto error;
    }
    
    char append[] = "_cont.tiff";
    char* aux_fileName = remove_ext(res->fileName, '.', '/');
    if(!(res->fileName = (char*)realloc(res->fileName, strlen(aux_fileName)+strlen(append)+1))){
        goto error;
    }
    res->fileName = concat(2, aux_fileName, append);
    free(aux_fileName);
    
    {//work-around for the goto scope error
        
        int* histogram = res->histogram;        
        // Calculate the size of image
        int levels = exp2(res->depth);
        int size = res->height * res->width;//check res!=0
        float alpha = ((float)levels-1)/size;
        
        // Calculate the probability of each intensity
        float PrRk[levels];
        for(i = 0; i < levels; i++){
            PrRk[i] = ((float)histogram[i]) / size;
        }
        
        // Generate cumulative frequency histogram
        int cumhistogram[levels];
        cumhist(histogram,cumhistogram);
        
        
        // Scale the histogram
        int Sk[levels];
        for(i = 0; i < levels; i++){
            Sk[i] = (int)round((float)cumhistogram[i] * alpha);
        }

        // Generate the equalized histogram
        float PsSk[levels];
        for(i = 0; i < levels; i++){
            PsSk[i] = 0;
        }
        for(i = 0; i < levels; i++){
            PsSk[Sk[i]] += PrRk[i];
        }
        //equalized histogram
        for(i = 0; i < levels; i++){
            res->histogram[i] = (int)round(PsSk[i]*(levels-1));
        }
        
        // Generate the equalized image
        for(y = 0; y < res->height; y++){
            for(x = 0; x < res->width; x++){
                res->image[y][x] = saturate_cast_uint8(Sk[img->image[y][x]]);
            }
        }
        
        //initialize statistics
        for(i=0; i<levels; i++){
            res->histogram[i] = 0;
        }
        res->minimum=255; res->maximum=0;
        //update statistics
        for(i=0; i<res->height; i++){
            createStatistics(res->image[i], res->width, &res->maximum, &res->minimum, res->histogram);
        }
        res->median  = getMedian(res->histogram, levels, res->height*res->width);
        res->average = getAverage(res->histogram, levels, res->height*res->width);
        
        return res;
    }
    
error:
    fprintf(stderr, "[CONTRAST]An error occurred\n");
    if(res) destroyTiffImage(res);
    return NULL;
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

TiffImage binImage8bit(TiffImage img, uint8 threshold) {
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
            if (img->image[i][j] > threshold)
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
