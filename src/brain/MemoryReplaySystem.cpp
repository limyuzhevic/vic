#include "MemoryReplaySystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

struct MemoryReplaySystem::Impl {
    Brain* brain;
    
    // Replay parameters for each state
    std::map<SleepState, ReplayParameters> replayParameters;
    
    // Statistics
    size_t totalReplayCount;
    float averageReplayStrength;
    float replayEfficiency;
    
    // Current replay state
    SleepState currentReplayState;
    float currentReplayProgress;
    
    // Active replay sequences
    std::vector<ReplaySequence> activeReplaySequences;
    
    // Error handling
    bool inErrorState;
    std::string lastErrorMessage;
    size_t consecutiveErrors;
    
    // Replay timing
    SimulationStep nextReplayStep;
    float replayInterval;
    float replayStrength;
    
    Impl() : brain(nullptr)
        , totalReplayCount(0)
        , averageReplayStrength(0.0f)
        , replayEfficiency(0.0f)
        , currentReplayState(SleepState::Wake)
        , currentReplayProgress(0.0f)
        , inErrorState(false)
        , consecutiveErrors(0)
        , nextReplayStep(0)
        , replayInterval(1.0f)
        , replayStrength(0.0f) {
        
        // Initialize default replay parameters for each sleep state
        replayParameters[SleepState::QuietSleep] = {
            2.0f,    // replayInterval
            0.3f,    // replayStrength
            0.7f,    // reconstructionFactor
            0.2f,    // consolidationWeight
            3,       // tracesPerReplay
            true     // integrateWithWorkingMemory
        };
        
        replayParameters[SleepState::DeepSleep] = {
            3.0f,    // replayInterval
            0.5f,    // replayStrength
            0.8f,    // reconstructionFactor
            0.7f,    // consolidationWeight
            2,       // tracesPerReplay
            true     // integrateWithWorkingMemory
        };
        
        replayParameters[SleepState::REMSleep] = {
            0.5f,    // replayInterval
            0.8f,    // replayStrength
            0.9f,    // reconstructionFactor
            0.6f,    // consolidationWeight
            5,       // tracesPerReplay
            true     // integrateWithWorkingMemory
        };
        
        replayParameters[SleepState::Wake] = {
            0.0f,    // replayInterval (no replay)
            0.0f,    // replayStrength (no replay)
            0.0f,    // reconstructionFactor (no replay)
            0.0f,    // consolidationWeight (no replay)
            0,       // tracesPerReplay (no replay)
            false    // integrateWithWorkingMemory
        };
        
        // Initialize state-specific characteristics
        stateCharacteristics[SleepState::QuietSleep] = {
            0.5f,    // replayFrequency
            2.0f,    // replayDuration
            0.3f,    // replayOverlap
            0.7f,    // reconstructionAccuracy
            0.4f,    // consolidationEfficiency
            0.3f     // integrationDepth
        };
        
        stateCharacteristics[SleepState::DeepSleep] = {
            0.2f,    // replayFrequency
            5.0f,    // replayDuration
            0.5f,    // replayOverlap
            0.9f,    // reconstructionAccuracy
            0.8f,    // consolidationEfficiency
            0.5f     // integrationDepth
        };
        
        stateCharacteristics[SleepState::REMSleep] = {
            0.8f,    // replayFrequency
            3.0f,    // replayDuration
            0.7f,    // replayOverlap
            0.95f,   // reconstructionAccuracy
            0.7f,    // consolidationEfficiency
            0.9f     // integrationDepth
        };
        
        stateCharacteristics[SleepState::Wake] = {
            0.0f,    // replayFrequency
            0.0f,    // replayDuration
            0.0f,    // replayOverlap
            0.0f,    // reconstructionAccuracy
            0.0f,    // consolidationEfficiency
            0.0f     // integrationDepth
        };
    }
};

MemoryReplaySystem::MemoryReplaySystem()
    : pImpl(new Impl)
    , brain_(nullptr)
    , totalReplayCount_(0)
    , averageReplayStrength_(0.0f)
    , replayEfficiency_(0.0f)
    , currentReplayState_(SleepState::Wake)
    , currentReplayProgress_(0.0f)
    , inErrorState_(false)
    , consecutiveErrors_(0)
    , nextReplayStep_(0)
    , replayInterval_(1.0f)
    , replayStrength_(0.0f)
{
}

MemoryReplaySystem::~MemoryReplaySystem() = default;

void MemoryReplaySystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("MemoryReplaySystem initialized");
}

void MemoryReplaySystem::executeReplay(SimulationStep currentStep, SleepState sleepState) {
    if (!brain_) {
        return;
    }
    
    currentReplayState_ = sleepState;
    currentReplayProgress_ = 0.0f;
    
    // Check if it's time for replay
    if (currentStep >= nextReplayStep_) {
        executeStateSpecificReplay(sleepState);
        scheduleNextReplay(sleepState, currentStep);
    }
    
    // Update active replay sequences
    for (auto& sequence : activeReplaySequences_) {
        if (sequence.completed) {
            continue;
        }
        
        float progress = static_cast<float>(currentStep - sequence.startTime) / 
                        static_cast<float>(sequence.endTime - sequence.startTime);
        sequence.memoryIndices.clear();
        
        // Execute sequence based on state
        if (sequence.state == SleepState::QuietSleep) {
            executeQuietSleepReplay();
        } else if (sequence.state == SleepState::DeepSleep) {
            executeDeepSleepReplay();
        } else if (sequence.state == SleepState::REMSleep) {
            executeREMSleepReplay();
        }
        
        currentReplayProgress_ += progress / activeReplaySequences_.size();
    }
}

void MemoryReplaySystem::executeQuietSleepReplay() {
    if (!brain_) return;
    
    // Get working memory with sleep enhancement
    auto* workingMemory = brain_->getWorkingMemory();
    auto* episodicMemory = brain_->getEpisodicMemory();
    
    if (workingMemory && workingMemory->getActiveTraces() > 0) {
        // Replay working memory traces during quiet sleep
        replayWorkingMemory(SleepState::QuietSleep);
    }
    
    if (episodicMemory && episodicMemory->getEpisodeCount() > 0) {
        // Replay episodic memories during quiet sleep
        replayEpisodicMemory(SleepState::QuietSleep);
    }
    
    updateReplayStatistics(pImpl->replayParameters[SleepState::QuietSleep].replayStrength, true);
}

void MemoryReplaySystem::executeDeepSleepReplay() {
    if (!brain_) return;
    
    // Get working memory with sleep enhancement
    auto* workingMemory = brain_->getWorkingMemory();
    auto* episodicMemory = brain_->getEpisodicMemory();
    
    if (workingMemory && workingMemory->getActiveTraces() > 0) {
        // Replay working memory traces during deep sleep
        replayWorkingMemory(SleepState::DeepSleep);
    }
    
    if (episodicMemory && episodicMemory->getEpisodeCount() > 0) {
        // Replay episodic memories during deep sleep
        replayEpisodicMemory(SleepState::DeepSleep);
    }
    
    updateReplayStatistics(pImpl->replayParameters[SleepState::DeepSleep].replayStrength, true);
}

void MemoryReplaySystem::executeREMSleepReplay() {
    if (!brain_) return;
    
    // Get working memory with sleep enhancement
    auto* workingMemory = brain_->getWorkingMemory();
    auto* episodicMemory = brain_->getEpisodicMemory();
    
    if (workingMemory && workingMemory->getActiveTraces() > 0) {
        // Replay working memory traces during REM sleep
        replayWorkingMemory(SleepState::REMSleep);
    }
    
    if (episodicMemory && episodicMemory->getEpisodeCount() > 0) {
        // Replay episodic memories during REM sleep
        replayEpisodicMemory(SleepState::REMSleep);
    }
    
    updateReplayStatistics(pImpl->replayParameters[SleepState::REMSleep].replayStrength, true);
}

void MemoryReplaySystem::executeWakeReplay() {
    // No replay during wake state
    currentReplayState_ = SleepState::Wake;
    currentReplayProgress_ = 0.0f;
    updateReplayStatistics(0.0f, false);
}

void MemoryReplaySystem::replayWorkingMemory(SleepState sleepState) {
    if (!brain_) return;
    
    const auto& params = pImpl->replayParameters[sleepState];
    
    // Access working memory with sleep enhancement
    // Note: In a full implementation, this would access NeuralWorkingMemoryWithSleep
    auto* workingMemory = brain_->getWorkingMemory();
    
    if (!workingMemory) return;
    
    // Determine how many traces to replay based on sleep state
    size_t tracesToReplay = std::min(params.tracesPerReplay, workingMemory->getActiveTraces());
    
    // Replay traces with state-specific strength
    for (size_t i = 0; i < tracesToReplay; ++i) {
        // Replay memory trace with state-specific characteristics
        // This would integrate with NeuralWorkingMemoryWithSleep methods
        // For now, we'll simulate the replay process
        float replayStrength = params.replayStrength * params.reconstructionFactor;
        
        // Simulate memory replay
        if (params.integrateWithWorkingMemory) {
            // Integrate replay with working memory
            // This would call methods on NeuralWorkingMemoryWithSleep
        }
    }
}

