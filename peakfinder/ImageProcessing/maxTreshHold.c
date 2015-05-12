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
    Point*      auxPoint      = NULL;
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