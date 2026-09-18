#pragma once

#include "../brain/Neuron.hpp"
#include "../brain/NeuralRegion.hpp"
#include <vector>
#include <memory>

namespace nlm {

class MotorDecoder {
public:
    MotorDecoder(const std::vector<Neuron*>& forward, const std::vector<Neuron*>& backward,
                 const std::vector<Neuron*>& left, const std::vector<Neuron*>& right,
                 const std::vector<Neuron*>& interact, const std::vector<Neuron*>& wait);
    
    MotorCommand decode() const;
    
    void setCuriosityEnabled(bool enabled) { curiosityEnabled_ = enabled; }
    void setCuriosityLevel(float level) { curiosityLevel_ = level; }
    
private:
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd) const;
    
    // Motor neuron groups
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Configuration
    bool curiosityEnabled_;
    float curiosityLevel_;
};

} // namespace nlm