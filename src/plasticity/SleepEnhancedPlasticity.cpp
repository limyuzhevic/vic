#include "SleepEnhancedPlasticity.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

struct SleepEnhancedPlasticity::Impl {
    Brain* brain;
    
    // State-specific plasticity parameters
    std::map<SleepState, StatePlasticityParameters> stateParameters;
    
    // Statistics
    float totalWeightChange;
    size_t consolidationCount;
    size_t reorganizationCount;
    
    // Error handling
    bool inErrorState;
    std::string lastErrorMessage;
    size_t consecutiveErrors;
    
    // State management
    SleepState currentState;
    
    // Synaptic homeostasis
    float homeostasisLevel;
    float scalingFactor;
    
    // Memory replay integration
    float replayIntegrationStrength;
    std::vector<float> lastReplayedPattern;
    
    Impl() : brain(nullptr)
        , totalWeightChange(0.0f)
        , consolidationCount(0)
        , reorganizationCount(0)
        , inErrorState(false)
        , consecutiveErrors(0)
        , currentState(SleepState::Wake)
        , homeostasisLevel(1.0f)
        , scalingFactor(1.0f)
        , replayIntegrationStrength(0.0f) {
        
        // Initialize state-specific parameters
        stateParameters[SleepState::QuietSleep] = {
            0.005f,    // stdpLearningRate
            0.01f,     // hebbianLearningRate
            0.3f,      // consolidationFactor
            0.1f,      // reorganizationFactor
            0.9f,      // downscalingFactor
            true,      // enableReplayIntegration
            0.8f       // neuromodulationSensitivity
        };
        
        stateParameters[SleepState::DeepSleep] = {
            0.002f,    // stdpLearningRate
            0.005f,    // hebbianLearningRate
            0.7f,      // consolidationFactor
            0.8f,      // reorganizationFactor
            0.7f,      // downscalingFactor
            true,      // enableReplayIntegration
            0.6f       // neuromodulationSensitivity
        };
        
        stateParameters[SleepState::REMSleep] = {
            0.008f,    // stdpLearningRate
            0.015f,    // hebbianLearningRate
            0.5f,      // consolidationFactor
            0.4f,      // reorganizationFactor
            0.85f,     // downscalingFactor
            true,      // enableReplayIntegration
            1.0f       // neuromodulationSensitivity
        };
        
        stateParameters[SleepState::Wake] = {
            0.01f,     // stdpLearningRate (normal during wake)
            0.02f,     // hebbianLearningRate (normal during wake)
            0.0f,      // consolidationFactor (no consolidation during wake)
            0.0f,      // reorganizationFactor (no reorganization during wake)
            1.0f,      // downscalingFactor (no downscaling during wake)
            false,     // enableReplayIntegration (no replay during wake)
            0.5f       // neuromodulationSensitivity
        };
    }
};

SleepEnhancedPlasticity::SleepEnhancedPlasticity()
    : pImpl(new Impl)
    , brain_(nullptr)
    , totalSynapticWeightChange_(0.0f)
    , consolidationCount_(0)
    , reorganizationCount_(0)
    , inErrorState_(false)
    , consecutiveErrors_(0)
    , currentSleepState_(SleepState::Wake)
    , synapticHomeostasisLevel_(1.0f)
    , synapticScalingFactor_(1.0f)
    , replayIntegrationStrength_(0.0f)
{
}

SleepEnhancedPlasticity::~SleepEnhancedPlasticity() = default;

void SleepEnhancedPlasticity::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("SleepEnhancedPlasticity initialized");
}

void SleepEnhancedPlasticity::updateSTDP(Synapse* synapse, const std::vector<Timestamp>& preSpikeTimes,
                                        const std::vector<Timestamp>& postSpikeTimes, Timestamp currentTime,
                                        SleepState sleepState) {
    if (!synapse || preSpikeTimes.empty() || postSpikeTimes.empty()) return;
    
    float learningRate = getStateSpecificSTDPLearningRate(sleepState);
    applyStateSpecificSTDP(synapse, preSpikeTimes, postSpikeTimes, learningRate);
}

