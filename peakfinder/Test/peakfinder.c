#ifndef RELEASE

//TODO:
//-adicionado histograma de pontos com base na sua intensidade de pixeis
//obhetivo: escolher o minimo local antes e depois dos pontos realçados para cortar

#include <stdio.h>//fgets, fprintf, etc
#include <stdlib.h>//realloc
#include <string.h>//strchr
//#include <fftw3.h>
//#include <inttypes.h>//PRId32
//#include <unistd.h>//usleep

#include "../Auxiliary/timer.h" //GET_TIME
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

void printArray(FILE* fd, uint8* array, int width);
void printIntMatrix(FILE* fd, uint8** matrix, int width, int height);
void showCentroid(TiffImage img, RegionLL regList);
int  local_max_min(double* histogram, int* histogramPoints, int size, double* local_max, double* local_min, int* local_max_dist, int* local_min_dist, int* maxPoints, int* minPoints);
void gnuplot(char* originalFileName, char* int_rad, char* var_rad);
void gnuplot_Peaks(char* originalFileName, char* int_log_rad, char* var_log_rad);
void gnuplot_histogram(char* outputFileName, char dataFileName[]);
void gnuplot_histogram_test(char* outputFileName, char dataFileName[]);

/*****************************************************************
########################  END PROTOTYPES    ######################
*****************************************************************/

