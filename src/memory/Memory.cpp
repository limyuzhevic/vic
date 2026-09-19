#include "Memory.hpp"
#include <algorithm>

namespace nlm {

// Working Memory Implementation
struct WorkingMemory::Impl {
    // Neuronal working memory with competition and decay
    struct MemoryItem {
        NeuronId neuron;
        float activation;           // Current activation level
        float maxActivation;        // Peak activation in this episode
        float decayRate;            // Item-specific decay rate
        float lastUpdated;          // Last time this item was updated
        bool isConsolidated;        // Whether item has been consolidated to long-term memory
    };
    
    std::vector<MemoryItem> items;
    std::unordered_map<NeuronId, size_t> neuronIndexMap;  // Fast lookup
    size_t capacity;
    float decayRate;              // Global decay rate
    size_t maxConsolidated;        // Maximum items that can be consolidated
    std::vector<NeuronId> consolidatedItems;  // Items moved to long-term storage
    
    Impl(size_t cap) : capacity(cap), decayRate(0.001f), maxConsolidated(cap / 10) {
        // Initialize with competitive dynamics
        items.reserve(capacity);
    }
};

WorkingMemory::WorkingMemory() : pImpl(new Impl(100)) {}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::store(NeuronId neuron, float value) {
    // Get or create memory item for this neuron
    auto& indexMap = pImpl->neuronIndexMap;
    auto& items = pImpl->items;
    
    // Check if item exists
    auto it = indexMap.find(neuron.value);
    if (it != indexMap.end()) {
        // Update existing item
        size_t idx = it->second;
        items[idx].activation = value;
        items[idx].maxActivation = std::max(items[idx].maxActivation, value);
        items[idx].lastUpdated = pImpl->items.size();  // Using item count as time proxy
    } else {
        // Create new item
        if (items.size() < pImpl->capacity) {
            size_t idx = items.size();
            items.emplace_back(MemoryItem{neuron, value, value, pImpl->decayRate, 
                                        static_cast<float>(items.size()), false});
            indexMap[neuron.value] = idx;
        } else {
            // Competitive replacement: remove lowest activation item
            size_t weakestIdx = 0;
            for (size_t i = 1; i < items.size(); ++i) {
                if (items[i].activation < items[weakestIdx].activation) {
                    weakestIdx = i;
                }
            }
            
            // Remove the weakest item from map
            indexMap.erase(items[weakestIdx].neuron.value);
            items[weakestIdx] = MemoryItem{neuron, value, value, pImpl->decayRate,
                                          static_cast<float>(items.size()), false};
            indexMap[neuron.value] = weakestIdx;
        }
    }
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    auto it = pImpl->neuronIndexMap.find(neuron.value);
    if (it != pImpl->neuronIndexMap.end()) {
        return pImpl->items[it->second].activation;
    }
    return 0.0f;
}

bool WorkingMemory::contains(NeuronId neuron) const {
    return pImpl->neuronIndexMap.find(neuron.value) != pImpl->neuronIndexMap.end();
}

void WorkingMemory::clear() {
    pImpl->items.clear();
    pImpl->neuronIndexMap.clear();
    pImpl->consolidatedItems.clear();
}

size_t WorkingMemory::getCapacity() const {
    return pImpl->capacity;
}

size_t WorkingMemory::getCurrentSize() const {
    return pImpl->items.size();
}

void WorkingMemory::decay(float decayRate) {
    // Apply global decay
    for (auto& item : pImpl->items) {
        item.activation *= (1.0f - decayRate);
        // Also apply item-specific decay
        item.activation *= (1.0f - item.decayRate * decayRate);
    }
    
    // Consolidate strong items
    consolidateStrongItems();
}

void WorkingMemory::consolidateStrongItems() {
    // Move items with high activation to long-term storage
    auto& items = pImpl->items;
    auto& consolidated = pImpl->consolidatedItems;
    
    for (size_t i = 0; i < items.size(); ++i) {
        if (!items[i].isConsolidated && items[i].maxActivation > 0.8f) {
            // Check if we've reached consolidation limit
            if (consolidated.size() < pImpl->maxConsolidated) {
                items[i].isConsolidated = true;
                consolidated.push_back(items[i].neuron);
            }
        }
    }
}

std::vector<NeuronId> WorkingMemory::getConsolidatedItems() const {
    return pImpl->consolidatedItems;
}

float WorkingMemory::getTotalActivation() const {
    float total = 0.0f;
    for (const auto& item : pImpl->items) {
        total += item.activation;
    }
    return total;
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
    // Implement real consolidation
    if (relevanceThreshold <= 0.0f) return;
    
    auto& episodes = pImpl->episodes;
    auto& workingMem = pImpl->workingMemory;
    
    // Find episodes with high relevance (based on activation)
    std::vector<size_t> relevantIndices;
    for (size_t i = 0; i < episodes.size(); ++i) {
        const auto& episode = episodes[i];
        
        // Calculate episode relevance from active neurons
        float relevance = 0.0f;
        for (const auto& neuronId : episode.neurons) {
            if (workingMem && workingMem->contains(neuronId)) {
                float activation = workingMem->retrieve(neuronId);
                relevance += std::min(activation * 10.0f, 1.0f);  // Scale and cap
            }
        }
        
        relevance /= std::max(static_cast<float>(episode.neurons.size()), 1.0f);
        
        if (relevance >= relevanceThreshold) {
            relevantIndices.push_back(i);
        }
    }
    
    // Sort by relevance and keep most relevant
    std::sort(relevantIndices.begin(), relevantIndices.end(),
        [&episodes](size_t a, size_t b) {
            return episodes[a].reward > episodes[b].reward;
        });
    
    // Keep only top 30% of relevant episodes
    size_t keepCount = static_cast<size_t>(episodes.size() * 0.3f);
    if (keepCount < relevantIndices.size()) {
        // Remove less relevant episodes (starting from lowest index first)
        std::set<size_t> keepSet(relevantIndices.end() - keepCount, relevantIndices.end());
        std::vector<EpisodicMemoryItem> filtered;
        
        for (size_t i = 0; i < episodes.size(); ++i) {
            if (keepSet.find(i) != keepSet.end()) {
                filtered.push_back(episodes[i]);
            }
        }
        
        episodes = filtered;
    }
    
    // Update episode indices
    for (auto& episode : episodes) {
        episode.timestamp = static_cast<SimulationStep>(episodes.size());
    }
}

// Semantic Memory Implementation
struct SemanticMemory::Impl {
    std::vector<std::pair<std::string, std::string>> facts;
    
