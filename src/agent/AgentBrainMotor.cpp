#pragma once

#include "AgentBrainComponents.hpp"
#include "AgentBrainConfig.hpp"
#include "AgentBrainError.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <stdexcept>

namespace nlm {

AgentBrainMotor::AgentBrainMotor() : curiosityLevel_(0.0f), config_(std::make_shared<AgentBrainConfig>()) {}

AgentBrainResult<void> AgentBrainMotor::distributeMotorNeurons(const Brain& brain) {
    if (!validateNotNull(&brain, "brain pointer")) {
        return AgentBrainInvalidInputError("AgentBrainMotor",
                                          "brain pointer cannot be null in distributeMotorNeurons");
    }
    
    motorForward_.clear();
    motorBackward_.clear();
    motorTurnLeft_.clear();
    motorTurnRight_.clear();
    motorInteract_.clear();
    motorWait_.clear();
    
    if (!brain.getRegions()) {
        return AgentBrainResult<void>(std::make_shared<AgentBrainNeuralError>(
            "AgentBrainMotor", "brain has no regions in distributeMotorNeurons"));
    }
    
    try {
        for (const auto& region : brain.getRegions()) {
            if (!validateNotNull(region, "region pointer")) {
                continue; // Graceful degradation: skip invalid region
            }
            
            if (!region->getPopulations()) continue;
            
            for (auto& pop : region->getPopulations()) {
                if (!validateNotNull(pop, "population pointer")) continue;
                
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    const auto& neurons = pop->getNeurons();
                    if (!validateNotNull(&neurons, "neurons vector")) continue;
                    
                    for (Neuron* n : neurons) {
                        if (!validateNotNull(n, "neuron pointer")) continue;
                        
                        // Distribute motor neurons to different action groups
                        size_t idx = motorForward_.size() + motorBackward_.size() + 
                                    motorTurnLeft_.size() + motorTurnRight_.size() +
                                    motorInteract_.size() + motorWait_.size();
                        
                        switch (idx % 6) {
                            case 0: motorForward_.push_back(n); break;
                            case 1: motorBackward_.push_back(n); break;
                            case 2: motorTurnLeft_.push_back(n); break;
                            case 3: motorTurnRight_.push_back(n); break;
                            case 4: motorInteract_.push_back(n); break;
                            case 5: motorWait_.push_back(n); break;
                        }
                    }
                }
            }
        }
        return AgentBrainResult<void>(true);
    } catch (const std::bad_alloc&) {
        return AgentBrainMemoryError("AgentBrainMotor", 
                                   "memory allocation failed in distributeMotorNeurons");
    } catch (const std::exception& e) {
        return AgentBrainResult<void>(std::make_shared<AgentBrainNeuralError>(
            "AgentBrainMotor", std::string("exception in distributeMotorNeurons: ") + e.what()));
    }
}

MotorCommand AgentBrainMotor::decodeCommand(const Brain& brain) {
    if (!validateNotNull(&brain, "brain pointer")) {
        return MotorCommand::Wait;
    }
    
    if (!brain.getRegions()) {
        return MotorCommand::Wait;
    }
    
    auto calcActivity = [](const std::vector<Neuron*>& neurons, float scale) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            // Use membrane potential deviation from rest as activity measure
            if (!validateNotNull(n, "neuron pointer")) continue;
            try {
                const auto& state = n->getState();
                sum += std::abs(state.membranePotential - state.restingPotential) * scale;
            } catch (const std::exception&) {
                continue; // Graceful degradation: skip neuron if state access fails
            }
        }
        return neurons.empty() ? 0.0f : sum / neurons.size();
    };
    
    float forwardAct = calcActivity(motorForward_, config_->getParameter<float>(AgentBrainConfig::ParameterID::MOTOR_FORWARD_SCALE));
    float backwardAct = calcActivity(motorBackward_, config_->getParameter<float>(AgentBrainConfig::ParameterID::MOTOR_BACKWARD_SCALE));
    float leftAct = calcActivity(motorTurnLeft_, config_->getParameter<float>(AgentBrainConfig::ParameterID::MOTOR_LEFT_SCALE));
    float rightAct = calcActivity(motorTurnRight_, config_->getParameter<float>(AgentBrainConfig::ParameterID::MOTOR_RIGHT_SCALE));
    float interactAct = calcActivity(motorInteract_, config_->getParameter<float>(AgentBrainConfig::ParameterID::MOTOR_INTERACT_SCALE));
    float waitAct = calcActivity(motorWait_, 1.0f); // No scaling for wait
    
    // Find maximum activity
    struct { MotorCommand cmd; float activity; } commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct; // Default to wait if nothing stronger
    
    for (const auto& c : commands) {
        if (c.activity > bestActivity) {
            bestActivity = c.activity;
            best = c.cmd;
        }
    }
    
    // Only act if there's meaningful activity (use config threshold)
    float threshold = config_->getParameter<float>(AgentBrainConfig::ParameterID::ACTION_SELECTION_THRESHOLD);
    if (bestActivity < threshold) {
        return MotorCommand::Wait;
    }
    
    return best;
}

MotorCommand AgentBrainMotor::selectWithCuriosity(MotorCommand defaultCmd, const Brain& brain) {
    if (!validateNotNull(&brain, "brain pointer")) {
        return defaultCmd;
    }
    
    if (!brain.getRandomGenerator()) {
        return defaultCmd;
    }
    
    float threshold = config_->getParameter<float>(AgentBrainConfig::ParameterID::RANDOM_EXPLORATION_THRESHOLD);
    if (curiosityLevel_ > threshold) {
        // Exploration: occasionally choose random action when curiosity is high
        float exploreChance = curiosityLevel_ * config_->getParameter<float>(AgentBrainConfig::ParameterID::EXPLORATION_CHANCE_MAX);
        
        float r = brain.getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
            // Random motor command (use config action count)
            int actionCount = config_->getParameter<int>(AgentBrainConfig::ParameterID::RANDOM_ACTION_COUNT);
            int choice = brain.getRandomGenerator()->uniformInt(0, actionCount);
            
            switch (choice) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::MoveBackward;
                case 2: return MotorCommand::TurnLeft;
                case 3: return MotorCommand::TurnRight;
                case 4: return MotorCommand::LookLeft;
                case 5: return MotorCommand::LookRight;
                case 6: return MotorCommand::Interact;
                default: return MotorCommand::Wait;
            }
        }
    }
    
    return defaultCmd;
}

AgentBrainResult<size_t> AgentBrainMotor::getMotorOutputSize() const {
    try {
        if (!validateNotNull(&curiosityLevel_, "curiosityLevel_ pointer")) {
            return AgentBrainResult<size_t>(size_t(6)); // Default return
        }
        return AgentBrainResult<size_t>(size_t(6));
    } catch (const std::exception& e) {
        return AgentBrainResult<size_t>(std::make_shared<AgentBrainError>(
            AgentBrainErrorCode::NEURAL_ERROR,
            "AgentBrainMotor",
            std::string("error in getMotorOutputSize: ") + e.what()));
    }
}

AgentBrainResult<void> AgentBrainMotor::reset() {
    try {
        motorForward_.clear();
        motorBackward_.clear();
        motorTurnLeft_.clear();
        motorTurnRight_.clear();
        motorInteract_.clear();
        motorWait_.clear();
        return AgentBrainResult<void>(true);
    } catch (const std::exception& e) {
        return AgentBrainResult<void>(std::make_shared<AgentBrainError>(
            AgentBrainErrorCode::NEURAL_ERROR,
            "AgentBrainMotor",
            std::string("error in reset: ") + e.what()));
    }
}

} // namespace nlm