#pragma once

#include <pch.h>
#include "../Distortion/Distortion.h"
#include "VariableDelay.h"

/**
 * Audio processor that implements delay line with feedback,
 * including filtering and distortion in the feedback path
 */ 
class DelayProc
{
public:
    DelayProc() = default;

    // processing functions
    void prepare (const dsp::ProcessSpec& spec);
    void reset();

    template<typename ProcessContext>
    void process (const ProcessContext& context);

    // flush delay line state
    void flushDelay();

    struct Parameters
    {
        float delayMs;
        float feedback;
        float lpfFreq;
        float hpfFreq;
        float distortion;
        float modFreq;
        float modDepth;
    };

    void setParameters (const Parameters& params, bool force = false);
    void setDelayType (VariableDelay::DelayType type) { delay.setDelayType (type); }
    float getModDepth() const noexcept { return 1000.0f * delayModValue / fs; }

private:
    template<typename SampleType>
    inline SampleType processSample (SampleType x, size_t ch);

    template<typename SampleType>
    inline SampleType processSampleSmooth (SampleType x, size_t ch);

    VariableDelay delay { 1 << 19 };

    float fs = 44100.0f;
    
    SmoothedValue<float, ValueSmoothingTypes::Linear> delaySmooth;
    SmoothedValue<float, ValueSmoothingTypes::Linear> feedback;
    SmoothedValue<float, ValueSmoothingTypes::Linear> inGain;
    std::vector<float> state;

    enum
    {
        lpfIdx,
        hpfIdx,
        distortionIdx,
    };

    MyProcessorChain<
        chowdsp::IIR::Filter<float, 1>,
        chowdsp::IIR::Filter<float, 1>,
        Distortion
        > procs;

    chowdsp::SineWave<float> modSine;
    float delayModValue = 0.0f;
    float modDepth = 0.0f;
    float modDepthFactor = 1.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayProc)
};
