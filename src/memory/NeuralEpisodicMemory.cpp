// NeuralEpisodicMemory.cpp - Enhanced Neural Episodic Memory Implementation

#include "NeuralEpisodicMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Types/Types.hpp"
#include "../memory/MemorySystemIntegration.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <chrono>
#include <thread>

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
    , replayEnabled_(true)
    , replayCount_(0)
    , consolidationCount_(0)
    , compressionCount_(0)
    , totalReplayTime_(0.0)
{
    NLM_LOG_INFO("NeuralEpisodicMemory: Created enhanced episodic memory system");
}

NeuralEpisodicMemory::~NeuralEpisodicMemory() {
    NLM_LOG_INFO("NeuralEpisodicMemory: Destroyed enhanced episodic memory system");
}

void NeuralEpisodicMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    // Initialize enhanced episodic memory components
    setupEnhancedIndexing();
    setupCompressionSystem();
    
    NLM_LOG_INFO("NeuralEpisodicMemory: Initialized with brain reference and enhanced systems");
}

void NeuralEpisodicMemory::setupEnhancedIndexing() {
    // Set up O(log n) indexing for fast retrieval
    index_.clear();
    patternVectors_.clear();
    cache_.clear();
    highPriorityCache_.clear();
    evictedIndices_.clear();
    
    NLM_LOG_INFO("NeuralEpisodicMemory: Enhanced indexing system initialized");
}

void NeuralEpisodicMemory::setupCompressionSystem() {
    // Set up memory compression for storage efficiency
    compressedEpisodes_.clear();
    episodeMap_.clear();
    
    NLM_LOG_INFO("NeuralEpisodicMemory: Compression system initialized");
}

void NeuralEpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    // Create episode copy with age 0
    EpisodicMemoryItem stored = episode;
    stored.age = 0;
    
    episodes_.push_back(stored);
    
    // Create episode neuron for pattern completion
    if (brain_ && !episode.sensoryState.empty()) {
        NeuronId epNeuron(episodes_.size() + 20000);
        pImpl->episodeNeurons.push_back(epNeuron);
        
        // Store episode index in the neuron
        // (This is a simplified approach - real implementation would use more distributed encoding)
    }
    
    // Index the episode for fast retrieval
    if (config_.enableIndexing) {
        indexEpisode(stored);
    }
    
    // Compress the episode if enabled
    if (config_.enableCompression) {
        compressEpisode(stored);
    }
    
    // Remove old episodes if over capacity
    while (episodes_.size() > maxEpisodes_) {
        size_t index = 0;
        
        // Remove from main arrays
        episodes_.erase(episodes_.begin());
        if (!pImpl->episodeNeurons.empty()) {
            pImpl->episodeNeurons.erase(pImpl->episodeNeurons.begin());
        }
        
        // Remove from index
        if (index < index_.size()) {
            index_.erase(index_.begin());
        }
        
        // Remove from compressed episodes
        if (index < compressedEpisodes_.size()) {
            compressedEpisodes_.erase(compressedEpisodes_.begin());
        }
        
        // Remove from cache if present
        if (index < index_.size()) {
            cache_.erase(index);
        }
    }
    
    // Age all episodes
    for (auto& ep : episodes_) {
        ep.age++;
    }
}

void NeuralEpisodicMemory::indexEpisode(const EpisodicMemoryItem& episode) {
    // Create index entry for fast O(log n) access
    MemoryIndexEntry indexEntry(
        episode.timestamp,
        episode.novelty,
        episode.novelty,  // Use novelty as importance proxy
        episodeMap_.size(),
        1.0f  // Default relevance score
    );
    
    index_.push_back(indexEntry);
    
    // Store pattern vector for similarity search
    patternVectors_.push_back(episode.sensoryState);
    
    // Update episode map
    episodeMap_[episodeMap_.size()] = episodeMap_.size();
    
    // Cache frequently accessed episodes
    if (config_.enableCaching) {
        cacheEpisode(episodeMap_.size() - 1);
    }
}

void NeuralEpisodicMemory::compressEpisode(const EpisodicMemoryItem& episode) {
    // Compress episode for storage efficiency
    std::vector<float> compressed = compressPattern(episode.sensoryState);
    
    compressedEpisodes_.push_back(compressed);
    
    // Also compress neuron activations
    std::vector<float> compressedActivations;
    for (float activation : episode.neuronActivations) {
        compressedActivations.push_back(activation * 0.5f);  // Simple compression
    }
    
    compressedEpisodes_.push_back(compressedActivations);
}

