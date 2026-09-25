#include "Memory.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Working Memory Implementation
struct WorkingMemory::Impl {
    std::vector<std::pair<NeuronId, float>> items;
    size_t capacity;
    float decayRate;
    
    Impl(size_t cap) : capacity(cap), decayRate(0.01f) {}
};

WorkingMemory::WorkingMemory() : pImpl(new Impl(100)) {}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::store(NeuronId neuron, float value) {
    // Enhanced storage with neural-inspired mechanisms
    for (auto& item : pImpl->items) {
        if (item.first == neuron) {
            // Hebbian-like potentiation for repeated activation
            item.second = std::fmax(0.0f, item.second * 1.1f + value * 0.9f);
            return;
        }
    }
    if (pImpl->items.size() < pImpl->capacity) {
        pImpl->items.emplace_back(neuron, value);
    } else {
        // Competition-based replacement
        size_t weakestIdx = 0;
        float weakestValue = pImpl->items[0].second;
        
        for (size_t i = 1; i < pImpl->items.size(); ++i) {
            if (pImpl->items[i].second < weakestValue) {
                weakestIdx = i;
                weakestValue = pImpl->items[i].second;
            }
        }
        
        if (value > weakestValue * 1.5f) {
            pImpl->items[weakestIdx] = {neuron, value};
        }
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
    pImpl->decayRate = decayRate;
    for (auto& item : pImpl->items) {
        item.second *= (1.0f - decayRate);
        // Prevent negative values
        if (item.second < 0.0f) {
            item.second = 0.0f;
        }
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
    // Enhanced storage with temporal ordering and neural encoding
    if (pImpl->episodes.size() >= pImpl->maxEpisodes) {
        // Remove oldest episode (FIFO)
        pImpl->episodes.erase(pImpl->episodes.begin());
    }
    
    // Create neural-inspired episode encoding
    EpisodicMemoryItem stored = episode;
    
    // Encode episode as neural activity pattern
    if (!stored.neurons.empty() && !stored.values.empty()) {
        // Normalize values to [0, 1] range
        float maxValue = 0.0f;
        for (float val : stored.values) {
            if (val > maxValue) maxValue = val;
        }
        if (maxValue > 0.0f) {
            for (size_t i = 0; i < stored.values.size() && i < stored.neurons.size(); ++i) {
                stored.values[i] /= maxValue;
            }
        }
    }
    
    stored.metadata = "neural_encoded:" + std::to_string(episode.timestamp);
    
    pImpl->episodes.push_back(stored);
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
    // Enhanced consolidation with neural mechanisms
    std::vector<EpisodicMemoryItem> consolidated;
    
    for (const auto& episode : pImpl->episodes) {
        // Compute episode relevance based on neural activation
        float relevance = 0.0f;
        if (!episode.values.empty()) {
            for (float val : episode.values) {
                relevance += std::abs(val);
            }
            relevance /= episode.values.size();
        }
        
        // Keep episodes above relevance threshold
        if (relevance >= relevanceThreshold) {
            consolidated.push_back(episode);
        }
    }
    
    // Replace with consolidated episodes
    pImpl->episodes = consolidated;
}

// Semantic Memory Implementation
struct SemanticMemory::Impl {
    std::vector<std::pair<std::string, std::string>> facts;
    float learningRate;
    
    Impl() : learningRate(0.1f) {}
};

SemanticMemory::SemanticMemory() : pImpl(new Impl) {}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::storeFact(const std::string& key, const std::string& value) {
    // Neural-inspired semantic storage with pattern completion
    for (auto& fact : pImpl->facts) {
        if (fact.first == key) {
            // Hebbian-like strengthening for repeated concepts
            fact.second = fact.second * 0.8f + value * 0.9f;
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
    float learningRate;
    
    Impl() : learningRate(0.1f) {}
};

ProceduralMemory::ProceduralMemory() : pImpl(new Impl) {}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    // Neural-inspired skill learning with pattern encoding
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            // Update skill pattern with neural Hebbian learning
            skill.neuralPattern = pattern;
            skill.proficiency = std::min(1.0f, skill.proficiency + 0.1f);
            return;
        }
    }
    
    Skill newSkill;
    newSkill.name = name;
    newSkill.neuralPattern = pattern;
    newSkill.proficiency = 0.1f; // Start with low proficiency
    pImpl->skills.push_back(newSkill);
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
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations;
    float learningRate;
    
    Impl() : learningRate(0.1f) {}
};

AssociativeMemory::AssociativeMemory() : pImpl(new Impl) {}

AssociativeMemory::~AssociativeMemory() = default;

void AssociativeMemory::associate(NeuronId a, NeuronId b, float strength) {
    // Neural-inspired associative learning with Hebbian plasticity
    for (auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            // Strengthen association
            std::get<2>(assoc) = std::min(1.0f, std::get<2>(assoc) + strength * 0.1f);
            return;
        }
    }
    
    // Create new association
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
