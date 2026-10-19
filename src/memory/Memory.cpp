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

WorkingMemory::~WorkingMemory() {
    delete pImpl;
}

void WorkingMemory::store(NeuronId neuron, float value) {
    // Implement real storage with capacity limits
    
    // Input validation
    if (value < -1000.0f || value > 1000.0f) {
        return;  // Reject extreme values
    }
    
    // Find existing entry
    for (auto& item : pImpl->items) {
        if (item.first == neuron) {
            item.second = value;
            return;
        }
    }
    
    // Add new entry if capacity allows
    if (pImpl->items.size() < pImpl->capacity) {
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
    if (decayRate <= 0.0f || decayRate >= 1.0f) {
        return;  // Invalid decay rate
    }
    
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

EpisodicMemory::~EpisodicMemory() {
    delete pImpl;
}

void EpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    if (episode.neurons.empty()) {
        return;  // Don't store empty episodes
    }
    
    // Check for duplicate episode timestamp
    for (const auto& existing : pImpl->episodes) {
        if (existing.timestamp == episode.timestamp) {
            return;  // Don't store duplicate
        }
    }
    
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
    if (count == 0) return result;
    
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
    // Implement real consolidation - move relevant episodes to long-term storage
    // For now, just clean up low-relevance episodes
    if (relevanceThreshold < 0.0f || relevanceThreshold > 1.0f) {
        return;  // Invalid threshold
    }
    
    std::vector<EpisodicMemoryItem> relevantEpisodes;
    for (const auto& episode : pImpl->episodes) {
        // Simple relevance calculation based on number of neurons and activation values
        if (!episode.neurons.empty() && !episode.values.empty()) {
            float avgActivation = 0.0f;
            for (float val : episode.values) {
                avgActivation += std::abs(val);
            }
            avgActivation /= episode.values.size();
            
            if (avgActivation >= relevanceThreshold) {
                relevantEpisodes.push_back(episode);
            }
        }
    }
    
    // Keep only the most recent relevant episodes
    if (relevantEpisodes.size() > pImpl->maxEpisodes) {
        relevantEpisodes.erase(relevantEpisodes.begin(), 
                              relevantEpisodes.begin() + (relevantEpisodes.size() - pImpl->maxEpisodes));
    }
    
    pImpl->episodes = std::move(relevantEpisodes);
}

// Semantic Memory Implementation
struct SemanticMemory::Impl {
    std::vector<std::pair<std::string, std::string>> facts;
};

SemanticMemory::SemanticMemory() : pImpl(new Impl) {}

SemanticMemory::~SemanticMemory() {
    delete pImpl;
}

void SemanticMemory::storeFact(const std::string& key, const std::string& value) {
    if (key.empty() || value.empty()) {
        return;  // Reject empty keys or values
    }
    
    // Limit key length
    if (key.length() > 100) {
        return;
    }
    
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

ProceduralMemory::~ProceduralMemory() {
    delete pImpl;
}

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    if (name.empty() || pattern.empty()) {
        return;  // Reject empty skills or patterns
    }
    
    // Limit skill name length
    if (name.length() > 50) {
        return;
    }
    
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            skill.neuralPattern = pattern;
            skill.proficiency = 0.5f;  // Reset proficiency
            return;
        }
    }
    
    Skill skill;
    skill.name = name;
    skill.neuralPattern = pattern;
    skill.proficiency = 0.5f;
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

AssociativeMemory::~AssociativeMemory() {
    delete pImpl;
}

void AssociativeMemory::associate(NeuronId a, NeuronId b, float strength) {
    // Validate input
    if (a.value == 0 || b.value == 0) {
        return;  // Invalid neuron IDs
    }
    
    if (strength < 0.0f || strength > 1.0f) {
        return;  // Invalid strength
    }
    
    // Ensure symmetry - create bidirectional association
    associatePair(a, b, strength);
    associatePair(b, a, strength);
}

void AssociativeMemory::associatePair(NeuronId a, NeuronId b, float strength) {
    for (auto& assoc : pImpl->associations) {
        if (std::get<0>(assoc) == a && std::get<1>(assoc) == b) {
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
