//  Copyright 2021 Allen Lee
//
//  Author:  Allen Lee (alee@meoworkshop.org)
//  
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
//  For more information, please refer to https://opensource.org/licenses/mit-license.php
//
//  ------------------------------------------------------------------------------------------------  //

#ifndef MW_AFXUNIT_GRANULAR_H_
#define MW_AFXUNIT_GRANULAR_H_

#include "arm_math.h"
#include "MW_AFXUnit_MiscUtils.h"

#define MAX_GRAIN_BUFFER_SIZE 6400

typedef struct
{
    float32_t   *tapeBuffer;
    int32_t     tapeBufferSize;
    int32_t     tapeBufferPtr;
    float32_t   fs;

    int32_t     randomNumbers[4];

    float32_t   grainBuffer[MAX_GRAIN_BUFFER_SIZE];
    int32_t     grainBufferPtr;
    int32_t     currentGrainSize;
    int32_t     changeGrainSize;
    int32_t     newGrainSize;

    int32_t     changeTimeBetweenGrains;
    int32_t     numSamplesBetweenGrainChanges;
    int32_t     newNumSamplesBetweenGrainChanges;

    int32_t     grainChangeCounter;

}MW_AFXUnit_Granular;


int32_t MW_AFXUnit_Granular_init(MW_AFXUnit_Granular *granular, float32_t *tapeBuffer, int32_t tapeBufferSize, float32_t grainSizeInSec, float32_t timeToChangeGrainInSec, float32_t fs);
void    MW_AFXUnit_Granular_changeGrainSize(MW_AFXUnit_Granular *granular, float32_t grainSizeInSec);
void    MW_AFXUnit_Granular_changeTimeToChangeGrain(MW_AFXUnit_Granular *granular, float32_t timeToChangeGrain);
void    MW_AFXUnit_Granular_process(MW_AFXUnit_Granular *granular, float32_t *buffer, size_t bufferSize);

void    MW_AFXUnit_Granular_windowGrain(MW_AFXUnit_Granular *granular);


#endif /* MW_AFXUNIT_GRANULAR_H_ */
