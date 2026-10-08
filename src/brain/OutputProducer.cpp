#include "OutputProducer.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <random>
#include <stdexcept>

namespace nlm {

OutputProducer::OutputProducer(std::shared_ptr<Brain> brain)
    : brain(brain)
    , motorExcitability(0.0f)
    , motorInhibition(0.0f) {
    
    if (!brain) {
        throw std::invalid_argument("Brain pointer cannot be null");
    }
    
    analyzeMotorActivity();
}

void OutputProducer::analyzeMotorActivity() {
    if (!brain) {
        return;
    }
    
    motorExcitability = 0.0f;
    motorInhibition = 0.0f;
    
    // Collect motor neuron statistics
    size_t totalMotorNeurons = 0;
    size_t firingMotorNeurons = 0;
    
    for (const auto& region : brain->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Motor) {
                totalMotorNeurons += pop->getNeuronCount();
                
                for (auto* neuron : pop->getNeurons()) {
                    if (neuron->isFiring()) {
                        firingMotorNeurons++;
                        
                        // Calculate motor excitability based on firing
                        float excitability = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
                        motorExcitability += excitability;
                        
                    } else {
                        // Calculate inhibition based on distance from threshold
                        float inhibition = neuron->getThreshold() - neuron->getState().membranePotential;
                        if (inhibition > 0.0f) {
                            motorInhibition += inhibition;
                        }
                    }
                }
            }
        }
    }
    
    // Normalize by total motor neurons
    if (totalMotorNeurons > 0) {
        motorExcitability /= static_cast<float>(totalMotorNeurons);
        motorInhibition /= static_cast<float>(totalMotorNeurons);
    }
}

std::unique_ptr<Action> OutputProducer::produceAction() {
    if (!brain) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    ActionType actionType = ActionType::Wait;
    
    // Decision logic based on motor activity analysis
    if (motorExcitability > motorInhibition * 1.5f) {
        // High excitation -> move forward
        actionType = ActionType::MoveForward;
    } else if (motorInhibition > motorExcitability * 1.5f) {
        // High inhibition -> wait/rest
        actionType = ActionType::Wait;
    } else if (motorExcitability > 0.0f && motorInhibition > 0.0f) {
        // Balanced activity -> turn based on small random bias
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(0.0f, 1.0f);
        
        if (dis(gen) < 0.5f) {
            actionType = ActionType::TurnLeft;
        } else {
            actionType = ActionType::TurnRight;
        }
    }
    
    return std::make_unique<Action>(actionType);
}

} // namespace nlm