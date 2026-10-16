#include "Memory.hpp"
#include <algorithm>

namespace nlm {

#include "Memory.hpp"
#include "../core/Logger/Logger.hpp"
#include "../prediction/NeuralPrediction.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../motor/Action.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <stdexcept>
#include <thread>
#include <functional>
#include <queue>
#include <unordered_map>
#include <cstring>

namespace nlm {

// Working Memory Implementation with LRU and statistics
struct WorkingMemory::Impl {
    // LRU structure: list maintains order (front = most recent, back = least recent)
    std::list<std::pair<NeuronId, float>> lruList;
    std::unordered_map<NeuronId, std::list<std::pair<NeuronId, float>>::iterator> lruMap;
    
    // Priority system
    std::unordered_map<NeuronId, int> priorities;
    
    // Compression support
    bool compressed;
    size_t compressedSize;
    
    // Statistics
    size_t totalAccesses;
    size_t cacheHits;
    size_t cacheMisses;
    
    // Configuration
    size_t capacity;
    bool useLRU;
    bool compressionEnabled;
    
    // Thread safety
    mutable std::mutex mutex;
    
    Impl(size_t cap) : 
        compressed(false), compressedSize(0),
        totalAccesses(0), cacheHits(0), cacheMisses(0),
        capacity(cap), useLRU(true), compressionEnabled(false) {}
    
    ~Impl() {
        clear();
    }
};

WorkingMemory::WorkingMemory(size_t capacity) : pImpl(new Impl(capacity)) {
    if (!ErrorHelper::validateMemorySize(capacity)) {
        NLM_THROW_RECOVERABLE_ERROR(MemoryError, false,
            "Invalid memory capacity", capacity);
    }
    
    compressionEnabled = false;
    useLRU = true;
}

WorkingMemory::WorkingMemory(const WorkingMemory& other) : 
    pImpl(new Impl(*other.pImpl)) {
    compressionEnabled = other.compressionEnabled;
    useLRU = other.useLRU;
}

WorkingMemory& WorkingMemory::operator=(const WorkingMemory& other) {
    if (this != &other) {
        delete pImpl;
        pImpl = new Impl(*other.pImpl);
        compressionEnabled = other.compressionEnabled;
        useLRU = other.useLRU;
    }
    return *this;
}

WorkingMemory::WorkingMemory(WorkingMemory&& other) noexcept : 
    pImpl(other.pImpl), stats(other.stats) {
    compressionEnabled = other.compressionEnabled;
    useLRU = other.useLRU;
    locked = other.locked;
    other.pImpl = nullptr;
}

WorkingMemory& WorkingMemory::operator=(WorkingMemory&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        stats = other.stats;
        compressionEnabled = other.compressionEnabled;
        useLRU = other.useLRU;
        locked = other.locked;
        other.pImpl = nullptr;
    }
    return *this;
}

WorkingMemory::~WorkingMemory() {
    delete pImpl;
}

void WorkingMemory::store(NeuronId neuron, float value, int priority) {
    try {
        if (std::isnan(value) || std::isinf(value)) {
            NLM_THROW_RECOVERABLE_ERROR(ValidationError, false,
                "Invalid memory value", std::to_string(value));
        }
        
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->totalAccesses++;
        
        auto it = pImpl->lruMap.find(neuron);
        if (it != pImpl->lruMap.end()) {
            // Update existing entry
            it->second->second = value;
            pImpl->priorities[neuron] = priority;
            // Move to front for LRU
            pImpl->lruList.splice(pImpl->lruList.begin(), pImpl->lruList, it->second);
            return;
        }
        
        // New entry
        if (pImpl->lruList.size() >= pImpl->capacity) {
            // Evict least recently used
            auto lruIt = pImpl->lruList.end();
            lruIt--;
            NeuronId evictedNeuron = lruIt->first;
            pImpl->lruMap.erase(evictedNeuron);
            pImpl->lruList.erase(lruIt);
            pImpl->priorities.erase(evictedNeuron);
        }
        
        pImpl->lruList.emplace_front(neuron, value);
        pImpl->lruMap[neuron] = pImpl->lruList.begin();
        pImpl->priorities[neuron] = priority;
    }
    catch (const std::bad_alloc&) {
        NLM_THROW_RECOVERABLE_ERROR(MemoryError, true,
            "Memory allocation failed while storing in working memory", "");
    }
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    try {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        pImpl->totalAccesses++;
        
        auto it = pImpl->lruMap.find(neuron);
        if (it != pImpl->lruMap.end()) {
            pImpl->cacheHits++;
            // Update LRU order
            if (pImpl->useLRU) {
                pImpl->lruList.splice(pImpl->lruList.begin(), pImpl->lruList, it->second);
            }
            return it->second->second;
        }
        
        pImpl->cacheMisses++;
        return 0.0f;
    }
    catch (const std::exception& e) {
        NLM_THROW_RECOVERABLE_ERROR(SystemError, false,
            "Error retrieving from working memory: " + std::string(e.what()), "");
        return 0.0f;
    }
}

bool WorkingMemory::contains(NeuronId neuron) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->totalAccesses++;
    
