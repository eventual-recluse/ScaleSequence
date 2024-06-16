#ifndef SCALESEQUENCE_CONTROLS_HPP
#define SCALESEQUENCE_CONTROLS_HPP

#include <array>

template <class T>
T limit (const T x, const T min, const T max)
{
    return (x < min ? min : (x > max ? max : x));
}

enum Parameters {
    kParameterMeasure =    0,
    kParameterMultiplier = 1,
    kParameterScaleGlide = 2,
    kParameterStep1      = 3,
    kParameterStep2      = 4,
    kParameterStep3      = 5,
    kParameterStep4      = 6,
    kParameterStep5      = 7,
    kParameterStep6      = 8,
    kParameterStep7      = 9,
    kParameterStep8      = 10,
    kParameterStep9      = 11,
    kParameterStep10     = 12,
    kParameterStep11     = 13,
    kParameterStep12     = 14,
    kParameterStep13     = 15,
    kParameterStep14     = 16,
    kParameterStep15     = 17,
    kParameterStep16     = 18,
    kParameterOffset     = 19,
    kParameterCurrentStep = 20,
    kParameterCount      = 21
};

enum States {
    kStateFileSCL1 = 0,
    kStateFileSCL2 = 1,
    kStateFileSCL3 = 2,
    kStateFileSCL4 = 3,
    kStateFileKBM1 = 4,
    kStateFileKBM2 = 5,
    kStateFileKBM3 = 6,
    kStateFileKBM4 = 7,
    kStateCount    = 8
};

static const std::array<std::pair<float, float>, kParameterCount> controlLimits =
{{
    {0.0f, 1.0f},    //kParameterMeasure
	{1.0f, 12.0f},   //kParameterMultiplier,
	{1.0f, 100.0f}, //kParameterScaleGlide
    {1.0f, 4.0f},    //kParameterStep1,
    {1.0f, 4.0f},    //kParameterStep2,
    {1.0f, 4.0f},    //kParameterStep3,
    {1.0f, 4.0f},    //kParameterStep4,
    {1.0f, 4.0f},    //kParameterStep5,
    {1.0f, 4.0f},    //kParameterStep6,
    {1.0f, 4.0f},    //kParameterStep7,
    {1.0f, 4.0f},    //kParameterStep8,
    {1.0f, 4.0f},    //kParameterStep9,
    {1.0f, 4.0f},    //kParameterStep10,
    {1.0f, 4.0f},    //kParameterStep11,
    {1.0f, 4.0f},    //kParameterStep12,
    {1.0f, 4.0f},    //kParameterStep13,
    {1.0f, 4.0f},    //kParameterStep14,
    {1.0f, 4.0f},    //kParameterStep15,
    {1.0f, 4.0f},    //kParameterStep16,
    {-1.0f, 1.0f},   //kParameterOffset
    {0.0f, 1.0f}    //kParameterCurrentStep
}};

static const float ParameterDefaults[kParameterCount] = {
	0.0f, //kParameterMeasure
	1.0f, //kParameterMultiplier,
	1.0f, //kParameterScaleGlide
    1.0f, //kParameterStep1,
    1.0f, //kParameterStep2,
    1.0f, //kParameterStep3,
    1.0f, //kParameterStep4,
    1.0f, //kParameterStep5,
    1.0f, //kParameterStep6,
    1.0f, //kParameterStep7,
    1.0f, //kParameterStep8,
    1.0f, //kParameterStep9,
    1.0f, //kParameterStep10,
    1.0f, //kParameterStep11,
    1.0f, //kParameterStep12,
    1.0f, //kParameterStep13,
    1.0f, //kParameterStep14,
    1.0f, //kParameterStep15,
    1.0f, //kParameterStep16,
    0.0f, //kParameterOffset
    1.0f  //kParameterCurrentStep (default not used)
	
};


#endif
