#include "RegionLL.h"

/*############## REGIONLL ##############*/

/**
 * SHOULD NOT BE USED OUTSIDE! -> Not declared in header file
 */
RegionLL newRegionLL(){
    RegionLL res;
    
    if(!(res = (RegionLL)malloc(sizeof(struct sRegionLL)))){
        goto error;
    }
    
    res->id         = 0;
    res->nextRegion = NULL;
    res->prevRegion = NULL;
    res->region     = NULL;
    
    return res;
    
error:
    fprintf(stderr, "ERROR: An error occurred -> no memory?\n");
    return NULL;
}

/** CONSTRUCTOR **/

RegionLL createNewRegionLL(Region region){
    RegionLL res;
    
    if(!(res = newRegionLL())){
        goto error;
    }
    
    //add first element
    res->id     = 1;
    region->id  = 1;
    res->region = region;
    
    return res;
    
error:
    return NULL;
}

/** INSERT **/

RegionLL addRegionLLEntry(RegionLL list, Region addRegion){
    //variables
    RegionLL lastRegion, newEntry;
    
    //validation
    if(!addRegion){
        fprintf(stderr, "Warning: No Region added\n");
        return list;
    }
    
    //if there is no list create a new one and return
    if(!list){
        return createNewRegionLL(addRegion);
    }
    
    //else - add to the tail of the list
    if(!(newEntry = newRegionLL())){
        goto error;
    }
    
    //get last RegionLL entry
    if(!(lastRegion = getLastRegionEntry(list))){
        goto error;//should never happen
    }
    
    //Put region info in the list
    newEntry->id         = lastRegion->id + 1;
    addRegion->id        = lastRegion->id + 1;
    newEntry->region     = addRegion;
    newEntry->prevRegion = lastRegion;
    newEntry->nextRegion = NULL;
    
    lastRegion->nextRegion = newEntry;
    
    return list;
    
error:
    fprintf(stderr, "ERROR: An error occurred\n");
    return NULL;
}

/** REMOVE **/

RegionLL remRegionLLEntry(RegionLL entry, int remId){
    //variables
    RegionLL prev, actual, next;
    
    if(!entry){
        fprintf(stderr, "Warning: No Region List\n");
        return NULL;
    }
    
    //from the start
    actual = getFirstRegionEntry(entry);
    
    while(actual->nextRegion){
        next = actual->nextRegion;
        prev = actual->prevRegion;
        if(actual->id == remId){
            if(next){
                next->prevRegion = prev;
            }
            if(prev){
                prev->nextRegion = next;
            }
            remRegion(actual->region);
            free(actual);
            updateIDNextRegionLL(next, -1);
        }
        actual = actual->nextRegion;
    }
    
    return entry;
}

void remAllRegionLL(RegionLL list){
    RegionLL aux, next;
    
    //validation
    if(!list){
        return;
    }
    
    aux = list;
    while(aux->nextRegion){
        next = aux->nextRegion;
        free(aux->region);
        free(aux);
        aux = next;
    }
}

/** GETS **/

RegionLL getLastRegionEntry(RegionLL list){
    //variables
    RegionLL aux;
    
    //validation
    if(!list){
        return NULL;
    }
    
    aux = list;
    
    while(aux->nextRegion){
        aux = aux->nextRegion;
    }

    return aux;
}

RegionLL getFirstRegionEntry(RegionLL list){
    //variables
    RegionLL aux;
    
    //validation
    if(!list){
        return NULL;
    }
    
    aux = list;
    
    while(aux->prevRegion){
        aux = aux->prevRegion;
    }

    return aux;
}

RegionLL getRegionEntry(RegionLL list, int regionID){
    RegionLL aux, res = NULL;
    
    if(!list){
        return NULL;
    }
    
    //from the start
    aux = getFirstRegionEntry(list);
    
    while(aux->nextRegion){
        if(aux->id == regionID){
            res = aux;
            break;
        }
        aux = aux->nextRegion;
    }
    
    return res;
}

/** OTHER **/

void updateIDNextRegionLL(RegionLL region, int increment){
    if(!region)
        return;
    region->id += increment;
    updateIDNextRegionLL(region->nextRegion, increment);
}

int regionCount(RegionLL list){
    RegionLL aux;
    int counter = 1;
    
    //validation
    if(!list){
        return 0;
    }
    
    aux=getFirstRegionEntry(list);
    while(aux->nextRegion){
        aux = aux->nextRegion;
        counter++;
    }
    
    return counter;

}

/*############## REGION ##############*/

/** CONSTRUCTOR **/

/**
 * SHOULD NOT BE USED OUTSIDE! -> Not declared in header file
 */
Region newRegion(){
    Region res;
    
    if(!(res = (Region)malloc(sizeof(struct sRegion)))){
        goto error;
    }
    
    res->id         = 0;
    res->coordXBeg  = 0;
    res->coordYBeg  = 0;
    res->coordXEnd  = 0;
    res->coordYEnd  = 0;
    res->minValue   = 0;
    res->maxValue   = 0;
    res->pointCount = 0;
    res->centroid.x = 0;
    res->centroid.y = 0;
    res->pointList  = NULL;
    
    return res;
    
error:
    fprintf(stderr, "ERROR: An error occurred -> no memory?\n");
    return NULL;
}

//NOTE: Borders regions (within 5 pixels) are deleted!
Region createNewRegion(PointLL pointList, uint32 width, uint32 height){
    Region res;
    PointCoord* aux;
    
    //validation
    if(!(res = newRegion())){
        goto error;
    }
    
    res->id         = 1;
    res->pointList  = pointList;
    getPointLLParameters(pointList, &res->coordXBeg, &res->coordYBeg,
            &res->coordXEnd, &res->coordYEnd, &res->minValue, &res->maxValue,
            &res->pointCount);
    
    //Border region -> delete
    if(res->coordXBeg < 5 || res->coordYBeg < 5 || res->coordXEnd > width-5 || res->coordYEnd > height-5 ){
        return NULL;
    }
    
    aux = compute2DPolygonCentroid(pointList, &res->centroid, res->coordXBeg,
            res->coordYBeg, res->coordXEnd-res->coordXBeg+1,
            res->coordYEnd-res->coordYBeg+1);
    
    //if could not calculate centroid, give this indication
    if(aux == NULL){
        return NULL;
    }
    
    return res;
    
error:
    return NULL;
}

/** REMOVE **/

void remRegion(Region region){
    //validation
    if(!region){
        fprintf(stderr, "Warning: No region to remove\n");
        return;
    }
    
    //free memory
    remAllPointLL(region->pointList);
    free(region);
}