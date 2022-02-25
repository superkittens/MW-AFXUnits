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


#include "MW_AFXUnit_GardnerReverb.h"

static int32_t      _delayIndices[MW_AFXUNIT_GARDNERREVERB_NUM_APCFS + MW_AFXUNIT_GARDNERREVERB_NUM_NESTEDAPCFS + MW_AFXUNIT_GARDNERREVERB_NUM_DELAYLINES];
static float32_t    _DELAY_LINE_LENGTHS[] = {0.035f, 0.0083f, 0.022f, 0.005f, 0.030f, 0.067f, 0.015f, 0.039f, 0.0098f, 0.108f};


/*
 *  MW_AFXUnit_GardnerReverb_init() expects one block of contiguous memory that will then be split up for each of the
 *  reverb's APCFs and delay lines.
 *  To see which element in calculatedIndices corresponds to which APCF/delay line, see _delayLineLengths
 *  
 *  Delay lengths are taken from the medium room reverberator topology in William Gardner's MS thesis (page 56)
 */
void MW_AFXUnit_GardnerReverb_calculateDelayIndices(int32_t *calculatedIndices, float32_t fs)
{
#ifdef NO_OPTIMIZE
    if (calculatedIndices == NULL) while(1);
    if (fs <= 0.f) while(1);
#endif

    calculatedIndices[0] = 0;

    for (int32_t i = 1; i < MW_AFXUNIT_GARDNERREVERB_TOTAL_DELAY_LINES; ++i)
    {
        float32_t delayLength = _DELAY_LINE_LENGTHS[i - 1] * fs;
        int32_t delayLengthInteger = (int32_t)delayLength;
        float32_t remainder = delayLength - (float32_t)delayLengthInteger;

        int32_t delayLengthToUse = delayLengthInteger;
        if (remainder > 0)
            delayLengthToUse++;

        calculatedIndices[i] = calculatedIndices[i - 1] + delayLengthToUse;
    }
}


/*
 *  Initialize a MW_AFXUnit_GardnerReverb structure
 *  This structure implements a "medium room" reverberator (see William Gardner's MS thesis [page 56])
 *  This function will not allocate memory for you for each delay line/APCF modules.  Therefore, you must 
 *  pre-allocate memory and pass it into this function.
 * 
 *  MW_AFXUnit_GardnerReverb_init() expects a pointer to a block of contiguous memory which will then be 
 *  split up for each APCF/delay line components
 * 
 *  delayLineMemory size accomodate at least 340 msec of sample data, or 0.34 * fs samples
 *
 *  Inputs:
 *    reverb:           Pointer to a MW_AFXUnit_GardnerReverb structure
 *    delayLineMemory:  Pointer to an array that will hold the audio samples.  Memory must be already allocated
 *    gain:             Reverb feedback gain (must be less than 1 and greater than 0)
 *    fs:               Samping frequency (must be greater than 0)
 */
