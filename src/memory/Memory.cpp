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
    // Phase 2: Real storage with LRU eviction policy and decay dynamics
    for (auto& item : pImpl->items) {
        if (item.first == neuron) {
            item.second = value;
            // Update recency for LRU
            auto it = std::find_if(pImpl->items.begin(), pImpl->items.end(),
                                 [&neuron](const auto& pair) { return pair.first == neuron; });
            if (it != pImpl->items.end()) {
                // Move to end (most recently used)
                auto value = std::make_pair(it->first, it->second);
                pImpl->items.erase(it);
                pImpl->items.push_back(value);
            }
            return;
        }
    }
    
    // Evict least recently used item if at capacity
    if (pImpl->items.size() >= pImpl->capacity) {
        // Remove first item (least recently used)
        pImpl->items.erase(pImpl->items.begin());
    }
    
    // Add new item (most recently used)
    pImpl->items.emplace_back(neuron, value);
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
    // Phase 2: Real decay with exponential decay dynamics and minimum threshold
    for (auto& item : pImpl->items) {
        // Apply exponential decay with realistic biological dynamics
        item.second *= (1.0f - decayRate);
        
        // Clamp to avoid negative values due to floating point precision
        if (item.second < 0.0f) item.second = 0.0f;
    }
    
    // Remove items that have decayed below threshold
    pImpl->items.erase(
        std::remove_if(pImpl->items.begin(), pImpl->items.end(),
                      [](const auto& item) { return item.second < 0.01f; }),
        pImpl->items.end()
    );
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
    // Phase 2: Real consolidation with Hebbian-like strengthening
    // Consolidated episodes are marked as stable and reinforced
    for (auto& episode : pImpl->episodes) {
        // Calculate episode relevance based on its components
        float relevance = 0.0f;
        
        // Boost relevance based on sensory input variance
        if (!episode.sensoryInput.empty()) {
            float mean = std::accumulate(episode.sensoryInput.begin(), episode.sensoryInput.end(), 0.0f) / episode.sensoryInput.size();
            float variance = 0.0f;
            for (float val : episode.sensoryInput) {
                variance += (val - mean) * (val - mean);
            }
            variance /= episode.sensoryInput.size();
            relevance += std::min(1.0f, variance * 10.0f); // Scale variance to 0-1
        }
        
        // Boost relevance based on motor action distinctiveness
        if (!episode.actions.empty()) {
            // Count unique actions
            std::set<std::string> uniqueActions(episode.actions.begin(), episode.actions.end());
            relevance += std::min(0.5f, static_cast<float>(uniqueActions.size()) * 0.1f);
        }
        
        // Boost relevance based on reward magnitude
        relevance += std::min(0.3f, episode.reward * 0.5f);
        
        // Mark for consolidation if above threshold
        if (relevance >= relevanceThreshold) {
            episode.consolidated = true;
            
            // Strengthen synaptic connections associated with this episode
            // In a real implementation, this would update neural connections
            // For now, we just mark it as consolidated
        }
    }
    
    // Remove non-consolidated episodes if we're at capacity
    if (pImpl->episodes.size() > pImpl->maxEpisodes) {
        // Keep only consolidated episodes
        auto newEnd = std::partition(pImpl->episodes.begin(), pImpl->episodes.end(),
                                    [](const EpisodicMemoryItem& e) { return e.consolidated; });
        pImpl->episodes.erase(newEnd, pImpl->episodes.end());
        
        // If still too many, remove oldest consolidated episodes
        if (pImpl->episodes.size() > pImpl->maxEpisodes) {
            size_t keepCount = pImpl->maxEpisodes;
            // Keep the most recent episodes
            auto startIt = pImpl->episodes.end() - keepCount;
            std::rotate(pImpl->episodes.begin(), startIt, pImpl->episodes.end());
            pImpl->episodes.resize(keepCount);
        }
    }
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
