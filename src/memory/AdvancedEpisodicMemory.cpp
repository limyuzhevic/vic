#pragma once

#include "AdvancedEpisodicMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <set>

namespace nlm {

AdvancedEpisodicMemory::AdvancedEpisodicMemory()
    : brain_(nullptr)
    , maxEpisodes_(1000)
    , compressionThreshold_(0.7f)
    , averageRelevance_(0.0f)
    , compressionRatio_(1.0f)
    , nextReplayStep_(0)
    , lastConsolidationStep_(0)
{
    NLM_LOG_INFO("AdvancedEpisodicMemory initialized");
}

AdvancedEpisodicMemory::~AdvancedEpisodicMemory() {
    NLM_LOG_INFO("AdvancedEpisodicMemory destroyed");
}

void AdvancedEpisodicMemory::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize AdvancedEpisodicMemory with null brain");
        return;
    }
    brain_ = brain;
    NLM_LOG_INFO("AdvancedEpisodicMemory initialized with brain reference");
}

void AdvancedEpisodicMemory::storeExperience(const std::vector<float>& sensoryState,
                                            const std::vector<NeuronId>& activeNeurons,
                                            const std::vector<float>& neuronActivations,
                                            ActionType action,
                                            float reward,
                                            float novelty) {
    EnhancedEpisodicMemoryItem episode;
    episode.timestamp = 0; // Would be set by simulation
    episode.sensoryState = sensoryState;
    episode.activeNeurons = activeNeurons;

    for (float activation : neuronActivations) {
        episode.neuronActivations.push_back(activation);
    }

    episode.action = action;
    episode.reward = reward;
    episode.energy = reward * 0.5f; // Simplified energy calculation
    episode.novelty = novelty;
    episode.age = 0;
    episode.relevanceScore = computeInitialRelevance(episode);

    episodes_.push_back(episode);

    // Enforce capacity
    while (episodes_.size() > maxEpisodes_) {
        episodes_.pop_front();
    }

    NLM_LOG_DEBUG("Stored enhanced episodic memory with relevance: " + std::to_string(episode.relevanceScore));
}

std::vector<const EnhancedEpisodicMemoryItem*> AdvancedEpisodicMemory::retrieveSimilar(
    const std::vector<float>& sensoryPattern,
    float minRelevance,
    size_t maxResults) const {

    std::vector<std::pair<float, size_t>> scoredEpisodes;

    for (size_t i = 0; i < episodes_.size(); ++i) {
        const EnhancedEpisodicMemoryItem& episode = episodes_[i];

        // Compute similarity
        float similarity = computeSimilarity(sensoryPattern, episode);

        // Check temporal relevance
        float temporalRelevance = 1.0f / (episode.age + 1.0f);

        // Combine relevance
        float relevance = (similarity * 0.6f + temporalRelevance * 0.4f) * episode.relevanceScore;

        if (relevance >= minRelevance) {
            scoredEpisodes.emplace_back(relevance, i);
        }
    }

    // Sort by relevance (descending)
    std::sort(scoredEpisodes.begin(), scoredEpisodes.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });

    // Return top results
    std::vector<const EnhancedEpisodicMemoryItem*> results;
    for (size_t i = 0; i < std::min(maxResults, scoredEpisodes.size()); ++i) {
        results.push_back(&episodes_[scoredEpisodes[i].second]);
    }

    return results;
}

void AdvancedEpisodicMemory::analyzeTemporalGaps() {
    if (episodes_.size() < 2) {
        return;
    }

    SimulationStep lastStep = episodes_.front().timestamp;
    size_t gapCount = 0;
    std::vector<TemporalGap> newGaps;

    for (size_t i = 1; i < episodes_.size(); ++i) {
        SimulationStep currentStep = episodes_[i].timestamp;
        SimulationStep gap = currentStep - lastStep - 1;

        if (gap > 10) { // Significant gap
            gapCount++;

            TemporalGap gapInfo;
            gapInfo.startStep = lastStep;
            gapInfo.endStep = currentStep;
            gapInfo.compressedEpisodeCount = i - (gapCount - 1);
            gapInfo.temporalDensity = static_cast<float>(gapCount) / (gap + 1);

            newGaps.push_back(gapInfo);
        }

        lastStep = currentStep;
    }

    // Merge with existing gaps
    temporalGaps_.insert(temporalGaps_.end(), newGaps.begin(), newGaps.end());

    NLM_LOG_DEBUG("Analyzed " + std::to_string(gapCount) + " temporal gaps");
}

