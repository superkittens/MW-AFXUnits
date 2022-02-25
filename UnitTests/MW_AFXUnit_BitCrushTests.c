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


#include "MW_AFXUnit_BitCrushTests.h"


static int32_t MW_AFXUnit_BitCrush_initializationTests()
{
    MW_AFXUnit_BitCrush bc;
    int32_t downSample = 8;
    float32_t bitDepth = 4;
    float32_t gain = 3.f;

    //  Some nullptr checks
    int32_t success = MW_AFXUnit_BitCrush_init(NULL, downSample, bitDepth, gain);
    if (success)
        return 0;

    success = MW_AFXUnit_BitCrush_init(&bc, -1, bitDepth, gain);
    if (success)
        return 0;

    success = MW_AFXUnit_BitCrush_init(&bc, downSample, -1, gain);
    if (success)
        return 0;

    success = MW_AFXUnit_BitCrush_init(&bc, downSample, bitDepth, gain);
    if (!success)
        return 0;

    //  Make sure that the member variables have been properly initialized
    if (bc._downSamplingRate != downSample)
        return 0;

    if (bc._sampleCounter != 0)
        return 0;

    if (bc._bitDepth != bitDepth)
        return 0;

    if (bc._currentSampleValue != 0)
        return 0;

    return 1;
}


int32_t MW_AFXUnit_BitCrush_runUnitTests()
{
    if (!MW_AFXUnit_BitCrush_initializationTests())
        return 0;

    return 1;
}