void NeuralEpisodicMemory::cacheEpisode(size_t episodeIndex, bool highPriority) {
    if (cache_.size() >= config_.cacheSize) {
        // Remove least recently used entries
        size_t toRemove = config_.cacheSize / 2;
        auto it = cache_.begin();
        for (size_t i = 0; i < toRemove && it != cache_.end(); ++i, ++it) {
            if (highPriority) {
                highPriorityCache_.erase(std::next(cache_.begin(), i));
            }
        }
        cache_.erase(cache_.begin(), std::next(cache_.begin(), toRemove));
    }
    
    // Add to cache
    cache_[episodeIndex] = getPatternFromIndex(episodeIndex);
    
    if (highPriority) {
        highPriorityCache_.insert(episodeIndex);
    }
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveSimilar(
    const std::vector<float>& sensoryPattern,
    size_t maxResults) const {
    
    std::vector<std::pair<float, size_t>> scores;  // (similarity, index)
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        float sim = computeSimilarity(sensoryPattern, episodes_[i]);
        scores.emplace_back(sim, i);
    }
    
    // Sort by similarity (descending)
    std::sort(scores.begin(), scores.end(),
        [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Return top results
    std::vector<const EpisodicMemoryItem*> results;
    for (size_t i = 0; i < std::min(maxResults, scores.size()); ++i) {
        if (scores[i].first > 0.3f) {  // Threshold
            results.push_back(&episodes_[scores[i].second]);
        }
    }
    
    return results;
}

std::vector<size_t> NeuralEpisodicMemory::searchSimilar(
    const std::vector<float>& sensoryPattern,
    size_t maxResults) const {
    
    std::vector<std::pair<float, size_t>> scores;
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        float sim = computeSimilarity(sensoryPattern, episodes_[i]);
        scores.emplace_back(sim, i);
    }
    
    // Sort by similarity (descending)
    std::sort(scores.begin(), scores.end(),
        [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Extract top results
    std::vector<size_t> results;
    for (size_t i = 0; i < std::min(maxResults, scores.size()); ++i) {
        results.push_back(scores[i].second);
    }
    
    return results;
}

std::vector<size_t> NeuralEpisodicMemory::searchTemporal(
    SimulationStep startTime,
    SimulationStep endTime,
    size_t maxResults) const {
    
    std::vector<size_t> results;
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        if (episodes_[i].timestamp >= startTime && episodes_[i].timestamp <= endTime) {
            results.push_back(i);
        }
    }
    
    return results;
}

std::vector<size_t> NeuralEpisodicMemory::searchByLocation(
    float x, float y,
    float radius,
    size_t maxResults) const {
    
    std::vector<std::pair<float, size_t>> distances;  // (distance, index)
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        float dx = episodes_[i].positionX - x;
        float dy = episodes_[i].positionY - y;
        float dist = std::sqrt(dx * dx + dy * dy);
        
        if (dist <= radius) {
            distances.emplace_back(dist, i);
        }
    }
    
    // Sort by distance
    std::sort(distances.begin(), distances.end(),
        [](const auto& a, const auto& b) { return a.first < b.first; });
    
    std::vector<size_t> results;
    for (size_t i = 0; i < std::min(maxResults, distances.size()); ++i) {
        results.push_back(distances[i].second);
    }
    
    return results;
}

void NeuralEpisodicMemory::compressMemories() {
    // Compress all episodes for storage efficiency
    size_t compressedCount = 0;
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        compressEpisode(episodes_[i]);
        compressedCount++;
    }
    
    compressionCount_.fetch_add(compressedCount, std::memory_order_relaxed);
    
    // Deduplicate compressed memories
    deduplicateMemories();
}

void NeuralEpisodicMemory::deduplicateMemories() {
    // Remove duplicate compressed episodes
    std::vector<size_t> toKeep;
    std::unordered_map<std::vector<float>, size_t> uniqueMap;
    
    for (size_t i = 0; i < compressedEpisodes_.size(); ++i) {
        if (uniqueMap.find(compressedEpisodes_[i]) == uniqueMap.end()) {
            uniqueMap[compressedEpisodes_[i]] = toKeep.size();
            toKeep.push_back(i);
        }
    }
    
    // Keep only unique episodes
    std::vector<size_t> newToKeep;
    std::vector<size_t> oldToNew(compressedEpisodes_.size(), -1);
    
    for (size_t i = 0; i < toKeep.size(); ++i) {
        oldToNew[toKeep[i]] = i;
        newToKeep.push_back(toKeep[i]);
    }
    
    // Remove duplicates
    compressedEpisodes_.clear();
    for (size_t i : newToKeep) {
        compressedEpisodes_.push_back(compressedEpisodes_[i]);
    }
}

