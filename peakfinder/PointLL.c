#include "PointLL.h"

/**
 * SHOULD NOT BE USED OUTSIDE! -> Not declared in header file
 */

PointLL newPointLL(){
    PointLL res;
    
    if(!(res = (PointLL)malloc(sizeof(struct sPointLL)))){
        goto error;
    }
    
    res->id        = 0;
    res->nextPoint = NULL;
    res->prevPoint = NULL;
    res->point     = NULL;
    
    return res;
    
error:
    fprintf(stderr, "ERROR:An error occurred -> no memory?\n");
    return NULL;    
}

/** CONSTRUCTOR **/

PointLL createNewPointLL(Point point){
    PointLL res;
    
    if(!(res = newPointLL())){
        goto error;
    }
    
    //add first element
    res->id    = 1;
    res->point = point;
    
    return res;
    
error:
    return NULL;   
}

/** INSERT **/

PointLL addPointLLEntry(PointLL list, Point addPoint){
    PointLL lastPoint, newEntry;

    if(!addPoint){
        return list;
    }

    if(!list){
        return createNewPointLL(addPoint);
    }

    if(!(newEntry = newPointLL())){
        goto error;
    }

    if(!(lastPoint = getLastPointEntry(list))){
        goto error;
    }

    newEntry->id        = lastPoint->id + 1;
    newEntry->point     = addPoint;
    newEntry->prevPoint = lastPoint;
    newEntry->nextPoint = NULL;

    lastPoint->nextPoint= newEntry;

    return list;

error:
    fprintf(stderr, "ERROR:An error occurred\n");
    return NULL;
}

/** REMOVE **/

PointLL remPointLLEntry(PointLL entry, int remId){
    PointLL prev, act, next;

    if(!entry){
        return NULL;
    }

    act = getPointEntry(entry, remId);

    if(act){
        //get neighbors pointers
        next = act->nextPoint;
        prev = act->prevPoint;

        //update neighbors pointers
        if(next){
            next->prevPoint = prev;
        }
        if(prev){
            prev->nextPoint = next;
        }

        //remove Point Entry and free memory
        remPoint(act->point);
        free(act);

        //update ID value
        updateIDNextPointLL(next,-1);
    }
    
    return entry;
}

void remAllPointLL(PointLL list){
    PointLL aux, next;
    
    //validation
    if(!list){
        return;
    }
    
    aux = list;
    while(aux->nextPoint){
        next = aux->nextPoint;
        free(aux->point);
        free(aux);
        aux = next;
    }
}

/** GETS **/

PointLL getPointEntry(PointLL list, int pointID){
    PointLL aux, res = NULL;
    
    if(!list){
        return NULL;
    }
    
    //from the start
    aux = getFirstPointEntry(list);
    
    while(aux->nextPoint){
        if(aux->id == pointID){
            res = aux;
            break;
        }
        aux = aux->nextPoint;
    }
    
    return res;
}

 PointLL getLastPointEntry(PointLL list){
    PointLL aux;
    
    //validation
    if(!list){
        return NULL;
    }
    
    aux = list;
    
    while(aux->nextPoint){
        aux = aux->nextPoint;
    }

    return aux;

}

PointLL getFirstPointEntry(PointLL list){
     PointLL aux;
    
    //validation
    if(!list){
        return NULL;
    }
    
    aux = list;
    
    while(aux->prevPoint){
        aux = aux->prevPoint;
    }

    return aux;

}

/** OTHER **/

void updateIDNextPointLL(PointLL point, int increment){
     if(!point)
        return;
    point->id += increment;
    updateIDNextPointLL(point->nextPoint, increment);

}

void getPointLLParameters(PointLL list, float *coordXBeg, float *coordYBeg,
        float *coordXEnd, float *coordYEnd, uint8 *minValue, uint8 *maxValue,
        int *pointCount){
//more readable definitions
#define MIN 0
#define MAX 1
    
    //variables
    float xMin, yMin, xMax, yMax;
    uint8 minV, maxV;
    int   counter;
    PointLL auxLL;
    Point   auxP;
    
    //validation
    if(!list)
        goto error;
    
    //init variables;
    xMax = yMax = FLT_MIN;//invalid
    xMin = yMin = FLT_MAX;
    minV = 255; maxV = 0;
    counter = 0;
    
    //Put pointer from the beginning of the list
    auxLL = getFirstPointEntry(list);
    while(auxLL){
        auxP = auxLL->point;
        if(auxP->coordinates.x < xMin) xMin = auxP->coordinates.x;
        if(auxP->coordinates.x > xMax) xMax = auxP->coordinates.x;
        if(auxP->coordinates.y < yMin) yMin = auxP->coordinates.y;
        if(auxP->coordinates.y > yMax) yMax = auxP->coordinates.y;
        if(auxP->value  < minV) minV = auxP->value;
        if(auxP->value  > maxV) maxV = auxP->value;
        counter++;
        auxLL = auxLL->nextPoint;
    }

    //save obtained values
    *coordXBeg  = xMin;
    *coordYBeg  = yMin;
    *coordXEnd  = xMax;
    *coordYEnd  = yMax;
    *minValue   = minV;
    *maxValue   = maxV;
    *pointCount = counter;
    
    return;

error:
    fprintf(stderr, "ERROR:An error occurred\n");
    return;
}

/**
 * Given a list of Points the start of X and Y Coordinates and their size
 * returns a Point of the geometric centroid.
 */
