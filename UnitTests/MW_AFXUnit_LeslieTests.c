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


#include "MW_AFXUnit_LeslieTests.h"

static float32_t EPSILON = 0.1;


static int32_t MW_AFXUnit_Leslie_initializationTests()
{
    MW_AFXUnit_Leslie leslie;
    float32_t delayLineBuffer[256];
    int16_t totalDelayLength = 256;
    float32_t fs = 44100.f;
    float32_t rpm = 100;

    //  Attempt to initialize AFXUnit with invalid parameters
    //  Null leslie instance
    int32_t success = MW_AFXUnit_Leslie_init(NULL, delayLineBuffer, totalDelayLength, fs, rpm);
    if (success)
        return 0;

    //  NULL delay line ptr
    success = MW_AFXUnit_Leslie_init(&leslie, NULL, totalDelayLength, fs, rpm);
    if (success)
        return 0;

    //  Invalid delay line length
    success = MW_AFXUnit_Leslie_init(&leslie, delayLineBuffer, -totalDelayLength, fs, rpm);
    if (success)
        return 0;

    //  Invalid sampling frequency
    success = MW_AFXUnit_Leslie_init(&leslie, delayLineBuffer, totalDelayLength, -fs, rpm);
    if (success)
        return 0;

    //  Invalid RPM (must be > 0)
    success = MW_AFXUnit_Leslie_init(&leslie, delayLineBuffer, totalDelayLength, fs, -rpm);
    if (success)
        return 0;



    //  Now properly initialize the AFXUnit
    success = MW_AFXUnit_Leslie_init(&leslie, delayLineBuffer, totalDelayLength, fs, rpm);
    if (!success)
        return 0;

    //  Check the MW_AFXUnit_Leslie struct members to make sure they are properly initialized
    float32_t expectedPhaseIncrement = 2.f * PI * (rpm / 60.f) / fs;
    if (leslie.phaseIncrement < expectedPhaseIncrement - EPSILON || leslie.phaseIncrement > expectedPhaseIncrement + EPSILON)
        return 0;

    float32_t expectedPhaseConstant = 2.f * PI / fs;
    if (leslie.phaseConstant < expectedPhaseConstant - EPSILON || leslie.phaseConstant > expectedPhaseConstant + EPSILON)
        return 0;

    if (leslie.phase != 0.f)
        return 0;

    float32_t expectedAngularVelocity = 2.f * PI * rpm / 60.f;
    if (leslie.angularVelocity < expectedAngularVelocity - EPSILON || leslie.angularVelocity > expectedAngularVelocity + EPSILON)
        return 0;

    float32_t expectedM = (float32_t)totalDelayLength / 2.f;
    if (leslie.M < expectedM - EPSILON || leslie.M > expectedM + EPSILON)
        return 0;

    return 1;
}


int32_t MW_AFXUnit_Leslie_standardOperationTests()
{
    MW_AFXUnit_Leslie leslie;
    float32_t delayLineBuffer[256];
    int16_t totalDelayLength = 256;
    float32_t fs = 44100.f;
    float32_t rpm = 100;

    int32_t success = MW_AFXUnit_Leslie_init(&leslie, delayLineBuffer, totalDelayLength, fs, rpm);
    if (!success)
        return 0;

    rpm = 400;

    MW_AFXUnit_Leslie_changeParameters(&leslie, rpm);

    float32_t expectedPhaseIncrement = 2.f * PI * (rpm / 60) / fs;
    if (leslie.phaseIncrement < expectedPhaseIncrement - EPSILON || leslie.phaseIncrement > expectedPhaseIncrement + EPSILON)
        return 0;

    return 1;
}


int32_t MW_AFXUnit_Leslie_runUnitTests()
{
    if (!MW_AFXUnit_Leslie_initializationTests())
        return 0;

    if (!MW_AFXUnit_Leslie_standardOperationTests())
        return 0;

    return 1;
}