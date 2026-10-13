#pragma once

#include "AgentBrain.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <vector>

namespace nlm {

/**
 * EventProcessor - Processes sensory input and generates neural events
 * Extracted from AgentBrain to improve separation of concerns
 */
class EventProcessor {
public:
    EventProcessor(std::shared_ptr<Brain> brain);
    ~EventProcessor() = default;
    
    // Disable copying, enable moving
    EventProcessor(const EventProcessor&) = delete;
    EventProcessor& operator=(const EventProcessor&) = delete;
    EventProcessor(EventProcessor&&) noexcept;
    EventProcessor& operator=(EventProcessor&&) noexcept;
    
    // Process sensory input and update neural activity
    void processSensoryInput(const SensoryPercept& percept);
    
    // Calculate novelty (pattern change detection)
    float calculateNovelty(const std::vector<float>& current, const std::vector<float>& previous) const;
    
    // Get current sensory distribution
    const std::vector<Neuron*>& getVisionNeurons() const { return visionNeurons_; }
    const std::vector<Neuron*>& getTouchNeurons() const { return touchNeurons_; }
    const std::vector<Neuron*>& getInternalNeurons() const { return internalNeurons_; }
    const std::vector<Neuron*>& getProprioceptionNeurons() const { return proprioceptionNeurons_; }
    
    // Configuration
    void setSensoryGain(float vision, float touch, float internal, float proprioception) {
        sensoryGain_.vision = vision;
        sensoryGain_.touch = touch;
        sensoryGain_.internal = internal;
        sensoryGain_.proprioception = proprioception;
    }
    
    // State management
    void clearPreviousInput() { previousVision_.clear(); }
    void setPreviousVision(const std::vector<float>& vision) { previousVision_ = vision; }
    
private:
    std::shared_ptr<Brain> brain_;
    
    // Sensory neuron distributions
    std::vector<Neuron*> visionNeurons_;
    std::vector<Neuron*> touchNeurons_;
    std::vector<Neuron*> internalNeurons_;
    std::vector<Neuron*> proprioceptionNeurons_;
    
    // Novelty tracking
    std::vector<float> previousVision_;
    float noveltyLevel_ = 0.0f;
    
    // Sensory configuration
    struct SensoryGain {
        float vision = 5.0f;
        float touch = 8.0f;
        float internal = 5.0f;
        float proprioception = 3.0f;
    } sensoryGain_;
};

/**
 * ActionDecoder - Decodes neural activity into motor commands
 * Extracted from AgentBrain to improve separation of concerns
 */
class ActionDecoder {
public:
    ActionDecoder(std::shared_ptr<Brain> brain);
    ~ActionDecoder() = default;
    
    // Disable copying, enable moving
    ActionDecoder(const ActionDecoder&) = delete;
    ActionDecoder& operator=(const ActionDecoder&) = delete;
    ActionDecoder(ActionDecoder&&) noexcept;
    ActionDecoder& operator=(ActionDecoder&&) noexcept;
    
    // Decode motor command from neural activity
    MotorCommand decodeMotorCommand();
    
    // Get motor neuron distributions
    const std::vector<Neuron*>& getMotorForward() const { return motorForward_; }
    const std::vector<Neuron*>& getMotorBackward() const { return motorBackward_; }
    const std::vector<Neuron*>& getMotorTurnLeft() const { return motorTurnLeft_; }
    const std::vector<Neuron*>& getMotorTurnRight() const { return motorTurnRight_; }
    const std::vector<Neuron*>& getMotorInteract() const { return motorInteract_; }
    const std::vector<Neuron*>& getMotorWait() const { return motorWait_; }
    
    // Configuration
    void setActivityThreshold(float threshold) { activityThreshold_ = threshold; }
    void setRandomExploration(bool enabled) { randomExplorationEnabled_ = enabled; }
    
    // State management
    void reset() {
        motorActivity_.clear();
    }
    
private:
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron distributions
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Neural activity tracking
    std::unordered_map<MotorCommand, float> motorActivity_;
    
    // Configuration
    float activityThreshold_ = 0.5f;
    bool randomExplorationEnabled_ = true;
    
    // Random number generator
    std::mt19937 rng_;
};

/**
 * NeuromodulationManager - Manages neuromodulatory signals and effects
 * Extracted from AgentBrain to improve separation of concerns
 */
class NeuromodulationManager {
public:
    NeuromodulationManager(std::shared_ptr<Brain> brain);
    ~NeuromodulationManager() = default;
    