int32_t MW_AFXUnit_GardnerReverb_init(MW_AFXUnit_GardnerReverb *reverb, float32_t *delayLineMemory, float32_t gain, float32_t fs)
{
    if (reverb == NULL || delayLineMemory == NULL)
        return 0;

    if (gain <= 0 || fs <= 0 || gain >= 1.f)
        return 0;

    int32_t delayLineStarts[MW_AFXUNIT_GARDNERREVERB_TOTAL_DELAY_LINES];
    MW_AFXUnit_GardnerReverb_calculateDelayIndices(delayLineStarts, fs);

    //  Calculate lengths (in samples) of all APCFs and delay lines
    int32_t delayLengthsInSamples[MW_AFXUNIT_GARDNERREVERB_TOTAL_DELAY_LINES];
    for (int32_t i = 0; i < MW_AFXUNIT_GARDNERREVERB_TOTAL_DELAY_LINES; ++i)
    {
        float32_t delayLength = _DELAY_LINE_LENGTHS[i] * fs;
        int32_t delayLengthInteger = (int32_t)delayLength;
        float32_t remainder = delayLength - (float32_t)delayLengthInteger;

        int32_t delayLengthToUse = delayLengthInteger;
        if (remainder > 0)
            delayLengthToUse++;

        delayLengthsInSamples[i] = delayLengthToUse;
    }

    //  Initialize APCFs and Delay Lines
    //  Initialize first set of inner APCFs
    int32_t success = MW_DSP_APCF_init(&reverb->innerAPCF1[0], delayLineMemory + delayLineStarts[1], delayLengthsInSamples[1], 0.7);
    if (!success)
        return 0;

    success = MW_DSP_APCF_init(&reverb->innerAPCF1[1], delayLineMemory + delayLineStarts[2], delayLengthsInSamples[2], 0.5);
    if (!success)
        return 0;

    success = MW_DSP_NestedAPCF_init(&reverb->nestedAPCFs[0], delayLineMemory, delayLengthsInSamples[0], 0.3, reverb->innerAPCF1, 2);
    if (!success)
        return 0;

    success = MW_DSP_DelayLine_init(&reverb->delayLines[0], delayLineMemory + delayLineStarts[3], delayLengthsInSamples[3]);
    if (!success)
        return 0;

    success = MW_DSP_APCF_init(&reverb->standaloneAPCF, delayLineMemory + delayLineStarts[4], delayLengthsInSamples[4], 0.5);
    if (!success)
        return 0;

    success = MW_DSP_DelayLine_init(&reverb->delayLines[1], delayLineMemory + delayLineStarts[5], delayLengthsInSamples[5]);
    if (!success)
        return 0;

    success = MW_DSP_DelayLine_init(&reverb->delayLines[2], delayLineMemory + delayLineStarts[6], delayLengthsInSamples[6]);
    if (!success)
        return 0;

    success = MW_DSP_APCF_init(&reverb->innerAPCF2, delayLineMemory + delayLineStarts[8], delayLengthsInSamples[8], 0.6);
    if (!success)
        return 0;

    success = MW_DSP_NestedAPCF_init(&reverb->nestedAPCFs[1], delayLineMemory + delayLineStarts[7], delayLengthsInSamples[7], 0.3, reverb->innerAPCF2, 1);
    if (!success)
        return 0;

    success = MW_DSP_DelayLine_init(&reverb->delayLines[3], delayLineMemory + delayLineStarts[9], delayLengthsInSamples[9]);
    if (!success)
        return 0;

    reverb->delayLineMemory = delayLineMemory;
    reverb->gain = gain;

    success = MW_AFXUnit_SVFilter_init(&reverb->feedbackLPF, MW_AFXUNIT_SVFILTER_LPF, fs, 2500.f, 0.707f);
    if (!success)
        return 0;

    return 1;
}


void MW_AFXUnit_GardnerReverb_changeParameters(MW_AFXUnit_GardnerReverb *reverb, float32_t gain)
{
    #ifdef NO_OPTIMIZE
    if (reverb == NULL) while(1);
    #endif

    reverb->gain = gain;
}


void MW_AFXUnit_GardnerReverb_process(MW_AFXUnit_GardnerReverb *reverb, float32_t *buffer, size_t bufferSize)
{
    #ifdef NO_OPTIMIZE
    if (reverb == NULL || buffer == NULL) while(1);
    #endif

    for (int32_t i = 0; i < bufferSize; ++i)
    {
        float32_t y = 0.f;
        //  Process reverb feedback loop
        float32_t temp = MW_DSP_DelayLine_peek(&reverb->delayLines[3]);
        MW_AFXUnit_SVFilter_process(&reverb->feedbackLPF, &temp, 1);

        //  Shift input and feedback into the first nested APCF stage
        temp = buffer[i] + (temp * reverb->gain);
        temp = MW_DSP_NestedAPCF_tick(&reverb->nestedAPCFs[0], temp);
        y += (temp * 0.5f);

        //  Shift value into first delay line
        temp = MW_DSP_DelayLine_tick(&reverb->delayLines[0], temp);

        //  Shift value into standalone APCF stage
        temp = MW_DSP_APCF_tick(&reverb->standaloneAPCF, temp);

        //  Shift value into second delay line
        temp = MW_DSP_DelayLine_tick(&reverb->delayLines[1], temp);
        y += (temp * 0.5f);

        //  Shift value into third delay line
        temp = MW_DSP_DelayLine_tick(&reverb->delayLines[2], temp);
        temp = (temp * reverb->gain) + buffer[i];

        //  Shift value into last nested APCF
        temp = temp = MW_DSP_NestedAPCF_tick(&reverb->nestedAPCFs[1], temp);
        y += (temp * 0.5f);

        buffer[i] = y;

        //  Shift value into last delay line
        temp = MW_DSP_DelayLine_tick(&reverb->delayLines[3], temp);
    }
}