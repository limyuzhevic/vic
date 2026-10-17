#pragma once

#include "SleepConsolidationSystem.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct SleepConsolidationSystem::Impl {
    Brain* brain;
    
    // Rest cycle timing
    size_t restStartStep;
    size_t restEndStep;
    
    // Consolidation scheduling
    std::vector<std::pair<size_t, bool>> scheduledConsolidations;
    
    // Fast consolidation buffer
    std::vector<float> fastConsolidationBuffer;
    
    // Slow consolidation buffer
    std::vector<std::pair<size_t, float>> slowConsolidationBuffer;
    
    Impl() : brain(nullptr), restStartStep(0), restEndStep(0), 
             consolidationDepth(1.0f), fastConsolidationInterval(100), 
             slowConsolidationInterval(5000), consolidationThreshold(0.5f),
             consolidationProgress(0.0f), stepsRemaining(0) {}
    
    float consolidationDepth;
    size_t fastConsolidationInterval;
    size_t slowConsolidationInterval;
    float consolidationThreshold;
    float consolidationProgress;
    size_t stepsRemaining;
};

SleepConsolidationSystem::SleepConsolidationSystem()
    : pImpl(new Impl)
    , brain_(nullptr)
    , isResting_(false)
    , consolidationDepth_(1.0f)
    , fastConsolidationInterval_(100)
    , slowConsolidationInterval_(5000)
    , consolidationThreshold_(0.5f)
    , consolidationProgress_(0.0f)
    , stepsRemaining_(0)
    , developmentalStage_(DevelopmentalStage::Adult)
{
}

SleepConsolidationSystem::~SleepConsolidationSystem() = default;

void SleepConsolidationSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("SleepConsolidationSystem initialized");
}

void SleepConsolidationSystem::startRestCycle() {
    if (isResting_) return;
    
    isResting_ = true;
    pImpl->restStartStep = pImpl->brain ? pImpl->brain->getCurrentStep() : 0;
    NLM_LOG_INFO("Starting rest cycle for memory consolidation");
    
    // Schedule slow consolidation to happen during rest
    scheduleConsolidation(pImpl->restStartStep + 100, true);
}

void SleepConsolidationSystem::endRestCycle() {
    if (!isResting_) return;
    
    size_t restEndStep = pImpl->brain ? pImpl->brain->getCurrentStep() : 0;
    size_t restDuration = restEndStep - pImpl->restStartStep;
    
    isResting_ = false;
    NLM_LOG_INFO("Ending rest cycle. Duration: " + std::to_string(restDuration) + " steps");
}

void SleepConsolidationSystem::triggerConsolidation() {
    if (!brain_) return;
    
    // Check neuromodulator levels to determine consolidation need
    float dopamineLevel = brain_->getDopamine() ? brain_->getDopamine()->getLevel() : 0.0f;
    float acetylcholineLevel = brain_->getAcetylcholine() ? 
        brain_->getAcetylcholine()->getLevel() : 0.0f;
    
    // Consolidation is triggered by high neuromodulator levels
    float consolidationNeed = dopamineLevel * 0.5f + acetylcholineLevel * 0.5f;
    
    if (consolidationNeed > consolidationThreshold_) {
        scheduleConsolidation(pImpl->brain->getCurrentStep(), true);
        NLM_LOG_INFO("Consolidation triggered by neuromodulators: dopamine=" + 
                    std::to_string(dopamineLevel) + ", acetylcholine=" + 
                    std::to_string(acetylcholineLevel));
    }
}

void SleepConsolidationSystem::scheduleConsolidation(size_t step, bool force) {
    pImpl->scheduledConsolidations.emplace_back(step, force);
}

void SleepConsolidationSystem::performFastConsolidation() {
    if (!brain_ || !brain_->getEpisodicMemory()) return;
    
    // Fast consolidation: strengthen recent episodes, remove very weak ones
    auto* em = brain_->getEpisodicMemory();
    
    // Buffer recent episodes for strengthening
    std::vector<const EpisodicMemoryItem*> recentEpisodes;
    for (size_t i = 0; i < std::min(size_t(50), em->getEpisodeCount()); ++i) {
        recentEpisodes.push_back(em->getEpisode(em->getEpisodeCount() - 1 - i));
    }
    
    // Strengthen recent episodes
    for (const auto* episode : recentEpisodes) {
        // Get the episode index
        for (size_t i = 0; i < em->getEpisodeCount(); ++i) {
            if (em->getEpisode(i) == episode) {
                // Increase relevance of recent episodes
                em->updateRelevance(i, 0.1f * consolidationDepth_);
                break;
            }
        }
    }
    
    // Remove weak episodes
    float threshold = 0.3f * (1.0f - consolidationDepth_);
    em->consolidate(threshold);
    
    pImpl->fastConsolidationBuffer.push_back(consolidationDepth_);
    
    NLM_LOG_INFO("Fast consolidation completed. Depth: " + std::to_string(consolidationDepth_));
}

void SleepConsolidationSystem::performSlowConsolidation() {
    if (!brain_ || !brain_->getEpisodicMemory()) return;
    
    auto* em = brain_->getEpisodicMemory();
    
    // Slow consolidation: deep integration of important memories
    std::vector<size_t> episodesToConsolidate;
    
    // Select episodes for deep consolidation based on importance
    for (size_t i = 0; i < em->getEpisodeCount(); ++i) {
        const auto* episode = em->getEpisode(i);
        float importance = calculateEpisodeImportance(episode);
        
        if (importance > consolidationThreshold_) {
            episodesToConsolidate.push_back(i);
        }
    }
    
    // Deep consolidation of selected episodes
    for (size_t id : episodesToConsolidate) {
        const auto* episode = em->getEpisode(id);
        
        // Enhance synaptic connections related to this episode
        if (brain_->getAssociativeMemory()) {
            // Convert episode to neural pattern
            std::vector<float> pattern;
            for (float activation : episode->neuronActivations) {
                pattern.push_back(activation);
            }
            
            // If we have resulting sensory state, associate with it
            if (!episode->resultingSensoryState.empty()) {
                brain_->getAssociativeMemory()->associateFromExperience(*episode);
            }
        }
        
        // Mark as consolidated
        em->updateRelevance(id, 0.5f * consolidationDepth_);
    }
    
    // Create slow consolidation entry
    pImpl->slowConsolidationBuffer.emplace_back(
        pImpl->brain->getCurrentStep(), consolidationDepth_);
    
    NLM_LOG_INFO("Slow consolidation completed. Consolidated " + 
                std::to_string(episodesToConsolidate.size()) + " episodes");
}

float SleepConsolidationSystem::calculateEpisodeImportance(const EpisodicMemoryItem* episode) const {
    if (!episode) return 0.0f;
    
    float importance = 0.0f;
    
    // Recency factor
    float recencyFactor = 1.0f / (episode->age + 1);
    importance += recencyFactor * 0.3f;
    
    // Reward factor
    float rewardFactor = std::min(1.0f, episode->reward / 10.0f);
    importance += rewardFactor * 0.3f;
    
    // Novelty factor
    float noveltyFactor = std::min(1.0f, episode->novelty / 5.0f);
    importance += noveltyFactor * 0.2f;
    
    // Energy factor
    float energyFactor = std::min(1.0f, episode->energy / 100.0f);
    importance += energyFactor * 0.2f;
    
    return importance;
}

} // namespace nlm