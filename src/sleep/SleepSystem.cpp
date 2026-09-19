#include "SleepSystem.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <chrono>
#include <random>

namespace nlm {

struct SleepSystem::Impl {
    class Brain* brain;
    
    // Sleep cycle state
    bool sleeping;
    SleepStage sleepStage;
    Timestamp sleepStartTime;
    Timestamp sleepDepth;
    float sleepDuration;
    
    // Stage timing
    Timestamp stageTimer;
    Timestamp wakeReadyDuration;
    Timestamp remDuration;
    Timestamp deepNREMResponse;
    Timestamp consolidationDuration;
    Timestamp minSleepDuration;
    
    // Sleep staging
    Timestamp timeInCurrentStage;
    Timestamp stageStartTime;
    
    // Memory systems
    NeuralWorkingMemory* workingMemory;
    NeuralEpisodicMemory* episodicMemory;
    
    // Neuromodulators
    Acetylcholine* acetylcholine;
    Norepinephrine* norepinephrine;
    Dopamine* dopamine;
    
    // Sleep staging constants
    static constexpr float MAX_SLEEP_DEPTH = 1.0f;
    static constexpr float MIN_SLEEP_DEPTH = 0.0f;
    static constexpr float DEFAULT_SLEEP_DURATION = 1.0f;  // 1 second default
    
    // Memory replay
    size_t totalMemoriesReplayed;
    size_t replayCount;
    
    // Memory consolidation
    size_t totalMemoriesConsolidated;
    size_t consolidationCount;
    
    // Synaptic plasticity during sleep
    float synapticDownscalingRate;
    float synapticUplearningRate;
    std::mt19937 rng;
    
    // Configuration
    float remAmplitude;
    float deepNREMThreshold;
    float consolidationPower;
    