    auto it = pImpl->lruMap.find(neuron);
    if (it != pImpl->lruMap.end()) {
        pImpl->cacheHits++;
        return true;
    }
    
    pImpl->cacheMisses++;
    return false;
}

void WorkingMemory::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->lruList.clear();
    pImpl->lruMap.clear();
    pImpl->priorities.clear();
}

size_t WorkingMemory::getCapacity() const {
    return pImpl->capacity;
}

size_t WorkingMemory::getCurrentSize() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->lruList.size();
}

void WorkingMemory::decay(float decayRate) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& item : pImpl->lruList) {
        item.second *= (1.0f - decayRate);
    }
}

void WorkingMemory::access(NeuronId neuron) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->lruMap.find(neuron);
    if (it != pImpl->lruMap.end() && pImpl->useLRU) {
        pImpl->lruList.splice(pImpl->lruList.begin(), pImpl->lruList, it->second);
    }
}

void WorkingMemory::setEvictionPolicy(bool useLRU) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->useLRU = useLRU;
}

void WorkingMemory::adjustPriority(NeuronId neuron, int delta) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->priorities.find(neuron);
    if (it != pImpl->priorities.end()) {
        it->second = std::max(0, it->second + delta);
    }
}

int WorkingMemory::getPriority(NeuronId neuron) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->priorities.find(neuron);
    if (it != pImpl->priorities.end()) {
        return it->second;
    }
    return 0;
}

void WorkingMemory::enableCompression(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->compressionEnabled = enable;
}

void WorkingMemory::compress() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (!pImpl->compressionEnabled) return;
    
    // Simple compression: pack values to reduce storage
    // In real implementation, would use proper compression algorithm
    std::vector<std::pair<NeuronId, float>> compressed;
    compressed.reserve(pImpl->lruList.size());
    
    for (const auto& item : pImpl->lruList) {
        // Apply lossy compression for storage
        float compressedValue = std::round(item.second * 100.0f) / 100.0f;
        compressed.emplace_back(item.first, compressedValue);
    }
    
    // Replace with compressed data
    pImpl->lruList.clear();
    pImpl->lruMap.clear();
    pImpl->priorities.clear();
    
    for (const auto& item : compressed) {
        pImpl->lruList.emplace_back(item.first, item.second);
        pImpl->lruMap[item.first] = std::prev(pImpl->lruList.end());
    }
    
    pImpl->compressed = true;
    pImpl->compressedSize = compressed.size();
}

void WorkingMemory::decompress() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (!pImpl->compressed) return;
    
    // Restore full precision
    std::vector<std::pair<NeuronId, float>> decompressed;
    decompressed.reserve(pImpl->lruList.size());
    
    for (const auto& item : pImpl->lruList) {
        // Restore original precision (simplified)
        float decompressedValue = item.second;
        decompressed.emplace_back(item.first, decompressedValue);
    }
    
    // Replace with decompressed data
    pImpl->lruList.clear();
    pImpl->lruMap.clear();
    pImpl->priorities.clear();
    
    for (const auto& item : decompressed) {
        pImpl->lruList.emplace_back(item.first, item.second);
        pImpl->lruMap[item.first] = std::prev(pImpl->lruList.end());
    }
    
    pImpl->compressed = false;
    pImpl->compressedSize = 0;
}

void WorkingMemory::lock() const {
    pImpl->mutex.lock();
}

void WorkingMemory::unlock() const {
    pImpl->mutex.unlock();
}

bool WorkingMemory::tryLock() const {
    return pImpl->mutex.try_lock();
}

// Episodic Memory Implementation with consolidation and replay
struct EpisodicMemory::Impl {
    std::vector<EpisodicMemoryItem> episodes;
    size_t maxEpisodes;
    
    // Consolidation
    SimulationStep lastConsolidationTime;
    bool consolidationDue;
    std::unordered_map<size_t, float> consolidationScores;
    
