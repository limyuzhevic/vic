#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Serotonin implementation with behavioral inhibition and mood regulation
class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Behavioral inhibition
    void enhanceBehavioralInhibition(float contextFactor);
    
    // Mood regulation
    float getMood() const;
    void signalReward(float reward, bool isExpected);
    
    // Social behavior
    void enhanceSocialBonding(float socialValue);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
