//REF: https://raw.githubusercontent.com/hawell/fpl/c14ddb33c074f8d7a0efe780b71456b7511b0836/src/clahe.h
/*
 *  Copyright (c) 2012 Arash Kordi <arash.cordi@gmail.com>
 *
 *  This file is part of fpl.
 *
 *  fpl is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  fpl is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with fpl.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * MODIFIED for uint8 support!
 */

#ifndef CLAHE_H_
#define CLAHE_H_

#include <stdlib.h>
#include <stdio.h>
#include <tiff.h>//uint8

#define uiNR_OF_GREY (256)

#define uiMAX_REG_X	16
#define uiMAX_REG_Y	16

int CLAHE(uint8* pImage, unsigned int uiXRes, unsigned int uiYRes, uint8 Min,
        uint8 Max, unsigned int uiNrX, unsigned int uiNrY,
        unsigned int uiNrBins, float fCliplimit);


#endif /* CLAHE_H_ */