void SleepEnhancedPlasticity::updateHebbian(Synapse* synapse, const std::vector<Timestamp>& preSpikeTimes,
                                           const std::vector<Timestamp>& postSpikeTimes, Timestamp currentTime,
                                           SleepState sleepState) {
    if (!synapse || preSpikeTimes.empty() || postSpikeTimes.empty()) return;
    
    float learningRate = getStateSpecificHebbianLearningRate(sleepState);
    applyStateSpecificHebbian(synapse, preSpikeTimes, postSpikeTimes, learningRate);
}

void SleepEnhancedPlasticity::consolidateSynapticConnections(SleepState sleepState) {
    const auto& params = pImpl->stateParameters[sleepState];
    
    // Apply consolidation based on sleep state
    switch (sleepState) {
        case SleepState::QuietSleep:
            // Synaptic strengthening during quiet sleep
            consolidateWithFactor(params.consolidationFactor, params.stdpLearningRate);
            break;
        case SleepState::DeepSleep:
            // Systems consolidation during deep sleep
            consolidateWithFactor(params.consolidationFactor * 1.5f, params.stdpLearningRate * 0.5f);
            break;
        case SleepState::REMSleep:
            // Integration during REM sleep
            consolidateWithFactor(params.consolidationFactor * 1.2f, params.stdpLearningRate * 1.5f);
            break;
        case SleepState::Wake:
            // No consolidation during wake
            break;
    }
    
    consolidationCount_++;
    updatePlasticityStatistics(1.0f, true);
}

void SleepEnhancedPlasticity::reorganizeSynapticConnections(SleepState sleepState) {
    const auto& params = pImpl->stateParameters[sleepState];
    
    // Apply reorganization based on sleep state
    if (sleepState == SleepState::DeepSleep || sleepState == SleepState::REMSleep) {
        float reorganizationStrength = params.reorganizationFactor;
        
        // Reorganize synaptic connections
        reorganizeWithStrength(reorganizationStrength);
        reorganizationCount_++;
        updatePlasticityStatistics(reorganizationStrength, true);
    }
}

void SleepEnhancedPlasticity::strengthenMemoryConnections(const std::vector<float>& pattern, SleepState sleepState) {
    if (pattern.empty() || !brain_) return;
    
    const auto& params = pImpl->stateParameters[sleepState];
    
    // Find related synapses and strengthen them
    strengthenRelatedSynapses(pattern, params.consolidationFactor);
}

void SleepEnhancedPlasticity::completeMemoryPattern(const std::vector<float>& pattern, SleepState sleepState) {
    if (pattern.empty() || !brain_) return;
    
    const auto& params = pImpl->stateParameters[sleepState];
    
    // Complete incomplete patterns using sleep state characteristics
    completePattern(pattern, params.reconstructionFactor);
}

void SleepEnhancedPlasticity::integrateWithSTDP(STDP* stdp, SleepState sleepState) {
    if (!stdp) return;
    
    const auto& params = pImpl->stateParameters[sleepState];
    
    // Configure STDP with state-specific parameters
    float ltpWeight = params.stdpLearningRate * 10.0f;  // Scale factor
    float ltdWeight = params.stdpLearningRate * 10.5f;  // Slightly higher LTD
    float tau = 20.0f;  // Fixed time constant
    
    stdp->configure(ltpWeight, ltdWeight, tau);
}

void SleepEnhancedPlasticity::integrateWithHebbian(Hebbian* hebbian, SleepState sleepState) {
    if (!hebbian) return;
    
    const auto& params = pImpl->stateParameters[sleepState];
    
    // Configure Hebbian learning with state-specific parameters
    float learningRate = params.hebbianLearningRate * 10.0f;  // Scale factor
    float decayRate = 0.01f;  // Fixed decay
    
    hebbian->configure(learningRate, decayRate);
}