    // Disable copying, enable moving
    NeuromodulationManager(const NeuromodulationManager&) = delete;
    NeuromodulationManager& operator=(const NeuromodulationManager&) = delete;
    NeuromodulationManager(NeuromodulationManager&&) noexcept;
    NeuromodulationManager& operator=(NeuromodulationManager&&) noexcept;
    
    // Apply reward modulation based on prediction error
    void applyRewardModulation(float reward, float predictedReward);
    
    // Update development effects
    void updateDevelopment(double timestep);
    
    // Get neuromodulation levels
    float getDopamineLevel() const { return dopamineLevel_; }
    float getCuriosityLevel() const { return curiosityLevel_; }
    float getNoveltyLevel() const { return noveltyLevel_; }
    float getPredictionError() const { return predictionError_; }
    float getExpectedReward() const { return expectedReward_; }
    float getDevelopmentalAge() const { return developmentalAge_; }
    float getPlasticityModifier() const { return plasticityModifier_; }
    
    // Configuration
    void setDevelopmentEnabled(bool enabled) { developmentEnabled_ = enabled; }
    void setCuriosityEnabled(bool enabled) { curiosityEnabled_ = enabled; }
    void setStructuralPlasticityEnabled(bool enabled) { structuralPlasticityEnabled_ = enabled; }
    void setRewardModulationEnabled(bool enabled) { rewardModulationEnabled_ = enabled; }
    
    // State management
    void reset() {
        dopamineLevel_ = 0.0f;
        noveltyLevel_ = 0.0f;
        curiosityLevel_ = 0.0f;
        predictionError_ = 0.0f;
        expectedReward_ = 0.0f;
        developmentalAge_ = 0.0f;
        plasticityModifier_ = 1.0f;
    }
    
private:
    std::shared_ptr<Brain> brain_;
    
    // Neuromodulation levels
    float dopamineLevel_ = 0.0f;
    float noveltyLevel_ = 0.0f;
    float curiosityLevel_ = 0.0f;
    float predictionError_ = 0.0f;
    float expectedReward_ = 0.0f;
    float developmentalAge_ = 0.0f;
    float plasticityModifier_ = 1.0f;
    
    // Configuration flags
    bool rewardModulationEnabled_ = true;
    bool structuralPlasticityEnabled_ = true;
    bool developmentEnabled_ = true;
    bool curiosityEnabled_ = true;
    bool sensoryNoveltyDecay_ = true;
    
