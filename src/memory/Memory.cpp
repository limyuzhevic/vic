#include "Memory.hpp"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <map>
#include <limits>

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
    // Implement real storage with capacity limits
    bool found = false;
    
    // Check if neuron exists and update value
    for (auto& item : pImpl->items) {
        if (item.first == neuron) {
            item.second = value;
            found = true;
            break;
        }
    }
    
    // If neuron doesn't exist, add it with capacity management
    if (!found) {
        // Use LRU (Least Recently Used) eviction policy
        if (pImpl->items.size() >= pImpl->capacity) {
            // Remove the oldest item (first in vector)
            pImpl->items.erase(pImpl->items.begin());
        }
        
        // Add new item at the end (most recent)
        pImpl->items.emplace_back(neuron, value);
        
        // Sort by neuron ID for deterministic behavior
        std::sort(pImpl->items.begin(), pImpl->items.end(),
                  [](const auto& a, const auto& b) { return a.first < b.first; });
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
    // Implement real consolidation mechanisms
    // Consolidate episodes by relevance, recency, and reward value
    
    if (pImpl->episodes.empty()) {
        return;
    }
    
    // Enhanced consolidation algorithm
    // Step 1: Calculate importance scores based on multiple factors
    std::vector<std::pair<float, size_t>> importance;
    importance.reserve(pImpl->episodes.size());
    
    for (size_t i = 0; i < pImpl->episodes.size(); ++i) {
        const auto& episode = pImpl->episodes[i];
        
        // Recency factor: more recent = higher importance
        float recencyFactor = 1.0f;
        if (i < pImpl->episodes.size() / 2) {
            recencyFactor += 2.0f;  // Recent episodes are much more important
        } else if (i < pImpl->episodes.size() / 3) {
            recencyFactor += 1.0f;
        }
        
        // Relevance factor: based on metadata and activity
        float relevanceFactor = 1.0f;
        if (!episode.metadata.empty()) {
            // Simple metadata relevance scoring
            size_t metadataRelevance = 0;
            for (char c : episode.metadata) {
                if (std::isalnum(c)) metadataRelevance++;
            }
            relevanceFactor += std::min(2.0f, static_cast<float>(metadataRelevance) * 0.1f);
        }
        
        // Reward factor: higher reward = higher importance
        float rewardFactor = 1.0f + episode.reward;
        
        // Neuronal activity factor: more active neurons = higher importance
        float activityFactor = 1.0f;
        if (!episode.neurons.empty() && !episode.values.empty()) {
            float totalActivity = 0.0f;
            for (float val : episode.values) {
                totalActivity += std::abs(val);
            }
            activityFactor += std::min(2.0f, totalActivity * 0.1f);
        }
        
        // Combined importance score
        float importanceScore = recencyFactor * 0.3f + 
                               relevanceFactor * 0.25f + 
                               rewardFactor * 0.25f + 
                               activityFactor * 0.2f;
        
        importance.emplace_back(importanceScore, i);
    }
    
    // Step 2: Sort by importance (descending) with higher importance first
    std::sort(importance.begin(), importance.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Step 3: Apply consolidation threshold - only keep episodes above threshold
    std::vector<EpisodicMemoryItem> consolidated;
    consolidated.reserve(std::min(pImpl->episodes.size(), pImpl->maxEpisodes));
    
    for (const auto& imp : importance) {
        if (imp.first >= relevanceThreshold) {
            consolidated.push_back(pImpl->episodes[imp.second]);
        }
    }
    
    // Step 4: If we have more episodes than capacity, keep the most important ones
    if (consolidated.size() > pImpl->maxEpisodes) {
        consolidated.resize(pImpl->maxEpisodes);
    }
    
    // Step 5: Re-sort to maintain time order (most recent first)
    // This helps with temporal coherence
    std::sort(consolidated.begin(), consolidated.end(),
              [](const EpisodicMemoryItem& a, const EpisodicMemoryItem& b) {
                  return a.timestamp > b.timestamp;
              });
    
    pImpl->episodes = std::move(consolidated);
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

} // namespace nlm
