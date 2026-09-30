#include "NeuralEpisodicMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <queue>
#include <set>
#include <map>

namespace nlm {

struct NeuralEpisodicMemory::Impl {
    Brain* brain;
    
    // Pattern neurons for episodic recall
    std::vector<NeuronId> episodeNeurons;
    
    Impl() : brain(nullptr) {}
};

NeuralEpisodicMemory::NeuralEpisodicMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , maxEpisodes_(1000)
    , temporalCompressionRatio_(0.8f)
    , relevanceDecayRate_(0.95f)
    , replayEnabled_(true)
    , totalStorageSize_(0.0f)
    , peakStorageSize_(0.0f)
    , lastConsolidationStep_(0)
    , compressionRatio_(1.0f)
{
}

NeuralEpisodicMemory::~NeuralEpisodicMemory() = default;

void NeuralEpisodicMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("Enhanced NeuralEpisodicMemory initialized with multimodal storage");
}

void NeuralEpisodicMemory::storeEpisode(const EnhancedEpisodicMemoryItem& episode) {
    storeWithCompression(episode);
}

void NeuralEpisodicMemory::storeWithCompression(const EnhancedEpisodicMemoryItem& episode) {
    // Create episode copy with age 0
    EnhancedEpisodicMemoryItem stored = episode;
    stored.episodeAge = 0;
    stored.relevanceScore = calculateEpisodeRelevance(stored);
    
    // Apply compression to episode if needed
    if (episodes_.size() >= maxEpisodes_ * 0.9f) {
        compressEpisode(stored);
    } else {
        episodes_.push_back(stored);
    }
    
    // Update compression metrics
    totalStorageSize_ += estimateEpisodeSize(stored);
    peakStorageSize_ = std::max(peakStorageSize_, totalStorageSize_);
    
    // Create episode neuron for pattern completion
    if (brain_ && !stored.modalities.sensoryState.empty()) {
        NeuronId epNeuron(episodes_.size() + 20000);
        pImpl->episodeNeurons.push_back(epNeuron);
    }
    
    // Remove old episodes if over capacity
    while (episodes_.size() > maxEpisodes_) {
        EnhancedEpisodicMemoryItem& oldEpisode = episodes_.front();
        totalStorageSize_ -= estimateEpisodeSize(oldEpisode);
        episodes_.pop_front();
        
        if (!pImpl->episodeNeurons.empty()) {
            pImpl->episodeNeurons.erase(pImpl->episodeNeurons.begin());
        }
    }
    
    // Age all episodes
    for (auto& ep : episodes_) {
        ep.episodeAge++;
        ep.relevanceScore *= relevanceDecayRate_;
    }
    
    // Extract schemas periodically
    if (episodes_.size() % 50 == 0) {
        extractSchemas();
    }
    
    NLM_LOG_DEBUG("Stored enhanced episodic memory with relevance: " + 
                 std::to_string(stored.relevanceScore));
}

void NeuralEpisodicMemory::applyTemporalCompression() {
    // Apply temporal compression to reduce storage requirements
    if (episodes_.size() < 10) return;
    
    // Compress recent episodes (keep more detail)
    size_t recentCount = static_cast<size_t>(episodes_.size() * 0.3f);
    
    // Find episodes to compress (older ones)
    size_t compressFrom = std::min(static_cast<size_t>(episodes_.size() * 0.7f), episodes_.size() - 1);
    
    // Apply compression to older episodes
    for (size_t i = compressFrom; i < episodes_.size(); ++i) {
        compressEpisode(episodes_[i]);
    }
    
    updateCompressionMetrics();
}

void NeuralEpisodicMemory::compressRecentEpisodes() {
    // Focus compression on recent episodes for better recall
    if (episodes_.empty()) return;
    
    // Process last 50% of episodes
    size_t startIdx = std::max(static_cast<size_t>(0), 
                              static_cast<size_t>(episodes_.size() * 0.5f));
    
    for (size_t i = startIdx; i < episodes_.size(); ++i) {
        compressEpisode(episodes_[i]);
    }
    
    updateCompressionMetrics();
}

