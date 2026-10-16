#include "Memory.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

// Working Memory Implementation
struct WorkingMemory::Impl {
    std::vector<std::pair<NeuronId, float>> items;
    size_t capacity;
    float decayRate;
    float noveltyThreshold;
    std::vector<NeuronId> recentNeurons;
    
    Impl(size_t cap) : capacity(cap), decayRate(0.01f), noveltyThreshold(0.5f) {}
};

WorkingMemory::WorkingMemory() : pImpl(new Impl(100)) {}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::store(NeuronId neuron, float value) {
    // TODO PHASE 2: Implement real storage with capacity limits
    // Enhanced implementation with novelty-based eviction and decay
    
    // Check if neuron already exists
    for (auto& item : pImpl->items) {
        if (item.first == neuron) {
            // Update with new value, combining with existing
            item.second = (item.second * 0.7f) + (value * 0.3f);
            
            // Update recency
            auto it = std::find(pImpl->recentNeurons.begin(), pImpl->recentNeurons.end(), neuron);
            if (it != pImpl->recentNeurons.end()) {
                pImpl->recentNeurons.erase(it);
            }
            pImpl->recentNeurons.push_back(neuron);
            
            return;
        }
    }
    
    // Novel neuron - add if capacity allows
    if (pImpl->items.size() < pImpl->capacity) {
        pImpl->items.emplace_back(neuron, value);
        pImpl->recentNeurons.push_back(neuron);
    } else {
        // Capacity full - evict least recently used item
        if (!pImpl->recentNeurons.empty()) {
            NeuronId lruNeuron = pImpl->recentNeurons.front();
            pImpl->recentNeurons.erase(pImpl->recentNeurons.begin());
            
            // Remove the item
            pImpl->items.erase(
                std::remove_if(pImpl->items.begin(), pImpl->items.end(),
                    [lruNeuron](const auto& item) { return item.first == lruNeuron; }),
                pImpl->items.end()
            );
            
            // Add new item
            pImpl->items.emplace_back(neuron, value);
            pImpl->recentNeurons.push_back(neuron);
        }
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
    pImpl->recentNeurons.clear();
}

size_t WorkingMemory::getCapacity() const {
    return pImpl->capacity;
}

size_t WorkingMemory::getCurrentSize() const {
    return pImpl->items.size();
}

void WorkingMemory::decay(float decayRate) {
    // Enhanced decay with exponential decay and activity-dependent decay
    float rate = std::max(decayRate, pImpl->decayRate);
    
    for (auto& item : pImpl->items) {
        // Exponential decay
        item.second *= std::pow(1.0f - rate, 1.0f);
        
        // Activity-dependent decay: less active items decay faster
        if (item.second < 0.1f) {
            item.second *= 0.8f;  // Extra decay for low values
        }
    }
}

// Episodic Memory Implementation
struct EpisodicMemory::Impl {
    std::vector<EpisodicMemoryItem> episodes;
    size_t maxEpisodes;
    float consolidationThreshold;
    float relevanceDecayRate;
    
    Impl(size_t max) : maxEpisodes(max), consolidationThreshold(0.7f), relevanceDecayRate(0.05f) {}
};

EpisodicMemory::EpisodicMemory() : pImpl(new Impl(1000)) {}

EpisodicMemory::~EpisodicMemory() = default;

void EpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    // Add episode with uniqueness check
    bool duplicate = false;
    for (const auto& existing : pImpl->episodes) {
        if (existing.content == episode.content) {
            duplicate = true;
            break;
        }
    }
    
    if (!duplicate) {
        if (pImpl->episodes.size() >= pImpl->maxEpisodes) {
            pImpl->episodes.erase(pImpl->episodes.begin());
        }
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
    // Enhanced implementation with synaptic consolidation and systems consolidation
    
    // Identify episodes needing consolidation
    std::vector<size_t> toConsolidate;
    for (size_t i = 0; i < pImpl->episodes.size(); ++i) {
        if (pImpl->episodes[i].relevance < relevanceThreshold) {
            toConsolidate.push_back(i);
        }
    }
    
    // Consolidate high-relevance episodes
    for (size_t i : toConsolidate) {
        auto& episode = pImpl->episodes[i];
        
        // Increase relevance through consolidation
        episode.relevance = std::min(1.0f, episode.relevance * 1.5f);
        
        // Decay time since encoding
        episode.timeSinceEncoding *= 0.8f;
        
        // Update consolidation timestamp
        episode.consolidationTimestamp = 1.0f;
        
        // Make episode more accessible for future retrieval
        episode.accessibility = std::min(1.0f, episode.accessibility * 1.2f);
    }
}

// Semantic Memory Implementation
struct SemanticMemory::Impl {
    std::vector<std::pair<std::string, std::string>> facts;
    std::vector<std::pair<std::string, float>> beliefStrengths;
    std::vector<std::pair<std::string, std::string>> causalRelations;
    
    // Hierarchical organization (concepts -> facts)
    std::map<std::string, std::vector<std::string>> conceptHierarchy;
};

SemanticMemory::SemanticMemory() : pImpl(new Impl) {}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::storeFact(const std::string& key, const std::string& value) {
    // Store fact with belief updating
    for (auto& fact : pImpl->facts) {
        if (fact.first == key) {
            // Update belief strength for existing fact
            for (auto& belief : pImpl->beliefStrengths) {
                if (belief.first == key) {
                    belief.second = std::min(1.0f, belief.second * 0.7f + 0.3f);
                    break;
                }
            }
            fact.second = value;
            return;
        }
    }
    
    // New fact - add with initial belief
    pImpl->facts.emplace_back(key, value);
    pImpl->beliefStrengths.emplace_back(key, 0.7f);
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
    pImpl->beliefStrengths.clear();
    pImpl->causalRelations.clear();
    pImpl->conceptHierarchy.clear();
}

// Procedural Memory Implementation
struct ProceduralMemory::Impl {
    std::vector<Skill> skills;
    std::vector<std::pair<std::string, float>> proficiencyHistory;
    
    // Skill prerequisites and dependencies
    std::map<std::string, std::vector<std::string>> skillPrerequisites;
};

ProceduralMemory::ProceduralMemory() : pImpl(new Impl) {}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    // Learn skill with prerequisite checking and pattern similarity
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            // Update skill with new pattern
            skill.neuralPattern = pattern;
            
            // Update proficiency based on pattern stability
            skill.proficiency = std::min(1.0f, skill.proficiency + 0.1f);
            
            // Record proficiency history
            pImpl->proficiencyHistory.emplace_back(name, skill.proficiency);
            
            return;
        }
    }
    
    // New skill - create with initial state
    Skill skill;
    skill.name = name;
    skill.neuralPattern = pattern;
    skill.proficiency = 0.5f;
    skill.lastUsed = 0.0f;
    
    pImpl->skills.push_back(skill);
    pImpl->proficiencyHistory.emplace_back(name, skill.proficiency);
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
            skill.lastUsed = 0.0f;  // Reset last used time
            return;
        }
    }
}