void SleepEnhancedPlasticity::integrateWithStructuralPlasticity(StructuralPlasticity* structuralPlasticity, SleepState sleepState) {
    if (!structuralPlasticity) return;
    
    // Integrate with structural plasticity based on sleep state
    if (sleepState == SleepState::DeepSleep) {
        // Enhanced structural changes during deep sleep
        structuralPlasticity->setSynaptogenesisRate(0.0001f * pImpl->stateParameters[sleepState].consolidationFactor);
        structuralPlasticity->setPruningRate(0.00001f * pImpl->stateParameters[sleepState].reorganizationFactor);
    }
}

void SleepEnhancedPlasticity::modulatePlasticityForState(SleepState sleepState) {
    currentSleepState_ = sleepState;
    
    // Apply state-specific modulation
    switch (sleepState) {
        case SleepState::QuietSleep:
            // Moderate plasticity enhancement
            synapticHomeostasisLevel_ = 0.9f;
            synapticScalingFactor_ = 1.05f;
            break;
        case SleepState::DeepSleep:
            // Strong plasticity suppression and downscaling
            synapticHomeostasisLevel_ = 0.5f;
            synapticScalingFactor_ = 0.8f;
            break;
        case SleepState::REMSleep:
            // High plasticity for integration
            synapticHomeostasisLevel_ = 1.2f;
            synapticScalingFactor_ = 1.1f;
            break;
        case SleepState::Wake:
            // Normal plasticity
            synapticHomeostasisLevel_ = 1.0f;
            synapticScalingFactor_ = 1.0f;
            break;
    }
}

float SleepEnhancedPlasticity::getStateSpecificSTDPLearningRate(SleepState sleepState) const {
    const auto& params = pImpl->stateParameters[sleepState];
    return params.stdpLearningRate;
}

float SleepEnhancedPlasticity::getStateSpecificHebbianLearningRate(SleepState sleepState) const {
    const auto& params = pImpl->stateParameters[sleepState];
    return params.hebbianLearningRate;
}

void SleepEnhancedPlasticity::applySynapticDownscaling(SleepState sleepState) {
    const auto& params = pImpl->stateParameters[sleepState];
    
    // Apply synaptic downscaling based on sleep state
    if (sleepState == SleepState::DeepSleep) {
        downscaleWeakConnections(sleepState);
    }
}

void SleepEnhancedPlasticity::downscaleWeakConnections(SleepState sleepState) {
    if (!brain_) return;
    
    float downscalingFactor = pImpl->stateParameters[sleepState].downscalingFactor;
    
    // Find and downscale weak connections
    downscaleConnections(downscalingFactor);
}

void SleepEnhancedPlasticity::integrateWithMemoryReplay(SleepState sleepState) {
    if (!brain_) return;
    
    const auto& params = pImpl->stateParameters[sleepState];
    
    if (params.enableReplayIntegration) {
        // Integrate with memory replay system
        replayIntegrationStrength_ = params.consolidationFactor;
        
        // Apply replayed activity to synaptic weights
        if (!lastReplayedPattern_.empty()) {
            applyReplayedActivityToSynapticWeights(lastReplayedPattern_, sleepState);
        }
    }
}

void SleepEnhancedPlasticity::applyReplayedActivityToSynapticWeights(const std::vector<float>& replayedPattern, SleepState sleepState) {
    if (!brain_ || replayedPattern.empty()) return;
    
    // Apply replayed pattern to synaptic weights
    // This integrates memory replay with synaptic plasticity
    
    const auto& params = pImpl->stateParameters[sleepState];
    
    // Find related neurons and adjust their synaptic weights
    for (float patternValue : replayedPattern) {
        // Adjust synaptic weights based on pattern value
        float weightChange = patternValue * params.consolidationFactor * replayIntegrationStrength_;
        totalSynapticWeightChange_ += weightChange;
    }
}

