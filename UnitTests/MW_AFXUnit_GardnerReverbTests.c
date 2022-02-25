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


#include "MW_AFXUnit_GardnerReverbTests.h"

static float32_t _EPSILON = 0.1;


static int32_t MW_AFXUnit_GardnerReverb_initializationTests()
{
    MW_AFXUnit_GardnerReverb reverb;
    float32_t reverbDelayBuffer[10852];
    float32_t reverbGain = 0.5f;
    float32_t fs = 32000.f;

    int32_t expectedStartIndices[MW_AFXUNIT_GARDNERREVERB_TOTAL_DELAY_LINES];


    //  nullptr checks
    int32_t success = MW_AFXUnit_GardnerReverb_init(NULL, reverbDelayBuffer, reverbGain, fs);
    if (success)
        return 0;

    success = MW_AFXUnit_GardnerReverb_init(&reverb, NULL, reverbGain, fs);
    if (success)
        return 0;

    //  Check gain is less than 1
    success = MW_AFXUnit_GardnerReverb_init(&reverb, reverbDelayBuffer, 1.2f, fs);
    if (success)
        return 0;

    success = MW_AFXUnit_GardnerReverb_init(&reverb, reverbDelayBuffer, -0.9f, fs);
    if (success)
        return 0;

    success = MW_AFXUnit_GardnerReverb_init(&reverb, reverbDelayBuffer, reverbGain, -fs);
    if (success)
        return 0;
    
    //  Proper initialization
    success = MW_AFXUnit_GardnerReverb_init(&reverb, reverbDelayBuffer, reverbGain, fs);
    if (!success)
        return 0;
    
    if (reverb.delayLineMemory != reverbDelayBuffer)
        return 0;

    MW_AFXUnit_GardnerReverb_calculateDelayIndices(expectedStartIndices, fs);

    if (reverb.nestedAPCFs[0].delayLine != reverbDelayBuffer + expectedStartIndices[0])
        return 0;

    if (reverb.innerAPCF1[0].delayLine != reverbDelayBuffer + expectedStartIndices[1])
        return 0;

    if (reverb.innerAPCF1[1].delayLine != reverbDelayBuffer + expectedStartIndices[2])
        return 0;

    if (reverb.delayLines[0].buffer != reverbDelayBuffer + expectedStartIndices[3])
        return 0;

    if (reverb.standaloneAPCF.delayLine != reverbDelayBuffer + expectedStartIndices[4])
        return 0;

    if (reverb.delayLines[1].buffer != reverbDelayBuffer + expectedStartIndices[5])
        return 0;

    if (reverb.delayLines[2].buffer != reverbDelayBuffer + expectedStartIndices[6])
        return 0;

    if (reverb.nestedAPCFs[1].delayLine != reverbDelayBuffer + expectedStartIndices[7])
        return 0;

    if (reverb.innerAPCF2[0].delayLine != reverbDelayBuffer + expectedStartIndices[8])
        return 0;

    if (reverb.delayLines[3].buffer != reverbDelayBuffer + expectedStartIndices[9])
        return 0;
    
    if (reverb.gain <= reverbGain - _EPSILON || reverb.gain >= reverbGain + _EPSILON)
        return 0;
    
    if (reverb.feedbackLPF.filterType != MW_AFXUNIT_SVFILTER_LPF)
        return 0;

    
    return 1;
}


int32_t MW_AFXUnit_GardnerReverb_runUnitTests()
{
    if (!MW_AFXUnit_GardnerReverb_initializationTests())
        return 0;

    return 1;
}
