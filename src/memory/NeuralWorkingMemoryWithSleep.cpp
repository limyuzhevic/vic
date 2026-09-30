#include "NeuralWorkingMemoryWithSleep.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

struct NeuralWorkingMemoryWithSleep::Impl {
    Brain* brain;
    
    // Maintenance connections (recurrent)
    std::vector<std::pair<NeuronId, NeuronId>> maintenanceSynapses;
    
    // Memory trace ages
    std::vector<SimulationStep> traceAges;
    
    // Sleep state-specific parameters
    std::map<SleepState, float> stateDecayRates;
    std::map<SleepState, float> stateReplayStrengths;
    std::map<SleepState, float> stateCompletionFactors;
    
    // Sleep replay statistics
    float memoryStabilization;
    float replayStrength;
    float consolidationProgress;
    
    // Replay scheduling
    float nextReplayTime;
    float replayInterval;
    size_t replayCount;
    
    // Sleep phase tracking
    float quietSleepPhase;
    float deepSleepPhase;
    float remSleepPhase;
    
    Impl() : brain(nullptr)
        , memoryStabilization(0.0f)
        , replayStrength(0.0f)
        , consolidationProgress(0.0f)
        , nextReplayTime(0.0f)
        , replayInterval(1.0f)
        , replayCount(0)
        , quietSleepPhase(0.0f)
        , deepSleepPhase(0.0f)
        , remSleepPhase(0.0f) {
        
        // Initialize state-specific parameters
        stateDecayRates[SleepState::QuietSleep] = 0.05f;
        stateDecayRates[SleepState::DeepSleep] = 0.02f;
        stateDecayRates[SleepState::REMSleep] = 0.08f;
        stateDecayRates[SleepState::Wake] = 0.01f;
        
        stateReplayStrengths[SleepState::QuietSleep] = 0.3f;
        stateReplayStrengths[SleepState::DeepSleep] = 0.5f;
        stateReplayStrengths[SleepState::REMSleep] = 0.8f;
        stateReplayStrengths[SleepState::Wake] = 0.0f;
        
        stateCompletionFactors[SleepState::QuietSleep] = 0.4f;
        stateCompletionFactors[SleepState::DeepSleep] = 0.3f;
        stateCompletionFactors[SleepState::REMSleep] = 0.9f;
        stateCompletionFactors[SleepState::Wake] = 0.0f;
    }
};

NeuralWorkingMemoryWithSleep::NeuralWorkingMemoryWithSleep()
    : pImpl(new Impl)
    , brain_(nullptr)
    , capacity_(100)
    , decayRate_(0.01f)
    , currentSleepState_(SleepState::Wake)
    , memoryStabilization_(0.0f)
    , replayStrength_(0.0f)
    , consolidationProgress_(0.0f)
    , nextReplayTime_(0.0f)
    , replayInterval_(1.0f)
    , replayCount_(0)
{
}

NeuralWorkingMemoryWithSleep::~NeuralWorkingMemoryWithSleep() = default;

void NeuralWorkingMemoryWithSleep::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralWorkingMemoryWithSleep initialized");
}

void NeuralWorkingMemoryWithSleep::store(const std::vector<float>& pattern, float strength) {
    if (pattern.empty() || !brain_) return;
    
    // Find neurons to encode this pattern
    size_t neuronsNeeded = std::min(pattern.size(), memoryNeurons_.size());
    
    for (size_t i = 0; i < neuronsNeeded; ++i) {
        NeuronId neuron = memoryNeurons_[i % memoryNeurons_.size()];
        float activation = pattern[i] * strength;
        
        // Set neuron activation
        if (auto* n = brain_->getRegion(neuron.getId() / 1000)->getAllNeurons()) {
            for (auto* nn : *n) {
                if (nn->getId() == neuron) {
                    nn->injectCurrent(activation * 5.0f);
                    break;
                }
            }
        }
        
        // Update stored activation
        if (i < memoryActivations_.size()) {
            memoryActivations_[i] = activation;
        } else {
            memoryActivations_.push_back(activation);
            memoryTimestamps_.push_back(0);
            memoryNeurons_.push_back(neuron);
        }
    }
    
    // Create maintenance connections if needed
    for (size_t i = 1; i < memoryNeurons_.size(); ++i) {
        createRecurrentConnection(memoryNeurons_[i-1], memoryNeurons_[i], strength * 0.5f);
    }
}

