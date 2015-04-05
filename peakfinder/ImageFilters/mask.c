#include "mask.h"

TiffImage aplyMask(TiffImage img, int r){
    int i,j;
    TiffImage res = cloneTiffImage(img);
    for(i=0; i<res->height; i++){
        for(j=0; j<res->width; j++){
            res->image[i][j] = 0;
        }
    }
    
    int** intmask = (int**)malloc(sizeof(int*)*(2*r+1));
    for(i=0; i<(2*r+1); i++){
        intmask[i] = (int*)calloc(sizeof(int)*(2*r+1), sizeof(int));
    }
    
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
    int regionIntensity[img->pointCount];
    int startCoordX, startCoordY;
    RegionLL auxRLL = img->listRegions;
    int intensity, intensitySum;//int ii=1;
    FILE* results = fopen("results.txt","w");
    fprintf(results, "Ponto (X,Y) Intensidade\n");
    while(auxRLL){
        startCoordX = (int)round(auxRLL->region->centroid.x) - r;
        startCoordY = (int)round(auxRLL->region->centroid.y) - r;
        intensitySum = 0;
        //fprintf(stdout, "%d\n", ii++);fflush(stdout);
        if(isInside(startCoordY, startCoordX, img->height, img->width) &&
           isInside(startCoordY+2*r, startCoordX+2*r, img->height, img->width)){
        
            for(i=0; i<2*r; i++){
                for(j=0;j<2*r; j++){
                    intensity = img->image[startCoordX+i][startCoordY+j] * intmask[i][j];
                    res->image[startCoordX+i][startCoordY+j] = intensity;
                    intensitySum += intensity;
                }
            }
        } else {
            fprintf(stderr, "Warning!!! outside matrix bounds-> point %d skipped!\n", auxRLL->id);
        }
        regionIntensity[auxRLL->id] = intensitySum;
        fprintf(results, "%d (%.3f, %.3f) %d\n", auxRLL->id, auxRLL->region->centroid.x, auxRLL->region->centroid.y, regionIntensity[auxRLL->id]);
        auxRLL = auxRLL->nextRegion;
    }
    fflush(results);
        
    return res;
}