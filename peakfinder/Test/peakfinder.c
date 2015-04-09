#ifndef RELEASE

#include <stdio.h>//fgets, fprintf, etc
#include <stdlib.h>//realloc
#include <string.h>//strchr
//#include <fftw3.h>
//#include <inttypes.h>//PRId32
//#include <unistd.h>//usleep

#include "../TiffImage/readTiff.h"//readTiffImage
#include "../TiffImage/writeTiff.h"//writeTiffImage
#include "../ImageFilters/contrast.h"//histogramEqualization,binImage8bit
#include "../Auxiliary/auxFunc.h"//
#include "../ImageProcessing/RegionLL.h"//RegionLL
#include "../ImageProcessing/maxTreshHold.h" //findRegion
#include "../ImageFilters/mask.h"//aplyMask
#include "../ImageFilters/filters.h"//mean, median, gaussian,...
#include "../ImageFilters/clahe.h"//CLAHE
#include "../ImageFilters/transformations.h"//closing
#include "../Auxiliary/complex.h"//Complex
#include "../ImageFilters/fourier.h"//fourier, inverseFourier,...

/*****************************************************************
########################    PROTOTYPES    #######################
*****************************************************************/

void  printArray(FILE* fd, uint8* array, int width);
void  printIntMatrix(FILE* fd, uint8** matrix, int width, int height);
char* readline(FILE *file);
void  showCentroid(TiffImage img, RegionLL regList);
void  getWDim(TiffImage img);
int   getDistances(TiffImage img);
int   compare( const void* a, const void* b);
char  alreadyExists(Region *neighbors, int pointCount, int id);

/*****************************************************************
########################  END PROTOTYPES    ######################
*****************************************************************/

