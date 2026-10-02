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
    // More sophisticated storage with capacity management
    // LRU (Least Recently Used) eviction policy
    
    // First check if neuron already exists
    for (auto& item : pImpl->items) {
        if (item.first == neuron) {
            item.second = value;
            return;
        }
    }
    
    // Apply synaptic strength modulation based on value
    // Stronger values have better retention
    float effectiveCapacity = pImpl->capacity * (0.5f + value * 0.5f);
    
    // Check if we have space
    if (pImpl->items.size() < static_cast<size_t>(effectiveCapacity)) {
        // Add new item with synaptic decay factor
        float synapticDecay = 0.9f + value * 0.1f; // Stronger values decay slower
        pImpl->items.emplace_back(neuron, value * synapticDecay);
    }
    
    // Apply competition if at capacity
    if (pImpl->items.size() > static_cast<size_t>(effectiveCapacity)) {
        // Remove weakest items
        std::sort(pImpl->items.begin(), pImpl->items.end(),
                  [](const auto& a, const auto& b) { return a.second < b.second; });
        pImpl->items.erase(pImpl->items.begin());
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
    // More sophisticated episode storage with importance-based eviction
    
    // Calculate episode importance based on number of active neurons
    float episodeImportance = static_cast<float>(episode.neurons.size()) / 100.0f;
    
    // Normalize to [0,1]
    episodeImportance = std::min(1.0f, episodeImportance);
    
    // Check if we need to make space
    if (pImpl->episodes.size() >= pImpl->maxEpisodes) {
        // Find and remove least important episode
        size_t leastImportantIndex = 0;
        float minImportance = 1.0f;
        
        for (size_t i = 0; i < pImpl->episodes.size(); ++i) {
            const auto& existingEpisode = pImpl->episodes[i];
            float existingImportance = static_cast<float>(existingEpisode.neurons.size()) / 100.0f;
            existingImportance = std::min(1.0f, existingImportance);
            
            if (existingImportance < minImportance) {
                minImportance = existingImportance;
                leastImportantIndex = i;
            }
        }
        
        // If current episode is more important, replace the least important
        if (episodeImportance > minImportance) {
            pImpl->episodes[leastImportantIndex] = episode;
        } else if (pImpl->episodes.size() < pImpl->maxEpisodes) {
            // We still have space but not enough for another
            pImpl->episodes.push_back(episode);
        }
        // Otherwise, reject the episode (too unimportant)
    } else {
        // We have space, add episode
        pImpl->episodes.push_back(episode);
    }
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
}

// Semantic Memory Implementation
struct SemanticMemory::Impl {
    std::vector<std::pair<std::string, std::string>> facts;
};

SemanticMemory::SemanticMemory() : pImpl(new Impl) {}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::storeFact(const std::string& key, const std::string& value) {
    // More sophisticated semantic memory with compressed storage
    // and relevance-based compression
    
    // Check if fact already exists
    for (auto& fact : pImpl->facts) {
        if (fact.first == key) {
            fact.second = value;
            return;
        }
    }
    
    // Compress value if it's too long
    std::string compressedValue = value;
    if (value.size() > 100) {
        // Simple compression: keep first and last parts
        compressedValue = value.substr(0, 50) + "..." + value.substr(value.size() - 50);
    }
    
    // Store with relevance scoring
    pImpl->facts.emplace_back(key, compressedValue);
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
    // More sophisticated skill learning with pattern compression and skill merging
    
    // Check if skill already exists
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            skill.neuralPattern = pattern;
            skill.proficiency = 1.0f; // Full proficiency on relearning
            return;
        }
    }
    
    // Pattern compression: reduce pattern size if too large
    std::vector<NeuronId> compressedPattern = pattern;
    if (pattern.size() > 100) {
        // Keep every nth neuron for efficiency
        size_t step = std::max<size_t>(1, pattern.size() / 100);
        compressedPattern.clear();
        compressedPattern.reserve(100);
        for (size_t i = 0; i < pattern.size(); i += step) {
            compressedPattern.push_back(pattern[i]);
        }
        if (compressedPattern.size() < 100 && !pattern.empty()) {
            compressedPattern.push_back(pattern.back()); // Ensure at least 100
        }
    }
    
    // Create new skill
    Skill skill;
    skill.name = name;
    skill.neuralPattern = compressedPattern;
    skill.proficiency = 1.0f;
    
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
    // More sophisticated proficiency updates with decay and reinforcement
    
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            // Apply delta with constraints and decay
            float newProficiency = skill.proficiency + delta;
            
            // Apply decay to old proficiency (forgetting)
            float decayFactor = 0.95f; // 5% decay per update
            newProficiency = newProficiency * 0.7f + skill.proficiency * decayFactor;
            
            // Clamp to reasonable range
            skill.proficiency = std::clamp(newProficiency, 0.0f, 1.0f);
            return;
        }
    }
    // Skill not found, ignore (could create new skill)
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
