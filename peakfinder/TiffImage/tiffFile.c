#include "tiffFile.h"

TiffImage initTiffImage(){
    TiffImage res;
    // Structure allocation
    if(!(res = (TiffImage)malloc(sizeof(struct sTiffImage)))){
        goto error;
    }
    res->ndirs              = 0;
    res->fileName           = NULL;
    res->image              = NULL;
    res->width              = 0;
    res->height             = 0;
    res->config             = 0;
    res->fillOrder          = 0;
    res->nSamples           = 0;
    res->depth              = 0;
    res->photometric        = 0;
    res->resUnit            = 0;
    res->xRes               = 0;
    res->yRes               = 0;
    res->maximum            = 0;
    res->minimum            = 255;
    res->instensityCounter  = NULL;
    res->median             = 0;
    res->average            = 0;
    res->listRegions        = NULL;
    res->pointCount         = 0;
    
    return res;
    
error:
    fprintf(stderr, "[TIFFFILE]An error occurred while alocating memory.\n");
    return NULL;
}

void* destroyTiffImage(TiffImage obj){
    int i;
    
    if(obj){
        if(obj->fileName) free(obj->fileName);
        if(obj->image){
            for(i=0; i<obj->height; i++)
                if(obj->image[i])
                    free(obj->image[i]);
            free(obj->image);
        }
        if(obj->instensityCounter){
            free(obj->instensityCounter);
        }
        free(obj);
    }
    
    return NULL;
}

TiffImage cloneTiffImage(TiffImage obj){
    TiffImage res;
    int i, j, height=0, width=0, levels;
    
    // Structure allocation
    if(!(res = initTiffImage())){
        goto error;
    }
    
    res->ndirs          = obj->ndirs;
    res->width = width  = obj->width;
    res->height= height = obj->height;
    res->config         = obj->config;
    res->fillOrder      = obj->fillOrder;
    res->nSamples       = obj->nSamples;
    res->depth          = obj->depth;
    res->photometric    = obj->photometric;
    res->resUnit        = obj->resUnit;
    res->xRes           = obj->xRes;
    res->yRes           = obj->yRes;
    res->maximum        = obj->maximum;
    res->minimum        = obj->minimum;
    res->median         = obj->median;
    res->average        = obj->average;
    res->pointCount     = obj->pointCount;
    res->listRegions    = obj->listRegions; //SHOULD BE COPIED VALUES-BY-VALUE!!!!
    levels = exp2(res->depth);
    
    //validation
    if(!(res->fileName = (char*)malloc(sizeof(char)*(strlen(obj->fileName)+1)))){
        goto error;
    }
    strcpy(res->fileName, obj->fileName);
    
    //validation
    if(!(res->image = (uint8**)malloc(sizeof(uint8*)*height))){
        goto error;
    }
    for(i=0; i<height; i++){
        //validation
        if(!(res->image[i]=(uint8*)malloc(sizeof(uint8)*width))){
            goto error;
        }
        //copy pixel values
        for(j=0; j<width; j++){
            res->image[i][j]=obj->image[i][j];
        }
    }
    
    //validation
    if(!(res->instensityCounter=(int*)malloc(sizeof(int)*levels))){
        goto error;
    }
    for(i=0; i<levels; i++){
        res->instensityCounter[i] = obj->instensityCounter[i];
    }
    
    return res;
    
error:
    fprintf(stderr, "[TIFFFILE]An error occurred\n");
    //clean up
    destroyTiffImage(res);
    return NULL;
}

void createStatistics(uint8* row, uint32 width, uint8* max, uint8* min, int* intensity){
    uint8 max_aux = 0, min_aux = 255, value;
    int i;
    for(i=0; i<width; i++){
        //put value in register
        value = row[i];
        //checks if is max
        if(value>max_aux){
            max_aux = value;
        }
        //checks if is min
        if(value<min_aux){
            min_aux = value;
        }
        //increments counter with that value
        intensity[value]++;
    }
    if(*max<max_aux) *max=max_aux;
    if(*min>min_aux) *min=min_aux;
}

uint8 getMedian(int* intensity, int levels, int size){
    int i, counter=0, half = size/2;
    for(i=0; i<levels && counter<half; i++){
        counter += intensity[i];
    }
    return i;
}

uint8 getAverage(int* intensity, int levels, int size){
    //variables
    int i;
    long int sum=0;
    uint8 avg=0;
    
    //validation
    if(size==0){
        goto error;
    }
    
    //sum all values
    for(i=0; i<levels; i++){
        sum += (intensity[i] * i);
    }
    
    //find avg
    avg = sum/size;
    
    return avg;
    
error:
    fprintf(stderr, "[AVG]Trying to divide by zero.\n");
    return 0;
}

void showHistogram(char* fileName, int* histogram, uint32 levels){
    int i;
    FILE* out;
    if(!(out = fopen(fileName,"w"))){
        goto error;
    }
    for(i=0; i<levels; i++){
        fprintf(out, "%d;", i);
    }
    fprintf(out, "\n");
    for(i=0; i<levels; i++){
        fprintf(out, "%d;", histogram[i]);
    }
    fflush(out);
    fclose(out);
    
error:
    fprintf(stderr,"An error occurred while creating CSV file\n");
    return;
}

void setTiffWarningsOff(){
    TIFFSetWarningHandler(NULL);
}