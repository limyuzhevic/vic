#pragma once

#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <stdexcept>

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding

// AgentBrain constructor with improved initialization
AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(std::move(brain))
    , neuralGroups_()
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
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain: Cannot initialize with null brain pointer");
        return;
    }
    
    NLM_LOG_INFO("AgentBrain initialized successfully");
}

AgentBrain::~AgentBrain() {
    neuralGroups_.clearAll();
}

void AgentBrain::initialize(const SimpleWorld& world) {
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain::initialize: Cannot initialize with null brain");
        return;
    }
    
    // Initialize neural groups efficiently
    neuralGroups_.clearAll();
    neuralGroups_.motorForward_.reserve(100);
    neuralGroups_.motorBackward_.reserve(100);
    neuralGroups_.motorTurnLeft_.reserve(100);
    neuralGroups_.motorTurnRight_.reserve(100);
    neuralGroups_.motorInteract_.reserve(100);
    neuralGroups_.motorWait_.reserve(100);
    
    neuralGroups_.sensoryVision_.reserve(1000);
    neuralGroups_.sensoryTouch_.reserve(100);
    neuralGroups_.sensoryInternal_.reserve(100);
    neuralGroups_.sensoryProprioception_.reserve(100);
    
    // Reserve space for vision state
    size_t visionSize = world.getVisionWidth() * world.getVisionHeight();
    previousVision_.assign(visionSize, 0.0f);
    
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(neuralGroups_.sensoryVision_.size()) + " vision sensory neurons, " +
                 std::to_string(neuralGroups_.sensoryTouch_.size()) + " touch sensory neurons, " +
                 std::to_string(neuralGroups_.sensoryInternal_.size()) + " internal sensory neurons");
}

void AgentBrain::updateNeuralGroups() {
    neuralGroups_.clearAll();
    
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    size_t idx = 0;
                    for (Neuron* n : pop->getNeurons()) {
                        switch (idx % 6) {
                            case 0: neuralGroups_.motorForward_.push_back(n); break;
                            case 1: neuralGroups_.motorBackward_.push_back(n); break;
                            case 2: neuralGroups_.motorTurnLeft_.push_back(n); break;
                            case 3: neuralGroups_.motorTurnRight_.push_back(n); break;
                            case 4: neuralGroups_.motorInteract_.push_back(n); break;
                            case 5: neuralGroups_.motorWait_.push_back(n); break;
                        }
                        idx++;
                    }
                } else if (type == NeuronType::Sensory) {
                    size_t idx = 0;
                    for (Neuron* n : pop->getNeurons()) {
                        switch (idx % 4) {
                            case 0: neuralGroups_.sensoryVision_.push_back(n); break;
                            case 1: neuralGroups_.sensoryTouch_.push_back(n); break;
                            case 2: neuralGroups_.sensoryInternal_.push_back(n); break;
                            case 3: neuralGroups_.sensoryProprioception_.push_back(n); break;
                        }
                        idx++;
                    }
                }
            }
        }
    }
}

size_t AgentBrain::getSensoryInputSize() const {
    return 256 + 8 + 4 + 6;
}

size_t AgentBrain::getMotorOutputSize() const {
    return 6;
}

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) {
        NLM_LOG_WARNING("AgentBrain::processSensoryInput: Null brain pointer");
        return;
    }
    
    const auto& vision = percept.getVision();
    
    // Process vision input with bounds checking
    for (size_t i = 0; i < neuralGroups_.sensoryVision_.size() && i < vision.size(); ++i) {
        Neuron* neuron = neuralGroups_.sensoryVision_[i];
        if (neuron) {
            float current = vision[i] * 5.0f;
            neuron->injectCurrent(current);
        }
    }
    
    // Process touch input
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < neuralGroups_.sensoryTouch_.size() && i < touch.size(); ++i) {
        Neuron* neuron = neuralGroups_.sensoryTouch_[i];
        if (neuron) {
            float current = touch[i] * 8.0f;
            neuron->injectCurrent(current);
        }
    }
    
    // Process internal signals
    const auto& internal = percept.getInternal();
    for (size_t i = 0; i < neuralGroups_.sensoryInternal_.size() && i < internal.size(); ++i) {
        Neuron* neuron = neuralGroups_.sensoryInternal_[i];
        if (neuron) {
            float current = (internal[i] * 2.0f - 1.0f) * 5.0f;
            neuron->injectCurrent(current);
        }
    }
    
    // Process proprioception
    const auto& proprioception = percept.getProprioception();
    for (size_t i = 0; i < neuralGroups_.sensoryProprioception_.size() && i < proprioception.size(); ++i) {
        Neuron* neuron = neuralGroups_.sensoryProprioception_[i];
        if (neuron) {
            float current = (proprioception[i] * 2.0f - 1.0f) * 3.0f;
            neuron->injectCurrent(current);
        }
    }
    
    // Compute novelty
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        size_t maxSize = std::min(vision.size(), previousVision_.size());
        
        for (size_t i = 0; i < maxSize; ++i) {
            float diff = std::abs(vision[i] - previousVision_[i]);
            totalDiff += diff;
        }
        
        noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
        noveltyLevel_ *= sensoryNoveltyDecay_;
        
        previousVision_ = vision;
    }
    
    // Update curiosity
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) {
        NLM_LOG_WARNING("AgentBrain::decodeMotorCommand: Null brain pointer");
        return MotorCommand::Wait;
    }
    
    // Calculate motor activity using enhanced neural group structure
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return sum / neurons.size();
    };
    
    float forwardAct = calcActivity(neuralGroups_.motorForward_);
    float backwardAct = calcActivity(neuralGroups_.motorBackward_);
    float leftAct = calcActivity(neuralGroups_.motorTurnLeft_);
    float rightAct = calcActivity(neuralGroups_.motorTurnRight_);
    float interactAct = calcActivity(neuralGroups_.motorInteract_);
    float waitAct = calcActivity(neuralGroups_.motorWait_);
    
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
    float bestActivity = waitAct;
    
    for (const auto& c : commands) {
        if (c.activity > bestActivity) {
            bestActivity = c.activity;
            best = c.cmd;
        }
    }
    
    // Only act if there's meaningful activity
    if (bestActivity < 0.5f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulationEnabled_) {
        NLM_LOG_DEBUG("AgentBrain::applyRewardModulation: Disabled or null brain");
        return;
    }
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal
    dopamineLevel_ = predictionError_;
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply reward-modulated weight changes
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                // Apply reward-modulated weight change
                float delta = eligibility * dopamineLevel_ * plasticityModifier_;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    // Modulate plasticity based on dopamine
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP rules
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentEnabled_) {
        NLM_LOG_DEBUG("AgentBrain::updateDevelopment: Disabled or null brain");
        return;
    }
    
    developmentalAge_ += timestep;
    
    // Determine development stage and plasticity modifier
    if (developmentalAge_ < 60.0) {  // ~1 minute
        plasticityModifier_ = 1.0f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0) {  // ~15 minutes
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = 0.2f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            float synRate = 0.0001f * plasticityModifier_;
            float pruneRate = 0.00001f * (2.0f - plasticityModifier_);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) return DevelopmentalStage::Initial;
    return brain_->getDevelopmentalStage();
}

float AgentBrain::getNeuromodulationLevel() const {
    return dopamineLevel_;
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
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Clear previous vision
    std::fill(previousVision_.begin(), previousVision_.end(), 0.0f);
    
    NLM_LOG_INFO("AgentBrain reset to initial state");
}

} // namespace nlm
