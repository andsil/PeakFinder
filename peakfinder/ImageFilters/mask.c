#include "mask.h"
#include <unistd.h>

/* LOCAL PROTOTYPES (private) */
int intensitySum(TiffImage img, int id, int startCoordX, int startCoordY, int r, int** intmask);
int maskApplication(TiffImage img, int id, int startCoordX, int startCoordY, int r, int** intmask, TiffImage res);

//DANGER! DO not garantee inexistent overlapping between regions
//-> map image pixels coords could work
TiffImage aplyMask(TiffImage img, int r){
    int i,j;
    TiffImage res = cloneTiffImage(img);
    //init image at black
    for(i=0; i<res->height; i++){
        for(j=0; j<res->width; j++){
            res->image[i][j] = 0;
        }
    }
    
    //construct mask matrix initialized at 0
    int** intmask = (int**)malloc(sizeof(int*)*(2*r+1));
    for(i=0; i<(2*r+1); i++){
        intmask[i] = (int*)calloc((2*r+1), sizeof(int));
    }
    
    //Gives the mask center the value of 1
    //% I=1;I<=DIAMETRO;I++; -> X
    for(i=0; i<2*r; i++){
        //% J=1;J<=DIAMETRO;J++; -> Y
        for(j=0;j<2*r; j++){
            //% (X-RAIO)^2 + (Y-RAIO)^2 < RAIO^2 -> pitagoras
            if (pow((i-r),2)+pow((j-r),2) < pow(r,2)){
                intmask[i][j] = 1;
            }
        }
    }
    
    int startCoordX, startCoordY;
    Region *auxR;
    int intensity;//int ii=1;
    
    //filename
    img->fileName = addExtension(img->fileName, "_results.csv");
    
    FILE* results = fopen(img->fileName,"w");
    fprintf(results, "Point;CoordX;CoordY;Intensity;\n");
    
    //validation
    if(!img->listRegions || !img->listRegions->regions){
        fprintf(stderr, "Error: No region list\n");
        return NULL;
    }
    
    //for each region
    for(i=0; i<img->listRegions->lenght; i++){
        auxR=&img->listRegions->regions[i];
        //get start coordinates from centroid
        startCoordX = (int)round(auxR->centroid.x) - r;
        startCoordY = (int)round(auxR->centroid.y) - r;
        //Get intensities from neighbors
        int centerInt=0, upInt=0, downInt=0, leftInt=0, rightInt=0, maxInt=0, lastInt=0;
        //flag for end of search
        char refinedCentroid = 0;//int counter = 0;
        //loop to find greater intensity sum
        while(!refinedCentroid) {
            refinedCentroid = 1;//counter++;if(counter > 10){sleep(10);}//if more than 10 moves stop (could be "deadlock")
            //gets the intensity sum of all four neighbors
            centerInt = intensitySum(img, i, startCoordX, startCoordY, r, intmask);  maxInt=centerInt;
            upInt     = intensitySum(img, i, startCoordX, startCoordY+1, r, intmask);if(upInt>maxInt)   maxInt=upInt;
            downInt   = intensitySum(img, i, startCoordX, startCoordY-1, r, intmask);if(downInt>maxInt) maxInt=downInt;
            leftInt   = intensitySum(img, i, startCoordX-1, startCoordY, r, intmask);if(leftInt>maxInt) maxInt=leftInt;
            rightInt  = intensitySum(img, i, startCoordX+1, startCoordY, r, intmask);if(rightInt>maxInt)maxInt=rightInt;
            //fprintf(stdout, "[%d]B %d-%d-%d-%d-%d (%d,%d)\n", auxRLL->id, centerInt, upInt, downInt, leftInt, rightInt, startCoordX, startCoordY);
            
            //avoid lock in jumping between two maximums
            if(maxInt==lastInt)
                break;
            
            //checks if some of them have greater intensity sum than the center
            //(second condition to garantee two oposite transformations are not applied in the same iteration)
            if(maxInt == upInt && refinedCentroid){//printf("up\n");
                auxR->centroid.y += 1;
                startCoordY +=1;
                refinedCentroid = 0;
            }
            if(maxInt == downInt && refinedCentroid){//printf("down\n");
                auxR->centroid.y -= 1;
                startCoordY -=1;
                refinedCentroid = 0;
            }
            if(maxInt == leftInt && refinedCentroid){//printf("left\n");
                auxR->centroid.x -= 1;
                startCoordX -=1;
                refinedCentroid = 0;
            }
            if(maxInt == rightInt && refinedCentroid){//printf("right\n");
                auxR->centroid.x += 1;
                startCoordX +=1;
                refinedCentroid = 0;
            }
            //if(!refinedCentroid)
            //    printf("[%d] Moved\n", auxRLL->id);
            lastInt=maxInt;
        }
        //apply refined coordinates to res image
        intensity = maskApplication(img, i, startCoordX, startCoordY, r, intmask, res);
        //save to CSV
        fprintf(results, "%d;%.3f;%.3f;%d;\n", i, auxR->centroid.x, auxR->centroid.y, intensity);
        //continue to next region
    }
    fflush(results);
    fclose(results);
        
    return res;
}

int intensitySum(TiffImage img, int id, int startCoordX, int startCoordY, int r, int** intmask) {
    int intensity, intensitySum = 0;
    int i,j;
    
    if(isInside(startCoordY, startCoordX, img->height, img->width) &&
       isInside(startCoordY+2*r, startCoordX+2*r, img->height, img->width)){

        for(i=0; i<2*r; i++){
            for(j=0;j<2*r; j++){
                intensity = img->image[startCoordY+i][startCoordX+j] * intmask[i][j];
                intensitySum += intensity;
            }
        }
    }
    return intensitySum;
}

int maskApplication(TiffImage img, int id, int startCoordX, int startCoordY, int r, int** intmask, TiffImage res) {
    int intensity, intensitySum = 0;
    int i,j;
    
    if(isInside(startCoordY, startCoordX, img->height, img->width) &&
       isInside(startCoordY+2*r, startCoordX+2*r, img->height, img->width)){

        for(i=0; i<2*r; i++){
            for(j=0;j<2*r; j++){
                intensity = img->image[startCoordY+i][startCoordX+j] * intmask[i][j];
                res->image[startCoordY+i][startCoordX+j] = intensity;
                intensitySum += intensity;
            }
        }
    } else {
        fprintf(stderr, "Warning!!! outside matrix bounds-> point %d skipped! (%d,%d)\n", id, startCoordX, startCoordY);
    }
    return intensitySum;
}