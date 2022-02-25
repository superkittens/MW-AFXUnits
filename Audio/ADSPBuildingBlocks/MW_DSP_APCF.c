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


#include "MW_DSP_APCF.h"


int32_t MW_DSP_APCF_init(MW_DSP_APCF *filter, float32_t *delayLineBuffer, int32_t N, float32_t gain)
{
    if (filter == NULL || delayLineBuffer == NULL)
        return 0;

    if (N <= 0 || gain >= 1.f)
        return 0;

    filter->delayLine = delayLineBuffer;
    filter->N = N;
    filter->gain = gain;
    filter->currentPtr = 0;

    return 1;
}



float32_t MW_DSP_APCF_tick(MW_DSP_APCF *filter, float32_t x)
{
    #ifdef NO_OPTIMIZE
    if (filter == NULL) while(1);
    #endif

    float32_t nextDelayOutput = filter->delayLine[filter->currentPtr];
    float32_t v = (nextDelayOutput * -filter->gain) + x;
    filter->delayLine[filter->currentPtr] = v;

    filter->currentPtr = (filter->currentPtr + 1) % filter->N;

    return nextDelayOutput + (v * filter->gain);
}



// ============================================================================================================== //


int32_t MW_DSP_NestedAPCF_init(MW_DSP_NestedAPCF *filter, float32_t *delayLineBuffer, int32_t N, float32_t gain, MW_DSP_APCF *innerAPCFs, int32_t numInnerAPCFs)
{
    if (filter == NULL || delayLineBuffer == NULL || innerAPCFs == NULL)
        return 0;

    if (N <= 0 || gain >= 1.f || numInnerAPCFs <= 0)
        return 0;

    filter->N = N;
    filter->gain = gain;
    filter->numInnerAPCFs = numInnerAPCFs;
    filter->innerAPCFs = innerAPCFs;
    filter->currentPtr = 0;
    filter->delayLine = delayLineBuffer;

    return 1;
}


float32_t MW_DSP_NestedAPCF_tick(MW_DSP_NestedAPCF *filter, float32_t x)
{
#ifdef NO_OPTIMIZE
if (filter == NULL) while(1);
#endif

    float32_t nextOuterDelayLineOut = filter->delayLine[filter->currentPtr];
    float32_t v = x + (-filter->gain);
    float32_t temp = v;

    for (int32_t i = 0; i < filter->numInnerAPCFs; ++i)
        temp = MW_DSP_APCF_tick(&filter->innerAPCFs[i], temp);
    
    filter->delayLine[filter->currentPtr] = temp;

    filter->currentPtr = (filter->currentPtr + 1) % filter->N;

    return nextOuterDelayLineOut + (v * filter->gain);
}