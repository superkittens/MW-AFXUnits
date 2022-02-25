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

#ifndef MW_DSP_APCF_H_
#define MW_DSP_APCF_H_

#include "arm_math.h"

typedef struct
{
    float32_t   *delayLine;
    float32_t   gain;
    int32_t     N;
    int32_t     currentPtr;
}MW_DSP_APCF;


/*
 *  Nested APCF structure
 *  Only two nested APCFs are supported but you can have as many inner APCFs connected in series
 */
typedef struct
{
    float32_t       *delayLine;
    float32_t       gain;
    int32_t         N;
    int32_t         currentPtr;

    MW_DSP_APCF     *innerAPCFs;
    int32_t         numInnerAPCFs;

}MW_DSP_NestedAPCF;



int32_t     MW_DSP_APCF_init(MW_DSP_APCF *filter, float32_t *delayLineBuffer, int32_t N, float32_t gain);
float32_t   MW_DSP_APCF_tick(MW_DSP_APCF *filter, float32_t x);


int32_t     MW_DSP_NestedAPCF_init(MW_DSP_NestedAPCF *filter, float32_t *delayLineBuffer, int32_t N, float32_t gain, MW_DSP_APCF *innerAPCFs, int32_t numInnerAPCFs);
float32_t   MW_DSP_NestedAPCF_tick(MW_DSP_NestedAPCF *filter, float32_t x);


#endif /* MW_DSP_APCF_H_ */
