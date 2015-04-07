#ifndef MASK_H
#define	MASK_H

#include "../TiffImage/tiffFile.h"

#ifdef	__cplusplus
extern "C" {
#endif

/*****************************************************************
########################  DATA STRUCTURES #######################
*****************************************************************/


/*****************************************************************
######################  END DATA STRUCTURES #####################
*****************************************************************/

/*****************************************************************
########################    PROTOTYPES    #######################
*****************************************************************/

TiffImage aplyMask(TiffImage img, int r);

/*****************************************************************
########################  END PROTOTYPES    ######################
*****************************************************************/


#ifdef	__cplusplus
}
#endif

#endif	/* MASK_H */

