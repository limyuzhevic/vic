#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
    const char* getName() const { return "5-HT"; }
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Mood effects on exploration/exploitation balance
    void signalMood(float moodLevel);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
