#include "PointLL.h"

/**
 * SHOULD NOT BE USED OUTSIDE! -> Not declared in header file
 */

PointLL newPointLL(){
    PointLL res;
    
    if(!(res = (PointLL)malloc(sizeof(struct sPointLL)))){
        goto error;
    }
    
    res->size = 8;
    res->points = (Point*) malloc(sizeof(Point)*res->size);
    res->lenght = 0;
    
    return res;
    
error:
    fprintf(stderr, "ERROR:An error occurred -> no memory?\n");
    return NULL;    
}

/** CONSTRUCTOR **/

PointLL createNewPointLL(Point* point){
    PointLL res;
    
    if(!(res = newPointLL())){
        goto error;
    }
    
    //if occupation of array is greater than 80% -> double array size
    if(((float)res->lenght)/res->size > 0.8){
        realocPointLL(res);
    }
    
    //add first element
    res->points[res->lenght].value = point->value;
    res->points[res->lenght].coordinates.x = point->coordinates.x;
    res->points[res->lenght].coordinates.y = point->coordinates.y;
    res->lenght++;
    
    //free Point memory
    remPoint(point);
    
    return res;
    
error:
    return NULL;   
}

PointLL realocPointLL(PointLL list){
    //validation
    if(list == NULL){
        return NULL;
    }
    
    list->size *= 2;
    list->points = (Point*) realloc(list->points, list->size*sizeof(Point));
    
    return list;
}

/** INSERT **/

PointLL addPointLLEntry(PointLL list, Point* addPoint){
    if(!addPoint){
        return list;
    }

    if(!list){
        return createNewPointLL(addPoint);
    }
    
    //if occupation of array is greater than 80% -> double array size
    if(((float)list->lenght)/list->size > 0.8){
        realocPointLL(list);
    }
    
    //adds point to the array
    list->points[list->lenght].value = addPoint->value;
    list->points[list->lenght].coordinates.x = addPoint->coordinates.x;
    list->points[list->lenght].coordinates.y = addPoint->coordinates.y;
    list->lenght++;
    
    //free Point memory
    remPoint(addPoint);
    return list;
}

/** REMOVE **/

PointLL remPointLLEntry(PointLL list, int remId){
    int i;
    
    //validation
    if(!list){
        return NULL;
    }
    
    //check array limits
    if(remId>=0 && remId<list->lenght){
        remPoint(&list->points[remId]);
        //changes array positions
        for(i=remId; i<list->lenght-1; i++){
            list->points[i] = list->points[i+1];
        }
        
        list->lenght--;
    }
    
    return list;
}

void remAllPointLL(PointLL list){
    free(list->points);
    free(list);
}

/** GETS **/

void getPointLLParameters(PointLL list, float *coordXBeg, float *coordYBeg,
        float *coordXEnd, float *coordYEnd, uint8 *minValue, uint8 *maxValue,
        int *pointCount){
//more readable definitions
#define MIN 0
#define MAX 1
    
    //variables
    float xMin, yMin, xMax, yMax;
    uint8 minV, maxV;
    int   i, counter;
    
    //validation
    if(!list)
        goto error;
    
    //init variables;
    xMax = yMax = FLT_MIN;//invalid
    xMin = yMin = FLT_MAX;
    minV = 255; maxV = 0;
    counter = 0;
    
    for(i=0; i<list->lenght; i++){
        if(list->points[i].coordinates.x < xMin) xMin = list->points[i].coordinates.x;
        if(list->points[i].coordinates.x > xMax) xMax = list->points[i].coordinates.x;
        if(list->points[i].coordinates.y < yMin) yMin = list->points[i].coordinates.y;
        if(list->points[i].coordinates.y > yMax) yMax = list->points[i].coordinates.y;
        if(list->points[i].value  < minV) minV = list->points[i].value;
        if(list->points[i].value  > maxV) maxV = list->points[i].value;
        counter++;
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
    }

    //Note: Following loop cannot be in getPointLLParameters because sizeY and 
    //SizeX are needed and these are only obtained after one pass.
    for(i=0; i<list->lenght; i++){
        auxX = list->points[i].coordinates.x;
        auxY = list->points[i].coordinates.y;
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
    //validation
    if(!list){
        return 0;
    }
    
    return list->lenght;
}

/*############## POINT ##############*/

/** CONSTRUCTOR **/

/**
 * SHOULD NOT BE USED OUTSIDE! -> Not declared in header file
 */
Point* newPoint(){
    Point* res;
    
    if(!(res = (Point*)malloc(sizeof(struct sPoint)))){
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


Point* createNewPoint(float coordX, float coordY, uint8 value){
     Point* res;
    
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

void remPoint(Point* point){
    free(point);
}

/** OTHER **/
