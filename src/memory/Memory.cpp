#include "Memory.hpp"
#include <algorithm>

namespace nlm {

// Working Memory Implementation
#include <unordered_map>
#include <algorithm>

struct WorkingMemory::Impl {
    std::unordered_map<NeuronId, float> items;  // O(1) lookup
    size_t capacity;
    
    Impl(size_t cap) : capacity(cap) {}
};

WorkingMemory::WorkingMemory() : pImpl(new Impl(100)) {}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::store(NeuronId neuron, float value) {
    // Update existing entry or add new one
    if (pImpl->items.size() >= pImpl->capacity && 
        !pImpl->items.contains(neuron)) {
        // Implement LFU (Least Frequently Used) eviction
        // Find item with lowest usage frequency
        NeuronId oldestNeuron = INVALID_NEURON_ID;
        float minUsage = std::numeric_limits<float>::max();
        
        for (const auto& [n, usage] : pImpl->items) {
            if (usage < minUsage) {
                minUsage = usage;
                oldestNeuron = n;
            }
        }
        
        if (oldestNeuron != INVALID_NEURON_ID) {
            pImpl->items.erase(oldestNeuron);
        }
    }
    
    pImpl->items[neuron] = value;
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    auto it = pImpl->items.find(neuron);
    if (it != pImpl->items.end()) {
        // Track usage for eviction policy
        return it->second;
    }
    return 0.0f;
}

bool WorkingMemory::contains(NeuronId neuron) const {
    return pImpl->items.contains(neuron);
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
        return;
    }
    
    for (auto& [neuron, value] : pImpl->items) {
        value *= (1.0f - decayRate);
    }
}

// Episodic Memory Implementation
#include <algorithm>
#include <numeric>
#include <cmath>

struct EpisodicMemory::Impl {
    std::vector<EpisodicMemoryItem> episodes;
    size_t maxEpisodes;
    
    Impl(size_t max) : maxEpisodes(max) {}
};

EpisodicMemory::EpisodicMemory() : pImpl(new Impl(1000)) {}

EpisodicMemory::~EpisodicMemory() = default;

void EpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    // Check if this episode already exists (update instead of duplicate)
    for (const auto& existing : pImpl->episodes) {
        if (existing.timestamp == episode.timestamp &&
            existing.activeNeurons == episode.activeNeurons) {
            // Update existing episode with new values
            for (size_t i = 0; i < pImpl->episodes.size() && i < episode.activeNeurons.size(); ++i) {
                pImpl->episodes[i].neuronActivations[i] = episode.neuronActivations[i];
            }
            return;
        }
    }
    
    // Add new episode with capacity management
    if (pImpl->episodes.size() >= pImpl->maxEpisodes) {
        // Remove oldest 10% of episodes to make room (better than just first one)
        size_t removeCount = std::max(size_t(1), pImpl->maxEpisodes / 10);
        pImpl->episodes.erase(pImpl->episodes.begin(),
                             pImpl->episodes.begin() + removeCount);
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
    result.reserve(std::min(count, pImpl->episodes.size()));
    
    size_t start = count > pImpl->episodes.size() ? 0 : 
                   pImpl->episodes.size() - count;
    
    for (size_t i = start; i < pImpl->episodes.size(); ++i) {
        result.push_back(pImpl->episodes[i]);
    }
    return result;
}

void EpisodicMemory::clear() {
    pImpl->episodes.clear();
}

void EpisodicMemory::consolidate(float relevanceThreshold) {
    // Implement proper consolidation: strengthen important memories, prune weak ones
    if (relevanceThreshold < 0.0f || relevanceThreshold > 1.0f) {
        return;
    }
    
    // Calculate episode relevance based on activation patterns
    std::vector<float> relevances;
    relevances.reserve(pImpl->episodes.size());
    
    for (const auto& episode : pImpl->episodes) {
        float avgActivation = 0.0f;
        if (!episode.neuronActivations.empty()) {
            avgActivation = std::accumulate(episode.neuronActivations.begin(), 
                                           episode.neuronActivations.end(), 0.0f) /
                           static_cast<float>(episode.neuronActivations.size());
        }
        relevances.push_back(std::min(1.0f, avgActivation * 2.0f)); // Scale activation to relevance
    }
    
    // Remove low-relevance episodes
    auto newEnd = std::remove_if(pImpl->episodes.begin(), pImpl->episodes.end(),
        [&relevances, threshold = relevanceThreshold](const EpisodicMemoryItem& episode, size_t idx) {
            return relevances[idx] < threshold;
        });
    
    pImpl->episodes.erase(newEnd, pImpl->episodes.end());
}

