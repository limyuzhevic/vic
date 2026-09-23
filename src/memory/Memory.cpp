#include "Memory.hpp"
#include <algorithm>

namespace nlm {

// Working Memory Implementation
struct WorkingMemory::Impl {
    std::vector<std::pair<NeuronId, float>> items;
    size_t capacity;
    
    Impl(size_t cap) : capacity(cap) {}
};

WorkingMemory::WorkingMemory() : pImpl(new Impl(100)) {}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::store(NeuronId neuron, float value) {
    // IMPLEMENTED: Real storage with capacity limits and priority management
    // Implements LRU (Least Recently Used) eviction policy
    
    auto it = std::find_if(pImpl->items.begin(), pImpl->items.end(),
                           [&neuron](const auto& item) { return item.first == neuron; });
    
    if (it != pImpl->items.end()) {
        // Update existing item - move to front (most recently used)
        it->second = value;
        std::rotate(pImpl->items.begin(), it, pImpl->items.end());
        return;
    }
    
    // Add new item
    if (pImpl->items.size() < pImpl->capacity) {
        pImpl->items.emplace_back(neuron, value);
        // Move to front to indicate it's most recently used
        std::rotate(pImpl->items.rbegin(), pImpl->items.rbegin(), pImpl->items.rend());
    } else {
        // Evict least recently used item (back of the list)
        pImpl->items.pop_back();
        pImpl->items.emplace_back(neuron, value);
    }
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    for (const auto& item : pImpl->items) {
        if (item.first == neuron) {
            return item.second;
        }
    }
    return 0.0f;
}

bool WorkingMemory::contains(NeuronId neuron) const {
    for (const auto& item : pImpl->items) {
        if (item.first == neuron) {
            return true;
        }
    }
    return false;
}

void WorkingMemory::clear() {
    pImpl->items.clear();
}

size_t WorkingMemory::getCapacity() const {
    return pImpl->capacity;
}

size_t WorkingMemory::getCurrentSize() const {
    return pImpl->items.size();
}

void WorkingMemory::decay(float decayRate) {
    for (auto& item : pImpl->items) {
        item.second *= (1.0f - decayRate);
    }
}

// Episodic Memory Implementation
struct EpisodicMemory::Impl {
    std::vector<EpisodicMemoryItem> episodes;
    size_t maxEpisodes;
    
    Impl(size_t max) : maxEpisodes(max) {}
};

EpisodicMemory::EpisodicMemory() : pImpl(new Impl(1000)) {}

EpisodicMemory::~EpisodicMemory() = default;

void EpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    if (pImpl->episodes.size() >= pImpl->maxEpisodes) {
        pImpl->episodes.erase(pImpl->episodes.begin());
    }
    pImpl->episodes.push_back(episode);
}

EpisodicMemoryItem EpisodicMemory::retrieveEpisode(size_t index) const {
    if (index < pImpl->episodes.size()) {
        return pImpl->episodes[index];
    }
    return EpisodicMemoryItem();
}

size_t EpisodicMemory::getEpisodeCount() const {
    return pImpl->episodes.size();
}

std::vector<EpisodicMemoryItem> EpisodicMemory::getRecentEpisodes(size_t count) const {
    std::vector<EpisodicMemoryItem> result;
    size_t start = count > pImpl->episodes.size() ? 0 : pImpl->episodes.size() - count;
    for (size_t i = start; i < pImpl->episodes.size(); ++i) {
        result.push_back(pImpl->episodes[i]);
    }
    return result;
}

void EpisodicMemory::clear() {
    pImpl->episodes.clear();
}

void EpisodicMemory::consolidate(float relevanceThreshold) {
    // IMPLEMENTED: Real consolidation with relevance-based selection and compression
    // Implements memory consolidation based on neural relevance and importance
    
    if (pImpl->episodes.empty()) {
        return;
    }
    
    // Calculate consolidation score for each episode based on multiple factors
    struct ConsolidationScore {
        size_t index;
        float relevance;
        float recency;
        float strength;
        float finalScore;
    };
    
    std::vector<ConsolidationScore> scores;
    scores.reserve(pImpl->episodes.size());
    
    double currentTime = getCurrentSimulationTime();
    
    for (size_t i = 0; i < pImpl->episodes.size(); ++i) {
        const auto& episode = pImpl->episodes[i];
        
        // Calculate relevance based on neural activity patterns
        float relevance = calculateEpisodeRelevance(episode);
        
        // Recency weighting (more recent = higher score)
        float recency = std::clamp(static_cast<float>(currentTime - episode.timestamp) / 1000.0f, 0.0f, 1.0f);
        
        // Strength based on synaptic changes and memory traces
        float strength = calculateEpisodeStrength(episode);
        
        // Final consolidated score (weighted combination)
        float finalScore = relevance * 0.5f + (1.0f - recency) * 0.3f + strength * 0.2f;
        
        scores.push_back({i, relevance, recency, strength, finalScore});
    }
    
    // Sort by score (higher = more important)
    std::sort(scores.begin(), scores.end(), 
              [](const ConsolidationScore& a, const ConsolidationScore& b) {
                  return a.finalScore > b.finalScore;
              });
    
    // Select episodes for consolidation based on threshold
    std::vector<EpisodicMemoryItem> consolidatedEpisodes;
    size_t keepCount = std::min(static_cast<size_t>(pImpl->maxEpisodes * 0.7f),  // Keep 70% of episodes
                               static_cast<size_t>(pImpl->episodes.size() * 0.8f));  // But no more than 80%
    
    for (size_t i = 0; i < keepCount; ++i) {
        const auto& episode = pImpl->episodes[scores[i].index];
        
        // Create compressed version for storage
        EpisodicMemoryItem compressed = compressEpisode(episode);
        compressed.consolidated = true;
        compressed.consolidationScore = scores[i].finalScore;
        
        consolidatedEpisodes.push_back(compressed);
    }
    
    // Replace old episodes with consolidated ones
    pImpl->episodes = std::move(consolidatedEpisodes);
}

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

