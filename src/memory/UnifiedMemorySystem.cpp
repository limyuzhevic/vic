#include "UnifiedMemorySystem.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <chrono>
#include <fstream>

namespace nlm {

// ============================================================================
// GLOBAL MEMORY SYSTEM INSTANCE
// ============================================================================

std::unique_ptr<MemorySystemManager> gMemorySystem = nullptr;

// ============================================================================
// NEURAL WORKING MEMORY IMPLEMENTATION
// ============================================================================

struct NeuralWorkingMemory::Impl {
    Brain* brain;
    std::vector<std::pair<NeuronId, float>> memoryNeurons;
    std::vector<float> memoryActivations;
    std::vector<SimulationStep> memoryTimestamps;
    std::vector<NeuronId> winners;
    
    // Maintenance connections (recurrent)
    std::vector<std::pair<NeuronId, NeuronId>> maintenanceSynapses;
    
    // Memory trace ages
    std::vector<SimulationStep> traceAges;
    
    // Integration state
    float plasticityInfluence;
    float dopamineLevel;
    
    Impl() : brain(nullptr), plasticityInfluence(1.0f), dopamineLevel(0.0f) {}
};

NeuralWorkingMemory::NeuralWorkingMemory() : pImpl(new Impl), brain_(nullptr), initialized_(false) {}

NeuralWorkingMemory::~NeuralWorkingMemory() = default;

void NeuralWorkingMemory::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize NeuralWorkingMemory with null brain");
        return;
    }
    
    brain_ = brain;
    initialized_ = true;
    
    // Initialize with biologically plausible parameters
    pImpl->memoryNeurons.clear();
    pImpl->memoryActivations.clear();
    pImpl->memoryTimestamps.clear();
    pImpl->winners.clear();
    pImpl->maintenanceSynapses.clear();
    pImpl->traceAges.clear();
    pImpl->plasticityInfluence = 1.0f;
    pImpl->dopamineLevel = 0.0f;
    
    NLM_LOG_INFO("NeuralWorkingMemory initialized with brain integration");
}

void NeuralWorkingMemory::clear() {
    pImpl->memoryNeurons.clear();
    pImpl->memoryActivations.clear();
    pImpl->memoryTimestamps.clear();
    pImpl->winners.clear();
    pImpl->maintenanceSynapses.clear();
    pImpl->traceAges.clear();
    pImpl->plasticityInfluence = 1.0f;
    pImpl->dopamineLevel = 0.0f;
    
    NLM_LOG_INFO("NeuralWorkingMemory cleared");
}

void NeuralWorkingMemory::logStatus() const {
    if (!brain_) {
        NLM_LOG_ERROR("NeuralWorkingMemory not initialized");
        return;
    }
    
    NLM_LOG_INFO("=== Neural Working Memory Status ===");
    NLM_LOG_INFO("Active traces: " + std::to_string(pImpl->memoryNeurons.size()));
    NLM_LOG_INFO("Current activity: " + std::to_string(getMemoryActivity()));
    NLM_LOG_INFO("Plasticity influence: " + std::to_string(pImpl->plasticityInfluence));
    NLM_LOG_INFO("Dopamine level: " + std::to_string(pImpl->dopamineLevel));
    NLM_LOG_INFO("Maintenance synapses: " + std::to_string(pImpl->maintenanceSynapses.size()));
}

void NeuralWorkingMemory::store(const std::vector<float>& pattern, float strength) {
    if (pattern.empty() || !brain_) return;
    
    // Find neurons to encode this pattern
    for (size_t i = 0; i < pattern.size() && i < pImpl->memoryNeurons.size(); ++i) {
        if (pattern[i] > 0.5f) {
            storeToNeuron(pImpl->memoryNeurons[i], std::min(1.0f, pattern[i] * strength));
        }
    }
}

void NeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
    // Find or add this neuron to memory
    auto it = std::find(pImpl->memoryNeurons.begin(), pImpl->memoryNeurons.end(), neuron);
    
    if (it != pImpl->memoryNeurons.end()) {
        size_t idx = std::distance(pImpl->memoryNeurons.begin(), it);
        pImpl->memoryActivations[idx] = std::max(pImpl->memoryActivations[idx], activation);
        pImpl->memoryTimestamps[idx] = currentTimeStep_;
    } else {
        pImpl->memoryNeurons.push_back(neuron);
        pImpl->memoryActivations.push_back(activation);
        pImpl->memoryTimestamps.push_back(currentTimeStep_);
        pImpl->traceAges.push_back(0);
    }
}

std::vector<float> NeuralWorkingMemory::retrieve() const {
    std::vector<float> result;
    result.reserve(pImpl->memoryActivations.size());
    
    for (const auto& activation : pImpl->memoryActivations) {
        result.push_back(activation);
    }
    
    return result;
}

bool NeuralWorkingMemory::contains(NeuronId neuron) const {
    return std::find(pImpl->memoryNeurons.begin(), pImpl->memoryNeurons.end(), neuron) != pImpl->memoryNeurons.end();
}

