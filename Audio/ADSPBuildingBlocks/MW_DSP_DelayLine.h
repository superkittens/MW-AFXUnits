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

#ifndef ADSPBUILDINGBLOCKS_MW_DSP_DELAYLINE_H_
#define ADSPBUILDINGBLOCKS_MW_DSP_DELAYLINE_H_

#include "arm_math.h"
#include "assert.h"

#define MW_DSP_FRAC_DELAY_MAX_DELAY_LENGTH 32768.f

typedef struct
{
  float32_t *buffer;
  size_t    N;
  size_t    currentPtr;
  int32_t   memoryDynamicallyAllocated;
}MW_DSP_DelayLine;


typedef struct
{
  float32_t   *buffer;
  int32_t     N;    
  int32_t     MInt; 
  float32_t   MFrac;
  int32_t     writePtr;
  int32_t     readPtr;
}MW_DSP_FractionalDelayLine;


int32_t   MW_DSP_DelayLine_init(MW_DSP_DelayLine *delayLine, float32_t *bufferMemory, size_t N);
int32_t   MW_DSP_DelayLine_init_memalloc(MW_DSP_DelayLine **delayLine, size_t N);
int32_t   MW_DSP_DelayLine_delete(MW_DSP_DelayLine **delayLine);

void      MW_DSP_DelayLine_setDelayLength(MW_DSP_DelayLine *delayLine, float32_t M);
float32_t MW_DSP_DelayLine_tick(MW_DSP_DelayLine *delayLine, float32_t x);
float32_t MW_DSP_DelayLine_peek(MW_DSP_DelayLine *delayLine);


int32_t   MW_DSP_FractionalDelayLine_init(MW_DSP_FractionalDelayLine *delayLine, float32_t *buffer, int16_t N, float32_t M);

void      MW_DSP_FractionalDelayLine_setDelayLength(MW_DSP_FractionalDelayLine *delayLine, float32_t M);
float32_t MW_DSP_FractionalDelayLine_tick(MW_DSP_FractionalDelayLine *delayLine, float32_t x);
void      MW_DSP_FractionalDelayLine_process(MW_DSP_FractionalDelayLine *delayLine, float32_t *buffer, size_t numSamples);
void      MW_DSP_FractionalDelayLine_reset(MW_DSP_FractionalDelayLine *delayLine);


#endif /* ADSPBUILDINGBLOCKS_MW_DSP_DELAYLINE_H_ */