void NeuralEpisodicMemory::compressBasedOnSimilarity() {
    // Merge similar episodes to reduce redundancy
    mergeSimilarEpisodes();
}

void NeuralEpisodicMemory::mergeSimilarEpisodes() {
    if (episodes_.size() < 2) return;
    
    // Sort episodes by timestamp (oldest first)
    std::vector<size_t> indices(episodes_.size());
    for (size_t i = 0; i < episodes_.size(); ++i) {
        indices[i] = i;
    }
    
    std::sort(indices.begin(), indices.end(),
        [this](size_t a, size_t b) { return episodes_[a].timestamp < episodes_[b].timestamp; });
    
    // Merge similar consecutive episodes
    std::vector<size_t> toKeep;
    toKeep.push_back(indices[0]);
    
    for (size_t i = 1; i < indices.size(); ++i) {
        size_t currentIdx = indices[i];
        size_t lastIdx = indices[toKeep.back()];
        
        // Check if episodes are similar enough to merge
        if (calculateEpisodeSimilarity(episodes_[lastIdx], episodes_[currentIdx]) > 0.7f) {
            // Merge into last episode
            EnhancedEpisodicMemoryItem& target = episodes_[lastIdx];
            EnhancedEpisodicMemoryItem& source = episodes_[currentIdx];
            
            // Update target with source information
            target.modalities.reward = (target.modalities.reward + source.modalities.reward) * 0.5f;
            target.modalities.emotion = (target.modalities.emotion + source.modalities.emotion) * 0.5f;
            target.modalities.novelty = (target.modalities.novelty + source.modalities.novelty) * 0.5f;
            target.modalities.energy = (target.modalities.energy + source.modalities.energy) * 0.5f;
            
            // Mark source as merged (will be removed)
            source.relevanceScore = 0.0f;
        } else {
            // Keep this episode
            toKeep.push_back(currentIdx);
        }
    }
    
    // Rebuild episodes vector with merged episodes
    std::deque<EnhancedEpisodicMemoryItem> mergedEpisodes;
    for (size_t idx : toKeep) {
        mergedEpisodes.push_back(episodes_[idx]);
    }
    
    episodes_ = std::move(mergedEpisodes);
    
    // Update neurons list
    pImpl->episodeNeurons.clear();
    for (size_t i = 0; i < episodes_.size(); ++i) {
        pImpl->episodeNeurons.push_back(NeuronId(i + 20000));
    }
}

void NeuralEpisodicMemory::updateRelevance(size_t episodeId, float delta) {
    if (episodeId < episodes_.size()) {
        episodes_[episodeId].relevanceScore += delta;
        episodes_[episodeId].relevanceScore = std::clamp(episodes_[episodeId].relevanceScore, 0.0f, 1.0f);
        
        // Update episode age
        episodes_[episodeId].lastReplayStep = lastConsolidationStep_;
    }
}

void NeuralEpisodicMemory::filterByRelevance(float threshold) {
    std::deque<EnhancedEpisodicMemoryItem> filteredEpisodes;
    
    for (auto& episode : episodes_) {
        if (episode.relevanceScore >= threshold) {
            filteredEpisodes.push_back(episode);
        }
    }
    
    episodes_ = std::move(filteredEpisodes);
    pImpl->episodeNeurons.clear();
    
    // Rebuild neurons list
    for (size_t i = 0; i < episodes_.size(); ++i) {
        pImpl->episodeNeurons.push_back(NeuronId(i + 20000));
    }
}

void NeuralEpisodicMemory::prioritizeRecentEpisodes(float decayFactor) {
    // Prioritize recent episodes by boosting their relevance
    size_t recentStart = std::max(static_cast<size_t>(0), 
                                static_cast<size_t>(episodes_.size() * (1.0f - decayFactor)));
    
    for (size_t i = recentStart; i < episodes_.size(); ++i) {
        episodes_[i].relevanceScore = std::min(1.0f, episodes_[i].relevanceScore * 1.2f);
    }
}