float NeuralWorkingMemory::getNeuronActivation(NeuronId neuron) const {
    auto it = std::find(pImpl->memoryNeurons.begin(), pImpl->memoryNeurons.end(), neuron);
    if (it != pImpl->memoryNeurons.end()) {
        size_t idx = std::distance(pImpl->memoryNeurons.begin(), it);
        return pImpl->memoryActivations[idx];
    }
    return 0.0f;
}

void NeuralWorkingMemory::update(TimestepDuration dt) {
    if (!brain_) return;
    
    // Update maintenance - reinforce active memory neurons
    updateRecurrentConnections();
    decayWeakTraces();
    
    // Update trace ages
    for (auto& age : pImpl->traceAges) {
        age++;
    }
    
    // Run competition for winner-take-all
    runCompetition();
}

void NeuralWorkingMemory::strengthenMemory(float factor) {
    for (auto& activation : pImpl->memoryActivations) {
        activation = std::min(1.0f, activation * factor);
    }
}

void NeuralWorkingMemory::createRecurrentConnection(NeuronId from, NeuronId to, float strength) {
    // Check if connection already exists
    for (const auto& conn : pImpl->maintenanceSynapses) {
        if (conn.first == from && conn.second == to) return;
    }
    
    pImpl->maintenanceSynapses.emplace_back(from, to);
}

void NeuralWorkingMemory::updateRecurrentConnections() {
    // Apply maintenance currents through recurrent connections
    for (const auto& conn : pImpl->maintenanceSynapses) {
        float fromActivation = 0.0f;
        auto it = std::find(pImpl->memoryNeurons.begin(), pImpl->memoryNeurons.end(), conn.first);
        if (it != pImpl->memoryNeurons.end()) {
            size_t idx = std::distance(pImpl->memoryNeurons.begin(), it);
            fromActivation = pImpl->memoryActivations[idx];
        }
        
        // Add activation to target neuron
        auto targetIt = std::find(pImpl->memoryNeurons.begin(), pImpl->memoryNeurons.end(), conn.second);
        if (targetIt != pImpl->memoryNeurons.end()) {
            size_t targetIdx = std::distance(pImpl->memoryNeurons.begin(), targetIt);
            pImpl->memoryActivations[targetIdx] += fromActivation * 0.1f;
            pImpl->memoryActivations[targetIdx] = std::min(1.0f, pImpl->memoryActivations[targetIdx]);
        }
    }
}

void NeuralWorkingMemory::decayWeakTraces() {
    std::vector<size_t> toRemove;
    
    for (size_t i = 0; i < pImpl->memoryNeurons.size(); ++i) {
        // Decay based on age and activation
        float decayFactor = 0.95f + (0.05f * (pImpl->traceAges[i] / 100.0f));
        pImpl->memoryActivations[i] *= decayFactor;
        
        // Remove very weak traces
        if (pImpl->memoryActivations[i] < 0.01f && pImpl->traceAges[i] > 50) {
            toRemove.push_back(i);
        }
    }
    
    // Remove weak traces
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        size_t idx = *it;
        pImpl->memoryNeurons.erase(pImpl->memoryNeurons.begin() + idx);
        pImpl->memoryActivations.erase(pImpl->memoryActivations.begin() + idx);
        pImpl->memoryTimestamps.erase(pImpl->memoryTimestamps.begin() + idx);
        pImpl->traceAges.erase(pImpl->traceAges.begin() + idx);
    }
}

void NeuralWorkingMemory::runCompetition() {
    winners_.clear();
    
    if (pImpl->memoryActivations.empty()) return;
    
    // Find winner neurons based on activation
    float maxActivation = 0.0f;
    for (size_t i = 0; i < pImpl->memoryActivations.size(); ++i) {
        if (pImpl->memoryActivations[i] > maxActivation) {
            maxActivation = pImpl->memoryActivations[i];
        }
    }
    
    // Select neurons with near-maximal activation
    float threshold = maxActivation * 0.8f;
    for (size_t i = 0; i < pImpl->memoryActivations.size(); ++i) {
        if (pImpl->memoryActivations[i] >= threshold) {
            winners_.push_back(pImpl->memoryNeurons[i]);
        }
    }
}

bool NeuralWorkingMemory::isWinning(NeuronId neuron) const {
    return std::find(winners_.begin(), winners_.end(), neuron) != winners_.end();
}

float NeuralWorkingMemory::getMemoryActivity() const {
    if (pImpl->memoryActivations.empty()) return 0.0f;
    
    float total = 0.0f;
    for (const auto& activation : pImpl->memoryActivations) {
        total += activation;
    }
    
    return total / pImpl->memoryActivations.size();
}

size_t NeuralWorkingMemory::getActiveTraceCount() const {
    return pImpl->memoryNeurons.size();
}

void NeuralWorkingMemory::integrateWithPlasticity(float plasticityInfluence) {
    pImpl->plasticityInfluence = std::clamp(plasticityInfluence, 0.0f, 2.0f);
}

void NeuralWorkingMemory::integrateWithNeuromodulation(float dopamineLevel) {
    pImpl->dopamineLevel = std::clamp(dopamineLevel, 0.0f, 1.0f);
}

