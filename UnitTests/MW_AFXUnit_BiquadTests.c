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


#include "MW_AFXUnit_BiquadTests.h"

static float32_t epsilon = 1.f;


static int32_t MW_AFXUnit_Biquad_initializationTests()
{
    MW_AFXUnit_Biquad biquad;

    //  Depending on the biquad type, we may need to add in a "copy buffer" 
    //  See the documentation in the Biquad initialization functions for more details
    float32_t copyBuffer[10];
    size_t copyBufferSize = 10;

    float32_t fs = 44100.f;
    float32_t fc = 100.f;
    float32_t Q = 0.707f;
    float32_t gain = 10.f;

    //  Initialize with valid parameters
    MW_AFXUnit_BiquadType biquadType = MW_BIQUAD_PARAM_EQ_NCQ;
    int32_t success = MW_AFXUnit_Biquad_init(&biquad, biquadType, fs, fc, Q, gain, copyBuffer, copyBufferSize);
    if (!success)
        return 0;

    //  Check that all MW_AFXUnit_Biquad struct members are properly initialized
    //  These checks are the same for ALL filter types
    //  Checks specific to different filters are performed in their respecitve tests
    if (biquad.filterType != MW_BIQUAD_PARAM_EQ_NCQ)
        return 0;

    if (biquad.fs != fs)
        return 0;

    //  Check to see if the biquadInstance struct has been initialized
    if (biquad.biquadInstance.numStages != 1)
        return 0;

    if (biquad.biquadInstance.pCoeffs != biquad.coefficients)
        return 0;

    if (biquad.biquadInstance.pState != biquad.stateVariables)
        return 0;

    
    //  Check invalid parameter protections
    //  Enter invalid MW_AFXUnit_Biquad instance
    success = MW_AFXUnit_Biquad_init(NULL, biquadType, fs, fc, Q, gain, copyBuffer, copyBufferSize);
    if (success)
        return 0;

    //  Enter invalid filter type
    success = MW_AFXUnit_Biquad_init(&biquad, -10, fs, fc, Q, gain, copyBuffer, copyBufferSize);
    if (success)
        return 0;

    //  Enter invalid fs
    success = MW_AFXUnit_Biquad_init(&biquad, biquadType, -fs, fc, Q, gain, copyBuffer, copyBufferSize);
    if (success)
        return 0;

    success = MW_AFXUnit_Biquad_init(&biquad, biquadType, 0, fc, Q, gain, copyBuffer, copyBufferSize);
    if (success)
        return 0;
    
    //  Enter invalid fc
    success = MW_AFXUnit_Biquad_init(&biquad, biquadType, fs, fs * 0.5f + 100, Q, gain, copyBuffer, copyBufferSize);
    if (success)
        return 0;

    //  Enter invalid Q
    success = MW_AFXUnit_Biquad_init(&biquad, biquadType, fs, fc, -Q, gain, copyBuffer, copyBufferSize);
    if (success)
        return 0;

    success = MW_AFXUnit_Biquad_init(&biquad, biquadType, fs, fc, 0, gain, copyBuffer, copyBufferSize);
    if (success)
        return 0;

    if (biquad.isInitializated == 0)
        return 0;

    return 1;
}


static int32_t MW_AFXUnit_Biquad_paramEQNCQInitializationTest()
{
    MW_AFXUnit_Biquad biquad;
    MW_AFXUnit_BiquadType biquadType = MW_BIQUAD_PARAM_EQ_NCQ;

    float32_t copyBuffer[10];
    size_t copyBufferSize = 10;

    float32_t fs = 44100.f;
    float32_t fc = 100.f;
    float32_t Q = 0.707f;
    float32_t gain = 10.f;

    int32_t success = MW_AFXUnit_Biquad_init(&biquad, biquadType, fs, fc, Q, gain, copyBuffer, copyBufferSize);
    if (!success)
        return 0;

    if (biquad.copyBuffer != copyBuffer)
        return 0;

    if (biquad.bufferSize != copyBufferSize)
        return 0;

    //  Check for protection against NULL copyBuffer and invalid copyBufferSize
    success = MW_AFXUnit_Biquad_init(&biquad, biquadType, fs, fc, Q, gain, NULL, copyBufferSize);
    if (success)
        return 0;

    success = MW_AFXUnit_Biquad_init(&biquad, biquadType, fs, fc, Q, gain, copyBuffer, 0);
    if (success)
        return 0;

    return 1;
}


