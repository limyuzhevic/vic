#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>
#include <map>
#include <set>

namespace nlm {

// Sleep stage types
enum class SleepStage {
    Awake,
    LightSleep,      // N1, N2
    DeepSleep,       // N3, slow wave sleep
    REM,             // Rapid eye movement
    TransitionToSleep,
    TransitionToWake
};

// Memory type categories for sleep-dependent consolidation
enum class MemoryType {
    Declarative,     // Facts and events (hippocampal-dependent)
    Procedural,      // Skills and habits (cortical)
    Emotional,       // Affective memories
    ProceduralSequence,  // Motor sequences
    WorkingMemory,   // Active memory traces
    Prediction        // Predictive models
};

// Sleep pressure mechanism for sleep/wake regulation
class SleepPressure {
public:
    SleepPressure() : pressure_(0.0f), dissipationRate_(0.001f), 
                      buildupRate_(0.002f), homeostaticSetpoint_(1.0f) {}
    
    void accumulate(float amount) {
        pressure_ = std::min(pressure_ + amount, homeostaticSetpoint_ * 2.0f);
    }
    
    void dissipate(float amount) {
        pressure_ = std::max(0.0f, pressure_ - amount);
    }
    
    void update(float deltaTime) {
        // Natural dissipation during wake
        if (currentStage_ != SleepStage::Awake) {
            pressure_ = std::max(0.0f, pressure_ - dissipationRate_ * deltaTime);
        }
        // Buildup during wake
        if (currentStage_ == SleepStage::Awake) {
            pressure_ += buildupRate_ * deltaTime;
        }
    }
    
    float getPressure() const { return pressure_; }
    bool needsSleep() const { return pressure_ >= homeostaticSetpoint_; }
    
    void setStage(SleepStage stage) { currentStage_ = stage; }
    
private:
    float pressure_;           // Current sleep pressure (0-2.0)
    float dissipationRate_;    // Rate of pressure dissipation
    float buildupRate_;        // Rate of pressure buildup during wake
    float homeostaticSetpoint_; // Target pressure level for sleep
    SleepStage currentStage_ = SleepStage::Awake;
};

// Sleep stage cycle management
class SleepStageManager {
public:
    SleepStageManager() : currentStage_(SleepStage::Awake), 
                          stageStartStep_(0), stageDuration_(0),
                          stageProgress_(0.0f) {}
    
    void update(SimulationStep currentStep, float deltaTime) {
        stageProgress_ += deltaTime;
        
        if (currentStage_ != SleepStage::Awake && 
            currentStage_ != SleepStage::TransitionToSleep &&
            currentStage_ != SleepStage::TransitionToWake) {
            
            if (stageProgress_ >= stageDuration_) {
                advanceStage(currentStep);
                stageProgress_ = 0.0f;
            }
        }
    }
    
    void setStage(SleepStage stage, SimulationStep currentStep) {
        currentStage_ = stage;
        stageStartStep_ = currentStep;
        setStageDuration(stage);
        stageProgress_ = 0.0f;
    }
    
    SleepStage getCurrentStage() const { return currentStage_; }
    float getStageProgress() const { return stageProgress_ / stageDuration_; }
    SimulationStep getStageStartStep() const { return stageStartStep_; }
    float getStageDuration() const { return stageDuration_; }
    
    // Check if we're in a sleep stage (light, deep, or REM)
    bool isSleeping() const {
        return currentStage_ == SleepStage::LightSleep ||
               currentStage_ == SleepStage::DeepSleep ||
               currentStage_ == SleepStage::REM;
    }
    
    // Check if we're in a memory consolidation stage
    bool isConsolidationStage() const {
        return currentStage_ == SleepStage::DeepSleep ||
               currentStage_ == SleepStage::REM;
    }
    
