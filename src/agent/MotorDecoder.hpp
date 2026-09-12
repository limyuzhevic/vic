#pragma once

#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../motor/Action.hpp"
#include <memory>
#include <vector>
#include <random>

namespace nlm {

class MotorDecoder {
public:
    MotorDecoder(std::shared_ptr<Brain> brain);
    ~MotorDecoder() = default;
    
    // Decode motor command from neuron activity
    MotorCommand decode(const std::vector<Neuron*>& neurons);
    
    // Calculate activity for a group of neurons
    float calculateActivity(const std::vector<Neuron*>& neurons);
    
    // Apply curiosity-based exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd, 
                                     float curiosityLevel,
                                     bool enabled);
    
    // Configuration
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Get motor output size
    size_t getMotorOutputSize() const { return 6; }
    
private:
    std::shared_ptr<Brain> brain_;
    bool curiosityEnabled_;
    std::mt19937 randomGen_;
};

} // namespace nlm