void NeuralWorkingMemoryWithSleep::storeToNeuron(NeuronId neuron, float activation) {
    // Find or add this neuron to memory
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        memoryActivations_[idx] = activation;
        memoryTimestamps_[idx] = 0;
    } else if (memoryNeurons_.size() < capacity_) {
        memoryNeurons_.push_back(neuron);
        memoryActivations_.push_back(activation);
        memoryTimestamps_.push_back(0);
    }
    
    // Inject current to maintain activation
    if (brain_) {
        brain_->injectCurrent(neuron, activation * 5.0f);
    }
}

std::vector<float> NeuralWorkingMemoryWithSleep::retrieve() const {
    std::vector<float> result;
    result.reserve(memoryActivations_.size());
    
    for (float activation : memoryActivations_) {
        result.push_back(activation);
    }
    
    return result;
}

bool NeuralWorkingMemoryWithSleep::contains(NeuronId neuron) const {
    return std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron) != memoryNeurons_.end();
}

float NeuralWorkingMemoryWithSleep::getNeuronActivation(NeuronId neuron) const {
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        return memoryActivations_[idx];
    }
    return 0.0f;
}

void NeuralWorkingMemoryWithSleep::update(TimestepDuration dt, SleepState sleepState) {
    currentSleepState_ = sleepState;
    
    if (!brain_) return;
    
    // Update maintenance - reinforce active memory neurons
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        NeuronId neuron = memoryNeurons_[i];
        float activation = memoryActivations_[i];
        
        if (activation > 0.1f) {
            // Inject maintenance current
            brain_->injectCurrent(neuron, activation * 2.0f);
            
            // Age the trace
            memoryTimestamps_[i]++;
            
            // Check if trace is too old
            if (memoryTimestamps_[i] > 1000) {
                activation *= (1.0f - decayRate_);
            }
            
            memoryActivations_[i] = activation;
        }
    }
    
    // Apply state-specific decay
    applyStateSpecificDecay(sleepState, dt);
    
    // Execute sleep replay if appropriate
    executeSleepReplay();
    
    // Pattern completion during sleep
    patternCompletionDuringSleep(sleepState);
    
    // Decay weak traces
    decayWeakTraces();
    
    // Run competition to select winners
    runCompetition();
}

void NeuralWorkingMemoryWithSleep::clear() {
    memoryNeurons_.clear();
    memoryActivations_.clear();
    memoryTimestamps_.clear();
    pImpl->maintenanceSynapses.clear();
    replayCount_ = 0;
    consolidationProgress_ = 0.0f;
}

float NeuralWorkingMemoryWithSleep::getDecayRate(SleepState state) const {
    auto it = pImpl->stateDecayRates.find(state);
    if (it != pImpl->stateDecayRates.end()) {
        return it->second;
    }
    return decayRate_;
}

void NeuralWorkingMemoryWithSleep::setDecayRate(SleepState state, float rate) {
    pImpl->stateDecayRates[state] = rate;
}

void NeuralWorkingMemoryWithSleep::strengthenMemory(float factor) {
    for (auto& activation : memoryActivations_) {
        activation = std::min(1.0f, activation * factor);
    }
}

void NeuralWorkingMemoryWithSleep::runCompetition() {
    winners_.clear();
    
    if (memoryActivations_.empty()) return;
    
    // Find neurons with above-threshold activation
    float threshold = 0.3f;
    
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        if (memoryActivations_[i] >= threshold) {
            winners_.push_back(memoryNeurons_[i]);
        }
    }
    
    // Inhibitory competition - suppress non-winners
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        bool isWinner = std::find(winners_.begin(), winners_.end(), memoryNeurons_[i]) != winners_.end();
        
        if (!isWinner && brain_) {
            // Apply strong inhibition
            brain_->injectCurrent(memoryNeurons_[i], -memoryActivations_[i] * 3.0f);
        }
    }
}