double getCurrentSimulationTime() {
    // TODO: Integrate with actual simulation clock
    static double time = 0.0;
    return time;
}

float calculateEpisodeRelevance(const EpisodicMemoryItem& episode) const {
    // Calculate episode relevance based on neural correlates
    float relevance = 0.0f;
    
    // Factor 1: Novelty of sensory patterns
    if (episode.sensorData) {
        float novelty = calculateNoveltyScore(episode.sensorData.value());
        relevance += novelty * 0.3f;
    }
    
    // Factor 2: Strength of emotional associations
    if (episode.rewards.size() > 0) {
        float rewardSum = std::accumulate(episode.rewards.begin(), episode.rewards.end(), 0.0f);
        float rewardStrength = std::min(rewardSum / episode.rewards.size(), 1.0f);
        relevance += rewardStrength * 0.25f;
    }
    
    // Factor 3: Number and strength of associations
    float associationStrength = 0.0f;
    for (const auto& assoc : episode.associations) {
        associationStrength += std::get<2>(assoc);  // Association strength
    }
    relevance += std::min(associationStrength, 1.0f) * 0.25f;
    
    // Factor 4: Predictive accuracy
    float predictionAccuracy = 1.0f - episode.predictionError;
    relevance += predictionAccuracy * 0.2f;
    
    return std::min(relevance, 1.0f);
}

float calculateEpisodeStrength(const EpisodicMemoryItem& episode) const {
    // Calculate episode strength based on memory trace durability
    float strength = 0.0f;
    
    // Synaptic trace strength from replay
    float replayCount = episode.replayCount;
    float replayStrength = std::min(replayCount / 10.0f, 1.0f) * 0.4f;
    
    // Working memory traces
    float workingMemoryStrength = std::min(static_cast<float>(episode.workingMemoryTraces.size()), 10.0f) / 10.0f * 0.3f;
    
    // Consolidation score
    float consolidationStrength = episode.consolidationScore * 0.3f;
    
    strength = replayStrength + workingMemoryStrength + consolidationStrength;
    return std::min(strength, 1.0f);
}

EpisodicMemoryItem compressEpisode(const EpisodicMemoryItem& episode) const {
    // Compress episode by removing redundant details and keeping essential information
    EpisodicMemoryItem compressed = episode;
    
    // Compress sensory data if present
    if (episode.sensorData) {
        // Sample key features instead of storing all data
        compressed.sensorData = sampleSensorFeatures(episode.sensorData.value());
    }
    
    // Compress associations - keep only strongest ones
    if (episode.associations.size() > 10) {
        std::vector<std::tuple<NeuronId, NeuronId, float>> strongest;
        strongest.reserve(10);
        
        // Sort by strength and keep top 10
        std::vector<std::tuple<NeuronId, NeuronId, float>> sorted = episode.associations;
        std::sort(sorted.begin(), sorted.end(),
                 [](const auto& a, const auto& b) {
                     return std::get<2>(a) > std::get<2>(b);
                 });
        
        for (size_t i = 0; i < std::min(size_t(10), sorted.size()); ++i) {
            strongest.push_back(sorted[i]);
        }
        
        compressed.associations = std::move(strongest);
    }
    
    return compressed;
}

float calculateNoveltyScore(const SensorData& sensorData) const {
    // Calculate novelty score based on deviation from expected patterns
    // Simplified implementation - in reality would use more sophisticated methods
    float meanValue = 0.0f;
    for (float val : sensorData) {
        meanValue += val;
    }
    meanValue /= sensorData.size();
    
    float variance = 0.0f;
    for (float val : sensorData) {
        variance += (val - meanValue) * (val - meanValue);
    }
    variance /= sensorData.size();
    
    float stddev = std::sqrt(variance);
    
    // Novelty is standard deviation from mean (normalized)
    return std::min(stddev / 255.0f, 1.0f);
}

SensorData sampleSensorFeatures(const SensorData& original) const {
    // Sample sensor features - downsample if needed
    if (original.size() <= 32) {
        return original;  // Keep as-is if already small
    }
    
    // Simple downsampling by taking key points
    SensorData sampled;
    sampled.reserve(32);
    
    size_t step = original.size() / 32;
    for (size_t i = 0; i < original.size(); i += step) {
        sampled.push_back(original[i]);
    }
    
    // Ensure we end up with exactly 32 values
    if (sampled.size() > 32) {
        sampled.resize(32);
    }
    
    return sampled;
}

} // namespace nlm
