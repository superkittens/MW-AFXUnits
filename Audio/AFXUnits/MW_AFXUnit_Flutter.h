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

#ifndef MW_AFXUNIT_FLUTTER_H_
#define MW_AFXUNIT_FLUTTER_H_

#include "arm_math.h"
#include "MW_DSP_DelayLine.h"

//  Theoretically, the min LFO frequency is about 4.6E-10 Hz (frequency with the number of samples that is within the int32_t range)
//  But we will cap the frequency to 0.001.  That should be enough right?
#define MW_AFXUNIT_FLUTTER_MIN_LFO_FREQ 0.001f

typedef struct
{
    MW_DSP_FractionalDelayLine  delay;
    float32_t                   lfoDepth;
    float32_t                   lfoFrequency;
    float32_t                   lfoPhase;       //  Maybe remove?
    int16_t                     lfoSamplesPerCycle;
    float32_t                   lfoPhaseCounter;
    float32_t                   lfoPhaseIncrement;
    float32_t                   fs;
    float32_t                   b0;
    float32_t                   M;
}MW_AFXUnit_Flutter;


int32_t     MW_AFXUnit_Flutter_init(MW_AFXUnit_Flutter *flutter, float32_t *buffer, float32_t fs, float32_t M, int16_t N, float32_t lfoDepth, float32_t lfoFrequency, float32_t b0);
void        MW_AFXUnit_Flutter_changeParameters(MW_AFXUnit_Flutter *flutter, float32_t lfoDepth, float32_t lfoFrequency, float32_t b0);
void        MW_AFXUnit_Flutter_process(MW_AFXUnit_Flutter *flutter, float32_t *buffer, size_t bufferSize);
void        MW_AFXUnit_Flutter_reset(MW_AFXUnit_Flutter *flutter);



#endif /* MW_AFXUNIT_FLUTTER_H_ */