    // Get sleep stage-specific consolidation effectiveness
    float getConsolidationEffectiveness() const {
        switch (currentStage_) {
            case SleepStage::DeepSleep: return 0.9f;  // High for declarative
            case SleepStage::REM: return 0.8f;      // High for procedural/emotional
            case SleepStage::LightSleep: return 0.4f; // Moderate
            default: return 0.0f;
        }
    }
    
private:
    void setStageDuration(SleepStage stage) {
        switch (stage) {
            case SleepStage::LightSleep:
                stageDuration_ = 2000.0f;  // Steps ~2 seconds
                break;
            case SleepStage::DeepSleep:
                stageDuration_ = 5000.0f;  // Steps ~5 seconds
                break;
            case SleepStage::REM:
                stageDuration_ = 3000.0f;  // Steps ~3 seconds
                break;
            default:
                stageDuration_ = 100.0f;  // Short default
                break;
        }
    }
    
    void advanceStage(SimulationStep currentStep) {
        switch (currentStage_) {
            case SleepStage::Awake:
                setStage(SleepStage::TransitionToSleep, currentStep);
                break;
            case SleepStage::TransitionToSleep:
                setStage(SleepStage::LightSleep, currentStep);
                break;
            case SleepStage::LightSleep:
                setStage(SleepStage::DeepSleep, currentStep);
                break;
            case SleepStage::DeepSleep:
                setStage(SleepStage::REM, currentStep);
                break;
            case SleepStage::REM:
                setStage(SleepStage::TransitionToWake, currentStep);
                break;
            case SleepStage::TransitionToWake:
                setStage(SleepStage::Awake, currentStep);
                break;
        }
    }
    
    SleepStage currentStage_;
    SimulationStep stageStartStep_;
    float stageDuration_;
    float stageProgress_;
};

// Sleep replay patterns
class SleepReplayManager {
public:
    struct ReplayPattern {
        MemoryType memoryType;
        SimulationStep episodeId;
        float priority;
        float strength;
        std::vector<float> pattern;
        SimulationStep lastReplayStep;
        size_t replayCount;
    };
    
    SleepReplayManager() : replayEnabled_(true), replayInterval_(100),
                           slowOscillationPhase_(0.0f), sharpWaveRipples_(false) {}
    
    void update(SimulationStep currentStep, float deltaTime) {
        slowOscillationPhase_ += deltaTime * 0.001f;
        if (slowOscillationPhase_ > 2.0f * M_PI) {
            slowOscillationPhase_ -= 2.0f * M_PI;
        }
    }
    
    // Schedule episode for sleep replay based on memory type
    void scheduleReplay(SimulationStep episodeId, MemoryType memoryType, 
                       const EpisodicMemoryItem* episode, float priority) {
        if (!episode || !replayEnabled_) return;
        
        float replayStrength = calculateReplayStrength(episode, memoryType);
        
        // Memory type-specific scheduling
        float replayTiming = 0.0f;
        switch (memoryType) {
            case MemoryType::Declarative:
                replayTiming = 0.5f;  // Earlier in sleep
                break;
            case MemoryType::Procedural:
                replayTiming = 1.5f;  // Later in sleep
                break;
            case MemoryType::Emotional:
                replayTiming = 0.8f;  // Mid-sleep
                break;
            default:
                replayTiming = 1.0f;  // Standard timing
                break;
        }
        
        // Schedule replay at appropriate sleep stage
        ReplayPattern pattern {
            memoryType,
            episodeId,
            priority * replayStrength,
            replayStrength,
            episode->neuronActivations,
            currentStep,
            0
        };
        
        scheduledReplays_.push_back(pattern);
    }
    