size_t NeuralEpisodicMemory::getCompressedMemoryCount() const {
    return compressedEpisodes_.size() / 2;  // Account for neuron activations
}

void NeuralEpisodicMemory::createMemoryTrace(const EpisodicMemoryItem& episode,
                                             float initialImportance) {
    // Create a new memory trace with enhanced features
    storeEpisode(episode);
    
    // Update index with importance
    if (!index_.empty()) {
        index_.back().importance = initialImportance;
        index_.back().relevanceScore = initialImportance;
    }
}

void NeuralEpisodicMemory::decayMemories(float decayRate, float minImportance) {
    // Apply decay to all episodes
    for (auto& episode : episodes_) {
        // Decay novelty and importance
        episode.novelty *= decayRate;
        
        if (episode.novelty < minImportance) {
            episode.novelty = 0.0f;
        }
    }
}

void NeuralEpisodicMemory::consolidate(float relevanceThreshold, float timeThreshold) {
    // Consolidate memories based on relevance and time
    consolidationCount_.fetch_add(1, std::memory_order_relaxed);
    
    // Remove episodes below relevance threshold
    std::vector<size_t> toRemove;
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        float relevance = computeRelevance(episodes_[i]);
        
        if (relevance < relevanceThreshold || episodes_[i].age > timeThreshold) {
            toRemove.push_back(i);
        }
    }
    
    // Remove low-relevance episodes
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        size_t index = *it;
        
        episodes_.erase(episodes_.begin() + index);
        
        if (index < index_.size()) {
            index_.erase(index_.begin() + index);
        }
        
        if (index < compressedEpisodes_.size()) {
            compressedEpisodes_.erase(compressedEpisodes_.begin() + index * 2);
            if (index * 2 + 1 < compressedEpisodes_.size()) {
                compressedEpisodes_.erase(compressedEpisodes_.begin() + index * 2);
            }
        }
        
        if (index < cache_.size()) {
            cache_.erase(index);
        }
    }
}

void NeuralEpisodicMemory::enhanceWithPlasticity(float plasticityFactor) {
    // Enhance memory with neural plasticity
    for (auto& episode : episodes_) {
        // Modify sensory state based on plasticity
        for (auto& value : episode.sensoryState) {
            value *= (1.0f + plasticityFactor * 0.1f);
        }
        
        // Update neuron activations
        for (auto& activation : episode.neuronActivations) {
            activation *= (1.0f + plasticityFactor * 0.05f);
        }
    }
}

void NeuralEpisodicMemory::adaptToEnvironment(const std::vector<float>& environmentalFactors) {
    // Adapt memory based on environmental factors
    for (auto& episode : episodes_) {
        // Modify episode based on environment
        for (size_t i = 0; i < std::min(episode.sensoryState.size(), 
                                       environmentalFactors.size()); ++i) {
            episode.sensoryState[i] *= (1.0f + environmentalFactors[i] * 0.1f);
        }
    }
}

void NeuralEpisodicMemory::coordinateAccess(MemorySystemType type,
                                            const std::vector<float>& pattern,
                                            AccessType accessType) {
    // Coordinate memory access with other systems
    if (coordinator_) {
        coordinator_->coordinateAccess(type, pattern, accessType);
    }
}

void NeuralEpisodicMemory::adjustEpisodePriority(SimulationStep episodeId, float priorityDelta) {
    // Adjust priority of an episode
    if (episodeId < episodes_.size()) {
        // Update episode novelty and importance
        episodes_[episodeId].novelty += priorityDelta;
        episodes_[episodeId].novelty = std::clamp(episodes_[episodeId].novelty, 0.0f, 1.0f);
        
        // Update index
        if (episodeId < index_.size()) {
            index_[episodeId].importance += priorityDelta;
            index_[episodeId].importance = std::clamp(index_[episodeId].importance, 0.0f, 1.0f);
        }
    }
}

