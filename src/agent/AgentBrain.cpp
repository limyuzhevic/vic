#pragma once

#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <memory>

namespace nlm {

// Dependency injection constructor for better testability
AgentBrain::AgentBrain(std::shared_ptr<Brain> brain, 
                      std::vector<NeuronGroupFactory> neuronGroupFactories,
                      NeuromodulationFactory neuromodulationFactory,
                      DevelopmentSystemFactory developmentFactory)
    : brain_(brain)
    , neuronGroupFactories_(std::move(neuronGroupFactories))
    , neuromodulationFactory_(std::move(neuromodulationFactory))
    , developmentFactory_(std::move(developmentFactory))
    , dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , developmentalAge_(0.0)
    , plasticityModifier_(1.0f)
    , rewardModulationEnabled_(true)
    , structuralPlasticityEnabled_(true)
    , developmentEnabled_(true)
    , curiosityEnabled_(true)
    , sensoryNoveltyDecay_(0.99f)
{
    // Initialize neuron groups using factories for better testability
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                // Use factory pattern for neuron group creation
                if (type == NeuronType::Motor && neuronGroupFactories_.motor) {
                    auto motorNeurons = neuronGroupFactories_.motor(pop->getNeurons(), "motor");
                    motorForward_.insert(motorForward_.end(), 
                                         motorNeurons.forward.begin(), motorNeurons.forward.end());
                    motorBackward_.insert(motorBackward_.end(), 
                                         motorNeurons.backward.begin(), motorNeurons.backward.end());
                    motorTurnLeft_.insert(motorTurnLeft_.end(), 
                                         motorNeurons.turnLeft.begin(), motorNeurons.turnLeft.end());
                    motorTurnRight_.insert(motorTurnRight_.end(), 
                                         motorNeurons.turnRight.begin(), motorNeurons.turnRight.end());
                    motorInteract_.insert(motorInteract_.end(), 
                                         motorNeurons.interact.begin(), motorNeurons.interact.end());
                    motorWait_.insert(motorWait_.end(), 
                                     motorNeurons.wait.begin(), motorNeurons.wait.end());
                } else if (type == NeuronType::Sensory && neuronGroupFactories_.sensory) {
                    auto sensoryNeurons = neuronGroupFactories_.sensory(pop->getNeurons(), "sensory");
                    sensoryVision_.insert(sensoryVision_.end(), 
                                         sensoryNeurons.vision.begin(), sensoryNeurons.vision.end());
                    sensoryTouch_.insert(sensoryTouch_.end(), 
                                        sensoryNeurons.touch.begin(), sensoryNeurons.touch.end());
                    sensoryInternal_.insert(sensoryInternal_.end(), 
                                          sensoryNeurons.internal.begin(), sensoryNeurons.internal.end());
                    sensoryProprioception_.insert(sensoryProprioception_.end(), 
                                                 sensoryNeurons.proprioception.begin(), sensoryNeurons.proprioception.end());
                }
            }
        }
    }
    
    // Initialize neuromodulation using factory
    if (neuromodulationFactory_) {
        neuromodulation_ = neuromodulationFactory_();
    }
    
    // Initialize development system using factory
    if (developmentFactory_) {
        development_ = developmentFactory_();
    }
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Initialize neuromodulation system
    if (neuromodulation_) {
        neuromodulation_->reset();
    }
    
    // Initialize development system
    if (development_) {
        development_->reset();
    }
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(sensoryInternal_.size()) + " internal sensory neurons");
}

// Default factory functions for backward compatibility
AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : AgentBrain(brain,
                 NeuronGroupFactory{
                     [](const std::vector<Neuron*>& neurons, const std::string& type) {
                         NeuronGroups groups;
                         size_t idx = 0;
                         for (Neuron* n : neurons) {
                             if (type == "motor") {
                                 switch (idx % 6) {
                                     case 0: groups.forward.push_back(n); break;
                                     case 1: groups.backward.push_back(n); break;
                                     case 2: groups.turnLeft.push_back(n); break;
                                     case 3: groups.turnRight.push_back(n); break;
                                     case 4: groups.interact.push_back(n); break;
                                     case 5: groups.wait.push_back(n); break;
                                 }
                             } else if (type == "sensory") {
                                 switch (idx % 4) {
                                     case 0: groups.vision.push_back(n); break;
                                     case 1: groups.touch.push_back(n); break;
                                     case 2: groups.internal.push_back(n); break;
                                     case 3: groups.proprioception.push_back(n); break;
                                 }
                             }
                             idx++;
                         }
                         return groups;
                     }
                 },
                 NeuromodulationFactory(),
                 DevelopmentSystemFactory()) {}

size_t AgentBrain::getSensoryInputSize() const {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return 256 + 8 + 4 + 6;
}