bool NeuralWorkingMemoryWithSleep::isWinning(NeuronId neuron) const {
    return std::find(winners_.begin(), winners_.end(), neuron) != winners_.end();
}

float NeuralWorkingMemoryWithSleep::getMemoryActivity() const {
    if (memoryActivations_.empty()) return 0.0f;
    
    float total = 0.0f;
    for (float act : memoryActivations_) {
        total += act;
    }
    return total / memoryActivations_.size();
}

void NeuralWorkingMemoryWithSleep::executeSleepReplay() {
    float currentTime = static_cast<float>(pImpl->brain->getTotalSpikeCount());
    
    // Check if it's time for replay
    if (currentTime >= nextReplayTime_) {
        switch (currentSleepState_) {
            case SleepState::QuietSleep:
                executeQuietSleepReplay();
                break;
            case SleepState::DeepSleep:
                executeDeepSleepReplay();
                break;
            case SleepState::REMSleep:
                executeREMSleepReplay();
                break;
            case SleepState::Wake:
                // No replay during wake
                break;
        }
        
        // Schedule next replay
        nextReplayTime_ = currentTime + replayInterval_;
        replayCount_++;
        
        // Update statistics
        updateReplayStatistics();
    }
}

void NeuralWorkingMemoryWithSleep::executeQuietSleepReplay() {
    if (!brain_ || memoryNeurons_.empty()) return;
    
    // Moderate replay frequency - replay 20% of traces
    size_t tracesToReplay = std::max<size_t>(1, memoryNeurons_.size() * pImpl->stateReplayStrengths[SleepState::QuietSleep]);
    
    for (size_t i = 0; i < tracesToReplay; ++i) {
        size_t idx = i % memoryNeurons_.size();
        NeuronId neuron = memoryNeurons_[idx];
        float activation = memoryActivations_[idx];
        
        // Reconstruct pattern with quiet sleep characteristics
        float reconstructionStrength = pImpl->stateReplayStrengths[SleepState::QuietSleep];
        float completionFactor = pImpl->stateCompletionFactors[SleepState::QuietSleep];
        
        // Inject current to reactivate pattern
        brain_->injectCurrent(neuron, activation * 3.0f * reconstructionStrength * completionFactor);
    }
}

void NeuralWorkingMemoryWithSleep::executeDeepSleepReplay() {
    if (!brain_ || memoryNeurons_.empty()) return;
    
    // Low frequency replay - replay 30% of traces with higher strength
    size_t tracesToReplay = std::max<size_t>(1, memoryNeurons_.size() * pImpl->stateReplayStrengths[SleepState::DeepSleep]);
    
    for (size_t i = 0; i < tracesToReplay; ++i) {
        size_t idx = i % memoryNeurons_.size();
        NeuronId neuron = memoryNeurons_[idx];
        float activation = memoryActivations_[idx];
        
        // Reconstruct pattern with deep sleep characteristics (slow oscillations)
        float reconstructionStrength = pImpl->stateReplayStrengths[SleepState::DeepSleep];
        float completionFactor = pImpl->stateCompletionFactors[SleepState::DeepSleep];
        
        // Inject current with synaptic downscaling influence
        brain_->injectCurrent(neuron, activation * 3.0f * reconstructionStrength * completionFactor * 0.8f);
    }
}

void NeuralWorkingMemoryWithSleep::executeREMSleepReplay() {
    if (!brain_ || memoryNeurons_.empty()) return;
    
    // High frequency replay - replay 60% of traces with maximum strength
    size_t tracesToReplay = std::max<size_t>(1, memoryNeurons_.size() * pImpl->stateReplayStrengths[SleepState::REMSleep]);
    
    for (size_t i = 0; i < tracesToReplay; ++i) {
        size_t idx = i % memoryNeurons_.size();
        NeuronId neuron = memoryNeurons_[idx];
        float activation = memoryActivations_[idx];
        
        // Reconstruct pattern with REM sleep characteristics (high integration)
        float reconstructionStrength = pImpl->stateReplayStrengths[SleepState::REMSleep];
        float completionFactor = pImpl->stateCompletionFactors[SleepState::REMSleep];
        
        // Inject current for pattern integration
        brain_->injectCurrent(neuron, activation * 3.0f * reconstructionStrength * completionFactor * 1.2f);
    }
}

