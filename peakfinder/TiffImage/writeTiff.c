#include "writeTiff.h"

int writeTiffImage(char* outputFileName, TiffImage img) {
    //variables
    TIFF          *TiffHndl;
    tdata_t       buf;
    unsigned int  y;

    //file descriptor
    TiffHndl = TIFFOpen(outputFileName,"w");
    if (TiffHndl == NULL){
	goto error;
    }
    
    //write image details
    TIFFSetField(TiffHndl, TIFFTAG_IMAGEWIDTH,      img->width);
    TIFFSetField(TiffHndl, TIFFTAG_IMAGELENGTH,     img->height);
    TIFFSetField(TiffHndl, TIFFTAG_PLANARCONFIG,    PLANARCONFIG_CONTIG);
    TIFFSetField(TiffHndl, TIFFTAG_PHOTOMETRIC,     PHOTOMETRIC_MINISBLACK);
    TIFFSetField(TiffHndl, TIFFTAG_BITSPERSAMPLE,   img->depth);
    TIFFSetField(TiffHndl, TIFFTAG_SAMPLESPERPIXEL, img->nSamples);
    //TIFFSetField(TiffHndl, TIFFTAG_ROWSPERSTRIP,    2);                     //ATENCAO!!!
    TIFFSetField(TiffHndl, TIFFTAG_ORIENTATION,     ORIENTATION_TOPLEFT);   //ATENCAO!!!
    TIFFSetField(TiffHndl, TIFFTAG_COMPRESSION,     COMPRESSION_NONE);
#if 0 //fixme: make this configureable
    TIFFSetField(TiffHndl, TIFFTAG_COMPRESSION, COMPRESSION_LZW);
    TIFFSetField(TiffHndl, TIFFTAG_PREDICTOR, 2);
#endif
    TIFFSetField(TiffHndl, TIFFTAG_RESOLUTIONUNIT,  img->resUnit);
    TIFFSetField(TiffHndl, TIFFTAG_XRESOLUTION,     img->xRes);
    TIFFSetField(TiffHndl, TIFFTAG_YRESOLUTION,     img->yRes);
    
    for (y = 0; y < img->height; y++) {
	buf = img->image[y];//img->data + 3*img->i.width*y;
	TIFFWriteScanline(TiffHndl, buf, y, 0);
    }
    
    TIFFClose(TiffHndl);
    return 0;
    
error:
    fprintf(stderr, "[WRITE_TIFF] An errror occurred\n");
    return -1;
}

