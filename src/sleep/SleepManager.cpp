#include "SleepManager.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../sensory/Vision.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct SleepManager::Impl {
    Brain* brain;
    SimulationStep lastObservationStep;
    
    Impl() : brain(nullptr), lastObservationStep(0) {}
};

SleepManager::SleepManager()
    : pImpl(new Impl)
    , brain_(nullptr)
    , lastSleepStart_(0)
    , sleepStartTime_(0)
    , wakeUpTime_(0)
    , currentStep_(0)
    , wakeCount_(0)
    , sleepCount_(0)
    , totalSleepTime_(0.0f)
    , lightSleepDuration_(1000)
    , deepSleepDuration_(2000)
    , remSleepDuration_(1500)
    , firingThreshold_(0.3f)
    , arousalThreshold_(0.7f)
    , stressThreshold_(0.8f)
    , timeInCurrentStage_(0)
    , stageProgressionCounter_(0) {}

SleepManager::~SleepManager() = default;

void SleepManager::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("SleepManager initialized");
}

void SleepManager::update(SimulationStep currentStep, float globalArousal) {
    currentStep_ = currentStep;
    
    if (!cycleState_.isActive) {
        // Check for sleep onset
        detectSleepNeed({}, {});
        if (cycleState_.isActive) {
            sleepStartTime_ = currentStep;
            lastSleepStart_ = currentStep;
            cycleState_.startTime = currentStep;
            timeInCurrentStage_ = 0;
            stageProgressionCounter_ = 0;
            NLM_LOG_INFO("Sleep onset detected - entering " + 
                         std::to_string(static_cast<int>(cycleState_.stage)) + " sleep");
        }
    } else {
        // Update current sleep state
        timeInCurrentStage_++;
        cycleState_.duration = currentStep - cycleState_.startTime;
        
        // Check wake-up conditions
        checkWakeConditions(SensoryInput(), stressSignals_, attentionGuidance_);
        
        // Advance sleep stage if needed
        advanceSleepStage(currentStep);
        
        // Coordinate memory consolidation
        coordinateMemoryConsolidation(currentStep);
        
        if (cycleState_.shouldWake) {
            // Wake up
            wakeUpTime_ = currentStep;
            totalSleepTime_ += cycleState_.duration;
            sleepCount_++;
            
            NLM_LOG_INFO("Sleep wake-up at step " + std::to_string(currentStep) +
                        " after " + std::to_string(cycleState_.duration) + " steps");
            
            cycleState_.isActive = false;
            cycleState_.shouldWake = false;
            wakeCount_++;
        }
    }
}

void SleepManager::detectSleepNeed(const std::vector<NeuronId>& firingNeurons,
                                  const std::vector<float>& neuronActivities) {
    float averageFiring = 0.0f;
    if (!neuronActivities.empty()) {
        float sum = std::accumulate(neuronActivities.begin(), neuronActivities.end(), 0.0f);
        averageFiring = sum / neuronActivities.size();
    }
    
    // Sleep when firing rate drops below threshold
    if (averageFiring < firingThreshold_) {
        cycleState_.isActive = true;
        cycleState_.stage = SleepStage::LightSleep;
        cycleState_.arousalLevel = 0.2f;
        cycleState_.stressLevel = 0.3f + (stressSignals_.cortisolLevel * 0.3f);
        cycleState_.attentionLevel = attentionGuidance_.globalAttention * 0.2f; // Reduced attention during sleep
        cycleState_.predictionError = stressSignals_.predictionError * 0.5f;
        cycleState_.neuromodulation[0] = std::min(0.3f, stressSignals_.noradrenalineLevel); // Reduced DA
        cycleState_.neuromodulation[1] = 0.4f; // Moderate Ach
        cycleState_.neuromodulation[2] = 0.2f + (stressSignals_.noradrenalineLevel * 0.2f); // Low NE
        
        NLM_LOG_INFO("Sleep detected - average firing: " + std::to_string(averageFiring));
    }
}

