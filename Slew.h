#pragma once
#include <vector>
#include <cmath>
#include <iostream>
#include <numeric>

class AM_Slew
{
public:
    inline AM_Slew();
    inline ~AM_Slew();
    inline float Process(float input);
    inline void Init(float ms, float hostSampleRate, int objectid);
    inline void setMs(float newMs);
private:
    int count = 0;
    int size = 0;
    float total = 0;
    int ID = 0;
    //float maxMs = 0;
    float maxSize = 0;
    float sampleRate = 0;
    
    std::vector<float> values;
};



//Constructor
AM_Slew::AM_Slew()
//Init vars
:
count(0),
size(0),
total(0),
ID(0),
maxSize(0),
sampleRate(0)
{
    values.clear();
};

//Desconstructor
AM_Slew::~AM_Slew()
{
    values.clear(); // delete old buffer
};

void AM_Slew::Init(float ms, float hostSampleRate, int objectid) {
    
    sampleRate = hostSampleRate;
    
    //The lowest ms valus @44.1k is .03, anything lower will cause the size to be zero!
    if (ms < 0.03) ms = 0.03;
    //Init values array for size
    size = std::round((sampleRate / 1000) *  ms);
    if (size < 0) size = 0;
    maxSize = size;
    
    values.clear();// delete old buffer
    values.resize(size);
    
    ID = objectid;
    
    total = 0;
};

void AM_Slew::setMs(float newMs) {
    
    size = std::round((sampleRate / 1000) *  newMs);
    if(size > maxSize) size = maxSize;
    if (size < 0) size = 0;
}



float AM_Slew::Process(float input){
    
    count += 1;
    if(count >= size) count = 0;

    total = total - values[count]; //Sub 1st sample from the current total
    total = total + input; //Add current sample from the current total
    values[count] = input; //insert current amount into vector
    
    float average = total / size;
    average = std::round(average * 1000.0) / 1000.0; //Set precision to fix c++ rounding errors.
    
    return average;
}





                 