/*****************************************************************
########################    FUNCTIONS     #######################
*****************************************************************/
int main(int argc, char* argv[]) {
    
    //Parse filename to image
    char* inputFileName;
    char* aux_FileName;
    int i,j;
    int res;
    
    if(argc<2){//iterative
        //printf("Put the path to File:");
        inputFileName = /*"InputImages/a0.tif";*/readline(stdin);
    } else {//automatic
        inputFileName = argv[1];
    }
    
    if(!inputFileName){
        //goto error;
        return -1;
    }
    
    //Disable libtiff warnings
    setTiffWarningsOff();

    //Read Image
    TiffImage image = readTiffImage(inputFileName);
    
    //validation
    if(!image){
        //goto error;
        return -2;
    }
    
    //test detail data
    fprintf(stdout, "Original Image:\n");fflush(stdout);
    fprintf(stdout, "ndirs=%d ,fileName=%s ,width=%d ,height=%u ,config=%u ,"
            "fillOrder=%d ,nSamples=%u ,depth=%u ,photometric=%u ,"
            "resUnit=%u ,xRes=%f ,yRes=%f ,maximum=%u ,"
            "minimum=%u ,median=%u ,average=%u\n",
            image->ndirs, image->fileName, image->width, image->height,
            image->config, image->fillOrder, image->nSamples, image->depth,
            image->photometric, image->resUnit, image->xRes, image->yRes,
            image->maximum, image->minimum, image->median, image->average);fflush(stdout);
    
/* BEGIN HISTOGRAM EQUILIZER CONTRAST */
    TiffImage contrasted;
    /*
    //contrasted = histogramEqualization(image);
    contrasted = histogramEqualization(gaussianFilter(image));
    //contrasted = cloneTiffImage(image);
    //CLAHE(&(contrasted->image[0][0]), (unsigned int)contrasted->width, (unsigned int)contrasted->height,contrasted->minimum, contrasted->maximum, 8, 8, 64, 0.25);
    
    fprintf(stdout, "Contrasted Image:\n");fflush(stdout);
    printf("ndirs=%d ,fileName=%s ,width=%d ,height=%u ,config=%u ,"
            "fillOrder=%d ,nSamples=%u ,depth=%u ,photometric=%u ,"
            "resUnit=%u ,xRes=%f ,yRes=%f ,maximum=%u ,"
            "minimum=%u ,median=%u,average=%u\n",
            contrasted->ndirs, contrasted->fileName, contrasted->width,
            contrasted->height, contrasted->config, contrasted->fillOrder,
            contrasted->nSamples, contrasted->depth, contrasted->photometric,
            contrasted->resUnit, contrasted->xRes, contrasted->yRes,
            contrasted->maximum, contrasted->minimum, contrasted->median,
            contrasted->average);
    
    //Write Image
    fprintf(stdout, "Writing contrasted Image\n");fflush(stdout);
    res = writeTiffImage(contrasted->fileName, contrasted);*/
    //int res = writeTiffImage("clahe.tif", contrasted);
    
/* END HISTOGRAM EQUILIZER CONTRAST */
    TiffImage aux;
    
/* BEGIN FOURIER */
    fprintf(stdout, "Fourier...\n");fflush(stdout);
    
    aux = cloneTiffImage(image);

    /* FILENAME */
    char fourierExt[] = "_fourier.tif";
    aux_FileName = remove_ext(aux->fileName, '.', '/');
    if(!(aux->fileName = (char*)realloc(aux->fileName, strlen(aux_FileName)+strlen(fourierExt)+1))){
        //goto error;
        return -3;
    }
    aux->fileName = concat(2, aux_FileName, fourierExt);
    free(aux_FileName);
    /* END FILENAME */
    
    //Alloc memory
    Complex** outComp = (Complex**) malloc (sizeof(Complex*)*aux->height);
    Complex** outCompFilter = (Complex**) malloc (sizeof(Complex*)*aux->height);
    int u;
    for(u=0; u<aux->height; u++){
        outComp[u] = (Complex*) malloc (sizeof(Complex)*aux->width);
        outCompFilter[u] = (Complex*) malloc (sizeof(Complex)*aux->width);
    }
    //Transform image into frequency domain
    fourier(outComp, aux->image, aux->height);
    
    //write image spectrum
    fourierSpectrumImage(aux->image, outComp, aux->height);
    
    //Output spectrum
    res = writeTiffImage(aux->fileName,aux);
        
/* BEGIN TEST FOURIER FILTERING */
    
    fprintf(stdout, "apply filter...\n");fflush(stdout);
    
    int width = aux->width; int height = aux->height; int D;
    
    //REF: https://github.com/ajatix/iplab/blob/3de740d83e05a449acfa37b9c1f506893176ac49/Expt6/FFTAnalysis.cpp
    //Butterworth_HPF 
    D = 1024;//-> size of mask
    /*for(i=0;i<height;i++) {
        for(j=0;j<width;j++) {
            outComp[i][j].Re = outComp[i][j].Re*(1/(1+pow((float)D/sqrt((float)(i-height/2)*(float)(i-height/2)+(float)(j-width/2)*(float)(j-width/2)),2)));
            outComp[i][j].Im = outComp[i][j].Im*(1/(1+pow((float)D/sqrt((float)(i-height/2)*(float)(i-height/2)+(float)(j-width/2)*(float)(j-width/2)),2)));
        }
    }*/
    
    //Butterworth_LPF 
    /*D = 1024;
    for(i=0;i<height;i++) {
        for(j=0;j<width;j++) {
            outComp[i][j].Re = outComp[i][j].Re*(1/(1+pow(sqrt((float)(i-height/2)*(float)(i-height/2)+(float)(j-width/2)*(float)(j-width/2))/(float)D,2)));
            outComp[i][j].Im = outComp[i][j].Im*(1/(1+pow(sqrt((float)(i-height/2)*(float)(i-height/2)+(float)(j-width/2)*(float)(j-width/2))/(float)D,2)));
        }
    }*/
    /*
    //LPF
    D = sqrt(pow(width,2)+pow(height,2));//pitagoras
    D = (int)D*0.99; //99% -> deletes center
    for(i=0;i<height;i++) {
        for(j=0;j<width;j++) {
            if( (pow(i-height/2,2) + pow(j-width/2,2)) >= pow(D/2,2) ) {
                outComp[i][j].Re = 0;
                outComp[i][j].Im = 0;
            }
        }
    }
    */
    double distance, rest;
    double module1, module2;
    
    int width_half = width/2;
    int height_half = height/2;
    
    int maxDis = sqrt(pow(height_half,2) + pow(width_half,2));
    double histogram[maxDis];
    int histogramPoints[maxDis];
    
    int coordX, coordY;

    for(i=0; i<maxDis; i++){
        histogram[i] = 0;
        histogramPoints[i] = 0;
    }
    
    fprintf(stdout, "(512,512)i:%f\n", (log10(compAbs(outComp[512][512])) * 100.0) + 255);
    
    for(i=0; i<height; i++){
      for(j=0; j<width; j++){
        coordY = height_half - i;
        coordX = width_half - j;
        distance = sqrt(pow(coordX,2) + pow(coordY,2));
        rest = ((distance/1.0)==distance)?0:distance - ((int)distance);
        module1 = (log10(compAbs(outComp[i][j])) * 100.0) + 255;
        if(module1<0){
            module1 = 0;
        } else if(module1>255){
            module1 = 255;
        }
        if(rest!=0){
            if(coordY>0 && coordX>0){//(+,+) -> 2nd quadrant
                module2 = (log10(compAbs(outComp[i+1][j-1])) * 100.0) + 255;
            } else if(coordY<0 && coordX >0){//(-,+) -> 3rd quadrant
                module2 = (log10(compAbs(outComp[i-1][j-1])) * 100.0) + 255;
            } else if(coordY<0 && coordX <0){//(-,-) -> 4th quadrant
                module2 = (log10(compAbs(outComp[i-1][j+1])) * 100.0) + 255;
            } else { //(+,-) -> 1st quadrant
                module2 = (log10(compAbs(outComp[i+1][j+1])) * 100.0) + 255;
            }
            if(module2<0){
                module2 = 0;
            } else if(module2>255){
                module2 = 255;
            }
            histogram[(int)(distance/1)+1] += module1*rest + module2*(1-rest);
            histogramPoints[(int)(distance/1)+1]++;
        } else {
            histogram[(int)distance] += module1;
            histogramPoints[(int)distance]++;
        }
      }
    }
    
    FILE* radialHisto = fopen("radialHisto.csv","w");
    fprintf(radialHisto, "Distance;points;avg intensity;\n");
    double local_max[3], local_min[2]; int local_max_dist[3], local_min_dist[2];
    int maxPoints=0, minPoints=0;
    double actual_avgInt, prev_avgInt;int inc=0; int infMax=0; int infMin=0;
    for(i=0; i<3; i++){
        local_max[i]=0;
    }
    for(i=0; i<maxDis; i++){
        actual_avgInt = histogram[i]/histogramPoints[i];
        if((maxPoints<3 || minPoints<2)&& i>0){
            if(actual_avgInt<prev_avgInt && inc==1){//if starts to decrement and previous iteration was incrementing
                infMax = 1;//declare point of inflection
            } else {
                infMax = 0;
            }
            if(actual_avgInt>prev_avgInt && inc==0){//if starts to increment and previous iteration was decrementing
                infMin = 1;//declare point of inflection
            } else {
                infMin = 0;
            }
            inc = (actual_avgInt>prev_avgInt)?1:0;//is it incrementing?1-Yes 0-No
            if(infMax==1){
                local_max[maxPoints] = prev_avgInt;
                local_max_dist[maxPoints] = i-1;
                maxPoints++;
            }
            if(infMin==1){
                if(maxPoints==0){
                    local_min[minPoints]=prev_avgInt;
                    local_min_dist[minPoints] = i-1;
                    minPoints++;
                }
                if(maxPoints==3){
                    local_min[minPoints]=prev_avgInt;
                    local_min_dist[minPoints] = i-1;
                    minPoints++;
                }
            }
        }
        fprintf(radialHisto, "%d;%d;%f;\n", i, histogramPoints[i], actual_avgInt);
        prev_avgInt = actual_avgInt;
    }
    fprintf(radialHisto, "Radius local max;Avg Intensity;\n");
    for(i=0; i<3; i++)
        fprintf(radialHisto, "%d;%f;\n",local_max_dist[i],local_max[i]);
    fprintf(radialHisto, "Radius local min;Avg Intensity;\n");
    for(i=0; i<2; i++)
        fprintf(radialHisto, "%d;%f;\n",local_min_dist[i],local_min[i]);
    fclose(radialHisto);
    
    /*
    //HPF
    D = sqrt(pow(width,2)+pow(height,2));//pitagoras
    D = (int)D*0.5;
    for(i=0;i<height;i++) {
        for(j=0;j<width;j++) {
            //removes every point with less than 250 pixel intensity
            if(((log10(compAbs(outComp[i][j])) * 100.0) + 255) <= 250){
                outComp[i][j].Re = 0;
                outComp[i][j].Im = 0;
            }
            *//*
            //Removes high frequencies
            if ((i<(height/2+D/2)) && (i>(height/2-D/2)) && (j<(width/2+D/2)) && (j>(width/2-D/2))) {
            //if(((i-height/2)*(i-height/2)+(j-width/2)*(j-width/2))>(D/2*D/2)) {
                outComp[i][j].Re = 0;
                outComp[i][j].Im = 0;
            }*//*
        }
    }
    */
    
    for(i=0;i<height;i++) {
        for(j=0;j<width;j++) {
            coordY = height_half - i;
            coordX = width_half - j;
            distance = sqrt(pow(coordX,2) + pow(coordY,2));
            if(distance<local_min_dist[0] || distance>local_min_dist[1]){
                outComp[i][j].Re = 0;
                outComp[i][j].Im = 0;
            } else {
                printf("(%d,%d)nao entrei\n",i,j);
            }
        }
    }
    
    fprintf(stdout, "inverseFourier...\n");fflush(stdout);
    //return to space domain
    inverseFourier(aux->image, outComp, aux->height);
    
    /* FILENAME */
    char filteredExt[] = "_filtered.tif";
    aux_FileName = remove_ext(aux->fileName, '.', '/');
    if(!(aux->fileName = (char*)realloc(aux->fileName, strlen(aux_FileName)+strlen(filteredExt)+1))){
        goto error;
    }
    aux->fileName = concat(2, aux_FileName, filteredExt);
    free(aux_FileName);
    /* END FILENAME */
    
    fprintf(stdout, "print result...\n");fflush(stdout);
    //print result
    res = writeTiffImage(aux->fileName,aux);
    
/* END TEST FOURIER FILTERING */
    
    //Get fourier spectrum of the frequency domain
    fourierSpectrumImage(aux->image, outComp, aux->height);
    
    //Output spectrum
    res = writeTiffImage(aux->fileName,aux);
    
    inverseFourier(aux->image, outComp, aux->height);
    contrasted = histogramEqualization(aux);
    fprintf(stdout, "Contrasted Image:\n");fflush(stdout);
    printf("ndirs=%d ,fileName=%s ,width=%d ,height=%u ,config=%u ,"
            "fillOrder=%d ,nSamples=%u ,depth=%u ,photometric=%u ,"
            "resUnit=%u ,xRes=%f ,yRes=%f ,maximum=%u ,"
            "minimum=%u ,median=%u,average=%u\n",
            contrasted->ndirs, contrasted->fileName, contrasted->width,
            contrasted->height, contrasted->config, contrasted->fillOrder,
            contrasted->nSamples, contrasted->depth, contrasted->photometric,
            contrasted->resUnit, contrasted->xRes, contrasted->yRes,
            contrasted->maximum, contrasted->minimum, contrasted->median,
            contrasted->average);
    
    //Write Image
    fprintf(stdout, "Writing contrasted Image\n");fflush(stdout);
    res = writeTiffImage(contrasted->fileName, contrasted);
    
    destroyTiffImage(aux);
    
    fprintf(stdout, "Done\n");fflush(stdout);
/* END FOURIER*/
    
/* BEGIN BINARIZING */
    fprintf(stdout, "Binarizing...\n");fflush(stdout);
    
    aux = cloneTiffImage(contrasted);
    
    /* FILENAME */
    char binarizedExt[] = "_binarized.tif";
    aux_FileName = remove_ext(aux->fileName, '.', '/');
    if(!(aux->fileName = (char*)realloc(aux->fileName, strlen(aux_FileName)+strlen(binarizedExt)+1))){
        goto error;
    }
    aux->fileName = concat(2, aux_FileName, binarizedExt);
    free(aux_FileName);
    /* END FILENAME */
    
#if 1
    //otsu's automatic binarization method
    int trg = getOtsuThreshold(aux->histogram, 0, aux->height, 0, aux->width);
    fprintf(stdout, "Threshold:%d\n", trg);
    binImage8bit(aux, trg);
#else
    //static threshold
    binImage8bitStatic(aux, (int) (0.684*255));
#endif
    res = writeTiffImage(aux->fileName,aux);
    
    /* BEGIN CLOSING*/
    
    /* FILENAME */
    char closedExt[] = "_closed.tif";
    aux_FileName = remove_ext(aux->fileName, '.', '/');
    if(!(aux->fileName = (char*)realloc(aux->fileName, strlen(aux_FileName)+strlen(closedExt)+1))){
        goto error;
    }
    aux->fileName = concat(2, aux_FileName, closedExt);
    free(aux_FileName);
    /* END FILENAME */
    
    aux = closing(aux);
    
    res = writeTiffImage(aux->fileName,aux);
    
    /* END CLOSING */
    
    fprintf(stdout, "Done\n");fflush(stdout);
    
/* END BINARIZING */
    
/* BEGIN REGION DETECTION*/
    fprintf(stdout, "Detecting regions...\n");fflush(stdout);
    
    if(aux == NULL || image == NULL){
        fprintf(stdout, "Something went wrong (images validation)\n");fflush(stdout);
        goto error;
    }
    
    //Find regions and count them
    RegionLL regionList = aux->listRegions = findRegions(aux);
    if(aux->listRegions == NULL){
        fprintf(stdout, "Something went wrong (finding regions)\n");fflush(stdout);
        goto error;
    }
    image->pointCount  = aux->pointCount  = regionCount(aux->listRegions);
    
    //save regions on both images:
    //  image-> for mask application
    //  aux  -> for further calculations
    image->listRegions = aux->listRegions = regionList;
    
    //Validation
    if(regionList != NULL){
        
        RegionLL auxRL = getLastRegionEntry(regionList);
        if(auxRL){
            fprintf(stdout, "There are %d regions\n" , auxRL->id);fflush(stdout);
        } else {
            fprintf(stdout, "Something went wrong (Getting last region)\n");fflush(stdout);
            goto error;
        }
        
        Region auxReg = auxRL->region;
        if(!auxReg){
            fprintf(stderr, "Something went wrong (No Region to present)\n");fflush(stdout);
            goto error;
        } else {
            fprintf(stdout, "Last Region starts at (%f, %f) end at (%f,%f) Pixels:%d minVal:%u maxVal:%u Centroid (%.3f,%.3f)\n" ,
                    auxReg->coordXBeg, auxReg->coordYBeg, auxReg->coordXEnd, auxReg->coordYEnd,
                    auxReg->pointCount, auxReg->minValue, auxReg->maxValue, auxReg->centroid.x, auxReg->centroid.y);fflush(stdout);
        }
        
        //show centroid for each region in the result image
        showCentroid(aux, regionList);
        
        /* FILENAME */
        char centroidExt[] = "_centroid.tif";
        aux_FileName = remove_ext(aux->fileName, '.', '/');
        if(!(aux->fileName = (char*)realloc(aux->fileName, strlen(aux_FileName)+strlen(centroidExt)+1))){
            goto error;
        }
        aux->fileName = concat(2, aux_FileName, centroidExt);
        free(aux_FileName);
        /* END FILENAME */
        
        res = writeTiffImage(aux->fileName, aux);
        
    } else {
        fprintf(stderr, "No list to present!\n");fflush(stdout);
        goto error;
    }
    
/* END REGION DETECTION*/
    
/* BEGIN APPLYING MASK */
    
    fprintf(stdout, "Applying mask...\n");fflush(stdout);
    
    //get minimum distance between Centroids
    int wdim = getDistances(aux);
    
    //Apply mask to original image
    TiffImage masked = aplyMask(image, wdim/2);
    
    //getWDim(aux);//->Does not work (something wrong)
    
    /* FILENAME */
    char maskedExt[] = "_masked.tif";
    aux_FileName = remove_ext(aux->fileName, '.', '/');
    if(!(aux->fileName = (char*)realloc(aux->fileName, strlen(aux_FileName)+strlen(maskedExt)+1))){
        goto error;
    }
    aux->fileName = concat(2, aux_FileName, maskedExt);
    free(aux_FileName);
    /* END FILENAME */
    res = writeTiffImage(aux->fileName, masked);
    
    fprintf(stdout, "Done wdim:%d\n",wdim);
    
/* END APPLYING MASK */
    
    //validation
    if(res != 0){
        goto error;
    }
    
    //clean up
    destroyTiffImage(image);
    destroyTiffImage(contrasted);
    destroyTiffImage(aux);
    return 0;

//error handling
error:
    fprintf(stderr,"[MAIN] An error occurred\n");
    return -1;
}