    // Replay
    bool replayEnabled;
    SimulationStep replayCooldown;
    SimulationStep lastReplayTime;
    
    // Integration with prediction
    float predictionErrorThreshold;
    float predictionConfidenceThreshold;
    
    // Thread safety
    mutable std::mutex mutex;
    
    Impl(size_t max) : 
        maxEpisodes(max), lastConsolidationTime(0), consolidationDue(false),
        replayEnabled(true), replayCooldown(100), lastReplayTime(0),
        predictionErrorThreshold(0.5f), predictionConfidenceThreshold(0.7f) {}
    
    ~Impl() {
        clear();
    }
};

EpisodicMemory::EpisodicMemory(size_t maxEpisodes) : pImpl(new Impl(maxEpisodes)) {
    locked = false;
}

EpisodicMemory::EpisodicMemory(const EpisodicMemory& other) : 
    pImpl(new Impl(*other.pImpl)) {
    locked = other.locked;
}

EpisodicMemory& EpisodicMemory::operator=(const EpisodicMemory& other) {
    if (this != &other) {
        delete pImpl;
        pImpl = new Impl(*other.pImpl);
        locked = other.locked;
    }
    return *this;
}

EpisodicMemory::EpisodicMemory(EpisodicMemory&& other) noexcept : 
    pImpl(other.pImpl), locked(other.locked) {
    other.pImpl = nullptr;
}

EpisodicMemory& EpisodicMemory::operator=(EpisodicMemory&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        locked = other.locked;
        other.pImpl = nullptr;
    }
    return *this;
}

EpisodicMemory::~EpisodicMemory() {
    delete pImpl;
}

void EpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Compute relevance score based on episode characteristics
    EpisodicMemoryItem newEpisode = episode;
    newEpisode.age = 0;
    newEpisode.isConsolidated = false;
    newEpisode.relevanceScore = computeRelevanceScore(newEpisode);
    newEpisode.consolidationQuality = 0.0f;
    newEpisode.replayCount = 0;
    newEpisode.replayEffectiveness = 0.0f;
    
    // Enforce capacity with FIFO eviction (simplified - could use LRU)
    if (pImpl->episodes.size() >= pImpl->maxEpisodes) {
        pImpl->episodes.erase(pImpl->episodes.begin());
    }
    
    pImpl->episodes.push_back(newEpisode);
}

EpisodicMemoryItem EpisodicMemory::retrieveEpisode(size_t index) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (index < pImpl->episodes.size()) {
        EpisodicMemoryItem episode = pImpl->episodes[index];
        episode.age++;
        return episode;
    }
    
    return EpisodicMemoryItem();
}

size_t EpisodicMemory::getEpisodeCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->episodes.size();
}

std::vector<EpisodicMemoryItem> EpisodicMemory::getRecentEpisodes(size_t count) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<EpisodicMemoryItem> result;
    size_t start = count > pImpl->episodes.size() ? 0 : pImpl->episodes.size() - count;
    
    for (size_t i = start; i < pImpl->episodes.size(); ++i) {
        result.push_back(pImpl->episodes[i]);
    }
    
    return result;
}

void EpisodicMemory::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->episodes.clear();
}

void EpisodicMemory::consolidate(float relevanceThreshold) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Update consolidation scores for all episodes
    for (size_t i = 0; i < pImpl->episodes.size(); ++i) {
        pImpl->consolidationScores[i] = computeConsolidationScore(i);
    }
    
    // Consolidate episodes that meet the threshold
    std::vector<size_t> toConsolidate;
    for (size_t i = 0; i < pImpl->episodes.size(); ++i) {
        if (pImpl->episodes[i].relevanceScore >= relevanceThreshold && 
            !pImpl->episodes[i].isConsolidated) {
            toConsolidate.push_back(i);
        }
    }
    
    // Mark episodes for consolidation
    for (size_t i : toConsolidate) {
        pImpl->episodes[i].isConsolidated = true;
        pImpl->episodes[i].consolidationQuality = 1.0f;
    }
    
    // Remove low-quality episodes if needed
    for (auto it = pImpl->episodes.begin(); it != pImpl->episodes.end(); ) {
        if (it->relevanceScore < relevanceThreshold * 0.1f && 
            it->age > 1000) {  // Aged enough to consider removal
            it = pImpl->episodes.erase(it);
        } else {
            ++it;
        }
    }
}