void MemoryReplaySystem::replayEpisodicMemory(SleepState sleepState) {
    if (!brain_) return;
    
    const auto& params = pImpl->replayParameters[sleepState];
    
    // Access episodic memory with sleep enhancement
    // Note: In a full implementation, this would access NeuralEpisodicMemoryWithSleep
    auto* episodicMemory = brain_->getEpisodicMemory();
    
    if (!episodicMemory) return;
    
    // Determine how many episodes to replay based on sleep state
    size_t episodesToReplay = std::min(params.tracesPerReplay, episodicMemory->getEpisodeCount());
    
    // Replay episodes with state-specific strength
    for (size_t i = 0; i < episodesToReplay; ++i) {
        // Replay episode with state-specific characteristics
        // This would integrate with NeuralEpisodicMemoryWithSleep methods
        // For now, we'll simulate the replay process
        float replayStrength = params.replayStrength * params.consolidationWeight;
        
        // Simulate memory replay
        if (params.integrateWithWorkingMemory) {
            // Integrate replay with episodic memory
            // This would call methods on NeuralEpisodicMemoryWithSleep
        }
    }
}

void MemoryReplaySystem::scheduleReplay(SleepState sleepState, SimulationStep currentStep) {
    const auto& characteristics = pImpl->stateCharacteristics[sleepState];
    const auto& params = pImpl->replayParameters[sleepState];
    
    // Calculate next replay step based on sleep state characteristics
    if (sleepState == SleepState::Wake) {
        // No replay during wake
        nextReplayStep_ = currentStep + static_cast<SimulationStep>(params.replayInterval);
        return;
    }
    
    // Calculate replay interval based on characteristics
    float replayInterval = characteristics.replayFrequency > 0.0f ? 
                           1.0f / characteristics.replayFrequency : 
                           params.replayInterval;
    
    nextReplayStep_ = currentStep + static_cast<SimulationStep>(replayInterval);
    
    // Update replay parameters
    replayInterval_ = replayInterval;
    replayStrength_ = params.replayStrength;
}

void MemoryReplaySystem::updateReplayTiming(SleepState sleepState, float dt) {
    const auto& characteristics = pImpl->stateCharacteristics[sleepState];
    
    // Update replay timing based on state characteristics
    replayInterval_ += characteristics.replayFrequency * dt;
    
    // Update replay strength based on consolidation
    replayStrength_ = std::min(1.0f, replayStrength_ + characteristics.consolidationEfficiency * dt);
}

const MemoryReplaySystem::ReplayParameters& MemoryReplaySystem::getReplayParameters(SleepState state) const {
    auto it = pImpl->replayParameters.find(state);
    if (it != pImpl->replayParameters.end()) {
        return it->second;
    }
    // Return default parameters for unknown state
    static ReplayParameters defaultParams = {0.0f, 0.0f, 0.0f, 0.0f, 0, false};
    return defaultParams;
}

void MemoryReplaySystem::setReplayParameters(SleepState state, const ReplayParameters& params) {
    if (validateReplayParameters(params)) {
        pImpl->replayParameters[state] = params;
    }
}

void MemoryReplaySystem::startReplaySequence(SleepState state, SimulationStep startTime) {
    ReplaySequence sequence;
    sequence.state = state;
    sequence.startTime = startTime;
    sequence.endTime = startTime + static_cast<SimulationStep>(
        pImpl->replayParameters[state].replayInterval);
    sequence.strength = pImpl->replayParameters[state].replayStrength;
    sequence.memoryIndices.clear();
    sequence.completed = false;
    
    activeReplaySequences_.push_back(sequence);
}

void MemoryReplaySystem::completeReplaySequence(size_t sequenceId) {
    if (sequenceId < activeReplaySequences_.size()) {
        activeReplaySequences_[sequenceId].completed = true;
        totalReplayCount_++;
    }
}

