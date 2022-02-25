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


#include "MW_AFXUnit_Granular.h"


void MW_AFXUnit_Granular_windowGrain(MW_AFXUnit_Granular *granular)
{
    #ifdef NO_OPTIMIZE
    if (granular == NULL)while(1);
    #endif

    if (granular == NULL)
        return;

    float32_t   slope;
    int32_t     midpoint;

    if ((granular->currentGrainSize - 1) % 2 == 0)
    {
        midpoint = (granular->currentGrainSize - 1) / 2;
        slope = 1.f / ((float32_t)midpoint);

        //  Apply first half of the envelope
        for (int32_t i = 0; i < (granular->currentGrainSize - 1) / 2; ++i)
            granular->grainBuffer[i] *= (slope * i);

        //  Apply seond half of the envelope
        for (int32_t i = midpoint; i < granular->currentGrainSize; ++i)
            granular->grainBuffer[i] *= (1.f - (slope * (i - midpoint)));
    }
    else
    {
        midpoint = (granular->currentGrainSize / 2) - 1;
        slope = 1.f / (float32_t)midpoint;

        //Again, apply the first half of the window
        for (int32_t i = 0; i < midpoint; ++i)
            granular->grainBuffer[i] *= (slope * i);

        for (int32_t i = midpoint + 1; i < granular->currentGrainSize; ++i)
            granular->grainBuffer[i] *= (1.f - (slope * (i - (midpoint + 1))));
    }
}



int32_t MW_AFXUnit_Granular_init(MW_AFXUnit_Granular *granular, float32_t *tapeBuffer, int32_t tapeBufferSize, float32_t grainSizeInSec, float32_t timeToChangeGrainInSec, float32_t fs)
{
    if (tapeBuffer == NULL || granular == NULL)
        return 0;
    
    if (fs <= 0)
        return 0;

    if (tapeBufferSize <= 0 || grainSizeInSec < 0 || timeToChangeGrainInSec < 0)
        return 0;

    int32_t grainSizeInSamples = fs * grainSizeInSec;
    if (grainSizeInSamples > MAX_GRAIN_BUFFER_SIZE)
        return 0;

    granular->currentGrainSize = grainSizeInSamples;

    int32_t numSamplesToChangeGrain = fs * timeToChangeGrainInSec;
    if (numSamplesToChangeGrain < 0)
        return 0;

    granular->tapeBuffer = tapeBuffer;
    granular->tapeBufferSize = tapeBufferSize;
    granular->tapeBufferPtr = 0;
    granular->numSamplesBetweenGrainChanges = numSamplesToChangeGrain;
    granular->fs = fs;
    granular->grainChangeCounter = 0;
    granular->grainBufferPtr = 0;
    granular->tapeBufferPtr = 0;
    granular->changeGrainSize = 0;
    granular->newGrainSize = 0;
    granular->changeTimeBetweenGrains = 0;


    arm_fill_f32(0, granular->grainBuffer, MAX_GRAIN_BUFFER_SIZE);
    arm_fill_f32(0, granular->tapeBuffer, granular->tapeBufferSize);

    return 1;
}


void MW_AFXUnit_Granular_changeGrainSize(MW_AFXUnit_Granular *granular, float32_t grainSizeInSec)
{
    #ifdef NO_OPTIMIZE
    if (granular == NULL) while(1);
    #endif

    if (granular == NULL) return;

    int32_t grainSizeInSamples = grainSizeInSec * granular->fs;
    if (grainSizeInSamples >= MAX_GRAIN_BUFFER_SIZE)
    {
        #ifdef NO_OPTIMIZE
        while(1);
        #endif

        grainSizeInSamples = grainSizeInSec * MAX_GRAIN_BUFFER_SIZE;
    }

    granular->changeGrainSize = 1;
    granular->newGrainSize = grainSizeInSamples;
}


void MW_AFXUnit_Granular_changeTimeToChangeGrain(MW_AFXUnit_Granular *granular, float32_t timeToChangeGrain)
{
    #ifdef NO_OPTIMIZE
    if (granular == NULL) while(1);
    #endif

    if (granular == NULL) return;

    int32_t numSamplesToChangeGrain = timeToChangeGrain * granular->fs;
    if (numSamplesToChangeGrain < 0)
    {
        #ifdef NO_OPTIMIZE
        while(1);
        #endif

        return;
    }

    granular->changeTimeBetweenGrains = 1;
    granular->newNumSamplesBetweenGrainChanges = numSamplesToChangeGrain;
}


void MW_AFXUnit_Granular_process(MW_AFXUnit_Granular *granular, float32_t *buffer, size_t bufferSize)
{
    #ifdef NO_OPTIMIZE
    if (granular == NULL || buffer == NULL) while(1);
    #endif

    if (granular == NULL || buffer == NULL) return;

    for (size_t i = 0; i < bufferSize; ++i)
    {
        granular->tapeBuffer[granular->tapeBufferPtr] = buffer[i];
        granular->tapeBufferPtr = (granular->tapeBufferPtr + 1) % granular->tapeBufferSize;

        buffer[i] = granular->grainBuffer[granular->grainBufferPtr++];

        granular->grainChangeCounter++;

        //  If we reached the end of the grain buffer, check to see if we need to change the grain contents
        if (granular->grainBufferPtr >= granular->currentGrainSize)
        {
            granular->grainBufferPtr = 0;

            if (granular->grainChangeCounter >= granular->numSamplesBetweenGrainChanges)
            {
                if (granular->changeGrainSize)
                {
                    granular->currentGrainSize = granular->newGrainSize;
                    granular->changeGrainSize = 0;
                }

                granular->grainChangeCounter = 0;

                //  Copy data from the tape buffer to the grain buffer
                int32_t keepOutIndex = granular->tapeBufferSize - granular->currentGrainSize;
                int32_t randomIndexUnMapped = rand();

                int32_t randomIndexMapped = (int32_t)MW_AFXUnit_Utils_mapToRange(randomIndexUnMapped, 0, __RAND_MAX, 0, keepOutIndex);
                int32_t startIndex = (granular->tapeBufferPtr + randomIndexMapped) % granular->tapeBufferSize;

                int32_t numSamplesRemaining = granular->tapeBufferSize - startIndex;
                if (numSamplesRemaining >= granular->currentGrainSize)
                    arm_copy_f32(&granular->tapeBuffer[startIndex], granular->grainBuffer, granular->currentGrainSize);
                else
                {
                    arm_copy_f32(&granular->tapeBuffer[startIndex], granular->grainBuffer, numSamplesRemaining);
                    arm_copy_f32(granular->tapeBuffer, &granular->grainBuffer[numSamplesRemaining], granular->currentGrainSize - numSamplesRemaining);
                }

                //  Window grain buffer to avoid audio clicks
                MW_AFXUnit_Granular_windowGrain(granular);
            }

            //  Check to see if we need to change the time between grain changes
            if (granular->changeTimeBetweenGrains)
            {
                granular->changeTimeBetweenGrains = 0;
                granular->numSamplesBetweenGrainChanges = granular->newNumSamplesBetweenGrainChanges;
            }
        }
    }
}