    // For pattern-based semantic associations
    std::vector<std::vector<std::string>> patterns;
    std::vector<float> patternStrengths;
};

SemanticMemory::SemanticMemory() : pImpl(new Impl) {}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::storeFact(const std::string& key, const std::string& value) {
    // Store semantic fact with pattern learning
    for (auto& fact : pImpl->facts) {
        if (fact.first == key) {
            fact.second = value;
            // Update associated patterns
            updateFactPatterns(key, value);
            return;
        }
    }
    pImpl->facts.emplace_back(key, value);
    
    // Create pattern from fact components
    std::vector<std::string> pattern;
    size_t spacePos = key.find(' ');
    if (spacePos != std::string::npos) {
        pattern.push_back(key.substr(0, spacePos));
        pattern.push_back(key.substr(spacePos + 1));
    } else {
        pattern.push_back(key);
    }
    pImpl->patterns.push_back(pattern);
    pImpl->patternStrengths.push_back(1.0f);
}

std::string SemanticMemory::retrieveFact(const std::string& key) const {
    for (const auto& fact : pImpl->facts) {
        if (fact.first == key) {
            return fact.second;
        }
    }
    
    // Try pattern matching
    std::vector<std::string> pattern;
    size_t spacePos = key.find(' ');
    if (spacePos != std::string::npos) {
        pattern.push_back(key.substr(0, spacePos));
        pattern.push_back(key.substr(spacePos + 1));
    } else {
        pattern.push_back(key);
    }
    
    for (size_t i = 0; i < pImpl->patterns.size(); ++i) {
        if (patternMatches(pattern, pImpl->patterns[i])) {
            return pImpl->facts[i].second;
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
    pImpl->patterns.clear();
    pImpl->patternStrengths.clear();
}

void SemanticMemory::updateFactPatterns(const std::string& key, const std::string& value) {
    std::vector<std::string> newPattern;
    size_t spacePos = key.find(' ');
    if (spacePos != std::string::npos) {
        newPattern.push_back(key.substr(0, spacePos));
        newPattern.push_back(key.substr(spacePos + 1));
    } else {
        newPattern.push_back(key);
    }
    
    // Find and update matching pattern
    for (size_t i = 0; i < pImpl->patterns.size(); ++i) {
        if (patternMatches(newPattern, pImpl->patterns[i])) {
            pImpl->patternStrengths[i] = std::min(pImpl->patternStrengths[i] * 1.1f, 1.0f);
            return;
        }
    }
    
    // Add new pattern
    pImpl->patterns.push_back(newPattern);
    pImpl->patternStrengths.push_back(1.0f);
}

bool SemanticMemory::patternMatches(const std::vector<std::string>& pattern1, const std::vector<std::string>& pattern2) const {
    if (pattern1.size() != pattern2.size()) return false;
    for (size_t i = 0; i < pattern1.size(); ++i) {
        if (pattern1[i] != pattern2[i]) return false;
    }
    return true;
}

// Procedural Memory Implementation
struct ProceduralMemory::Impl {
    std::vector<Skill> skills;
    
    // For skill association learning
    std::vector<std::vector<std::string>> skillPatterns;
    std::vector<float> skillProficiencies;
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
    
    // Create skill pattern
    std::vector<std::string> skillPattern;
    // Extract action components from pattern
    for (const auto& neuronId : pattern) {
        skillPattern.push_back(std::to_string(neuronId.value));
    }
    pImpl->skillPatterns.push_back(skillPattern);
    pImpl->skillProficiencies.push_back(0.0f);
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
    for (size_t i = 0; i < pImpl->skills.size(); ++i) {
        if (pImpl->skills[i].name == name) {
            pImpl->skills[i].proficiency = std::clamp(pImpl->skills[i].proficiency + delta, 0.0f, 1.0f);
            if (i < pImpl->skillProficiencies.size()) {
                pImpl->skillProficiencies[i] = pImpl->skills[i].proficiency;
            }
            return;
        }
    }
}

void ProceduralMemory::clear() {
    pImpl->skills.clear();
    pImpl->skillPatterns.clear();
    pImpl->skillProficiencies.clear();
}

// Associative Memory Implementation
struct AssociativeMemory::Impl {
    // Pairs of (a, b) associations with strength
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations;
    
    // For temporal associations
    std::vector<std::tuple<SimulationStep, NeuronId, NeuronId, float>> temporalAssociations;
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
    
    // Also check temporal associations (filter by recent)
    for (const auto& temporal : pImpl->temporalAssociations) {
        if (std::get<1>(temporal) == neuron) {
            result.push_back(std::get<2>(temporal));
        } else if (std::get<2>(temporal) == neuron) {
            result.push_back(std::get<1>(temporal));
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
    
    // Check temporal associations (weighted by recency)
    float maxStrength = 0.0f;
    for (const auto& temporal : pImpl->temporalAssociations) {
        auto a1 = std::get<1>(temporal);
        auto b1 = std::get<2>(temporal);
        float strength = std::get<3>(temporal);
        if (((a1 == a && b1 == b) || (a1 == b && b1 == a)) && strength > maxStrength) {
            maxStrength = strength;
        }
    }
    return maxStrength;
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

void AssociativeMemory::recordTemporalAssociation(SimulationStep time, NeuronId a, NeuronId b, float strength) {
    pImpl->temporalAssociations.emplace_back(time, a, b, strength);
    
    // Add to standard associations with decayed strength for backward compatibility
    associate(a, b, strength * 0.5f);
}

std::vector<AssociativeMemory::TemporalAssociation> AssociativeMemory::getRecentTemporalAssociations(SimulationStep cutoff) const {
    std::vector<TemporalAssociation> result;
    for (const auto& temporal : pImpl->temporalAssociations) {
        if (std::get<0>(temporal) >= cutoff) {
            result.push_back(TemporalAssociation{
                std::get<0>(temporal),
                std::get<1>(temporal),
                std::get<2>(temporal),
                std::get<3>(temporal)
            });
        }
    }
    return result;
}

void AssociativeMemory::clear() {
    pImpl->associations.clear();
    pImpl->temporalAssociations.clear();
}

} // namespace nlm