void NeuralWorkingMemory::consolidateWithEpisodicMemory(NeuralEpisodicMemory* episodicMem) {
    if (!episodicMem || episodicMem->getEpisodeCount() >= episodicMem->getMaxEpisodicEpisodes()) {
        return;
    }
    
    // Convert strongest working memory traces to episodic episodes
    for (size_t i = 0; i < pImpl->memoryNeurons.size(); ++i) {
        if (pImpl->memoryActivations[i] > 0.7f) {
            EpisodicMemoryItem episode;
            episode.timestamp = currentTimeStep_;
            episode.timePoint = std::chrono::system_clock::now();
            
            // Add to episodic memory
            episodicMem->storeEpisode(episode);
        }
    }
}

void NeuralWorkingMemory::replayForConsolidation() {
    // Recreate neural patterns for consolidation
    for (size_t i = 0; i < pImpl->memoryNeurons.size(); ++i) {
        // Simulate replay by updating neurons
        if (brain_) {
            brain_->injectCurrent(pImpl->memoryNeurons[i], pImpl->memoryActivations[i] * 0.5f);
        }
    }
}

// ============================================================================
// NEURAL EPISODIC MEMORY IMPLEMENTATION
// ============================================================================

struct NeuralEpisodicMemory::Impl {
    std::vector<std::vector<float>> patternCache;
    std::vector<float> patternSimilarity;
    std::vector<size_t> accessCounts;
    float cacheDecayRate;
    size_t maxCacheSize;
    
    Impl() : cacheDecayRate(0.99f), maxCacheSize(1000) {}
};

NeuralEpisodicMemory::NeuralEpisodicMemory() : pImpl(new Impl), brain_(nullptr), 
    maxEpisodicEpisodes_(1000), replayEnabled_(false), timeStep_(0), 
    consolidationThreshold_(0.5f) {}

NeuralEpisodicMemory::~NeuralEpisodicMemory() = default;

void NeuralEpisodicMemory::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize NeuralEpisodicMemory with null brain");
        return;
    }
    
    brain_ = brain;
    episodes_.clear();
    longTermStorage_.clear();
    pImpl->patternCache.clear();
    pImpl->patternSimilarity.clear();
    pImpl->accessCounts.clear();
    
    NLM_LOG_INFO("NeuralEpisodicMemory initialized with brain integration");
}

void NeuralEpisodicMemory::clear() {
    episodes_.clear();
    longTermStorage_.clear();
    pImpl->patternCache.clear();
    pImpl->patternSimilarity.clear();
    pImpl->accessCounts.clear();
    
    NLM_LOG_INFO("NeuralEpisodicMemory cleared");
}

void NeuralEpisodicMemory::logStatus() const {
    if (!brain_) {
        NLM_LOG_ERROR("NeuralEpisodicMemory not initialized");
        return;
    }
    
    NLM_LOG_INFO("=== Neural Episodic Memory Status ===");
    NLM_LOG_INFO("Episodic episodes: " + std::to_string(getEpisodeCount()));
    NLM_LOG_INFO("Long-term storage: " + std::to_string(longTermStorage_.size()));
    NLM_LOG_INFO("Replay enabled: " + std::string(replayEnabled_ ? "yes" : "no"));
    NLM_LOG_INFO("Current time step: " + std::to_string(timeStep_));
    NLM_LOG_INFO("Consolidation threshold: " + std::to_string(consolidationThreshold_));
}

void NeuralEpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    if (episodes_.size() >= maxEpisodicEpisodes_) {
        // Remove oldest episode
        episodes_.pop_front();
    }
    
    // Add current time step
    EpisodeMemoryItem newEpisode = episode;
    newEpisode.timestamp = timeStep_++;
    
    episodes_.push_back(newEpisode);
    
    // Update cache
    size_t episodeIndex = episodes_.size() - 1;
    pImpl->patternCache.push_back(newEpisode.sensoryState);
    pImpl->patternSimilarity.push_back(1.0f); // Perfect match for stored episode
    pImpl->accessCounts.push_back(1);
    
    NLM_LOG_DEBUG("Stored episodic memory at time step " + std::to_string(newEpisode.timestamp));
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveSimilar(const std::vector<float>& sensoryPattern,
                                                                              size_t maxResults) const {
    std::vector<std::pair<float, const EpisodicMemoryItem*>> scoredEpisodes;
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        float similarity = computeSimilarity(sensoryPattern, episodes_[i]);
        if (similarity > 0.1f) { // Threshold for relevance
            scoredEpisodes.emplace_back(similarity, &episodes_[i]);
        }
    }
    
    // Sort by similarity (highest first)
    std::sort(scoredEpisodes.begin(), scoredEpisodes.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Return top results
    std::vector<const EpisodicMemoryItem*> results;
    size_t count = std::min(maxResults, scoredEpisodes.size());
    for (size_t i = 0; i < count; ++i) {
        results.push_back(scoredEpisodes[i].second);
    }
    
    return results;
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveTemporal(SimulationStep startTime,
                                                                               SimulationStep endTime,
                                                                               size_t maxResults) const {
    std::vector<const EpisodicMemoryItem*> results;
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        if (episodes_[i].timestamp >= startTime && episodes_[i].timestamp <= endTime) {
            results.push_back(&episodes_[i]);
            if (results.size() >= maxResults) break;
        }
    }
    
    return results;
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveByLocation(float x, float y,
                                                                                 float radius,
                                                                                 size_t maxResults) const {
    std::vector<const EpisodicMemoryItem*> results;
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        float distance = std::sqrt(std::pow(episodes_[i].positionX - x, 2) +
                                   std::pow(episodes_[i].positionY - y, 2));
        if (distance <= radius) {
            results.push_back(&episodes_[i]);
            if (results.size() >= maxResults) break;
        }
    }
    
    return results;
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveAfterAction(ActionType action,
                                                                                    size_t maxResults) const {
    std::vector<const EpisodicMemoryItem*> results;
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        if (episodes_[i].action == action) {
            results.push_back(&episodes_[i]);
            if (results.size() >= maxResults) break;
        }
    }
    
    return results;
}

