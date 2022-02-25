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

#ifndef MW_AFXUNIT_LESLIE_H_
#define MW_AFXUNIT_LESLIE_H_

#include "arm_math.h"
#include "MW_DSP_DelayLine.h"
#include "MW_AFXUnit_Biquad.h"

typedef struct
{
    MW_DSP_FractionalDelayLine delay;
    float32_t angularVelocity;
    float32_t phaseIncrement;
    float32_t phaseConstant;
    float32_t phase;
    float32_t M;
    float32_t a;
    float32_t s[2];
    MW_AFXUnit_Biquad biquads[4];
}MW_AFXUnit_Leslie;


int32_t MW_AFXUnit_Leslie_init(MW_AFXUnit_Leslie *leslie, float32_t *delayLineBuffer, int16_t delayLineLength, float32_t fs, float32_t rpm);
void    MW_AFXUnit_Leslie_changeParameters(MW_AFXUnit_Leslie *leslie, float32_t rpm);
void    MW_AFXUnit_Leslie_process(MW_AFXUnit_Leslie *leslie, float32_t *buffer, size_t bufferSize);


#endif /* MW_AFXUNIT_LESLIE_H_ */