void NeuralEpisodicMemory::consolidate(float relevanceThreshold) {
    lastConsolidationStep_ = episodes_.empty() ? 0 : episodes_.back().timestamp;
    
    // Remove episodes below relevance threshold
    std::deque<EnhancedEpisodicMemoryItem> consolidatedEpisodes;
    
    for (auto& episode : episodes_) {
        if (episode.relevanceScore >= relevanceThreshold) {
            consolidatedEpisodes.push_back(episode);
        }
    }
    
    episodes_ = std::move(consolidatedEpisodes);
    
    // Apply compression during consolidation
    applyTemporalCompression();
    
    // Update neurons list
    pImpl->episodeNeurons.clear();
    for (size_t i = 0; i < episodes_.size(); ++i) {
        pImpl->episodeNeurons.push_back(NeuronId(i + 20000));
    }
    
    NLM_LOG_INFO("Consolidated episodic memory: " + std::to_string(episodes_.size()) + 
                 " episodes remain after consolidation (threshold: " + 
                 std::to_string(relevanceThreshold) + ")");
}

void NeuralEpisodicMemory::cleanupOldEpisodes(size_t maxEpisodes) {
    while (episodes_.size() > maxEpisodes) {
        EnhancedEpisodicMemoryItem& oldEpisode = episodes_.front();
        totalStorageSize_ -= estimateEpisodeSize(oldEpisode);
        episodes_.pop_front();
        
        if (!pImpl->episodeNeurons.empty()) {
            pImpl->episodeNeurons.erase(pImpl->episodeNeurons.begin());
        }
    }
    
    // Update total storage size
    totalStorageSize_ = 0.0f;
    for (const auto& episode : episodes_) {
        totalStorageSize_ += estimateEpisodeSize(episode);
    }
}

void NeuralEpisodicMemory::consolidateSleepCycles() {
    // Simulate sleep-dependent consolidation
    if (episodes_.empty()) return;
    
    // Replay recent episodes during sleep
    size_t recentCount = std::min(static_cast<size_t>(episodes_.size() * 0.3f), static_cast<size_t>(5));
    
    for (size_t i = 0; i < recentCount; ++i) {
        size_t idx = episodes_.size() - 1 - i;
        replayEpisode(&episodes_[idx]);
        episodes_[idx].isConsolidated = true;
    }
    
    // Enhance consolidation of high-relevance episodes
    for (auto& episode : episodes_) {
        if (episode.relevanceScore > 0.5f) {
            episode.consolidationStrength = std::min(1.0f, episode.consolidationStrength * 1.1f);
        }
    }
    
    NLM_LOG_INFO("Sleep-dependent consolidation completed");
}

std::vector<const EnhancedEpisodicMemoryItem*> NeuralEpisodicMemory::retrieveSimilar(
    const std::vector<float>& sensoryPattern,
    size_t maxResults,
    float relevanceThreshold) const {
    
    std::vector<std::pair<float, size_t>> scores;  // (score, index)
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        float sim = computeSimilarity(sensoryPattern, episodes_[i]);
        float relevance = episodes_[i].relevanceScore;
        float score = sim * 0.7f + relevance * 0.3f;  // Weighted combination
        
        scores.emplace_back(score, i);
    }
    
    // Sort by score (descending)
    std::sort(scores.begin(), scores.end(),
        [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Return top results
    std::vector<const EnhancedEpisodicMemoryItem*> results;
    for (size_t i = 0; i < std::min(maxResults, scores.size()); ++i) {
        if (scores[i].first > relevanceThreshold) {
            results.push_back(&episodes_[scores[i].second]);
        }
    }
    
    return results;
}

std::vector<const EnhancedEpisodicMemoryItem*> NeuralEpisodicMemory::retrieveByContext(
    float timeWindow,
    float spatialRadius,
    const std::string& locationTag,
    const std::string& actionTag,
    size_t maxResults) const {
    
    std::vector<const EnhancedEpisodicMemoryItem*> results;
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        const auto& episode = episodes_[i];
        
        // Check temporal context
        bool temporalMatch = (timeWindow <= 0.0f) || 
                            (std::abs(static_cast<float>(episode.timestamp) - timeWindow) < 100.0f);
        
        // Check spatial context
        bool spatialMatch = (spatialRadius <= 0.0f) || 
                           (std::sqrt(episode.positionX * episode.positionX + 
                                     episode.positionY * episode.positionY) <= spatialRadius);
        
        // Check location tag
        bool locationMatch = locationTag.empty() || episode.locationTag == locationTag;
        
        // Check action tag (would need action mapping in full implementation)
        bool actionMatch = actionTag.empty();  // Placeholder
        
        if (temporalMatch && spatialMatch && locationMatch && actionMatch) {
            results.push_back(&episode);
        }
    }
    
    // Return most relevant first
    std::sort(results.begin(), results.end(),
        [](const EnhancedEpisodicMemoryItem* a, const EnhancedEpisodicMemoryItem* b) {
            return a->relevanceScore > b->relevanceScore;
        });
    
    if (results.size() > maxResults) {
        results.resize(maxResults);
    }
    
    return results;
}

