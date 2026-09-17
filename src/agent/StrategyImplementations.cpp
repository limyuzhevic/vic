// Motor Decoding Strategy Implementations
#include "../agent/ModularityLayer.hpp"
#include "../brain/Neuron.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>

namespace nlm {

// DefaultMotorDecodingStrategy implementation
MotorCommand DefaultMotorDecodingStrategy::decode(const std::vector<Neuron*>& neurons) {
    if (neurons.empty()) return MotorCommand::Wait;
    
    float maxActivity = -1.0f;
    MotorCommand bestCommand = MotorCommand::Wait;
    
    for (size_t i = 0; i < neurons.size() && i < 6; ++i) {
        if (!neurons[i]) continue;
        
        float activity = std::abs(neurons[i]->getState().membranePotential - 
                                 neurons[i]->getState().restingPotential);
        
        if (activity > maxActivity) {
            maxActivity = activity;
            switch (i) {
                case 0: bestCommand = MotorCommand::MoveForward; break;
                case 1: bestCommand = MotorCommand::MoveBackward; break;
                case 2: bestCommand = MotorCommand::TurnLeft; break;
                case 3: bestCommand = MotorCommand::TurnRight; break;
                case 4: bestCommand = MotorCommand::Interact; break;
                case 5: bestCommand = MotorCommand::Wait; break;
            }
        }
    }
    
    return bestCommand;
}

// ThresholdMotorDecodingStrategy implementation
ThresholdMotorDecodingStrategy::ThresholdMotorDecodingStrategy(float threshold)
    : threshold_(threshold) {}

MotorCommand ThresholdMotorDecodingStrategy::decode(const std::vector<Neuron*>& neurons) {
    if (neurons.empty()) return MotorCommand::Wait;
    
    float maxActivity = -1.0f;
    MotorCommand bestCommand = MotorCommand::Wait;
    
    for (size_t i = 0; i < neurons.size() && i < 6; ++i) {
        if (!neurons[i]) continue;
        
        float activity = std::abs(neurons[i]->getState().membranePotential - 
                                 neurons[i]->getState().restingPotential);
        
        if (activity > maxActivity && activity >= threshold_) {
            maxActivity = activity;
            switch (i) {
                case 0: bestCommand = MotorCommand::MoveForward; break;
                case 1: bestCommand = MotorCommand::MoveBackward; break;
                case 2: bestCommand = MotorCommand::TurnLeft; break;
                case 3: bestCommand = MotorCommand::TurnRight; break;
                case 4: bestCommand = MotorCommand::Interact; break;
                case 5: bestCommand = MotorCommand::Wait; break;
            }
        }
    }
    
    return bestCommand;
}

// SimpleCuriosityHandler implementation
SimpleCuriosityHandler::SimpleCuriosityHandler(float noveltyWeight, float errorWeight)
    : noveltyWeight_(noveltyWeight), errorWeight_(errorWeight), curiosityLevel_(0.0f), decayRate_(0.99f) {}

void SimpleCuriosityHandler::update(float novelty, float predictionError, double timestep) {
    float rawCuriosity = novelty * noveltyWeight_ + std::abs(predictionError) * errorWeight_;
    curiosityLevel_ = std::clamp(rawCuriosity, 0.0f, 1.0f);
    curiosityLevel_ *= std::pow(decayRate_, timestep);
}