/**
 * Read line from stdin
 */
char* readline(FILE *file){
    size_t size  = 80;
    size_t curr  = 0;
    char *buffer = malloc(size);
    while(fgets(buffer + curr, size - curr, file)) {
        if(strchr(buffer + curr, '\n')){
            return buffer; // success
        }
        curr = size - 1;
        size *= 2;
        char *tmp = realloc(buffer, size);
        if(tmp == NULL){//handle error
            goto error;
        }
        buffer = tmp;
    }
    return buffer;

error:
    fprintf(stderr,"[MAIN]No space left to allocate\n");
    return NULL;
}

/**
 * Print to fd file descriptor integer values of each image pixel
 */
void printIntMatrix(FILE* fd, uint8** matrix, int width, int height){
    int i,j;
    for(i=0; i<height; i++){
        for(j=0; j<width; j++){
            fprintf(fd, "%01u ", matrix[i][j]);
        }
        fprintf(fd, "\n");
    }
}

/**
 * Print to fd file descriptor integer values of each row pixel
 */
void printArray(FILE* fd, uint8* array, int width){
    int i;
    for(i=0; i<width; i++){
        fprintf(fd, "%03u ", array[i]);
    }
    fprintf(fd, "\n");
}

void showCentroid(TiffImage img, RegionLL regList){
    RegionLL auxRLL = getFirstRegionEntry(regList);
    Region   auxR;
    
    while(auxRLL){
        auxR = auxRLL->region;
        img->image[(int)round(auxR->centroid.y)][(int)round(auxR->centroid.x)] = 0;
        auxRLL = auxRLL->nextRegion;
    }
}