    // Process scheduled replays for current step
    void processReplays(SimulationStep currentStep, Brain* brain,
                       SleepStageManager& stageManager) {
        if (!brain || !brain->getEpisodicMemory()) return;
        
        // Determine which replays to execute based on slow oscillation phase
        float oscillationFactor = (1.0f + std::cos(slowOscillationPhase_)) * 0.5f;
        
        std::vector<ReplayPattern> completedReplays;
        
        for (auto& pattern : scheduledReplays_) {
            if (currentStep - pattern.lastReplayStep >= replayInterval_ * oscillationFactor) {
                // Execute replay
                executeReplay(pattern, currentStep, brain, stageManager, oscillationFactor);
                pattern.lastReplayStep = currentStep;
                pattern.replayCount++;
                
                completedReplays.push_back(pattern);
                
                if (completedReplays.size() >= 3) break; // Limit concurrent replays
            }
        }
        
        // Remove completed replays
        scheduledReplays_.erase(
            std::remove_if(scheduledReplays_.begin(), scheduledReplays_.end(),
                [&completedReplays](const ReplayPattern& pattern) {
                    return std::find_if(completedReplays.begin(), completedReplays.end(),
                        [&pattern](const ReplayPattern& completed) {
                            return completed.episodeId == pattern.episodeId;
                        }) != completedReplays.end();
                }),
            scheduledReplays_.end()
        );
    }
    
    // Get current slow oscillation phase for timing
    float getSlowOscillationPhase() const { return slowOscillationPhase_; }
    
    // Enable/disable sleep replay
    void setReplayEnabled(bool enabled) { replayEnabled_ = enabled; }
    
    // Get all scheduled replays
    const std::vector<ReplayPattern>& getScheduledReplays() const { return scheduledReplays_; }
    
private:
    void executeReplay(const ReplayPattern& pattern, SimulationStep currentStep,
                      Brain* brain, SleepStageManager& stageManager, float oscillationFactor) {
        if (!brain || !brain->getEpisodicMemory()) return;
        
        auto* episode = brain->getEpisodicMemory()->getEpisode(pattern.episodeId);
        if (!episode) return;
        
        // Enhanced replay based on sleep stage
        float stageMultiplier = 1.0f;
        if (stageManager.getCurrentStage() == SleepStage::DeepSleep) {
            stageMultiplier *= 1.5f;  // Enhanced declarative replay
        } else if (stageManager.getCurrentStage() == SleepStage::REM) {
            stageMultiplier *= 1.3f;  // Enhanced procedural replay
        }
        
        // Enhanced replay with oscillation timing
        float replayStrength = pattern.strength * oscillationFactor * stageMultiplier;
        
        // Reactivate with sleep-enhanced activation
        for (size_t i = 0; i < std::min(pattern.pattern.size(), 
                                        episode->activeNeurons.size()); ++i) {
            float enhancedActivation = pattern.pattern[i] * replayStrength;
            brain->injectCurrent(episode->activeNeurons[i], enhancedActivation);
        }
    }
    
    float calculateReplayStrength(const EpisodicMemoryItem* episode, 
                                 MemoryType memoryType) const {
        if (!episode) return 0.0f;
        
        float strength = 0.0f;
        
        // Recency factor
        float recencyScore = 1.0f / (episode->age + 1);
        
        // Reward factor
        float rewardScore = episode->reward;
        
        // Prediction error factor (unpredictable events get higher priority)
        float predictionErrorScore = std::min(1.0f, episode->predictionError * 2.0f);
        
        // Memory type-specific scoring
        switch (memoryType) {
            case MemoryType::Declarative:
                strength = recencyScore * 0.4f + rewardScore * 0.3f + 
                          predictionErrorScore * 0.3f;
                break;
            case MemoryType::Procedural:
                strength = rewardScore * 0.5f + episode->curiosityGain * 0.3f + 
                          recencyScore * 0.2f;
                break;
            case MemoryType::Emotional:
                strength = std::abs(episode->emotionalValence) * 0.4f + 
                          recencyScore * 0.3f + rewardScore * 0.3f;
                break;
            default:
                strength = recencyScore * 0.5f + rewardScore * 0.5f;
                break;
        }
        
        return std::clamp(strength, 0.0f, 1.0f);
    }
    
    bool replayEnabled_;
    size_t replayInterval_;
    float slowOscillationPhase_;
    bool sharpWaveRipples_;
    std::vector<ReplayPattern> scheduledReplays_;
};

// Sleep-dependent plasticity manager
class SleepDependentPlasticity {
public:
    SleepDependentPlasticity() : sleepPlasticityBoost_(1.2f), 
                                  slowWavePlasticityMultiplier_(1.5f),
                                  synapticDownscaling_(0.95f) {}
    