    Impl()
        : brain(nullptr)
        , sleeping(false)
        , sleepStage(SleepStage::Awake)
        , sleepStartTime(0.0)
        , sleepDepth(0.0)
        , sleepDuration(DEFAULT_SLEEP_DURATION)
        , stageTimer(0.0)
        , timeInCurrentStage(0.0)
        , stageStartTime(0.0)
        , workingMemory(nullptr)
        , episodicMemory(nullptr)
        , acetylcholine(nullptr)
        , norepinephrine(nullptr)
        , dopamine(nullptr)
        , totalMemoriesReplayed(0)
        , replayCount(0)
        , totalMemoriesConsolidated(0)
        , consolidationCount(0)
        , synapticDownscalingRate(0.01f)
        , synapticUplearningRate(0.005f)
        , rng(std::chrono::steady_clock::now().time_since_epoch().count())
        , remAmplitude(0.8f)
        , deepNREMThreshold(0.6f)
        , consolidationPower(0.7f)
    {
        // Initialize with default durations
        wakeReadyDuration = Brain::WAKE_READY_DURATION;
        remDuration = Brain::REM_DURATION;
        deepNREMResponse = Brain::DEEP_NREM_DURATION;
        consolidationDuration = Brain::CONSOLIDATION_DURATION;
        minSleepDuration = Brain::MIN_SLEEP_DURATION;
    }
};

SleepSystem::SleepSystem() : pImpl(new Impl()) {}

SleepSystem::~SleepSystem() {}

void SleepSystem::initialize(class Brain* brain) {
    pImpl->brain = brain;
    pImpl->workingMemory = brain->getWorkingMemory();
    pImpl->episodicMemory = brain->getEpisodicMemory();
    pImpl->acetylcholine = brain->getAcetylcholine();
    pImpl->norepinephrine = brain->getNorepinephrine();
    pImpl->dopamine = brain->getDopamine();
    
    NLM_LOG_INFO("SleepSystem initialized with brain reference");
}

void SleepSystem::update(Timestamp currentTime, SimulationStep currentStep) {
    if (!pImpl->brain || !pImpl->sleeping) {
        return;
    }
    
    // Update sleep timing
    pImpl->stageTimer = currentTime - pImpl->stageStartTime;
    pImpl->sleepDuration = currentTime - pImpl->sleepStartTime;
    
    // Gradually increase sleep depth
    if (pImpl->sleepDepth < pImpl->MAX_SLEEP_DEPTH) {
        pImpl->sleepDepth += 0.001f * (1.0f - pImpl->sleepDepth);
    }
    
    // Check if we need to transition to next stage
    bool stageTransition = false;
    
    switch (pImpl->sleepStage) {
        case SleepStage::Awake:
            if (pImpl->sleepStage != SleepStage::Awake) break;  // Not in awake stage
            // Transition to Wake_Ready after minimum time
            if (pImpl->sleepDuration >= pImpl->wakeReadyDuration) {
                pImpl->sleepStage = SleepStage::Wake_Ready;
                pImpl->stageStartTime = currentTime;
                pImpl->timeInCurrentStage = 0.0;
                stageTransition = true;
            }
            break;
            
        case SleepStage::Wake_Ready:
            // Transition to REM after preparation
            if (pImpl->stageTimer >= pImpl->remDuration) {
                pImpl->sleepStage = SleepStage::REM;
                pImpl->stageStartTime = currentTime;
                pImpl->timeInCurrentStage = 0.0;
                stageTransition = true;
            }
            break;
            
        case SleepStage::REM:
            // Check if we should end REM for deep sleep
            if (pImpl->stageTimer >= pImpl->remDuration) {
                // Decide whether to go to Deep_NREM or Consolidation based on memory strength
                float memoryStrength = pImpl->brain->getTotalSpikeCount() / 100.0f;
                if (memoryStrength > pImpl->deepNREMThreshold && pImpl->sleepDuration > pImpl->minSleepDuration) {
                    pImpl->sleepStage = SleepStage::Deep_NREM;
                } else {
                    pImpl->sleepStage = SleepStage::Consolidation;
                }
                pImpl->stageStartTime = currentTime;
                pImpl->timeInCurrentStage = 0.0;
                stageTransition = true;
            }
            break;
            
        case SleepStage::Deep_NREM:
            // Transition to consolidation after deep sleep
            if (pImpl->stageTimer >= pImpl->deepNREMResponse) {
                pImpl->sleepStage = SleepStage::Consolidation;
                pImpl->stageStartTime = currentTime;
                pImpl->timeInCurrentStage = 0.0;
                stageTransition = true;
            }
            break;
            
        case SleepStage::Consolidation:
            // End of sleep cycle
            if (pImpl->sleepDuration >= pImpl->sleepDuration) {
                endSleep();
                return;
            }
            break;
    }
    
    // Apply stage-specific effects
    applyStageEffects(currentTime, currentStep, stageTransition);
    
    // Update time in current stage
    pImpl->timeInCurrentStage = currentTime - pImpl->stageStartTime;
}

bool SleepSystem::startSleep() {
    if (pImpl->sleeping) {
        NLM_LOG_WARNING("SleepSystem::startSleep called but already sleeping");
        return false;
    }
    
    pImpl->sleeping = true;
    pImpl->sleepStage = SleepStage::Awake;
    pImpl->sleepStartTime = 0.0;  // Will be set when we have currentTime
    pImpl->sleepDepth = 0.0f;
    pImpl->stageStartTime = 0.0;
    pImpl->timeInCurrentStage = 0.0;
    
    NLM_LOG_INFO("SleepSystem: Starting sleep cycle");
    return true;
}

void SleepSystem::endSleep() {
    if (!pImpl->sleeping) {
        return;
    }
    
    NLM_LOG_INFO("SleepSystem: Ending sleep cycle after " + 
                 std::to_string(pImpl->sleepDuration) + " seconds");
    
    pImpl->sleeping = false;
    pImpl->sleepStage = SleepStage::Awake;
    pImpl->sleepDepth = 0.0f;
    pImpl->stageTimer = 0.0;
    pImpl->timeInCurrentStage = 0.0;
}

bool SleepSystem::isSleeping() const {
    return pImpl->sleeping;
}

SleepStage SleepSystem::getSleepStage() const {
    return pImpl->sleepStage;
}

float SleepSystem::getSleepDepth() const {
    return pImpl->sleepDepth;
}

float SleepSystem::getSleepDuration() const {
    return pImpl->sleepDuration;
}

float SleepSystem::getTimeInCurrentStage() const {
    return pImpl->timeInCurrentStage;
}

void SleepSystem::advanceToNextStage() {
    // Force transition to next stage (for testing)
    if (!pImpl->sleeping) return;
    
    switch (pImpl->sleepStage) {
        case SleepStage::Awake:
            pImpl->sleepStage = SleepStage::Wake_Ready;
            break;
        case SleepStage::Wake_Ready:
            pImpl->sleepStage = SleepStage::REM;
            break;
        case SleepStage::REM:
            pImpl->sleepStage = SleepStage::Deep_NREM;
            break;
        case SleepStage::Deep_NREM:
            pImpl->sleepStage = SleepStage::Consolidation;
            break;
        case SleepStage::Consolidation:
            endSleep();
            return;
    }
    
    pImpl->stageStartTime = 0.0;  // Would use currentTime
    pImpl->timeInCurrentStage = 0.0;
}

void SleepSystem::applyStageEffects(Timestamp currentTime, SimulationStep currentStep, bool stageTransition) {
    // Apply neuromodulator levels for each stage
    switch (pImpl->sleepStage) {
        case SleepStage::Awake:
            // Wake state - lower ACh, NE
            if (pImpl->acetylcholine) {
                pImpl->acetylcholine->setLevel(0.1f + pImpl->sleepDepth * 0.2f);
            }
            if (pImpl->norepinephrine) {
                pImpl->norepinephrine->setLevel(0.1f + pImpl->sleepDepth * 0.3f);
            }
            if (pImpl->dopamine) {
                pImpl->dopamine->setLevel(0.2f * (1.0f - pImpl->sleepDepth));
            }
            break;
            
        case SleepStage::Wake_Ready:
            // Preparation phase - gradual increase in ACh
            if (pImpl->acetylcholine) {
                pImpl->acetylcholine->setLevel(0.3f + pImpl->sleepDepth * 0.5f);
            }
            if (pImpl->norepinephrine) {
                pImpl->norepinephrine->setLevel(0.2f + pImpl->sleepDepth * 0.4f);
            }
            break;
            
        case SleepStage::REM:
            // REM phase - high ACh, low NE, for memory replay
            if (pImpl->acetylcholine) {
                pImpl->acetylcholine->setLevel(0.8f + pImpl->remAmplitude * std::sin(pImpl->timeInCurrentStage * 2.0f));
            }
            if (pImpl->norepinephrine) {
                pImpl->norepinephrine->setLevel(0.1f);
            }
            if (pImpl->dopamine) {
                pImpl->dopamine->setLevel(0.3f);
            }
            
            // Perform memory replay during REM
            performMemoryReplay(currentStep);
            break;
            
        case SleepStage::Deep_NREM:
            // Deep NREM - moderate ACh, low NE and DA, synaptic downscaling
            if (pImpl->acetylcholine) {
                pImpl->acetylcholine->setLevel(0.4f * (1.0f - pImpl->sleepDepth * 0.5f));
            }
            if (pImpl->norepinephrine) {
                pImpl->norepinephrine->setLevel(0.1f);
            }
            if (pImpl->dopamine) {
                pImpl->dopamine->setLevel(0.1f);
            }
            
            // Perform synaptic downscaling
            performSynapticDownscaling();
            break;
            
        case SleepStage::Consolidation:
            // Consolidation phase - high ACh, low NE, memory integration
            if (pImpl->acetylcholine) {
                pImpl->acetylcholine->setLevel(0.6f + pImpl->consolidationPower * std::sin(pImpl->timeInCurrentStage * 1.0f));
            }
            if (pImpl->norepinephrine) {
                pImpl->norepinephrine->setLevel(0.15f);
            }
            if (pImpl->dopamine) {
                pImpl->dopamine->setLevel(0.2f);
            }
            
            // Perform memory consolidation
            performMemoryConsolidation();
            break;
    }
    
    // Apply sleep stage effects to neurons if we have access
    if (pImpl->brain && stageTransition) {
        // Inject current based on sleep stage
        switch (pImpl->sleepStage) {
            case SleepStage::REM:
                // REM-specific neuromodulatory effects
                pImpl->brain->injectCurrentToNeurons(NeuronType::Internal, 0.5f * pImpl->sleepDepth);
                break;
            case SleepStage::Deep_NREM:
                // Deep sleep - strong inhibition to promote slow oscillations
                pImpl->brain->injectCurrentToNeurons(NeuronType::Excitatory, -0.3f * pImpl->sleepDepth);
                pImpl->brain->injectCurrentToNeurons(NeuronType::Internal, 0.2f * pImpl->sleepDepth);
                break;
            case SleepStage::Consolidation:
                // Consolidation - balanced excitation
                pImpl->brain->injectCurrentToNeurons(NeuronType::Internal, 0.3f * pImpl->sleepDepth);
                break;
            default:
                break;
        }
    }
}

void SleepSystem::performMemoryReplay(SimulationStep currentStep) {
    if (!pImpl->episodicMemory) return;
    
    // Replay strong episodic memories during REM sleep
    auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
    if (episodesToReplay.empty()) return;
    
    pImpl->replayCount++;
    pImpl->totalMemoriesReplayed += episodesToReplay.size();
    
    for (const auto* episode : episodesToReplay) {
        // Replay the episode to strengthen memory traces
        pImpl->episodicMemory->replayEpisode(episode);
    }
    
    NLM_LOG_DEBUG("SleepSystem: Replayed " + std::to_string(episodesToReplay.size()) + 
                 " memories during REM (total: " + std::to_string(pImpl->totalMemoriesReplayed) + ")");
}

void SleepSystem::performMemoryConsolidation() {
    if (!pImpl->episodicMemory) return;
    
    // Consolidate memories during NREM and consolidation phases
    pImpl->consolidationCount++;
    
    // Use moderate consolidation threshold for system consolidation
    pImpl->episodicMemory->consolidate(0.5f);
    
    NLM_LOG_DEBUG("SleepSystem: Consolidated memories (count: " + 
                 std::to_string(pImpl->consolidationCount) + ")");
}

void SleepSystem::performSynapticDownscaling() {
    if (!pImpl->brain) return;
    
    // Get all regions and apply synaptic downscaling
    auto regions = pImpl->brain->getRegions();
    for (auto& region : regions) {
        for (auto& syn : region->getSynapses()) {
            // Synaptic downscaling - weaken unimportant connections
            float weight = syn->getWeight();
            if (std::abs(weight) > 0.01f) {  // Only scale significant weights
                // Downscale based on current sleep depth and random factor
                float scaleFactor = 1.0f - pImpl->synapticDownscalingRate * pImpl->sleepDepth;
                scaleFactor += (pImpl->rng() % 100) / 100.0f * 0.1f;  // Add some randomness
                
                // Apply downscaling
                float newWeight = weight * scaleFactor;
                syn->setWeight(newWeight);
            }
        }
    }
    
    // Synaptic uplearning - strengthen important connections
    // This happens less frequently and with smaller magnitude
    std::uniform_int_distribution<size_t> dist(0, regions.size() - 1);
    for (size_t i = 0; i < std::min<size_t>(3, regions.size()); ++i) {
        size_t regionIdx = dist(pImpl->rng);
        auto& region = regions[regionIdx];
        
        // Pick a few synapses to upregulate
        auto& synapses = region->getSynapses();
        if (synapses.empty()) continue;
        
        std::uniform_int_distribution<size_t> synDist(0, synapses.size() - 1);
        for (size_t j = 0; j < 3; ++j) {
            size_t synIdx = synDist(pImpl->rng);
            auto& syn = synapses[synIdx];
            
            // Uplearn significant connections
            float weight = syn->getWeight();
            if (std::abs(weight) > 0.05f) {  // Only uplearn strong connections
                float uplearnFactor = 1.0f + pImpl->synapticUplearningRate * pImpl->sleepDepth;
                float newWeight = weight * uplearnFactor;
                syn->setWeight(newWeight);
            }
        }
    }
    
    NLM_LOG_DEBUG("SleepSystem: Performed synaptic downscaling/uplearning during deep NREM");
}

void SleepSystem::reset() {
    if (pImpl->sleeping) {
        endSleep();
    }
    
    pImpl->sleepStage = SleepStage::Awake;
    pImpl->sleepDepth = 0.0f;
    pImpl->stageTimer = 0.0;
    pImpl->timeInCurrentStage = 0.0;
    pImpl->stageStartTime = 0.0;
    pImpl->sleepDuration = 0.0f;
    
    // Reset statistics
    pImpl->totalMemoriesReplayed = 0;
    pImpl->replayCount = 0;
    pImpl->totalMemoriesConsolidated = 0;
    pImpl->consolidationCount = 0;
    
    NLM_LOG_INFO("SleepSystem: Reset completed");
}

void SleepSystem::setSleepDuration(float duration) {
    pImpl->sleepDuration = duration;
}

void SleepSystem::setWakeReadyDuration(Timestamp duration) {
    pImpl->wakeReadyDuration = duration;
}

void SleepSystem::setREMAmplitude(float amplitude) {
    pImpl->remAmplitude = amplitude;
}

void SleepSystem::setDeepNREMThreshold(float threshold) {
    pImpl->deepNREMThreshold = threshold;
}

void SleepSystem::setConsolidationPower(float power) {
    pImpl->consolidationPower = power;
}

void SleepSystem::setSynapticDownscalingRate(float rate) {
    pImpl->synapticDownscalingRate = rate;
}

void SleepSystem::setSynapticUplearningRate(float rate) {
    pImpl->synapticUplearningRate = rate;
}

size_t SleepSystem::getTotalMemoriesReplayed() const {
    return pImpl->totalMemoriesReplayed;
}

size_t SleepSystem::getReplayCount() const {
    return pImpl->replayCount;
}

size_t SleepSystem::getTotalMemoriesConsolidated() const {
    return pImpl->totalMemoriesConsolidated;
}

size_t SleepSystem::getConsolidationCount() const {
    return pImpl->consolidationCount;
}

float SleepSystem::getAverageSynapticWeight() const {
    // This would require iterating through all synapses
    // For now, return a placeholder
    return 0.5f;
}

float SleepSystem::getSynapticPruningRate() const {
    return pImpl->synapticDownscalingRate;
}

} // namespace nlm
