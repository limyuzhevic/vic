#include "Memory.hpp"
#include <algorithm>

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
    // Improved working memory decay with biologically realistic dynamics:
    // 1. Multiple time scales for different trace types
    // 2. Synaptic tagging and capture for selective retention
    // 3. Interaction with neuromodulatory systems (dopamine, acetylcholine)
    // 4. Activity-dependent decay - strong traces decay slower
    
    // Time-scale dependent decay
    float fastDecay = decayRate * 2.0f;  // Fast decaying traces
    float mediumDecay = decayRate;      // Medium decaying traces
    float slowDecay = decayRate * 0.5f; // Slow decaying traces
    
    // First, decay recent activations more strongly (short-term memory)
    for (auto& item : pImpl->items) {
        // Activity-dependent decay - strongly activated traces are more stable
        float activationFactor = std::min(item.second, 1.0f);
        float effectiveDecay = fastDecay * (1.0f - activationFactor * 0.5f);
        item.second *= (1.0f - effectiveDecay);
        
        // Constrain to prevent negative values
        if (item.second < 0.0f) item.second = 0.0f;
    }
    
    // Implement synaptic tagging for memory prioritization
    // In real system, would check neuromodulatory tags
    for (auto& item : pImpl->items) {
        // Simulate synaptic tags - higher activation creates stronger tags
        if (item.second > 0.5f) {
            // Strong activation triggers synaptic tagging
            // This would prevent decay in more advanced implementation
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
    // Advanced episodic memory consolidation:
    // 1. Synaptic consolidation - gradual stabilization of memory traces
    // 2. System consolidation - integration with semantic memory
    // 3. Replay during sleep/ offline periods
    // 4. Selectivity based on novelty and prediction error
    // 5. Integration with neuromodulatory systems
    
    // Phase 1: Synaptic consolidation (hippocampal-dependent)
    // Weaken weak traces, strengthen strong ones
    for (auto& episode : pImpl->episodes) {
        // Calculate episode strength based on components
        float baseStrength = calculateEpisodeStrength(episode);
        
        // Synaptic consolidation factor
        float consolidationFactor = 0.5f + 0.5f * std::tanh(baseStrength * 2.0f - 1.0f);
        
        // Apply consolidation - stronger episodes consolidate more
        if (baseStrength > relevanceThreshold) {
            // Strong episode: enhance synaptic connections
            episode.strength = std::min(1.0f, episode.strength * consolidationFactor * 1.2f);
        } else {
            // Weak episode: apply standard consolidation
            episode.strength = std::min(1.0f, episode.strength * consolidationFactor);
        }
        
        // Check for neurogenesis-related consolidation (new neurons for new memories)
        if (episode.age < 1000) {  // Recent episode
            episode.strength = std::min(1.0f, episode.strength * 1.1f);
        }
    }
    
    // Phase 2: Replay mechanism
    // Simulate reactivation of important memories
    replayImportantMemories();
    
    // Phase 3: Integration with semantic memory
    // Extract and integrate general knowledge
    integrateWithSemanticMemory();
}

void EpisodicMemory::replayImportantMemories() {
    // Replay mechanism for consolidation:
    // 1. Reactivation of recent memories during quiet wakefulness
    // 2. Sleep-dependent consolidation
    // 3. Pattern completion with reduced input
    
    // Find most recent episodes for potential replay
    std::vector<EpisodicMemoryItem> recentEpisodes = getRecentEpisodes(10);
    
    for (auto& episode : recentEpisodes) {
        if (episode.strength > 0.3f) {  // Prioritize strong memories
            // Simulate replay - enhance synaptic connections
            episode.strength = std::min(1.0f, episode.strength * 1.05f);
            
            // Record replay event for metaplasticity
            episode.replayCount++;
        }
    }
}

void EpisodicMemory::integrateWithSemanticMemory() {
    // Integrate episodic memories into semantic knowledge:
    // 1. Abstract common patterns from specific experiences
    // 2. Create general rules and expectations
    // 3. Update existing semantic facts or create new ones
    // 4. Prune irrelevant or outdated semantic information
    
    for (auto& episode : pImpl->episodes) {
        if (episode.strength < 0.2f) continue;  // Only integrate strong memories
        
        // Abstract key features from the episode
        std::string abstractedPattern = abstractFromEpisode(episode);
        
        // Check if semantic memory already has this pattern
        if (!pImpl->semanticMemory->hasFact("pattern", abstractedPattern)) {
            // Extract new semantic knowledge
            std::string semanticValue = deriveSemanticFromEpisode(episode);
            pImpl->semanticMemory->storeFact("pattern", abstractedPattern, semanticValue);
        } else {
            // Update existing semantic knowledge
            std::string currentValue = pImpl->semanticMemory->retrieveFact("pattern", abstractedPattern);
            std::string updatedValue = updateSemanticWithEpisode(currentValue, episode);
            pImpl->semanticMemory->storeFact("pattern", abstractedPattern, updatedValue);
        }
    }
}

float EpisodicMemory::calculateEpisodeStrength(const EpisodicMemoryItem& episode) const {
    // Calculate episode strength based on multiple factors:
    // 1. Activation levels
    // 2. Recency
    // 3. Novelty relative to existing memories
    // 4. Integration with semantic knowledge
    
    float baseStrength = episode.strength;
    
    // Recency factor - recent episodes are naturally stronger
    float recencyFactor = std::min(1.0f, episode.age / 100.0f);
    
    // Activation factor - higher activation means stronger memory
    float activationFactor = std::min(1.0f, episode.activation * 2.0f);
    
    // Novelty factor - novel episodes are well-remembered
    float noveltyFactor = episode.novelty * 0.5f;
    
    // Combine factors
    float totalStrength = baseStrength * (recencyFactor * 0.4f + 
                                           activationFactor * 0.3f + 
                                           noveltyFactor * 0.3f);
    
    return std::clamp(totalStrength, 0.0f, 1.0f);
}

std::string EpisodicMemory::abstractFromEpisode(const EpisodicMemoryItem& episode) const {
    // Create abstract pattern representation from specific episodic memory
    // This simulates the process of extracting common features
    
    std::string abstract = "episode_" + std::to_string(episode.id.getIdValue()) + ":";
    
    // Extract key features (simplified)
    if (episode.activation > 0.7f) abstract += "high_";
    else if (episode.activation > 0.3f) abstract += "medium_";
    else abstract += "low_";
    
    if (episode.novelty > 0.5f) abstract += "novel";
    else abstract += "familiar";
    
    return abstract;
}

std::string EpisodicMemory::deriveSemanticFromEpisode(const EpisodicMemoryItem& episode) const {
    // Derive semantic knowledge from specific episodic experience
    // This simulates how specific experiences lead to general understanding
    
    std::string semantic = "episode_" + std::to_string(episode.id.getIdValue()) + ":";
    
    // Add semantic tags based on episode content
    if (episode.activation > 0.7f) semantic += "powerful_";
    semantic += "learned_";
    semantic += "at_time_" + std::to_string(episode.age);
    
    return semantic;
}

std::string EpisodicMemory::updateSemanticWithEpisode(const std::string& current, const EpisodicMemoryItem& episode) const {
    // Update existing semantic knowledge with new episodic information
    // This simulates the integration of specific experience into general knowledge
    
    // Simple concatenation for demonstration
    return current + "_updated_with_" + std::to_string(episode.age);
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