void NeuralEpisodicMemory::replayForConsolidation(size_t count) {
    // Replay memories for consolidation
    replayCount_.fetch_add(count, std::memory_order_relaxed);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    auto episodesToReplay = getEpisodesForReplay(count);
    
    for (const auto* episode : episodesToReplay) {
        replayEpisode(episode);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double replayTime = std::chrono::duration<double, std::milliseconds>(endTime - startTime).count();
    
    totalReplayTime_.fetch_add(replayTime, std::memory_order_relaxed);
}

void NeuralEpisodicMemory::consolidateWithReplay(size_t replayCount, float consolidationThreshold) {
    // Consolidate memories with replay
    replayForConsolidation(replayCount);
    consolidate(consolidationThreshold);
}

void NeuralEpisodicMemory::performSleepReplay() {
    // Perform sleep-like replay for consolidation
    // Replay 50% of most important memories
    size_t replayCount = std::max(size_t(1), episodes_.size() / 2);
    replayForConsolidation(replayCount);
}

void NeuralEpisodicMemory::rankMemoriesByImportance() {
    // Rank memories by importance and update index
    std::vector<size_t> indices(episodes_.size());
    std::iota(indices.begin(), indices.end(), 0);
    
    // Sort by relevance
    std::sort(indices.begin(), indices.end(),
        [this](size_t a, size_t b) {
            return computeRelevance(episodes_[a]) > computeRelevance(episodes_[b]);
        });
    
    // Update index with new order
    std::vector<MemoryIndexEntry> newIndex;
    for (size_t i : indices) {
        newIndex.push_back(index_[i]);
    }
    
    index_ = std::move(newIndex);
}

std::vector<size_t> NeuralEpisodicMemory::getTopPriorityEpisodes(size_t count) const {
    std::vector<size_t> results;
    
    // Rank by importance and get top results
    std::vector<std::pair<float, size_t>> scored;
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        float relevance = computeRelevance(episodes_[i]);
        scored.emplace_back(relevance, i);
    }
    
    // Sort by relevance (descending)
    std::sort(scored.begin(), scored.end(),
        [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Extract top results
    for (size_t i = 0; i < std::min(count, scored.size()); ++i) {
        results.push_back(scored[i].second);
    }
    
    return results;
}

void NeuralEpisodicMemory::updatePriority(SimulationStep episodeId, float importanceChange) {
    // Update priority of an episode
    if (episodeId < episodes_.size()) {
        // Update episode importance
        float newImportance = episodes_[episodeId].novelty + importanceChange;
        episodes_[episodeId].novelty = std::clamp(newImportance, 0.0f, 1.0f);
        
        // Update index
        if (episodeId < index_.size()) {
            index_[episodeId].importance = episodes_[episodeId].novelty;
        }
    }
}

void NeuralEpisodicMemory::applyDecay(float decayRate, float halfLife) {
    // Apply exponential decay to memories
    float decayFactor = std::exp(-decayRate * halfLife);
    
    for (auto& episode : episodes_) {
        // Decay novelty and importance
        episode.novelty *= decayFactor;
        
        if (episode.novelty < 0.01f) {
            episode.novelty = 0.0f;
        }
    }
}

void NeuralEpisodicMemory::forgetIrrelevantMemories(float forgettingRate, float relevanceThreshold) {
    // Forget memories below relevance threshold
    std::vector<size_t> toForget;
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        float relevance = computeRelevance(episodes_[i]);
        
        if (relevance < relevanceThreshold) {
            toForget.push_back(i);
        }
    }
    
    // Forget low-relevance memories
    for (auto it = toForget.rbegin(); it != toForget.rend(); ++it) {
        size_t index = *it;
        
        episodes_.erase(episodes_.begin() + index);
        
        if (index < index_.size()) {
            index_.erase(index_.begin() + index);
        }
        
        if (index < compressedEpisodes_.size()) {
            compressedEpisodes_.erase(compressedEpisodes_.begin() + index * 2);
            if (index * 2 + 1 < compressedEpisodes_.size()) {
                compressedEpisodes_.erase(compressedEpisodes_.begin() + index * 2);
            }
        }
        
        if (index < cache_.size()) {
            cache_.erase(index);
        }
    }
}

std::vector<SimulationStep> NeuralEpisodicMemory::getForgettingCandidates(float threshold) const {
    std::vector<SimulationStep> candidates;
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        float relevance = computeRelevance(episodes_[i]);
        
        if (relevance < threshold) {
            candidates.push_back(episodes_[i].timestamp);
        }
    }
    
    return candidates;
}