size_t AgentBrain::getMotorOutputSize() const {
    // One motor neuron per action
    return 6;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 3.0f;
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal input (4 values -> sensoryInternal_ neurons)
    const auto& internal = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < internal.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = internal[i] * 2.0f;
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception input (6 values -> sensoryProprioception_ neurons)
    const auto& proprioception = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprioception.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = proprioception[i];
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // Novelty detection using previous vision state
    if (!previousVision_.empty() && !vision.empty()) {
        float novelty = 0.0f;
        for (size_t i = 0; i < std::min(previousVision_.size(), vision.size()); ++i) {
            novelty += std::abs(vision[i] - previousVision_[i]);
        }
        novelty = novelty / previousVision_.size();
        
        // Apply novelty decay
        noveltyLevel_ = novelty * (1.0f - sensoryNoveltyDecay_);
        
        if (curiosityEnabled_) {
            // Curiosity response to novelty
            curiosityLevel_ = std::min(1.0f, curiosityLevel_ + novelty * 0.1f);
        }
        
        // Update previous vision state
        previousVision_ = vision;
    }
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!neuromodulation_) return;
    
    expectedReward_ = predictedReward;
    
    if (rewardModulationEnabled_) {
        // Calculate reward prediction error
        predictionError_ = reward - predictedReward;
        
        // Dopamine response to prediction error
        if (predictionError_ > 0.1f) {
            // Positive prediction error -> dopamine increase
            dopamineLevel_ = std::min(1.0f, dopamineLevel_ + 0.1f * predictionError_);
        } else if (predictionError_ < -0.1f) {
            // Negative prediction error -> dopamine decrease
            dopamineLevel_ = std::max(0.0f, dopamineLevel_ + predictionError_ * 0.2f);
        }
        
        // Apply neuromodulation to brain
        neuromodulation_->applyDopamine(dopamineLevel_);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    developmentalAge_ += timestep;
    
    // Update development system
    if (development_) {
        development_->update(developmentalAge_, timestep);
        
        // Adjust plasticity based on development stage
        plasticityModifier_ = development_->getPlasticityModifier();
    }
    
    // Age-related neuromodulation changes
    if (developmentalAge_ > 100.0) {
        // Aging brain - reduced neuromodulation
        dopamineLevel_ *= 0.9f;
        curiosityLevel_ *= 0.8f;
    }
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (development_) {
        return development_->getStage();
    }
    return DevelopmentalStage::Adult;
}

float AgentBrain::getNeuromodulationLevel() const {
    return (dopamineLevel_ + noveltyLevel_ + curiosityLevel_) / 3.0f;
}

float AgentBrain::getCuriosityLevel() const {
    return curiosityLevel_;
}

float AgentBrain::getNoveltyLevel() const {
    return noveltyLevel_;
}

float AgentBrain::getPredictionError() const {
    return predictionError_;
}

void AgentBrain::reset() {
    previousVision_.clear();
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    if (neuromodulation_) {
        neuromodulation_->reset();
    }
    
    if (development_) {
        development_->reset();
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    // Select motor command based on current neuromodulation state
    MotorCommand defaultCmd = decodeFromMotorNeurons();
    
    if (curiosityEnabled_ && curiosityLevel_ > 0.5f) {
        // Curiosity-driven exploration
        return selectWithCuriosity(defaultCmd);
    }
    
    return defaultCmd;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Count active neurons in each motor group
    std::vector<std::pair<MotorCommand, float>> commandScores;
    
    // Forward motion
    int forwardCount = 0;
    for (Neuron* n : motorForward_) {
        if (n && n->isActive()) forwardCount++;
    }
    commandScores.emplace_back(MotorCommand{ActionType::MoveForward, 1.0f}, 
                              static_cast<float>(forwardCount) / motorForward_.size());
    
    // Backward motion
    int backwardCount = 0;
    for (Neuron* n : motorBackward_) {
        if (n && n->isActive()) backwardCount++;
    }
    commandScores.emplace_back(MotorCommand{ActionType::TurnLeft, 1.0f}, 
                              static_cast<float>(backwardCount) / motorBackward_.size());
    
    // Turn left
    int turnLeftCount = 0;
    for (Neuron* n : motorTurnLeft_) {
        if (n && n->isActive()) turnLeftCount++;
    }
    commandScores.emplace_back(MotorCommand{ActionType::TurnRight, 1.0f}, 
                              static_cast<float>(turnLeftCount) / motorTurnLeft_.size());
    
    // Turn right
    int turnRightCount = 0;
    for (Neuron* n : motorTurnRight_) {
        if (n && n->isActive()) turnRightCount++;
    }
    commandScores.emplace_back(MotorCommand{ActionType::Interact, 1.0f}, 
                              static_cast<float>(turnRightCount) / motorTurnRight_.size());
    
    // Interact
    int interactCount = 0;
    for (Neuron* n : motorInteract_) {
        if (n && n->isActive()) interactCount++;
    }
    commandScores.emplace_back(MotorCommand{ActionType::Wait, 1.0f}, 
                              static_cast<float>(interactCount) / motorInteract_.size());
    
    // Wait
    int waitCount = 0;
    for (Neuron* n : motorWait_) {
        if (n && n->isActive()) waitCount++;
    }
    commandScores.emplace_back(MotorCommand{ActionType::MoveForward, 1.0f}, 
                              static_cast<float>(waitCount) / motorWait_.size());
    
    // Select command with highest score
    if (commandScores.empty()) {
        return MotorCommand{ActionType::Wait, 1.0f, 1.0f};
    }
    
    auto best = std::max_element(commandScores.begin(), commandScores.end(),
                                [](const auto& a, const auto& b) {
                                    return a.second < b.second;
                                });
    
    return best->first;
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    // Increase curiosity for exploration of novel states
    float curiosityMultiplier = 1.0f + curiosityLevel_ * 0.5f;
    
    // Modify default command for exploration
    switch (defaultCmd.type) {
        case ActionType::MoveForward:
            return MotorCommand{ActionType::MoveForward, defaultCmd.magnitude * curiosityMultiplier, 
                              defaultCmd.duration * 0.8f};
        case ActionType::TurnLeft:
            return MotorCommand{ActionType::TurnLeft, defaultCmd.magnitude * curiosityMultiplier,
                              defaultCmd.duration * 0.8f};
        case ActionType::TurnRight:
            return MotorCommand{ActionType::TurnRight, defaultCmd.magnitude * curiosityMultiplier,
                              defaultCmd.duration * 0.8f};
        default:
            return defaultCmd;
    }
}

} // namespace nlm
