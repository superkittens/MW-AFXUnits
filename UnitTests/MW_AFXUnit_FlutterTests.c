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

#include "MW_AFXUnit_FlutterTests.h"

static int32_t MW_AFXUnit_Flutter_initializationTests()
{
    MW_AFXUnit_Flutter flutter;
    float32_t delayLine[1000];
    float32_t lfoFreq = 10.f;
    float32_t lfoDepth = 10.f;
    float32_t fs = 44100.f;
    float32_t b0 = 1.f;
    size_t N = 1000;
    float32_t M = 500;

    //  Attempt to initialize flutter unit with invalid parameters
    int32_t success = MW_AFXUnit_Flutter_init(NULL, delayLine, fs, M, N, lfoDepth, lfoFreq, b0);
    if (success)
        return 0;

    //  NULL delay line
    success = MW_AFXUnit_Flutter_init(&flutter, NULL, fs, M, N, lfoDepth, lfoFreq, b0);
    if (success)
        return 0;
    
    //  Invalid delay length (M)
    success = MW_AFXUnit_Flutter_init(&flutter, delayLine, fs, N + 1, N, lfoDepth, lfoFreq, b0);
    if (success)
        return 0;

    //  Invalid total delay length (N)
    success = MW_AFXUnit_Flutter_init(&flutter, delayLine, fs, M, 0, lfoDepth, lfoFreq, b0);
    if (success)
        return 0;

    //  Invalid LFO depth
    success = MW_AFXUnit_Flutter_init(&flutter, delayLine, fs, M, N, M + 1, lfoFreq, b0);
    if (success)
        return 0;
    
    //  Invalid LFO frequencies
    success = MW_AFXUnit_Flutter_init(&flutter, delayLine, fs, M, N, M, 0.0001, b0);
    if (success)
        return 0;

    success = MW_AFXUnit_Flutter_init(&flutter, delayLine, fs, M, N, M, fs, b0);
    if (success)
        return 0;


    //  Initialization with valid parameters
    success = MW_AFXUnit_Flutter_init(&flutter, delayLine, fs, M, N, lfoDepth, lfoFreq, b0);
    if (!success)
        return 0;


    if (flutter.fs != fs)
        return 0;

    if (flutter.lfoSamplesPerCycle != (int32_t)(fs / lfoFreq))
        return 0;

    if (flutter.b0 != b0)
        return 0;
    
    if (flutter.lfoDepth != lfoDepth)
        return 0;

    if (flutter.lfoFrequency != lfoFreq)
        return 0;

    return 1;
}


static int32_t MW_AFXUnit_Flutter_standardOperationTests()
{
    MW_AFXUnit_Flutter flutter;
    float32_t delayLine[1000];
    float32_t lfoFreq = 0.1f;
    float32_t lfoDepth = 10.f;
    float32_t fs = 44100.f;
    float32_t b0 = 1.f;
    size_t N = 1000;
    float32_t M = 500;

    int32_t success = MW_AFXUnit_Flutter_init(&flutter, delayLine, fs, M, N, lfoDepth, lfoFreq, b0);
    if (!success)
        return 0;
    
    //  Change LFO parameters
    lfoFreq = 1.f;
    lfoDepth = 1.f;
    b0 = 2.f;

    MW_AFXUnit_Flutter_changeParameters(&flutter, lfoDepth, lfoFreq, b0);

    if (flutter.lfoFrequency != lfoFreq)
        return 0;

    if (flutter.lfoDepth != lfoDepth)
        return 0;

    if (flutter.b0 != b0)
        return 0;


    //  There aren't really any parameters that we can catch for invalid inputs except for the assertions when NO_OPTIMIZE is defined
    //  We'll add process() here anyway just to illustrate the use of the flutter unit
    float32_t sampleInputData[5];
    MW_AFXUnit_Flutter_process(&flutter, sampleInputData, 5);

    return 1;
}



int32_t MW_AFXUnit_Flutter_runUnitTests()
{
    if (!MW_AFXUnit_Flutter_initializationTests())
        return 0;

    if (!MW_AFXUnit_Flutter_standardOperationTests())
        return 0;

    return 1;
}
