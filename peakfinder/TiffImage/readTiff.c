#include "readTiff.h"

/*****************************************************************
########################    FUNCTIONS     #######################
*****************************************************************/
TiffImage readTiffImage(char* fileName){
    //Variables
    TiffImage res = NULL;
    TIFF* tif = NULL;
    uint32 height=0, width=0;
    uint8 max=0, min=255;
    int* histogram = NULL;
    uint16 sample, nSample;
    tdata_t buf = NULL;
    int row;
    
    // Structure allocation
    if(!(res = initTiffImage())){
        goto error;
    }
    //Read Tiff file
    if (!(tif = TIFFOpen(fileName, "r"))) {
        goto error;
    }
    
    //Populate structure
    res->fileName = (char*)malloc(sizeof(char)*(strlen(fileName)+1));
    strcpy(res->fileName, fileName);
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH,      &res->width);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH,     &res->height);
    TIFFGetField(tif, TIFFTAG_PLANARCONFIG,    &res->config);
    TIFFGetField(tif, TIFFTAG_FILLORDER,       &res->fillOrder);
    TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &res->nSamples);
    TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE,   &res->depth);
    TIFFGetField(tif, TIFFTAG_PHOTOMETRIC,     &res->photometric);
    TIFFGetField(tif, TIFFTAG_RESOLUTIONUNIT,  &res->resUnit);
    TIFFGetField(tif, TIFFTAG_XRESOLUTION,     &res->xRes);
    TIFFGetField(tif, TIFFTAG_YRESOLUTION,     &res->yRes);
    res->image = (uint8**) malloc(sizeof(uint8*) * res->height);
    res->ndirs = 1;
    while (TIFFReadDirectory(tif)){
        res->ndirs++;
    }
    
    //allocate space
    histogram = (int*) calloc(exp2(res->depth), sizeof(int));

    //validation
    if(!histogram){
        goto error;
    }
    
    //Read image and get statistics
    nSample = res->nSamples;
    height  = res->height;
    width   = res->width;
    for (sample = 0; sample < nSample; sample++) {//1-B&W, 3-RGB
        for (row = 0; row < height; row++) {
            if(!(buf = _TIFFmalloc(TIFFScanlineSize(tif)))) goto error;
            //read image line
            TIFFReadScanline(tif, buf, row, sample);
            res->image[row]=(uint8*)buf;
            //printArray(res->image[row], res->width);
            createStatistics(res->image[row], width, &max, &min, histogram);
        }
    }
    
    //register image statistics
    res->histogram = histogram;
    res->minimum = min;
    res->maximum = max;
    res->median  = getMedian(histogram, exp2(res->depth), width*height);
    res->average = getAverage(histogram, exp2(res->depth), width*height);
    //showHistogram("imageHisto.csv",res->instensityCounter, exp2(res->depth));
    //printf("Min:%u Max:%u Median:%u\n", res->minimum, res->maximum, res->median);
    
    //close file
    TIFFClose(tif);
    
    return res;
    
error:
    printf("An error occurred\n");
    if(res) free(res);
    if(tif) TIFFClose(tif);
    if(buf) _TIFFfree(buf);
    if(histogram) free(histogram);
    return NULL;
}