void NeuralWorkingMemoryWithSleep::consolidateDuringSleep(SleepState sleepState, float duration) {
    // Apply sleep-dependent consolidation
    float consolidationRate = pImpl->stateDecayRates[sleepState] * 0.5f;
    float consolidationDelta = consolidationRate * duration;
    
    // Consolidate important memories
    float importanceThreshold = 0.5f;
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        float& activation = memoryActivations_[i];
        float age = static_cast<float>(memoryTimestamps_[i]);
        
        if (activation > importanceThreshold) {
            // Strengthen important memories
            float boost = importanceThreshold * consolidationDelta * pImpl->stateReplayStrengths[sleepState];
            activation = std::min(1.0f, activation + boost);
            consolidationProgress_ += boost;
        } else if (activation < 0.1f) {
            // Decay weak memories (synaptic downscaling)
            float decay = consolidationDelta * 0.5f;
            activation *= (1.0f - decay);
        }
    }
}

void NeuralWorkingMemoryWithSleep::patternCompletionDuringSleep(SleepState sleepState) {
    if (!brain_ || memoryNeurons_.empty()) return;
    
    switch (sleepState) {
        case SleepState::QuietSleep:
            completePatternsQuietSleep();
            break;
        case SleepState::DeepSleep:
            completePatternsDeepSleep();
            break;
        case SleepState::REMSleep:
            completePatternsREMSleep();
            break;
        case SleepState::Wake:
            // No pattern completion during wake
            break;
    }
}

void NeuralWorkingMemoryWithSleep::completePatternsQuietSleep() {
    // Moderate pattern completion for initial consolidation
    float completionFactor = pImpl->stateCompletionFactors[SleepState::QuietSleep];
    
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        NeuronId neuron = memoryNeurons_[i];
        float currentActivation = memoryActivations_[i];
        
        // Complete pattern with quiet sleep characteristics
        float targetActivation = currentActivation * completionFactor;
        float delta = targetActivation - currentActivation;
        
        if (std::abs(delta) > 0.01f) {
            brain_->injectCurrent(neuron, delta * 2.0f);
            memoryActivations_[i] = targetActivation;
        }
    }
}

void NeuralWorkingMemoryWithSleep::completePatternsDeepSleep() {
    // Low-level pattern completion with synaptic downscaling
    float completionFactor = pImpl->stateCompletionFactors[SleepState::DeepSleep];
    
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        NeuronId neuron = memoryNeurons_[i];
        float currentActivation = memoryActivations_[i];
        
        // Complete pattern with deep sleep characteristics (slower, more stable)
        float targetActivation = currentActivation * completionFactor;
        float delta = targetActivation - currentActivation;
        
        if (std::abs(delta) > 0.01f) {
            brain_->injectCurrent(neuron, delta * 1.5f * 0.8f); // Downscaling factor
            memoryActivations_[i] = targetActivation;
        }
    }
}

void NeuralWorkingMemoryWithSleep::completePatternsREMSleep() {
    // High-level pattern completion with schema formation
    float completionFactor = pImpl->stateCompletionFactors[SleepState::REMSleep];
    
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        NeuronId neuron = memoryNeurons_[i];
        float currentActivation = memoryActivations_[i];
        
        // Complete pattern with REM sleep characteristics (high integration)
        float targetActivation = currentActivation * completionFactor;
        float delta = targetActivation - currentActivation;
        
        if (std::abs(delta) > 0.01f) {
            brain_->injectCurrent(neuron, delta * 2.0f * 1.2f); // Integration factor
            memoryActivations_[i] = targetActivation;
        }
    }
}

