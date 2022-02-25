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

#ifndef MW_AFXUNIT_MISCUTILS_H_
#define MW_AFXUNIT_MISCUTILS_H_

#include "arm_math.h"
#include "assert.h"


//  Optimzed sin/cos calculations
//  Based on approximations found in:
//  https://www.musicdsp.org/en/latest/Synthesis/10-fast-sine-and-cosine-calculation.html
//  To get the next sin/cos value, call MW_AFXUnit_Utils_FastSine_update()
//  The cos value is s[0] and the sin value is s[1]
typedef struct
{
    float32_t s[2];
    float32_t a;
    float32_t fs;
    float32_t A;
}MW_AFXUnit_FastSine;

//  Fast(er) trig functions
int32_t   MW_AFXUnit_Utils_FastSine_init(MW_AFXUnit_FastSine *fastSine, float32_t A, float32_t f, float32_t fs);
void      MW_AFXUnit_Utils_FastSine_update(MW_AFXUnit_FastSine *fastSine);
void      MW_AFXUnit_Utils_FastSine_setFrequency(MW_AFXUnit_FastSine *fastSine, float32_t f);
void      MW_AFXUnit_Utils_FastSine_reset(MW_AFXUnit_FastSine *fastSine);

//  Crossfading Utilities
int32_t   MW_AFXUnit_Utils_createFadeEnvelope(float32_t *dest, size_t size, uint32_t fadeType);
void      MW_AFXUnit_Utils_crossFade(float32_t *fadeInEnvelope, float32_t *fadeOutEnvelope, float32_t *fadeInData, float32_t *fadeOutData, float32_t *dest, uint32_t numSamples);

//  Value mapping functions
float32_t   MW_AFXUnit_Utils_mapToRange(float32_t value, float32_t low1, float32_t high1, float32_t low2, float32_t high2);



#endif /* MW_AFXUNIT_MISCUTILS_H_ */
