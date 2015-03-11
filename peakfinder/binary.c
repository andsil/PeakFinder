#include "binary.h"

TiffImage binImage8bitStatic(TiffImage img, float threshlevel){
    //variables
    uint8 trgLevel = ((int)(255*threshlevel))%255;
    return binImage8bit(img, trgLevel);
}

TiffImage binImage8bitStaticHalf(TiffImage img){
    //variables
    uint8 trgLevel = 127;
    return binImage8bit(img, trgLevel);
}

TiffImage binImage8bitAutoMedian(TiffImage img){
    //variables
    uint8 trgLevel = img->median;
    return binImage8bit(img, trgLevel);
}

TiffImage binImage8bitAutoAverage(TiffImage img){
    //variables
    uint8 trgLevel = img->average;
    return binImage8bit(img, trgLevel);
}

TiffImage binImage8bitDynamic(TiffImage img, float threshlevel){
    uint8 trgLevel = ((int)((img->maximum-img->minimum)*threshlevel))%255;
    return binImage8bit(img, trgLevel);
}

TiffImage binImage8bitDynamicHalf(TiffImage img){
    uint8 trgLevel = (img->maximum-img->minimum)/2;
    return binImage8bit(img, trgLevel);
}

TiffImage binImage8bit(TiffImage img, uint8 trgLevel){
    //variables
    int i,j;
    uint8** res = NULL;
    
    //validation
    if(!img){
        goto error;
    }
    
    //allocate memory for bin image
    if(!(res = (uint8**)malloc(sizeof(uint8*)*img->height))){
        goto error;
    }
    for(i=0; i<img->height; i++){
        //put the value to 0
        if(!(res[i] = (uint8*)calloc(sizeof(uint8)*img->width, sizeof(uint8)))){
            goto error;
        }
    }
    
    //construct bin image
    for(i=0; i<img->height; i++){
        for(j=0; j<img->width; j++){
            if(img->image[i][j]>trgLevel)
                res[i][j] = 255;
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
    if(res){
        for(i=0; i<img->height; i++){
            if(res[i]) free(res[i]);
        }
        free(res);
    }
    return NULL;
}