void NeuralWorkingMemoryWithSleep::integrateWithStateManagement(SleepState sleepState, float dt) {
    // Update sleep state management integration
    switch (sleepState) {
        case SleepState::QuietSleep:
            pImpl->quietSleepPhase += dt;
            break;
        case SleepState::DeepSleep:
            pImpl->deepSleepPhase += dt;
            break;
        case SleepState::REMSleep:
            pImpl->remSleepPhase += dt;
            break;
        case SleepState::Wake:
            // Reset phases when waking up
            pImpl->quietSleepPhase = 0.0f;
            pImpl->deepSleepPhase = 0.0f;
            pImpl->remSleepPhase = 0.0f;
            break;
    }
}

void NeuralWorkingMemoryWithSleep::handleStateTransition(SleepState oldState, SleepState newState) {
    // Handle memory reorganization during state transitions
    if (oldState != newState) {
        NLM_LOG_INFO("NeuralWorkingMemoryWithSleep: State transition from " +
                     std::to_string(static_cast<int>(oldState)) + " to " +
                     std::to_string(static_cast<int>(newState)));
        
        // Reorganization during state transitions
        for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
            float& activation = memoryActivations_[i];
            
            // Smooth transition between states
            float oldDecay = pImpl->stateDecayRates[oldState];
            float newDecay = pImpl->stateDecayRates[newState];
            
            // Adjust activation based on state change
            if (newDecay > oldDecay) {
                // More decay in new state - reduce activation
                activation *= (1.0f - (newDecay - oldDecay) * 0.5f);
            } else {
                // Less decay in new state - maintain activation
                activation = std::min(1.0f, activation / (1.0f - (oldDecay - newDecay) * 0.5f));
            }
        }
    }
}

float NeuralWorkingMemoryWithSleep::calculateReplayProbability(SleepState state, float memoryAge) const {
    // Calculate probability of replay based on sleep state and memory age
    float baseProbability = pImpl->stateReplayStrengths[state];
    
    // Older memories are more likely to be replayed
    float ageFactor = std::min(1.0f, memoryAge / 1000.0f);
    
    return baseProbability * (1.0f + ageFactor * 0.5f);
}

float NeuralWorkingMemoryWithSleep::getStateSpecificReconstructionStrength(SleepState state) const {
    return pImpl->stateReplayStrengths[state];
}

bool NeuralWorkingMemoryWithSleep::shouldReplayDuringState(SleepState state, float memoryImportance) const {
    if (state == SleepState::Wake) return false;
    
    float replayThreshold = 0.3f;
    return (pImpl->stateReplayStrengths[state] > replayThreshold && memoryImportance > 0.5f);
}

void NeuralWorkingMemoryWithSleep::updateReplayStatistics() {
    // Update replay-related statistics
    if (!memoryNeurons_.empty()) {
        float totalImportance = 0.0f;
        for (float activation : memoryActivations_) {
            totalImportance += activation;
        }
        memoryStabilization_ = totalImportance / memoryNeurons_.size();
    }
    
    replayStrength_ = pImpl->stateReplayStrengths[currentSleepState_];
}

void NeuralWorkingMemoryWithSleep::createRecurrentConnection(NeuronId from, NeuronId to, float strength) {
    // Check if connection already exists
    for (const auto& conn : pImpl->maintenanceSynapses) {
        if (conn.first == from && conn.second == to) return;
    }
    
    pImpl->maintenanceSynapses.emplace_back(from, to);
}

void NeuralWorkingMemoryWithSleep::updateRecurrentConnections() {
    // Apply maintenance currents through recurrent connections
    for (const auto& conn : pImpl->maintenanceSynapses) {
        float fromActivation = 0.0f;
        
        // Find from neuron activation
        auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), conn.first);
        if (it != memoryNeurons_.end()) {
            size_t idx = std::distance(memoryNeurons_.begin(), it);
            fromActivation = memoryActivations_[idx];
        }
        
        if (fromActivation > 0.1f && brain_) {
            // Send maintenance signal
            brain_->injectCurrent(conn.second, fromActivation * 2.0f);
        }
    }
}