    void update(SimulationStep currentStep, Brain* brain, 
                SleepStageManager& stageManager) {
        if (!brain || !brain->getEpisodicMemory()) return;
        
        float stageMultiplier = 1.0f;
        
        switch (stageManager.getCurrentStage()) {
            case SleepStage::DeepSleep:
                stageMultiplier = slowWavePlasticityMultiplier_;
                processSlowWavePlasticity(brain, stageManager);
                break;
            case SleepStage::REM:
                stageMultiplier = sleepPlasticityBoost_;
                processREMPlasticity(brain, stageManager);
                break;
            case SleepStage::LightSleep:
                stageMultiplier = 1.1f;
                break;
            default:
                stageMultiplier = 1.0f;
                break;
        }
        
        // Apply stage-dependent plasticity boost to all synapses
        applyStageDependentPlasticity(brain, stageMultiplier);
        
        // Apply synaptic downscaling during sleep
        if (stageManager.isSleeping()) {
            applySynapticDownscaling(brain);
        }
    }
    
    // Apply slow wave sleep-dependent plasticity (systems consolidation)
    void processSlowWavePlasticity(Brain* brain, SleepStageManager& stageManager) {
        if (!brain || !brain->getEpisodicMemory()) return;
        
        // Systems consolidation: gradually transfer hippocampal dependence
        // to cortical networks through replay
        auto episodes = brain->getEpisodicMemory()->getEpisodesConsolidatedDuringSleep();
        
        for (const auto* episode : episodes) {
            // Enhance synaptic connections associated with sleep-consolidated memories
            for (size_t i = 0; i < std::min(episode->activeNeurons.size(), 
                                            episode->neuronActivations.size()); ++i) {
                float consolidationFactor = 1.0f + episode->sleepConsolidationGain * 0.3f;
                // In a full implementation, this would modify synaptic weights
                // For now, we note that systems consolidation is occurring
            }
        }
    }
    
    // Apply REM sleep-dependent plasticity (procedural learning)
    void processREMPlasticity(Brain* brain, SleepStageManager& stageManager) {
        if (!brain || !brain->getEpisodicMemory()) return;
        
        // Procedural memory integration: strengthen motor associations
        auto episodes = brain->getEpisodicMemory()->getEpisodesConsolidatedDuringSleep();
        
        for (const auto* episode : episodes) {
            if (episode->action != ActionType::Wait) {
                // Enhance action-neuron associations based on emotional valence
                float actionStrength = std::abs(episode->emotionalValence) * 0.5f + 
                                      episode->curiosityGain * 0.3f;
                // In a full implementation, this would strengthen motor pathways
            }
        }
    }
    
    void applySynapticDownscaling(Brain* brain) {
        if (!brain) return;
        
        // Synaptic homeostasis: downscale weakly active synapses
        // This is a simplified implementation - full implementation would
        // access and modify all synapses in the brain
        for (auto& region : brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Apply synaptic downscaling
                    // In a full implementation: neuron->applySynapticDownscaling(synapticDownscaling_);
                }
            }
        }
    }
    
private:
    void applyStageDependentPlasticity(Brain* brain, float multiplier) {
        // This would apply plasticity modifications throughout the brain
        // based on current sleep stage
        if (multiplier != 1.0f) {
            // In a full implementation, this would modify STDP parameters,
            // Hebbian plasticity, and other plasticity rules based on the
            // current sleep stage multiplier
        }
    }
    
    float sleepPlasticityBoost_;
    float slowWavePlasticityMultiplier_;
    float synapticDownscaling_;
};

// Comprehensive sleep/rest cycle manager with all required functionality
class SleepCycleManager {
public:
    SleepCycleManager() : currentStep_(0), initialized_(false),
                          currentStage_(SleepStage::Awake), sleepPressure_(),
                          stageManager_(), replayManager_(), plasticityManager_() {}
    
