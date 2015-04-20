#include "maxTreshHold.h"

/* LOCAL PROTOTYPES */
char alreadyExists(Region *neighbors, int pointCount, int id);

/**
 * Auxiliary functions that returns the maximum of two points
 */
int maxOf(int a, int b){
    return (a>b)? a : b;
}

/**
 * Go through the image pixels and aggregates them in Regions
 */
RegionLL findRegions(TiffImage img){
    //variables
    Point       auxPoint      = NULL;
    PointLL     auxPointList  = NULL;
    Region      auxRegion     = NULL;
    RegionLL    resRegionList = NULL;
    int i, j, width, height, column, row;
    char** mark;
    
    //validation
    if(img == NULL){
        goto error;
    }
    
    {//goto error workaround
        
    width  = img->width;
    height = img->height;
    //auxiliary array for the pixels status
    mark = (char**)malloc(sizeof(char*)*height);
    for(i=0; i<height; i++){
        //initialization to 0 -> NOTVISITED
        mark[i] = (char*)calloc(width, sizeof(char));
    }
    
    //LIFO queue
    int sp = 0;
    /* UNECESSARY SPACE ALOCATED!!!!! COULD BE OPTIMIZED */
    PointCoord queueStack[width*2+height*2];
    //ignores the first and last 6 pixels
    for(i=6; i<height-6; i++){
        for(j=6; j<width-6; j++){
            //check if this is a white point and was not visited yet
            if(img->image[i][j] == WHITE && mark[i][j] == NOTVISITED){
                
                //check surroundings (clockwise) for WHITE Points
                if(isInside(i-1, j, height, width)){ //UP
                    sp = addQueue(img->image, mark, queueStack, sp, i, j);
                }
                if(isInside(i, j+1, height, width)){ //RIGHT
                    sp = addQueue(img->image, mark, queueStack, sp, i, j);
                }
                if(isInside(i+1, j, height, width)){ //DOWN
                    sp = addQueue(img->image, mark, queueStack, sp, i, j);
                }
                if(isInside(i, j-1, height, width)){ //LEFT
                    sp = addQueue(img->image, mark, queueStack, sp, i, j);
                }
                
                //Add Points to the stack and then to the point list -> LIFO
                while(sp > 0){
                    column  = queueStack[sp-1].x;
                    row     = queueStack[sp-1].y;
                    //POP
                    sp--;
                    
                    //create Point in heap memory
                    auxPoint = createNewPoint(column, row, 255);
                    //add point to the list of points;
                    auxPointList = addPointLLEntry(auxPointList, auxPoint);
                    
                    //check surroundings (clockwise) for WHITE Points
                    if(isInside(row-1, column, height, width)){ //UP
                        sp = addQueue(img->image, mark, queueStack, sp, row-1, column);
                    }
                    if(isInside(row, column+1, height, width)){ //RIGHT
                        sp = addQueue(img->image, mark, queueStack, sp, row, column+1);
                    }
                    if(isInside(row+1, column, height, width)){ //DOWN
                        sp = addQueue(img->image, mark, queueStack, sp, row+1, column);
                    }
                    if(isInside(row, column-1, height, width)){ //LEFT
                        sp = addQueue(img->image, mark, queueStack, sp, row, column-1);
                    }
                    
                    //mark current position as visited
                    mark[row][column] = VISITED;
                }
                
                //check if the previous loop was executed
                if(auxPointList != NULL){
                    //create a region
                    auxRegion = createNewRegion(auxPointList, img->width, img->height);
                    
                    if(auxRegion != NULL){
                        //add the region to the current list of regions
                        resRegionList = addRegionLLEntry(resRegionList, auxRegion);
                    } else {
                        //if failed free memory
                        remAllPointLL(auxPointList);
                    }
                    
                    //reset pointer to Point list for the next iteration;
                    auxPointList  = NULL;
                }
            }
            
            //mark current position as visited
            mark[i][j] = VISITED;
        }
    }
    
    }//end of goto error workaround
    
    return resRegionList;
    
error:
    fprintf(stderr, "An error occurred -> image = NULL\n");
    return NULL;
}

/**
 * Checks if the point (j,i) -> (x,y) is white. If it is add it to the queue.
 * Returns the new stack pointer if there were changes.  is.
 */
int addQueue(uint8** img, char** mark, PointCoord* queueStack, int sp, int i, int j){
    //only not Visited have interest
    if(mark[i][j] == NOTVISITED){
        //check if is a white point
        if(img[i][j] == WHITE){
            //add to queue
            mark[i][j] = QUEUED;
            //insert coordinates in the queue
            queueStack[sp].y = i;
            queueStack[sp].x = j;
            //increment stack pointer -> PUSH
            sp += 1;
        }
    }
    
    return sp;
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
        //if points distance less than 5, skip minimum distance
        if((distanceMin > minDistance[id] && minDistance[id] > 5) || distanceMin<0){
            distanceMin = minDistance[id];
        }
        if(distanceMax < minDistance[id]){
            distanceMax = minDistance[id];
        }
    }
    
    //fprintf(stdout, "Min:%.3f Max:%.3f Percentage:%.3f\n", distanceMin, distanceMax, (distanceMax-distanceMin)/distanceMin*100);
    return ((int)round(distanceMin));
}