NeuralEpisodicMemory::EpisodeStats NeuralEpisodicMemory::getStats() const {
    EpisodeStats stats;
    
    stats.totalEpisodes = episodes_.size();
    stats.replayedEpisodes = replayCount_.load(std::memory_order_relaxed);
    stats.consolidatedEpisodes = consolidationCount_.load(std::memory_order_relaxed);
    
    // Calculate average reward
    if (!episodes_.empty()) {
        float sum = 0.0f;
        for (const auto& ep : episodes_) {
            sum += ep.reward;
        }
        stats.averageReward = sum / episodes_.size();
    } else {
        stats.averageReward = 0.0f;
    }
    
    // Calculate average importance
    if (!index_.empty()) {
        float sum = 0.0f;
        for (const auto& idx : index_) {
            sum += idx.importance;
        }
        stats.averageImportance = sum / index_.size();
    } else {
        stats.averageImportance = 0.0f;
    }
    
    // Calculate memory utilization
    size_t totalMemory = getTotalMemoryUsage();
    stats.memoryUtilization = static_cast<float>(totalMemory) / config_.maxMemoryUsage;
    
    // Calculate replay efficiency
    if (!episodes_.empty()) {
        stats.replayEfficiency = static_cast<float>(replayCount_.load()) / 
                                 static_cast<float>(episodes_.size());
    } else {
        stats.replayEfficiency = 0.0f;
    }
    
    // Calculate average replay time
    if (replayCount_.load() > 0) {
        stats.averageReplayTime = std::chrono::milliseconds(
            static_cast<long>(totalReplayTime_.load() / replayCount_.load()));
    } else {
        stats.averageReplayTime = std::chrono::milliseconds(0);
    }
    
    return stats;
}

void NeuralEpisodicMemory::optimizeAccessPatterns() {
    // Optimize access patterns for better performance
    // Move frequently accessed episodes to high priority cache
    for (const auto& [index, pattern] : cache_) {
        highPriorityCache_.insert(index);
    }
}

void NeuralEpisodicMemory::optimizeStorageLayout() {
    // Optimize storage layout for better cache locality
    rankMemoriesByImportance();
    
    // Reorganize storage based on importance
    std::vector<EpisodicMemoryItem> newEpisodes;
    std::vector<MemoryIndexEntry> newIndex;
    std::vector<std::vector<float>> newCompressed;
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        // Move important episodes to front
        if (computeRelevance(episodes_[i]) > 0.5f) {
            newEpisodes.insert(newEpisodes.begin(), episodes_[i]);
            newIndex.insert(newIndex.begin(), index_[i]);
            newCompressed.insert(newCompressed.begin(), compressedEpisodes_[i * 2]);
            if (i * 2 + 1 < compressedEpisodes_.size()) {
                newCompressed.insert(newCompressed.begin() + 1, compressedEpisodes_[i * 2 + 1]);
            }
        } else {
            newEpisodes.push_back(episodes_[i]);
            newIndex.push_back(index_[i]);
            newCompressed.push_back(compressedEpisodes_[i * 2]);
            if (i * 2 + 1 < compressedEpisodes_.size()) {
                newCompressed.push_back(compressedEpisodes_[i * 2 + 1]);
            }
        }
    }
    
    episodes_ = std::move(newEpisodes);
    index_ = std::move(newIndex);
    compressedEpisodes_ = std::move(newCompressed);
}

size_t NeuralEpisodicMemory::getTotalMemoryUsage() const {
    size_t total = 0;
    
    // Main memory
    total += episodes_.size() * sizeof(EpisodicMemoryItem);
    
    // Index
    total += index_.size() * sizeof(MemoryIndexEntry);
    
    // Compressed storage
    for (const auto& compressed : compressedEpisodes_) {
        total += compressed.size() * sizeof(float);
    }
    
    // Cache
    for (const auto& [index, pattern] : cache_) {
        total += pattern.size() * sizeof(float);
    }
    
    return total;
}

double NeuralEpisodicMemory::getAverageReplayTime() const {
    if (replayCount_.load(std::memory_order_relaxed) == 0) {
        return 0.0;
    }
    
    double totalTime = totalReplayTime_.load(std::memory_order_relaxed);
    size_t count = replayCount_.load(std::memory_order_relaxed);
    
    return totalTime / count;
}