static int32_t MW_AFXUnit_Biquad_lpfInitializationTests()
{
    MW_AFXUnit_Biquad biquad;
    MW_AFXUnit_BiquadType biquadType = MW_BIQUAD_LPF;

    float32_t fs = 44100.f;
    float32_t fc = 100.f;
    float32_t Q = 0.707f;
    float32_t gain = 10.f;

    //  Set coeffients to some known, dummy variable
    float32_t dummyValue = -999999.f;

    for (int32_t i = 0; i < 5; ++i)
        biquad.coefficients[i] = dummyValue;

    int32_t success = MW_AFXUnit_Biquad_init(&biquad, biquadType, fs, fc, Q, gain, NULL, 0);
    if (!success)
        return 0;
    
    for (int32_t i = 0; i < 5; ++i)
        if (biquad.coefficients[i] >= dummyValue - epsilon && biquad.coefficients[i] <= dummyValue + epsilon)
            return 0;

    //  Create new instance of MW_AFXUnit_Biquad and then immediately try to call MW_AFXUnit_Biquad_changeParameters()
    //  If NO_OPTIMIZE is enabled, then the program should hang in an infinite loop
    //  If calculateCoefficients does not have a case for a LPF, then this test should catch that
    MW_AFXUnit_Biquad newBiquad;
    for (int32_t i = 0; i < 5; ++i)
        newBiquad.coefficients[i] = dummyValue;
    
    MW_AFXUnit_Biquad_changeParameters(&newBiquad, biquadType, fc, Q, gain);

    for (int32_t i = 0; i < 5; ++i)
        if (newBiquad.coefficients[i] >= dummyValue - epsilon && newBiquad.coefficients[i] <= dummyValue + epsilon)
            return 0;

    return 1;
}


static int32_t MW_AFXUnit_Biquad_hpfInitializationTests()
{
    MW_AFXUnit_Biquad biquad;
    MW_AFXUnit_BiquadType biquadType = MW_BIQUAD_HPF;

    float32_t fs = 44100.f;
    float32_t fc = 100.f;
    float32_t Q = 0.707f;
    float32_t gain = 10.f;

    //  Set coeffients to some known, dummy variable
    float32_t dummyValue = -999999.f;

    for (int32_t i = 0; i < 5; ++i)
        biquad.coefficients[i] = dummyValue;

    int32_t success = MW_AFXUnit_Biquad_init(&biquad, biquadType, fs, fc, Q, gain, NULL, 0);
    if (!success)
        return 0;
    
    for (int32_t i = 0; i < 5; ++i)
        if (biquad.coefficients[i] >= dummyValue - epsilon && biquad.coefficients[i] <= dummyValue + epsilon)
            return 0;

    //  Create new instance of MW_AFXUnit_Biquad and then immediately try to call MW_AFXUnit_Biquad_changeParameters()
    //  If NO_OPTIMIZE is enabled, then the program should hang in an infinite loop
    MW_AFXUnit_Biquad newBiquad;
    for (int32_t i = 0; i < 5; ++i)
        newBiquad.coefficients[i] = dummyValue;
    
    MW_AFXUnit_Biquad_changeParameters(&newBiquad, biquadType, fc, Q, gain);

    for (int32_t i = 0; i < 5; ++i)
        if (newBiquad.coefficients[i] >= dummyValue - epsilon && newBiquad.coefficients[i] <= dummyValue + epsilon)
            return 0;

    return 1;
}