void SleepManager::checkWakeConditions(const SensoryInput& input,
                                      const StressSignals& stress,
                                      const AttentionGuidance& attention) {
    cycleState_.shouldWake = false;
    
    // Wake up if arousal exceeds threshold
    if (stress.arousalLevel > arousalThreshold_) {
        cycleState_.shouldWake = true;
        cycleState_.stage = SleepStage::Awake;
        NLM_LOG_INFO("Wake-up due to high arousal level: " + std::to_string(stress.arousalLevel));
    }
    
    // Wake up if stress exceeds threshold
    if (stress.cortisolLevel > stressThreshold_) {
        cycleState_.shouldWake = true;
        NLM_LOG_INFO("Wake-up due to high stress level: " + std::to_string(stress.cortisolLevel));
    }
    
    // Wake up if sensory input exceeds threshold
    if (!input.sensoryData.empty()) {
        float inputIntensity = 0.0f;
        for (float val : input.sensoryData) {
            inputIntensity += std::abs(val);
        }
        inputIntensity /= input.sensoryData.size();
        if (inputIntensity > 0.5f) {
            cycleState_.shouldWake = true;
            NLM_LOG_INFO("Wake-up due to sensory input: " + std::to_string(inputIntensity));
        }
    }
    
    // Wake up if attention level is too high (strong external stimulus)
    if (attention.globalAttention > 0.8f) {
        cycleState_.shouldWake = true;
        NLM_LOG_INFO("Wake-up due to high attention level: " + std::to_string(attention.globalAttention));
    }
}

void SleepManager::advanceSleepStage(SimulationStep currentStep) {
    stageProgressionCounter_++;
    
    if (cycleState_.stage == SleepStage::LightSleep) {
        timeInCurrentStage_ %= lightSleepDuration_;
        if (timeInCurrentStage_ == 0) {
            cycleState_.stage = SleepStage::DeepSleep;
            timeInCurrentStage_ = 0;
            NLM_LOG_INFO("Transition to Deep Sleep (slow-wave sleep)");
        }
    } else if (cycleState_.stage == SleepStage::DeepSleep) {
        timeInCurrentStage_ %= deepSleepDuration_;
        if (timeInCurrentStage_ == 0) {
            cycleState_.stage = SleepStage::REM;
            timeInCurrentStage_ = 0;
            NLM_LOG_INFO("Transition to REM Sleep");
        }
    } else if (cycleState_.stage == SleepStage::REM) {
        timeInCurrentStage_ %= remSleepDuration_;
        if (timeInCurrentStage_ == 0) {
            cycleState_.stage = SleepStage::DeepSleep; // Go back to deep sleep (typical cycle)
            timeInCurrentStage_ = 0;
            NLM_LOG_INFO("Return to Deep Sleep after REM");
        }
    }
}

void SleepManager::updateStressSignals(const StressSignals& stress) {
    stressSignals_ = stress;
    
    // Apply stress to consolidation priorities
    applyStressToConsolidation();
}

void SleepManager::applyStressToConsolidation() {
    // High stress enhances consolidation of salient/negative memories
    cycleState_.stressLevel = stressSignals_.cortisolLevel + stressSignals_.noradrenalineLevel * 0.5f;
    cycleState_.stressLevel = std::min(1.0f, cycleState_.stressLevel);
    
    // Stress enhances memory reversal and integration
    cycleState_.memoryReversalScore = cycleState_.stressLevel * 0.7f;
}

void SleepManager::updateAttentionGuidance(const AttentionGuidance& attention) {
    attentionGuidance_ = attention;
    
    // Apply attention to consolidation priorities
    applyAttentionToConsolidation();
}