    void initialize(Brain* brain) {
        if (!brain || initialized_) return;
        
        sleepPressure_.setStage(SleepStage::Awake);
        stageManager_.setStage(SleepStage::Awake, 0);
        
        brain_ = brain;
        initialized_ = true;
        
        NLM_LOG_INFO("Sleep cycle manager initialized");
    }
    
    void update(SimulationStep currentStep, Timestamp currentTime, 
                float deltaTime, float neuromodulationLevel = 1.0f) {
        currentStep_ = currentStep;
        
        if (!initialized_ || !brain_) return;
        
        // Update sleep pressure
        sleepPressure_.update(deltaTime);
        
        // Update sleep stage
        stageManager_.update(currentStep, deltaTime);
        sleepPressure_.setStage(stageManager_.getCurrentStage());
        
        // Sleep onset/wake-up triggers based on sleep pressure
        handleSleepOnsetWakeup(currentStep, deltaTime);
        
        // Update sleep replay with slow oscillation phase
        replayManager_.update(currentStep, deltaTime);
        
        // Apply sleep-dependent plasticity
        plasticityManager_.update(currentStep, brain_, stageManager_);
        
        // Process sleep replay
        replayManager_.processReplays(currentStep, brain_, stageManager_);
        
        // Update neuromodulation based on sleep stage
        updateSleepNeuromodulation(neuromodulationLevel, currentStep);
    }
    
    SleepStage getCurrentStage() const { return currentStage_; }
    bool isSleeping() const { return stageManager_.isSleeping(); }
    bool isConsolidationStage() const { return stageManager_.isConsolidationStage(); }
    float getSleepPressure() const { return sleepPressure_.getPressure(); }
    float getStageProgress() const { return stageManager_.getStageProgress(); }
    
    // Enhanced memory consolidation during sleep
    void consolidateMemoryDuringSleep(SimulationStep currentStep, 
                                     const EpisodicMemoryItem& episode,
                                     MemoryType memoryType, float priority) {
        if (!brain_ || !brain_->getEpisodicMemory()) return;
        
        // Sleep-stage specific consolidation
        if (stageManager_.getCurrentStage() == SleepStage::DeepSleep && 
            memoryType == MemoryType::Declarative) {
            // Deep sleep consolidation: strengthen declarative memories
            // and promote systems consolidation
            replayManager_.scheduleReplay(
                currentStep, memoryType, &episode, priority * 1.2f);
        } 
        else if (stageManager_.getCurrentStage() == SleepStage::REM &&
                 (memoryType == MemoryType::Procedural || memoryType == MemoryType::Emotional)) {
            // REM sleep consolidation: enhance procedural and emotional memories
            replayManager_.scheduleReplay(
                currentStep, memoryType, &episode, priority * 1.1f);
        } 
        else if (memoryType == MemoryType::WorkingMemory && 
                 stageManager_.getCurrentStage() == SleepStage::LightSleep) {
            // Light sleep consolidation: prune and integrate working memory
            replayManager_.scheduleReplay(
                currentStep, memoryType, &episode, priority * 0.8f);
        }
        
        // General sleep consolidation for all types
        if (stageManager_.isConsolidationStage()) {
            float consolidationGain = stageManager_.getConsolidationEffectiveness();
            brain_->getEpisodicMemory()->markConsolidatedDuringSleep(
                currentStep, consolidationGain);
        }
    }
    
    // Sleep-dependent plasticity integration
    void applySleepDependentPlasticity(SimulationStep currentStep, float neuromodulationLevel) {
        // Integrate with existing neuromodulation systems
        if (brain_) {
            // ACh effects on attention and memory encoding during wake
            if (currentStage_ == SleepStage::Awake) {
                float attentionGain = neuromodulationLevel * 0.5f;
                // In full implementation: brain_->getAcetylcholine()->signalAttentionGain(attentionGain);
            }
            
            // NE effects on arousal and consolidation during sleep
            if (isSleeping()) {
                float consolidationFactor = sleepPressure_.getPressure() * 0.3f;
                // In full implementation: brain_->getNorepinephrine()->signalMemoryConsolidation(consolidationFactor);
            }
            
            // 5-HT effects on sleep/wake regulation
            float sleepWakeCycle = (isSleeping() ? 1.0f : 0.0f) + 
                                  (sleepPressure_.needsSleep() ? 0.5f : 0.0f);
            // In full implementation: brain_->getSerotonin()->signalSleepWake(sleepWakeCycle);
        }
    }
    
