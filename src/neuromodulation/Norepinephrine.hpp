#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Norepinephrine: Arousal and vigilance
class Norepinephrine {
public:
    Norepinephrine();
    ~Norepinephrine();
    
    const char* getName() const { return "NE"; }
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Arousal effects on learning rate and salience
    void signalArousal(float arousalLevel);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