PointCoord* compute2DPolygonCentroid(PointLL list, PointCoord* centroid, int startX, int startY, int sizeX, int sizeY){
//more readable definitions
#define MIN 0
#define MAX 1
    
    //variables
    PointLL auxPLL;
    Point auxP = NULL;
    float signedArea = 0.0;
    int x0 = 0.0; // Current vertex X
    int y0 = 0.0; // Current vertex Y
    int x1 = 0.0; // Next vertex X
    int y1 = 0.0; // Next vertex Y
    float a = 0.0;  // Partial signed area
    //Auxiliary variables for perimeter calculation
    int i;
    int auxX, auxY;
    //in a square, there is two x coordinates for each y -> minX and maxX
    int perimeter[sizeY][2];
    //Pointer to the original PointLL
    //PointLL perimeterPointer[sizeY][2];
    
    //validation
    if(!list){
        goto error;
    }
    
    //init
    centroid->x = 0.0;
    centroid->y = 0.0;
    for(i=0; i<sizeY+1; i++){
        perimeter[i][MIN] = -1;//startX+sizeX; //init minimum at maximum
        perimeter[i][MAX] = -1;//sizeX; //init maximum at minimum
        //perimeterPointer[i][MIN] = NULL;
        //perimeterPointer[i][MAX] = NULL;
    }

    //Note: Following loop cannot be in getPointLLParameters because sizeY and 
    //SizeX are needed and these are only obtained after one pass.
    auxPLL = getFirstPointEntry(list);
    while(auxPLL){
        auxP = auxPLL->point;
        //validation
        if(!auxP){
            //Should not enter here!
            fprintf(stderr, "Warning: Skipped a Point -> index out of range\n");
        } else {
            auxX = auxP->coordinates.x;
            auxY = auxP->coordinates.y;
            //checks if the point is valid to be used as perimeter array index
            if(isInside(auxY-startY, auxX-startX, sizeY, sizeX)){
                //if X coordinate for some Y is lower than recorded before
                if(perimeter[auxY-startY][MIN] > auxX || perimeter[auxY-startY][MIN] < 0){
                    perimeter[auxY-startY][MIN] = auxX;
                    //perimeterPointer[auxY-startY][MIN] = auxPLL;
                }
                //if X coordinate for some Y is higher than recorded before
                if(perimeter[auxY-startY][MAX] < auxX || perimeter[auxY-startY][MAX] < 0){
                    perimeter[auxY-startY][MAX] = auxX;
                    //perimeterPointer[auxY-startY][MAX] = auxPLL;
                }
            }
        }
        auxPLL = auxPLL->nextPoint;
    }
    
    //Following calculations are specified in:
    //http://en.wikipedia.org/wiki/Centroid#Centroid_of_polygon
    
    // For all vertices except last in one side (MIN X)
    for (i=0; i<sizeY-1; i++) {
        x0 = perimeter[i][MIN];
        y0 = startY + i;
        x1 = perimeter[i+1][MIN];
        y1 = startY + i + 1;
        a = x0*y1 - x1*y0;
        signedArea += a;
        centroid->x += (x0 + x1)*a;
        centroid->y += (y0 + y1)*a;
    }

    // Do last vertex do the connection with the other side
    x0 = perimeter[sizeY-1][MIN];
    y0 = startY + sizeY-1;
    x1 = perimeter[sizeY-1][MAX];
    y1 = startY + sizeY-1;
    a = x0*y1 - x1*y0;
    signedArea += a;
    centroid->x += (x0 + x1)*a;
    centroid->y += (y0 + y1)*a;
    
    // For all vertices except last in other side (MAX X)
    for (i=sizeY-1; i>0; i--) {
        x0 = perimeter[i][MAX];
        y0 = startY + i;
        x1 = perimeter[i-1][MAX];
        y1 = startY + i - 1;
        a = x0*y1 - x1*y0;
        signedArea += a;
        centroid->x += (x0 + x1)*a;
        centroid->y += (y0 + y1)*a;
    }
    
    //Connect ends
    x0 = perimeter[0][MAX];
    y0 = startY;
    x1 = perimeter[0][MIN];
    y1 = startY;
    a = x0*y1 - x1*y0;
    signedArea += a;
    centroid->x += (x0 + x1)*a;
    centroid->y += (y0 + y1)*a;

    //Final calculation
    signedArea *= 0.5;
    //if could not calculate signed area return null
    //otherwise would be doing division by 0
    if(signedArea == 0){
        return NULL;
    } else {
        centroid->x /= (6.0*signedArea);
        centroid->y /= (6.0*signedArea);
    }

    return centroid;
    
error:
    fprintf(stderr, "Warning: Centroid arguments not valid");
    return NULL;
}


int pointCount(PointLL list){
    PointLL aux;
    int counter = 1;
    
    //validation
    if(!list){
        return 0;
    }
    
    aux=list;
    while(aux->nextPoint){
        aux = aux->nextPoint;
        counter++;
    }
    
    return counter;

}

/*############## POINT ##############*/

/** CONSTRUCTOR **/

/**
 * SHOULD NOT BE USED OUTSIDE! -> Not declared in header file
 */
Point newPoint(){
    Point res;
    
    if(!(res = (Point)malloc(sizeof(struct sPoint)))){
        goto error;
    }
    
    res->coordinates.x = 0;
    res->coordinates.y = 0;
    res->value  = 0;
    
    return res;
    
error:
    fprintf(stderr, "ERROR:An error occurred -> no memory?\n");
    return NULL;
}


Point createNewPoint(float coordX, float coordY, uint8 value){
     Point res;
    
    //validation
    if(!(res = newPoint())){
        goto error;
    }
    
    res->coordinates.x = coordX;
    res->coordinates.y = coordY;
    res->value  = value;
    
    return res;
    
error:
    return NULL;

}

/** REMOVE **/

void remPoint(Point point){
    free(point);
}

/** OTHER **/
