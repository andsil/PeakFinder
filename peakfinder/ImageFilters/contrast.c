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
        
        int* histogram = res->instensityCounter;        
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
            res->instensityCounter[i] = (int)round(PsSk[i]*(levels-1));
        }
        
        // Generate the equalized image
        for(y = 0; y < res->height; y++){
            for(x = 0; x < res->width; x++){
                res->image[y][x] = saturate_cast_uint8(Sk[img->image[y][x]]);
            }
        }
        
        //initialize statistics
        for(i=0; i<levels; i++){
            res->instensityCounter[i] = 0;
        }
        res->minimum=255; res->maximum=0;
        //update statistics
        for(i=0; i<res->height; i++){
            createStatistics(res->image[i], res->width, &res->maximum, &res->minimum, res->instensityCounter);
        }
        res->median  = getMedian(res->instensityCounter, levels, res->height*res->width);
        res->average = getAverage(res->instensityCounter, levels, res->height*res->width);
        
        return res;
    }
    
error:
    fprintf(stderr, "[CONTRAST]An error occurred\n");
    if(res) destroyTiffImage(res);
    return NULL;
}