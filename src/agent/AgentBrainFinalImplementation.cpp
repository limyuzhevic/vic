// Final integrated AgentBrain implementation using helper classes
// This combines all the improvements for better encapsulation and error handling

#include "AgentBrain.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <cassert>
#include <stdexcept>

namespace nlm {

// AgentBrain constructor - simplified and using helper classes
AgentBrain::AgentBrain(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , neuronManager_()
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
    , lastWinnerGroup_(-1)
    , lastWinnerMargin_(0.0f)
    , consecutiveWinnerCount_(0)
    , stepsSinceTurn_(0)
    , repeatCount_(0)
    , reflexBonusCount_(0)
    , reflexSuppressCount_(0)
    , wtaPulseCount_(0)
    , lastDecoded_(MotorCommand::Wait)
    , hasLastDecoded_(false)
    , selfModelReady_(false)
    , socialReady_(false)
    , plannerOverrideCount_(0)
    , plannerAgreementCount_(0)
    , plannerDecisionCount_(0)
    , conceptBiasCount_(0)
    , selfBiasCount_(0)
    , socialBiasCount_(0)
{
    NLM_LOG_INFO("Creating AgentBrain with " + std::to_string(brain ? 1 : 0) + " brain pointer");
    
    // Validate input brain pointer
    if (!AgentBrainStateValidator::validateBrainPointer(brain_)) {
        NLM_LOG_ERROR("Failed to create AgentBrain: invalid brain pointer");
        // Note: This doesn't throw exception due to C++ design constraints
        return;
    }
    
    // Initialize neuron manager using helper class
    initializeNeuronManager();
    
    // Validate initial state
    validateState();
    
    NLM_LOG_INFO("AgentBrain created successfully");
}

// AgentBrain destructor
AgentBrain::~AgentBrain() {
    NLM_LOG_INFO("Destroying AgentBrain");
}

// Initialize with world - using helper classes for validation
void AgentBrain::initialize(const SimpleWorld& world) {
    NLM_LOG_INFO("Initializing AgentBrain with world");
    
    if (!brain_) {
        NLM_LOG_ERROR("Cannot initialize AgentBrain: brain pointer is null");
        return;
    }
    
    // Validate world parameter
    if (world.getWidth() <= 0 || world.getHeight() <= 0) {
        NLM_LOG_ERROR("Invalid world dimensions: " + std::to_string(world.getWidth()) + "x" + std::to_string(world.getHeight()));
        return;
    }
    
    // Reinitialize neuron manager with current brain
    initializeNeuronManager();
    
    // Reset developmental state
    resetDevelopmentalState();
    
    // Validate state after initialization
    validateState();
    
    NLM_LOG_INFO("AgentBrain initialization complete");
}

// Reset developmental state
void AgentBrain::resetDevelopmentalState() {
    NLM_LOG_DEBUG("Resetting developmental state");
    
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Reset neuromodulation levels
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    
    // Reset cognitive counters
    lastWinnerGroup_ = -1;
    lastWinnerMargin_ = 0.0f;
    consecutiveWinnerCount_ = 0;
    stepsSinceTurn_ = 0;
    repeatCount_ = 0;
    
    // Reset reflex counters
    reflexBonusCount_ = 0;
    reflexSuppressCount_ = 0;
    wtaPulseCount_ = 0;
    
    // Reset action tracking
    lastDecoded_ = MotorCommand::Wait;
    hasLastDecoded_ = false;
    
    // Reset cognitive state
    lastVision_.clear();
    lastTouch_.clear();
    lastText_.clear();
    thoughtPercept_ = "none";
    thoughtPlanned_ = MotorCommand::Wait;
    hasThoughtPlanned_ = false;
    thoughtReward_ = 0.0f;
    hasThoughtOutcome_ = false;
    
    // Reset system states
    selfModelReady_ = false;
    socialReady_ = false;
    
    // Reset planner/concept/social counters
    plannerOverrideCount_ = 0;
    plannerAgreementCount_ = 0;
    plannerDecisionCount_ = 0;
    conceptBiasCount_ = 0;
    selfBiasCount_ = 0;
    socialBiasCount_ = 0;
    
    NLM_LOG_DEBUG("Developmental state reset complete");
}

// Reset agent for new episode - public interface
void AgentBrain::reset() {
    NLM_LOG_INFO("Resetting AgentBrain for new episode");
    
    if (!brain_) {
        NLM_LOG_ERROR("Cannot reset AgentBrain: brain pointer is null");
        return;
    }
    
    resetDevelopmentalState();
    validateState();
    
    NLM_LOG_INFO("AgentBrain reset complete");
}

// Get all sensory signals
std::vector<float> AgentBrain::getAllSignals() const {
    NLM_LOG_DEBUG("Getting all sensory signals");
    
    std::vector<float> allSignals;
    
    // Get vision signals
    allSignals.insert(allSignals.end(), vision_.begin(), vision_.end());
    
    // Get touch signals
    allSignals.insert(allSignals.end(), touch_.begin(), touch_.end());
    
    // Get internal signals
    allSignals.insert(allSignals.end(), internal_.begin(), internal_.end());
    
    // Get proprioception signals
    allSignals.insert(allSignals.end(), proprioception_.begin(), proprioception_.end());
    
    return allSignals;
}

// Get motor neuron groups
const std::vector<Neuron*>& AgentBrain::getMotorForward() const {
    return neuronManager_.getMotorForward();
}

const std::vector<Neuron*>& AgentBrain::getMotorBackward() const {
    return neuronManager_.getMotorBackward();
}

const std::vector<Neuron*>& AgentBrain::getMotorTurnLeft() const {
    return neuronManager_.getMotorTurnLeft();
}

const std::vector<Neuron*>& AgentBrain::getMotorTurnRight() const {
    return neuronManager_.getMotorTurnRight();
}

const std::vector<Neuron*>& AgentBrain::getMotorInteract() const {
    return neuronManager_.getMotorInteract();
}

const std::vector<Neuron*>& AgentBrain::getMotorWait() const {
    return neuronManager_.getMotorWait();
}

// Get sensory neuron groups
const std::vector<Neuron*>& AgentBrain::getSensoryVision() const {
    return neuronManager_.getSensoryVision();
}

const std::vector<Neuron*>& AgentBrain::getSensoryTouch() const {
    return neuronManager_.getSensoryTouch();
}

const std::vector<Neuron*>& AgentBrain::getSensoryInternal() const {
    return neuronManager_.getSensoryInternal();
}

const std::vector<Neuron*>& AgentBrain::getSensoryProprioception() const {
    return neuronManager_.getSensoryProprioception();
}

const std::vector<Neuron*>& AgentBrain::getInternalNeurons() const {
    return neuronManager_.getInternalNeurons();
}

// Validate state using helper class
bool AgentBrain::validateState() const {
    return AgentBrainStateValidator::validateOverallState(
        neuronManager_, dopamineLevel_, noveltyLevel_, curiosityLevel_, predictionError_,
        developmentalAge_, plasticityModifier_, rewardModulationEnabled_,
        structuralPlasticityEnabled_, developmentEnabled_, curiosityEnabled_,
        lastVision_, lastTouch_, lastText_);
}

// Initialize neuron manager
void AgentBrain::initializeNeuronManager() {
    neuronManager_.addNeurons(brain_);
}

} // namespace nlm