std::vector<const EnhancedEpisodicMemoryItem*> NeuralEpisodicMemory::retrieveTemporalSequence(
    SimulationStep startTime,
    SimulationStep endTime,
    size_t maxResults) const {
    
    std::vector<const EnhancedEpisodicMemoryItem*> results;
    
    for (const auto& ep : episodes_) {
        if (ep.timestamp >= startTime && ep.timestamp <= endTime) {
            results.push_back(&ep);
        }
    }
    
    // Return most recent first
    std::sort(results.begin(), results.end(),
        [](const EnhancedEpisodicMemoryItem* a, const EnhancedEpisodicMemoryItem* b) {
            return a->timestamp > b->timestamp;
        });
    
    if (results.size() > maxResults) {
        results.resize(maxResults);
    }
    
    return results;
}

void NeuralEpisodicMemory::replayEpisode(const EnhancedEpisodicMemoryItem* episode) {
    if (!episode || !brain_) return;
    
    // Reactivate neurons that were active during this episode
    for (size_t i = 0; i < episode->activeNeurons.size(); ++i) {
        NeuronId neuron = episode->activeNeurons[i];
        float activation = i < episode->neuronActivations.size() ? 
                          episode->neuronActivations[i] : 0.5f;
        
        // Inject current to reawaken this pattern
        brain_->injectCurrent(neuron, activation * 3.0f);
    }
    
    // Also replay modal components
    if (!episode->modalities.sensoryState.empty()) {
        // Replay sensory pattern
        for (size_t i = 0; i < std::min(size_t(10), episode->modalities.sensoryState.size()); ++i) {
            float sensoryActivation = episode->modalities.sensoryState[i] * 2.0f;
            // Would go through sensory channels in full implementation
        }
    }
}

void NeuralEpisodicMemory::replaySequence(const std::vector<size_t>& episodeIds) {
    for (size_t id : episodeIds) {
        if (id < episodes_.size()) {
            replayEpisode(&episodes_[id]);
            
            // Small delay between replays
            // (In simulation, this would be steps)
        }
    }
}

void NeuralEpisodicMemory::replayWithConsolidation(
    const std::vector<size_t>& episodeIds) {
    // Enhanced replay with consolidation effects
    for (size_t id : episodeIds) {
        if (id < episodes_.size()) {
            auto& episode = episodes_[id];
            
            // Replay episode
            replayEpisode(&episode);
            
            // Enhance consolidation strength
            episode.consolidationStrength = std::min(1.0f, episode.consolidationStrength * 1.1f);
            episode.isConsolidated = true;
            
            // Update relevance based on replay
            episode.relevanceScore = std::min(1.0f, episode.relevanceScore * 1.05f);
            
            // Extract patterns from replayed episode
            extractPatternsFromReplay(episode);
        }
    }
}