void NeuralWorkingMemoryWithSleep::decayWeakTraces() {
    std::vector<size_t> toRemove;
    
    for (size_t i = 0; i < memoryActivations_.size(); ++i) {
        memoryActivations_[i] *= (1.0f - decayRate_);
        
        if (memoryActivations_[i] < 0.01f) {
            toRemove.push_back(i);
        }
    }
    
    // Remove weak traces (in reverse order to maintain indices)
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        memoryNeurons_.erase(memoryNeurons_.begin() + *it);
        memoryActivations_.erase(memoryActivations_.begin() + *it);
        memoryTimestamps_.erase(memoryTimestamps_.begin() + *it);
    }
}

void NeuralWorkingMemoryWithSleep::applyStateSpecificDecay(SleepState state, float dt) {
    float stateDecayRate = pImpl->stateDecayRates[state];
    
    // Apply state-specific decay
    for (size_t i = 0; i < memoryActivations_.size(); ++i) {
        float& activation = memoryActivations_[i];
        
        // Different decay rates for different states
        float decay = stateDecayRate * dt;
        activation *= (1.0f - decay);
        
        // State-specific decay effects
        switch (state) {
            case SleepState::QuietSleep:
                // Moderate decay with synaptic homeostasis
                break;
            case SleepState::DeepSleep:
                // Synaptic downscaling during deep sleep
                if (activation > 0.3f && activation < 0.7f) {
                    activation *= 0.95f; // Stronger downscaling for medium-strength traces
                }
                break;
            case SleepState::REMSleep:
                // Integration during REM - maintain but reorganize
                break;
            case SleepState::Wake:
                // Normal decay during wake
                break;
        }
    }
}

std::string NeuralWorkingMemoryWithSleep::serialize() const {
    std::stringstream ss;
    ss << memoryNeurons_.size() << " ";
    ss << memoryActivations_.size() << " ";
    ss << memoryTimestamps_.size() << " ";
    
    // Serialize memory neurons
    for (const auto& neuron : memoryNeurons_) {
        ss << neuron.value << " ";
    }
    
    // Serialize activations
    for (float activation : memoryActivations_) {
        ss << activation << " ";
    }
    
    // Serialize timestamps
    for (const auto& timestamp : memoryTimestamps_) {
        ss << timestamp << " ";
    }
    
    // Serialize sleep state
    ss << static_cast<int>(currentSleepState_) << " ";
    
    // Serialize replay statistics
    ss << memoryStabilization_ << " " << replayStrength_ << " " << consolidationProgress_ << " ";
    ss << nextReplayTime_ << " " << replayInterval_ << " " << replayCount_ << " ";
    
    // Serialize state-specific parameters
    ss << pImpl->quietSleepPhase << " " << pImpl->deepSleepPhase << " " << pImpl->remSleepPhase << " ";
    
    return ss.str();
}

bool NeuralWorkingMemoryWithSleep::deserialize(const std::string& data) {
    std::stringstream ss(data);
    
    size_t neuronCount, activationCount, timestampCount;
    ss >> neuronCount >> activationCount >> timestampCount;
    
    // Clear existing data
    memoryNeurons_.clear();
    memoryActivations_.clear();
    memoryTimestamps_.clear();
    
    // Deserialize memory neurons
    memoryNeurons_.resize(neuronCount);
    for (size_t i = 0; i < neuronCount; ++i) {
        ss >> memoryNeurons_[i].value;
    }
    
    // Deserialize activations
    memoryActivations_.resize(activationCount);
    for (size_t i = 0; i < activationCount; ++i) {
        ss >> memoryActivations_[i];
    }
    
    // Deserialize timestamps
    memoryTimestamps_.resize(timestampCount);
    for (size_t i = 0; i < timestampCount; ++i) {
        ss >> memoryTimestamps_[i];
    }
    
    // Deserialize sleep state
    int stateInt;
    ss >> stateInt;
    currentSleepState_ = static_cast<SleepState>(stateInt);
    
    // Deserialize replay statistics
    ss >> memoryStabilization_ >> replayStrength_ >> consolidationProgress_;
    ss >> nextReplayTime_ >> replayInterval_ >> replayCount_;
    
    // Deserialize sleep phase tracking
    ss >> pImpl->quietSleepPhase >> pImpl->deepSleepPhase >> pImpl->remSleepPhase;
    
    return !ss.fail();
}

} // namespace nlm
