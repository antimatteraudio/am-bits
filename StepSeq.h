/*
  ==============================================================================

    StepSeq.h
    Created: 28 Jun 2025 9:44:18am
    Author:  jeph

  ==============================================================================
*/

#include <JuceHeader.h>
#include <vector>
//#include "AM_Slew.h"
//#include "Parameters.h"
//#include "ParamUtils.h"

#pragma once

class StepSeq
{
public:
    //Constructor/deconstructor
    inline StepSeq();
    inline ~StepSeq();
    
    //inline static Parameters parameters;
    //inline static ParamUtils utils;
    
    inline void init(int SlewMs, float hostSampleRate);
    
    //inline void setParams(ParamUtils::seqParams &sp);
    
    inline float process(float loopPPQ);
    
   inline  float process_ppqRamp(float loopPPQ);
    
    //AM_Slew outSlew;
    
    float divisor = 0;
    int newStep = 0;
    float prevPpqOut = 0;
    int length = 0;
    float prevOut = 0;
    int step = 0;
    int slewMs = 0;
    float amp = 0;
    int syncType = 0;
    float hz = 0;
    float sampleRate = 0;
    int rampSampleSize = 0;
    int rampSamples = 0;
    int reset = 0;
    
    //{"2", "1", "1/2", "1/3","1/4", "1/8", "1/16", "1/32", "1/64"}
    const double syncMults[9] = {0.125, 0.25, 0.5, 0.75,1, 2, 4, 8, 16};
    
std::vector<float> values;
    
private:
    
};


StepSeq::StepSeq()
//Init vars
:divisor(0),
newStep(0),
prevPpqOut(0),
length(0),
prevOut(0),
step(0),
slewMs(0),
amp(0),
syncType(0),
hz(0),
sampleRate(0),
rampSampleSize(0),
rampSamples(0),
reset(0)

{
    values.clear();
}

//Desconstructor
StepSeq::~StepSeq()
{
};

void StepSeq::init(int SlewMs, float hostSampleRate) {
    
    sampleRate = hostSampleRate;
    
    //outSlew.Init(SlewMs, hostSampleRate);
    values.clear();
    values.resize(16);
    
    //Remove these once setvars is good to go
    length = 16;
    divisor = 4;
    
    newStep = 0;
    prevPpqOut = 0;
    prevOut = 0;
    step = 0;
    slewMs = 0;
    
    reset = 1;
}

//void StepSeq::setParams(ParamUtils::seqParams &sp) {
//
//    amp = utils.minMax(sp.amp , parameters.range[seq1_amp].start, parameters.range[seq1_amp].end);
//
//    for (int i = 0; i < 16; i++) {
//        float val = sp.values[i];
//        val = utils.minMax(val , parameters.range[seq1_step1].start, parameters.range[seq1_step1].end);
//        values[i] = val;
//    }
//
//    //Low, High, Synced, Sixteenths, Thirds
//    syncType = utils.minMax(sp.syncType , parameters.range[seq1_syncType].start, parameters.range[seq1_syncType].end);
//
//    int syncChoice = utils.minMax(sp.syncRate , parameters.range[seq1_syncRate].start, parameters.range[seq1_syncRate].end);
//    int thirds = utils.minMax(sp.thirds , parameters.range[seq1_thirds].start, parameters.range[seq1_thirds].end);
//    int sixteenths = utils.minMax(sp.sixteenths , parameters.range[seq1_sixteenths].start, parameters.range[seq1_sixteenths].end);
//
//    if (syncType == 0) hz = utils.minMax(sp.hertz , parameters.range[seq1_hertz].start, parameters.range[seq1_hertz].end);
//    if (syncType == 2) hz = utils.minMax(sp.hertzHi , parameters.range[seq1_hertzHi].start, parameters.range[seq1_hertzHi].end);
//    if (syncType == 2) divisor = syncMults[syncChoice]; //Normal sync menu
//    if (syncType == 3) divisor = 0.75 / float(thirds + 1); //1/3 times
//    if (syncType == 4) divisor = 4.0 / float(sixteenths + 1); //1/16 times
//
//}

float StepSeq::process(float loopPPQ){
    
    float out = prevOut;
    
    if (syncType > 1) { //Synced times
        process_ppqRamp(loopPPQ);
    } else {
        //increment ramp per sample, reset random value once per cycle
        rampSamples += 1;
        if (rampSamples > rampSampleSize) {
            rampSamples = (rampSamples - rampSampleSize) -1;
            newStep = 1;
        }
        
        rampSampleSize = sampleRate / hz;
        
        if (reset == 1) {
            rampSamples = 0;
            reset = 0;
        }
    }
    
    //if (syncType == 1) newStep = 1; //Audio rate
    
    //New step started
    if(newStep == 1) {
        //Increment step
        step += 1;
        if(step >= length) {
            step = 0;
        }
        //Output from array of values
        out = values[step];
        newStep = 0;
    }
    
    //out = outSlew.Process(out);
    
    prevOut = out;

    return out;
}

//Reference ppqramp from the host
float StepSeq::process_ppqRamp(float loopPPQ) {
    float outPut = 0.000;
    outPut = fmod(loopPPQ, 1 / divisor);
    outPut = outPut * divisor;
    
    if (abs(prevPpqOut - outPut) > 0.5) {
        newStep = 1;
        outPut = 0;
    }
    
    prevPpqOut = outPut;
    return outPut;
}


    