void SleepManager::applyAttentionToConsolidation() {
    // Attention during sleep guides which memories get consolidated
    cycleState_.attentionLevel = attentionGuidance_.globalAttention * 0.3f;
    
    // Attention guides memory replay and integration
    if (attentionGuidance_.workingMemoryFocus > 0.5f) {
        // Focus on working memory consolidation
        NLM_LOG_INFO("Sleep consolidation focused on working memory");
    }
    
    if (attentionGuidance_.episodicFocus > 0.5f) {
        // Focus on episodic memory consolidation
        NLM_LOG_INFO("Sleep consolidation focused on episodic memory");
    }
}

void SleepManager::coordinateMemoryConsolidation(SimulationStep currentStep) {
    // Trigger sleep replay at appropriate intervals
    triggerSleepReplay(currentStep);
    
    // Trigger offline learning based on sleep stage
    triggerOfflineLearning(currentStep);
}

void SleepManager::triggerSleepReplay(SimulationStep currentStep) {
    // Sleep replay happens every 500-800 steps during sleep
    if (currentStep % 600 == 0 && currentStep > 0) {
        NLM_LOG_INFO("Sleep replay triggered at step " + std::to_string(currentStep) +
                    " (stage: " + std::to_string(static_cast<int>(cycleState_.stage)) + ")");
        
        // This will be called from Brain.cpp to trigger replay
        // In a real implementation, this would interact with memory systems
    }
}

void SleepManager::triggerOfflineLearning(SimulationStep currentStep) {
    // Offline learning based on sleep stage
    if (cycleState_.stage == SleepStage::DeepSleep) {
        // Deep sleep: systems consolidation (hippocampal-neocortical integration)
        NLM_LOG_INFO("Deep sleep - systems consolidation (hippocampal-neocortical)");
    } else if (cycleState_.stage == SleepStage::REM) {
        // REM sleep: procedural and emotional memory consolidation
        NLM_LOG_INFO("REM sleep - procedural and emotional memory consolidation");
    }
    
    // Apply stress and attention guidance to offline learning
    if (cycleState_.stressLevel > 0.5f) {
        NLM_LOG_INFO("Stress-guided consolidation with stress level: " + std::to_string(cycleState_.stressLevel));
    }
    
    if (cycleState_.attentionLevel > 0.3f) {
        NLM_LOG_INFO("Attention-guided consolidation with attention level: " + std::to_string(cycleState_.attentionLevel));
    }
}

std::vector<float> SleepManager::getStressBasedPriorities() const {
    std::vector<float> priorities;
    
    // Generate stress-based memory consolidation priorities
    // High cortisol -> prioritize salient/negative memories
    float stressPriority = cycleState_.stressLevel * 0.7f + stressSignals_.predictionError * 0.3f;
    priorities.push_back(stressPriority);
    
    // Novelty during stress -> enhance consolidation
    float noveltyPriority = stressSignals_.noveltyLevel * cycleState_.stressLevel * 0.5f;
    priorities.push_back(noveltyPriority);
    
    // Prediction error during stress -> enhance learning
    float predictionPriority = stressSignals_.predictionError * (1.0f - stressSignals_.predictionError) * 0.8f;
    priorities.push_back(predictionPriority);
    
    return priorities;
}

std::vector<NeuronId> SleepManager::getAttentionGuidedTargets() const {
    std::vector<NeuronId> targets;
    
    // Generate attention-guided consolidation targets
    // Based on attention during sleep
    float attention = cycleState_.attentionLevel;
    
    if (attention > 0.7f) {
        // High attention - consolidate all active memories
        targets.resize(20);
        for (size_t i = 0; i < targets.size(); ++i) {
            targets[i] = NeuronId(i + 50000);
        }
    } else if (attention > 0.3f) {
        // Moderate attention - consolidate most important memories
        targets.resize(10);
        for (size_t i = 0; i < targets.size(); ++i) {
            targets[i] = NeuronId(i + 40000);
        }
    }
    
    return targets;
}

} // namespace nlm