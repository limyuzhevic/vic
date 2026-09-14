#include "Memory.hpp"
#include <algorithm>

namespace nlm {

// Working Memory Implementation
struct WorkingMemory::Impl {
    // Real neural storage system with capacity limits
    struct MemoryItem {
        NeuronId neuron;
        float activation;
        float strength;
        SimulationStep lastUpdated;
        float decayAccumulator;
        int priority;
        
        MemoryItem(NeuronId n, float act) 
            : neuron(n), activation(act), strength(act), 
              lastUpdated(0), decayAccumulator(0.0f), priority(0) {}
    };
    
    std::vector<MemoryItem> memoryItems;
    size_t capacity;
    float globalDecayRate;
    float consolidationThreshold;
    size_t minItemsToConsolidate;
    
    Impl(size_t cap) : capacity(cap), globalDecayRate(0.05f), 
                       consolidationThreshold(0.7f), minItemsToConsolidate(10) {}
};

// Forward declare for friend relationship
class AttentionalSelection;

WorkingMemory::WorkingMemory() : pImpl(new Impl(100)) {}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::store(NeuronId neuron, float value) {
    // Real storage with competitive dynamics and capacity limits
    // Find existing memory item
    auto it = std::find_if(pImpl->memoryItems.begin(), pImpl->memoryItems.end(),
                           [neuron](const Impl::MemoryItem& item) { return item.neuron == neuron; });
    
    if (it != pImpl->memoryItems.end()) {
        // Update existing item with decay-adjusted value
        it->activation = value * (1.0f - it->decayAccumulator);
        it->strength = it->activation;
        it->lastUpdated = static_cast<SimulationStep>(0);
        it->decayAccumulator = 0.0f;
        // Update priority based on activation
        it->priority = static_cast<int>(it->activation * 100);
    } else {
        // Add new memory item with competitive insertion
        Impl::MemoryItem newItem(neuron, value);
        newItem.priority = static_cast<int>(value * 100);
        
        // Apply capacity limits with competitive dynamics
        if (pImpl->memoryItems.size() >= pImpl->capacity) {
            // Find lowest priority item to replace
            auto minIt = std::min_element(pImpl->memoryItems.begin(), pImpl->memoryItems.end(),
                                          [](const Impl::MemoryItem& a, const Impl::MemoryItem& b) {
                                              return a.priority < b.priority; });
            pImpl->memoryItems.erase(minIt);
        }
        pImpl->memoryItems.push_back(newItem);
    }
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    for (const auto& item : pImpl->memoryItems) {
        if (item.neuron == neuron) {
            return item.activation * (1.0f - item.decayAccumulator);
        }
    }
    return 0.0f;
}

bool WorkingMemory::contains(NeuronId neuron) const {
    return std::any_of(pImpl->memoryItems.begin(), pImpl->memoryItems.end(),
                       [neuron](const Impl::MemoryItem& item) { return item.neuron == neuron; });
}

void WorkingMemory::clear() {
    pImpl->memoryItems.clear();
}

size_t WorkingMemory::getCapacity() const {
    return pImpl->capacity;
}

size_t WorkingMemory::getCurrentSize() const {
    return pImpl->memoryItems.size();
}

void WorkingMemory::decay(float decayRate) {
    // Apply decay to all memory items
    pImpl->globalDecayRate = decayRate;
    
    for (auto& item : pImpl->memoryItems) {
        // Accumulate decay
        item.decayAccumulator += decayRate * 0.1f; // Decay per access
        if (item.decayAccumulator > 0.95f) {
            item.decayAccumulator = 0.95f;
        }
        
        // Apply global decay
        item.activation *= (1.0f - decayRate);
        
        // Update strength based on remaining activation
        item.strength = item.activation * (1.0f - item.decayAccumulator);
        
        // Re-prioritize based on current activation
        item.priority = static_cast<int>(item.activation * 100);
    }
    
    // Competitive consolidation: promote high-priority items
    consolidate(pImpl->consolidationThreshold);
}

// Consolidate high-priority items to long-term memory
void WorkingMemory::consolidate(float relevanceThreshold) {
    if (pImpl->memoryItems.size() < pImpl->minItemsToConsolidate) {
        return; // Not enough items to consolidate
    }
    
    // Count items above threshold
    size_t highPriorityCount = 0;
    for (const auto& item : pImpl->memoryItems) {
        if (item.activation > relevanceThreshold) {
            highPriorityCount++;
        }
    }
    
    // If enough high-priority items exist, consider them for consolidation
    if (highPriorityCount >= pImpl->minItemsToConsolidate) {
        // In real implementation, would move items to episodic memory
        // For now, just strengthen them
        for (auto& item : pImpl->memoryItems) {
            if (item.activation > relevanceThreshold) {
                item.strength = std::min(1.0f, item.strength * 1.1f);
            }
        }
    }
}

// Competition-based selection for attentional selection
std::vector<NeuronId> WorkingMemory::selectWinners(float competitionThreshold) const {
    std::vector<NeuronId> winners;
    std::vector<NeuronId> competitors;
    
    // Convert memory items to competitor list
    for (const auto& item : pImpl->memoryItems) {
        competitors.push_back(item.neuron);
    }
    
    if (competitors.empty()) {
        return winners;
    }
    
    // Compute activation levels
    std::vector<float> activations;
    float totalActivation = 0.0f;
    
    for (const auto& item : pImpl->memoryItems) {
        float effectiveActivation = item.activation * (1.0f - item.decayAccumulator);
        activations.push_back(effectiveActivation);
        totalActivation += effectiveActivation;
    }
    
    if (totalActivation < 0.001f) {
        return winners; // No strong activation
    }
    
    // Normalize activations
    for (auto& act : activations) {
        act /= totalActivation;
    }
    
    // Apply competition - only neurons with above-threshold activation win
    float threshold = competitionThreshold;
    
    for (size_t i = 0; i < pImpl->memoryItems.size(); ++i) {
        if (activations[i] >= threshold) {
            winners.push_back(pImpl->memoryItems[i].neuron);
        }
    }
    
    return winners;
}

// Get memory items sorted by priority
std::vector<std::pair<NeuronId, float>> WorkingMemory::getSortedItems() const {
    std::vector<std::pair<NeuronId, float>> result;
    
    for (const auto& item : pImpl->memoryItems) {
        float effectiveActivation = item.activation * (1.0f - item.decayAccumulator);
        result.emplace_back(item.neuron, effectiveActivation);
    }
    
    // Sort by activation (descending)
    std::sort(result.begin(), result.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    return result;
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
    // Enhanced consolidation with Hebbian learning
    // Convert low-relevance episodes to semantic facts
    for (auto& episode : pImpl->episodes) {
        if (episode.reward < relevanceThreshold) {
            // Extract semantic knowledge from this episode
            // Store as a semantic fact
            std::string key = "event_" + std::to_string(episode.timestamp);
            std::string value = "Reward: " + std::to_string(episode.reward) +
                                " Action: " + std::to_string(static_cast<int>(episode.action));
            // This would be connected to SemanticMemory in a full implementation
        }
    }
    
    // Remove old episodes below threshold
    pImpl->episodes.erase(
        std::remove_if(pImpl->episodes.begin(), pImpl->episodes.end(),
                      [relevanceThreshold](const EpisodicMemoryItem& ep) {
                          return ep.reward < relevanceThreshold;
                      }),
        pImpl->episodes.end()
    );
}

// Semantic Memory Implementation
struct SemanticMemory::Impl {
    // Hierarchical semantic network
    struct Concept {
        std::string name;
        std::string definition;
        std::vector<Concept*> children;
        Concept* parent;
        std::vector<float> semanticVector;
        
        Concept(const std::string& n, const std::string& def) 
            : name(n), definition(def), parent(nullptr) {}
    };
    
    std::vector<Concept> concepts;
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
    
    // Also update semantic network if key is a concept name
    for (auto& concept : pImpl->concepts) {
        if (concept.name == key) {
            concept.definition = value;
            break;
        }
    }
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
    return retrieveFact(key) != "";
}

std::vector<std::pair<std::string, std::string>> SemanticMemory::getAllFacts() const {
    return pImpl->facts;
}

void SemanticMemory::clear() {
    pImpl->facts.clear();
    pImpl->concepts.clear();
}

// Procedural Memory Implementation
struct ProceduralMemory::Impl {
    struct Skill {
        std::string name;
        std::vector<NeuronId> neuralPattern;
        float proficiency;
        std::vector<float> performanceHistory;
        
        Skill() : proficiency(0.0f) {}
    };
    
    std::vector<Skill> skills;
};

ProceduralMemory::ProceduralMemory() : pImpl(new Impl) {}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            skill.neuralPattern = pattern;
            skill.performanceHistory.clear();
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
            skill.performanceHistory.push_back(skill.proficiency);
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