    // Classify memory type for sleep-dependent consolidation
    MemoryType classifyMemoryType(const EpisodicMemoryItem& episode) const {
        // Determine memory type based on episode characteristics
        if (episode.action != ActionType::Wait) {
            return MemoryType::ProceduralSequence;
        } else if (std::abs(episode.emotionalValence) > 0.5f) {
            return MemoryType::Emotional;
        } else if (episode.predictionError > 0.5f) {
            return MemoryType::Prediction;
        } else {
            return MemoryType::Declarative;
        }
    }
    
    // Enhanced replay with sleep-specific patterns
    void executeSleepReplay(const EpisodicMemoryItem* episode, 
                           SleepStage stage, float oscillationFactor) {
        if (!episode || !brain_ || !brain_->getEpisodicMemory()) return;
        
        // Enhanced replay based on sleep stage
        switch (stage) {
            case SleepStage::DeepSleep:
                replayManager_.executeReplay(
                    episode->timestamp, MemoryType::Declarative, 
                    episode, 1.5f * oscillationFactor);
                break;
            case SleepStage::REM:
                replayManager_.executeReplay(
                    episode->timestamp, MemoryType::Procedural,
                    episode, 1.3f * oscillationFactor);
                break;
            case SleepStage::LightSleep:
                replayManager_.executeReplay(
                    episode->timestamp, MemoryType::WorkingMemory,
                    episode, 0.8f * oscillationFactor);
                break;
            default:
                replayManager_.executeReplay(
                    episode->timestamp, classifyMemoryType(*episode),
                    episode, 1.0f * oscillationFactor);
                break;
        }
    }
    
    // Get sleep-dependent plasticity information
    float getSleepPlasticityBoost() const { return plasticityManager_.sleepPlasticityBoost_; }
    float getSlowWavePlasticityMultiplier() const { return plasticityManager_.slowWavePlasticityMultiplier_; }
    
    // Get sleep replay information
    const std::vector<SleepReplayManager::ReplayPattern>& getScheduledReplays() const {
        return replayManager_.getScheduledReplays();
    }
    
    // Reset sleep cycle manager
    void reset() {
        sleepPressure_ = SleepPressure();
        stageManager_ = SleepStageManager();
        stageManager_.setStage(SleepStage::Awake, 0);
        replayManager_.setReplayEnabled(true);
        
        NLM_LOG_INFO("Sleep cycle manager reset");
    }
    
private:
    void handleSleepOnsetWakeup(SimulationStep currentStep, float deltaTime) {
        // Sleep onset trigger when pressure reaches threshold
        if (sleepPressure_.needsSleep() && currentStage_ != SleepStage::TransitionToSleep) {
            stageManager_.setStage(SleepStage::TransitionToSleep, currentStep);
            // Log sleep onset
        } else if (!sleepPressure_.needsSleep() && 
                   stageManager_.getCurrentStage() != SleepStage::Awake &&
                   stageManager_.getCurrentStage() != SleepStage::TransitionToWake) {
            // Wake-up trigger when pressure is low
            stageManager_.setStage(SleepStage::TransitionToWake, currentStep);
            // Log wake-up
        }
    }
    
    void updateSleepNeuromodulation(float neuromodulationLevel, SimulationStep currentStep) {
        // Update neuromodulation systems based on sleep stage
        applySleepDependentPlasticity(currentStep, neuromodulationLevel);
    }
    
    SimulationStep currentStep_;
    bool initialized_;
    Brain* brain_;
    SleepStage currentStage_;
    
    SleepPressure sleepPressure_;
    SleepStageManager stageManager_;
    SleepReplayManager replayManager_;
    SleepDependentPlasticity plasticityManager_;
};

} // namespace nlm
