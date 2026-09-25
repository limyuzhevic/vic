#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
{
    // Initialize components with default settings
    stateManager_.setSensoryNoveltyDecay(0.99f);
    curiosityManager_.setCuriosityEnabled(true);
    rewardModulator_.setRewardModulationEnabled(true);
    developmentalController_.setDevelopmentEnabled(true);
    developmentalController_.setStructuralPlasticityEnabled(true);
    
    NLM_LOG_INFO("AgentBrain created with modular components");
}

AgentBrain::~AgentBrain() = default;

void AgentBrain::initialize(const SimpleWorld& world) {
    // Initialize state manager with world vision dimensions
    previousVision_.resize(world.getVisionWidth() * world.getVisionHeight(), 0.0f);
    stateManager_.setPreviousVision(previousVision_);
    
    // Update motor distribution with brain neurons
    if (brain_) {
        updateComponentNeuronalGroups();
    }
    
    developmentalController_.reset();
    stateManager_.reset();
    
    NLM_LOG_INFO("AgentBrain initialized with " + 
                 std::to_string(stateManager_.getPreviousVision().size()) + " vision memory slots");
}

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
    
    // Update motor distribution with latest brain neurons
    if (brain_->getRegions().size() > 0) {
        auto neurons = brain_->getRegions()[0]->getAllNeurons();
        motorDistribution_.distributeMotorNeurons(neurons);
        updateComponentNeuronalGroups();
    }
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    sensoryProcessor_.processVisionInput(
        sensoryVision_, vision, 5.0f  // Scale factor
    );
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    sensoryProcessor_.processTouchInput(
        sensoryTouch_, touch, 8.0f  // Collision signal
    );
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    sensoryProcessor_.processInternalInput(
        sensoryInternal_, intern, 5.0f  // Center and scale
    );
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    sensoryProcessor_.processProprioceptionInput(
        sensoryProprioception_, proprio, 3.0f  // Center and scale
    );
    
    // Compute novelty (difference from previous vision)
    if (!vision.empty()) {
        float novelty = sensoryProcessor_.computeNovelty(vision, previousVision_);
        
        // Store for next time
        previousVision_ = vision;
        stateManager_.setPreviousVision(previousVision_);
        
        // Update curiosity based on novelty
        if (curiosityManager_.isCuriosityEnabled()) {
            float predictionError = stateManager_.getPredictionError();
            float curiosity = curiosityManager_.updateCuriosityLevel(
                novelty, predictionError, 2.0f, 0.5f
            );
        }
    }
    
    NLM_LOG_DEBUG("Processed sensory input: vision size=" + std::to_string(vision.size()) +
                 " novelty=" + std::to_string(stateManager_.getNoveltyLevel()));
}