void MemoryReplaySystem::pauseReplaySequence(size_t sequenceId) {
    if (sequenceId < activeReplaySequences_.size()) {
        // Pause the sequence
        activeReplaySequences_[sequenceId].completed = true;
    }
}

void MemoryReplaySystem::resumeReplaySequence(size_t sequenceId) {
    if (sequenceId < activeReplaySequences_.size()) {
        // Resume the sequence
        activeReplaySequences_[sequenceId].completed = false;
    }
}

void MemoryReplaySystem::executeStateSpecificReplay(SleepState state) {
    switch (state) {
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
            executeWakeReplay();
            break;
    }
}

void MemoryReplaySystem::integrateReplayWithWorkingMemory(SleepState state) {
    // Integrate replay with working memory system
    // This would call methods on NeuralWorkingMemoryWithSleep
    // For now, we'll simulate the integration
    if (brain_ && brain_->getWorkingMemory()) {
        // Simulate integration with working memory
    }
}

void MemoryReplaySystem::integrateReplayWithEpisodicMemory(SleepState state) {
    // Integrate replay with episodic memory system
    // This would call methods on NeuralEpisodicMemoryWithSleep
    // For now, we'll simulate the integration
    if (brain_ && brain_->getEpisodicMemory()) {
        // Simulate integration with episodic memory
    }
}

void MemoryReplaySystem::updateReplayStatistics(float strength, bool success) {
    if (success) {
        totalReplayCount_++;
        
        // Update average replay strength
        float oldAvg = averageReplayStrength_ * (totalReplayCount_ - 1);
        averageReplayStrength_ = (oldAvg + strength) / totalReplayCount_;
        
        // Calculate efficiency
        replayEfficiency_ = (averageReplayStrength_ * success) / (1.0f + consecutiveErrors_);
        
        // Reset error count on success
        consecutiveErrors_ = 0;
    } else {
        consecutiveErrors_++;
        
        // Calculate efficiency (penalize errors)
        replayEfficiency_ = std::max(0.0f, replayEfficiency_ - 0.1f * consecutiveErrors_);
    }
}

void MemoryReplaySystem::resetReplayState() {
    currentReplayState_ = SleepState::Wake;
    currentReplayProgress_ = 0.0f;
    activeReplaySequences_.clear();
    inErrorState_ = false;
    lastErrorMessage_.clear();
    consecutiveErrors_ = 0;
}

bool MemoryReplaySystem::validateReplayParameters(const ReplayParameters& params) const {
    // Validate replay parameters
    if (params.replayInterval < 0.0f) return false;
    if (params.replayStrength < 0.0f || params.replayStrength > 1.0f) return false;
    if (params.reconstructionFactor < 0.0f || params.reconstructionFactor > 1.0f) return false;
    if (params.consolidationWeight < 0.0f || params.consolidationWeight > 1.0f) return false;
    if (params.tracesPerReplay < 0) return false;
    
    return true;
}

void MemoryReplaySystem::scheduleNextReplay(SleepState state, SimulationStep currentStep) {
    // Schedule the next replay based on state characteristics
    if (state == SleepState::Wake) {
        nextReplayStep_ = currentStep + 1; // Check every step during wake
        return;
    }
    
    const auto& characteristics = pImpl->stateCharacteristics[state];
    const auto& params = pImpl->replayParameters[state];
    
    // Calculate interval based on replay frequency
    if (characteristics.replayFrequency > 0.0f) {
        nextReplayStep_ = currentStep + static_cast<SimulationStep>(1.0f / characteristics.replayFrequency);
    } else {
        nextReplayStep_ = currentStep + static_cast<SimulationStep>(params.replayInterval);
    }
}

void MemoryReplaySystem::integrateWithNeuromodulation(SleepState sleepState) {
    // Integrate replay with neuromodulation systems
    // This would set up callbacks with dopamine, novelty, curiosity systems
    if (brain_) {
        // Set up neuromodulation integration
        // In a full implementation, this would call neuromodulation system methods
    }
}

void MemoryReplaySystem::integrateWithPredictionSystem(SleepState sleepState) {
    // Integrate replay with prediction system
    // This would set up prediction error computation during replay
    if (brain_ && brain_->getPredictionSystem()) {
        // Set up prediction integration
    }
}

void MemoryReplaySystem::integrateWithPlanningSystem(SleepState sleepState) {
    // Integrate replay with planning system
    // This would use replayed memories for action planning
    if (brain_ && brain_->getPlanner()) {
        // Set up planning integration
    }
}