void SleepEnhancedPlasticity::applyStateSpecificSTDP(Synapse* synapse, const std::vector<Timestamp>& preSpikeTimes,
                                                    const std::vector<Timestamp>& postSpikeTimes, float learningRate) {
    if (!synapse) return;
    
    // Apply STDP with state-specific learning rate
    // This is a simplified implementation - real STDP would calculate precise weight changes
    float weightChange = learningRate * (postSpikeTimes.size() - preSpikeTimes.size());
    
    float currentWeight = synapse->getWeight();
    float newWeight = std::clamp(currentWeight + weightChange, 0.0f, 1.0f);
    
    synapse->setWeight(newWeight);
    totalSynapticWeightChange_ += std::abs(weightChange);
}

void SleepEnhancedPlasticity::applyStateSpecificHebbian(Synapse* synapse, const std::vector<Timestamp>& preSpikeTimes,
                                                       const std::vector<Timestamp>& postSpikeTimes, float learningRate) {
    if (!synapse) return;
    
    // Apply Hebbian learning with state-specific learning rate
    // Simplified implementation
    float weightChange = learningRate * preSpikeTimes.size() * postSpikeTimes.size();
    
    float currentWeight = synapse->getWeight();
    float newWeight = std::clamp(currentWeight + weightChange, 0.0f, 1.0f);
    
    synapse->setWeight(newWeight);
    totalSynapticWeightChange_ += std::abs(weightChange);
}

void SleepEnhancedPlasticity::consolidateWithFactor(float consolidationFactor, float learningRate) {
    if (!brain_) return;
    
    // Consolidate synaptic connections with given factor
    // This would iterate through all synapses and apply consolidation
    
    // Simulate consolidation by updating total weight change
    float consolidationEffect = consolidationFactor * learningRate * 100.0f;
    totalSynapticWeightChange_ += consolidationEffect;
}

void SleepEnhancedPlasticity::reorganizeWithStrength(float reorganizationStrength) {
    if (!brain_) return;
    
    // Reorganize synaptic connections with given strength
    // This would modify connection patterns and weights
    
    // Simulate reorganization
    float reorganizationEffect = reorganizationStrength * 50.0f;
    totalSynapticWeightChange_ += reorganizationEffect;
}

void SleepEnhancedPlasticity::strengthenRelatedSynapses(const std::vector<float>& pattern, float factor) {
    if (!brain_ || pattern.empty()) return;
    
    // Find and strengthen synapses related to the pattern
    // This would identify neurons that respond to the pattern
    
    // Simulate strengthening
    float strengtheningEffect = factor * pattern.size() * 10.0f;
    totalSynapticWeightChange_ += strengtheningEffect;
}

void SleepEnhancedPlasticity::completePattern(const std::vector<float>& pattern, float reconstructionFactor) {
    if (pattern.empty()) return;
    
    // Complete incomplete patterns using reconstruction factor
    // This would fill in missing activations in the pattern
    
    // Simulate pattern completion
    float completionEffect = reconstructionFactor * pattern.size() * 5.0f;
    totalSynapticWeightChange_ += completionEffect;
}

void SleepEnhancedPlasticity::downscaleConnections(float downscalingFactor) {
    if (!brain_) return;
    
    // Downscale weak synaptic connections
    // This would find and downscale connections with low weights
    
    // Simulate downscaling
    float downscalingEffect = downscalingFactor * 1000.0f;
    totalSynapticWeightChange_ -= downscalingEffect; // Negative effect
}

void SleepEnhancedPlasticity::handlePlasticityError(const std::string& errorMessage) {
    inErrorState_ = true;
    lastErrorMessage_ = errorMessage;
    consecutiveErrors_++;
    
    NLM_LOG_ERROR("SleepEnhancedPlasticity error: " + errorMessage);
    
    // Attempt recovery
    recoverFromPlasticityError();
}

