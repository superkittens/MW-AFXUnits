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


#include "MW_AFXUnit_BitCrush.h"


int32_t MW_AFXUnit_BitCrush_init(MW_AFXUnit_BitCrush *bc, int32_t downSamplingRate, float32_t bitDepth, float32_t gain)
{
    if (bc == NULL || downSamplingRate < 0 || bitDepth < 0)
        return 0;

    MW_AFXUnit_BitCrush_changeParameters(bc, downSamplingRate, bitDepth, gain);

    bc->_sampleCounter = 0;

    return 1;
}


void MW_AFXUnit_BitCrush_changeParameters(MW_AFXUnit_BitCrush *bc, int32_t downSamplingRate, float32_t bitDepth, float32_t gain)
{
    #ifdef NO_OPTIMIZE
    if (bc == NULL) while(1);
    if (downSamplingRate < 0 || bitDepth < 0) while(1);
    #endif

    bc->_downSamplingRate = downSamplingRate;
    bc->_bitDepth = bitDepth;
    bc->_currentSampleValue = 0;
    bc->_ql = 2.f / (powf(2, bitDepth) - 1);
    bc->_gain = powf(10, gain / 20.f);
}


void MW_AFXUnit_BitCrush_process(MW_AFXUnit_BitCrush *bc, float32_t* restrict inputBuffer, size_t numSamples)
{
    #ifdef NO_OPTIMIZE
    if (bc == NULL || inputBuffer == NULL) while(1);
    #endif


    for (int32_t i = 0; i < numSamples; ++i)
    {
        if (bc->_sampleCounter >= bc->_downSamplingRate)
        {
            bc->_sampleCounter = 0;
            int32_t A = (int32_t)(inputBuffer[i] * bc->_gain / bc->_ql);
            float32_t B = bc->_ql * A;
            bc->_currentSampleValue = bc->_ql * A;

            if (bc->_currentSampleValue > 1.f)
                bc->_currentSampleValue = 1.f;

            if (bc->_currentSampleValue < -1.f)
                bc->_currentSampleValue = -1.f;
        }

        inputBuffer[i] = bc->_currentSampleValue;
        bc->_sampleCounter += 1;
    }
}