void NeuralEpisodicMemory::replayEpisode(const EpisodicMemoryItem* episode) {
    if (!brain_ || !episode) return;
    
    // Recreate the neural pattern for this episode
    for (size_t i = 0; i < episode->activeNeurons.size(); ++i) {
        brain_->injectCurrent(episode->activeNeurons[i], episode->neuronActivations[i] * 0.5f);
    }
    
    NLM_LOG_DEBUG("Replaying episodic memory episode");
}

void NeuralEpisodicMemory::updateRelevance(SimulationStep episodeIndex, float relevanceDelta) {
    if (episodeIndex < episodes_.size()) {
        // Update the episode's strength based on relevance
        episodes_[episodeIndex].strength += relevanceDelta;
        episodes_[episodeIndex].strength = std::clamp(episodes_[episodeIndex].strength, 0.0f, 1.0f);
        
        // Update cache entry if it exists
        if (episodeIndex < pImpl->patternSimilarity.size()) {
            pImpl->patternSimilarity[episodeIndex] = episodes_[episodeIndex].strength;
        }
    }
}

const EpisodicMemoryItem* NeuralEpisodicMemory::getEpisode(size_t index) const {
    if (index < episodes_.size()) {
        return &episodes_[index];
    }
    return nullptr;
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::getRecentEpisodes(size_t count) const {
    std::vector<const EpisodicMemoryItem*> results;
    size_t start = count > episodes_.size() ? 0 : episodes_.size() - count;
    
    for (size_t i = start; i < episodes_.size(); ++i) {
        results.push_back(&episodes_[i]);
    }
    
    return results;
}

float NeuralEpisodicMemory::getAverageReward() const {
    if (episodes_.empty()) return 0.0f;
    
    float totalReward = 0.0f;
    for (const auto& episode : episodes_) {
        totalReward += episode.reward;
    }
    
    return totalReward / episodes_.size();
}

void NeuralEpisodicMemory::consolidate(float relevanceThreshold) {
    // Move relevant episodes to long-term storage
    for (size_t i = 0; i < episodes_.size(); ++i) {
        if (episodes_[i].strength >= relevanceThreshold && !episodes_[i].isConsolidated) {
            episodes_[i].isConsolidated = true;
            episodes_[i].consolidationTime = timeStep_;
            
            // Move to long-term storage
            longTermStorage_.push_back(episodes_[i]);
            
            NLM_LOG_DEBUG("Consolidated episodic memory episode");
        }
    }
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::getEpisodesForReplay(size_t count) const {
    std::vector<std::pair<float, const EpisodicMemoryItem*>> replayCandidates;
    
    // Select episodes for replay based on relevance and recency
    for (size_t i = 0; i < episodes_.size(); ++i) {
        if (episodes_[i].strength > 0.0f) {
            // Score based on strength and recency
            float recencyFactor = std::exp(-episodes_[i].age / 1000.0f);
            float score = episodes_[i].strength * recencyFactor;
            replayCandidates.emplace_back(score, &episodes_[i]);
        }
    }
    
    // Sort by score (highest first)
    std::sort(replayCandidates.begin(), replayCandidates.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Return top candidates
    std::vector<const EpisodicMemoryItem*> results;
    size_t selectedCount = std::min(count, replayCandidates.size());
    for (size_t i = 0; i < selectedCount; ++i) {
        results.push_back(replayCandidates[i].second);
    }
    
    return results;
}

void NeuralEpisodicMemory::replaySequence(const std::vector<size_t>& episodeIds) {
    for (size_t id : episodeIds) {
        if (id < episodes_.size()) {
            replayEpisode(&episodes_[id]);
        }
    }
}

float NeuralEpisodicMemory::computeSimilarity(const std::vector<float>& query,
                                            const EpisodicMemoryItem& episode) const {
    if (query.empty() || episode.sensoryState.empty()) return 0.0f;
    
    size_t minSize = std::min(query.size(), episode.sensoryState.size());
    float dotProduct = 0.0f;
    
    for (size_t i = 0; i < minSize; ++i) {
        dotProduct += query[i] * episode.sensoryState[i];
    }
    
    float magnitude1 = 0.0f, magnitude2 = 0.0f;
    for (size_t i = 0; i < query.size(); ++i) {
        magnitude1 += query[i] * query[i];
    }
    magnitude1 = std::sqrt(magnitude1);
    
    for (size_t i = 0; i < episode.sensoryState.size(); ++i) {
        magnitude2 += episode.sensoryState[i] * episode.sensoryState[i];
    }
    magnitude2 = std::sqrt(magnitude2);
    
    if (magnitude1 == 0.0f || magnitude2 == 0.0f) return 0.0f;
    
    return dotProduct / (magnitude1 * magnitude2);
}

bool NeuralEpisodicMemory::matchesCriteria(const EpisodicMemoryItem& episode,
                                           const std::vector<float>* sensoryQuery,
                                           SimulationStep* startTime,
                                           SimulationStep* endTime,
                                           ActionType* actionQuery) const {
    bool matches = true;
    
    if (sensoryQuery && !sensoryQuery->empty()) {
        float similarity = computeSimilarity(*sensoryQuery, episode);
        if (similarity < 0.5f) matches = false;
    }
    
    if (startTime && episode.timestamp < *startTime) matches = false;
    if (endTime && episode.timestamp > *endTime) matches = false;
    
    if (actionQuery && episode.action != *actionQuery) matches = false;
    
    return matches;
}

void NeuralEpisodicMemory::integrateWithWorkingMemory(NeuralWorkingMemory* workingMem) {
    if (!workingMem) return;
    
    // Process working memory into episodic memories
    for (size_t i = 0; i < workingMem->getActiveTraceCount(); ++i) {
        if (workingMem->getNeuronActivation(i) > 0.5f) {
            EpisodicMemoryItem episode;
            episode.timestamp = timeStep_++;
            episode.timePoint = std::chrono::system_clock::now();
            episode.sensoryState = workingMem->retrieve();
            episode.strength = workingMem->getNeuronActivation(i);
            
            storeEpisode(episode);
        }
    }
}

void NeuralEpisodicMemory::processIncomingReplay(const std::vector<NeuronId>& replayPattern) {
    // Update pattern cache based on replay
    if (replayPattern.size() < pImpl->maxCacheSize) {
        pImpl->patternCache.push_back(replayPattern);
        pImpl->patternSimilarity.push_back(1.0f);
        pImpl->accessCounts.push_back(1);
    }
}

void NeuralEpisodicMemory::cleanupExpiredEpisodes(SimulationStep maxAge) {
    auto newEnd = std::remove_if(episodes_.begin(), episodes_.end(),
        [this, maxAge](const EpisodicMemoryItem& episode) {
            return episode.age > maxAge;
        });
    
    episodes_.erase(newEnd, episodes_.end());
    
    // Also cleanup cache
    auto cacheIt = std::remove_if(pImpl->patternCache.begin(), pImpl->patternCache.end(),
                                  [](const std::vector<float>& pattern) { return pattern.empty(); });
    pImpl->patternCache.erase(cacheIt, pImpl->patternCache.end());
}

void NeuralEpisodicMemory::compressMemory(size_t targetSize) {
    if (episodes_.size() <= targetSize) return;
    
    // Remove weakest episodes
    std::vector<EpisodicMemoryItem> tempVector(episodes_.begin(), episodes_.end());
    std::sort(tempVector.begin(), tempVector.end(),
              [](const EpisodicMemoryItem& a, const EpisodicMemoryItem& b) {
                  return a.strength < b.strength;
              });
    
    // Keep strongest episodes
    size_t keepCount = std::min(targetSize, tempVector.size());
    episodes_.clear();
    for (size_t i = 0; i < keepCount; ++i) {
        episodes_.push_back(tempVector[i]);
    }
}

// ============================================================================
// NEURAL ASSOCIATIVE MEMORY IMPLEMENTATION
// ============================================================================

struct NeuralAssociativeMemory::Impl {
    std::vector<std::vector<float>> patternNeurons;
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations;
    std::vector<size_t> activationCounts;
    float decayFactor;
    size_t maxPatterns;
    
    Impl() : decayFactor(0.99f), maxPatterns(1000) {}
};

NeuralAssociativeMemory::NeuralAssociativeMemory() : pImpl(new Impl), brain_(nullptr) {}

NeuralAssociativeMemory::~NeuralAssociativeMemory() = default;

void NeuralAssociativeMemory::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize NeuralAssociativeMemory with null brain");
        return;
    }
    
    brain_ = brain;
    associations_.clear();
    pImpl->patternNeurons.clear();
    pImpl->activationCounts.clear();
    
    NLM_LOG_INFO("NeuralAssociativeMemory initialized with brain integration");
}

void NeuralAssociativeMemory::clear() {
    associations_.clear();
    pImpl->patternNeurons.clear();
    pImpl->activationCounts.clear();
    
    NLM_LOG_INFO("NeuralAssociativeMemory cleared");
}

void NeuralAssociativeMemory::logStatus() const {
    if (!brain_) {
        NLM_LOG_ERROR("NeuralAssociativeMemory not initialized");
        return;
    }
    
    NLM_LOG_INFO("=== Neural Associative Memory Status ===");
    NLM_LOG_INFO("Association count: " + std::to_string(getAssociationCount()));
    NLM_LOG_INFO("Pattern count: " + std::to_string(pImpl->patternNeurons.size()));
    NLM_LOG_INFO("Decay factor: " + std::to_string(pImpl->decayFactor));
}

void NeuralAssociativeMemory::associate(const std::vector<float>& patternA,
                                        const std::vector<float>& patternB,
                                        float strength) {
    // Get or create pattern neurons
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    // Update or create association
    bool found = false;
    for (auto& assoc : associations_) {
        if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
            (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
            std::get<2>(assoc) = std::max(std::get<2>(assoc), strength);
            found = true;
            break;
        }
    }
    
    if (!found) {
        associations_.emplace_back(neuronA, neuronB, strength);
    }
}

void NeuralAssociativeMemory::associateFromExperience(const EpisodicMemoryItem& episode) {
    if (episode.activeNeurons.size() < 2) return;
    
    // Create associations between all pairs of active neurons
    for (size_t i = 0; i < episode.activeNeurons.size(); ++i) {
        for (size_t j = i + 1; j < episode.activeNeurons.size(); ++j) {
            float strength = episode.neuronActivations[i] * episode.neuronActivations[j];
            associate(episode.activeNeurons[i], episode.activeNeurons[j], strength);
        }
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::retrieve(const std::vector<float>& queryPattern,
                                                                   size_t maxResults) const {
    std::vector<std::pair<float, std::vector<float>>> scoredPatterns;
    
    for (size_t i = 0; i < pImpl->patternNeurons.size(); ++i) {
        float similarity = computeSimilarity(queryPattern, pImpl->patternNeurons[i]);
        if (similarity > 0.1f) {
            scoredPatterns.emplace_back(similarity, pImpl->patternNeurons[i]);
        }
    }
    
    // Sort by similarity
    std::sort(scoredPatterns.begin(), scoredPatterns.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Return top results
    std::vector<std::vector<float>> results;
    size_t count = std::min(maxResults, scoredPatterns.size());
    for (size_t i = 0; i < count; ++i) {
        results.push_back(scoredPatterns[i].second);
    }
    
    return results;
}

float NeuralAssociativeMemory::getAssociationStrength(const std::vector<float>& patternA,
                                                      const std::vector<float>& patternB) const {
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    for (const auto& assoc : associations_) {
        if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
            (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
            return std::get<2>(assoc);
        }
    }
    
    return 0.0f;
}

void NeuralAssociativeMemory::updateAssociation(const std::vector<float>& patternA,
                                                const std::vector<float>& patternB,
                                                float delta) {
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    for (auto& assoc : associations_) {
        if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
            (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
            std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + delta, 0.0f, 1.0f);
            return;
        }
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::spreadActivation(const std::vector<float>& cuePattern,
                                                                          size_t steps) const {
    std::vector<std::vector<float>> currentLevel = {cuePattern};
    std::vector<std::vector<float>> nextLevel;
    
    for (size_t step = 0; step < steps; ++step) {
        for (const auto& pattern : currentLevel) {
            // Find associated patterns
            std::vector<float> associatedPattern;
            if (findPatternNeuron(pattern) != std::numeric_limits<NeuronId>::max()) {
                // This is a simplified implementation - in reality, you'd
                // use the actual association graph
            }
        }
        currentLevel = nextLevel;
    }
    
    return currentLevel;
}

void NeuralAssociativeMemory::integrateWithSTDP(STDP* stdp) {
    if (!stdp) return;
    
    // Integrate STDP with associative memory
    // This allows spike timing to modify associations
}

void NeuralAssociativeMemory::integrateWithHebbian(Hebbian* hebbian) {
    if (!hebbian) return;
    
    // Integrate Hebbian learning with associative memory
    // This allows co-activation to strengthen associations
}

void NeuralAssociativeMemory::strengthenAssociations(float factor) {
    for (auto& assoc : associations_) {
        std::get<2>(assoc) = std::min(1.0f, std::get<2>(assoc) * factor);
    }
}

void NeuralAssociativeMemory::decayAssociations(float factor) {
    for (auto& assoc : associations_) {
        std::get<2>(assoc) *= factor;
    }
}

void NeuralAssociativeMemory::cleanupWeakAssociations(float minStrength) {
    auto newEnd = std::remove_if(associations_.begin(), associations_.end(),
        [minStrength](const auto& assoc) {
            return std::get<2>(assoc) < minStrength;
        });
    
    associations_.erase(newEnd, associations_.end());
}

void NeuralAssociativeMemory::compressAssociations() {
    // Remove weakest associations to maintain memory efficiency
    std::sort(associations_.begin(), associations_.end(),
              [](const auto& a, const auto& b) {
                  return std::get<2>(a) < std::get<2>(b);
              });
    
    size_t keepCount = std::min(pImpl->maxPatterns, associations_.size());
    associations_.resize(keepCount);
}

NeuronId NeuralAssociativeMemory::findPatternNeuron(const std::vector<float>& pattern) {
    // Find existing neuron for this pattern or create new one
    for (size_t i = 0; i < pImpl->patternNeurons.size(); ++i) {
        if (computeSimilarity(pattern, pImpl->patternNeurons[i]) > 0.9f) {
            return static_cast<NeuronId>(i);
        }
    }
    
    // Create new neuron
    pImpl->patternNeurons.push_back(pattern);
    pImpl->activationCounts.push_back(0);
    return static_cast<NeuronId>(pImpl->patternNeurons.size() - 1);
}

float NeuralAssociativeMemory::computeSimilarity(const std::vector<float>& a,
                                                const std::vector<float>& b) const {
    if (a.empty() || b.empty()) return 0.0f;
    
    size_t minSize = std::min(a.size(), b.size());
    float dotProduct = 0.0f;
    
    for (size_t i = 0; i < minSize; ++i) {
        dotProduct += a[i] * b[i];
    }
    
    float magnitude1 = 0.0f, magnitude2 = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        magnitude1 += a[i] * a[i];
    }
    magnitude1 = std::sqrt(magnitude1);
    
    for (size_t i = 0; i < b.size(); ++i) {
        magnitude2 += b[i] * b[i];
    }
    magnitude2 = std::sqrt(magnitude2);
    
    if (magnitude1 == 0.0f || magnitude2 == 0.0f) return 0.0f;
    
    return dotProduct / (magnitude1 * magnitude2);
}

// ============================================================================
// MEMORY SYSTEM MANAGER IMPLEMENTATION
// ============================================================================

MemorySystemManager::MemorySystemManager() : 
    brain_(nullptr), currentTimeStep_(0), stepsSinceLastReplay_(0), 
    stepsSinceLastConsolidation_(0), replayInterval_(100), 
    consolidationInterval_(1000), replayEnabled_(false), 
    consolidationEnabled_(true), systemsInitialized_(false),
    workingMemoryCapacity_(100), episodicMemoryCapacity_(1000),
    associationMinStrength_(0.1f), maxAssociations_(1000) {}

MemorySystemManager::~MemorySystemManager() {
    destroyMemorySystems();
}

bool MemorySystemManager::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize MemorySystemManager with null brain");
        return false;
    }
    
    brain_ = brain;
    createMemorySystems();
    integrateAllSystems();
    
    systemsInitialized_ = true;
    
    NLM_LOG_INFO("MemorySystemManager initialized successfully");
    return true;
}

void MemorySystemManager::createMemorySystems() {
    workingMemory_ = std::make_unique<NeuralWorkingMemory>();
    episodicMemory_ = std::make_unique<NeuralEpisodicMemory>();
    associativeMemory_ = std::make_unique<NeuralAssociativeMemory>();
    
    workingMemory_->initialize(brain_);
    episodicMemory_->initialize(brain_);
    associativeMemory_->initialize(brain_);
}

void MemorySystemManager::destroyMemorySystems() {
    workingMemory_.reset();
    episodicMemory_.reset();
    associativeMemory_.reset();
    
    NLM_LOG_INFO("Memory systems destroyed");
}

void MemorySystemManager::initializeMemorySystems() {
    if (!brain_) return;
    
    workingMemory_->initialize(brain_);
    episodicMemory_->initialize(brain_);
    associativeMemory_->initialize(brain_);
}

void MemorySystemManager::updateAll(TimestepDuration dt) {
    if (!brain_ || !systemsInitialized_) return;
    
    // Update time step
    currentTimeStep_++;
    
    // Update each memory system
    workingMemory_->update(dt);
    episodicMemory_->update(dt);
    associativeMemory_->update(dt);
    
    // Update system state
    stepsSinceLastReplay_++;
    stepsSinceLastConsolidation_++;
    
    // Update replay schedule
    updateReplaySchedule();
    
    // Update consolidation schedule
    updateConsolidationSchedule();
}

void MemorySystemManager::consolidateAll() {
    if (!brain_) return;
    
    workingMemory_->consolidateWithEpisodicMemory(episodicMemory_.get());
    episodicMemory_->consolidate(consolidationThreshold_);
    
    stepsSinceLastConsolidation_ = 0;
}

void MemorySystemManager::clearAll() {
    if (!brain_) return;
    
    workingMemory_->clear();
    episodicMemory_->clear();
    associativeMemory_->clear();
}

void MemorySystemManager::enableReplay(bool enable) {
    replayEnabled_ = enable;
    stepsSinceLastReplay_ = 0;
}

void MemorySystemManager::setReplayInterval(size_t steps) {
    replayInterval_ = steps;
}

void MemorySystemManager::enableConsolidation(bool enable) {
    consolidationEnabled_ = enable;
    stepsSinceLastConsolidation_ = 0;
}

void MemorySystemManager::setConsolidationInterval(size_t steps) {
    consolidationInterval_ = steps;
}

void MemorySystemManager::logStatus() const {
    NLM_LOG_INFO("=== Unified Memory System Status ===");
    NLM_LOG_INFO("Current time step: " + std::to_string(currentTimeStep_));
    NLM_LOG_INFO("Replay enabled: " + std::string(replayEnabled_ ? "yes" : "no"));
    NLM_LOG_INFO("Replay interval: " + std::to_string(replayInterval_));
    NLM_LOG_INFO("Consolidation enabled: " + std::string(consolidationEnabled_ ? "yes" : "no"));
    NLM_LOG_INFO("Consolidation interval: " + std::to_string(consolidationInterval_));
    
    if (workingMemory_) workingMemory_->logStatus();
    if (episodicMemory_) episodicMemory_->logStatus();
    if (associativeMemory_) associativeMemory_->logStatus();
}

float MemorySystemManager::getSystemIntegrity() const {
    if (!systemsInitialized_) return 0.0f;
    
    float workingMemIntegrity = workingMemory_ ? workingMemory_->getMemoryActivity() : 0.0f;
    float episodicMemIntegrity = episodicMemory_ ? episodicMemory_->getAverageReward() : 0.0f;
    float associativeMemIntegrity = associativeMemory_ ? associativeMemory_->getAssociationCount() : 0.0f;
    
    return (workingMemIntegrity + episodicMemIntegrity + associativeMemIntegrity) / 3.0f;
}

size_t MemorySystemManager::getTotalMemoryUsage() const {
    size_t totalUsage = 0;
    if (workingMemory_) totalUsage += workingMemory_->getActiveTraceCount() * sizeof(float);
    if (episodicMemory_) totalUsage += episodicMemory_->getEpisodeCount() * sizeof(EpisodicMemoryItem);
    if (associativeMemory_) totalUsage += associativeMemory_->getAssociationCount() * sizeof(float);
    
    return totalUsage;
}

float MemorySystemManager::getMemoryEfficiency() const {
    float totalCapacity = workingMemoryCapacity_ + episodicMemoryCapacity_ + maxAssociations_;
    float totalUsage = getTotalMemoryUsage();
    
    if (totalCapacity == 0.0f) return 0.0f;
    
    return totalUsage / totalCapacity;
}

void MemorySystemManager::integrateAllSystems() {
    if (!brain_ || !systemsInitialized_) return;
    
    integrateWorkingEpisodicMemory();
    integrateEpisodicAssociativeMemory();
}

void MemorySystemManager::integrateWorkingEpisodicMemory() {
    if (!workingMemory_ || !episodicMemory_) return;
    
    workingMemory_->integrateWithEpisodicMemory(episodicMemory_.get());
}

void MemorySystemManager::integrateEpisodicAssociativeMemory() {
    if (!episodicMemory_ || !associativeMemory_) return;
    
    // Integration between episodic and associative memory
}

void MemorySystemManager::updateReplaySchedule() {
    if (!replayEnabled_ || replayInterval_ == 0) return;
    
    if (stepsSinceLastReplay_ >= replayInterval_) {
        // Trigger replay
        size_t episodeCount = episodicMemory_ ? episodicMemory_->getEpisodeCount() : 0;
        if (episodeCount > 0) {
            std::vector<size_t> episodeIds;
            for (size_t i = 0; i < std::min<size_t>(10, episodeCount); ++i) {
                episodeIds.push_back(i);
            }
            
            if (episodicMemory_) {
                episodicMemory_->replaySequence(episodeIds);
            }
        }
        
        stepsSinceLastReplay_ = 0;
    }
}

void MemorySystemManager::updateConsolidationSchedule() {
    if (!consolidationEnabled_ || consolidationInterval_ == 0) return;
    
    if (stepsSinceLastConsolidation_ >= consolidationInterval_) {
        consolidateAll();
    }
}

void MemorySystemManager::cleanupExpiredMemory() {
    if (episodicMemory_) {
        episodicMemory_->cleanupExpiredEpisodes(10000); // Max age
    }
}

void MemorySystemManager::optimizeMemoryUsage() {
    if (episodicMemory_) {
        episodicMemory_->compressMemory(episodicMemoryCapacity_);
    }
    
    if (associativeMemory_) {
        associativeMemory_->compressAssociations();
    }
}

void MemorySystemManager::configureMemorySystem(const std::string& configFile) {
    // Load memory system configuration from file
    std::ifstream file(configFile);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Could not open memory system config file: " + configFile);
        return;
    }
    
    // Parse configuration (simplified)
    // This would typically use a proper config parser
}

void MemorySystemManager::setMemoryCapacity(size_t workingMemSize, size_t episodicMemSize) {
    workingMemoryCapacity_ = workingMemSize;
    episodicMemoryCapacity_ = episodicMemSize;
    
    // Apply to systems if they exist
    if (workingMemory_) {
        // Would need to adjust internal working memory capacity
    }
    if (episodicMemory_) {
        // Would need to adjust internal episodic memory capacity
    }
}

void MemorySystemManager::setAssociationParameters(float minStrength, size_t maxAssociations) {
    associationMinStrength_ = minStrength;
    maxAssociations_ = maxAssociations;
    
    if (associativeMemory_) {
        // Apply to associative memory system
    }
}

// ============================================================================
// GLOBAL MEMORY SYSTEM FUNCTIONS
// ============================================================================

void initializeMemorySystem(Brain* brain) {
    if (gMemorySystem) {
        gMemorySystem->destroyMemorySystems();
    }
    
    gMemorySystem = std::make_unique<MemorySystemManager>();
    gMemorySystem->initialize(brain);
}

void shutdownMemorySystem() {
    if (gMemorySystem) {
        gMemorySystem->clearAll();
        gMemorySystem.reset();
    }
}

NeuralWorkingMemory* getWorkingMemory() {
    return gMemorySystem ? gMemorySystem->getWorkingMemory() : nullptr;
}

NeuralEpisodicMemory* getEpisodicMemory() {
    return gMemorySystem ? gMemorySystem->getEpisodicMemory() : nullptr;
}

NeuralAssociativeMemory* getAssociativeMemory() {
    return gMemorySystem ? gMemorySystem->getAssociativeMemory() : nullptr;
}

} // namespace nlm
