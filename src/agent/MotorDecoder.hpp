#pragma once

#include "../brain/Neuron.hpp"
#include "../agent/AgentBody.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

class MotorDecoder {
public:
    MotorDecoder();
    ~MotorDecoder();
    
    // Set motor neuron groups for decoding
    void setMotorGroups(
        const std::vector<Neuron*>& motorForward,
        const std::vector<Neuron*>& motorBackward,
        const std::vector<Neuron*>& motorTurnLeft,
        const std::vector<Neuron*>& motorTurnRight,
        const std::vector<Neuron*>& motorInteract,
        const std::vector<Neuron*>& motorWait
    );
    
    // Decode motor neuron activity into motor command
    MotorCommand decodeMotorCommand();
    
    // Calculate activity for a single neuron group
    float calculateActivity(const std::vector<Neuron*>& neurons) const;
    
    // Set activity threshold for action selection
    void setActivityThreshold(float threshold) { activityThreshold_ = threshold; }
    
    // Get activity threshold
    float getActivityThreshold() const { return activityThreshold_; }
    
    // Reset internal state
    void reset();
    
private:
    // Motor neuron groups for different actions
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Activity threshold for action selection
    float activityThreshold_;
    
    // Last computed activity values for debugging
    float lastForwardActivity_;
    float lastBackwardActivity_;
    float lastLeftActivity_;
    float lastRightActivity_;
    float lastInteractActivity_;
    float lastWaitActivity_;
};

} // namespace nlm