static int32_t MW_AFXUnit_Biquad_bpfInitializationTests()
{
    MW_AFXUnit_Biquad biquad;
    MW_AFXUnit_BiquadType biquadType = MW_BIQUAD_BPF;

    float32_t fs = 44100.f;
    float32_t fc = 100.f;
    float32_t Q = 0.707f;
    float32_t gain = 10.f;

    //  Set coeffients to some known, dummy variable
    float32_t dummyValue = -999999.f;

    for (int32_t i = 0; i < 5; ++i)
        biquad.coefficients[i] = dummyValue;

    int32_t success = MW_AFXUnit_Biquad_init(&biquad, biquadType, fs, fc, Q, gain, NULL, 0);
    if (!success)
        return 0;
    
    for (int32_t i = 0; i < 5; ++i)
        if (biquad.coefficients[i] >= dummyValue - epsilon && biquad.coefficients[i] <= dummyValue + epsilon)
            return 0;

    //  Create new instance of MW_AFXUnit_Biquad and then immediately try to call MW_AFXUnit_Biquad_changeParameters()
    //  If NO_OPTIMIZE is enabled, then the program should hang in an infinite loop
    //  If calculateCoefficients does not have a case for a LPF, then this test should catch that
    MW_AFXUnit_Biquad newBiquad;
    for (int32_t i = 0; i < 5; ++i)
        newBiquad.coefficients[i] = dummyValue;
    
    MW_AFXUnit_Biquad_changeParameters(&newBiquad, biquadType, fc, Q, gain);

    for (int32_t i = 0; i < 5; ++i)
        if (newBiquad.coefficients[i] >= dummyValue - epsilon && newBiquad.coefficients[i] <= dummyValue + epsilon)
            return 0;

    return 1;
}


static int32_t MW_AFXUnit_Biquad_lowShelfInitializationTests()
{
    MW_AFXUnit_Biquad biquad;
    MW_AFXUnit_BiquadType biquadType = MW_BIQUAD_LOW_SHELF;

    float32_t fs = 44100.f;
    float32_t fc = 100.f;
    float32_t Q = 0.707f;
    float32_t gain = 10.f;

    float32_t copyBuffer[10];
    size_t copyBufferSize = 10;

    //  Set coeffients to some known, dummy variable
    float32_t dummyValue = -999999.f;

    for (int32_t i = 0; i < 6; ++i)
        biquad.coefficients[i] = dummyValue;

    int32_t success = MW_AFXUnit_Biquad_init(&biquad, biquadType, fs, fc, Q, gain, copyBuffer, copyBufferSize);
    if (!success)
        return 0;
    
    for (int32_t i = 0; i < 6; ++i)
        if (biquad.coefficients[i] >= dummyValue - epsilon && biquad.coefficients[i] <= dummyValue + epsilon)
            return 0;

    //  Create new instance of MW_AFXUnit_Biquad and then immediately try to call MW_AFXUnit_Biquad_changeParameters()
    //  If NO_OPTIMIZE is enabled, then the program should hang in an infinite loop
    MW_AFXUnit_Biquad newBiquad;
    for (int32_t i = 0; i < 6; ++i)
        newBiquad.coefficients[i] = dummyValue;
    
    MW_AFXUnit_Biquad_changeParameters(&newBiquad, biquadType, fc, Q, gain);

    for (int32_t i = 0; i < 6; ++i)
        if (newBiquad.coefficients[i] >= dummyValue - epsilon && newBiquad.coefficients[i] <= dummyValue + epsilon)
            return 0;

    return 1;
}



int32_t MW_AFXUnit_Biquad_runUnitTests()
{
    if (!MW_AFXUnit_Biquad_initializationTests())
        return 0;

    if (!MW_AFXUnit_Biquad_paramEQNCQInitializationTest())
        return 0;

    if (!MW_AFXUnit_Biquad_lpfInitializationTests())
        return 0;

    if (!MW_AFXUnit_Biquad_hpfInitializationTests())
        return 0;

    if (!MW_AFXUnit_Biquad_bpfInitializationTests())
        return 0;

    // if (!MW_AFXUnit_Biquad_lowShelfInitializationTests())
    //     return 0;

    return 1;
}