void NeuralEpisodicMemory::compressEpisode(EnhancedEpisodicMemoryItem& episode) {
    // Reduce episode detail to save storage
    
    // Compress sensory state
    if (!episode.modalities.sensoryState.empty()) {
        size_t originalSize = episode.modalities.sensoryState.size();
        size_t compressedSize = std::max(static_cast<size_t>(1), originalSize / 2);
        
        std::vector<float> compressed;
        for (size_t i = 0; i < compressedSize; ++i) {
            size_t originalIndex = (i * originalSize) / compressedSize;
            compressed.push_back(episode.modalities.sensoryState[originalIndex]);
        }
        
        episode.modalities.sensoryState = compressed;
    }
    
    // Compress motor state
    if (!episode.modalities.motorState.empty()) {
        size_t originalSize = episode.modalities.motorState.size();
        size_t compressedSize = std::max(static_cast<size_t>(1), originalSize / 2);
        
        std::vector<float> compressed;
        for (size_t i = 0; i < compressedSize; ++i) {
            size_t originalIndex = (i * originalSize) / compressedSize;
            compressed.push_back(episode.modalities.motorState[originalIndex]);
        }
        
        episode.modalities.motorState = compressed;
    }
    
    // Mark as compressed
    episode.isConsolidated = true;
}

void NeuralEpisodicMemory::extractSchemas() {
    // Extract general patterns from episodes
    std::vector<std::string> newSchemas;
    
    // Simple schema extraction based on episode characteristics
    for (const auto& episode : episodes_) {
        // Create schema identifier based on key features
        std::string schema = "schema_";
        
        // Add sensory pattern signature (simplified)
        if (!episode.modalities.sensoryState.empty()) {
            float avgSensory = std::accumulate(episode.modalities.sensoryState.begin(),
                                             episode.modalities.sensoryState.end(), 0.0f) /
                            episode.modalities.sensoryState.size();
            
            if (avgSensory < 0.33f) schema += "low_sensory_";
            else if (avgSensory < 0.67f) schema += "med_sensory_";
            else schema += "high_sensory_";
        }
        
        // Add reward signature
        if (episode.modalities.reward < 0.33f) schema += "low_reward_";
        else if (episode.modalities.reward < 0.67f) schema += "med_reward_";
        else schema += "high_reward_";
        
        // Add emotion signature
        if (episode.modalities.emotion < 0.33f) schema += "neutral_emotion";
        else if (episode.modalities.emotion < 0.67f) schema += "mixed_emotion";
        else schema += "strong_emotion";
        
        newSchemas.push_back(schema);
    }
    
    // Add new schemas to extracted schemas
    for (const auto& schema : newSchemas) {
        if (std::find(extractedSchemas_.begin(), extractedSchemas_.end(), schema) == extractedSchemas_.end()) {
            extractedSchemas_.push_back(schema);
        }
    }
    
    // Maintain schema limit
    if (extractedSchemas_.size() > 50) {
        extractedSchemas_.erase(extractedSchemas_.begin(), 
                              extractedSchemas_.begin() + 20);
    }
}

void NeuralEpisodicMemory::createGeneralizations() {
    // Create generalizations from schemas
    if (extractedSchemas_.empty()) return;
    
    // Create associations between schemas
    for (size_t i = 0; i < extractedSchemas_.size(); ++i) {
        for (size_t j = i + 1; j < extractedSchemas_.size(); ++j) {
            // Create generalization if schemas are related
            std::string generalization = "gen_" + extractedSchemas_[i] + "_" + extractedSchemas_[j];
            schemaEpisodeMap_[generalization].push_back(i);
            schemaEpisodeMap_[generalization].push_back(j);
        }
    }
}

