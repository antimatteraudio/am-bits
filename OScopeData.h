/*
  ==============================================================================

    OScopeData.h
    Created: 7 Jan 2025 1:19:00pm
    Author:  jeph

  ==============================================================================
*/

// OSCILLOSCOPE

#pragma once

#include <JuceHeader.h>
#include <vector>

class OScopeData
{
public:
    inline OScopeData();
    inline ~OScopeData();
    inline void init(float sampleRate);
    inline void reset();
    
    inline void oscilloCalc(float input, float thresh);

//    juce::Array<juce::var> outputArray;
    
    juce::Array<juce::var> outputArray;
    
    int newWave = 0;
    int length = 0;
    
private:
    
    juce::Array<juce::var> cycleArray;
    
    int resolution = 0;
    int trigger = 0;
    int cycleindex = 0;
    
//    float cycleArray[100000];
    
    float previousInput = 0;
    
    inline void oscilloStretch(int cycleLength);
//    void variableChecker();
    
};

//--------------------------------

//Constructor
OScopeData::OScopeData()
//Init vars
:
newWave(0),
resolution(0),
trigger(0),
cycleindex(0),
previousInput(0)
{
    outputArray.clear();
    cycleArray.clear();
};

//Desconstructor
OScopeData::~OScopeData()
{
    outputArray.clear();
    cycleArray.clear();
};

void OScopeData::init(float sampleRate)
{
    newWave = 0;
    resolution = sampleRate / 100;
    outputArray.clear();
    cycleArray.clear();
    outputArray.resize(resolution);
    cycleArray.resize(resolution);
    length = 0;
};

void OScopeData::reset()
{
    newWave = 0;
};

//Creates and array to send to the oscilloscope
void OScopeData::oscilloCalc(float input, float thresh)
{
    int cycleEnd = 0;
    cycleindex += 1;

    //If the waveform is too long or not triggering, we can't record samples forever
    if (cycleindex >= (resolution * 10)) cycleEnd = 1;

    if(input >= thresh && previousInput < thresh && trigger == 1) cycleEnd = 1;
    
    //Trigger fall - end of waveform -  update output array
    if(cycleEnd == 1) {

        //outputArray.clearQuick();

        //If the cycle data is too long, summarize it
        if (cycleindex > resolution) {

            int outIndex = 0;
//            outputArray.resize(resolution);
            length = resolution;
            

            //Fill the output array with the closest array # in the cycle array. Later on we may average theese to increase fidelity?
            float outMult = cycleindex / resolution;

            for (int i = 0; i < resolution; i++) {

                outIndex += outMult;

                if(i == 0) outIndex = 0;

                //outputArray.insert(i, cycleArray[outIndex]);
                outputArray.set(i, cycleArray[outIndex]);
            }

        } else {
            //Stuff the outputArray
            if(outputArray.size() > 0)
//            outputArray.resize(cycleindex);
                length = cycleindex;
            
            for (int i = 0; i < cycleindex; i ++) {
                //outputArray.insert(i, cycleArray[i]);
                outputArray.set(i, cycleArray[i]);
            }

        }

        //cycleArray.clearQuick();
        cycleEnd = 0;
        cycleindex = 0;
        trigger = 0;
        newWave = 1;
    }
    
    //Trigger rise
    if(input >= thresh && previousInput < thresh && trigger == 0) {
        trigger = 1;
        cycleindex = 0;
    }

//    cycleArray.insert(cycleindex, input);
    cycleArray.set(cycleindex, input);

    previousInput = input;
};












//    if(cycleArray[cycleindex] == NULL) {
//        std::cout << "POOP" << std::endl;
//    }

//Check those stinking crazy variables - now with POOP!
//void OScopeData::variableChecker()
//{
//    if(resolution > 10000){
//        std::cout << resolution << "resolutiongreaterthanpoop" << std::endl;
//    }
//    if(trigger > 1){
//        std::cout << trigger << "triggerpoop" << std::endl;
//    }
//    if(cycleindex > (100000)){
//        std::cout << cycleindex << "cycleIndexgreaterthanpoop" << std::endl;
//    }
//    if(newWave > 1){
//        std::cout << newWave << "newWavegreaterthanpoop" << std::endl;
//    }
//    if(previousInput > 100){
//        std::cout << previousInput << "previousInputpoop" << std::endl;
//    }
//
//};
