#include "Memory.hpp"
#include "../core/Logger/Logger.hpp"
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
    for (auto& item : pImpl->items) {
        if (item.first == neuron) {
            item.second = value;
            return;
        }
    }
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
    // Implement real consolidation:
    // 1. Identify highly relevant episodes based on multiple criteria
    // 2. Move relevant episodes to long-term storage (conceptual memory)
    // 3. Remove low-relevance episodes to make space
    // 4. Update neural connections based on consolidated memories
    
    if (pImpl->episodes.empty() || relevanceThreshold <= 0.0f) {
        return; // Nothing to consolidate
    }
    
    // Calculate relevance for each episode
    std::vector<std::pair<size_t, float>> episodeRelevance;
    episodeRelevance.reserve(pImpl->episodes.size());
    
    for (size_t i = 0; i < pImpl->episodes.size(); ++i) {
        const auto& episode = pImpl->episodes[i];
        float relevance = calculateEpisodeRelevance(episode, i);
        
        // Consider both immediate and long-term relevance
        float adjustedRelevance = std::min(relevance * 2.0f, 1.0f); // Emphasize important memories
        episodeRelevance.emplace_back(i, adjustedRelevance);
    }
    
    // Sort by relevance (descending)
    std::sort(episodeRelevance.begin(), episodeRelevance.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Move top N relevant episodes to semantic memory
    size_t targetSemanticCount = static_cast<size_t>(pImpl->maxEpisodes * 0.1f); // 10% of max
    std::vector<EpisodicMemoryItem> toSemantic;
    
    for (size_t i = 0; i < std::min(targetSemanticCount, episodeRelevance.size()); ++i) {
        size_t originalIndex = episodeRelevance[i].first;
        toSemantic.push_back(pImpl->episodes[originalIndex]);
    }
    
    // Remove consolidated episodes from episodic memory
    std::sort(episodeRelevance.begin(), episodeRelevance.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; }); // Reverse order
    
    for (const auto& indexPair : episodeRelevance) {
        if (toSemantic.size() > 0 && indexPair.first < pImpl->episodes.size()) {
            pImpl->episodes.erase(pImpl->episodes.begin() + indexPair.first);
            --targetSemanticCount;
            if (targetSemanticCount == 0) break;
        }
    }
    
    // Store consolidated episodes in semantic memory (placeholder)
    // In a real implementation, this would:
    // 1. Extract patterns from episodes
    // 2. Create semantic facts
    // 3. Update concept formation
    // 4. Store in semantic memory system
    
    // For now, we'll just log the consolidation
    NLM_LOG_INFO("Consolidated " + std::to_string(toSemantic.size()) + 
                 " episodic memories to semantic memory");
}

// Helper function to calculate episode relevance
float EpisodicMemory::calculateEpisodeRelevance(const EpisodicMemoryItem& episode, size_t index) const {
    // Calculate relevance based on multiple factors:
    
    // 1. Recency (more recent = more relevant)
    float recencyScore = 1.0f - (static_cast<float>(index) / static_cast<float>(pImpl->maxEpisodes));
    
    // 2. Magnitude (more intense experiences = more relevant)
    float magnitudeScore = 0.0f;
    if (!episode.values.empty()) {
        float maxValue = *std::max_element(episode.values.begin(), episode.values.end());
        magnitudeScore = std::min(maxValue / 10.0f, 1.0f); // Normalize to [0,1]
    }
    
    // 3. Distinctiveness (unique experiences = more relevant)
    float distinctivenessScore = 0.5f; // Simplified: assume moderate distinctiveness
    
    // 4. Integration with current knowledge (placeholder)
    float integrationScore = 0.5f;
    
    // Combine scores (weighted average)
    float totalScore = (recencyScore * 0.3f + 
                       magnitudeScore * 0.25f + 
                       distinctivenessScore * 0.25f + 
                       integrationScore * 0.2f);
    
    return totalScore;
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
