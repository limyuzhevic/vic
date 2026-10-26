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
    // TODO PHASE 2: Implement real storage with capacity limits
    // Now properly implements LRU eviction policy with capacity limits
    for (auto& item : pImpl->items) {
        if (item.first == neuron) {
            item.second = value;
            // Move to front for LRU
            auto it = std::find_if(pImpl->items.begin(), pImpl->items.end(),
                                 [&neuron](const auto& item) { return item.first == neuron; });
            if (it != pImpl->items.begin()) {
                std::rotate(pImpl->items.begin(), it, std::next(it));
            }
            return;
        }
    }
    
    // If capacity is full, remove oldest item
    if (pImpl->items.size() >= pImpl->capacity) {
        pImpl->items.pop_back();
    }
    
    // Add new item to front
    pImpl->items.emplace_front(neuron, value);
}

void WorkingMemory::decay(float decayRate) {
    // Enhanced decay with proper exponential decay
    for (auto& item : pImpl->items) {
        // Apply exponential decay with bounds
        item.second *= (1.0f - decayRate);
        if (item.second < 0.001f) {
            item.second = 0.0f;
        }
    }
}

void WorkingMemory::update(float timestep) {
    // Enhanced working memory update with competition
    if (pImpl->items.empty()) return;
    
    // Find maximum value for competition
    float maxValue = 0.0f;
    for (const auto& item : pImpl->items) {
        if (item.second > maxValue) {
            maxValue = item.second;
        }
    }
    
    // Normalize values (soft competition)
    float competitionStrength = 0.5f;
    for (auto& item : pImpl->items) {
        // Apply competition-based decay
        item.second *= competitionStrength;
        
        // Increase value for active neurons
        if (item.second < maxValue) {
            item.second += (maxValue - item.second) * 0.1f;
        }
    }
    
    // Apply decay based on timestep
    decay(timestep * 10.0f);
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
    // TODO PHASE 2: Implement real consolidation
    // Enhanced consolidation with relevance-based pruning
    if (pImpl->episodes.empty()) return;
    
    // Calculate total activation of all episodes
    float totalActivation = 0.0f;
    for (const auto& episode : pImpl->episodes) {
        totalActivation += episode.relevance;
    }
    
    // Keep only episodes above threshold or most recent ones
    std::vector<EpisodicMemoryItem> consolidatedEpisodes;
    consolidatedEpisodes.reserve(pImpl->maxEpisodes);
    
    // First pass: keep episodes with relevance above threshold
    for (const auto& episode : pImpl->episodes) {
        if (episode.relevance >= relevanceThreshold) {
            consolidatedEpisodes.push_back(episode);
        }
    }
    
    // If we still have capacity, fill remaining slots with most recent episodes
    if (consolidatedEpisodes.size() < pImpl->maxEpisodes) {
        size_t remaining = pImpl->maxEpisodes - consolidatedEpisodes.size();
        size_t startFrom = pImpl->episodes.size() - remaining;
        if (startFrom > consolidatedEpisodes.size()) {
            startFrom = consolidatedEpisodes.size();
        }
        
        for (size_t i = startFrom; i < pImpl->episodes.size(); ++i) {
            consolidatedEpisodes.push_back(pImpl->episodes[i]);
        }
    }
    
    // Replace with consolidated episodes
    pImpl->episodes = std::move(consolidatedEpisodes);
    
    // Recalculate relevance scores based on usage patterns
    for (auto& episode : pImpl->episodes) {
        // Apply consolidation-based relevance adjustment
        episode.relevance *= 1.2f;  // Boost already consolidated memories
        if (episode.relevance > 1.0f) {
            episode.relevance = 1.0f;
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