void NeuralEpisodicMemory::setEpisodeConfig(const EpisodeMemoryConfig& config) {
    config_ = config;
}

void NeuralEpisodicMemory::autoTune(float performanceThreshold) {
    // Auto-tune episodic memory configuration based on performance
    analyzePerformance();
    
    float currentScore = getPerformanceScore();
    
    if (currentScore < performanceThreshold) {
        // Adjust configuration for better performance
        if (config_.maxEpisodes > 100) {
            config_.maxEpisodes = static_cast<size_t>(config_.maxEpisodes * 0.9f);
        }
        if (config_.indexSize > 64) {
            config_.indexSize = static_cast<size_t>(config_.indexSize * 0.9f);
        }
        if (config_.cacheSize > 16) {
            config_.cacheSize = static_cast<size_t>(config_.cacheSize * 0.9f);
        }
        
        // Adjust compression threshold
        config_.compressionThreshold = std::max(0.5f, config_.compressionThreshold * 1.1f);
    } else {
        // Improve configuration for better performance
        if (config_.maxEpisodes < 5000) {
            config_.maxEpisodes = static_cast<size_t>(config_.maxEpisodes * 1.1f);
        }
        if (config_.indexSize < 1024) {
            config_.indexSize = static_cast<size_t>(config_.indexSize * 1.1f);
        }
        if (config_.cacheSize < 256) {
            config_.cacheSize = static_cast<size_t>(config_.cacheSize * 1.1f);
        }
        
        // Adjust compression threshold
        config_.compressionThreshold = std::min(0.95f, config_.compressionThreshold * 0.9f);
    }
}

void NeuralEpisodicMemory::analyzePerformance() {
    // Analyze performance metrics
    size_t episodeCount = episodes_.size();
    size_t replayCount = replayCount_.load(std::memory_order_relaxed);
    
    // Check if performance is within acceptable bounds
    if (episodeCount > 5000) {
        NLM_LOG_WARN("NeuralEpisodicMemory: High episode count: " + std::to_string(episodeCount));
    }
    
    if (replayCount > 100) {
        float replayEfficiency = static_cast<float>(replayCount) / 
                                 static_cast<float>(episodeCount + 1);
        NLM_LOG_INFO("NeuralEpisodicMemory: Replay efficiency: " + 
                    std::to_string(replayEfficiency * 100.0f) + "%");
    }
}

float NeuralEpisodicMemory::getPerformanceScore() const {
    float episodeCountScore = std::min(1.0f, 5000.0f / static_cast<float>(episodes_.size()));
    float replayScore = std::min(1.0f, static_cast<float>(replayCount_.load()) / 100.0f);
    float compressionScore = std::min(1.0f, static_cast<float>(compressionCount_.load()) / 1000.0f);
    
    return (episodeCountScore + replayScore + compressionScore) / 3.0f;
}

float NeuralEpisodicMemory::computeRelevance(const EpisodicMemoryItem& episode) const {
    // Compute relevance of an episode
    float recencyScore = 1.0f / (episode.age + 1);
    float rewardScore = episode.reward;
    float noveltyScore = episode.novelty;
    
    return recencyScore * 0.3f + rewardScore * 0.5f + noveltyScore * 0.2f;
}

std::vector<float> NeuralEpisodicMemory::compressPattern(const std::vector<float>& pattern) const {
    // Compress pattern for storage efficiency
    if (!config_.enableCompression) {
        return pattern;
    }
    
    // Simple compression: downsample
    size_t compressedSize = std::max(size_t(1), pattern.size() / 4);
    std::vector<float> compressed(compressedSize);
    
    for (size_t i = 0; i < compressedSize; ++i) {
        float sum = 0.0f;
        size_t count = 0;
        for (size_t j = 0; j < 4; ++j) {
            size_t idx = i * 4 + j;
            if (idx < pattern.size()) {
                sum += pattern[idx];
                count++;
            }
        }
        compressed[i] = sum / static_cast<float>(count);
    }
    
    return compressed;
}

std::vector<float> NeuralEpisodicMemory::getPatternFromIndex(size_t index) const {
    // Get pattern from index
    if (index < episodes_.size()) {
        return episodes_[index].sensoryState;
    }
    return {};
}

} // namespace nlm