void EpisodicMemory::replayEpisode(size_t index) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (index >= pImpl->episodes.size()) return;
    
    // Check replay cooldown
    SimulationStep currentTime = pImpl->lastReplayTime + 1;
    if (currentTime < pImpl->replayCooldown) return;
    
    EpisodicMemoryItem& episode = pImpl->episodes[index];
    
    // Replay the episode
    episode.replayCount++;
    
    // Simulate replay effect - increase consolidation quality
    episode.consolidationQuality = std::min(1.0f, episode.consolidationQuality + 0.1f);
    episode.replayEffectiveness = std::min(1.0f, episode.replayEffectiveness + 0.05f);
    
    // In real implementation, would reactivate neural patterns
    pImpl->lastReplayTime = currentTime;
}

void EpisodicMemory::replayRecentEpisodes(size_t count) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<size_t> indices;
    size_t start = count > pImpl->episodes.size() ? 0 : pImpl->episodes.size() - count;
    
    for (size_t i = start; i < pImpl->episodes.size(); ++i) {
        indices.push_back(i);
    }
    
    replaySequence(indices);
}

void EpisodicMemory::replaySequence(const std::vector<size_t>& episodeIndices) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (size_t index : episodeIndices) {
        if (index < pImpl->episodes.size()) {
            replayEpisode(index);
        }
    }
}

void EpisodicMemory::updateWithPrediction(const NeuralPrediction& prediction) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    float error = prediction.getPredictionError();
    float confidence = prediction.getPredictionConfidence();
    
    // Update prediction-related fields in recent episodes
    for (auto& episode : pImpl->episodes) {
        episode.predictionError = error;
        episode.predictionConfidence = confidence;
        
        // Adjust relevance based on prediction error
        if (error > pImpl->predictionErrorThreshold) {
            episode.relevanceScore = std::max(0.0f, episode.relevanceScore - 0.1f);
        }
        
        if (confidence > pImpl->predictionConfidenceThreshold) {
            episode.relevanceScore = std::min(1.0f, episode.relevanceScore + 0.1f);
        }
    }
}

float EpisodicMemory::computeConsolidationScore(size_t episodeIndex) const {
    if (episodeIndex >= pImpl->episodes.size()) return 0.0f;
    
    const auto& episode = pImpl->episodes[episodeIndex];
    
    // Compute score based on multiple factors
    float relevance = episode.relevanceScore;
    float ageFactor = std::min(1.0f, static_cast<float>(episode.age) / 1000.0f);
    float replayFactor = std::min(1.0f, static_cast<float>(episode.replayCount) * 0.1f);
    float consolidationFactor = episode.consolidationQuality;
    
    // Weighted score
    return relevance * 0.4f + ageFactor * 0.2f + replayFactor * 0.2f + consolidationFactor * 0.2f;
}

void EpisodicMemory::scheduleConsolidation(SimulationStep timestep) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->lastConsolidationTime = timestep;
    pImpl->consolidationDue = true;
}

bool EpisodicMemory::isConsolidationDue(size_t episodeIndex) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (episodeIndex >= pImpl->episodes.size()) return false;
    
    const auto& episode = pImpl->episodes[episodeIndex];
    return episode.age > 500 && !episode.isConsolidated;
}

float EpisodicMemory::computeRelevanceScore(const EpisodicMemoryItem& episode) const {
    // Compute relevance based on episode characteristics
    float score = 0.0f;
    
    // Higher reward increases relevance
    score += std::min(1.0f, episode.reward * 0.5f);
    
    // Novelty increases relevance
    score += episode.novelty * 0.3f;
    
    // Recency increases relevance
    score += std::min(1.0f, static_cast<float>(episode.age) * 0.001f) * 0.2f;
    
    return score;
}

void EpisodicMemory::synchronizeWithWorkingMemory(const WorkingMemory& workingMemory) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Use working memory content to update episodic memory relevance
    size_t currentSize = workingMemory.getCurrentSize();
    
    for (auto& episode : pImpl->episodes) {
        // Update relevance based on working memory activation
        if (currentSize > 0) {
            episode.relevanceScore = std::min(1.0f, episode.relevanceScore + 0.05f);
        }
    }
}

void EpisodicMemory::updateWithSensoryInput(const std::vector<float>& sensoryState) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Update relevance of recent episodes based on sensory input
    for (size_t i = 0; i < std::min(size_t(10), pImpl->episodes.size()); ++i) {
        size_t index = pImpl->episodes.size() - 1 - i;
        if (index < pImpl->episodes.size()) {
            // Simulate updating relevance based on sensory state
            pImpl->episodes[index].relevanceScore = std::min(1.0f, 
                pImpl->episodes[index].relevanceScore + 0.01f);
        }
    }
}

