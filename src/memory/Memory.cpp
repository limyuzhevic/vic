#include "Memory.hpp"
#include <algorithm>
#include <numeric>
#include <random>
#include <cmath>

namespace nlm {

// Working Memory Implementation
struct WorkingMemory::Impl {
    // Core storage for neuron activations
    std::unordered_map<NeuronId, float> activations;
    std::unordered_map<NeuronId, std::unordered_map<std::string, std::string>> metadata;
    
    // Competition and normalization state
    std::vector<float> competitionWeights;
    float globalGain;
    float globalInhibition;
    
    // Decay and plasticity state
    std::unordered_map<NeuronId, SimulationStep> lastUpdated;
    SimulationStep currentTime;
    
    // Integration state
    std::unordered_map<NeuronId, size_t> episodicReferences;
    
    // Configuration
    MemoryConfig config;
    size_t timeStepCounter;
    
    Impl(const MemoryConfig& cfg) 
        : globalGain(1.0f), globalInhibition(0.5f), 
          currentTime(0), timeStepCounter(0), config(cfg) {
        competitionWeights.resize(cfg.workingMemoryCapacity, 1.0f);
    }
};

WorkingMemory::WorkingMemory(const MemoryConfig& config) : pImpl(std::make_unique<Impl>(config)) {
}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::store(const std::string& key, const std::vector<float>& value,
                         const std::unordered_map<std::string, std::string>& metadata) {
    // Convert string key to neuron ID for backward compatibility
    // In real implementation, this would map to actual neurons
    std::hash<std::string> hasher;
    uint64_t hashValue = hasher(key);
    NeuronId neuronId(hashValue);
    
    storeActivation(neuronId, !value.empty() ? value[0] : 0.0f);
    
    // Store metadata
    for (const auto& pair : metadata) {
        pImpl->metadata[neuronId][pair.first] = pair.second;
    }
}

std::vector<float> WorkingMemory::retrieve(const std::string& key) const {
    std::hash<std::string> hasher;
    uint64_t hashValue = hasher(key);
    NeuronId neuronId(hashValue);
    
    std::vector<float> result;
    if (pImpl->activations.find(neuronId) != pImpl->activations.end()) {
        result.push_back(pImpl->activations.at(neuronId));
    }
    return result;
}

bool WorkingMemory::exists(const std::string& key) const {
    std::hash<std::string> hasher;
    uint64_t hashValue = hasher(key);
    NeuronId neuronId(hashValue);
    return pImpl->activations.find(neuronId) != pImpl->activations.end();
}

void WorkingMemory::clear() {
    pImpl->activations.clear();
    pImpl->metadata.clear();
    pImpl->episodicReferences.clear();
}

size_t WorkingMemory::size() const {
    return pImpl->activations.size();
}

void WorkingMemory::update(float deltaTime) {
    pImpl->currentTime += 1;
    pImpl->timeStepCounter++;
    
    // Apply decay
    decay(pImpl->config.workingMemoryDecayRate);
    
    // Apply competition
    if (pImpl->timeStepCounter % 10 == 0) { // Every 10th update
        applyCompetition();
    }
    
    // Update episodic references
    for (const auto& ref : pImpl->episodicReferences) {
        auto it = pImpl->activations.find(ref.first);
        if (it != pImpl->activations.end()) {
            it->second *= (1.0f - pImpl->config.workingMemoryDecayRate);
        }
    }
}

void WorkingMemory::storeActivation(const NeuronId& neuron, float activation) {
    pImpl->activations[neuron] = std::clamp(activation, 0.0f, 1.0f);
    pImpl->lastUpdated[neuron] = pImpl->currentTime;
}

float WorkingMemory::getActivation(const NeuronId& neuron) const {
    auto it = pImpl->activations.find(neuron);
    if (it != pImpl->activations.end()) {
        return it->second;
    }
    return 0.0f;
}

bool WorkingMemory::hasNeuron(const NeuronId& neuron) const {
    return pImpl->activations.find(neuron) != pImpl->activations.end();
}

void WorkingMemory::clearNeuron(const NeuronId& neuron) {
    pImpl->activations.erase(neuron);
    pImpl->lastUpdated.erase(neuron);
    pImpl->episodicReferences.erase(neuron);
}

void WorkingMemory::applyCompetition() {
    if (pImpl->activations.empty()) return;
    
    // Find maximum activation
    float maxActivation = 0.0f;
    for (const auto& pair : pImpl->activations) {
        maxActivation = std::max(maxActivation, pair.second);
    }
    
    if (maxActivation > 0.0f) {
        // Apply soft competition
        float totalNormalized = 0.0f;
        std::vector<std::pair<NeuronId, float>> normalized;
        
        for (const auto& pair : pImpl->activations) {
            float normalizedValue = pair.second / maxActivation;
            normalized.emplace_back(pair.first, normalizedValue);
            totalNormalized += normalizedValue;
        }
        
        // Normalize to maintain sum
        if (totalNormalized > 0.0f) {
            for (auto& pair : normalized) {
                pair.second /= totalNormalized;
            }
        }
        
        // Update activations
        pImpl->activations.clear();
        for (const auto& pair : normalized) {
            pImpl->activations[pair.first] = pair.second;
        }
    }
}

void WorkingMemory::applyNormalization() {
    if (pImpl->activations.empty()) return;
    
    // Apply normalization based on capacity
    size_t currentSize = pImpl->activations.size();
    size_t capacity = pImpl->config.workingMemoryCapacity;
    
    if (currentSize > capacity) {
        // Remove weakest activations
        std::vector<std::pair<NeuronId, float>> sorted;
        for (const auto& pair : pImpl->activations) {
            sorted.emplace_back(pair.first, pair.second);
        }
        
        std::sort(sorted.begin(), sorted.end(),
                 [](const auto& a, const auto& b) { return a.second < b.second; });
        
        pImpl->activations.clear();
        for (size_t i = 0; i < capacity && i < sorted.size(); ++i) {
            pImpl->activations[sorted[i].first] = sorted[i].second;
        }
    }
}

size_t WorkingMemory::getCapacity() const {
    return pImpl->config.workingMemoryCapacity;
}

size_t WorkingMemory::getCurrentSize() const {
    return pImpl->activations.size();
}

float WorkingMemory::getLoadPercentage() const {
    if (pImpl->config.workingMemoryCapacity == 0) return 0.0f;
    return static_cast<float>(pImpl->activations.size()) / 
           static_cast<float>(pImpl->config.workingMemoryCapacity);
}

void WorkingMemory::decay(float decayRate) {
    for (auto& pair : pImpl->activations) {
        pair.second *= (1.0f - decayRate);
        if (pair.second < 0.001f) {
            pImpl->activations.erase(pair.first);
        }
    }
}

void WorkingMemory::applyPlasticity(const std::vector<float>& inputPattern, float strength) {
    // Apply Hebbian-like plasticity based on input pattern
    float totalInput = std::accumulate(inputPattern.begin(), inputPattern.end(), 0.0f);
    if (totalInput > 0.0f) {
        for (auto& pair : pImpl->activations) {
            pair.second += strength * totalInput * (1.0f - pair.second);
            pair.second = std::clamp(pair.second, 0.0f, 1.0f);
        }
    }
}

std::string WorkingMemory::getEpisodicMemoryKey(const NeuronId& neuron) const {
    auto it = pImpl->episodicReferences.find(neuron);
    if (it != pImpl->episodicReferences.end()) {
        return std::to_string(it->second);
    }
    return "";
}

void WorkingMemory::storeToEpisodicMemory(const NeuronId& neuron, float value) {
    // Reference episodic memory
    pImpl->episodicReferences[neuron] = pImpl->currentTime % 1000; // Simplified
}

// Episodic Memory Implementation
struct EpisodicMemory::Impl {
    std::vector<MemoryEpisode> episodes;
    std::unordered_map<NeuronId, size_t> neuronEpisodeMap;
    std::unordered_map<std::string, std::vector<size_t>> contextIndex;
    
