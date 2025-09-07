//There will need to be timing compensation if oversampling is used

#pragma once

#include <JuceHeader.h>
#include "Waveshapes.h"
//#include "Slew.h"
//#include "Parameters.h"
//#include "ParamUtils.h"
//#include "LFO_Proc.h"

#define PI 3.14159265

class SyncedLFO
{
public:
    
    //inline static Parameters parameters;
    //inline static ParamUtils utils;
    
    //Constructor/deconstructor
    inline SyncedLFO();
    inline ~SyncedLFO();
    
    //Methods
    //inline void setParams(ParamUtils::lfoParams &lp);
    
    inline double process_SyncedLFO();
    
    //double process_ppqRamp(double ppqLoop, double syncMult);
    inline double process_ppqRamp(float loopPPQ); //Creates ramp based on host timing (which is a lower resolution)
    
    inline double process_LFO();
    
    inline double process(float loopPpq);
    
    inline void init(int SlewMs, float hostSampleRate,float hostBPM);
    
    int rampSampleSize = 0;
    
    int rampSamples = 0;
    int prevRampSamples = 0;
    
    float randParam = 0;
    float randAmount = 1023;

    float sampleRate = 0;
    int syncChoice = 0;
    int reset = 0;
    float ppqRamp = 0;
    float bpm = 0;
    float phaseMod = 0;
    float width = 0;
    int shape = 0;
    float lfoHz = 0;
    float amplitude = 0;
    float syncType = 0;
    float ppqLoop = 0;
    float prevPpqOut = 0;
    int slewTime = 0;
    float divisor = 0;
    float prevOut = 0;
    int biPolar = 0;
    float SyncOffset = 0;
    
    //{"2", "1", "1/2", "1/3","1/4", "1/8", "1/16", "1/32", "1/64"}
    const double syncMults[9] = {0.125, 0.25, 0.5, 0.75,1, 2, 4, 8, 16};
    
private:
    
    inline double FullRamp(float phaseMod, float shape);
    
    inline double xfade(float newValue, float currentValue, double ms, double sampleRate);
    
    inline double waveCalc(int waveshape, int input);
    
    inline void VarAudit(float sampleRate);
    
    
    juce::Random ampRandom;
    
    AM_Slew phaseSlew;
    AM_Slew rateSlew;
    //AM_Slew ampSlew;
    //AM_Slew outSlew;
    AM_Slew widthSlew;
    
};

//-----------------------------------------

SyncedLFO::SyncedLFO()
//Init va
:rampSampleSize(0),
rampSamples(0),
randParam(0),
randAmount(0),
syncChoice(0),
reset(0),
ppqRamp(0),
bpm(0),
phaseMod(0),
width(0),
shape(0),
lfoHz(0),
amplitude(0),
syncType(0),
ppqLoop(0),
prevPpqOut(0),
slewTime(0),
divisor(0),
prevOut(0),
biPolar(0),
SyncOffset(0)
{
}

//Desconstructor
SyncedLFO::~SyncedLFO()
{
};

void SyncedLFO::init(int SlewMs, float hostSampleRate, float hostBPM) {
    sampleRate = hostSampleRate;
    slewTime = SlewMs;
    phaseSlew.Init(SlewMs * 4, sampleRate,10);
    rateSlew.Init(100, sampleRate, 10);
    widthSlew.Init(SlewMs * 10, sampleRate,10);
    //ampSlew.Init(SlewMs, sampleRate, 10);
    //outSlew.Init(0.15, sampleRate, 10);
    rampSamples = 0;
    bpm = hostBPM;
    reset = 1;
}

//Translate vector params
//void SyncedLFO::setParams(ParamUtils::lfoParams& lp) {
//
//    biPolar = 0; //lp.biPolar; Handled by matrix
//    amplitude = 1; //Moved to matrix
//
//    phaseMod = utils.minMax(lp.phaseMod , parameters.range[LFO1_Phase].start, parameters.range[LFO1_Phase].end);
//    phaseMod = phaseMod / 100;
//    phaseMod *= -1; //Invert phasemod so that it is more intuitive (+ pushes the wave forward)
//
//    width = utils.minMax(lp.width , parameters.range[LFO1_Width].start, parameters.range[LFO1_Width].end);
//    width = 1 - width;
//
//    shape = utils.minMax(lp.shape , parameters.range[LFO1_Wave].start, parameters.range[LFO1_Wave].end);
//    randParam = utils.minMax(lp.random , parameters.range[LFO1_Rand].start, parameters.range[LFO1_Rand].end);
//    syncType = utils.minMax(lp.syncType , parameters.range[LFO1_SyncType].start, parameters.range[LFO1_SyncType].end);
//
//    if(syncType == 0) lfoHz = utils.minMax(lp.lfoHz , parameters.range[LFO1_Hz].start, parameters.range[LFO1_Hz].end);
//    if(syncType == 1) lfoHz = utils.minMax(lp.lfoHz , parameters.range[LFO1_Hz_Hi].start, parameters.range[LFO1_Hz_Hi].end);
//
//    syncChoice = utils.minMax(lp.syncChoice , parameters.range[LFO1_SyncRate].start, parameters.range[LFO1_SyncRate].end);
//
//    SyncOffset = utils.minMax(lp.syncOffset , parameters.range[LFO1_Sync_Offset].start, parameters.range[LFO1_Sync_Offset].end);
//    SyncOffset /= 100;
//
//    //"Low","High","Synced","Dotted","Fast Dot","Thirds","Sixteenths"
//    if (syncType == 2) divisor = syncMults[syncChoice]; //Normal sync menu
//    if (syncType == 3) divisor = 0.75 / float(syncChoice + 1); //1/3 times
//    if (syncType == 4) divisor = 4.0 / float(syncChoice + 1); //1/16 times
//    if (SyncOffset != 0) divisor =  (divisor - (divisor * (SyncOffset /2))) * -1;
//}

