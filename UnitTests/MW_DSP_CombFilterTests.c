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


#include "MW_DSP_CombFilterTests.h"


static int32_t MW_DSP_CombFilter_runInitializationTests()
{
    MW_DSP_FBCF filter;
    float32_t filterDelayLine[10];
    int32_t N = 10;
    float32_t b0 = 1.f;
    float32_t am = -0.5;

    //  nullptr checks
    int32_t success = MW_DSP_FBCF_init(NULL, filterDelayLine, N, b0, am);
    if (success)
        return 0;

    success = MW_DSP_FBCF_init(&filter, NULL, N, b0, am);
    if (success)
        return 0;
    
    //  Make sure the N > 0
    success = MW_DSP_FBCF_init(&filter, filterDelayLine, 0, b0, am);
    if (success)
        return 0;

    success = MW_DSP_FBCF_init(&filter, filterDelayLine, N, b0, am);
    if (!success)
        return 0;


    //  Check to make sure that the FBCF member variables have been properly set
    if (filter.delayLine != filterDelayLine)
        return 0;

    if (filter.b0 != b0)
        return 0;

    if (filter.am != am)
        return 0;

    if (filter.currentPtr != 0)
        return 0;

    return 1;
}


int32_t MW_DSP_CombFilter_runUnitTests()
{
    MW_DSP_CombFilter_runInitializationTests();
    return 1;
}