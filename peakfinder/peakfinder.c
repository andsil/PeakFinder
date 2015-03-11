#include <stdio.h>//fgets, fprintf, etc
#include <stdlib.h>//realloc
#include <string.h>//strchr
//#include <inttypes.h>//PRId32
//#include <unistd.h>//usleep

#include "readTiff.h"//readTiffImage
#include "writeTiff.h"//writeTiffImage
#include "contrast.h"//histogramEqualization
#include "binary.h"//binImage8bit...
#include "auxFunc.h"
#include "RegionLL.h"
#include "maxTreshHold.h" //findRegion, imageBinarization
#include "mask.h"


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
    
    if(argc<2){//iterative
        //printf("Put the path to File:");
        inputFileName = "oct_simul.tiff";//readline(stdout);
    } else {//automatic
        inputFileName = argv[1];
    }
    
    if(!inputFileName){
        goto error;
    }
    
    //Disable libtiff warnings
    setTiffWarningsOff();

    //Read Image
    TiffImage image = readTiffImage(inputFileName);
    
    //validation
    if(!image){
        goto error;
    }
    
    //test detail data
    printf("ndirs=%d ,"
            "fileName=%s ,"
            "width=%d ,"
            "height=%u ,"
            "config=%u ,"
            "fillOrder=%d ,"
            "nSamples=%u ,"
            "depth=%u ,"
            "photometric=%u ,"
            "resUnit=%u ,"
            "xRes=%f ,"
            "yRes=%f ,"
            "maximum=%u ,"
            "minimum=%u ,"
            "median=%u,"
            "average=%u\n",
            image->ndirs,
            image->fileName,
            image->width,
            image->height,
            image->config,
            image->fillOrder,
            image->nSamples,
            image->depth,
            image->photometric,
            image->resUnit,
            image->xRes,
            image->yRes,
            image->maximum,
            image->minimum,
            image->median,
            image->average);
    
    FILE* out = fopen("outMatrix.txt", "w");
    
    //view image intensity
    int width = image->width, height = image->height;
    printIntMatrix(out, image->image, width, height);
    fprintf(out, "\n\n\n");
    
    //contrast image
    TiffImage contrasted = histogramEqualization(image);
    
    printf("ndirs=%d ,"
            "fileName=%s ,"
            "width=%d ,"
            "height=%u ,"
            "config=%u ,"
            "fillOrder=%d ,"
            "nSamples=%u ,"
            "depth=%u ,"
            "photometric=%u ,"
            "resUnit=%u ,"
            "xRes=%f ,"
            "yRes=%f ,"
            "maximum=%u ,"
            "minimum=%u ,"
            "median=%u,"
            "average=%u\n",
            contrasted->ndirs,
            contrasted->fileName,
            contrasted->width,
            contrasted->height,
            contrasted->config,
            contrasted->fillOrder,
            contrasted->nSamples,
            contrasted->depth,
            contrasted->photometric,
            contrasted->resUnit,
            contrasted->xRes,
            contrasted->yRes,
            contrasted->maximum,
            contrasted->minimum,
            contrasted->median,
            contrasted->average);
    
    //view image intensity
    width = contrasted->width; height = contrasted->height;
    printIntMatrix(out, contrasted->image, width, height);
    
    //Write Image
    fprintf(stdout, "Writing Image\n");fflush(stdout);//usleep(500000);//500ms
    int res = writeTiffImage(contrasted->fileName, contrasted);
    
    //->TEST!!!
    TiffImage aux;
    //aux = cloneTiffImage(contrasted);res = writeTiffImage("1.mediana.tiff", binImage8bitAutoMedian(aux));free(aux);
    //aux = cloneTiffImage(contrasted);res = writeTiffImage("2.media.tiff", binImage8bitAutoAverage(aux));free(aux);
    //aux = cloneTiffImage(contrasted);res = writeTiffImage("3.estatico127.tiff", binImage8bitStaticHalf(aux));free(aux);
    //aux = cloneTiffImage(contrasted);res = writeTiffImage("4.estatico70.tiff", binImage8bitStatic(aux, 0.7));free(aux);
    //aux = cloneTiffImage(contrasted);res = writeTiffImage("5.dinamicoMetade.tiff", binImage8bitDynamicHalf(aux));free(aux);
    //aux = cloneTiffImage(contrasted);res = writeTiffImage("6.dinamico70.tiff", binImage8bitDynamic(aux, 0.7));free(aux);
    
    aux = cloneTiffImage(contrasted);
    binImage8bitStatic(aux, 0.684);
    image->listRegions = aux->listRegions = findRegions(aux);
    image->pointCount  = aux->pointCount  = regionCount(aux->listRegions);
    //Heavy calculations!!! Should be avoided!
    //aux = imageBinarization(aux, 0.6, 100);//MEMORY LEAK! THERE ARE 2 IMAGE-> 'AUX' ARG AND 'AUX' RETURN -> 'AUX' ARG POINTER IS LOST WITHOUT MEMORY RELEASE!
    if(aux != NULL){
        fprintf(stdout, "OK\n");
    } else {
        fprintf(stdout, "FAIL\n");
    }
    RegionLL regionList = aux->listRegions;
    if(regionList != NULL){
        RegionLL auxRL = getLastRegionEntry(regionList);
        if(auxRL)
        fprintf(stdout, "SUCCESS: there are %d regions\n" , auxRL->id);
        Region auxReg = auxRL->region;
        if(!auxReg){
            fprintf(stderr, "No Region to present!\n");
        } else {
            fprintf(stdout, "Last Region starts at (%f, %f) end at (%f,%f) Pixels:%d minVal:%u maxVal:%u Centroid (%.3f,%.3f)\n" ,
                    auxReg->coordXBeg, auxReg->coordYBeg, auxReg->coordXEnd, auxReg->coordYEnd,
                    auxReg->pointCount, auxReg->minValue, auxReg->maxValue, auxReg->centroid.x, auxReg->centroid.y);
        }
        
        //show centroid for each region in the result image
        showCentroid(aux, regionList);
    } else {
        fprintf(stderr, "No list to present!\n");
    }
    fprintf(stdout, "BEGIN\n");
    int wdim = getDistances(aux);
    TiffImage masked = aplyMask(image, wdim/2);
    //getWDim(aux);//->Does not work (something wrong)
    fprintf(stdout, "END wdim:%d\n",wdim);
    res = writeTiffImage("estatico70WithCentroid.tiff", aux);
    res = writeTiffImage("Mascarado.tiff", masked);
    free(aux);
    //->END TEST!!!
    
    //validation
    if(res != 0){
        goto error;
    }
    
    //clean up
    destroyTiffImage(image);
    destroyTiffImage(contrasted);
    fflush(out);
    fclose(out);
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
            fprintf(fd, "%03u ", matrix[i][j]);
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
        img->image[(int)round(auxR->centroid.y)][(int)round(auxR->centroid.x)] = 127;
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