// Semantic Memory Implementation
#include <unordered_map>
#include <algorithm>
#include <chrono>
#include <thread>

struct SemanticMemory::Impl {
    // Use map for O(log n) lookups, better than unordered_map for smaller datasets
    std::map<std::string, std::string> facts;
    // Track fact importance/relevance for retrieval ranking
    std::map<std::string, float> factImportance;
    
    Impl() {}
};

SemanticMemory::SemanticMemory() : pImpl(new Impl) {}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::storeFact(const std::string& key, const std::string& value) {
    // Input validation
    if (key.empty()) {
        return;
    }
    
    // Normalize key for case-insensitive lookups
    std::string normalizedKey = key;
    std::transform(normalizedKey.begin(), normalizedKey.end(), 
                   normalizedKey.begin(), ::tolower);
    
    bool existed = pImpl->facts.find(normalizedKey) != pImpl->facts.end();
    
    // Store the fact
    pImpl->facts[normalizedKey] = value;
    
    // Update importance score if fact already existed
    if (existed) {
        // Incremental importance update based on value changes
        auto it = pImpl->factImportance.find(normalizedKey);
        if (it != pImpl->factImportance.end()) {
            it->second = std::min(1.0f, it->second * 1.1f);
        }
    } else {
        // New fact gets initial importance
        pImpl->factImportance[normalizedKey] = 0.5f;
    }
}

std::string SemanticMemory::retrieveFact(const std::string& key) const {
    if (key.empty()) {
        return "";
    }
    
    std::string normalizedKey = key;
    std::transform(normalizedKey.begin(), normalizedKey.end(), 
                   normalizedKey.begin(), ::tolower);
    
    auto it = pImpl->facts.find(normalizedKey);
    if (it != pImpl->facts.end()) {
        return it->second;
    }
    return "";
}

bool SemanticMemory::hasFact(const std::string& key) const {
    if (key.empty()) {
        return false;
    }
    
    std::string normalizedKey = key;
    std::transform(normalizedKey.begin(), normalizedKey.end(), 
                   normalizedKey.begin(), ::tolower);
    
    return pImpl->facts.find(normalizedKey) != pImpl->facts.end();
}

std::vector<std::pair<std::string, std::string>> SemanticMemory::getAllFacts() const {
    std::vector<std::pair<std::string, std::string>> result;
    result.reserve(pImpl->facts.size());
    
    for (const auto& [key, value] : pImpl->facts) {
        result.emplace_back(key, value);
    }
    
    return result;
}

void SemanticMemory::clear() {
    pImpl->facts.clear();
    pImpl->factImportance.clear();
}

// Procedural Memory Implementation
#include <algorithm>
#include <cmath>
#include <unordered_map>

struct ProceduralMemory::Impl {
    std::vector<Skill> skills;
    // Map for O(1) skill lookup by name
    std::unordered_map<std::string, Skill*> skillMap;
    
    Impl() {}
};

ProceduralMemory::ProceduralMemory() : pImpl(new Impl) {}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    // Validate inputs
    if (name.empty() || pattern.empty()) {
        return;
    }
    
    // Check if skill already exists
    auto it = pImpl->skillMap.find(name);
    if (it != pImpl->skillMap.end()) {
        // Update existing skill
        it->second->neuralPattern = pattern;
        return;
    }
    
    // Create new skill with enhanced properties
    Skill skill;
    skill.name = name;
    skill.neuralPattern = pattern;
    skill.proficiency = 0.0f;
    skill.lastUsed = 0.0f;
    skill.successCount = 0;
    skill.failureCount = 0;
    
    // Add to both collections
    pImpl->skills.push_back(skill);
    pImpl->skillMap[name] = &pImpl->skills.back();
}

