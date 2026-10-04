#include "SleepConsolidationCycle.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>

namespace nlm {

// Forward declarations
class Dopamine;
class Curiosity;
class PredictionError;
class Novelty;

SleepConsolidationCycle::SleepConsolidationCycle() : 
    brain_(nullptr),
    lastSleepCheck_(0),
    sleepInterval_(200.0),  // Check for sleep every 200 steps
    sleepDuration_(100.0),  // Sleep for 100 simulation steps
    minNoveltyForSleep_(0.3f),
    minMemoryLoadForSleep_(100),
    minStressForSleep_(-0.5f)
{
}

SleepConsolidationCycle::~SleepConsolidationCycle() = default;

void SleepConsolidationCycle::initialize(Brain* brain) {
    brain_ = brain;
    NLM_LOG_INFO("SleepConsolidationCycle initialized");
}

void SleepConsolidationCycle::update(double timestep) {
    if (!brain_) {
        return;
    }
    
    // Check if it's time to check for sleep
    lastSleepCheck_ += timestep;
    if (lastSleepCheck_ >= sleepInterval_) {
        lastSleepCheck_ = 0.0;
        
        // Check if brain should enter sleep
        if (shouldEnterSleep()) {
            enterSleep();
        }
        
        // If in sleep, update sleep progression
        if (isSleeping_) {
            updateSleepProgress(timestep);
        }
    }
    
    // If in sleep, reduce sensory processing
    if (isSleeping_) {
        applySleepEffects();
    }
}

bool SleepConsolidationCycle::shouldEnterSleep() const {
    if (!brain_) {
        return false;
    }
    
    // Check novelty level
    float novelty = 0.0f;
    if (brain_->getNovelty()) {
        novelty = brain_->getNovelty()->getLevel();
    }
    if (novelty > minNoveltyForSleep_) {
        NLM_LOG_INFO("High novelty levels (" + std::to_string(novelty) + ") triggering sleep");
        return true;
    }
    
    // Check memory load
    size_t memoryLoad = 0;
    if (brain_->getWorkingMemory()) {
        memoryLoad += brain_->getWorkingMemory()->getActiveTraces();
    }
    if (brain_->getEpisodicMemory()) {
        memoryLoad += brain_->getEpisodicMemory()->getEpisodeCount();
    }
    if (memoryLoad > minMemoryLoadForSleep_) {
        NLM_LOG_INFO("High memory load (" + std::to_string(memoryLoad) + ") triggering sleep");
        return true;
    }
    
    // Check stress level (dopamine)
    if (brain_->getDopamine()) {
        float dopamine = brain_->getDopamine()->getLevel();
        if (dopamine < minStressForSleep_) {
            NLM_LOG_INFO("Elevated stress (dopamine: " + std::to_string(dopamine) + ") triggering sleep");
            return true;
        }
    }
    
    // Time-based sleep check
    if (sleepTimer_ >= sleepInterval_) {
        NLM_LOG_INFO("Time-based sleep trigger (timer: " + std::to_string(sleepTimer_) + ")");
        return true;
    }
    
    return false;
}

void SleepConsolidationCycle::enterSleep() {
    if (isSleeping_) {
        return;  // Already sleeping
    }
    
    isSleeping_ = true;
    sleepTimer_ = 0.0;
    
    // Record pre-sleep state
    preSleepNovelty_ = (brain_->getNovelty()) ? brain_->getNovelty()->getLevel() : 0.0f;
    preSleepDopamine_ = (brain_->getDopamine()) ? brain_->getDopamine()->getLevel() : 0.0f;
    preSleepCuriosity_ = (brain_->getCuriosity()) ? brain_->getCuriosity()->getLevel() : 0.0f;
    
    // Log sleep entry
    NLM_LOG_INFO("Brain entering sleep/rest cycle");
    
    // Perform memory consolidation
    consolidateMemoriesDuringSleep();
    
    // Replay memories
    replayMemoriesDuringSleep();
    
    // Reset neuromodulation
    resetNeuromodulationDuringSleep();
    
    // Update development
    developmentDuringSleep();
}

void SleepConsolidationCycle::updateSleepProgress(double timestep) {
    sleepTimer_ += timestep;
    
    if (sleepTimer_ >= sleepDuration_) {
        // Sleep cycle complete
        exitSleep();
    }
}

void SleepConsolidationCycle::exitSleep() {
    if (!isSleeping_) {
        return;  // Already awake
    }
    
    isSleeping_ = false;
    sleepTimer_ = 0.0;
    
    // Log sleep exit
    NLM_LOG_INFO("Brain exited sleep/rest cycle, ready for new learning");
    
    // Increment developmental age
    if (brain_) {
        // In a real brain, sleep contributes to development
        // For now, we just log the event
        NLM_LOG_INFO("Sleep completed, development continuing");
    }
}

void SleepConsolidationCycle::applySleepEffects() {
    // During sleep, reduce sensory processing
    // This could be implemented by:
    // 1. Reducing sensory neuron excitability
    // 2. Decreasing sensory input gain
    // 3. Increasing sensory neuron inhibition
    
    if (brain_) {
        // Reduce sensory input by decreasing neuron excitability
        for (const auto& region : brain_->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                if (pop->getNeuronType() == NeuronType::Sensory) {
                    for (Neuron* neuron : pop->getNeurons()) {
                        // Reduce excitability during sleep
                        // This would be implemented in the neuron's sleep mode
                    }
                }
            }
        }
    }
}