MotorCommand AgentBrain::decodeMotorCommand() {
    if (!brain_) return MotorCommand::Wait;
    
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // Apply curiosity-based exploration
    if (curiosityManager_.isCuriosityEnabled() && 
        stateManager_.getCuriosityLevel() > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::decodeFromMotorNeurons() {
    // Calculate average activity in each motor group using MotorDecoder
    float forwardAct = motorDecoder_.calculateActivity(motorForward_);
    float backwardAct = motorDecoder_.calculateActivity(motorBackward_);
    float leftAct = motorDecoder_.calculateActivity(motorTurnLeft_);
    float rightAct = motorDecoder_.calculateActivity(motorTurnRight_);
    float interactAct = motorDecoder_.calculateActivity(motorInteract_);
    float waitAct = motorDecoder_.calculateActivity(motorWait_);
    
    // Get motor command from activity comparison
    return motorDecoder_.selectCommandFromActivity(
        forwardAct, backwardAct, leftAct, rightAct, interactAct, waitAct
    );
}

MotorCommand AgentBrain::selectWithCuriosity(MotorCommand defaultCmd) {
    float curiosityLevel = stateManager_.getCuriosityLevel();
    
    // Exploration: occasionally choose random action when curiosity is high
    if (curiosityLevel > 0.5f) {
        // Higher curiosity = more exploration
        float exploreChance = curiosityLevel * 0.3f;  // Up to 30% random
        
        float r = curiosityLevel; // Simplified for demo
        if (r < exploreChance) {
            // Random motor command
            int choice = static_cast<int>(curiosityLevel * 7.0f) % 8; // 0-7
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

void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    if (!brain_ || !rewardModulator_.isRewardModulationEnabled()) return;
    
    // Apply reward modulation using RewardModulator component
    rewardModulator_.applyRewardModulation(
        reward, predictedReward, stateManager_.getPlasticityModifier()
    );
    
    // Sync state with RewardModulator
    stateManager_.setDopamineLevel(rewardModulator_.getDopamineLevel());
    stateManager_.setPredictionError(rewardModulator_.getPredictionError());
    stateManager_.setExpectedReward(rewardModulator_.getExpectedReward());
    
    NLM_LOG_DEBUG("Applied reward modulation: reward=" + std::to_string(reward) +
                 " prediction_error=" + std::to_string(rewardModulator_.getPredictionError()));
}

void AgentBrain::updateDevelopment(double timestep) {
    if (!brain_ || !developmentalController_.isDevelopmentEnabled()) return;
    
    double developmentalAge = stateManager_.getDevelopmentalAge();
    float plasticityModifier = stateManager_.getPlasticityModifier();
    
    // Update development using DevelopmentalController
    developmentalController_.updateDevelopment(
        timestep, developmentalAge, plasticityModifier, true
    );
    
    // Sync state with DevelopmentalController
    stateManager_.setDevelopmentalAge(developmentalAge);
    stateManager_.setPlasticityModifier(plasticityModifier);
    
    // Log developmental stage
    DevelopmentalStage stage = developmentalController_.getDevelopmentalStage(developmentalAge);
    NLM_LOG_INFO("Development stage: " + std::string(developmentalController_.developmentalStageToString(stage)) +
                 " (age=" + std::to_string(developmentalAge) + "s, plasticity=" + std::to_string(plasticityModifier) + ")");
}

DevelopmentalStage AgentBrain::getDevelopmentalStage() const {
    if (!brain_) return DevelopmentalStage::Initial;
    return developmentalController_.getDevelopmentalStage(stateManager_.getDevelopmentalAge());
}

float AgentBrain::getNeuromodulationLevel() const {
    return stateManager_.getDopamineLevel();
}

float AgentBrain::getCuriosityLevel() const {
    return stateManager_.getCuriosityLevel();
}

float AgentBrain::getNoveltyLevel() const {
    return sensoryProcessor_.getNoveltyLevel();
}

float AgentBrain::getPredictionError() const {
    return stateManager_.getPredictionError();
}

void AgentBrain::reset() {
    // Reset all components
    stateManager_.reset();
    sensoryProcessor_.reset();
    curiosityManager_.reset();
    rewardModulator_.reset();
    developmentalController_.reset();
    
    // Clear motor and sensory neuron groups
    motorForward_.clear();
    motorBackward_.clear();
    motorTurnLeft_.clear();
    motorTurnRight_.clear();
    motorInteract_.clear();
    motorWait_.clear();
    
    sensoryVision_.clear();
    sensoryTouch_.clear();
    sensoryInternal_.clear();
    sensoryProprioception_.clear();
    
    NLM_LOG_INFO("AgentBrain reset and ready for new episode");
}

// Helper method to update neuronal groups from brain
void AgentBrain::updateComponentNeuronalGroups() {
    if (!brain_ || brain_->getRegions().empty()) {
        return;
    }
    
    auto neurons = brain_->getRegions()[0]->getAllNeurons();
    
    // Distribute and categorize neurons
    motorDistribution_.distributeMotorNeurons(neurons);
    
    // Update local references
    motorForward_ = motorDistribution_.getMotorForward();
    motorBackward_ = motorDistribution_.getMotorBackward();
    motorTurnLeft_ = motorDistribution_.getMotorTurnLeft();
    motorTurnRight_ = motorDistribution_.getMotorTurnRight();
    motorInteract_ = motorDistribution_.getMotorInteract();
    motorWait_ = motorDistribution_.getMotorWait();
    
    // For now, we'll need to populate sensory neurons based on brain structure
    // This is a simplified version - in a full implementation, we'd need proper
    // sensory neuron identification from the brain structure
}

// Helper method to synchronize state
void AgentBrain::synchronizeStateWithComponents() {
    // Update state manager from component states
    stateManager_.setCuriosityLevel(curiosityManager_.getCuriosityLevel());
    stateManager_.setNoveltyLevel(sensoryProcessor_.getNoveltyLevel());
}

} // namespace nlm