double SyncedLFO::process(float loopPPQ) {
    
    ppqLoop = loopPPQ;
    
    double out = 0;
    
    if (syncType < 2) {
        out = process_LFO();
    } else {
        out = process_SyncedLFO();
        ppqRamp = process_ppqRamp(ppqLoop);ppqRamp = process_ppqRamp(ppqLoop);
    }
    
    out = FullRamp(phaseMod,shape);
    
    //double ampAverage = ampSlew.Process(amplitude);
    //double ampAverage = amplitude;
    //out *= ampAverage;
    
    return out;
}

double SyncedLFO::process_SyncedLFO() {
    
    //Adjust sampleRate for BPM (using sampleRate for accuracy since it is the biggest number in the calc)
    float bpmMult = 0;     //This only needs to run on a bpm change
    bpmMult =  120 / bpm;
    
    float bpmSampleRate = sampleRate * bpmMult; //Mult the sampleRate * adjusted bpm. 120 bpm = 1
    
    //ramp size in samples for current division. This only needs to be calced on a divisor change
    rampSampleSize = std::round(bpmSampleRate / (divisor * 2)) -1;
    
    //increment ramp per sample
    rampSamples += 1;
    if (rampSamples > rampSampleSize) {
        rampSamples = (rampSamples - rampSampleSize) -1;
    }
    
    if (reset == 1) {
        rampSamples = 0;
        reset = 0;
    }
    
    //!Do not delete if (abs(rampSamples - prevRampSamples) > 1 && abs(rampSamples - prevRampSamples) < 1000) //Catches the reset glitch
    
    prevRampSamples = rampSamples;
    return FullRamp(phaseMod,shape);
}

//it is legit to share vars between sync/unsync since only one will be active, but may need to init vars when switching between them
double SyncedLFO::process_LFO(){
    
    //increment ramp per sample, reset random value once per cycle
    rampSamples += 1;
    if (rampSamples > rampSampleSize) {
        rampSamples = (rampSamples - rampSampleSize) -1;
    }
    
    double lfoHzAverage = rateSlew.Process(lfoHz);
    //double lfoHzAverage = lfoHz;
    
    rampSampleSize = sampleRate / lfoHzAverage;
    
    if (reset == 1) {
        rampSamples = 0;
        reset = 0;
    }
    
    float out = FullRamp(phaseMod,shape);
    
    return out;
}

//Calc a higher-res ramp
double SyncedLFO::FullRamp(float phaseMod, float shape){
    
    //Calc the ramp table position based on the % the ramp has completed
    double rampOut = (double)rampSamples  / (double)rampSampleSize;
    
    int rampTablePos = std::round(rampOut * 1023);
    
    double phaseModAverage = phaseSlew.Process(phaseMod);
    rampTablePos += (phaseModAverage * 1023);
    
    if (rampTablePos > 1023) {
        rampTablePos = fmod(rampTablePos, 1023);
        //rampTablePos = 0; //PWM-ish with + 400 or whatever on the rampTablePos
    }
    if (rampTablePos > 1023) {
        rampTablePos = 1023;
    }
    if (rampTablePos < 0) rampTablePos = 1023 - abs(rampTablePos);
    if (rampTablePos < 0) {
        rampTablePos = 0;
    }
    
    double out = waveCalc(shape, rampTablePos);
    
    //VarAudit(sampleRate);
    
    //out = outSlew.Process(out);
    
    return out;
}