void AdvancedEpisodicMemory::compressTemporalGaps() {
    // Remove gaps with low density
    std::vector<TemporalGap> significantGaps;

    for (const auto& gap : temporalGaps_) {
        if (gap.temporalDensity > 0.5f) {
            significantGaps.push_back(gap);
        }
    }

    temporalGaps_ = significantGaps;

    // Compress episodes within significant gaps
    if (compressionThreshold_ > 0.0f) {
        std::vector<EnhancedEpisodicMemoryItem> compressedEpisodes;

        for (const auto& gap : temporalGaps_) {
            // Keep most relevant episodes from this gap
            size_t keepCount = std::max(static_cast<size_t>(2), static_cast<size_t>(gap.compressedEpisodeCount * compressionThreshold_));

            for (size_t i = gap.compressedEpisodeCount - keepCount; i < gap.compressedEpisodeCount; ++i) {
                if (i < episodes_.size()) {
                    compressedEpisodes.push_back(episodes_[i]);
                }
            }
        }

        // Remove old episodes and add compressed ones
        episodes_.clear();
        episodes_ = std::deque<EnhancedEpisodicMemoryItem>(compressedEpisodes.begin(), compressedEpisodes.end());
    }

    NLM_LOG_DEBUG("Compressed episodic memory, removed gaps: " + std::to_string(temporalGaps_.size()));
}

void AdvancedEpisodicMemory::applyForgetting(float forgettingRate) {
    for (auto& episode : episodes_) {
        episode.relevanceScore *= (1.0f - forgettingRate);
        episode.age++;

        // Decay based on age
        if (episode.age > 1000) {
            episode.relevanceScore *= 0.5f;
        }
    }

    NLM_LOG_DEBUG("Applied forgetting to episodic memory");
}

void AdvancedEpisodicMemory::ageAllEpisodes() {
    for (auto& episode : episodes_) {
        episode.age++;
    }
}

void AdvancedEpisodicMemory::updateRelevanceBasedOnPredictionError(float predictionError,
                                                                 float confidence) {
    float predictionRelevance = std::min(1.0f, predictionError * confidence);

    for (auto& episode : episodes_) {
        episode.relevanceScore += predictionRelevance * 0.1f;
        episode.relevanceScore = std::min(1.0f, episode.relevanceScore);
    }

    NLM_LOG_DEBUG("Updated episodic memory relevance based on prediction error");
}

void AdvancedEpisodicMemory::markImportantForConsolidation(const std::vector<size_t>& episodeIndices) {
    for (size_t index : episodeIndices) {
        if (index < episodes_.size()) {
            episodes_[index].consolidationPriority += 1.0f;
        }
    }

    NLM_LOG_DEBUG("Marked episodes for consolidation");
}

void AdvancedEpisodicMemory::buildTemporalIndex() {
    // Build temporal index for fast retrieval
    // Would implement more sophisticated indexing
}

void AdvancedEpisodicMemory::buildSensoryIndex() {
    // Build sensory index for fast similarity search
    // Would implement similarity index
}

void AdvancedEpisodicMemory::buildActionIndex() {
    // Build action index for fast action-based retrieval
    actionIndex_.clear();
    for (size_t i = 0; i < episodes_.size(); ++i) {
        actionIndex_[episodes_[i].action].push_back(i);
    }
}