void SleepConsolidationCycle::consolidateMemoriesDuringSleep() {
    if (!brain_) {
        return;
    }
    
    // Working memory consolidation
    if (brain_->getWorkingMemory()) {
        // Transfer working memory to episodic memory
        auto workingMemoryTraces = brain_->getWorkingMemory()->getMemoryNeurons();
        size_t traceCount = workingMemoryTraces.size();
        
        if (traceCount > 0) {
            NLM_LOG_INFO("Consolidating " + std::to_string(traceCount) + " working memory traces");
            
            // In a full implementation, this would create episodic memory entries
            // For now, we just log the consolidation
        }
        
        // Clear working memory
        brain_->getWorkingMemory()->clear();
    }
    
    // Episodic memory consolidation
    if (brain_->getEpisodicMemory()) {
        // Consolidate by removing weak memories
        float consolidationThreshold = 0.2f;  // More aggressive consolidation during sleep
        brain_->getEpisodicMemory()->consolidate(consolidationThreshold);
        
        size_t remainingEpisodes = brain_->getEpisodicMemory()->getEpisodeCount();
        NLM_LOG_INFO("Episodic memory consolidation complete: " + std::to_string(remainingEpisodes) + " episodes remaining");
    }
}

void SleepConsolidationCycle::replayMemoriesDuringSleep() {
    if (!brain_->getEpisodicMemory()) {
        return;
    }
    
    // Get recent episodes for replay
    auto recentEpisodes = brain_->getEpisodicMemory()->getRecentEpisodes(5);
    
    // Replay each episode to strengthen connections
    for (const auto* episode : recentEpisodes) {
        if (episode) {
            brain_->getEpisodicMemory()->replayEpisode(episode);
        }
    }
    
    NLM_LOG_INFO("Replayed " + std::to_string(recentEpisodes.size()) + " episodes during sleep");
}

void SleepConsolidationCycle::resetNeuromodulationDuringSleep() {
    if (!brain_) {
        return;
    }
    
    // Reset neuromodulation systems
    if (brain_->getDopamine()) {
        float currentDopamine = brain_->getDopamine()->getLevel();
        // Decay dopamine toward zero
        if (currentDopamine > 0.0f) {
            brain_->getDopamine()->setLevel(currentDopamine * 0.3f);
        } else if (currentDopamine < 0.0f) {
            brain_->getDopamine()->setLevel(currentDopamine * 0.7f);
        }
    }
    
    if (brain_->getCuriosity()) {
        // Curiosity decays during sleep
        float currentCuriosity = brain_->getCuriosity()->getLevel();
        brain_->getCuriosity()->setLevel(currentCuriosity * 0.5f);
    }
    
    if (brain_->getPredictionError()) {
        // Prediction error stabilizes during sleep
        float currentPredictionError = brain_->getPredictionError()->getMagnitude();
        brain_->getPredictionError()->setLevel(currentPredictionError * 0.7f);
    }
    
    NLM_LOG_INFO("Neuromodulation systems reset during sleep");
}

void SleepConsolidationCycle::developmentDuringSleep() {
    if (!brain_->getDevelopmentSystem()) {
        return;
    }
    
    // Progress development during sleep
    brain_->getDevelopmentSystem()->update(brain_.get(), *brain_->getRandomGenerator(), sleepDuration_);
    
    // Update developmental stage
    DevelopmentalStage currentStage = brain_->getDevelopmentalStage();
    
    // Simple developmental progression
    if (currentStage == DevelopmentalStage::Initial) {
        // Progress after sufficient time
        if (brain_->getDevelopmentalAge() >= 60.0) {
            brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
            NLM_LOG_INFO("Brain progressed to Critical Period development stage");
        }
    } else if (currentStage == DevelopmentalStage::CriticalPeriod) {
        if (brain_->getDevelopmentalAge() >= 300.0) {
            brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
            NLM_LOG_INFO("Brain progressed to Maturation development stage");
        }
    } else if (currentStage == DevelopmentalStage::Maturation) {
        if (brain_->getDevelopmentalAge() >= 900.0) {
            brain_->setDevelopmentalStage(DevelopmentalStage::Adult);
            NLM_LOG_INFO("Brain progressed to Adult development stage");
        }
    }
}

void SleepConsolidationCycle::setSleepParameters(double checkInterval, double duration) {
    sleepInterval_ = checkInterval;
    sleepDuration_ = duration;
    NLM_LOG_INFO("Sleep consolidation parameters updated: check interval " + 
                 std::to_string(sleepInterval_) + "s, duration " + std::to_string(sleepDuration_) + "s");
}

void SleepConsolidationCycle::setSleepTriggers(float minNovelty, size_t minMemoryLoad, float minStress) {
    minNoveltyForSleep_ = minNovelty;
    minMemoryLoadForSleep_ = minMemoryLoad;
    minStressForSleep_ = minStress;
    NLM_LOG_INFO("Sleep consolidation triggers updated");
}

bool SleepConsolidationCycle::isCurrentlySleeping() const {
    return isSleeping_;
}

float SleepConsolidationCycle::getSleepProgress() const {
    if (!isSleeping_) {
        return 0.0f;
    }
    return std::min(1.0f, static_cast<float>(sleepTimer_) / sleepDuration_);
}

void SleepConsolidationCycle::reset() {
    isSleeping_ = false;
    sleepTimer_ = 0.0;
    lastSleepCheck_ = 0.0;
    
    preSleepNovelty_ = 0.0f;
    preSleepDopamine_ = 0.0f;
    preSleepCuriosity_ = 0.0f;
    
    NLM_LOG_INFO("Sleep consolidation cycle reset");
}

} // namespace nlm