void MemoryReplaySystem::handleReplayError(const std::string& errorMessage) {
    inErrorState_ = true;
    lastErrorMessage_ = errorMessage;
    consecutiveErrors_++;
    
    NLM_LOG_ERROR("Memory replay error: " + errorMessage);
    
    // Attempt recovery
    recoverFromReplayError();
}

void MemoryReplaySystem::recoverFromReplayError() {
    // Attempt to recover from replay error
    if (consecutiveErrors_ > 0) {
        // Reduce replay parameters on error
        for (auto& pair : pImpl->replayParameters) {
            pair.second.replayStrength *= 0.5f;
            pair.second.reconstructionFactor *= 0.5f;
        }
        
        // Reset state
        inErrorState_ = false;
        
        NLM_LOG_INFO("Memory replay recovered from error");
    }
}

std::string MemoryReplaySystem::serialize() const {
    std::stringstream ss;
    
    // Serialize replay parameters for each state
    ss << pImpl->replayParameters.size() << " ";
    for (const auto& pair : pImpl->replayParameters) {
        ss << static_cast<int>(pair.first) << " ";
        ss << pair.second.replayInterval << " " << pair.second.replayStrength << " ";
        ss << pair.second.reconstructionFactor << " " << pair.second.consolidationWeight << " ";
        ss << pair.second.tracesPerReplay << " " << pair.second.integrateWithWorkingMemory << " ";
    }
    
    // Serialize statistics
    ss << totalReplayCount_ << " ";
    ss << averageReplayStrength_ << " " << replayEfficiency_ << " ";
    
    // Serialize current state
    ss << static_cast<int>(currentReplayState_) << " ";
    ss << currentReplayProgress_ << " ";
    
    // Serialize active sequences
    ss << activeReplaySequences_.size() << " ";
    for (const auto& sequence : activeReplaySequences_) {
        ss << static_cast<int>(sequence.state) << " ";
        ss << sequence.startTime << " " << sequence.endTime << " ";
        ss << sequence.strength << " ";
        ss << sequence.memoryIndices.size() << " ";
        for (size_t idx : sequence.memoryIndices) {
            ss << idx << " ";
        }
        ss << sequence.completed << " ";
    }
    
    // Serialize error state
    ss << inErrorState_ << " ";
    ss << consecutiveErrors_ << " ";
    
    return ss.str();
}

bool MemoryReplaySystem::deserialize(const std::string& data) {
    std::stringstream ss(data);
    
    size_t paramCount;
    ss >> paramCount;
    
    // Clear existing parameters
    pImpl->replayParameters.clear();
    
    // Deserialize replay parameters
    for (size_t i = 0; i < paramCount; ++i) {
        int stateInt;
        ss >> stateInt;
        SleepState state = static_cast<SleepState>(stateInt);
        
        ReplayParameters params;
        ss >> params.replayInterval >> params.replayStrength;
        ss >> params.reconstructionFactor >> params.consolidationWeight;
        ss >> params.tracesPerReplay >> params.integrateWithWorkingMemory;
        
        pImpl->replayParameters[state] = params;
    }
    
    // Deserialize statistics
    ss >> totalReplayCount_;
    ss >> averageReplayStrength_ >> replayEfficiency_;
    
    // Deserialize current state
    int stateInt;
    ss >> stateInt;
    currentReplayState_ = static_cast<SleepState>(stateInt);
    
    ss >> currentReplayProgress_;
    
    // Deserialize active sequences
    size_t sequenceCount;
    ss >> sequenceCount;
    
    activeReplaySequences_.clear();
    for (size_t i = 0; i < sequenceCount; ++i) {
        ReplaySequence sequence;
        
        ss >> stateInt;
        sequence.state = static_cast<SleepState>(stateInt);
        
        ss >> sequence.startTime >> sequence.endTime;
        ss >> sequence.strength;
        
        size_t memoryIndicesCount;
        ss >> memoryIndicesCount;
        sequence.memoryIndices.resize(memoryIndicesCount);
        for (size_t j = 0; j < memoryIndicesCount; ++j) {
            ss >> sequence.memoryIndices[j];
        }
        
        ss >> sequence.completed;
        
        activeReplaySequences_.push_back(sequence);
    }
    
    // Deserialize error state
    ss >> inErrorState_;
    ss >> consecutiveErrors_;
    
    return !ss.fail();
}

} // namespace nlm