void SleepEnhancedPlasticity::recoverFromPlasticityError() {
    // Attempt to recover from plasticity error
    if (consecutiveErrors_ > 0) {
        // Reduce learning rates on error
        for (auto& pair : pImpl->stateParameters) {
            pair.second.stdpLearningRate *= 0.5f;
            pair.second.hebbianLearningRate *= 0.5f;
        }
        
        // Reset state
        inErrorState_ = false;
        
        NLM_LOG_INFO("SleepEnhancedPlasticity recovered from error");
    }
}

void SleepEnhancedPlasticity::calculateSynapticHomeostasis() {
    // Calculate synaptic homeostasis level
    synapticHomeostasisLevel_ = std::clamp(synapticHomeostasisLevel_, 0.5f, 2.0f);
}

void SleepEnhancedPlasticity::adjustSynapticWeightsForHomeostasis() {
    if (!brain_) return;
    
    // Adjust synaptic weights for homeostasis
    // This would normalize synaptic weights across the network
    
    // Simulate homeostasis adjustment
    float homeostasisEffect = synapticHomeostasisLevel_ * 100.0f;
    totalSynapticWeightChange_ += homeostasisEffect;
}

void SleepEnhancedPlasticity::updatePlasticityStatistics(float weightChange, bool success) {
    totalSynapticWeightChange_ += weightChange;
}

std::string SleepEnhancedPlasticity::serialize() const {
    std::stringstream ss;
    
    // Serialize state parameters for each state
    ss << pImpl->stateParameters.size() << " ";
    for (const auto& pair : pImpl->stateParameters) {
        ss << static_cast<int>(pair.first) << " ";
        ss << pair.second.stdpLearningRate << " " << pair.second.hebbianLearningRate << " ";
        ss << pair.second.consolidationFactor << " " << pair.second.reorganizationFactor << " ";
        ss << pair.second.downscalingFactor << " " << pair.second.enableReplayIntegration << " ";
        ss << pair.second.neuromodulationSensitivity << " ";
    }
    
    // Serialize statistics
    ss << totalSynapticWeightChange_ << " ";
    ss << consolidationCount_ << " " << reorganizationCount_ << " ";
    
    // Serialize current state
    ss << static_cast<int>(currentSleepState_) << " ";
    ss << synapticHomeostasisLevel_ << " " << synapticScalingFactor_ << " ";
    ss << replayIntegrationStrength_ << " ";
    
    return ss.str();
}

bool SleepEnhancedPlasticity::deserialize(const std::string& data) {
    std::stringstream ss(data);
    
    size_t paramCount;
    ss >> paramCount;
    
    // Clear existing parameters
    pImpl->stateParameters.clear();
    
    // Deserialize state parameters
    for (size_t i = 0; i < paramCount; ++i) {
        int stateInt;
        ss >> stateInt;
        SleepState state = static_cast<SleepState>(stateInt);
        
        StatePlasticityParameters params;
        ss >> params.stdpLearningRate >> params.hebbianLearningRate;
        ss >> params.consolidationFactor >> params.reorganizationFactor;
        ss >> params.downscalingFactor >> params.enableReplayIntegration;
        ss >> params.neuromulationSensitivity;
        
        pImpl->stateParameters[state] = params;
    }
    
    // Deserialize statistics
    ss >> totalSynapticWeightChange_;
    ss >> consolidationCount_ >> reorganizationCount_;
    
    // Deserialize current state
    int stateInt;
    ss >> stateInt;
    currentSleepState_ = static_cast<SleepState>(stateInt);
    
    ss >> synapticHomeostasisLevel_ >> synapticScalingFactor_;
    ss >> replayIntegrationStrength_;
    
    return !ss.fail();
}

} // namespace nlm
