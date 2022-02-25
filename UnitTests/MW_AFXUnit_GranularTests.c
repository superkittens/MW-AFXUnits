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


#include "MW_AFXUnit_GranularTests.h"

static int32_t MW_AFXUnit_GranularTests_initializationTests()
{
    MW_AFXUnit_Granular granular;
    int32_t             tapeBufferSize = 10;
    float32_t           tapeBuffer[10];
    float32_t           grainSize = 0.05;           //  Grain size is in seconds
    float32_t           timeToChangeGrain = 0.1;    //  Time to change grain is also in seconds
    float32_t           fs = 32000.f;

    //  Pass in a null granular instance
    int32_t success = MW_AFXUnit_Granular_init(NULL, tapeBuffer, tapeBufferSize, grainSize, timeToChangeGrain, fs);
    if (success) return 0;

    //  Pass in a null pointer to the tape buffer argument
    success = MW_AFXUnit_Granular_init(&granular, NULL, tapeBufferSize, grainSize, timeToChangeGrain, fs);
    if (success) return 0;

    //  Pass in an invalid grain size (max grain size is 0.1 sec)
    success = MW_AFXUnit_Granular_init(&granular, tapeBuffer, tapeBufferSize, 0.15, timeToChangeGrain, fs);
    if (success) return 0;

    //  No negative values allowed!
    success = MW_AFXUnit_Granular_init(&granular, tapeBuffer, tapeBufferSize, -grainSize, timeToChangeGrain, fs);
    if (success) return 0;

    success = MW_AFXUnit_Granular_init(&granular, tapeBuffer, tapeBufferSize, grainSize, -timeToChangeGrain, fs);
    if (success) return 0;

    //  Invalid sampling frequency
    success = MW_AFXUnit_Granular_init(&granular, tapeBuffer, tapeBufferSize, grainSize, timeToChangeGrain, -fs);
    if (success) return 0;

    //  Calling init with valid arguments
    success = MW_AFXUnit_Granular_init(&granular, tapeBuffer, tapeBufferSize, grainSize, timeToChangeGrain, fs);
    if (!success) return 0;


    //  Make sure that all the Granular members have been initialized
    if (granular.tapeBuffer != tapeBuffer) return 0;
    if (granular.tapeBufferSize != tapeBufferSize) return 0;
    if (granular.grainBufferPtr != 0) return 0;
    if (granular.fs != fs) return 0;
    if (granular.currentGrainSize != (int32_t)(grainSize * fs)) return 0;
    if (granular.changeGrainSize != 0) return 0;
    if (granular.changeTimeBetweenGrains != 0) return 0;
    if (granular.numSamplesBetweenGrainChanges != (int32_t)(timeToChangeGrain * fs)) return 0;
    if (granular.newNumSamplesBetweenGrainChanges != 0) return 0;
    if (granular.grainChangeCounter != 0) return 0;


    return 1;
}


static int32_t MW_AFXUnit_GranularTests_windowingTest()
{
    MW_AFXUnit_Granular granular;
    int32_t             tapeBufferSize = 10;
    float32_t           tapeBuffer[10];
    float32_t           grainSize = 5;           //  Grain size is in seconds
    float32_t           timeToChangeGrain = 0.1;    //  Time to change grain is also in seconds
    float32_t           fs = 1.f;

    float32_t           expectedValuesOdd[] = {0.f, 0.5f, 1.f, 0.5f, 0.f};
    float32_t           expectedValuesEven[] = {0.f, 0.5f, 1.f, 1.f, 0.5f, 0.f};

    int32_t success = MW_AFXUnit_Granular_init(&granular, tapeBuffer, tapeBufferSize, grainSize, timeToChangeGrain, fs);
    if (!success) return 0;

    arm_fill_f32(1.f, granular.grainBuffer, MAX_GRAIN_BUFFER_SIZE);

    MW_AFXUnit_Granular_windowGrain(&granular);
    for (int32_t i = 0; i < grainSize; ++i)
        if (granular.grainBuffer[i] != expectedValuesOdd[i]) return 0;

    //  Check windowing for even grain buffer length case
    grainSize = 6;
    success = MW_AFXUnit_Granular_init(&granular, tapeBuffer, tapeBufferSize, grainSize, timeToChangeGrain, fs);
    if (!success) return 0;

    arm_fill_f32(1.f, granular.grainBuffer, MAX_GRAIN_BUFFER_SIZE);

    MW_AFXUnit_Granular_windowGrain(&granular);
    for (int32_t i = 0; i < grainSize; ++i)
        if (granular.grainBuffer[i] != expectedValuesEven[i]) return 0;

    return 1;
}




int32_t MW_AFXUnit_GranularTests_runUnitTests()
{
    if (!MW_AFXUnit_GranularTests_initializationTests())
        return 0;

    if (!MW_AFXUnit_GranularTests_windowingTest())
        return 0;
        
    return 1;
}