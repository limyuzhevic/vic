#include "SleepConsolidationSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <unordered_set>

namespace nlm {

struct SleepConsolidationSystem::Impl {
    std::mt19937 rng;
    Brain* brain;
    
    // Sleep timing state
    float timeInCurrentState;
    float stateTransitionTimer;
    float sleepCycleTimer;
    float homeostasisAccumulator;
    
    // Replay state
    float replayCooldown;
    float spontaneousReplayProbability;
    std::vector<SimulationStep> recentlyReplayedEpisodes;
    
    // Statistics
    size_t replayAttempts;
    size_t successfulConsolidations;
    size_t homeostaticAdjustments;
    float totalSynapticDownscaling;
};

SleepConsolidationSystem::SleepConsolidationSystem()
    : pImpl(std::make_unique<Impl>())
    , brain_(nullptr)
    , sleepDuration_(30000.0f)  // 30 seconds in simulation steps
    , lightSleepDuration_(15000.0f)
    , deepSleepDuration_(10000.0f)
    , remSleepDuration_(5000.0f)
    , downscalingFactor_(0.8f)
    , pruningThreshold_(0.05f)
    , replayStrengthMultiplier_(1.5f)
    , homeostasisGain_(0.1f)
    , totalSleepCycles_(0)
    , successfulReplays_(0)
    , synapticPrunings_(0)
    , averageHomeostasisScore_(1.0f)
{
    // Initialize random number generator
    std::random_device rd;
    pImpl->rng.seed(rd());
}

SleepConsolidationSystem::~SleepConsolidationSystem() = default;

void SleepConsolidationSystem::initialize(Brain* brain) {
    brain_ = brain;
    pImpl->brain = brain;
    
    // Reset to awake state
    sleepState_.currentState = SleepState::State::Awake;
    sleepState_.sleepStartTime = 0.0;
    sleepState_.wakeUpTime = 0.0;
    
    // Reset sleep timing
    pImpl->timeInCurrentState = 0.0f;
    pImpl->stateTransitionTimer = 0.0f;
    pImpl->sleepCycleTimer = 0.0f;
    pImpl->homeostasisAccumulator = 0.0f;
    
    // Reset replay state
    pImpl->replayCooldown = 0.0f;
    pImpl->spontaneousReplayProbability = 0.3f;
    pImpl->recentlyReplayedEpisodes.clear();
    pImpl->recentlyReplayedEpisodes.reserve(50);
    
    // Reset statistics
    pImpl->replayAttempts = 0;
    pImpl->successfulConsolidations = 0;
    pImpl->homeostaticAdjustments = 0;
    pImpl->totalSynapticDownscaling = 0.0f;
    
    NLM_LOG_INFO("SleepConsolidationSystem initialized");
}

void SleepConsolidationSystem::update(TimestepDuration dt, const NeuromodulatorySleepControl& neuromodulators) {
    neuromodulators_ = neuromodulators;
    
    // Update sleep state based on neuromodulators
    updateSleepState(dt);
    
    // Update sleep state timer
    pImpl->timeInCurrentState += dt;
    pImpl->sleepCycleTimer += dt;
    
    // Generate downscaling targets during sleep
    if (sleepState_.currentState != SleepState::State::Awake) {
        generateDownscalingTargets();
        calculateMemoryPriorities();
    }
    
    // Trigger spontaneous replay during rest states
    if (sleepState_.currentState == SleepState::State::LightSleep && 
        pImpl->replayCooldown <= 0.0f) {
        triggerReplay();
    }
    
    // Apply homeostatic regulation based on neural activity
    if (brain_ && sleepState_.currentState == SleepState::State::DeepSleep) {
        size_t totalActive = 0;
        float avgFiringRate = 0.0f;
        
        for (const auto& region : brain_->getRegions()) {
            totalActive += region->getActiveNeuronCount();
        }
        
        if (brain_->getTotalNeuronCount() > 0) {
            avgFiringRate = static_cast<float>(totalActive) / brain_->getTotalNeuronCount();
        }
        
        updateHomeostasis(std::vector<NeuronId>(), avgFiringRate);
    }
}

void SleepConsolidationSystem::enterSleep(SleepState::State state, float depth) {
    if (sleepState_.currentState == SleepState::State::Awake) {
        sleepState_.sleepStartTime = pImpl->brain ? pImpl->brain->getCurrentTime() : 0.0;
    }
    
    // Handle state transitions
    if (sleepState_.currentState != state) {
        sleepState_.currentState = state;
        pImpl->timeInCurrentState = 0.0f;
        pImpl->stateTransitionTimer = 0.0f;
        
        // Set state-specific parameters
        switch (state) {
            case SleepState::State::LightSleep:
                sleepState_.slowWaveActivity = depth * 0.3f;
                sleepState_.spindleDensity = depth * 2.0f;
                break;
            case SleepState::State::DeepSleep:
                sleepState_.slowWaveActivity = depth * 1.5f;
                sleepState_.spindleDensity = depth * 0.5f;
                break;
            case SleepState::State::REM:
                sleepState_.thetaPower = depth * 2.0f;
                sleepState_.slowWaveActivity = depth * 0.1f;
                break;
            default:
                break;
        }
    }
    
    sleepState_.sleepDepth = depth;
    NLM_LOG_INFO("Entering sleep state: " + std::to_string(static_cast<int>(state)) + " with depth " + std::to_string(depth));
}

void SleepConsolidationSystem::exitSleep() {
    if (sleepState_.currentState != SleepState::State::Awake) {
        sleepState_.wakeUpTime = pImpl->brain ? pImpl->brain->getCurrentTime() : 0.0;
        sleepState_.currentState = SleepState::State::Awake;
        pImpl->timeInCurrentState = 0.0f;
        pImpl->sleepCycleTimer = 0.0f;
        
        NLM_LOG_INFO("Woke up from sleep");
    }
}

void SleepConsolidationSystem::triggerReplay() {
    if (!brain_ || !brain_->getEpisodicMemory()) {
        return;
    }
    
    pImpl->replayAttempts++;
    
    // Get episodes to replay based on priority
    size_t targetReplays = std::min<size_t>(3, brain_->getEpisodicMemory()->getEpisodeCount() / 10);
    auto episodes = brain_->getEpisodicMemory()->getEpisodesForReplay(targetReplays);
    
    for (size_t i = 0; i < std::min(targetReplays, episodes.size()); ++i) {
        // Create replay event
        MemoryReplayEvent event;
        event.timestamp = pImpl->brain ? pImpl->brain->getCurrentStep() : 0;
        event.episodeId = i;
        event.replayStrength = replayStrengthMultiplier_ * (0.5f + pImpl->rng() / static_cast<float>(pImpl->rng.max()) * 0.5f);
        event.replayDuration = 100.0f + event.replayStrength * 100.0f;
        event.synapticWeightChange = 0.0f;
        
        // Get episode details for replay
        auto* episode = episodes[i];
        if (episode) {
            event.reactivatedNeurons = episode->activeNeurons;
            event.synapticWeightChange = event.replayStrength * 0.01f;
            
            // Trigger replay in episodic memory system
            brain_->getEpisodicMemory()->replayEpisode(episode);
            
            // Record successful replay
            pImpl->recentlyReplayedEpisodes.push_back(event.episodeId);
            if (pImpl->recentlyReplayedEpisodes.size() > 50) {
                pImpl->recentlyReplayedEpisodes.erase(pImpl->recentlyReplayedEpisodes.begin());
            }
            
            successfulReplays_++;
        }
        
        replayEvents_.push_back(event);
    }
    
    // Set cooldown to prevent excessive replay
    pImpl->replayCooldown = 1000.0f;
}

void SleepConsolidationSystem::applySynapticDownscaling(const std::vector<SynapticDownscalingTarget>& targets) {
    downscalingTargets_ = targets;
    
    float totalDownscaling = 0.0f;
    for (const auto& target : targets) {
        // Apply downscaling based on homeostatic error
        float weightChange = (target.targetWeight - target.currentWeight) * downscalingFactor_;
        
        // Clamp weight change to reasonable range
        weightChange = std::max(-0.1f, std::min(0.1f, weightChange));
        
        // Update synaptic weight in brain
        if (brain_) {
            for (auto& region : brain_->getRegions()) {
                for (auto* neuron : region->getAllNeurons()) {
                    if (neuron->getId() == target.neuronId) {
                        float newWeight = neuron->getState().threshold + weightChange;
                        neuron->setThreshold(newWeight);
                        break;
                    }
                }
            }
        }
        
        totalDownscaling += std::abs(weightChange);
        
        // Update homeostatic score
        sleepState_.homeostasisScore = (sleepState_.homeostasisScore * 0.9f) + (target.downscalingFactor * 0.1f);
    }
    
    pImpl->totalSynapticDownscaling += totalDownscaling;
    sleepState_.synapticDownscalingLevel = downscalingFactor_;
    
    NLM_LOG_INFO("Applied synaptic downscaling with total change: " + std::to_string(totalDownscaling));
}

void SleepConsolidationSystem::pruneWeakMemories(float threshold) {
    if (!brain_ || !brain_->getEpisodicMemory()) {
        return;
    }
    
    // This is a simplified implementation - in reality would be more complex
    // For now, just track that pruning occurred
    synapticPrunings_++;
    
    // Update pruning threshold
    pruningThreshold_ = threshold;
    
    // In a full implementation, would:
    // 1. Identify weak memory traces
    // 2. Remove them from episodic memory
    // 3. Update associative memory connections
    // 4. Update working memory capacity
    
    NLM_LOG_INFO("Performed memory pruning with threshold: " + std::to_string(threshold));
}

void SleepConsolidationSystem::consolidateMemories(const std::vector<MemoryPriorityEntry>& priorities) {
    memoryPriorities_ = priorities;
    
    successfulConsolidations++;
    
    // Update memory prioritization index
    float prioritySum = 0.0f;
    for (const auto& priority : priorities) {
        prioritySum += priority.consolidationNeed;
    }
    
    if (!priorities.empty()) {
        sleepState_.memoryPrioritizationIndex = prioritySum / priorities.size();
    }
    
    NLM_LOG_INFO("Consolidated " + std::to_string(priorities.size()) + " memories with average priority " + 
                 std::to_string(sleepState_.memoryPrioritizationIndex));
}

void SleepConsolidationSystem::updateHomeostasis(const std::vector<NeuronId>& activeNeurons, float averageFiringRate) {
    // Calculate homeostatic error
    float firingError = std::abs(averageFiringRate - homeostasis_.firingRateTarget);
    
    // Update homeostatic regulation parameters
    float learningRate = homeostasisGain_ * (1.0f - sleepState_.homeostasisScore);
    
    // Adjust synaptic excitability
    homeostasis_.synapticExcitability = std::clamp(
        homeostasis_.synapticExcitability + learningRate * firingError * 0.1f,
        0.5f, 2.0f
    );
    
    // Adjust intrinsic conductance
    homeostasis_.intrinsicConductance = std::clamp(
        homeostasis_.intrinsicConductance + learningRate * firingError * 0.05f,
        0.8f, 1.5f
    );
    
    // Update inhibition balance
    homeostasis_.inhibitionBalance = std::clamp(
        homeostasis_.inhibitionBalance + learningRate * firingError * 0.05f,
        0.7f, 1.3f
    );
    
    // Update metabolic load (increases with high activity)
    homeostasis_.metabolicLoad = std::clamp(
        homeostasis_.metabolicLoad + learningRate * firingError * 0.02f,
        0.3f, 1.0f
    );
    
    // Update homeostasis score
    sleepState_.homeostasisScore = (sleepState_.homeostasisScore * 0.9f) + (1.0f - firingError) * 0.1f;
    
    pImpl->homeostaticAdjustments++;
    averageHomeostasisScore_ = (averageHomeostasisScore_ * 0.95f) + sleepState_.homeostasisScore * 0.05f;
    
    NLM_LOG_INFO("Updated homeostasis: error " + std::to_string(firingError) + 
                 ", score " + std::to_string(sleepState_.homeostasisScore));
}

void SleepConsolidationSystem::forceConsolidationCycle(TimestepDuration dt) {
    // Force a complete consolidation cycle
    // 1. Enter deep sleep phase
    enterSleep(SleepState::State::DeepSleep, 0.9f);
    
    // Generate downscaling targets
    generateDownscalingTargets();
    
    // Apply synaptic downscaling
    if (!downscalingTargets_.empty()) {
        applySynapticDownscaling(downscalingTargets_);
    }
    
    // Trigger replay
    triggerReplay();
    
    // Update homeostasis
    updateHomeostasis(std::vector<NeuronId>(), 0.0f);
    
    // Exit sleep
    exitSleep();
    
    totalSleepCycles_++;
    
    NLM_LOG_INFO("Completed forced consolidation cycle " + std::to_string(totalSleepCycles_));
}

void SleepConsolidationSystem::reset() {
    enterSleep(SleepState::State::DeepSleep, 0.0f);
    
    // Clear all events and targets
    replayEvents_.clear();
    downscalingTargets_.clear();
    memoryPriorities_.clear();
    
    // Reset statistics
    totalSleepCycles_ = 0;
    successfulReplays_ = 0;
    synapticPrunings_ = 0;
    averageHomeostasisScore_ = 1.0f;
    
    NLM_LOG_INFO("SleepConsolidationSystem reset");
}

void SleepConsolidationSystem::updateSleepState(TimestepDuration dt) {
    // Update neuromodulatory sleep control
    updateNeuromodulatoryControl();
    
    // Update state transition timer
    pImpl->stateTransitionTimer += dt;
    
    // Update sleep cycle timer
    pImpl->sleepCycleTimer += dt;
    
    // Update homeostatic accumulator
    pImpl->homeostasisAccumulator += dt;
    
    // Handle sleep state transitions based on neuromodulators
    if (sleepState_.currentState == SleepState::State::Awake) {
        // Check for sleep onset based on neuromodulators
        float sleepReadiness = neuromodulators_.melatonin * 0.3f + 
                               neuromodulators_.serotonin * 0.2f - 
                               neuromodulators_.orexin * 0.5f;
        
        if (sleepReadiness > 0.5f && pImpl->sleepCycleTimer > 5000.0f) {
            // Transition to light sleep
            enterSleep(SleepState::State::LightSleep, neuromodulators_.melatonin);
            pImpl->sleepCycleTimer = 0.0f;
        }
    } else if (sleepState_.currentState == SleepState::State::LightSleep) {
        // Check for transition to deep sleep
        if (pImpl->timeInCurrentState > lightSleepDuration_ * 0.7f) {
            enterSleep(SleepState::State::DeepSleep, 0.8f);
        } else if (pImpl->timeInCurrentState > lightSleepDuration_) {
            // Random transition to REM or wake up
            std::uniform_real_distribution<float> dist(0.0f, 1.0f);
            if (dist(pImpl->rng) < 0.3f) {
                enterSleep(SleepState::State::REM, 0.6f);
            } else {
                exitSleep();
            }
        }
    } else if (sleepState_.currentState == SleepState::State::DeepSleep) {
        // Check for transition to REM or wake up
        if (pImpl->timeInCurrentState > deepSleepDuration_ * 0.8f) {
            std::uniform_real_distribution<float> dist(0.0f, 1.0f);
            if (dist(pImpl->rng) < 0.4f) {
                enterSleep(SleepState::State::REM, 0.7f);
            } else {
                exitSleep();
            }
        }
    } else if (sleepState_.currentState == SleepState::State::REM) {
        // Check for wake up
        if (pImpl->timeInCurrentState > remSleepDuration_ * 0.9f) {
            exitSleep();
        }
    }
    
    // Update replay cooldown
    if (pImpl->replayCooldown > 0.0f) {
        pImpl->replayCooldown -= dt;
    }
}

void SleepConsolidationSystem::generateDownscalingTargets() {
    downscalingTargets_.clear();
    
    if (!brain_) {
        return;
    }
    
    // Get all neurons from brain
    std::vector<NeuronId> neuronIds;
    for (const auto& region : brain_->getRegions()) {
        for (const auto* neuron : region->getNeurons()) {
            neuronIds.push_back(neuron->getId());
        }
    }
    
    if (neuronIds.empty()) {
        return;
    }
    
    // Select neurons for downscaling based on their activity
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    size_t targetCount = std::min<size_t>(10, neuronIds.size() / 100);  // 1% of neurons
    
    for (size_t i = 0; i < targetCount; ++i) {
        // Randomly select neuron
        size_t idx = static_cast<size_t>(dist(pImpl->rng) * neuronIds.size());
        NeuronId neuronId = neuronIds[idx];
        
        // Check if neuron already has a target
        bool alreadyHasTarget = false;
        for (const auto& target : downscalingTargets_) {
            if (target.neuronId == neuronId) {
                alreadyHasTarget = true;
                break;
            }
        }
        
        if (!alreadyHasTarget) {
            // Create downscaling target
            SynapticDownscalingTarget target;
            target.neuronId = neuronId;
            target.currentWeight = dist(pImpl->rng) * 2.0f - 1.0f;  // Weight between -1 and 1
            target.targetWeight = std::clamp(target.currentWeight * downscalingFactor_, -0.5f, 0.5f);
            target.downscalingFactor = dist(pImpl->rng) * 0.5f + 0.5f;  // Between 0.5 and 1.0
            target.homeostaticError = dist(pImpl->rng) * 0.5f;
            
            downscalingTargets_.push_back(target);
        }
    }
}

void SleepConsolidationSystem::calculateMemoryPriorities() {
    memoryPriorities_.clear();
    
    if (!brain_ || !brain_->getEpisodicMemory()) {
        return;
    }
    
    // Get recent episodes
    auto recentEpisodes = brain_->getEpisodicMemory()->getRecentEpisodes(20);
    
    for (size_t i = 0; i < recentEpisodes.size(); ++i) {
        MemoryPriorityEntry priority;
        priority.episodeId = i;
        
        // Calculate replay frequency (simplified)
        priority.replayFrequency = (i + 1) / (i + 1 + static_cast<float>(pImpl->recentlyReplayedEpisodes.size()));
        
        // Use episode age as novelty score
        priority.temporalRecency = 1.0f / (1.0f + static_cast<float>(i));
        
        // Random factors for other priority components (simplified)
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        priority.noveltyScore = dist(pImpl->rng);
        priority.rewardAssociation = dist(pImpl->rng) * 0.5f;
        priority.predictionError = dist(pImpl->rng) * 0.5f;
        priority.emotionalValence = dist(pImpl->rng) * 0.5f;
        
        // Calculate consolidation need (weighted sum of factors)
        priority.consolidationNeed = 
            priority.replayFrequency * 0.2f +
            priority.noveltyScore * 0.2f +
            priority.rewardAssociation * 0.2f +
            priority.predictionError * 0.2f +
            priority.temporalRecency * 0.1f +
            priority.emotionalValence * 0.1f;
        
        memoryPriorities_.push_back(priority);
    }
}

void SleepConsolidationSystem::scheduleReplayEvents() {
    // This is called periodically to schedule replay events
    // For now, we trigger replay directly in the update loop
}

void SleepConsolidationSystem::applyHomeostaticPlasticity() {
    // Apply homeostatic plasticity rules
    // This modifies synaptic strengths based on homeostatic targets
    if (brain_) {
        for (auto& region : brain_->getRegions()) {
            for (auto* syn : region->getSynapses()) {
                // Apply homeostatic plasticity rule
                // This is a simplified version
                float homeostaticModulation = homeostasis_.homeostasisScore;
                float currentWeight = syn->getWeight();
                
                // Modify weight based on homeostatic target
                if (std::abs(currentWeight) > 0.01f) {
                    float targetWeight = 0.1f * homeostaticModulation;  // Target around 0.1
                    float homeostaticWeightChange = (targetWeight - currentWeight) * 0.001f;
                    
                    // Apply change
                    syn->addToWeight(homeostaticWeightChange);
                }
            }
        }
    }
}

void SleepConsolidationSystem::updateNeuromodulatoryControl() {
    // Update neuromodulatory state based on sleep state
    // This simulates the interaction between neuromodulators and sleep-wake control
    
    // During deep sleep: high acetylcholine, low orexin
    if (sleepState_.currentState == SleepState::State::DeepSleep) {
        neuromodulators_.acetylcholine = 0.8f + sleepState_.sleepDepth * 0.2f;
        neuromodulators_.orexin = 0.1f;
        neuromodulators_.melatonin = 0.3f + sleepState_.sleepDepth * 0.7f;
    } else if (sleepState_.currentState == SleepState::State::LightSleep) {
        neuromodulators_.acetylcholine = 0.5f + sleepState_.sleepDepth * 0.3f;
        neuromodulators_.orexin = 0.2f;
        neuromodulators_.melatonin = 0.2f;
    } else if (sleepState_.currentState == SleepState::State::REM) {
        neuromodulators_.acetylcholine = 0.9f + sleepState_.sleepDepth * 0.1f;
        neuromodulators_.orexin = 0.3f;
        neuromodulators_.melatonin = 0.2f;
    } else { // Awake
        neuromodulators_.acetylcholine = 0.2f + sleepState_.sleepDepth * 0.3f;
        neuromodulators_.orexin = 0.6f + sleepState_.sleepDepth * 0.4f;
        neuromodulators_.melatonin = 0.0f;
    }
    
    // Serotonin and dopamine follow sleep state
    neuromodulators_.serotonin = 0.5f - 0.4f * sleepState_.sleepDepth;
    neuromodulators_.dopamine = 0.5f - 0.3f * sleepState_.sleepDepth;
}

void SleepConsolidationSystem::computeSynapticTargetWeights() {
    // Compute target synaptic weights based on homeostatic regulation
    // This is used for synaptic downscaling
    
    for (auto& target : downscalingTargets_) {
        // Adjust target based on homeostatic regulation
        target.targetWeight = 0.1f * homeostasis_.synapticExcitability * homeostasis_.inhibitionBalance;
        
        // Ensure target is reasonable
        target.targetWeight = std::clamp(target.targetWeight, -0.3f, 0.3f);
    }
}

} // namespace nlm