//BRUTE FORCE
//ONLY to make things work
int getDistances(TiffImage img){
    //validation
    if(!img || !(img->listRegions)){
        fprintf(stderr, "Warning: An error occurred\n");
        return -1;
    }
    
    //variables
    int pointCount  = img->pointCount;
    
    //three arrays. 2 Arrays for each axis indicating the id of the point
    //A third array indexed by pointID contains the pointer for the region.
    Region  point[pointCount];
    double  minDistance[pointCount];//Minimum
    
    //Auxiliary variables
    RegionLL auxRLL;
    Region  auxR;
    int     auxPointID, id;
    
    double  distanceX, distanceY, distance, distanceMin, distanceMax;
    
    //for each region register the ID
    auxRLL = img->listRegions;
    while(auxRLL){
        auxR  = auxRLL->region;
        auxPointID = auxRLL->id;
        if(auxR){
            //register the point with regionID with Region pointer
            point[auxPointID-1] = auxR;
        } else {
            fprintf(stderr, "Warning: Distances -> Point skipped\n");
        }
        auxRLL = auxRLL->nextRegion;
    }
    
    //init
    distanceMin = -1;
    distanceMax = -1;
    for(id=0; id<=pointCount; id++){
        minDistance[id] = -1;
    }
    
    //TOTALLY UNNECESSARY!
    //Too much computation and space wasted.
    //for each point computes the distance to every other point
    for(id=0; id<pointCount; id++){
        for(auxPointID=id+1; auxPointID<pointCount; auxPointID++){//previous distances already checked
            distanceX = fabs(point[auxPointID]->centroid.x - point[id]->centroid.x);
            distanceY = fabs(point[auxPointID]->centroid.y - point[id]->centroid.y);
            distance = sqrt(pow(distanceX,2)+pow(distanceY,2));
            //set two way distance
            if(minDistance[id] > distance || minDistance[id]<0){
                minDistance[id] = distance;
            }
            if(minDistance[auxPointID] > distance || minDistance[auxPointID]<0){
                minDistance[auxPointID] = distance;
            }
        }
        //fprintf(stdout, "[%d] (%.3f,%.3f) %.3f\n", id+1, point[id]->centroid.x, point[id]->centroid.y, minDistance[id]);
    }
    
    for(id=0; id<pointCount; id++){
        if(distanceMin > minDistance[id] || distanceMin<0){
            distanceMin = minDistance[id];
        }
        if(distanceMax < minDistance[id]){
            distanceMax = minDistance[id];
        }
    }
    
    //fprintf(stdout, "Min:%.3f Max:%.3f Percentage:%.3f\n", distanceMin, distanceMax, (distanceMax-distanceMin)/distanceMin*100);
    return ((int)round(distanceMin));
}