    // Replay and consolidation state
    std::vector<size_t> replayQueue;
    size_t currentReplayIndex;
    float replayStrength;
    
    // Pattern completion state
    std::vector<std::vector<float>> patternMatrix;
    std::vector<std::vector<float>> similarityMatrix;
    
    // Configuration
    MemoryConfig config;
    SimulationStep currentTime;
    
    Impl(const MemoryConfig& cfg) 
        : currentReplayIndex(0), replayStrength(0.8f), currentTime(0), config(cfg) {}
};

EpisodicMemory::EpisodicMemory(const MemoryConfig& config) : pImpl(std::make_unique<Impl>(config)) {
}

EpisodicMemory::~EpisodicMemory() = default;

void EpisodicMemory::store(const std::string& key, const std::vector<float>& value,
                         const std::unordered_map<std::string, std::string>& metadata) {
    // Create episode from input
    MemoryEpisode episode;
    episode.timestamp = pImpl->currentTime++;
    episode.replayTime = episode.timestamp;
    episode.relevanceScore = calculateRelevance(metadata);
    episode.contextSignature = generateContextSignature(metadata);
    
    // Extract neuron pattern from key (simplified)
    std::hash<std::string> hasher;
    uint64_t hashValue = hasher(key);
    NeuronId neuronId(hashValue);
    episode.neuronPattern.push_back(neuronId);
    
    // Extract activation pattern from value
    episode.activationPattern = value;
    
    // Store in memory
    storeEpisode(episode);
}

std::vector<float> EpisodicMemory::retrieve(const std::string& key) const {
    std::vector<float> result;
    
    // Find episode by key (simplified search)
    for (const auto& episode : pImpl->episodes) {
        if (!episode.contextSignature.empty() && 
            episode.contextSignature.find(key) != std::string::npos) {
            result = episode.activationPattern;
            break;
        }
    }
    
    return result;
}

bool EpisodicMemory::exists(const std::string& key) const {
    return !retrieve(key).empty();
}

void EpisodicMemory::clear() {
    pImpl->episodes.clear();
    pImpl->neuronEpisodeMap.clear();
    pImpl->contextIndex.clear();
    pImpl->patternMatrix.clear();
    pImpl->similarityMatrix.clear();
}

size_t EpisodicMemory::size() const {
    return pImpl->episodes.size();
}

void EpisodicMemory::update(float deltaTime) {
    pImpl->currentTime += static_cast<SimulationStep>(deltaTime * 1000.0f);
    
    // Update replay queue
    updateReplayQueue();
}

void EpisodicMemory::storeEpisode(const MemoryEpisode& episode) {
    // Add to episodes
    pImpl->episodes.push_back(episode);
    
    // Update indexes
    for (const auto& neuron : episode.neuronPattern) {
        pImpl->neuronEpisodeMap[neuron] = pImpl->episodes.size() - 1;
    }
    
    // Index by context
    if (!episode.contextSignature.empty()) {
        pImpl->contextIndex[episode.contextSignature].push_back(pImpl->episodes.size() - 1);
    }
    
    // Update pattern matrix for completion
    updatePatternMatrix();
}

MemoryEpisode EpisodicMemory::retrieveEpisode(size_t index) const {
    if (index < pImpl->episodes.size()) {
        return pImpl->episodes[index];
    }
    return MemoryEpisode();
}

size_t EpisodicMemory::getEpisodeCount() const {
    return pImpl->episodes.size();
}

std::vector<MemoryEpisode> EpisodicMemory::getRecentEpisodes(size_t count) const {
    std::vector<MemoryEpisode> result;
    size_t start = std::max(static_cast<size_t>(0), 
                          static_cast<size_t>(pImpl->episodes.size()) - count);
    
    for (size_t i = start; i < pImpl->episodes.size(); ++i) {
        result.push_back(pImpl->episodes[i]);
    }
    
    return result;
}

std::vector<MemoryEpisode> EpisodicMemory::getReplayEpisodes(size_t count, float minRelevance) const {
    std::vector<MemoryEpisode> result;
    
    // Select episodes based on relevance and recency
    std::vector<std::pair<size_t, float>> episodeScores;
    
    for (size_t i = 0; i < pImpl->episodes.size(); ++i) {
        float score = pImpl->episodes[i].relevanceScore;
        // Boost score for recent episodes
        size_t age = static_cast<size_t>(pImpl->currentTime - pImpl->episodes[i].timestamp);
        score *= std::max(0.5f, 1.0f - static_cast<float>(age) / 1000.0f);
        
        episodeScores.emplace_back(i, score);
    }
    
    // Sort by score
    std::sort(episodeScores.begin(), episodeScores.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Select top episodes
    size_t selectedCount = std::min(count, episodeScores.size());
    for (size_t i = 0; i < selectedCount; ++i) {
        size_t index = episodeScores[i].first;
        if (episodeScores[i].second >= minRelevance) {
            result.push_back(pImpl->episodes[index]);
        }
    }
    
    return result;
}

void EpisodicMemory::replayEpisode(size_t index, std::vector<float>& outPattern) const {
    if (index < pImpl->episodes.size()) {
        outPattern = pImpl->episodes[index].activationPattern;
        
        // Apply replay strength
        for (auto& value : outPattern) {
            value *= pImpl->replayStrength;
        }
    }
}

void EpisodicMemory::integrateWithWorkingMemory(const WorkingMemory& workingMem) {
    // Transfer recent activations to episodic memory
    for (const auto& pair : workingMem.pImpl->activations) {
        NeuronId neuron = pair.first;
        float activation = pair.second;
        
        // Find corresponding episode or create new one
        auto it = pImpl->neuronEpisodeMap.find(neuron);
        if (it != pImpl->neuronEpisodeMap.end()) {
            size_t episodeIndex = it->second;
            if (episodeIndex < pImpl->episodes.size()) {
                // Update existing episode
                pImpl->episodes[episodeIndex].activationPattern.push_back(activation);
            }
        }
    }
}

std::vector<NeuronId> EpisodicMemory::getReplicatedNeurons() const {
    std::vector<NeuronId> result;
    for (const auto& pair : pImpl->neuronEpisodeMap) {
        result.push_back(pair.first);
    }
    return result;
}

void EpisodicMemory::consolidateEpisodes(float threshold) {
    std::vector<MemoryEpisode> consolidated;
    
    for (const auto& episode : pImpl->episodes) {
        if (episode.relevanceScore >= threshold) {
            consolidated.push_back(episode);
        }
    }
    
    pImpl->episodes = consolidated;
    updateIndexes();
}

void EpisodicMemory::pruneOldEpisodes(size_t maxCount) {
    if (pImpl->episodes.size() <= maxCount) return;
    
    std::vector<MemoryEpisode> pruned;
    size_t start = pImpl->episodes.size() - maxCount;
    
    for (size_t i = start; i < pImpl->episodes.size(); ++i) {
        pruned.push_back(pImpl->episodes[i]);
    }
    
    pImpl->episodes = pruned;
    updateIndexes();
}

std::vector<float> EpisodicMemory::completePattern(const std::vector<float>& partialPattern) const {
    std::vector<float> completedPattern = partialPattern;
    
    if (completedPattern.empty() && !pImpl->episodes.empty()) {
        // Use first episode as base
        completedPattern = pImpl->episodes[0].activationPattern;
    }
    
    // Simple completion based on pattern completion
    for (auto& value : completedPattern) {
        value = std::min(value * 1.5f, 1.0f);
    }
    
    return completedPattern;
}

float EpisodicMemory::getPatternSimilarity(const std::vector<float>& pattern1,
                                          const std::vector<float>& pattern2) const {
    if (pattern1.empty() || pattern2.empty()) return 0.0f;
    
    size_t minSize = std::min(pattern1.size(), pattern2.size());
    float similarity = 0.0f;
    
    for (size_t i = 0; i < minSize; ++i) {
        similarity += std::abs(pattern1[i] - pattern2[i]);
    }
    
    return 1.0f - (similarity / minSize);
}

float EpisodicMemory::getEpisodeRelevance(size_t index) const {
    if (index < pImpl->episodes.size()) {
        return pImpl->episodes[index].relevanceScore;
    }
    return 0.0f;
}

void EpisodicMemory::updateEpisodeRelevance(size_t index, float delta) {
    if (index < pImpl->episodes.size()) {
        pImpl->episodes[index].relevanceScore = 
            std::clamp(pImpl->episodes[index].relevanceScore + delta, 0.0f, 1.0f);
    }
}

void EpisodicMemory::updateReplayQueue() {
    if (pImpl->episodes.empty()) return;
    
    // Simple queue management - all episodes are in queue
    pImpl->replayQueue.clear();
    for (size_t i = 0; i < pImpl->episodes.size(); ++i) {
        pImpl->replayQueue.push_back(i);
    }
}

void EpisodicMemory::updatePatternMatrix() {
    size_t episodeCount = pImpl->episodes.size();
    pImpl->patternMatrix.clear();
    pImpl->similarityMatrix.clear();
    
    if (episodeCount == 0) return;
    
    pImpl->patternMatrix.resize(episodeCount, std::vector<float>(5, 0.0f)); // 5-dimensional pattern
    pImpl->similarityMatrix.resize(episodeCount, std::vector<float>(episodeCount, 0.0f));
    
    for (size_t i = 0; i < episodeCount; ++i) {
        const auto& episode = pImpl->episodes[i];
        size_t patternSize = std::min(episode.activationPattern.size(), 
                                    static_cast<size_t>(5));
        
        for (size_t j = 0; j < patternSize; ++j) {
            pImpl->patternMatrix[i][j] = episode.activationPattern[j];
        }
    }
    
    // Calculate similarity matrix
    for (size_t i = 0; i < episodeCount; ++i) {
        for (size_t j = i + 1; j < episodeCount; ++j) {
            float similarity = getPatternSimilarity(pImpl->patternMatrix[i], 
                                                  pImpl->patternMatrix[j]);
            pImpl->similarityMatrix[i][j] = similarity;
            pImpl->similarityMatrix[j][i] = similarity;
        }
    }
}

void EpisodicMemory::updateIndexes() {
    pImpl->neuronEpisodeMap.clear();
    pImpl->contextIndex.clear();
    
    for (size_t i = 0; i < pImpl->episodes.size(); ++i) {
        const auto& episode = pImpl->episodes[i];
        
        for (const auto& neuron : episode.neuronPattern) {
            pImpl->neuronEpisodeMap[neuron] = i;
        }
        
        if (!episode.contextSignature.empty()) {
            pImpl->contextIndex[episode.contextSignature].push_back(i);
        }
    }
}

float EpisodicMemory::calculateRelevance(const std::unordered_map<std::string, std::string>& metadata) const {
    float relevance = 0.0f;
    
    // Simple relevance calculation based on metadata
    if (metadata.find("reward") != metadata.end()) {
        relevance += 0.5f;
    }
    if (metadata.find("action") != metadata.end()) {
        relevance += 0.3f;
    }
    if (metadata.find("input") != metadata.end()) {
        relevance += 0.2f;
    }
    
    return std::min(relevance, 1.0f);
}

std::string EpisodicMemory::generateContextSignature(const std::unordered_map<std::string, std::string>& metadata) const {
    std::string signature;
    
    // Generate simple signature from metadata
    for (const auto& pair : metadata) {
        signature += pair.first + "=" + pair.second + ";";
    }
    
    return signature;
}

// Semantic Memory Implementation
struct SemanticMemory::Impl {
    std::unordered_map<std::string, std::vector<std::string>> conceptInstances;
    std::unordered_map<std::string, std::vector<float>> conceptActivations;
    std::unordered_map<std::string, std::vector<std::string>> conceptCategories;
    std::unordered_map<std::string, std::unordered_map<std::string, float>> conceptRelationships;
    
    // Activation spreading
    std::unordered_map<std::string, std::vector<float>> conceptSpreading;
    std::unordered_map<std::string, float> conceptActivityLevel;
    
    // Configuration
    MemoryConfig config;
    
    Impl(const MemoryConfig& cfg) : config(cfg) {}
};

SemanticMemory::SemanticMemory(const MemoryConfig& config) : pImpl(std::make_unique<Impl>(config)) {
}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::store(const std::string& key, const std::vector<float>& value,
                         const std::unordered_map<std::string, std::string>& metadata) {
    storeConcept(key, extractInstances(metadata), value);
}

std::vector<float> SemanticMemory::retrieve(const std::string& key) const {
    auto it = pImpl->conceptActivations.find(key);
    if (it != pImpl->conceptActivations.end()) {
        return it->second;
    }
    return std::vector<float>();
}

bool SemanticMemory::exists(const std::string& key) const {
    return pImpl->conceptActivations.find(key) != pImpl->conceptActivations.end();
}

void SemanticMemory::clear() {
    pImpl->conceptInstances.clear();
    pImpl->conceptActivations.clear();
    pImpl->conceptCategories.clear();
    pImpl->conceptRelationships.clear();
    pImpl->conceptSpreading.clear();
    pImpl->conceptActivityLevel.clear();
}

size_t SemanticMemory::size() const {
    return pImpl->conceptInstances.size();
}

void SemanticMemory::update(float deltaTime) {
    // Spread activation through semantic network
    spreadActivation(deltaTime);
}

void SemanticMemory::storeConcept(const std::string& concept, 
                                 const std::vector<std::string>& instances,
                                 const std::vector<float>& activationPattern) {
    pImpl->conceptInstances[concept] = instances;
    pImpl->conceptActivations[concept] = activationPattern;
    
    // Initialize activation
    pImpl->conceptActivityLevel[concept] = 0.1f;
}

std::vector<std::string> SemanticMemory::retrieveConcept(const std::string& concept) const {
    auto it = pImpl->conceptInstances.find(concept);
    if (it != pImpl->conceptInstances.end()) {
        return it->second;
    }
    return std::vector<std::string>();
}

std::vector<float> SemanticMemory::getConceptActivation(const std::string& concept) const {
    auto it = pImpl->conceptActivations.find(concept);
    if (it != pImpl->conceptActivations.end()) {
        return it->second;
    }
    return std::vector<float>();
}

void SemanticMemory::createCategory(const std::string& parent, const std::string& child) {
    pImpl->conceptCategories[child].push_back(parent);
}

std::vector<std::string> SemanticMemory::getSubcategories(const std::string& parent) const {
    std::vector<std::string> result;
    for (const auto& pair : pImpl->conceptCategories) {
        for (const auto& cat : pair.second) {
            if (cat == parent) {
                result.push_back(pair.first);
                break;
            }
        }
    }
    return result;
}

std::vector<std::string> SemanticMemory::getSupercategories(const std::string& child) const {
    auto it = pImpl->conceptCategories.find(child);
    if (it != pImpl->conceptCategories.end()) {
        return it->second;
    }
    return std::vector<std::string>();
}

void SemanticMemory::establishRelationship(const std::string& concept1, const std::string& concept2,
                                          const std::string& relationshipType, float strength) {
    pImpl->conceptRelationships[concept1][concept2] = strength;
    pImpl->conceptRelationships[concept2][concept1] = strength;
}

std::vector<std::pair<std::string, float>> SemanticMemory::getRelatedConcepts(const std::string& concept) const {
    std::vector<std::pair<std::string, float>> result;
    
    auto it1 = pImpl->conceptRelationships.find(concept);
    if (it1 != pImpl->conceptRelationships.end()) {
        for (const auto& pair : it1->second) {
            result.emplace_back(pair.first, pair.second);
        }
    }
    
    auto it2 = pImpl->conceptRelationships.find(concept);
    if (it2 != pImpl->conceptRelationships.end()) {
        for (const auto& pair : it2->second) {
            if (pair.first != concept) {
                result.emplace_back(pair.first, pair.second);
            }
        }
    }
    
    // Remove duplicates
    std::sort(result.begin(), result.end());
    result.erase(std::unique(result.begin(), result.end()), result.end());
    
    return result;
}

void SemanticMemory::spreadActivation(const std::vector<std::string>& activeConcepts) {
    // Clear previous spreading
    pImpl->conceptSpreading.clear();
    
    // Calculate spreading
    for (const auto& concept : activeConcepts) {
        spreadActivation(concept, 1.0f);
    }
    
    // Apply spreading over iterations
    for (int iteration = 0; iteration < 3; ++iteration) {
        for (auto& pair : pImpl->conceptSpreading) {
            for (const auto& related : pImpl->conceptRelationships[pair.first]) {
                pair.second += pImpl->conceptRelationships[pair.first][related] * 0.1f;
            }
        }
    }
}

void SemanticMemory::spreadActivation(const std::string& concept, float amount) {
    if (pImpl->conceptSpreading.find(concept) == pImpl->conceptSpreading.end()) {
        pImpl->conceptSpreading[concept] = 0.0f;
    }
    
    pImpl->conceptSpreading[concept] += amount;
    
    // Update activity level
    pImpl->conceptActivityLevel[concept] = 
        std::min(pImpl->conceptActivityLevel[concept] + amount, 1.0f);
}

std::vector<std::pair<std::string, float>> SemanticMemory::getAllActiveConcepts() const {
    std::vector<std::pair<std::string, float>> result;
    
    for (const auto& pair : pImpl->conceptActivityLevel) {
        if (pair.second > 0.01f) {
            result.emplace_back(pair.first, pair.second);
        }
    }
    
    // Sort by activity level
    std::sort(result.begin(), result.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    return result;
}

std::vector<std::string> SemanticMemory::inferFromConcepts(const std::vector<std::string>& inputConcepts) const {
    std::vector<std::string> inferred;
    
    for (const auto& concept : inputConcepts) {
        auto related = getRelatedConcepts(concept);
        for (const auto& pair : related) {
            if (std::find(inferred.begin(), inferred.end(), pair.first) == inferred.end()) {
                inferred.push_back(pair.first);
            }
        }
    }
    
    return inferred;
}

std::string SemanticMemory::getConceptExplanation(const std::string& concept) const {
    // Simple explanation based on relationships
    std::string explanation = "Concept: " + concept + "\n";
    
    auto related = getRelatedConcepts(concept);
    if (!related.empty()) {
        explanation += "Related concepts: ";
        for (size_t i = 0; i < std::min(related.size(), static_cast<size_t>(3)); ++i) {
            explanation += related[i].first;
            if (i < related.size() - 1) explanation += ", ";
        }
    }
    
    return explanation;
}

std::vector<std::string> SemanticMemory::extractInstances(const std::unordered_map<std::string, std::string>& metadata) const {
    std::vector<std::string> instances;
    
    for (const auto& pair : metadata) {
        if (pair.first.find("instance") != std::string::npos || 
            pair.first.find("object") != std::string::npos) {
            instances.push_back(pair.second);
        }
    }
    
    return instances;
}

// Procedural Memory Implementation
struct ProceduralMemory::Impl {
    std::vector<LearnedSkill> skills;
    std::unordered_map<std::string, size_t> skillIndex;
    
    // Execution tracking
    std::unordered_map<std::string, size_t> executionCount;
    std::unordered_map<std::string, float> skillProficiency;
    
    // Configuration
    MemoryConfig config;
    
    Impl(const MemoryConfig& cfg) : config(cfg) {}
};

ProceduralMemory::ProceduralMemory(const MemoryConfig& config) : pImpl(std::make_unique<Impl>(config)) {
}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::store(const std::string& key, const std::vector<float>& value,
                            const std::unordered_map<std::string, std::string>& metadata) {
    // Procedural memory stores skills, not raw data
    if (metadata.find("skill_name") != metadata.end()) {
        std::string skillName = metadata.at("skill_name");
        std::vector<NeuronId> pattern;
        std::vector<std::string> states;
        
        // Extract pattern from value (simplified)
        for (float v : value) {
            pattern.push_back(NeuronId(static_cast<uint64_t>(v * 1000)));
        }
        
        // Extract states from metadata
        for (const auto& pair : metadata) {
            if (pair.first.find("state") != std::string::npos) {
                states.push_back(pair.second);
            }
        }
        
        learnSkill(skillName, pattern, states);
    }
}

std::vector<float> ProceduralMemory::retrieve(const std::string& key) const {
    // Procedural memory doesn't store raw data, only skills
    return std::vector<float>();
}

bool ProceduralMemory::exists(const std::string& key) const {
    return skillIndex.find(key) != skillIndex.end();
}

void ProceduralMemory::clear() {
    pImpl->skills.clear();
    pImpl->skillIndex.clear();
    pImpl->executionCount.clear();
    pImpl->skillProficiency.clear();
}

size_t ProceduralMemory::size() const {
    return pImpl->skills.size();
}

void ProceduralMemory::update(float deltaTime) {
    // Update skill proficiency based on usage
    for (auto& skill : pImpl->skills) {
        if (skill.executionCount > 0) {
            float proficiencyGain = pImpl->config.skillLearningRate * 
                                  skill.successRate * skill.executionCount;
            skill.proficiency = std::min(skill.proficiency + proficiencyGain, 1.0f);
        }
    }
}

void ProceduralMemory::learnSkill(const std::string& skillName,
                                 const std::vector<NeuronId>& neuralPattern,
                                 const std::vector<std::string>& requiredStates) {
    // Check if skill already exists
    if (skillIndex.find(skillName) != skillIndex.end()) {
        // Update existing skill
        for (auto& skill : pImpl->skills) {
            if (skill.skillName == skillName) {
                skill.neuralPattern = neuralPattern;
                skill.requiredStates = requiredStates;
                return;
            }
        }
    }
    
    // Add new skill
    LearnedSkill newSkill(skillName, neuralPattern, requiredStates);
    pImpl->skills.push_back(newSkill);
    pImpl->skillIndex[skillName] = pImpl->skills.size() - 1;
}

LearnedSkill* ProceduralMemory::getSkill(const std::string& skillName) {
    auto it = skillIndex.find(skillName);
    if (it != skillIndex.end() && it->second < pImpl->skills.size()) {
        return &pImpl->skills[it->second];
    }
    return nullptr;
}

std::vector<LearnedSkill*> ProceduralMemory::getAllSkills() const {
    std::vector<LearnedSkill*> result;
    for (auto& skill : pImpl->skills) {
        result.push_back(&skill);
    }
    return result;
}

bool ProceduralMemory::executeSkill(const std::string& skillName,
                                   const std::vector<std::string>& currentStates,
                                   std::vector<std::string>& resultingStates) {
    LearnedSkill* skill = getSkill(skillName);
    if (!skill) return false;
    
    if (!skill->canExecute(currentStates)) {
        return false;
    }
    
    // Execute skill
    resultingStates = currentStates;
    // In real implementation, this would modify states based on skill
    
    // Record execution
    skill->executeCount++;
    skill->lastExecuted = pImpl->config.criticalMemoryGrowthPeriod;
    
    // Update proficiency
    skill->proficiency = (skill->proficiency + 0.1f) * 0.5f + 0.5f * skill->getExecutionProbability();
    
    return true;
}

void ProceduralMemory::updateProficiency(const std::string& skillName, float delta, bool success) {
    LearnedSkill* skill = getSkill(skillName);
    if (skill) {
        float oldProficiency = skill->proficiency;
        skill->proficiency = std::clamp(skill->proficiency + delta, 0.0f, 1.0f);
        
        if (success) {
            skill->successRate = (skill->successRate * 0.9f) + 0.1f;
        }
        
        // Record execution
        skill->executionCount++;
    }
}

void ProceduralMemory::practiceSkill(const std::string& skillName,
                                     const std::vector<std::string>& currentStates) {
    // Simply increase execution count for practice
    if (skillIndex.find(skillName) != skillIndex.end()) {
        pImpl->skills[skillIndex[skillName]].executionCount++;
        pImpl->skills[skillIndex[skillName]].lastExecuted = pImpl->config.criticalMemoryGrowthPeriod;
    }
}

void ProceduralMemory::transferSkill(const std::string& sourceSkill, const std::string& targetSkill,
                                     float transferRate) {
    // Transfer skill between skills (simplified)
    if (sourceSkill == targetSkill) return;
    
    auto sourceIt = skillIndex.find(sourceSkill);
    auto targetIt = skillIndex.find(targetSkill);
    
    if (sourceIt != skillIndex.end() && targetIt != skillIndex.end()) {
        // Blend skills
        size_t sourceIdx = sourceIt->second;
        size_t targetIdx = targetIt->second;
        
        // Blend neural patterns (simplified)
        std::vector<NeuronId> blendedPattern = pImpl->skills[sourceIdx].neuralPattern;
        blendedPattern.insert(blendedPattern.end(),
                             pImpl->skills[targetIdx].neuralPattern.begin(),
                             pImpl->skills[targetIdx].neuralPattern.end());
        
        pImpl->skills[targetIdx].neuralPattern = blendedPattern;
        pImpl->skills[targetIdx].proficiency = 
            (pImpl->skills[targetIdx].proficiency * (1.0f - transferRate)) +
            (pImpl->skills[sourceIdx].proficiency * transferRate);
    }
}

std::vector<std::string> ProceduralMemory::findMatchingSkills(const std::vector<std::string>& currentStates) const {
    std::vector<std::string> matchingSkills;
    
    for (const auto& skill : pImpl->skills) {
        if (skill.canExecute(currentStates)) {
            matchingSkills.push_back(skill.skillName);
        }
    }
    
    return matchingSkills;
}

float ProceduralMemory::getSkillReadiness(const std::string& skillName) const {
    auto it = skillIndex.find(skillName);
    if (it != skillIndex.end() && it->second < pImpl->skills.size()) {
        return pImpl->skills[it->second].getExecutionProbability();
    }
    return 0.0f;
}

// Associative Memory Implementation
struct AssociativeMemory::Impl {
    // Core associations
    std::vector<NeuralAssociation> associations;
    std::unordered_map<NeuronId, std::vector<size_t>> neuronAssociationIndices;
    std::unordered_map<std::string, std::vector<size_t>> contextAssociations;
    
    // Hebbian learning state
    std::vector<std::vector<float>> hebbianMatrix;
    float globalHebbianRate;
    
    // Configuration
    MemoryConfig config;
    SimulationStep currentTime;
    
    Impl(const MemoryConfig& cfg) 
        : globalHebbianRate(cfg.associativeHebbianEnabled ? 0.01f : 0.0f),
          currentTime(0), config(cfg) {}
};

AssociativeMemory::AssociativeMemory(const MemoryConfig& config) : pImpl(std::make_unique<Impl>(config)) {
}

AssociativeMemory::~AssociativeMemory() = default;

void AssociativeMemory::store(const std::string& key, const std::vector<float>& value,
                            const std::unordered_map<std::string, std::string>& metadata) {
    // Create association from key-value pair
    std::hash<std::string> hasher;
    uint64_t hashValue = hasher(key);
    NeuronId source = NeuronId(hashValue);
    
    if (!value.empty()) {
        NeuronId target = NeuronId(static_cast<uint64_t>(value[0] * 1000000));
        float strength = value.size() > 1 ? value[1] : 1.0f;
        
        createAssociation(source, target, strength);
    }
}

std::vector<float> AssociativeMemory::retrieve(const std::string& key) const {
    std::vector<float> result;
    std::hash<std::string> hasher;
    uint64_t hashValue = hasher(key);
    NeuronId source = NeuronId(hashValue);
    
    auto it = pImpl->neuronAssociationIndices.find(source);
    if (it != pImpl->neuronAssociationIndices.end()) {
        for (size_t index : it->second) {
            if (index < pImpl->associations.size()) {
                result.push_back(pImpl->associations[index].strength);
            }
        }
    }
    
    return result;
}

bool AssociativeMemory::exists(const std::string& key) const {
    std::hash<std::string> hasher;
    uint64_t hashValue = hasher(key);
    NeuronId source = NeuronId(hashValue);
    
    return pImpl->neuronAssociationIndices.find(source) != pImpl->neuronAssociationIndices.end();
}

void AssociativeMemory::clear() {
    pImpl->associations.clear();
    pImpl->neuronAssociationIndices.clear();
    pImpl->contextAssociations.clear();
    pImpl->hebbianMatrix.clear();
}

size_t AssociativeMemory::size() const {
    return pImpl->associations.size();
}

void AssociativeMemory::update(float deltaTime) {
    pImpl->currentTime += 1;
    
    // Apply decay to associations
    decayAssociations(pImpl->config.associativeDecayRate);
    
    // Update Hebbian learning
    updateHebbianLearning();
}

void AssociativeMemory::createAssociation(NeuronId source, NeuronId target, float strength) {
    // Check if association already exists
    for (size_t i = 0; i < pImpl->associations.size(); ++i) {
        const auto& assoc = pImpl->associations[i];
        if ((assoc.sourceNeuron == source && assoc.targetNeuron == target) ||
            (assoc.sourceNeuron == target && assoc.targetNeuron == source)) {
            // Update existing association
            pImpl->associations[i].strength = strength;
            pImpl->associations[i].lastActivation = pImpl->currentTime;
            return;
        }
    }
    
    // Add new association
    NeuralAssociation newAssoc(source, target, strength);
    pImpl->associations.push_back(newAssoc);
    
    // Update indexes
    pImpl->neuronAssociationIndices[source].push_back(pImpl->associations.size() - 1);
    pImpl->neuronAssociationIndices[target].push_back(pImpl->associations.size() - 1);
}

void AssociativeMemory::strengthenAssociation(NeuronId source, NeuronId target, float delta) {
    for (auto& assoc : pImpl->associations) {
        if ((assoc.sourceNeuron == source && assoc.targetNeuron == target) ||
            (assoc.sourceNeuron == target && assoc.targetNeuron == source)) {
            assoc.strength = std::min(assoc.strength + delta, 1.0f);
            break;
        }
    }
}

void AssociativeMemory::weakenAssociation(NeuronId source, NeuronId target, float delta) {
    for (auto& assoc : pImpl->associations) {
        if ((assoc.sourceNeuron == source && assoc.targetNeuron == target) ||
            (assoc.sourceNeuron == target && assoc.targetNeuron == source)) {
            assoc.strength = std::max(assoc.strength - delta, 0.0f);
            break;
        }
    }
}

std::vector<NeuronId> AssociativeMemory::getAssociatedNeurons(NeuronId neuron) const {
    std::vector<NeuronId> result;
    
    auto it = pImpl->neuronAssociationIndices.find(neuron);
    if (it != pImpl->neuronAssociationIndices.end()) {
        std::unordered_set<NeuronId> uniqueNeurons;
        
        for (size_t index : it->second) {
            if (index < pImpl->associations.size()) {
                const auto& assoc = pImpl->associations[index];
                NeuronId other = (assoc.sourceNeuron == neuron) ? 
                               assoc.targetNeuron : assoc.sourceNeuron;
                
                if (uniqueNeurons.find(other) == uniqueNeurons.end()) {
                    uniqueNeurons.insert(other);
                    result.push_back(other);
                }
            }
        }
    }
    
    return result;
}

float AssociativeMemory::getAssociationStrength(NeuronId source, NeuronId target) const {
    for (const auto& assoc : pImpl->associations) {
        if ((assoc.sourceNeuron == source && assoc.targetNeuron == target) ||
            (assoc.sourceNeuron == target && assoc.targetNeuron == source)) {
            return assoc.strength;
        }
    }
    return 0.0f;
}

void AssociativeMemory::hebbianLearning(const std::vector<NeuronId>& activeNeurons, float learningRate) {
    if (activeNeurons.size() < 2) return;
    
    // Create all-to-all associations among active neurons
    for (size_t i = 0; i < activeNeurons.size(); ++i) {
        for (size_t j = i + 1; j < activeNeurons.size(); ++j) {
            NeuronId src = activeNeurons[i];
            NeuronId tgt = activeNeurons[j];
            
            float currentStrength = getAssociationStrength(src, tgt);
            float newStrength = currentStrength + learningRate;
            
            strengthenAssociation(src, tgt, newStrength - currentStrength);
        }
    }
}

void AssociativeMemory::antiHebbianLearning(const std::vector<NeuronId>& activeNeurons, float learningRate) {
    if (activeNeurons.size() < 2) return;
    
    // Weaken associations among active neurons
    for (size_t i = 0; i < activeNeurons.size(); ++i) {
        for (size_t j = i + 1; j < activeNeurons.size(); ++j) {
            NeuronId src = activeNeurons[i];
            NeuronId tgt = activeNeurons[j];
            
            float currentStrength = getAssociationStrength(src, tgt);
            float newStrength = currentStrength - learningRate;
            
            weakenAssociation(src, tgt, currentStrength - newStrength);
        }
    }
}

void AssociativeMemory::setContextForAssociation(NeuronId source, NeuronId target,
                                                const std::vector<float>& context) {
    for (auto& assoc : pImpl->associations) {
        if ((assoc.sourceNeuron == source && assoc.targetNeuron == target) ||
            (assoc.sourceNeuron == target && assoc.targetNeuron == source)) {
            assoc.contextVector = context;
            break;
        }
    }
}

std::vector<NeuronId> AssociativeMemory::getAssociatedNeuronsInContext(NeuronId neuron,
                                                      const std::vector<float>& context) const {
    std::vector<NeuronId> result;
    
    auto it = pImpl->neuronAssociationIndices.find(neuron);
    if (it != pImpl->neuronAssociationIndices.end()) {
        for (size_t index : it->second) {
            if (index < pImpl->associations.size()) {
                const auto& assoc = pImpl->associations[index];
                
                // Simple context matching
                if (contextMatch(assoc.contextVector, context)) {
                    NeuronId other = (assoc.sourceNeuron == neuron) ? 
                                   assoc.targetNeuron : assoc.sourceNeuron;
                    result.push_back(other);
                }
            }
        }
    }
    
    return result;
}

void AssociativeMemory::decayAssociations(float decayRate) {
    for (auto& assoc : pImpl->associations) {
        assoc.strength *= (1.0f - decayRate);
        
        // Age the association
        size_t age = pImpl->currentTime - assoc.lastActivation;
        if (age > 1000) { // Remove very old associations
            assoc.strength *= 0.5f;
        }
    }
    
    // Prune weak associations
    pruneWeakAssociations(0.01f);
}

void AssociativeMemory::pruneWeakAssociations(float threshold) {
    std::vector<NeuralAssociation> pruned;
    std::unordered_map<NeuronId, std::vector<size_t>> newIndices;
    
    for (size_t i = 0; i < pImpl->associations.size(); ++i) {
        if (pImpl->associations[i].strength >= threshold) {
            pruned.push_back(pImpl->associations[i]);
            
            // Update indexes
            newIndices[pImpl->associations[i].sourceNeuron].push_back(pruned.size() - 1);
            newIndices[pImpl->associations[i].targetNeuron].push_back(pruned.size() - 1);
        }
    }
    
    pImpl->associations = pruned;
    pImpl->neuronAssociationIndices = newIndices;
}

size_t AssociativeMemory::getAssociationCount() const {
    return pImpl->associations.size();
}

void AssociativeMemory::updateFromWorkingMemory(const WorkingMemory& workingMem) {
    // Convert working memory activations to associations
    for (const auto& pair : workingMem.pImpl->activations) {
        NeuronId neuron = pair.first;
        float activation = pair.second;
        
        if (activation > 0.5f) {
            // Create association with random other neuron (simplified)
            std::hash<uint64_t> hasher;
            uint64_t hashValue = hasher(static_cast<uint64_t>(activation * 1000000));
            NeuronId other = NeuronId(hashValue);
            
            if (neuron != other) {
                createAssociation(neuron, other, activation);
            }
        }
    }
}

std::vector<std::pair<NeuronId, NeuronId>> AssociativeMemory::getStrongAssociations() const {
    std::vector<std::pair<NeuronId, NeuronId>> result;
    
    for (const auto& assoc : pImpl->associations) {
        if (assoc.strength > 0.5f) {
            result.emplace_back(assoc.sourceNeuron, assoc.targetNeuron);
        }
    }
    
    return result;
}

void AssociativeMemory::updateHebbianLearning() {
    // Simple Hebbian learning based on activation patterns
    if (pImpl->hebbianMatrix.empty() && !pImpl->associations.empty()) {
        // Initialize Hebbian matrix
        size_t assocCount = pImpl->associations.size();
        pImpl->hebbianMatrix.resize(assocCount, std::vector<float>(assocCount, 0.0f));
        
        // Fill with current strengths
        for (size_t i = 0; i < assocCount; ++i) {
            for (size_t j = 0; j < assocCount; ++j) {
                if (i == j) {
                    pImpl->hebbianMatrix[i][j] = 1.0f;
                } else {
                    pImpl->hebbianMatrix[i][j] = 0.0f;
                }
            }
        }
    }
    
    // Simple Hebbian update
    for (auto& row : pImpl->hebbianMatrix) {
        for (auto& value : row) {
            value *= pImpl->globalHebbianRate;
        }
    }
}

bool AssociativeMemory::contextMatch(const std::vector<float>& context1,
                                    const std::vector<float>& context2) const {
    if (context1.size() != context2.size()) return false;
    
    float similarity = 0.0f;
    for (size_t i = 0; i < context1.size(); ++i) {
        similarity += std::abs(context1[i] - context2[i]);
    }
    
    return similarity < 0.1f; // Simple threshold match
}

// Unified Memory System Implementation
struct UnifiedMemorySystem::Impl {
    WorkingMemory workingMemory;
    EpisodicMemory episodicMemory;
    SemanticMemory semanticMemory;
    ProceduralMemory proceduralMemory;
    AssociativeMemory associativeMemory;
    
    MemoryConfig config;
    
    Impl(const MemoryConfig& cfg) 
        : workingMemory(cfg), episodicMemory(cfg), semanticMemory(cfg),
          proceduralMemory(cfg), associativeMemory(cfg), config(cfg) {}
};

UnifiedMemorySystem::UnifiedMemorySystem(const MemoryConfig& config) : pImpl(std::make_unique<Impl>(config)) {
}

UnifiedMemorySystem::~UnifiedMemorySystem() = default;

WorkingMemory* UnifiedMemorySystem::getWorkingMemory() {
    return &pImpl->workingMemory;
}

EpisodicMemory* UnifiedMemorySystem::getEpisodicMemory() {
    return &pImpl->episodicMemory;
}

SemanticMemory* UnifiedMemorySystem::getSemanticMemory() {
    return &pImpl->semanticMemory;
}

ProceduralMemory* UnifiedMemorySystem::getProceduralMemory() {
    return &pImpl->proceduralMemory;
}

AssociativeMemory* UnifiedMemorySystem::getAssociativeMemory() {
    return &pImpl->associativeMemory;
}

void UnifiedMemorySystem::store(const std::string& key, const std::vector<float>& value,
                               const std::unordered_map<std::string, std::string>& metadata) {
    // Store in working memory as primary storage
    pImpl->workingMemory.store(key, value, metadata);
    
    // Also store in episodic memory for long-term storage
    pImpl->episodicMemory.store(key, value, metadata);
}

std::vector<float> UnifiedMemorySystem::retrieve(const std::string& key) const {
    return pImpl->workingMemory.retrieve(key);
}

bool UnifiedMemorySystem::exists(const std::string& key) const {
    return pImpl->workingMemory.exists(key);
}

void UnifiedMemorySystem::clearAll() {
    pImpl->workingMemory.clear();
    pImpl->episodicMemory.clear();
    pImpl->semanticMemory.clear();
    pImpl->proceduralMemory.clear();
    pImpl->associativeMemory.clear();
}

size_t UnifiedMemorySystem::getTotalMemorySize() const {
    return pImpl->workingMemory.size() + pImpl->episodicMemory.size() +
           pImpl->semanticMemory.size() + pImpl->proceduralMemory.size() +
           pImpl->associativeMemory.size();
}

std::vector<std::string> UnifiedMemorySystem::getAllMemoryTypes() const {
    return {"WorkingMemory", "EpisodicMemory", "SemanticMemory", 
            "ProceduralMemory", "AssociativeMemory"};
}

void UnifiedMemorySystem::integrateAllSystems() {
    // Integrate all memory systems
    pImpl->episodicMemory.integrateWithWorkingMemory(pImpl->workingMemory);
    pImpl->associativeMemory.updateFromWorkingMemory(pImpl->workingMemory);
}

void UnifiedMemorySystem::updateAll(float deltaTime) {
    // Update all memory systems
    pImpl->workingMemory.update(deltaTime);
    pImpl->episodicMemory.update(deltaTime);
    pImpl->semanticMemory.update(deltaTime);
    pImpl->proceduralMemory.update(deltaTime);
    pImpl->associativeMemory.update(deltaTime);
    
    // Integration after updates
    integrateAllSystems();
}

void UnifiedMemorySystem::initiateSystemReplay() {
    pImpl->episodicMemory.updateReplayQueue();
}

void UnifiedMemorySystem::consolidateAll(float threshold) {
    pImpl->episodicMemory.consolidateEpisodes(threshold);
}

void UnifiedMemorySystem::updateConfig(const MemoryConfig& newConfig) {
    pImpl->config = newConfig;
}

MemoryConfig UnifiedMemorySystem::getCurrentConfig() const {
    return pImpl->config;
}

std::string UnifiedMemorySystem::getMemoryStatusReport() const {
    std::stringstream ss;
    ss << "=== Unified Memory System Status ===\n";
    ss << "Working Memory: " << pImpl->workingMemory.size() << "/" 
       << pImpl->workingMemory.getCapacity() << " entries\n";
    ss << "Episodic Memory: " << pImpl->episodicMemory.getEpisodeCount() << " episodes\n";
    ss << "Semantic Memory: " << pImpl->semanticMemory.size() << " concepts\n";
    ss << "Procedural Memory: " << pImpl->proceduralMemory.size() << " skills\n";
    ss << "Associative Memory: " << pImpl->associativeMemory.getAssociationCount() << " associations\n";
    ss << "Total Memory Size: " << getTotalMemorySize() << " elements\n";
    ss << "Integration Score: " << getSystemIntegrationScore() << "/1.0\n";
    
    return ss.str();
}

float UnifiedMemorySystem::getSystemIntegrationScore() const {
    float totalScore = 0.0f;
    float maxScore = 5.0f; // 5 different memory systems
    
    // Check if each system has content
    if (!pImpl->workingMemory.size()) totalScore += 0.2f;
    if (!pImpl->episodicMemory.getEpisodeCount()) totalScore += 0.2f;
    if (!pImpl->semanticMemory.size()) totalScore += 0.2f;
    if (!pImpl->proceduralMemory.size()) totalScore += 0.2f;
    if (!pImpl->associativeMemory.getAssociationCount()) totalScore += 0.2f;
    
    return totalScore / maxScore;
}

void UnifiedMemorySystem::applyDevelopmentChanges(float developmentalStage) {
    // Adjust memory systems based on developmental stage
    if (developmentalStage > 0.5f) {
        // Mature systems - increase capacity
        pImpl->config.workingMemoryCapacity *= 1.5f;
        pImpl->config.episodicMaxEpisodes *= 2;
    } else {
        // Developing systems - decrease capacity
        pImpl->config.workingMemoryCapacity *= 0.8f;
        pImpl->config.episodicMaxEpisodes *= 0.8f;
    }
}

void UnifiedMemorySystem::adjustPlasticityRates(float plasticityFactor) {
    // Adjust plasticity rates based on plasticity factor
    pImpl->config.associativeHebbianEnabled = (plasticityFactor > 0.5f);
}

} // namespace nlm
