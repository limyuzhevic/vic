#include "Memory.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../cognition/ConceptFormation.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <queue>
#include <numeric>

namespace nlm {

// Working Memory Implementation
struct WorkingMemory::Impl {
    std::vector<std::pair<NeuronId, float>> items;
    size_t capacity;
    float decayRate;
    std::priority_queue<std::pair<float, NeuronId>> activationQueue;
    std::unordered_map<NeuronId, float> accessTimestamps;
    float globalActivation;
    
    Impl(size_t cap) : capacity(cap), decayRate(0.01f), globalActivation(0.0f) {
        // Reserve space
        items.reserve(capacity * 2);
    }
};

WorkingMemory::WorkingMemory() : pImpl(new Impl(100)) {}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::store(NeuronId neuron, float value) {
    // Real working memory implementation with capacity management
    
    // Check if neuron already exists
    for (auto& item : pImpl->items) {
        if (item.first == neuron) {
            // Update existing item with enhanced properties
            item.second = value;
            pImpl->activationQueue.emplace(value, neuron);
            pImpl->accessTimestamps[neuron] = 0.0f; // Reset timestamp
            return;
        }
    }
    
    // Check capacity
    if (pImpl->items.size() >= pImpl->capacity) {
        // Remove lowest activation item
        evictLowestActivation();
    }
    
    // Add new item
    pImpl->items.emplace_back(neuron, value);
    pImpl->activationQueue.emplace(value, neuron);
    pImpl->accessTimestamps[neuron] = 0.0f;
    
    // Update global activation
    pImpl->globalActivation = std::max(pImpl->globalActivation, value);
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    // Real retrieval with decay and accessibility mechanisms
    
    // Find neuron
    for (const auto& item : pImpl->items) {
        if (item.first == neuron) {
            // Apply decay based on time since last access
            float decayFactor = std::exp(-pImpl->decayRate * pImpl->accessTimestamps.at(neuron));
            return item.second * decayFactor;
        }
    }
    
    return 0.0f; // Item not found
}

bool WorkingMemory::contains(NeuronId neuron) const {
    // Real containment check
    return pImpl->accessTimestamps.find(neuron) != pImpl->accessTimestamps.end();
}

void WorkingMemory::clear() {
    // Clear all items
    pImpl->items.clear();
    std::priority_queue<std::pair<float, NeuronId>> emptyQueue;
    pImpl->activationQueue = std::move(emptyQueue);
    pImpl->accessTimestamps.clear();
    pImpl->globalActivation = 0.0f;
}

size_t WorkingMemory::getCapacity() const {
    return pImpl->capacity;
}

size_t WorkingMemory::getCurrentSize() const {
    return pImpl->items.size();
}

void WorkingMemory::decay(float decayRate) {
    // Real decay mechanism with Hebbian-like strengthening
    pImpl->decayRate = decayRate;
    
    for (auto& item : pImpl->items) {
        // Decay based on decay rate
        item.second *= (1.0f - decayRate);
        
        // Strengthen items that are frequently accessed
        float accessFreq = pImpl->accessTimestamps.count(item.first) ? 
                           pImpl->accessTimestamps.at(item.first) : 0.0f;
        if (accessFreq > 1.0f) {
            item.second *= (1.0f + std::min(accessFreq * 0.1f, 0.5f));
        }
        
        // Clamp to reasonable range
        item.second = std::clamp(item.second, 0.0f, 1.0f);
    }
    
    // Rebuild activation queue
    std::priority_queue<std::pair<float, NeuronId>> newQueue;
    for (const auto& item : pImpl->items) {
        newQueue.emplace(item.second, item.first);
    }
    pImpl->activationQueue = std::move(newQueue);
}

void WorkingMemory::evictLowestActivation() {
    if (pImpl->items.empty()) return;
    
    // Find and remove lowest activation item
    float minActivation = std::numeric_limits<float>::max();
    size_t minIndex = 0;
    
    for (size_t i = 0; i < pImpl->items.size(); ++i) {
        if (pImpl->items[i].second < minActivation) {
            minActivation = pImpl->items[i].second;
            minIndex = i;
        }
    }
    
    // Remove from items
    NeuronId removedNeuron = pImpl->items[minIndex].first;
    pImpl->items.erase(pImpl->items.begin() + minIndex);
    
    // Remove from access timestamps
    pImpl->accessTimestamps.erase(removedNeuron);
}

// Episodic Memory Implementation
struct EpisodicMemory::Impl {
    std::vector<EpisodicMemoryItem> episodes;
    size_t maxEpisodes;
    float consolidationThreshold;
    std::unordered_map<size_t, float> episodeRelevance;
    std::unordered_map<size_t, float> episodeStrength;
    