int compare( const void* a, const void* b){
     int double_a = * ( (double*) a );
     int double_b = * ( (double*) b );

     if ( double_a == double_b ) return 0;
     else if ( double_a < double_b ) return -1;
     else return 1;
}

//SOMETHING IS WRONG IN THIS CODE
//Some points are more distant than the radius, which should be impossible
//in a circle
void getWDim(TiffImage img){
//more readable definitions
#define MIN 0
#define MAX 1
    
    //validation
    if(!img || !(img->listRegions)){
        fprintf(stderr, "Warning: An error occurred\n");
        return;
    }
    
    //variables
    int height      = img->height;
    int width       = img->width;
    int pointCount  = img->pointCount;
    
    //three arrays. 2 Arrays for each axis indicating the id of the point
    //A third array indexed by pointID contains the pointer for the region.
    PointLL yAxis[height];
    PointLL xAxis[width];
    Region  point[pointCount];
    
    //Auxiliary variables
    RegionLL auxRLL;
    Region  auxR;
    PointLL auxPLL;
    Point   auxP;
    int     coordX, coordY, auxPointID;
    float   coordFX, coordFY;
    
    //up to 4 neighbors for each point
    Region  neighbors[pointCount][4];
    float   neighborsDistances[pointCount][4];
    int     neighborsCount[pointCount];
    
    //Auxiliary variables for distance calculation
    PointLL auxPLLRow, auxPLLCol;
    double  distanceX, distanceY, distance;
    int     id, row, column, radius = 0, prevrow;
    char    out, found;
    
    //init
    int i;
    for(i=0; i<height; i++){
        yAxis[i] = NULL;
    }
    for(i=0; i<width; i++){
        xAxis[i] = NULL;
    }
    
    //for each region register the ID and centroid coords
    auxRLL = img->listRegions;
    while(auxRLL){
        auxR  = auxRLL->region;
        auxPointID = auxRLL->id;
        if(auxR){
            //get coords
            coordFX = auxR->centroid.x;
            coordFY = auxR->centroid.y;
            //create a new Point (value is irrelevant)
            auxP = createNewPoint(coordFX, coordFY, 0);
            //get integer coords
            coordX = (int)round(coordFX);
            coordY = (int)round(coordFY);
            //add the current centroid point to the (possible) list
            xAxis[coordX] = addPointLLEntry(xAxis[coordX], auxP);
            yAxis[coordY] = addPointLLEntry(xAxis[coordY], auxP);
            //(validation) sets the point ID to the same Region ID
            if(xAxis[coordX]){ xAxis[coordX]->id = auxPointID; }
            if(yAxis[coordY]){ yAxis[coordY]->id = auxPointID; }
            //register the point with regionID with Region pointer
            point[auxPointID] = auxR;
        } else {
            fprintf(stderr, "Warning: WDim -> Point skipped\n");
        }
        auxRLL = auxRLL->nextRegion;
    }
    
    printf("POINTCOUNT:%d\n", pointCount);
    
    //for each point find its 4 neighbors
    for(id=1; id<=10; id++){
        //get Point and centroid coords
        auxR = point[id];
        coordFY = auxR->centroid.y;//rows
        coordFX = auxR->centroid.x;//columns
        coordY  = (int)round(coordFY);
        coordX  = (int)round(coordFX);
        //init
        neighborsCount[id] = 0;
        radius = 0;
        row = 0;
        out = FALSE;
        
        //increment radius until distance to 4 points are discovered or circle
        //range is outside of image space
        while(neighborsCount[id]<4 && !out){
            //update
            radius++;//starts at 1
            
            for(column=0; column<=radius && neighborsCount[id]<4 && !out; column++){
                prevrow = row;
                //circle radius constant & column decrement -> calculate row
                //radius²  = row² + column²
                //<=> row² = radius² - column²
                //<=> row  = sqrt(radius² - column²)
                row = /*radius-column;*/(int)round(sqrt(pow(radius,2)-pow(column,2)));//CAN BE OPTIMIZED!! do pow(radius,2) only once and store in variable
                
                //Q1 -> first quadrant NE
                //if inside image area
                for(i=prevrow; i<row; i++){
                    if(isInside(coordY+row, coordX+column, height, width)){
                        //get points in the row
                        auxPLLRow = yAxis[coordY+i];
                        //get points in the column
                        auxPLLCol = xAxis[coordX+column];
                        //if exists points within the row and column
                        if(auxPLLRow && auxPLLCol){
                            //find intersect point
                            found = FALSE;
                            while(auxPLLRow && !found){
                                auxPLL = auxPLLCol;
                                while(auxPLL && !found){
                                    if(auxPLLRow->id == auxPLL->id && auxPLL->id != id){
                                        auxPointID = auxPLL->id;
                                        found = TRUE;
                                    }
                                    auxPLL = auxPLL->nextPoint;
                                }
                                auxPLLRow = auxPLLRow->nextPoint;
                            }
                            //if intersect point was found
                            if(found && neighborsCount[id]<4 && 
                                    !alreadyExists(neighbors[id], neighborsCount[id], auxPointID)){
                                printf("Q1:%d R:%d\n", auxPointID, radius);
                                //save Region in neighbors array
                                neighbors[id][neighborsCount[id]] = point[auxPointID];
                                //Pitagoras Theorem to get distance between points
                                distanceX = fabs(point[auxPointID]->centroid.x-coordFX);
                                distanceY = fabs(point[auxPointID]->centroid.y-coordFY);
                                distance = sqrt(pow(distanceX,2)+pow(distanceY,2));
                                //save distances
                                neighborsDistances[id][neighborsCount[id]] = distance;
                                //increment counter
                                neighborsCount[id]++;
                            }
                        }
                    } else {
                        out = TRUE;
                        printf("Q1: OUT! (%d,%d) R:%d\n", column, i, radius);
                    }
                }
                
                //Q2 -> second quadrant NW
                //if inside image area
                for(i=prevrow; i<row; i++){
                if(isInside(coordY+row, coordX-column, height, width)){
                    //get points in the row
                    auxPLLRow = yAxis[coordY+i];
                    //get points in the column
                    auxPLLCol = xAxis[coordX-column];
                    //if exists points within the row and column
                    if(auxPLLRow && auxPLLCol){
                        //find intersect point
                        found = FALSE;
                        while(auxPLLRow && !found){
                            auxPLL = auxPLLCol;
                            while(auxPLL && !found){
                                if(auxPLLRow->id == auxPLL->id && auxPLL->id != id){
                                    auxPointID = auxPLL->id;
                                    found = TRUE;
                                }
                                auxPLL = auxPLL->nextPoint;
                            }
                            auxPLLRow = auxPLLRow->nextPoint;
                        }
                        //if intersect point was found
                        if(found && neighborsCount[id]<4 && 
                                !alreadyExists(neighbors[id], neighborsCount[id], auxPointID)){
                            printf("Q2:%d R:%d\n", auxPointID, radius);
                            //save Region in neighbors array
                            neighbors[id][neighborsCount[id]] = point[auxPointID];
                            //Pitagoras Theorem to get distance between points
                            distanceX = fabs(point[auxPointID]->centroid.x-coordFX);
                            distanceY = fabs(point[auxPointID]->centroid.y-coordFY);
                            distance = sqrt(pow(distanceX,2)+pow(distanceY,2));
                            //save distances
                            neighborsDistances[id][neighborsCount[id]] = distance;
                            //increment counter
                            neighborsCount[id]++;
                        }
                    }
                } else {
                    out = TRUE;
                    printf("Q2: OUT! (%d,%d) R:%d\n", column, i, radius);
                }}
                
                //Q3 -> third quadrant SW
                //if inside image area
                for(i=prevrow; i<row; i++){
                if(isInside(coordY-row, coordX-column, height, width)){
                    //get points in the row
                    auxPLLRow = yAxis[coordY-i];
                    //get points in the column
                    auxPLLCol = xAxis[coordX-column];
                    //if exists points within the row and column
                    if(auxPLLRow && auxPLLCol){
                        //find intersect point
                        found = FALSE;
                        while(auxPLLRow && !found){
                            auxPLL = auxPLLCol;
                            while(auxPLL && !found){
                                if(auxPLLRow->id == auxPLL->id && auxPLL->id != id){
                                    auxPointID = auxPLL->id;
                                    found = TRUE;
                                }
                                auxPLL = auxPLL->nextPoint;
                            }
                            auxPLLRow = auxPLLRow->nextPoint;
                        }
                        //if intersect point was found
                        if(found && neighborsCount[id]<4 && 
                                !alreadyExists(neighbors[id], neighborsCount[id], auxPointID)){
                            printf("Q3:%d R:%d\n", auxPointID, radius);
                            //save Region in neighbors array
                            neighbors[id][neighborsCount[id]] = point[auxPointID];
                            //Pitagoras Theorem to get distance between points
                            distanceX = fabs(point[auxPointID]->centroid.x-coordFX);
                            distanceY = fabs(point[auxPointID]->centroid.y-coordFY);
                            distance = sqrt(pow(distanceX,2)+pow(distanceY,2));
                            //save distances
                            neighborsDistances[id][neighborsCount[id]] = distance;
                            //increment counter
                            neighborsCount[id]++;
                        }
                    }
                } else {
                    out = TRUE;
                    printf("Q3: OUT! (%d,%d) R:%d\n", column, i, radius);
                }}
                
                //Q4 -> fourth quadrant SE
                //if inside image area
                for(i=prevrow; i<row; i++){
                if(isInside(coordY-row, coordX+column, height, width)){
                    //get points in the row
                    auxPLLRow = yAxis[coordY-i];
                    //get points in the column
                    auxPLLCol = xAxis[coordX+column];
                    //if exists points within the row and column
                    if(auxPLLRow && auxPLLCol){
                        //find intersect point
                        found = FALSE;
                        while(auxPLLRow && !found){
                            auxPLL = auxPLLCol;
                            while(auxPLL && !found){
                                if(auxPLLRow->id == auxPLL->id && auxPLL->id != id){
                                    auxPointID = auxPLL->id;
                                    found = TRUE;
                                }
                                auxPLL = auxPLL->nextPoint;
                            }
                            auxPLLRow = auxPLLRow->nextPoint;
                        }
                        //if intersect point was found
                        if(found && neighborsCount[id]<4 && 
                                !alreadyExists(neighbors[id], neighborsCount[id], auxPointID)){
                            printf("Q4:%d R:%d\n", auxPointID, radius);
                            //save Region in neighbors array
                            neighbors[id][neighborsCount[id]] = point[auxPointID];
                            //Pitagoras Theorem to get distance between points
                            distanceX = fabs(point[auxPointID]->centroid.x-coordFX);
                            distanceY = fabs(point[auxPointID]->centroid.y-coordFY);
                            distance = sqrt(pow(distanceX,2)+pow(distanceY,2));
                            //save distances
                            neighborsDistances[id][neighborsCount[id]] = distance;
                            //increment counter
                            neighborsCount[id]++;
                        }
                    }
                } else {
                    out = TRUE;
                    printf("Q4: OUT! (%d,%d) R:%d\n", column, i, radius);
                }}
            }
        }
        
        fprintf(stdout, "[%d] (%.3f,%.3f)", id, coordFX, coordFY);
        //int i;
        for(i=0; i<neighborsCount[id]; i++){
            fprintf(stdout, " [%d]->(%.3f,%.3f) %f", neighbors[id][i]->id, neighbors[id][i]->centroid.x, neighbors[id][i]->centroid.y, neighborsDistances[id][i]);
        }
        fprintf(stdout, "\n");
    }
    

}

char alreadyExists(Region *neighbors, int pointCount, int id){
    int i;
    for(i=0; i<pointCount; i++){
        if(neighbors[i]->id == id){
            return TRUE;
        }
    }
    return FALSE;
}

#endif