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
    // Validate neuron ID
    if (neuron == INVALID_NEURON_ID) {
        NLM_LOG_WARNING("Cannot store working memory for invalid neuron ID");
        return;
    }
    
    // Check for value validity (optional: check for NaN or inf)
    if (std::isnan(value) || std::isinf(value)) {
        NLM_LOG_WARNING("Invalid working memory value: " + std::to_string(value));
        return;
    }
    
    // Update existing item or add new one
    for (auto& item : pImpl->items) {
        if (item.first == neuron) {
            item.second = value;
            NLM_LOG_DEBUG("Updated working memory for neuron " + std::to_string(neuron));
            return;
        }
    }
    
    // Add new item if capacity allows
    if (pImpl->items.size() < pImpl->capacity) {
        pImpl->items.emplace_back(neuron, value);
        NLM_LOG_DEBUG("Added working memory item for neuron " + std::to_string(neuron));
    } else {
        // Remove oldest item and add new one (FIFO replacement)
        pImpl->items.erase(pImpl->items.begin());
        pImpl->items.emplace_back(neuron, value);
        NLM_LOG_INFO("Working memory full, replaced oldest item for neuron " + std::to_string(neuron));
    }
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    // Validate neuron ID
    if (neuron == INVALID_NEURON_ID) {
        NLM_LOG_WARNING("Cannot retrieve working memory for invalid neuron ID");
        return 0.0f;
    }
    
    for (const auto& item : pImpl->items) {
        if (item.first == neuron) {
            return item.second;
        }
    }
    
    return 0.0f;
}

bool WorkingMemory::contains(NeuronId neuron) const {
    // Validate neuron ID
    if (neuron == INVALID_NEURON_ID) {
        return false;
    }
    
    for (const auto& item : pImpl->items) {
        if (item.first == neuron) {
            return true;
        }
    }
    
    return false;
}

void WorkingMemory::clear() {
    if (!pImpl->items.empty()) {
        pImpl->items.clear();
        NLM_LOG_DEBUG("Cleared working memory");
    }
}

size_t WorkingMemory::getCapacity() const {
    return pImpl->capacity;
}

size_t WorkingMemory::getCurrentSize() const {
    return pImpl->items.size();
}

void WorkingMemory::decay(float decayRate) {
    // Validate decay rate
    if (decayRate < 0.0f || decayRate > 1.0f) {
        NLM_LOG_WARNING("Invalid decay rate: " + std::to_string(decayRate));
        return;
    }
    
    if (!pImpl->items.empty() && decayRate > 0.0f) {
        for (auto& item : pImpl->items) {
            item.second *= (1.0f - decayRate);
        }
        NLM_LOG_DEBUG("Decayed working memory items by " + std::to_string(decayRate));
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
    // Validate relevance threshold
    if (relevanceThreshold < 0.0f || relevanceThreshold > 1.0f) {
        NLM_LOG_WARNING("Invalid relevance threshold for consolidation: " + std::to_string(relevanceThreshold));
        return;
    }
    
    if (pImpl->episodes.empty()) {
        return;
    }
    
    // Step 1: Calculate relevance scores for each episode
    std::vector<std::pair<size_t, float>> relevanceScores;
    relevanceScores.reserve(pImpl->episodes.size());
    
    for (size_t i = 0; i < pImpl->episodes.size(); ++i) {
        float score = calculateEpisodeRelevance(pImpl->episodes[i], i);
        relevanceScores.emplace_back(i, score);
    }
    
    // Step 2: Sort by relevance (descending) - most relevant first
    std::sort(relevanceScores.begin(), relevanceScores.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Step 3: Keep only high-relevance episodes
    std::vector<EpisodicMemoryItem> keptEpisodes;
    keptEpisodes.reserve(std::min(static_cast<size_t>(pImpl->maxEpisodes * 0.8f), pImpl->episodes.size()));
    
    for (const auto& pair : relevanceScores) {
        if (pair.second >= relevanceThreshold || 
            keptEpisodes.size() < static_cast<size_t>(pImpl->maxEpisodes * 0.8f)) {
            keptEpisodes.push_back(pImpl->episodes[pair.first]);
        }
    }
    
    // Step 4: If we still have more episodes than capacity, use FIFO eviction
    if (keptEpisodes.size() > pImpl->maxEpisodes) {
        keptEpisodes.resize(pImpl->maxEpisodes);
    }
    
    // Step 5: Replace old episodes with consolidated ones
    pImpl->episodes = std::move(keptEpisodes);
    
    NLM_LOG_INFO("Episodic memory consolidated: " + std::to_string(pImpl->episodes.size()) + 
                " episodes retained, threshold=" + std::to_string(relevanceThreshold));
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

// Helper function to calculate episode relevance score
float calculateEpisodeRelevance(const EpisodicMemoryItem& episode, size_t index) {
    float relevance = 1.0f;
    
    // Higher weight for recent episodes (recency effect)
    float recencyWeight = 1.0f - (static_cast<float>(index) / std::max(static_cast<size_t>(1), 10));
    relevance *= recencyWeight;
    
    // Size of episode (number of neurons + data)
    float episodeSize = static_cast<float>(episode.neurons.size() + episode.values.size());
    float sizeScore = std::min(episodeSize / 20.0f, 1.0f);  // Normalize to 0-1
    relevance *= (0.3f + 0.7f * sizeScore);
    
    // Metadata factor if present
    if (!episode.metadata.empty()) {
        // More complex metadata generally indicates more significant episode
        relevance *= (0.5f + 0.5f * std::min(static_cast<float>(episode.metadata.size()) / 100.0f, 1.0f));
    }
    
    // Clamp to valid range
    return std::clamp(relevance, 0.0f, 1.0f);
}

} // namespace nlm