void EpisodicMemory::updateWithAction(const ActionType& action, float reward) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Create or update an episode for this action
    EpisodicMemoryItem newEpisode;
    newEpisode.timestamp = pImpl->lastReplayTime;
    newEpisode.action = action;
    newEpisode.reward = reward;
    newEpisode.novelty = std::min(1.0f, reward * 0.1f);
    
    // Store the episode
    storeEpisode(newEpisode);
}

void EpisodicMemory::lock() const {
    pImpl->mutex.lock();
}

void EpisodicMemory::unlock() const {
    pImpl->mutex.unlock();
}

bool EpisodicMemory::tryLock() const {
    return pImpl->mutex.try_lock();
}

// Helper functions
const std::string getCompressionErrorMessage() {
    return "Compression operation failed";
}

const std::string getMemoryErrorMessage() {
    return "Memory operation failed";
}

}} // namespace nlm

// Semantic Memory Implementation
struct SemanticMemory::Impl {
    std::vector<std::pair<std::string, std::string>> facts;
};

SemanticMemory::SemanticMemory() : pImpl(new Impl) {}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::storeFact(const std::string& key, const std::string& value) {
    for (auto& fact : pImpl->facts) {
        if (fact.first == key) {
            fact.second = value;
            return;
        }
    }
    pImpl->facts.emplace_back(key, value);
}

std::string SemanticMemory::retrieveFact(const std::string& key) const {
    for (const auto& fact : pImpl->facts) {
        if (fact.first == key) {
            return fact.second;
        }
    }
    return "";
}

bool SemanticMemory::hasFact(const std::string& key) const {
    for (const auto& fact : pImpl->facts) {
        if (fact.first == key) {
            return true;
        }
    }
    return false;
}

std::vector<std::pair<std::string, std::string>> SemanticMemory::getAllFacts() const {
    return pImpl->facts;
}

void SemanticMemory::clear() {
    pImpl->facts.clear();
}

// Procedural Memory Implementation
struct ProceduralMemory::Impl {
    std::vector<Skill> skills;
};

ProceduralMemory::ProceduralMemory() : pImpl(new Impl) {}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            skill.neuralPattern = pattern;
            return;
        }
    }
    Skill skill;
    skill.name = name;
    skill.neuralPattern = pattern;
    pImpl->skills.push_back(skill);
}

Skill* ProceduralMemory::getSkill(const std::string& name) {
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            return &skill;
        }
    }
    return nullptr;
}

std::vector<Skill*> ProceduralMemory::getAllSkills() {
    std::vector<Skill*> result;
    for (auto& skill : pImpl->skills) {
        result.push_back(&skill);
    }
    return result;
}

void ProceduralMemory::updateProficiency(const std::string& name, float delta) {
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            skill.proficiency = std::clamp(skill.proficiency + delta, 0.0f, 1.0f);
            return;
        }
    }
}

void ProceduralMemory::clear() {
    pImpl->skills.clear();
}

// Associative Memory Implementation
struct AssociativeMemory::Impl {
    // Pairs of (a, b) associations with strength
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations;
};

AssociativeMemory::AssociativeMemory() : pImpl(new Impl) {}

AssociativeMemory::~AssociativeMemory() = default;

void AssociativeMemory::associate(NeuronId a, NeuronId b, float strength) {
    for (auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            std::get<2>(assoc) = strength;
            return;
        }
    }
    pImpl->associations.emplace_back(a, b, strength);
}

std::vector<NeuronId> AssociativeMemory::getAssociations(NeuronId neuron) const {
    std::vector<NeuronId> result;
    for (const auto& assoc : pImpl->associations) {
        if (std::get<0>(assoc) == neuron) {
            result.push_back(std::get<1>(assoc));
        } else if (std::get<1>(assoc) == neuron) {
            result.push_back(std::get<0>(assoc));
        }
    }
    return result;
}

float AssociativeMemory::getAssociationStrength(NeuronId a, NeuronId b) const {
    for (const auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            return std::get<2>(assoc);
        }
    }
    return 0.0f;
}

void AssociativeMemory::updateAssociation(NeuronId a, NeuronId b, float delta) {
    for (auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + delta, 0.0f, 1.0f);
            return;
        }
    }
}

void AssociativeMemory::clear() {
    pImpl->associations.clear();
}

} // namespace nlm