Skill* ProceduralMemory::getSkill(const std::string& name) {
    auto it = pImpl->skillMap.find(name);
    if (it != pImpl->skillMap.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<Skill*> ProceduralMemory::getAllSkills() {
    std::vector<Skill*> result;
    result.reserve(pImpl->skills.size());
    
    for (auto& skill : pImpl->skills) {
        result.push_back(&skill);
    }
    
    return result;
}

void ProceduralMemory::updateProficiency(const std::string& name, float delta) {
    auto it = pImpl->skillMap.find(name);
    if (it != pImpl->skillMap.end()) {
        Skill* skill = it->second;
        skill->proficiency = std::clamp(skill->proficiency + delta, 0.0f, 1.0f);
        skill->lastUsed = skill->lastUsed + 1.0f; // Increment usage counter
    }
}

void ProceduralMemory::clear() {
    pImpl->skills.clear();
    pImpl->skillMap.clear();
}

// Associative Memory Implementation
#include <algorithm>
#include <cmath>
#include <unordered_map>

struct AssociativeMemory::Impl {
    // Use multiple data structures for different access patterns
    // 1. Vector for sequential access (preserves order)
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations;
    // 2. Hash maps for O(1) lookups from either direction
    std::unordered_map<NeuronId, std::unordered_map<NeuronId, float>> strengthMap;
    
    Impl() {}
};

AssociativeMemory::AssociativeMemory() : pImpl(new Impl) {}

AssociativeMemory::~AssociativeMemory() = default;

void AssociativeMemory::associate(NeuronId a, NeuronId b, float strength) {
    // Input validation
    if (a == INVALID_NEURON_ID || b == INVALID_NEURON_ID) {
        return;
    }
    
    // Ensure a <= b for consistent ordering
    NeuronId first = std::min(a, b);
    NeuronId second = std::max(a, b);
    
    // Check if association already exists
    for (auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == first && std::get<1>(assoc) == second) ||
            (std::get<0>(assoc) == second && std::get<1>(assoc) == first)) {
            std::get<2>(assoc) = strength;
            pImpl->strengthMap[first][second] = strength;
            return;
        }
    }
    
    // Create new association
    pImpl->associations.emplace_back(first, second, strength);
    pImpl->strengthMap[first][second] = strength;
}

std::vector<NeuronId> AssociativeMemory::getAssociations(NeuronId neuron) const {
    std::vector<NeuronId> result;
    
    // Use efficient lookup from both data structures
    if (neuron == INVALID_NEURON_ID) {
        return result;
    }
    
    // Search in hash map first (O(1))
    auto it1 = pImpl->strengthMap.find(neuron);
    if (it1 != pImpl->strengthMap.end()) {
        for (const auto& [other, strength] : it1->second) {
            result.push_back(other);
        }
    }
    
    // Also search reverse direction if needed (we stored both ways)
    auto it2 = pImpl->strengthMap.find(neuron);
    if (it2 != pImpl->strengthMap.end()) {
        for (const auto& [other, strength] : it2->second) {
            if (std::find(result.begin(), result.end(), other) == result.end()) {
                result.push_back(other);
            }
        }
    }
    
    return result;
}

float AssociativeMemory::getAssociationStrength(NeuronId a, NeuronId b) const {
    // Input validation
    if (a == INVALID_NEURON_ID || b == INVALID_NEURON_ID) {
        return 0.0f;
    }
    
    // Ensure consistent ordering
    NeuronId first = std::min(a, b);
    NeuronId second = std::max(a, b);
    
    // Use fast hash map lookup
    auto it1 = pImpl->strengthMap.find(first);
    if (it1 != pImpl->strengthMap.end()) {
        auto it2 = it1->second.find(second);
        if (it2 != it1->second.end()) {
            return it2->second;
        }
    }
    
    // Fallback to linear search (should not reach here with proper updates)
    for (const auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == first && std::get<1>(assoc) == second) ||
            (std::get<0>(assoc) == second && std::get<1>(assoc) == first)) {
            return std::get<2>(assoc);
        }
    }
    
    return 0.0f;
}

void AssociativeMemory::updateAssociation(NeuronId a, NeuronId b, float delta) {
    if (a == INVALID_NEURON_ID || b == INVALID_NEURON_ID) {
        return;
    }
    
    NeuronId first = std::min(a, b);
    NeuronId second = std::max(a, b);
    
    auto it1 = pImpl->strengthMap.find(first);
    if (it1 != pImpl->strengthMap.end()) {
        auto it2 = it1->second.find(second);
        if (it2 != it1->second.end()) {
            it2->second = std::clamp(it2->second + delta, 0.0f, 1.0f);
            
            // Also update vector version
            for (auto& assoc : pImpl->associations) {
                if ((std::get<0>(assoc) == first && std::get<1>(assoc) == second) ||
                    (std::get<0>(assoc) == second && std::get<1>(assoc) == first)) {
                    std::get<2>(assoc) = it2->second;
                    return;
                }
            }
        }
    }
}

void AssociativeMemory::clear() {
    pImpl->associations.clear();
    pImpl->strengthMap.clear();
}

} // namespace nlm