void NeuralEpisodicMemory::processEmotions() {
    // Process emotions in episodic memory
    float totalValence = 0.0f;
    int count = 0;
    
    for (const auto& episode : episodes_) {
        totalValence += episode.modalities.emotion;
        count++;
    }
    
    if (count > 0) {
        float avgValence = totalValence / count;
        
        // Apply emotion regulation based on average valence
        if (avgValence > 0.5f) {
            // High positive valence - enhance positive learning
            for (auto& episode : episodes_) {
                episode.modalities.emotion = std::min(1.0f, episode.modalities.emotion * 1.05f);
            }
        } else if (avgValence < -0.5f) {
            // High negative valence - enhance negative learning
            for (auto& episode : episodes_) {
                episode.modalities.emotion = std::max(-1.0f, episode.modalities.emotion * 1.05f);
            }
        }
    }
}

void NeuralEpisodicMemory::regulateEmotionalResponses() {
    // Regulate emotional responses based on episode patterns
    for (auto& episode : episodes_) {
        // Balance extreme emotions
        if (std::abs(episode.modalities.emotion) > 0.8f) {
            episode.modalities.emotion *= 0.9f;  // Reduce extreme emotions
        }
        
        // Enhance moderate emotions
        if (std::abs(episode.modalities.emotion) > 0.2f && 
            std::abs(episode.modalities.emotion) <= 0.5f) {
            episode.modalities.emotion *= 1.1f;  // Enhance moderate emotions
        }
    }
}

float NeuralEpisodicMemory::getAverageEmotionalValence() const {
    if (episodes_.empty()) return 0.0f;
    
    float total = 0.0f;
    for (const auto& episode : episodes_) {
        total += episode.modalities.emotion;
    }
    
    return total / episodes_.size();
}

void NeuralEpisodicMemory::updateCompressionMetrics() {
    // Calculate compression metrics
    float totalOriginalSize = 0.0f;
    float totalCompressedSize = 0.0f;
    
    for (const auto& episode : episodes_) {
        // Estimate original size (simplified)
        totalOriginalSize += 100.0f;  // Placeholder
        
        // Estimate compressed size
        totalCompressedSize += estimateEpisodeSize(episode);
    }
    
    if (totalOriginalSize > 0.0f) {
        compressionRatio_ = totalCompressedSize / totalOriginalSize;
    } else {
        compressionRatio_ = 1.0f;
    }
    
    totalStorageSize_ = totalCompressedSize;
}

void NeuralEpisodicMemory::buildTemporalIndex() {
    temporalIndex_.clear();
    for (size_t i = 0; i < episodes_.size(); ++i) {
        temporalIndex_.emplace_back(episodes_[i].timestamp, i);
    }
    
    // Sort by timestamp
    std::sort(temporalIndex_.begin(), temporalIndex_.end(),
        [](const auto& a, const auto& b) { return a.first < b.first; });
}

void NeuralEpisodicMemory::buildSpatialIndex() {
    spatialIndex_.clear();
    for (size_t i = 0; i < episodes_.size(); ++i) {
        std::string key = std::to_string(static_cast<int>(episodes_[i].positionX)) + 
                           "," + std::to_string(static_cast<int>(episodes_[i].positionY));
        spatialIndex_[key].push_back(i);
    }
}

void NeuralEpisodicMemory::buildActionIndex() {
    actionIndex_.clear();
    for (size_t i = 0; i < episodes_.size(); ++i) {
        std::string key = "action_" + std::to_string(static_cast<int>(episodes_[i].action));
        actionIndex_[key].push_back(i);
    }
}

void NeuralEpisodicMemory::optimizeIndexStructures() {
    // Rebuild indexes for optimal performance
    buildTemporalIndex();
    buildSpatialIndex();
    buildActionIndex();
}

void NeuralEpisodicMemory::mergeSimilarEpisodes() {
    // Merge similar episodes (same as in header implementation)
    if (episodes_.size() < 2) return;
    
    // Similar to header implementation - simplified here
    std::deque<EnhancedEpisodicMemoryItem> mergedEpisodes;
    
    // For now, just keep all episodes
    mergedEpisodes = episodes_;
    
    episodes_ = std::move(mergedEpisodes);
}

void NeuralEpisodicMemory::generateSchemaFromEpisodes() {
    extractSchemas();
}