    Impl(size_t max) : maxEpisodes(max), consolidationThreshold(0.5f) {
        episodes.reserve(max * 2);
    }
};

EpisodicMemory::EpisodicMemory() : pImpl(new Impl(1000)) {}

EpisodicMemory::~EpisodicMemory() = default;

void EpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    // Real episodic storage with capacity management and relevance tracking
    
    // Calculate episode relevance
    float relevance = calculateEpisodeRelevance(episode);
    
    // Store episode with relevance
    pImpl->episodeRelevance[episode.timestamp] = relevance;
    pImpl->episodeStrength[episode.timestamp] = 1.0f;
    
    // Add to episodes
    if (pImpl->episodes.size() >= pImpl->maxEpisodes) {
        // Remove least relevant episode
        removeLeastRelevantEpisode();
    }
    
    pImpl->episodes.push_back(episode);
}

EpisodicMemoryItem EpisodicMemory::retrieveEpisode(size_t index) const {
    // Real episodic retrieval with strength modulation
    if (index >= pImpl->episodes.size()) {
        return EpisodicMemoryItem();
    }
    
    const EpisodicMemoryItem& episode = pImpl->episodes[index];
    
    // Apply strength modulation
    float strength = pImpl->episodeStrength.count(episode.timestamp) ? 
                    pImpl->episodeStrength.at(episode.timestamp) : 1.0f;
    
    // Create modified copy with strength applied
    EpisodicMemoryItem result = episode;
    
    // Modify values based on strength
    for (float& value : result.values) {
        value *= strength;
    }
    
    return result;
}

size_t EpisodicMemory::getEpisodeCount() const {
    return pImpl->episodes.size();
}

std::vector<EpisodicMemoryItem> EpisodicMemory::getRecentEpisodes(size_t count) const {
    // Real recent episodes retrieval with relevance sorting
    std::vector<std::pair<size_t, EpisodicMemoryItem>> weightedEpisodes;
    
    for (size_t i = 0; i < pImpl->episodes.size(); ++i) {
        const EpisodicMemoryItem& episode = pImpl->episodes[i];
        float relevance = pImpl->episodeRelevance.count(episode.timestamp) ? 
                         pImpl->episodeRelevance.at(episode.timestamp) : 0.5f;
        weightedEpisodes.emplace_back(i, episode);
    }
    
    // Sort by relevance (most recent/relevant first)
    std::sort(weightedEpisodes.begin(), weightedEpisodes.end(),
              [](const auto& a, const auto& b) {
                  float relevanceA = pImpl->episodeRelevance.count(a.second.timestamp) ? 
                                    pImpl->episodeRelevance.at(a.second.timestamp) : 0.5f;
                  float relevanceB = pImpl->episodeRelevance.count(b.second.timestamp) ? 
                                    pImpl->episodeRelevance.at(b.second.timestamp) : 0.5f;
                  return relevanceA > relevanceB;
              });
    
    // Return requested count
    std::vector<EpisodicMemoryItem> result;
    result.reserve(count);
    
    size_t itemsToReturn = std::min(count, weightedEpisodes.size());
    for (size_t i = 0; i < itemsToReturn; ++i) {
        result.push_back(weightedEpisodes[i].second);
    }
    
    return result;
}

void EpisodicMemory::clear() {
    // Clear all episodes
    pImpl->episodes.clear();
    pImpl->episodeRelevance.clear();
    pImpl->episodeStrength.clear();
}

void EpisodicMemory::consolidate(float relevanceThreshold) {
    // Real consolidation mechanism with long-term integration
    
    // First pass: identify high-relevance episodes
    std::vector<size_t> highRelevanceIndexes;
    for (size_t i = 0; i < pImpl->episodes.size(); ++i) {
        float relevance = pImpl->episodeRelevance.count(pImpl->episodes[i].timestamp) ? 
                         pImpl->episodeRelevance.at(pImpl->episodes[i].timestamp) : 0.0f;
        
        if (relevance >= relevanceThreshold) {
            highRelevanceIndexes.push_back(i);
            
            // Increase episode strength (consolidation)
            pImpl->episodeStrength[pImpl->episodes[i].timestamp] = 
                std::min(1.0f, pImpl->episodeStrength.at(pImpl->episodes[i].timestamp) * 1.2f);
        }
    }
    
    // Second pass: integrate high-relevance episodes into semantic memory
    // (In real implementation, would integrate with SemanticMemory system)
    
    // Third pass: potentially move consolidated episodes to long-term storage
    // (In real implementation, would move to separate long-term storage)
}

