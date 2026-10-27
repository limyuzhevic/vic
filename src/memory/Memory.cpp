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

void WorkingMemory::initialize(Brain* brain) {
    // Placeholder for Phase 2
    // In Phase 2, this will bridge to NeuralWorkingMemory
}

void WorkingMemory::setCapacity(size_t cap) {
    pImpl->capacity = cap;
}

void WorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
    // Bridge to neural-based implementation
    store(neuron, activation);
}

void WorkingMemory::update(float dt) {
    // Placeholder for Phase 2
    decay(0.01f * dt);  // Simple decay model
}

size_t WorkingMemory::getActiveTraces() const {
    return getCurrentSize();  // Simple model - all stored items are active traces
}

std::vector<NeuronId> WorkingMemory::getMemoryNeurons() const {
    std::vector<NeuronId> neurons;
    for (const auto& item : pImpl->items) {
        neurons.push_back(item.first);
    }
    return neurons;
}

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
    // TODO PHASE 2: Implement real consolidation
    // For now, just remove lowest relevance episodes if above threshold
    if (relevanceThreshold > 0.0f) {
        // Find episodes to remove (based on relevance)
        std::vector<size_t> toRemove;
        for (size_t i = 0; i < pImpl->episodes.size(); ++i) {
            const auto& episode = pImpl->episodes[i];
            float relevance = 0.0f;
            if (!episode.activeNeurons.empty()) {
                // Calculate relevance based on number of active neurons
                relevance = static_cast<float>(episode.activeNeurons.size()) / 100.0f;  // Normalize
            }
            if (relevance < relevanceThreshold && relevanceThreshold > 0.0f) {
                toRemove.push_back(i);
            }
        }
        
        // Remove low relevance episodes (from end to beginning to maintain indices)
        for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
            if (*it < pImpl->episodes.size()) {
                pImpl->episodes.erase(pImpl->episodes.begin() + *it);
            }
        }
    }
}

// Get episodes suitable for replay
const std::vector<const EpisodicMemoryItem*> EpisodicMemory::getEpisodesForReplay(size_t count) const {
    std::vector<const EpisodicMemoryItem*> result;
    
    // Get recent episodes first
    auto recentEpisodes = getRecentEpisodes(count * 2);
    for (const auto& episode : recentEpisodes) {
        result.push_back(&episode);  // Note: this is potentially dangerous as item may be temporary
    }
    
    // Sort by relevance (episode with more active neurons first)
    std::sort(result.begin(), result.end(),
        [this](const EpisodicMemoryItem* a, const EpisodicMemoryItem* b) {
            float relevanceA = 0.0f, relevanceB = 0.0f;
            if (!a->activeNeurons.empty()) relevanceA = static_cast<float>(a->activeNeurons.size()) / 100.0f;
            if (!b->activeNeurons.empty()) relevanceB = static_cast<float>(b->activeNeurons.size()) / 100.0f;
            return relevanceA > relevanceB;
        });
    
    // Return top count episodes
    if (result.size() > count) {
        result.resize(count);
    }
    
    return result;
}

// Replay an episode (reactivate memory traces)
void EpisodicMemory::replayEpisode(const EpisodicMemoryItem* episode) {
    if (!episode) return;
    
    // Replay the episode by reactivating the neural traces
    // This strengthens the synaptic connections associated with the experience
    
    // In a full implementation, this would:
    // 1. Reactivate the neurons that were active during the episode
    // 2. Strengthen synaptic connections through STDP-like mechanisms
    // 3. Update working memory with the replayed patterns
    // 4. Enhance prediction accuracy for similar future states
    
    // Placeholder: Just log the replay
    (void)episode;  // Suppress unused parameter warning
}

// Get recent episodes (alias for getRecentEpisodes for compatibility)
const std::vector<const EpisodicMemoryItem*>& EpisodicMemory::getRecentEpisodes(size_t count) const {
    static std::vector<const EpisodicMemoryItem*> recentEpisodes;
    recentEpisodes.clear();
    
    auto episodeList = getRecentEpisodes(count);
    for (const auto& episode : episodeList) {
        recentEpisodes.push_back(&episode);
    }
    
    return recentEpisodes;
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