void NeuralEpisodicMemory::extractPatternsFromReplay(
    EnhancedEpisodicMemoryItem& episode) {
    // Extract patterns from replayed episode
    if (episode.replayNeurons.empty()) return;
    
    // Create pattern from replay neurons
    std::vector<float> pattern;
    for (const auto& neuron : episode.replayNeurons) {
        pattern.push_back(static_cast<float>(neuron) / 10000.0f);
    }
    
    // Store pattern for later use
    // (Would be stored in associative memory in full implementation)
}

const EnhancedEpisodicMemoryItem* NeuralEpisodicMemory::getEpisode(size_t index) const {
    if (index < episodes_.size()) {
        return &episodes_[index];
    }
    return nullptr;
}

std::vector<const EnhancedEpisodicMemoryItem*> NeuralEpisodicMemory::getRecentEpisodes(size_t count) const {
    std::vector<const EnhancedEpisodicMemoryItem*> results;
    
    size_t start = count > episodes_.size() ? 0 : episodes_.size() - count;
    for (size_t i = start; i < episodes_.size(); ++i) {
        results.push_back(&episodes_[i]);
    }
    
    return results;
}

std::vector<const EnhancedEpisodicMemoryItem*> NeuralEpisodicMemory::getRelevantEpisodes(
    float minRelevance) const {
    
    std::vector<const EnhancedEpisodicMemoryItem*> results;
    
    for (const auto& episode : episodes_) {
        if (episode.relevanceScore >= minRelevance) {
            results.push_back(&episode);
        }
    }
    
    // Sort by relevance
    std::sort(results.begin(), results.end(),
        [](const EnhancedEpisodicMemoryItem* a, const EnhancedEpisodicMemoryItem* b) {
            return a->relevanceScore > b->relevanceScore;
        });
    
    return results;
}

