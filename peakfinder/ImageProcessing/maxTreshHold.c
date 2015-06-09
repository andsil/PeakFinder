#include "maxTreshHold.h"

/* LOCAL PROTOTYPES */
char alreadyExists(Region **neighbors, int pointCount, int id);

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
    Region*     auxRegion     = NULL;
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
    //#pragma omp parallel for default(shared) private(j, sp, column, row, auxPoint, auxPointList, auxRegion)
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

char alreadyExists(Region **neighbors, int pointCount, int id){
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
    Region  *point;
    double  minDistance[pointCount];//Minimum
    
    //Auxiliary variables
    int     auxPointID, id;
    
    double  distanceX, distanceY, distance, distanceMin, distanceMax;

    //validation
    if(!img->listRegions || !img->listRegions->regions){
        fprintf(stderr, "Error: No region list\n");
        return -1;
    }
    
    //for each region register the ID
    point = img->listRegions->regions;
    
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
            distanceX = fabs(point[auxPointID].centroid.x - point[id].centroid.x);
            distanceY = fabs(point[auxPointID].centroid.y - point[id].centroid.y);
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


//BRUTE FORCE -> NOT anymore
//based on incremental radius
int getDistancesV2(TiffImage img){
    //validation
    if(!img || !(img->listRegions)){
        fprintf(stderr, "Warning: An error occurred\n");
        return -1;
    }
    
    //variables
    int pointCount  = img->pointCount;
    int width   = img->width;
    int height  = img->height;
    int i,ii,j,jj;
    
    //Region array to store the pointers for each region mapped as coordinates.
    //Array indexed by pointID contains the pointer for the region.
    Region  *regionLocs[height][width];
    double  minDistance[pointCount];//Minimum
    
    //Auxiliary variables
    PointCoord  cent;//, minimumCandidate;
    Region      *auxR, *auxR2;
    int         id, auxX, auxY;
    int         coordX, coordY, r, d;
    int         height_half = height/2;
    
    //init
    for(i=0; i<height; i++){
        for(j=0; j<width; j++){
            regionLocs[i][j] = NULL;
        }
    }
    
    double distance, distanceCandidate, distanceMin, distanceMax, distanceX, distanceY;
    
    if(!img->listRegions || !img->listRegions->regions){
        fprintf(stderr, "Error: No region list\n");
        return -1;
    }
    
    //for each region register the ID
    for(i=0; i<img->listRegions->lenght; i++){
            cent = img->listRegions->regions[i].centroid;

            //insert pointer to original region in centroid coordinates
            auxX=(int)round(cent.x);
            auxY=(int)round(cent.y);
            if(auxY<0 || auxX<0 || auxY>=height || auxX>=width){
                fprintf(stderr, "WARNING: [GetDistance] Coordinates out of bounds\n");
            } else {
                regionLocs[auxY][auxX] = &img->listRegions->regions[i];
            }
    }
    
    //init
    distanceMin = -1;
    distanceMax = -1;
    for(id=0; id<=pointCount; id++){
        minDistance[id] = -1;
    }
    
    //for all points -> could be minimized?
    #pragma omp parallel for default(shared) private(j,distanceCandidate, auxR, r, d, ii, jj, coordX, coordY, auxR2, distanceX, distanceY, distance)
    for(i=0; i<height; i++){
        for(j=0; j<width; j++){
            //init
            distanceCandidate = -1;
            
            //if there is a point in this coordinates
            auxR = regionLocs[i][j];
            if(auxR!=NULL){
                
                //start radius = 1 and increment until diameter equals image height
                for(r=3; r<height_half; r++){
                    
                    //diameter
                    d = 2*r;
                    
                    //for all coordinates in between, see if there is another centroid
                    //should NOT enter for inner points already parsed!!!!!
                    //only process first and last rows/columns
                    for(ii=0; ii<d; ii++){
                    //for(ii=0; ii<=1; ii++){
                        for(jj=0; jj<d; jj++){
                        //for(jj=0; jj<=1; jj++){
                            
                            //get absolute coordinates
                            coordY = i+ii-r;
                            coordX = j+jj-r;
                            //NOTE: diameter increments by 2 units each iteration
                            //so, if ii/jj is 0, then is the first row/column
                            //if ii/jj is 1, then is the last row/column
                            //the inner points had already been processed
                            //coordY = i+(ii*d)-r;
                            //coordX = j+(jj*d)-r;
                            
                            //if out of image area CONTINUE for next iteration
                            if(coordY<0 || coordX<0 || coordY>=height || coordX>=width || (coordY==i && coordX==j)) continue;
                            
                            //if exists a centroid
                            auxR2 = regionLocs[coordY][coordX];
                            if(auxR2 != NULL){
                                //if inside, get distance
                                distanceX = fabs(auxR->centroid.x - auxR2->centroid.x);
                                distanceY = fabs(auxR->centroid.y - auxR2->centroid.y);
                                distance = sqrt(pow(distanceX,2)+pow(distanceY,2));
                                //checks if is better then any previously found
                                if(distanceCandidate < 0 || distance < distanceCandidate){
                                    distanceCandidate = distance;
                                    //minimumCandidate.x = coordX;
                                    //minimumCandidate.y = coordY;
                                }
                            }
                            
                        }
                    }
                    
                    //if there is a candidate closer then current radius
                    if(distanceCandidate > 0 && distanceCandidate < r){
                        //save & stop
                        minDistance[auxR->id-1] = distanceCandidate;
                        break;
                    }
                }
            }
            
        }
    }
    
    for(id=0; id<pointCount; id++){
        //if points distance less than 5, skip minimum distance
        //if((distanceMin > minDistance[id] && minDistance[id] > 5) || distanceMin<0){
        if((distanceMin > minDistance[id] || distanceMin<0) && minDistance[id] > 5){
            //printf("[%d] %f\n", id, minDistance[id]);
            distanceMin = minDistance[id];
        }
        if(distanceMax < minDistance[id]){
            distanceMax = minDistance[id];
        }
    }
    return ((int)ceil(distanceMin));
}