#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

class Config;

// Simple world interface for AgentBrain
class SimpleWorld {
public:
    virtual ~SimpleWorld() = default;
    virtual size_t getVisionWidth() const = 0;
    virtual size_t getVisionHeight() const = 0;
};

// Motor command enumeration
enum class MotorCommand {
    Wait,
    MoveForward,
    MoveBackward,
    TurnLeft,
    TurnRight,
    Interact,
    LookLeft,
    LookRight
};

// Sensory perception class
class SensoryPercept {
public:
    // Vision data (16x16 grid)
    std::vector<float> vision_;
    
    // Touch data
    std::vector<float> touch_;
    
    // Internal signals
    std::vector<float> internal_;
    
    // Proprioception
    std::vector<float> proprioception_;
    
    // Audio data
    std::vector<float> audio_;
    
    SensoryPercept() {
        vision_.resize(16 * 16, 0.0f);
        touch_.resize(8, 0.0f);
        internal_.resize(4, 0.0f);
        proprioception_.resize(6, 0.0f);
        audio_.resize(0, 0.0f);
    }
    
    const std::vector<float>& getVision() const { return vision_; }
    const std::vector<float>& getTouch() const { return touch_; }
    const std::vector<float>& getInternal() const { return internal_; }
    const std::vector<float>& getProprioception() const { return proprioception_; }
    const std::vector<float>& getAudio() const { return audio_; }
};

// Observation class for reward computation
class Observation {
public:
    float getValue() const { return value_; }
    void setValue(float value) { value_ = value; }
    
private:
    float value_ = 0.0f;
};

} // namespace nlm
