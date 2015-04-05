#include "maxTreshHold.h"

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
    for(i=0; i<height; i++){
        for(j=0; j<width; j++){
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
                    auxRegion = createNewRegion(auxPointList);
                    
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