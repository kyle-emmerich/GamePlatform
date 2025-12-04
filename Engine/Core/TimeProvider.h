#pragma once
#include <chrono>

class ITimeProvider { 
public:
    virtual ~ITimeProvider() = default;

    virtual double GetTimeSeconds() const = 0;
};

class StdTimeProvider : public ITimeProvider {  
public:
    double GetTimeSeconds() const override {
        return std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }
};