/*****************************************************************
########################    FUNCTIONS     #######################
*****************************************************************/
int main(int argc, char* argv[]) {
    //Timer
    double startTime, finishTime;
    //Start timer
    GET_TIME(startTime);
    
    //Parse filename to image
    char* inputFileName;
    char* originalFileName = NULL;
    char verbose = 0;
    int i,j;
    
    if(argc<2){//iterative
        printf("Put the path to File:");
        inputFileName = /*"InputImages/a0.tif";*/readline(stdin);
    } else {//automatic
        inputFileName = argv[1];
        if(argc==3 && (strcmp(argv[2], "-v")==0 || strcmp(argv[2], "--verbose")==0)){
            verbose = 1;
        }
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
            
    originalFileName = strdup(image->fileName);
    
    if(verbose){
        //filename
        char* histogramGP = strdup(originalFileName);
        histogramGP = addExtension(histogramGP, "_originalHisto.png");
        char* histogramFile = strdup(originalFileName);
        histogramFile = addExtension(histogramFile, "_originalHisto.csv");
        FILE* histogramCSV = fopen(histogramFile,"w");
        fprintf(histogramCSV, "Intensity;Pixels;\n");
        for(i=0; i<255; i++){
            fprintf(histogramCSV, "%d;%d;\n", i, image->histogram[i]);
        }
        fclose(histogramCSV);
    
        gnuplot_histogram(histogramGP, histogramFile);
    }
    
/* BEGIN HISTOGRAM EQUILIZER CONTRAST */
    /*TiffImage contrasted;
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
    writeTiffImage(contrasted->fileName, contrasted);
    
    //filename
    char* histogramCountGP = strdup(originalFileName);
    histogramCountGP = addExtension(histogramCountGP, "_contHisto.png");
    FILE* histogramContCSV = fopen("histogramCont.csv","w");
    fprintf(histogramContCSV, "Intensity;Pixels;\n");
    for(i=0; i<255; i++){
        fprintf(histogramContCSV, "%d;%d;\n", i, contrasted->histogram[i]);
    }
    fclose(histogramContCSV);
    
    gnuplot_histogram(histogramCountGP, "histogramCont.csv");
    
    //int res = writeTiffImage("clahe.tif", contrasted);
    */
    
/* END HISTOGRAM EQUILIZER CONTRAST */
    TiffImage aux;
    
/* BEGIN FOURIER */
    fprintf(stdout, "Fourier...\n");fflush(stdout);
    
    aux = cloneTiffImage(image);

    //filename
    aux->fileName = addExtension(aux->fileName, "_fourier.tif");
    
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
    if(verbose){
        fourierSpectrumImage(aux->image, outComp, aux->height);
    
        //Output spectrum
        writeTiffImage(aux->fileName,aux);
    }
        
/* BEGIN TEST FOURIER FILTERING */
    
    fprintf(stdout, "apply filter...\n");fflush(stdout);
    
    int width = aux->width; int height = aux->height;
    //int D;
    
    //REF: https://github.com/ajatix/iplab/blob/3de740d83e05a449acfa37b9c1f506893176ac49/Expt6/FFTAnalysis.cpp
    //Butterworth_HPF 
    /*D = 1024;//-> size of mask
    for(i=0;i<height;i++) {
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
    /**/
    double distance, rest;
    double module1, module2;
    double module1_log, module2_log;
    
    int width_half = width/2;
    int height_half = height/2;
    
    //maximum distance
    int maxDis = ceil(sqrt(pow(height_half,2) + pow(width_half,2)));
    
    //histogram based analysis
    double histogram[maxDis];
    int histogramPoints[maxDis];
    double histogramMaxVar[maxDis];
    double histogramMinVar[maxDis];
    
    //intensity peaks analysis
    double peaks[maxDis];
    int peaksPoints[maxDis];
    double peaksMaxVar[maxDis];
    double peaksMinVar[maxDis];
    
    int pointCounter=0;//test variable
    
    int coordX, coordY;
    /*
    //LPF
    D = height_half;
    D = (int)D*0.05; //5% -> deletes center
    for(i=0;i<height;i++) {
        for(j=0;j<width;j++) {
            coordY = height_half - ((i + height / 2) % height);
            coordX = width_half - ((j + width / 2) % width);
            distance = sqrt(pow(coordX,2) + pow(coordY,2));
            if( distance<D  ) {
                outComp[i][j].Re = 0;
                outComp[i][j].Im = 0;
            }
        }
    }*/

    //BEGIN RADIAL HISTOGRAM
    
    //init
    for(i=0; i<maxDis; i++){
        histogram[i] = 0;
        histogramPoints[i] = 0;
        histogramMaxVar[i] = 0;
        histogramMinVar[i] = 1000000000;
        peaks[i] = 0;
        peaksPoints[i] = 1;//will be used in division
        peaksMaxVar[i] = 0;
        peaksMinVar[i] = 1000000000;
    }
    
    for(i=0; i<height; i++){
      for(j=0; j<width; j++){
        //coordY = height_half - i;
        //coordX = width_half - j;
        coordY = height_half - ((i + height / 2) % height);
        coordX = width_half - ((j + width / 2) % width);
        distance = sqrt(pow(coordX,2) + pow(coordY,2));
        rest = ((distance/1.0)==distance)?0:distance - ((int)distance);
        module1 = compAbs(outComp[i][j]);
        
        //pixel intensity representation
        module1_log = (log10(module1)*100.0)+255;
        if(module1_log < 0){
            module1_log = 0;
        } else if(module1_log > 255){
            module1_log = 255;
        }
        
        //Warning: Comments in following ifs could make no sense (needs revision)
        if(rest!=0){
            if(coordY>0 && coordX>0){//(+,+) -> 2nd quadrant
                if(((float)coordY)/coordX > 1.5){//more y than x
                    module2 = compAbs(outComp[i+1][j]);
                } else if(((float)coordX)/coordY > 1.5){//more x than y
                    module2 = compAbs(outComp[i][j-1]);
                } else {
                    module2 = compAbs(outComp[i+1][j-1]);
                }
            } else if(coordY<0 && coordX>0){//(-,+) -> 3rd quadrant
                if(((float)abs(coordY))/coordX > 1.5){//more y than x
                    module2 = compAbs(outComp[i-1][j]);
                } else if(((float)coordX)/abs(coordY) > 1.5){//more x than y
                    module2 = compAbs(outComp[i][j-1]);
                } else {
                    module2 = compAbs(outComp[i-1][j-1]);
                }
            } else if(coordY<0 && coordX<0){//(-,-) -> 4th quadrant
                if(((float)abs(coordY))/abs(coordX) > 1.5){//more y than x
                    module2 = compAbs(outComp[i-1][j]);
                } else if(((float)abs(coordX))/abs(coordY) > 1.5){//more x than y
                    module2 = compAbs(outComp[i][j+1]);
                } else {
                    module2 = compAbs(outComp[i-1][j+1]);
                }
            } else { //(+,-) -> 1st quadrant
                if(((float)coordY)/abs(coordX) > 1.5){//more y than x
                    module2 = compAbs(outComp[i+1][j]);
                } else if(((float)abs(coordX))/coordY > 1.5){//more x than y
                    module2 = compAbs(outComp[i][j+1]);
                } else {
                    module2 = compAbs(outComp[i+1][j+1]);
                }
            }
            //pixel intensity representation
            module2_log = (log10(module2)*100.0)+255;
            if(module2_log < 0){
                module2_log = 0;
            } else if(module2_log > 255){
                module2_log = 255;
            }
            
            //update module sum and increment the number of pixels at distance
            histogram[(int)(distance/1)+1] += module1*(1-rest) + module2*rest;
            histogramPoints[(int)(distance/1)+1]++;
            //save maximum
            if(histogramMaxVar[(int)(distance/1)+1] < module1*(1-rest) + module2*rest){
                histogramMaxVar[(int)(distance/1)+1] = module1*(1-rest) + module2*rest;
            }
            //save minimum
            if(histogramMinVar[(int)(distance/1)+1] > module1*(1-rest) + module2*rest){
                histogramMinVar[(int)(distance/1)+1] = module1*(1-rest) + module2*rest;
            }
            
            //update intensity sum and increment the number of pixels at distance
            if((module1_log > 250 || module2_log > 250) && (coordX * coordY)!=0){//horizontal and vertical ignored
                peaks[(int)(distance/1)+1] += module1_log*(1-rest)+module2_log*rest;
                peaksPoints[(int)(distance/1)+1]++;
            }
            //save maximum
            if(peaksMaxVar[(int)(distance/1)+1] < module1_log*(1-rest) + module2_log*rest){
                peaksMaxVar[(int)(distance/1)+1] = module1_log*(1-rest) + module2_log*rest;
            }
            //save minimum
            if(peaksMinVar[(int)(distance/1)+1] > module1_log*(1-rest) + module2_log*rest){
                peaksMinVar[(int)(distance/1)+1] = module1_log*(1-rest) + module2_log*rest;
            }
            
        } else {
            //update module sum and increment the number of pixels at distance
            histogram[(int)distance] += module1;
            histogramPoints[(int)distance]++;
            //save maximum
            if(histogramMaxVar[(int)distance] < module1){
                histogramMaxVar[(int)distance] = module1;
            }
            //save minimum
            if(histogramMinVar[(int)distance] > module1){
                histogramMinVar[(int)distance] = module1;
            }
            
            //update intensity sum and increment the number of pixels at distance
            if(module1_log > 250 && (coordX * coordY)!=0){//horizontal and vertical ignored
                peaks[(int)distance] += module1_log;
                peaksPoints[(int)distance]++;
            }
            //save maximum
            if(peaksMaxVar[(int)distance] < module1_log){
                peaksMaxVar[(int)distance] = module1_log;
            }
            //save minimum
            if(peaksMinVar[(int)distance] > module1_log){
                peaksMinVar[(int)distance] = module1_log;
            }
        }
        pointCounter++;
      }
    }
    
    //END RADIAL HISTOGRAM
    
    FILE *radialHisto, *radialVarHisto;
    FILE *radialPeaks, *radialVarPeaks;
    char *int_rad, *var_rad, *int_log_rad, *var_log_rad;
    if(verbose){
        // Filename
        //Intensities
        int_rad = strdup(originalFileName);
        int_rad = addExtension(int_rad, "_radialIntHisto.csv");
        //Variations
        var_rad = strdup(originalFileName);
        var_rad = addExtension(var_rad, "_radialVarHisto.csv");

        //Intensities
        int_log_rad = strdup(originalFileName);
        int_log_rad = addExtension(int_log_rad, "_radialLogIntHisto.csv");
        //Variations
        var_log_rad = strdup(originalFileName);
        var_log_rad = addExtension(var_log_rad, "_radialLogVarHisto.csv");

        //CSV open
        radialHisto = fopen(int_rad,"w");
        radialVarHisto = fopen(var_rad,"w");
        radialPeaks = fopen(int_log_rad,"w");//TEST
        radialVarPeaks = fopen(var_log_rad,"w");//TEST
    }
    
    // Maximums and Minimums arrays
    int half_dis = maxDis/2;//worst case is max-min-max-min...
    double local_max[half_dis], local_min[half_dis];//Intensities
    int local_max_dist[half_dis], local_min_dist[half_dis];//distances from origin
    int maxPoints=0, minPoints=0;
    double mul;
    
    //calculate the maximums and minimums of the histogram array
    local_max_min(histogram, histogramPoints, maxDis, local_max, local_min, local_max_dist, local_min_dist, &maxPoints, &minPoints);
    
    //Print histograms
    if(verbose){
        fprintf(radialHisto, "Distance;Points;Avg.Intensity;\n");
        fprintf(radialVarHisto, "Distance;Points;MinVar;MaxVar;Diff;\n");
        fprintf(radialPeaks, "Distance;Points;Avg.Intensity;\n");//TEST
        fprintf(radialVarPeaks, "Distance;Points;MinVar;MaxVar;Diff;\n");//TEST
        for(i=0; i<maxDis; i++){
            fprintf(radialHisto, "%d;%d;%f;\n", i, histogramPoints[i], histogram[i]/histogramPoints[i]);
            fprintf(radialVarHisto, "%d;%d;%f;%f;%f;\n", i, histogramPoints[i], histogramMinVar[i], histogramMaxVar[i], histogramMaxVar[i]-histogramMinVar[i]);
            fprintf(radialPeaks, "%d;%d;%f;\n", i, peaksPoints[i], peaks[i]/peaksPoints[i]);//TEST
            if(peaks[i]>0)  mul = 1.0;
            else            mul = 0.0;
            fprintf(radialVarPeaks, "%d;%d;%f;%f;%f;\n", i, peaksPoints[i], peaksMinVar[i], peaksMaxVar[i], (peaksMaxVar[i]-peaksMinVar[i])*mul);//TEST
        }
    }
    
    // Sorting maximums by intensity registered
    quickSort_mod(local_max, local_max_dist, 0, maxPoints-1);
    
    if(verbose){
        fprintf(radialHisto, "Radius local max;Avg.Intensity;\n");
        for(i=0; i<maxPoints; i++)
            fprintf(radialHisto, "%d;%f;\n",local_max_dist[i],local_max[i]);
        fprintf(radialHisto, "Radius local min;Avg.Intensity;\n");
        for(i=0; i<minPoints; i++)
            fprintf(radialHisto, "%d;%f;\n",local_min_dist[i],local_min[i]);
    
        //Close files
        fclose(radialHisto);
        fclose(radialVarHisto);
        fclose(radialPeaks);//TEST
        fclose(radialVarPeaks);//TEST
    
    /* GNUPLOT*/
        gnuplot(originalFileName, int_rad, var_rad);
        gnuplot_Peaks(originalFileName, int_log_rad, var_log_rad);
    
    /* END GNUPLOT*/
    
        free(int_rad);free(var_rad);
        free(int_log_rad);free(var_log_rad);
    
    }
    
    int dis_min_beg=maxDis, dis_min_end=0;
    
#if 0 //based on local maximums and minimums of intensities
    int firstNmax = 2;
    int dis_max_beg=maxDis, dis_max_end=0;
    
    for(i=0; i<firstNmax; i++){
        if(local_max_dist[i]<dis_max_beg){
            dis_max_beg = local_max_dist[i];
        }
        if(local_max_dist[i]>dis_max_end){
            dis_max_end = local_max_dist[i];
        }
    }
    
    printf("Maximum begin:%d Maximum end:%d\n", dis_max_beg, dis_max_end);
    
    for(i=5; i<maxDis; i++){
        if(local_min_dist[i] < dis_max_beg){
            dis_min_beg = local_min_dist[i];
        }
        if(local_min_dist[i] > dis_max_end){
            dis_min_end = local_min_dist[i+3];
            break;
        }
    }
    
#else //If based on peaks
    
    char start = 0;//only start counting after have passed the center bright region
    int int_trg = 0.8*255;//select peaks within 30% from maximum variation(255)
    
    for(i=0; i<maxDis; i++){
        if(peaks[i] > 0 && (peaksMaxVar[i]-peaksMinVar[i]) > int_trg){
            if(i<dis_min_beg && start){
                dis_min_beg = i;
            }
            if(i>dis_min_end && start){
                dis_min_end = i;
            }
        } else {
            start = 1;
        }
    }
#endif
    
    printf("Minimum begin:%d Minimum end:%d\n", dis_min_beg, dis_min_end);
    
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
    
    //Pass Band Filter (everything outside dis_min_beg and dis_min_end is erased)
    for(i=0;i<height;i++) {
        for(j=0;j<width;j++) {
            coordY = height_half - ((i + height / 2) % height);
            coordX = width_half - ((j + width / 2) % width);
            distance = sqrt(pow(coordX,2) + pow(coordY,2));
            if(distance<dis_min_beg-(0.2*dis_min_beg) || distance>dis_min_end+(0.2*dis_min_beg)){
                outComp[i][j].Re = 0;
                outComp[i][j].Im = 0;
            }
        }
    }
    
    fprintf(stdout, "inverseFourier...\n");fflush(stdout);
    //return to space domain
    inverseFourier(aux->image, outComp, aux->height);
    
    //filename
    aux->fileName = addExtension(aux->fileName, "_inverse.tif");
    
    //print result
    if(verbose){
        fprintf(stdout, "print result...\n");fflush(stdout);
        writeTiffImage(aux->fileName,aux);
    }
    
/* END TEST FOURIER FILTERING */
    if(verbose){
        //Get fourier spectrum of the frequency domain
        fourierSpectrumImage(aux->image, outComp, aux->height);

        //filename
        aux->fileName = addExtension(aux->fileName, "_filtered.tif");
        
        //Output spectrum
        writeTiffImage(aux->fileName,aux);
    
        inverseFourier(aux->image, outComp, aux->height);
    }
    
    fprintf(stdout, "Done\n");fflush(stdout);
/* END FOURIER*/
    
    //BEGIN TEST
    int levels = exp2(8);
    //initialize statistics
    for(i=0; i<levels; i++){
        aux->histogram[i] = 0;
    }
    aux->minimum=255; aux->maximum=0;
    //update statistics
    for(i=0; i<aux->height; i++){
        createStatistics(aux->image[i], aux->width, &aux->maximum, &aux->minimum, aux->histogram);
    }
    aux->median  = getMedian(aux->histogram, levels, aux->height*aux->width);
    aux->average = getAverage(aux->histogram, levels, aux->height*aux->width);
    //END TEST
    
    //RegionLL regionList = NULL;
    //otsu's automatic binarization method
    int trg = getOtsuThreshold(aux->histogram, 0, aux->height, 0, aux->width);
    
#if 0 /* BEGIN AUTOMATIC TRESHOLD ALGORITHM */
    
    fprintf(stdout, "Automatic threshold algorithm...\n");fflush(stdout);
    
    TiffImage aux_temp = NULL;
    int prev_regionCount=0, actual_regionCount=0;
    int decr_regionCount_max=0; int decr_max_Threshold;
    int actual_trg;
    int prev_trg;
    double increment = 1.0, decrement = 1.0;
    int iterations = 0;
    char decr = 1;//start decrementing before incrementing
    
    decr_max_Threshold = actual_trg = prev_trg = trg;
    
    while(prev_regionCount<=actual_regionCount){
        
        fprintf(stdout, "Finding optimal threshold - Iteration:%d\n", iterations++);fflush(stdout);
    
        //if not first iteration delete modified image
        if(aux_temp != NULL){
            destroyTiffImage(aux_temp);
        }
        
        //duplicate filtered image
        aux_temp = cloneTiffImage(aux);
        
        fprintf(stdout, "Finding optimal threshold - Threshold:%d\n", actual_trg);
        //BINARIZING
        binImage8bit(aux_temp, actual_trg);

        //CLOSING
        aux_temp = closing(aux_temp);

        //validation
        if(aux_temp == NULL || image == NULL){
            fprintf(stdout, "Something went wrong (images validation)\n");fflush(stdout);
            goto error;
        }

        //REGION DETECTION
        //save regions on both images:
        //  image-> for mask application
        //  aux  -> for further calculations
        image->listRegions = aux_temp->listRegions = findRegions(aux_temp);
        if(aux_temp->listRegions == NULL){
            fprintf(stdout, "Something went wrong (finding regions)\n");fflush(stdout);
            goto error;
        }
        
        //update counters for next iteration
        prev_regionCount = actual_regionCount;
        
        //count regions found
        actual_regionCount = image->pointCount = aux_temp->pointCount  = regionCount(aux_temp->listRegions);
        
        fprintf(stdout, "Finding optimal threshold - Previous region count:%d new region count:%d\n", prev_regionCount, actual_regionCount);
        
        //if better before and still left to increment
        if(prev_regionCount > actual_regionCount){
            if(decr){
                fprintf(stdout, "FLIP:%d\n", trg);
                //state increment only
                decr = 0;
                //save maximum number of regions for decrementing
                decr_regionCount_max = prev_regionCount;
                //save threshold for decrementing
                decr_max_Threshold = prev_trg;
                //reset counters
                prev_regionCount = 0; actual_regionCount = 0;
                actual_trg = trg;
            }
        } else {
            //save threshold for maximum regions count
            prev_trg = actual_trg;
        }

        //10% decrement/increment at each iteration
        if(decr){
            decrement *= 0.9;
            actual_trg *= decrement;
        } else {
            increment *= 1.1;
            actual_trg *= increment;
        }
        
    }
    
    //if decrementing threshold gives more regions, update variables
    if(decr_regionCount_max > prev_regionCount){
        prev_regionCount = decr_regionCount_max;
        prev_trg = decr_max_Threshold;
    }
    
    fprintf(stdout, "Finding optimal threshold Results - Threshold:%d Number of regions:%d \n", prev_trg, prev_regionCount);
    trg = prev_trg;
    
    if(aux_temp != NULL){
        destroyTiffImage(aux_temp);
    }
    
    fprintf(stdout, "Done\n");fflush(stdout);
    
#endif /* END AUTOMATIC TRESHOLD ALGORITHM */
    
/* BEGIN BINARIZING */
    fprintf(stdout, "Binarizing...\n");fflush(stdout);
    
    fprintf(stdout, "Threshold:%d\n", trg);
    aux = binImage8bit(aux, trg);

    //filename
    aux->fileName = addExtension(aux->fileName, "_binarized.tif");
    if(verbose){
        writeTiffImage(aux->fileName,aux);
    }

/* BEGIN CLOSING*/
    aux = closing(aux);

    //filename
    aux->fileName = addExtension(aux->fileName, "_closed.tif");    
    if(verbose){
        writeTiffImage(aux->fileName,aux);
    }

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
    //save regions on both images:
    //  image-> for mask application
    //  aux  -> for further calculations
    /*regionList = */image->listRegions = aux->listRegions = findRegions(aux);
    if(aux->listRegions == NULL){
        fprintf(stdout, "Something went wrong (finding regions)\n");fflush(stdout);
        goto error;
    }
    
    image->pointCount = aux->pointCount = regionCount(aux->listRegions);
    
    /*
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
        
    } else {
        fprintf(stderr, "No list to present!\n");fflush(stdout);
        goto error;
    }
*/
    //show centroid for each region in the result image
    showCentroid(aux, aux->listRegions);

    //filename
    aux->fileName = addExtension(aux->fileName, "_centroid.tif");

    if(verbose){
        writeTiffImage(aux->fileName, aux);
    }
    
/* END REGION DETECTION*/
    
/* BEGIN APPLYING MASK */
    
    fprintf(stdout, "Applying mask...\n");fflush(stdout);
    
    //get minimum distance between Centroids efficiently
    //getWDim(aux);//->Does not work (something wrong)
    
    //get minimum distance between Centroids BRUTE FORCE!
    int wdim = getDistances(aux);
    
    fprintf(stdout, "First wdim:%d\n",wdim);
    
    //Apply mask to original image (corrects centroids for maximum region intensity->could override regions!)
    TiffImage masked = aplyMask(image, wdim/2);
    
    //get minimum distance between (corrected) Centroids
    wdim = getDistances(masked);
    
    //(BUG) reset filename
    free(image->fileName);image->fileName=strdup(originalFileName);
    
    //Apply mask to original image (with override safe mask))
    masked = aplyMask(image, wdim/2);
    
    //filename
    aux->fileName = addExtension(aux->fileName, "_masked.tif");
    
    if(verbose){
        writeTiffImage(aux->fileName, masked);
    }
    
    fprintf(stdout, "Done Final wdim:%d\n",wdim);
    
/* END APPLYING MASK */
    
    //clean up
    destroyTiffImage(image);
    //destroyTiffImage(contrasted);
    destroyTiffImage(aux);
    
    //Stop timer
    GET_TIME(finishTime);
    fprintf(stdout,"The code to be timed took %f seconds\n", finishTime - startTime);
    
    return 0;

//error handling
error:
    fprintf(stderr,"[MAIN] An error occurred\n");
    return -1;
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

int local_max_min(double* histogram, int* histogramPoints, int size, double* local_max, double* local_min, int* local_max_dist, int* local_min_dist, int* maxPoints, int* minPoints){
    int i;
    
    //Counters
    *maxPoints=0; *minPoints=0;
    //temporary variables
    double actual_avgInt, prev_avgInt;
    //Flags
    char inc=0;//is it incrementing?
    char infMax=0;//local maximum -> inflection Max
    char infMin=0;//local minimum -> inflection Min
    
    //init not necessary because there are counters which indicate the last array position occupied
    
    for(i=0; i<size; i++){
        actual_avgInt = histogram[i]/histogramPoints[i];
        
        //first iteration does not have prev_avgInt initialized (no history)
        if(i>0){
            
            //if starts to decrement and previous iteration was incrementing
            if(actual_avgInt<prev_avgInt && inc==1){
                infMax = 1;//declare point of inflection
            } else {
                infMax = 0;
            }
            
            //if starts to increment and previous iteration was decrementing
            if(actual_avgInt>prev_avgInt && inc==0){
                infMin = 1;//declare point of inflection
            } else {
                infMin = 0;
            }
            
            //update increment flag
            inc = (actual_avgInt>prev_avgInt)?1:0;//is it incrementing?1-Yes 0-No
            
            //if local maximum -> store in array
            if(infMax){
                local_max[*maxPoints] = prev_avgInt;
                local_max_dist[*maxPoints] = i-1;
                (*maxPoints)++;
            }
            
            //if local minimum -> store in array
            if(infMin){
                local_min[*minPoints]=prev_avgInt;
                local_min_dist[*minPoints] = i-1;
                (*minPoints)++;
            }
        }
        prev_avgInt = actual_avgInt;
    }
    return 0;
}

void gnuplot(char* originalFileName, char* int_rad, char* var_rad){
    
    //filename
    //Intensities
    char* int_gnu = strdup(originalFileName);
    int_gnu = addExtension(int_gnu, "_plot_int.png");
    //Variation
    char* var_gnu = strdup(originalFileName);
    var_gnu = addExtension(var_gnu, "_plot_int_var.png");
    
    FILE *pipe;
    pipe = popen( "gnuplot -persist","w");
    fprintf(pipe, "set terminal png transparent nocrop enhanced size 1920,800 font \"arial,16\"\n");
    fprintf(pipe, "set output '%s'\n", int_gnu);
    fprintf(pipe, "set style fill   solid 1.00 border lt -1\n");
    fprintf(pipe, "set datafile separator \";\"\n");
    fprintf(pipe, "set autoscale xfix\n");
    fprintf(pipe, "set xtics auto\n");
    fprintf(pipe, "set ytics auto\n");
    fprintf(pipe, "set title \"Fourier transform - radial module analysis\" \n");
    fprintf(pipe, "set xrange [ 0.00000 : ]\n");
    fprintf(pipe, "set xlabel \"Distance (px)\"\n");
    fprintf(pipe, "set yrange [ 0.00000 : 0.800 ] noreverse nowriteback\n");
    fprintf(pipe, "set ylabel \"Module Avg.\"\n");
    fprintf(pipe, "plot \"%s\" every ::1::724 using 3 with boxes\n", int_rad);
    pclose(pipe);
    
    pipe = popen( "gnuplot -persist","w");
    fprintf(pipe, "set terminal png transparent nocrop enhanced size 1920,800 font \"arial,16\"\n");
    fprintf(pipe, "set output '%s'\n", var_gnu);
    fprintf(pipe, "set style fill   solid 1.00 border lt -1\n");
    fprintf(pipe, "set datafile separator \";\"\n");
    fprintf(pipe, "set autoscale xfix\n");
    fprintf(pipe, "set xtics auto\n");
    fprintf(pipe, "set ytics auto\n");
    fprintf(pipe, "set title \"Fourier transform - radial module variation analysis\" \n");
    fprintf(pipe, "set xrange [ 0.00000 : ]\n");
    fprintf(pipe, "set xlabel \"Distance (px)\"\n");
    fprintf(pipe, "set yrange [ 0.00000 : 7.000 ] noreverse nowriteback\n");
    fprintf(pipe, "set ylabel \"Module variation\"\n");
    fprintf(pipe, "plot \"%s\" every ::1::724 using 5 with boxes\n", var_rad);
    pclose(pipe);
    
    free(int_gnu);
    free(var_gnu);
}

void gnuplot_Peaks(char* originalFileName, char* int_log_rad, char* var_log_rad){
    
    //filename
    //Intensities
    char* int_log_gnu = strdup(originalFileName);
    int_log_gnu = addExtension(int_log_gnu, "_plot_peak_int.png");
    //Variation
    char* var_log_gnu = strdup(originalFileName);
    var_log_gnu = addExtension(var_log_gnu, "_plot_peak_int_var.png");
    
    FILE *pipe;
    pipe = popen( "gnuplot -persist","w");
    fprintf(pipe, "set terminal png transparent nocrop enhanced size 1920,800 font \"arial,16\"\n");
    fprintf(pipe, "set output '%s'\n", int_log_gnu);
    fprintf(pipe, "set style fill   solid 1.00 border lt -1\n");
    fprintf(pipe, "set datafile separator \";\"\n");
    fprintf(pipe, "set autoscale xfix\n");
    fprintf(pipe, "set xtics auto\n");
    fprintf(pipe, "set ytics auto\n");
    fprintf(pipe, "set title \"Fourier transform - Peaks intensities analysis\" \n");
    fprintf(pipe, "set xrange [ 0.00000 : ]\n");
    fprintf(pipe, "set xlabel \"Distance (px)\"\n");
    fprintf(pipe, "set yrange [ 0.00000 : ] noreverse nowriteback\n");
    fprintf(pipe, "set ylabel \"Intensity Avg\"\n");
    fprintf(pipe, "plot \"%s\" every ::1::724 using 3 with boxes\n", int_log_rad);
    pclose(pipe);
    
    pipe = popen( "gnuplot -persist","w");
    fprintf(pipe, "set terminal png transparent nocrop enhanced size 1920,800 font \"arial,16\"\n");
    fprintf(pipe, "set output '%s'\n", var_log_gnu);
    fprintf(pipe, "set style fill   solid 1.00 border lt -1\n");
    fprintf(pipe, "set datafile separator \";\"\n");
    fprintf(pipe, "set autoscale xfix\n");
    fprintf(pipe, "set xtics auto\n");
    fprintf(pipe, "set ytics auto\n");
    fprintf(pipe, "set title \"Fourier transform - Peaks intensities variation analysis\" \n");
    fprintf(pipe, "set xrange [ 0.00000 : ]\n");
    fprintf(pipe, "set xlabel \"Distance (px)\"\n");
    fprintf(pipe, "set yrange [ 0.00000 : ] noreverse nowriteback\n");
    fprintf(pipe, "set ylabel \"Intensity variation\"\n");
    fprintf(pipe, "plot \"%s\" every ::1::724 using 5 with boxes\n", var_log_rad);
    pclose(pipe);
    
    free(int_log_gnu);
    free(var_log_gnu);
}

void gnuplot_histogram(char* outputFileName, char dataFileName[]){
    
    FILE *pipe;
    pipe = popen( "gnuplot -persist","w");
    fprintf(pipe, "set terminal png transparent nocrop enhanced size 1920,800 font \"arial,16\"\n");
    fprintf(pipe, "set output '%s'\n", outputFileName);
    fprintf(pipe, "set style fill   solid 1.00 border lt -1\n");
    fprintf(pipe, "set datafile separator \";\"\n");
    fprintf(pipe, "set autoscale xfix\n");
    fprintf(pipe, "set xtics auto\n");
    fprintf(pipe, "set ytics auto\n");
    fprintf(pipe, "set title \"Image histogram\" \n");
    fprintf(pipe, "set xrange [ 0.00000 : ]\n");
    fprintf(pipe, "set xlabel \"Intensity\"\n");
    fprintf(pipe, "set yrange [ 0.00000 : ] noreverse nowriteback\n");
    fprintf(pipe, "set ylabel \"Counter\"\n");
    fprintf(pipe, "plot \"%s\" every ::1::255 using 2 with boxes\n", dataFileName);
    pclose(pipe);
}

void gnuplot_histogram_test(char* outputFileName, char dataFileName[]){
    
    FILE *pipe;
    pipe = popen( "gnuplot -persist","w");
    fprintf(pipe, "set terminal png transparent nocrop enhanced size 1920,800 font \"arial,16\"\n");
    fprintf(pipe, "set output '%s'\n", outputFileName);
    fprintf(pipe, "set style fill   solid 1.00 border lt -1\n");
    fprintf(pipe, "set datafile separator \";\"\n");
    fprintf(pipe, "set autoscale xfix\n");
    fprintf(pipe, "set xtics auto\n");
    fprintf(pipe, "set ytics auto\n");
    fprintf(pipe, "set title \"Image peak modules histogram\" \n");
    fprintf(pipe, "set xrange [ 0.00000 : ]\n");
    fprintf(pipe, "set xlabel \"Intensity\"\n");
    fprintf(pipe, "set yrange [ 0.00000 : ] noreverse nowriteback\n");
    fprintf(pipe, "set ylabel \"Counter\"\n");
    fprintf(pipe, "plot \"%s\" every ::1::724 using 3 with boxes\n", dataFileName);
    pclose(pipe);
}

#endif