    // Novelty decay factor
    float noveltyDecayFactor_ = 0.99f;
};

// EventProcessor implementation
EventProcessor::EventProcessor(std::shared_ptr<Brain> brain) : brain_(std::move(brain)) {
    if (brain_) {
        // Initialize vision sensory neurons
        visionNeurons_ = brain_->getVisionNeurons();
        touchNeurons_ = brain_->getTouchNeurons();
        internalNeurons_ = brain_->getInternalNeurons();
        proprioceptionNeurons_ = brain_->getProprioceptionNeurons();
    }
}

EventProcessor::EventProcessor(EventProcessor&& other) noexcept
    : brain_(std::move(other.brain_))
    , visionNeurons_(std::move(other.visionNeurons_))
    , touchNeurons_(std::move(other.touchNeurons_))
    , internalNeurons_(std::move(other.internalNeurons_))
    , proprioceptionNeurons_(std::move(other.proprioceptionNeurons_))
    , previousVision_(std::move(other.previousVision_))
    , noveltyLevel_(other.noveltyLevel_)
    , sensoryGain_(other.sensoryGain_) {
}

EventProcessor& EventProcessor::operator=(EventProcessor&& other) noexcept {
    if (this != &other) {
        brain_ = std::move(other.brain_);
        visionNeurons_ = std::move(other.visionNeurons_);
        touchNeurons_ = std::move(other.touchNeurons_);
        internalNeurons_ = std::move(other.internalNeurons_);
        proprioceptionNeurons_ = std::move(other.proprioceptionNeurons_);
        previousVision_ = std::move(other.previousVision_);
        noveltyLevel_ = other.noveltyLevel_;
        sensoryGain_ = other.sensoryGain_;
    }
    return *this;
}

void EventProcessor::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Vision input (256 values -> visionNeurons_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < visionNeurons_.size() && i < vision.size(); ++i) {
        if (visionNeurons_[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * sensoryGain_.vision;  // Scale factor
            visionNeurons_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> touchNeurons_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < touchNeurons_.size() && i < touch.size(); ++i) {
        if (touchNeurons_[i]) {
            float current = touch[i] * sensoryGain_.touch;  // Collision signal
            touchNeurons_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> internalNeurons_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < internalNeurons_.size() && i < intern.size(); ++i) {
        if (internalNeurons_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * sensoryGain_.internal;  // Center and scale
            internalNeurons_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> proprioceptionNeurons_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < proprioceptionNeurons_.size() && i < proprio.size(); ++i) {
        if (proprioceptionNeurons_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * sensoryGain_.proprioception;  // Center and scale
            proprioceptionNeurons_[i]->injectCurrent(current);
        }
    }
    
    // Compute novelty (difference from previous vision)
    if (!vision.empty()) {
        noveltyLevel_ = calculateNovelty(vision, previousVision_);
        
        // Decay novelty
        if (sensoryNoveltyDecay_) {
            noveltyLevel_ *= noveltyDecayFactor_;
        }
        
        // Store for next time
        previousVision_ = vision;
    }
}

float EventProcessor::calculateNovelty(const std::vector<float>& current, const std::vector<float>& previous) const {
    if (current.empty()) return 0.0f;
    
    float totalDiff = 0.0f;
    for (size_t i = 0; i < current.size() && i < previous.size(); ++i) {
        float diff = std::abs(current[i] - previous[i]);
        totalDiff += diff;
    }
    
    // Normalize
    return totalDiff / static_cast<float>(std::max<size_t>(current.size(), 1));
}

// ActionDecoder implementation
ActionDecoder::ActionDecoder(std::shared_ptr<Brain> brain) : brain_(std::move(brain)) {
    if (brain_) {
        // Initialize motor and sensory neuron groups (simplified from AgentBrain)
        for (const auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                NeuronType type = pop->getNeuronType();
                
                if (type == NeuronType::Motor) {
                    for (Neuron* n : pop->getNeurons()) {
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
                } else if (type == NeuronType::Sensory) {
                    for (Neuron* n : pop->getNeurons()) {
                        // Store sensory neurons for later use
                        // (In real implementation, would categorize by modality)
                        motorForward_.push_back(n); // Placeholder
                    }
                }
            }
        }
    }
    
    // Initialize random number generator
    std::random_device rd;
    rng_ = std::mt19937(rd());
}

ActionDecoder::ActionDecoder(ActionDecoder&& other) noexcept
    : brain_(std::move(other.brain_))
    , motorForward_(std::move(other.motorForward_))
    , motorBackward_(std::move(other.motorBackward_))
    , motorTurnLeft_(std::move(other.motorTurnLeft_))
    , motorTurnRight_(std::move(other.motorTurnRight_))
    , motorInteract_(std::move(other.motorInteract_))
    , motorWait_(std::move(other.motorWait_))
    , motorActivity_(std::move(other.motorActivity_))
    , activityThreshold_(other.activityThreshold_)
    , randomExplorationEnabled_(other.randomExplorationEnabled_)
    , rng_(std::move(other.rng_)) {
}

ActionDecoder& ActionDecoder::operator=(ActionDecoder&& other) noexcept {
    if (this != &other) {
        brain_ = std::move(other.brain_);
        motorForward_ = std::move(other.motorForward_);
        motorBackward_ = std::move(other.motorBackward_);
        motorTurnLeft_ = std::move(other.motorTurnLeft_);
        motorTurnRight_ = std::move(other.motorTurnRight_);
        motorInteract_ = std::move(other.motorInteract_);
        motorWait_ = std::move(other.motorWait_);
        motorActivity_ = std::move(other.motorActivity_);
        activityThreshold_ = other.activityThreshold_;
        randomExplorationEnabled_ = other.randomExplorationEnabled_;
        rng_ = std::move(other.rng_);
    }
    return *this;
}

MotorCommand ActionDecoder::decodeMotorCommand() {
    // Calculate average activity in each motor group
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            // Use membrane potential deviation from rest as activity measure
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return sum / neurons.size();
    };
    
    float forwardAct = calcActivity(motorForward_);
    float backwardAct = calcActivity(motorBackward_);
    float leftAct = calcActivity(motorTurnLeft_);
    float rightAct = calcActivity(motorTurnRight_);
    float interactAct = calcActivity(motorInteract_);
    float waitAct = calcActivity(motorWait_);
    
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
    float bestActivity = waitAct;  // Default to wait if nothing stronger
    
    for (const auto& c : commands) {
        if (c.activity > bestActivity) {
            bestActivity = c.activity;
            best = c.cmd;
        }
    }
    
    // Only act if there's meaningful activity
    if (bestActivity < activityThreshold_) {
        return MotorCommand::Wait;
    }
    
    return best;
}

// NeuromodulationManager implementation
NeuromodulationManager::NeuromodulationManager(std::shared_ptr<Brain> brain) : brain_(std::move(brain)) {
}

NeuromodulationManager::NeuromodulationManager(NeuromodulationManager&& other) noexcept
    : brain_(std::move(other.brain_))
    , dopamineLevel_(other.dopamineLevel_)
    , noveltyLevel_(other.noveltyLevel_)
    , curiosityLevel_(other.curiosityLevel_)
    , predictionError_(other.predictionError_)
    , expectedReward_(other.expectedReward_)
    , developmentalAge_(other.developmentalAge_)
    , plasticityModifier_(other.plasticityModifier_)
    , rewardModulationEnabled_(other.rewardModulationEnabled_)
    , structuralPlasticityEnabled_(other.structuralPlasticityEnabled_)
    , developmentEnabled_(other.developmentEnabled_)
    , curiosityEnabled_(other.curiosityEnabled_)
    , noveltyDecayFactor_(other.noveltyDecayFactor_) {
}

NeuromodulationManager& NeuromodulationManager::operator=(NeuromodulationManager&& other) noexcept {
    if (this != &other) {
        brain_ = std::move(other.brain_);
        dopamineLevel_ = other.dopamineLevel_;
        noveltyLevel_ = other.noveltyLevel_;
        curiosityLevel_ = other.curiosityLevel_;
        predictionError_ = other.predictionError_;
        expectedReward_ = other.expectedReward_;
        developmentalAge_ = other.developmentalAge_;
        plasticityModifier_ = other.plasticityModifier_;
        rewardModulationEnabled_ = other.rewardModulationEnabled_;
        structuralPlasticityEnabled_ = other.structuralPlasticityEnabled_;
        developmentEnabled_ = other.developmentEnabled_;
        curiosityEnabled_ = other.curiosityEnabled_;
        noveltyDecayFactor_ = other.noveltyDecayFactor_;
    }
    return *this;
}

void NeuromodulationManager::applyRewardModulation(float reward, float predictedReward) {
    if (!rewardModulationEnabled_ || !brain_) return;
    
    // Compute prediction error
    predictionError_ = reward - predictedReward;
    
    // Update expected reward (exponential moving average)
    expectedReward_ = 0.95f * expectedReward_ + 0.05f * reward;
    
    // Dopamine-like signal (based on prediction error)
    dopamineLevel_ = predictionError_;
    
    // Clamp to reasonable range
    dopamineLevel_ = std::clamp(dopamineLevel_, -1.0f, 1.0f);
    
    // Apply to all synapses with eligibility traces
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
    // Positive dopamine increases plasticity, negative decreases
    float plasticityFactor = 0.5f + 0.5f * dopamineLevel_;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Apply to STDP
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        stdp->setLTPWeight(0.01f * plasticityFactor);
        stdp->setLTDWeight(0.012f * plasticityFactor);
    }
}

void NeuromodulationManager::updateDevelopment(double timestep) {
    if (!developmentEnabled_ || !brain_) return;
    
    developmentalAge_ += timestep;
    
    // Simple developmental stages based on age
    // This is a biologically inspired approximation
    if (developmentalAge_ < 60.0) {  // ~1 minute
        plasticityModifier_ = 1.0f;  // High plasticity
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0) {  // ~5 minutes
        plasticityModifier_ = 0.8f;
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0) {  // ~15 minutes
        plasticityModifier_ = 0.5f;
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        plasticityModifier_ = 0.2f;  // Adult - more stable
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Structural plasticity changes with development
    if (structuralPlasticityEnabled_) {
        auto* sp = brain_->getStructuralPlasticity();
        if (sp) {
            // Higher synaptogenesis in early development
            float synRate = 0.0001f * plasticityModifier_;
            float pruneRate = 0.00001f * (2.0f - plasticityModifier_);
            sp->setSynaptogenesisRate(synRate);
            sp->setPruningRate(pruneRate);
        }
    }
}

} // namespace nlm