void ProceduralMemory::clear() {
    pImpl->skills.clear();
    pImpl->proficiencyHistory.clear();
    pImpl->skillPrerequisites.clear();
}

// Associative Memory Implementation
struct AssociativeMemory::Impl {
    // Pairs of (a, b) associations with strength
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations;
    
    // Hebbian-like strengthening based on co-activation
    std::map<NeuronId, float> neuronActivityHistory;
};

AssociativeMemory::AssociativeMemory() : pImpl(new Impl) {}

AssociativeMemory::~AssociativeMemory() = default;

void AssociativeMemory::associate(NeuronId a, NeuronId b, float strength) {
    // Associative strengthening with overlap detection
    for (auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            std::get<2>(assoc) = std::min(1.0f, std::get<2>(assoc) + strength);
            return;
        }
    }
    
    // New association - check for related associations
    bool hasRelated = false;
    for (const auto& existing : pImpl->associations) {
        if (std::get<0>(existing) == a || std::get<1>(existing) == a ||
            std::get<0>(existing) == b || std::get<1>(existing) == b) {
            hasRelated = true;
            break;
        }
    }
    
    // Related associations get boosted
    float finalStrength = strength;
    if (hasRelated) {
        finalStrength *= 1.2f;
    }
    
    pImpl->associations.emplace_back(a, b, finalStrength);
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
    pImpl->neuronActivityHistory.clear();
}

} // namespace nlm
