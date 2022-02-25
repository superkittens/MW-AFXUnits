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

#ifndef MW_AFXUNIT_GARDNERREVERB_H_
#define MW_AFXUNIT_GARDNERREVERB_H_

#include "arm_math.h"
#include "MW_DSP_APCF.h"
#include "MW_DSP_DelayLine.h"
#include "MW_AFXUnit_SVFilter.h"

#define MW_AFXUNIT_GARDNERREVERB_NUM_APCFS 4
#define MW_AFXUNIT_GARDNERREVERB_NUM_NESTEDAPCFS 2
#define MW_AFXUNIT_GARDNERREVERB_NUM_DELAYLINES 4
#define MW_AFXUNIT_GARDNERREVERB_TOTAL_DELAY_LINES (MW_AFXUNIT_GARDNERREVERB_NUM_APCFS + MW_AFXUNIT_GARDNERREVERB_NUM_NESTEDAPCFS + MW_AFXUNIT_GARDNERREVERB_NUM_DELAYLINES)


typedef struct
{
    //  delayLineMemory must point to a block of memory that accomodates at least 340 msec worth of samples
    //  If fs = 32000, then the array size must be at least 0.34 * 32000 = 10880 samples
    float32_t           *delayLineMemory;
    MW_DSP_APCF         innerAPCF1[2];
    MW_DSP_APCF         innerAPCF2[1];
    MW_DSP_APCF         standaloneAPCF;
    MW_DSP_NestedAPCF   nestedAPCFs[MW_AFXUNIT_GARDNERREVERB_NUM_NESTEDAPCFS];
    MW_DSP_DelayLine    delayLines[MW_AFXUNIT_GARDNERREVERB_NUM_DELAYLINES];
    MW_AFXUnit_SVFilter feedbackLPF;
    float32_t           gain;

}MW_AFXUnit_GardnerReverb;


int32_t     MW_AFXUnit_GardnerReverb_init(MW_AFXUnit_GardnerReverb *reverb, float32_t *delayLineMemory, float32_t gain, float32_t fs);
void        MW_AFXUnit_GardnerReverb_changeParameters(MW_AFXUnit_GardnerReverb *reverb, float32_t gain);
void        MW_AFXUnit_GardnerReverb_process(MW_AFXUnit_GardnerReverb *reverb, float32_t *buffer, size_t bufferSize);


void        MW_AFXUnit_GardnerReverb_calculateDelayIndices(int32_t *calculatedIndices, float32_t fs);



#endif /* MW_AFXUNIT_GARDNERREVERB_H_ */
