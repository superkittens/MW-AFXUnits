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


#include "MW_AFXUnit_Flutter.h"



/*
 *  Initialize an instancce of MW_AFXUnit_Flutter
 *  MW_AFXUnit_Flutter_init() will NOT automatically allocate memory for its internal delay line!
 *  You must pass a pointer to a pre-allocated array in the buffer parameter
 * 
 *  Inputs:
 *      flutter:        Instance to MW_AFXUnit_Flutter instance
 *      buffer:         Pointer to array that will hold delayed samples
 *      M:              Desired delay length size (can be fractional)
 *      N:              Total delay length size - ie. size of buffer array (cannot be fractional)
 *      lfoDepth:       LFO modulation depth.  Take care that your depth does not stray out of the delay line bounds
 *      lfoFrequency:   LFO frequency.  Limited to MW_AFXUNIT_FLUTTER_MIN_LFO_FREQ to Nyquist rate
 *      lfoPhase:       LFO phase in radians
 *      b0:             Flutter output gain
 * 
 *  Returns:
 *      0 if initialization unsuccessful
 *      1 otherwise
 */
int32_t MW_AFXUnit_Flutter_init(MW_AFXUnit_Flutter *flutter, float32_t *buffer, float32_t fs, float32_t M, int16_t N, float32_t lfoDepth, float32_t lfoFrequency, float32_t b0)
{
    if (flutter == NULL || buffer == NULL)
        return 0;

    if (M > N || M < 0 || N == 0)
        return 0;

    if (lfoFrequency > (fs / 2.f) || lfoFrequency < MW_AFXUNIT_FLUTTER_MIN_LFO_FREQ)
        return 0;

    if (lfoDepth + M > N || M - lfoDepth < 0)
        return 0;

    int32_t success = MW_DSP_FractionalDelayLine_init(&flutter->delay, buffer, N, M);
    if (!success)
        return 0;
    
    flutter->lfoSamplesPerCycle = (int32_t)(fs / lfoFrequency);
    flutter->fs = fs;
    flutter->lfoDepth = lfoDepth;
    flutter->lfoFrequency = lfoFrequency;
    flutter->lfoPhaseCounter = 0;
    flutter->b0 = b0;
    flutter->lfoPhaseIncrement = 2.f * PI / flutter->lfoSamplesPerCycle;
    flutter->M = M;

    return 1;
}


/*
 *  Change LFO settings in the flutter effect
 *  One word of caution, changing parameters will likely result in audio artifacts as there is no smoothing implmented (yet)
 *  The option to change parameters is available but be warned!
 * 
 *  Inputs:
 *      flutter:        Pointer to MW_AFXUnit_Flutter instance
 *      lfoDepth:       LFO modulation depth.  Take care that your depth does not stray out of the delay line bounds
 *      LFO frequency.  Limited to MW_AFXUNIT_FLUTTER_MIN_LFO_FREQ to Nyquist rate
 *      lfoPhase:       LFO phase in radians
 *      b0:             Flutter output gain
 * 
 *  Returns:
 *      None
 */ 
void MW_AFXUnit_Flutter_changeParameters(MW_AFXUnit_Flutter *flutter, float32_t lfoDepth, float32_t lfoFrequency, float32_t b0)
{
#ifdef NO_OPTIMIZE
    assert(flutter != NULL);
    assert(lfoDepth + flutter->delay.M < flutter->delay.delay.N);
    assert(lfoFrequency >= MW_AFXUNIT_FLUTTER_MIN_LFO_FREQ);
    assert(lfoFrequency <= (flutter->fs / 2.f))
#endif

    flutter->lfoDepth = lfoDepth;
    flutter->lfoFrequency = lfoFrequency;
    flutter->b0 = b0;

    flutter->lfoSamplesPerCycle = (int32_t)(flutter->fs / lfoFrequency);
    flutter->lfoPhaseCounter = 0;
}


/*
 *  Process a block of samples
 * 
 *  Inputs:
 *      flutter:        Pointer to MW_AFXUnit_Flutter instance
 *      buffer:         Buffer of samples to process
 *      bufferSize:     Number of samples to process
 * 
 *  Returns:
 *      None
 */
void MW_AFXUnit_Flutter_process(MW_AFXUnit_Flutter *flutter, float32_t *buffer, size_t bufferSize)
{
#ifdef NO_OPTIMIZE
    assert(flutter != NULL);
    assert(buffer != NULL);
#endif

    for (int32_t i = 0; i < bufferSize; ++i)
    {
        buffer[i] = MW_DSP_FractionalDelayLine_tick(&flutter->delay, buffer[i]) * flutter->b0;
        //  TODO:  Use more optimized version of sin
        MW_DSP_FractionalDelayLine_setDelayLength(&flutter->delay, flutter->M + (flutter->lfoDepth * arm_sin_f32(flutter->lfoPhaseCounter)));

        flutter->lfoPhaseCounter += flutter->lfoPhaseIncrement;
    }

    if (flutter->lfoPhaseCounter > 2.f * PI)
        flutter->lfoPhaseCounter -= (2.f * PI);
}


void MW_AFXUnit_Flutter_reset(MW_AFXUnit_Flutter *flutter)
{
    #ifdef NO_OPTIMIZE
    if (flutter == NULL) while(1);
    #endif

    if (flutter == NULL) return;

    flutter->lfoPhaseCounter = 0;
    MW_DSP_FractionalDelayLine_reset(&flutter->delay);
}