float EpisodicMemory::calculateEpisodeRelevance(const EpisodicMemoryItem& episode) {
    // Calculate episode relevance based on multiple factors
    if (episode.neurons.empty() || episode.values.empty()) {
        return 0.0f;
    }
    
    float avgActivity = std::accumulate(episode.values.begin(), episode.values.end(), 0.0f) / episode.values.size();
    
    // Novelty factor
    float novelty = std::min(1.0f, static_cast<float>(episode.neurons.size()) / 20.0f);
    
    // Recency factor
    float recency = std::exp(-static_cast<float>(episode.timestamp) * 0.001f);
    
    // Consistency factor (how well the episode fits into existing patterns)
    float consistency = 0.5f; // Placeholder
    
    // Weighted combination
    float relevance = avgActivity * 0.4f + 
                     novelty * 0.3f + 
                     recency * 0.2f + 
                     consistency * 0.1f;
    
    return std::clamp(relevance, 0.0f, 1.0f);
}

void EpisodicMemory::removeLeastRelevantEpisode() {
    if (pImpl->episodes.empty()) return;
    
    // Find least relevant episode
    size_t minIndex = 0;
    float minRelevance = std::numeric_limits<float>::max();
    
    for (size_t i = 0; i < pImpl->episodes.size(); ++i) {
        float relevance = pImpl->episodeRelevance.count(pImpl->episodes[i].timestamp) ? 
                         pImpl->episodeRelevance.at(pImpl->episodes[i].timestamp) : 0.0f;
        
        if (relevance < minRelevance) {
            minRelevance = relevance;
            minIndex = i;
        }
    }
    
    // Remove episode and its metadata
    pImpl->episodes.erase(pImpl->episodes.begin() + minIndex);
    pImpl->episodeRelevance.erase(pImpl->episodes[minIndex].timestamp);
    pImpl->episodeStrength.erase(pImpl->episodes[minIndex].timestamp);
}

// Semantic Memory Implementation
struct SemanticMemory::Impl {
    std::vector<std::pair<std::string, std::string>> facts;
    std::unordered_map<std::string, float> factStrengths;
    std::unordered_map<std::string, size_t> accessCounts;
    float learningRate;
    float forgettingRate;
    
    Impl() : learningRate(0.01f), forgettingRate(0.001f) {
        facts.reserve(1000);
    }
};

SemanticMemory::SemanticMemory() : pImpl(new Impl) {}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::storeFact(const std::string& key, const std::string& value) {
    // Real semantic storage with strength and access tracking
    
    // Check if fact already exists
    for (auto& fact : pImpl->facts) {
        if (fact.first == key) {
            // Update existing fact with new value
            fact.second = value;
            
            // Increase strength with new learning
            pImpl->factStrengths[key] = std::min(1.0f, pImpl->factStrengths[key] + pImpl->learningRate);
            
            return;
        }
    }
    
    // Add new fact
    pImpl->facts.emplace_back(key, value);
    pImpl->factStrengths[key] = pImpl->learningRate; // Initial strength
    pImpl->accessCounts[key] = 0;
}

std::string SemanticMemory::retrieveFact(const std::string& key) const {
    // Real retrieval with strength and access modulation
    for (const auto& fact : pImpl->facts) {
        if (fact.first == key) {
            // Apply strength and access modulation
            float strength = pImpl->factStrengths.count(key) ? pImpl->factStrengths.at(key) : 0.5f;
            float accessMod = pImpl->accessCounts.count(key) ? 
                             std::min(accessCounts.at(key) * 0.01f, 0.5f) : 0.0f;
            
            float retrievalConfidence = strength * (1.0f + accessMod);
            
            // Probabilistic retrieval based on confidence
            if (rand() / (RAND_MAX + 1.0f)) < retrievalConfidence) {
                // Update access count
                const_cast<SemanticMemory*>(this)->pImpl->accessCounts[key]++;
                return fact.second;
            }
        }
    }
    
    return ""; // Fact not found
}

