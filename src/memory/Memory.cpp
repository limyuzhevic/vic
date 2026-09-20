#include "Memory.hpp"
#include <algorithm>

namespace nlm {

// Working Memory Implementation - Enhanced with neural dynamics
struct WorkingMemory::Impl {
    // Store neural activations by neuron ID
    std::unordered_map<NeuronId, std::vector<std::pair<float, SimulationStep>>> itemHistory;
    std::unordered_map<NeuronId, float> currentValues;
    size_t capacity;
    float decayRate;
    SimulationStep timestamp;
    
    Impl(size_t cap) : capacity(cap), decayRate(0.01f), timestamp(0) {}
};

WorkingMemory::WorkingMemory() : pImpl(new Impl(100)) {
    NLM_LOG_INFO("WorkingMemory initialized with capacity: " + std::to_string(pImpl->capacity));
}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::store(NeuronId neuron, float value) {
    // Check capacity and remove oldest items if needed
    if (pImpl->currentValues.size() >= pImpl->capacity && 
        pImpl->currentValues.find(neuron) == pImpl->currentValues.end()) {
        // Remove oldest item (simple FIFO for now)
        auto oldestIt = pImpl->currentValues.begin();
        pImpl->currentValues.erase(oldestIt);
        // Also remove from history
        auto historyIt = pImpl->itemHistory.find(oldestIt->first);
        if (historyIt != pImpl->itemHistory.end()) {
            historyIt->second.clear();
        }
    }
    
    // Store current value and timestamp
    pImpl->currentValues[neuron] = value;
    pImpl->itemHistory[neuron].emplace_back(value, ++pImpl->timestamp);
    
    NLM_LOG_DEBUG("WorkingMemory: Stored neuron " + std::to_string(neuron.index()) + 
                  " value: " + std::to_string(value));
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    auto it = pImpl->currentValues.find(neuron);
    if (it != pImpl->currentValues.end()) {
        return it->second;
    }
    return 0.0f;
}

bool WorkingMemory::contains(NeuronId neuron) const {
    return pImpl->currentValues.find(neuron) != pImpl->currentValues.end();
}

void WorkingMemory::clear() {
    pImpl->currentValues.clear();
    pImpl->itemHistory.clear();
    NLM_LOG_INFO("WorkingMemory cleared");
}

size_t WorkingMemory::getCapacity() const {
    return pImpl->capacity;
}

size_t WorkingMemory::getCurrentSize() const {
    return pImpl->currentValues.size();
}

void WorkingMemory::decay(float decayRate) {
    for (auto& item : pImpl->currentValues) {
        item.second *= (1.0f - decayRate);
    }
}

// Episodic Memory Implementation - Enhanced with replay
struct EpisodicMemory::Impl {
    std::vector<EpisodicMemoryItem> episodes;
    size_t maxEpisodes;
    std::unordered_map<size_t, std::vector<size_t>> replayIndices;
    float consolidationThreshold;
    
    Impl(size_t max) : maxEpisodes(max), consolidationThreshold(0.5f) {}
};

EpisodicMemory::EpisodicMemory() : pImpl(new Impl(1000)) {
    NLM_LOG_INFO("EpisodicMemory initialized with capacity: " + std::to_string(pImpl->maxEpisodes));
}

EpisodicMemory::~EpisodicMemory() = default;

void EpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    if (pImpl->episodes.size() >= pImpl->maxEpisodes) {
        // Remove oldest episode for replay
        auto oldest = pImpl->episodes.begin();
        pImpl->episodes.erase(oldest);
    }
    pImpl->episodes.push_back(episode);
    
    // Mark for potential replay
    if (pImpl->episodes.size() % 10 == 0) {
        pImpl->replayIndices[pImpl->episodes.size() - 1] = std::vector<size_t>();
    }
    
    NLM_LOG_INFO("EpisodicMemory: Stored episode " + std::to_string(episode.timestamp));
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
    if (count > pImpl->episodes.size()) {
        result = pImpl->episodes;
    } else {
        size_t start = pImpl->episodes.size() - count;
        result.assign(pImpl->episodes.begin() + start, pImpl->episodes.end());
    }
    return result;
}

void EpisodicMemory::clear() {
    pImpl->episodes.clear();
    pImpl->replayIndices.clear();
    NLM_LOG_INFO("EpisodicMemory cleared");
}

void EpisodicMemory::consolidate(float relevanceThreshold) {
    // Simple consolidation: keep episodes above relevance threshold
    std::vector<EpisodicMemoryItem> toKeep;
    
    for (const auto& episode : pImpl->episodes) {
        // Calculate relevance based on neuron activations and rewards
        float relevance = 0.0f;
        for (float value : episode.values) {
            relevance += std::abs(value);
        }
        relevance /= std::max<size_t>(episode.values.size(), 1);
        
        if (relevance >= relevanceThreshold) {
            toKeep.push_back(episode);
        }
    }
    
    // Replace with consolidated memory
    pImpl->episodes = toKeep;
    NLM_LOG_INFO("EpisodicMemory consolidated: " + std::to_string(pImpl->episodes.size()) + 
                 " episodes retained");
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
