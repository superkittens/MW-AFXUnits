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

#ifndef AFXUNITS_MW_AFXUNIT_DOPPLER_H_
#define AFXUNITS_MW_AFXUNIT_DOPPLER_H_

#include "arm_math.h"
#include "MW_AFXUnit_MiscUtils.h"


typedef struct
{
  float32_t *buffer;
  int32_t   N;
  float32_t readPtr;
  int32_t   writePtr;
  float32_t g;

  int32_t   inTransition;
  int32_t   readWritePtrCross;
  float32_t prevOutput;

  MW_AFXUnit_FastSine sine;
  float32_t transitionSignalBias;
  float32_t transitionSignalSign;
  float32_t transitionSignalPhase;
  float32_t transitionPhaseIncrement;

}MW_AFXUnit_Doppler;


int32_t   MW_AFXUnit_Doppler_init(MW_AFXUnit_Doppler *dopplerUnit, float32_t *delayLineBuffer, int32_t delayLineBufferSize, float32_t fs);
void      MW_AFXUnit_Doppler_changeParameters(MW_AFXUnit_Doppler *dopplerUnit, float32_t g);
void      MW_AFXUnit_Doppler_process(MW_AFXUnit_Doppler *dopplerUnit, float32_t *buffer, size_t bufferSize);
void      MW_AFXUnit_Doppler_reset(MW_AFXUnit_Doppler *dopplerUnit);

#endif /* AFXUNITS_MW_AFXUNIT_DOPPLER_H_ */
