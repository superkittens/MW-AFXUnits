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

#ifndef MW_AFXUNIT_BITCRUSH_H_
#define MW_AFXUNIT_BITCRUSH_H_

#include "arm_math.h"

typedef struct
{
    int32_t _isInitialized;
    int32_t _downSamplingRate;
    float32_t _bitDepth;
    float32_t _gain;
    int32_t _sampleCounter;
    float32_t _currentSampleValue;
    float32_t _ql;
}MW_AFXUnit_BitCrush;



int32_t MW_AFXUnit_BitCrush_init(MW_AFXUnit_BitCrush *bc, int32_t downSamplingRate, float32_t bitDepth, float32_t gain);
void    MW_AFXUnit_BitCrush_changeParameters(MW_AFXUnit_BitCrush *bc, int32_t downSamplingRate, float32_t bitDepth, float32_t gain);
void    MW_AFXUnit_BitCrush_process(MW_AFXUnit_BitCrush *bc, float32_t* inputBuffer, size_t numSamples);

#endif /* MW_AFXUNIT_BITCRUSH_H_ */
