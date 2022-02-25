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


#include "MW_AFXUnit_Leslie.h"


/*
 *  Initialize an instance of AFXUnit_Leslie
 *  Note that MW_AFXUnit_Leslie_init() will NOT automatically allocate memory for its internal delay line.  
 *  You must pass in a block of preallocated memory. 
 * 
 *  This version of the Leslie speaker is actually quite different from a conventional leslie speaker.
 *  The sound is more like a large rotating loudhorn instead of a small rapidly rotating horn in an enclosure
 *  but I still call it a Leslie effect since the driving principles are more or less the same
 * 
 *  Inputs:
 *      leslie:             Pointer to instance of MW_AFXUnit_Leslie
 *      delayLineBuffer:    Pointer to preallocated buffer for internal delay line
 *      delayLineLength:    Length of delay line buffer
 *      fs:                 Sampling frequency
 *      rpm:                Rotation speed (RPM)
 * 
 *  Returns:
 *      0:  If initialization unsuccessful
 *      1:  Otherwise
 */
int32_t MW_AFXUnit_Leslie_init(MW_AFXUnit_Leslie *leslie, float32_t *delayLineBuffer, int16_t delayLineLength, float32_t fs, float32_t rpm)
{
    if (leslie == NULL || delayLineBuffer == NULL)
        return 0;

    if (delayLineLength < 0 || fs < 0 || rpm < 0)
        return 0;

    leslie->M = (float32_t)delayLineLength / 2.f;

    int32_t success = MW_DSP_FractionalDelayLine_init(&leslie->delay, delayLineBuffer, delayLineLength, leslie->M);
    if (!success)
        return 0;

    //  Initialize filters for loudspeaker filter effect
    success = MW_AFXUnit_Biquad_init(&leslie->biquads[0], MW_BIQUAD_PARAM_EQ_CQ, fs, 525.f, 1.1f, 8.7f, NULL, 0);
    if (!success)
        return 0;

    success = MW_AFXUnit_Biquad_init(&leslie->biquads[1], MW_BIQUAD_PARAM_EQ_CQ, fs, 975.f, 1.6f, 22.f, NULL, 0);
    if (!success)
        return 0;

    success = MW_AFXUnit_Biquad_init(&leslie->biquads[2], MW_BIQUAD_PARAM_EQ_CQ, fs, 1570.f, 8.2f, 6.2f, NULL, 0);
    if (!success)
        return 0;

    success = MW_AFXUnit_Biquad_init(&leslie->biquads[3], MW_BIQUAD_PARAM_EQ_CQ, fs, 2460.f, 4.f, 10.6f, NULL, 0);
    if (!success)
        return 0;

    leslie->phaseConstant = PI / fs;
    leslie->phaseIncrement = leslie->phaseConstant * rpm * 0.0167f;
    leslie->angularVelocity = rpm * 0.105;
    leslie->phase = 0.f;
    leslie->a = 2.f * PI * arm_sin_f32(PI * rpm * 0.0167f / fs);
    leslie->s[0] = 0.5f;
    leslie->s[1] = 0.f;

    return 1;
}


/*
 *  Change rotation speed of the Leslie speaker
 * 
 *  Inputs:
 *      leslie:     Pointer to MW_AFXUnit_Leslie instance
 *      rpm:        Rotational speed (RPM)
 * 
 *  Returns:
 *      None
 */
void MW_AFXUnit_Leslie_changeParameters(MW_AFXUnit_Leslie *leslie, float32_t rpm)
{
#ifdef NO_OPTIMIZE
    assert(leslie != NULL);
    assert(rpm >= 0);
#endif

    leslie->phaseIncrement = leslie->phaseConstant * rpm * 0.0167 * 2.f;
    leslie->angularVelocity = rpm * 0.105;
    leslie->a = 2.f * PI * arm_sin_f32(leslie->phaseConstant * rpm * 0.0167f);

    return;
}


/*
 *  Process a block of samples
 * 
 *  Inputs:
 *      leslie:         Pointer to MW_AFXUnit_Leslie instance
 *      buffer:         Buffer of samples to process
 *      bufferSize:     Number of samples to process
 * 
 *  Returns:
 *      None
 */
void MW_AFXUnit_Leslie_process(MW_AFXUnit_Leslie *leslie, float32_t *buffer, size_t bufferSize)
{
#ifdef NO_OPTIMIZE
    assert(leslie != NULL);
    assert(buffer != NULL);
#endif

    //  TODO:  Apply signal to BPF bank
    for (int32_t i = 0; i < 4; ++i)
        MW_AFXUnit_Biquad_process(&leslie->biquads[i], buffer, bufferSize);

    for (size_t i = 0; i < bufferSize; ++i)
    {
        float32_t y = MW_DSP_FractionalDelayLine_tick(&leslie->delay, buffer[i]);

        leslie->s[0] = leslie->s[0] - (leslie->a * leslie->s[1]);
        leslie->s[1] = leslie->s[1] + (leslie->a * leslie->s[0]);
        
        //  Apply modulation envelope to output signal and mix with dry signal
        //buffer[i] = (0.2f * buffer[i]) + (y * envelope);
        buffer[i] = (0.1f * buffer[i]) + (y * (1.f + leslie->s[1]));
        // buffer[i] = (y * (1.f + leslie->s[1]));
        //buffer[i] = 0.2f * buffer[i] + (y * (1.5f + leslie->s[1]));

        //  Modify delay line length based on the physical model of a leslie speaker
        float32_t newDelayLineLength = leslie->M + (-1.5f * leslie->angularVelocity * leslie->s[1]);
        //float32_t newDelayLineLength = leslie->M;
        //float32_t newDelayLineLength = leslie->M + (-0.07 * leslie->angularVelocity * lfoValue);
        MW_DSP_FractionalDelayLine_setDelayLength(&leslie->delay, newDelayLineLength);

        // buffer[i] = y;
    }

    return;
}