void AdvancedEpisodicMemory::scheduleReplay(SimulationStep delaySteps) {
    for (size_t i = 0; i < episodes_.size(); ++i) {
        if (episodes_[i].consolidationPriority > 0.5f) {
            replayQueue_.push_back(i);
        }
    }

    NLM_LOG_DEBUG("Scheduled replay for " + std::to_string(replayQueue_.size()) + " episodes");
}

void AdvancedEpisodicMemory::replaySequence(const std::vector<size_t>& episodeIds) {
    for (size_t id : episodeIds) {
        if (id < episodes_.size()) {
            performRehearsal(id);

            // Small delay between replays
            // (In simulation, this would be steps)
        }
    }
}

void AdvancedEpisodicMemory::performRehearsal(size_t episodeIndex) {
    if (episodeIndex >= episodes_.size()) {
        return;
    }

    const EnhancedEpisodicMemoryItem& episode = episodes_[episodeIndex];

    // Reactivate neurons associated with this episode
    for (size_t i = 0; i < episode.activeNeurons.size(); ++i) {
        NeuronId neuron = episode.activeNeurons[i];
        float activation = i < episode.neuronActivations.size() ? 
                          episode.neuronActivations[i] : 0.5f;

        // Inject current to reawaken this pattern
        if (brain_) {
            brain_->injectCurrent(neuron, activation * 3.0f);
        }
    }

    NLM_LOG_DEBUG("Rehearsed episodic memory with priority: " + std::to_string(episode.consolidationPriority));
}

void AdvancedEpisodicMemory::consolidate(float relevanceThreshold) {
    std::vector<EnhancedEpisodicMemoryItem> consolidatedEpisodes;

    for (auto& episode : episodes_) {
        if (episode.relevanceScore >= relevanceThreshold) {
            consolidatedEpisodes.push_back(episode);
        }
    }

    // Update statistics
    float oldRelevanceSum = 0.0f;
    for (const auto& episode : episodes_) {
        oldRelevanceSum += episode.relevanceScore;
    }

    episodes_ = std::deque<EnhancedEpisodicMemoryItem>(consolidatedEpisodes.begin(), consolidatedEpisodes.end());

    if (episodes_.size() > 0) {
        averageRelevance_ = oldRelevanceSum / episodes_.size();
    }

    // Update compression ratio
    compressionRatio_ = static_cast<float>(episodes_.size()) / maxEpisodes_;

    lastConsolidationStep_ = 0; // Would be set by simulation

    NLM_LOG_DEBUG("Consolidated episodic memory, kept " + std::to_string(episodes_.size()) + " episodes");
}

void AdvancedEpisodicMemory::clear() {
    episodes_.clear();
    temporalGaps_.clear();
    replayQueue_.clear();

    averageRelevance_ = 0.0f;
    compressionRatio_ = 1.0f;

    NLM_LOG_INFO("AdvancedEpisodicMemory cleared");
}

float AdvancedEpisodicMemory::computeSimilarity(const std::vector<float>& query,
                                               const EnhancedEpisodicMemoryItem& episode) const {
    if (query.empty() || episode.sensoryState.empty()) return 0.0f;
    if (query.size() != episode.sensoryState.size()) return 0.0f;

    // Cosine similarity
    float dot = 0.0f, normQ = 0.0f, normE = 0.0f;
    for (size_t i = 0; i < query.size(); ++i) {
        dot += query[i] * episode.sensoryState[i];
        normQ += query[i] * query[i];
        normE += episode.sensoryState[i] * episode.sensoryState[i];
    }

    if (normQ < 0.0001f || normE < 0.0001f) return 0.0f;

    return dot / (std::sqrt(normQ) * std::sqrt(normE));
}

float AdvancedEpisodicMemory::computeInitialRelevance(const EnhancedEpisodicMemoryItem& episode) const {
    float relevance = 0.5f;

    // Base relevance on reward
    relevance += episode.reward * 0.3f;

    // Higher relevance for novel experiences
    relevance += episode.novelty * 0.2f;

    return std::min(1.0f, relevance);
}

} // namespace nlm