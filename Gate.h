/*
  ==============================================================================

    EnvGate.h
    Created: 28 Aug 2025 10:30:42am
    Author:  jeph

  ==============================================================================
*/

#pragma once
#include <cmath>

class GateDetect
{
public:
    
    inline GateDetect();
    inline ~GateDetect();
    
    inline int process(float input);
    inline void init(float hostSampleRate);
    
    inline void setParams();
    
    float prevInput = 0;
    float gateThresh = 0;
    int gateState = 0;
    float gateHyst = 0;
    float gateHoldCount = 0;
    float holdMs = 0;
    float oneMs = 0;
    
private:
    
    
};


//Constructor
GateDetect::GateDetect()
//Init vars
:
prevInput(0),
gateThresh(0),
gateState(0),
gateHyst(0),
gateHoldCount(0),
holdMs(0)
{
};

//Desconstructor
GateDetect::~GateDetect()
{
};

void GateDetect::init(float hostSampleRate) {
    oneMs = hostSampleRate / 1000;
}

void GateDetect::setParams()
{
    holdMs *= oneMs;
    gateHyst = gateThresh - 20; //move to set params
    if(gateHyst < 0) gateHyst = 0;
}

int GateDetect::process(float input)
{
    float gateAv = 0.0;
    
    gateAv = (input + prevInput) / 2;
    
    if (abs(gateAv) >= gateThresh) {
            gateHoldCount = 0;
            gateState = 1;
        }
    
    if(gateHoldCount > holdMs) {
        if (gateAv < (gateThresh - gateHyst)) {
            if (gateState == 1 ) {
                gateHoldCount = 0;
                gateState = 0;
            }
        }
    }
    
    gateHoldCount +=1;
    
    prevInput = input;
    return gateState;
};




