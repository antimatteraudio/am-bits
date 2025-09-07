#pragma once
#include <cmath>
#include "EnvelopeFollower.h"

class EnvelopeFollower
{
public:
    
    inline  EnvelopeFollower();
    inline ~EnvelopeFollower();
    
    inline void init(float hostSampleRate);
    inline void setParams();
    inline float process(float input);
    
    float attackMs = 10.0f;
    float decayMs = 100.0f;
    
private:
    float sampleRate = 44100.0f;
    float attack = 0.0f;
    float decay = 0.0f;
    float envelope = 0.0f;
};


//Constructor
EnvelopeFollower::EnvelopeFollower()
//Init vars
:
attackMs(10.0f),
decayMs(100.0f),
sampleRate(44100.0f),
attack(0.0f),
decay(0.0f),
envelope(0.0f)
{
};

//Desconstructor
EnvelopeFollower::~EnvelopeFollower()
{
};


void EnvelopeFollower::setParams()
{
    attackMs = std::fmax(attackMs, 0.01f);
    attack = std::exp(-1.0f / (attackMs * 0.001f * sampleRate));
    
    decayMs = std::fmax(decayMs, 0.01f);
    decay = std::exp(-1.0f / (decayMs * 0.001f * sampleRate));
}


float EnvelopeFollower::process(float input)
{
    float inputLevel = std::abs(input);
    if (inputLevel > envelope)
        envelope = attack * (envelope - inputLevel) + inputLevel;
    else
        envelope = decay * (envelope - inputLevel) + inputLevel;
    
    return envelope;
}


void EnvelopeFollower::init(float hostSampleRate) {
    envelope = 0.0f;
    sampleRate = hostSampleRate;
}