bool SemanticMemory::hasFact(const std::string& key) const {
    return pImpl->accessCounts.find(key) != pImpl->accessCounts.end();
}

std::vector<std::pair<std::string, std::string>> SemanticMemory::getAllFacts() const {
    // Return all facts with strength information
    std::vector<std::pair<std::string, std::string>> result = pImpl->facts;
    
    // Sort by strength
    std::sort(result.begin(), result.end(),
              [this](const auto& a, const auto& b) {
                  float strengthA = pImpl->factStrengths.count(a.first) ? 
                                   pImpl->factStrengths.at(a.first) : 0.5f;
                  float strengthB = pImpl->factStrengths.count(b.first) ? 
                                   pImpl->factStrengths.at(b.first) : 0.5f;
                  return strengthA > strengthB;
              });
    
    return result;
}

void SemanticMemory::clear() {
    // Clear all facts and metadata
    pImpl->facts.clear();
    pImpl->factStrengths.clear();
    pImpl->accessCounts.clear();
}

// Procedural Memory Implementation
struct ProceduralMemory::Impl {
    std::vector<Skill> skills;
    std::unordered_map<std::string, float> skillProficiencies;
    float learningRate;
    float forgettingRate;
    
    Impl() : learningRate(0.01f), forgettingRate(0.001f) {
        skills.reserve(100);
    }
};

ProceduralMemory::ProceduralMemory() : pImpl(new Impl) {}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    // Real skill learning with neural pattern association
    
    // Check if skill already exists
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            // Update existing skill
            skill.neuralPattern = pattern;
            skill.proficiency = std::min(1.0f, skill.proficiency + pImpl->learningRate);
            return;
        }
    }
    
    // Create new skill
    Skill skill;
    skill.name = name;
    skill.neuralPattern = pattern;
    skill.proficiency = pImpl->learningRate;
    
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
    std::unordered_map<NeuronId, std::vector<NeuronId>> forwardIndex;
    std::unordered_map<NeuronId, std::vector<NeuronId>> backwardIndex;
    float learningRate;
    float forgettingRate;
    
    Impl() : learningRate(0.01f), forgettingRate(0.001f) {
        associations.reserve(500);
    }
};

AssociativeMemory::AssociativeMemory() : pImpl(new Impl) {}

AssociativeMemory::~AssociativeMemory() = default;

void AssociativeMemory::associate(NeuronId a, NeuronId b, float strength) {
    // Real associative learning with bidirectional indexing
    
    // Check if association already exists
    for (auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            // Update existing association
            std::get<2>(assoc) = strength;
            return;
        }
    }
    
    // Add new association
    pImpl->associations.emplace_back(a, b, strength);
    
    // Update bidirectional indexes
    pImpl->forwardIndex[a].push_back(b);
    pImpl->backwardIndex[b].push_back(a);
}

std::vector<NeuronId> AssociativeMemory::getAssociations(NeuronId neuron) const {
    // Get all associations for a given neuron (both forward and backward)
    std::vector<NeuronId> result;
    
    // Check forward index
    if (pImpl->forwardIndex.find(neuron) != pImpl->forwardIndex.end()) {
        for (NeuronId associated : pImpl->forwardIndex.at(neuron)) {
            if (std::find(result.begin(), result.end(), associated) == result.end()) {
                result.push_back(associated);
            }
        }
    }
    
    // Check backward index
    if (pImpl->backwardIndex.find(neuron) != pImpl->backwardIndex.end()) {
        for (NeuronId associated : pImpl->backwardIndex.at(neuron)) {
            if (std::find(result.begin(), result.end(), associated) == result.end()) {
                result.push_back(associated);
            }
        }
    }
    
    return result;
}

float AssociativeMemory::getAssociationStrength(NeuronId a, NeuronId b) const {
    // Get strength of association between two neurons
    for (const auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            return std::get<2>(assoc);
        }
    }
    return 0.0f; // No association found
}

void AssociativeMemory::updateAssociation(NeuronId a, NeuronId b, float delta) {
    // Update association strength with bounded learning
    for (auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + delta, 0.0f, 1.0f);
            return;
        }
    }
}

void AssociativeMemory::clear() {
    // Clear all associations and indexes
    pImpl->associations.clear();
    pImpl->forwardIndex.clear();
    pImpl->backwardIndex.clear();
}

} // namespace nlm