//Get waveshape, set random value
double SyncedLFO::waveCalc(int waveshape, int input) {
    
    double out = 0.0f;
    
    if(waveshape == 0 || waveshape == 3 || waveshape == 5  || waveshape == 6) input = 1023 - input;
    
    float smoothWidth = widthSlew.Process(width);
    
    int pwmInput = input * (1 + (4 * smoothWidth));
    
    if(pwmInput > 1023) {
        pwmInput = 0;
    }
    
    if(waveshape == 0) {
        out = sineTable[pwmInput]; //Sine
        if(prevOut < 0 && out >= 0) randAmount = randParam * ampRandom.nextFloat();
        if(prevOut > 0 && out <= 0) randAmount = randParam * ampRandom.nextFloat();
    }
    
    if(waveshape == 1) { //Tri
        out = (pwmInput * 2) > 1023 ? rampTable[2046 - (pwmInput * 2)] : rampTable[pwmInput * 2];
        if(prevOut < 0 && out >= 0) randAmount = randParam * ampRandom.nextFloat();
        if(prevOut > 0 && out <= 0) randAmount = randParam * ampRandom.nextFloat();
    }
    
    if(waveshape == 2) { //Ramp
        out = rampTable[pwmInput];
        if(abs(prevOut - out) > .3) randAmount = randParam * ampRandom.nextFloat();
    }
    
    if(waveshape == 3) { //Reverse ramp
        out = rampTable[pwmInput] ; // * -1
        if(abs(prevOut - out) > .3) randAmount = randParam * ampRandom.nextFloat();
    }
    
    if(waveshape == 4) {
        out = expTable[pwmInput]; //Exp
        //out *= 1.1; //The out slew makes the EXP lose a little amplitude
        if(abs(prevOut - out) > .3) randAmount = randParam * ampRandom.nextFloat();
    }
    
    if(waveshape == 5) { //Reverse exp
        out = expTable[pwmInput];
        //out *= 1.1; //The out slew makes the EXP lose a little amplitude
        if(abs(prevOut - out) > .3) randAmount = randParam * ampRandom.nextFloat();
    }
    
    if(waveshape == 6) {  //Square
        //if(input > (round(pwm * 1000))) {
        if(pwmInput > 511) {
            out = 1.0;
        } else {
            out = 0.0;
        }
        if(abs(prevOut - out) > .3) randAmount = randParam * ampRandom.nextFloat();
    }
    
    prevOut = out;
    
    out *= (1 - randAmount);
    
    if(biPolar == 1) out = out - 0.5;
    
    return out;
}

//Reference ppqramp from the host
double SyncedLFO::process_ppqRamp(float loopPPQ) {
    float outPut = 0.000;
    outPut = fmod(loopPPQ, 1 / divisor);
    outPut = outPut * divisor;
    
    if (abs(prevPpqOut - outPut) > 0.5) {
        reset = 1; //Sometimes may cause an endless reset loop? Try adding && prevReset != 1 to the if statement?
        outPut = 0;
    }
    
    prevPpqOut = outPut;
    return outPut;
}

void SyncedLFO::VarAudit(float sampleRate) {
    if (rampSamples > rampSampleSize || rampSamples < 0) {
        std::cout << "Rampsamples " << rampSamples << "/" << rampSampleSize << std::endl; //Keeps tripping, check rampsamplesize
    }
    if (prevRampSamples > rampSampleSize) {
        std::cout << "prevRampSamples " << prevRampSamples << "/" << rampSampleSize << std::endl; //Only used by synced
    }
    
    if (rampSampleSize > (sampleRate * 4) || rampSampleSize < 1) {
        //std::cout << "RampSampleSize " << rampSampleSize << std::endl;
    }
    
    if (sampleRate > 200000 || sampleRate < 20000) {
        std::cout << "samplerate " << sampleRate << std::endl;
    }
    if (bpm > 400 || bpm < 20) {
        //std::cout << "bpm " << bpm << std::endl;
    }
    if (randParam > 1 || randParam < 0) {
        std::cout << "randParam " << randParam << std::endl;
    }
    if (randAmount > 1 || randAmount < 0) {
        std::cout << "randAmount " << randAmount << std::endl;
    }
    if (syncChoice > 8 || syncChoice < 0) {
        std::cout << "syncChoice " << syncChoice << std::endl;
    }
    if (reset > 1 || reset < 0) {
        std::cout << "reset " << syncChoice << std::endl;
    }
    if (ppqRamp > 1 || ppqRamp < 0) {
        std::cout << "ppqRamp " << ppqRamp << std::endl;
    }
    if (phaseMod > 1 || phaseMod < 0) {
        std::cout << "phaseMod " << phaseMod << std::endl;
    }
    if (shape > 6 || shape < 0) {
        std::cout << "shape " << shape << std::endl;
    }
    if (amplitude > 1 || amplitude < 0) {
        std::cout << "amplitude " << amplitude << std::endl;
    }
    if (slewTime > 5 || slewTime < 0) {
        std::cout << "slewTime " << slewTime << std::endl;
    }
    if (lfoHz > 10000 || lfoHz < 0) {
        std::cout << "lfoHz " << lfoHz << std::endl;
    }
    
    /*
     float syncType = 0;
     float ppqLoop = 0;
     float prevPpqOut = 0;
     float divisor = 0;
     float prevOut = 0;
     */
    
}

//    for (int i = 0; i < 1024; i++) {
//        double poo = rampTable[i] + 0.5;
//        std::cout << std::fixed << std::setprecision(8) << poo << ",";
//    }
