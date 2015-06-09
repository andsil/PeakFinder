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

    res->size = 8;
    res->regions = (Region*) malloc(sizeof(Region)*res->size);
    res->lenght = 0;
    
    return res;
    
error:
    fprintf(stderr, "ERROR: An error occurred -> no memory?\n");
    return NULL;
}

/** CONSTRUCTOR **/

RegionLL createNewRegionLL(Region *region){
    RegionLL res;
    
    if(!(res = newRegionLL())){
        goto error;
    }
    
    //if occupation of array is greater than 80% -> double array size
    if(((float)res->lenght)/res->size > 0.8){
        realocRegionLL(res);
    }
    
    //add first element
    cloneRegion(region, &res->regions[res->lenght]);
    res->regions[res->lenght].id = res->lenght;
    res->lenght++;
    
    //free memory (not PointLL inside)
    free(region);
    
    return res;
    
error:
    return NULL;
}

RegionLL realocRegionLL(RegionLL list){
    //validation
    if(list == NULL){
        return NULL;
    }
    
    list->size *= 2;
    list->regions = (Region*) realloc(list->regions, list->size*sizeof(Region));
    
    return list;
}

/** INSERT **/

RegionLL addRegionLLEntry(RegionLL list, Region *addRegion){
    //validation
    if(!addRegion){
        fprintf(stderr, "Warning: No Region added\n");
        return list;
    }
    
    //if there is no list create a new one and return
    if(!list){
        return createNewRegionLL(addRegion);
    }
    
    //if occupation of array is greater than 80% -> double array size
    if(((float)list->lenght)/list->size > 0.8){
        realocRegionLL(list);
    }
    
    //add element
    cloneRegion(addRegion, &list->regions[list->lenght]);
    list->regions[list->lenght].id = list->lenght;
    list->lenght++;
    
    //free memory (not PointLL inside)
    free(addRegion);
    
    return list;
}

/** REMOVE **/

RegionLL remRegionLLEntry(RegionLL entry, int remId){
    //variables
    int i;
    
    if(!entry){
        fprintf(stderr, "Warning: No Region List\n");
        return NULL;
    }
    
    //check array limits
    if(remId>=0 && remId<entry->lenght){
        remRegion(&entry->regions[remId]);
        //changes array positions
        for(i=remId; i<entry->lenght-1; i++){
            entry->regions[i] = entry->regions[i+1];
            entry->regions[i].id--;
        }
        
        entry->lenght--;
    }
    
    return entry;
}

void remAllRegionLL(RegionLL list){
    free(list->regions);
    free(list);
}

/** GETS **/

int regionCount(RegionLL list){
    return list->lenght;
}

/*############## REGION ##############*/

/** CONSTRUCTOR **/

/**
 * SHOULD NOT BE USED OUTSIDE! -> Not declared in header file
 */
Region* newRegion(){
    Region* res;
    
    if(!(res = (Region*)malloc(sizeof(Region)))){
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
Region* createNewRegion(PointLL pointList, uint32 width, uint32 height){
    Region* res;
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

void remRegion(Region *region){
    //validation
    if(!region){
        fprintf(stderr, "Warning: No region to remove\n");
        return;
    }
    
    //free memory
    remAllPointLL(region->pointList);
    free(region);
}

/** COPY **/

void cloneRegion(Region *src, Region *dst){
    //validation
    if(!src || !dst){
        fprintf(stderr, "Warning: No region to copy\n");
        return;
    }
    
    dst->id         = src->id        ;
    dst->coordXBeg  = src->coordXBeg ;
    dst->coordYBeg  = src->coordYBeg ;
    dst->coordXEnd  = src->coordXEnd ;
    dst->coordYEnd  = src->coordYEnd ;
    dst->minValue   = src->minValue  ;
    dst->maxValue   = src->maxValue  ;
    dst->pointCount = src->pointCount;
    dst->centroid.x = src->centroid.x;
    dst->centroid.y = src->centroid.y;
}