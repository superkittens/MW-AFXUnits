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


#include "MW_DSP_CombFilter.h"

int32_t MW_DSP_FBCF_init(MW_DSP_FBCF *filter, float32_t *delayLine, int32_t N, float32_t b0, float32_t am)
{
    if (filter == NULL || delayLine == NULL || N <= 0)
        return 0;

    filter->delayLine = delayLine;
    filter->b0 = b0;
    filter->am = am;
    filter->currentPtr = 0;
    filter->N = N;

    return 1;
}


float32_t MW_DSP_FBCF_tick(MW_DSP_FBCF *filter, float32_t x)
{
    #ifdef NO_OPTIMIZE
    if (filter == NULL)
        while(1);
    #endif

    float32_t v = (x * filter->b0) + (filter->delayLine[filter->currentPtr] * filter->am);
    filter->delayLine[filter->currentPtr] = v;

    filter->currentPtr = (filter->currentPtr + 1) % filter->N;

    return v;
}