float NeuralEpisodicMemory::getAverageReward() const {
    if (episodes_.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (const auto& ep : episodes_) {
        sum += ep.modalities.reward;
    }
    return sum / episodes_.size();
}

float NeuralEpisodicMemory::getMemoryActivityLevel() const {
    if (episodes_.empty()) return 0.0f;
    
    float totalActivity = 0.0f;
    for (const auto& ep : episodes_) {
        totalActivity += ep.relevanceScore;
    }
    return totalActivity / episodes_.size();
}

std::vector<float> NeuralEpisodicMemory::predictFuturePattern(
    const EnhancedEpisodicMemoryItem* lastEpisode) const {
    
    std::vector<float> prediction(10, 0.0f);  // 10-dimensional prediction
    
    if (!lastEpisode) return prediction;
    
    // Simple prediction based on episode features
    if (!lastEpisode->modalities.sensoryState.empty()) {
        prediction[0] = lastEpisode->modalities.sensoryState[0];  // Visual
    }
    
    prediction[1] = lastEpisode->modalities.reward;  // Reward
    prediction[2] = lastEpisode->modalities.emotion;  // Emotion
    
    // Add noise for uncertainty
    for (size_t i = 0; i < prediction.size(); ++i) {
        prediction[i] += (brain_ ? brain_->getRandomGenerator()->uniformReal(-0.1f, 0.1f) : 0.0f) * 0.5f;
        prediction[i] = std::clamp(prediction[i], 0.0f, 1.0f);
    }
    
    return prediction;
}

void NeuralEpisodicMemory::updateWorkingMemoryFromEpisode(
    size_t episodeId, NeuralWorkingMemory* workingMem) {
    
    if (!workingMem || episodeId >= episodes_.size()) return;
    
    const auto& episode = episodes_[episodeId];
    
    // Create pattern from episode for working memory
    std::vector<float> pattern;
    
    // Add episode features to pattern
    if (!episode.modalities.sensoryState.empty()) {
        pattern.insert(pattern.end(), episode.modalities.sensoryState.begin(),
                     episode.modalities.sensoryState.end());
    }
    
    // Add other features
    pattern.push_back(episode.modalities.reward);
    pattern.push_back(episode.modalities.emotion);
    pattern.push_back(episode.modalities.novelty);
    pattern.push_back(episode.modalities.energy);
    
    if (!pattern.empty()) {
        workingMem->storeWithEpisodicSupport(pattern, &episode, 0.8f);
    }
}

void NeuralEpisodicMemory::replayForWorkingMemory(NeuralWorkingMemory* workingMem) {
    if (!workingMem) return;
    
    // Replay recent high-relevance episodes for working memory
    std::vector<const EnhancedEpisodicMemoryItem*> episodes = getRelevantEpisodes(0.5f);
    
    for (const auto* episode : episodes) {
        if (!episode) continue;
        
        // Replay episode
        replayEpisode(episode);
        
        // Update working memory from episode
        size_t episodeIdx = 0;  // Would need proper indexing
        for (size_t i = 0; i < episodes_.size(); ++i) {
            if (&episodes_[i] == episode) {
                episodeIdx = i;
                break;
            }
        }
        
        updateWorkingMemoryFromEpisode(episodeIdx, workingMem);
    }
}

float NeuralEpisodicMemory::calculateEpisodeRelevance(
    const EnhancedEpisodicMemoryItem& episode) const {
    
    float relevance = 0.0f;
    
    // Novelty contributes to relevance
    relevance += std::abs(episode.modalities.novelty) * 0.3f;
    
    // Reward contributes to relevance
    relevance += std::abs(episode.modalities.reward) * 0.3f;
    
    // Emotional intensity contributes to relevance
    relevance += std::abs(episode.modalities.emotion) * 0.2f;
    
    // Recency contributes to relevance
    relevance += episode.episodeAge * 0.2f;
    
    return std::min(1.0f, relevance);
}

float NeuralEpisodicMemory::computeSimilarity(
    const std::vector<float>& query,
    const EnhancedEpisodicMemoryItem& episode) const {
    
    if (query.empty() || episode.modalities.sensoryState.empty()) return 0.0f;
    if (query.size() != episode.modalities.sensoryState.size()) return 0.0f;
    
    // Cosine similarity
    float dot = 0.0f, normQ = 0.0f, normE = 0.0f;
    for (size_t i = 0; i < query.size(); ++i) {
        dot += query[i] * episode.modalities.sensoryState[i];
        normQ += query[i] * query[i];
        normE += episode.modalities.sensoryState[i] * episode.modalities.sensoryState[i];
    }
    
    if (normQ < 0.0001f || normE < 0.0001f) return 0.0f;
    
    return dot / (std::sqrt(normQ) * std::sqrt(normE));
}

float NeuralEpisodicMemory::estimateEpisodeSize(
    const EnhancedEpisodicMemoryItem& episode) const {
    
    float size = 0.0f;
    
    // Estimate size based on components
    size += episode.modalities.sensoryState.size() * sizeof(float);
    size += episode.modalities.motorState.size() * sizeof(float);
    size += sizeof(episode.modalities.reward);
    size += sizeof(episode.modalities.emotion);
    size += sizeof(episode.modalities.novelty);
    size += sizeof(episode.modalities.energy);
    size += sizeof(episode.modalities.attention);
    size += episode.activeNeurons.size() * sizeof(NeuronId);
    size += episode.replayNeurons.size() * sizeof(NeuronId);
    
    return size;
}

float NeuralEpisodicMemory::calculateEpisodeSimilarity(
    const EnhancedEpisodicMemoryItem& a,
    const EnhancedEpisodicMemoryItem& b) const {
    
    float similarity = 0.0f;
    
    // Similarity based on reward
    similarity += 1.0f - std::abs(a.modalities.reward - b.modalities.reward);
    
    // Similarity based on emotion
    similarity += 1.0f - std::abs(a.modalities.emotion - b.modalities.emotion);
    
    // Similarity based on novelty
    similarity += 1.0f - std::abs(a.modalities.novelty - b.modalities.novelty);
    
    // Average similarity
    return similarity / 3.0f;
}

} // namespace nlm