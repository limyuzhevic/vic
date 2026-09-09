// Core memory implementation with neural-based systems

#include "Memory.hpp"
#include <algorithm>

namespace nlm {

// Working Memory Implementation - neural-based working memory
struct WorkingMemory::Impl {
    std::vector<std::pair<NeuronId, float>> items;
    size_t capacity;
    float decayRate;
    
    Impl(size_t cap) : capacity(cap), decayRate(0.01f) {}
};

WorkingMemory::WorkingMemory() : pImpl(new Impl(100)) {}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::store(NeuronId neuron, float value) {
    // Find existing entry
    for (auto& item : pImpl->items) {
        if (item.first == neuron) {
            item.second = value;
            return;
        }
    }
    
    // Add new entry if capacity allows
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
    pImpl->decayRate = decayRate;
    for (auto& item : pImpl->items) {
        item.second *= (1.0f - decayRate);
    }
}

void WorkingMemory::update(float decayRate) {
    decay(decayRate);
}

// Episodic Memory Implementation - stores experience episodes
struct EpisodicMemory::Impl {
    std::vector<EpisodicMemoryItem> episodes;
    size_t maxEpisodes;
    size_t replayInterval;
    
    Impl(size_t max, size_t replay) : maxEpisodes(max), replayInterval(replay) {}
};

EpisodicMemory::EpisodicMemory() : pImpl(new Impl(1000, 100)) {}

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
    // Remove episodes with low relevance based on rewards and recent activation
    std::vector<EpisodicMemoryItem> consolidated;
    
    for (const auto& episode : pImpl->episodes) {
        // Calculate relevance score
        float relevance = 0.0f;
        
        // Higher reward = higher relevance
        relevance += std::min(1.0f, episode.reward);
        
        // More recent = higher relevance  
        relevance += 1.0f - std::min(1.0f, static_cast<float>(episode.age) / 100.0f);
        
        // Many active neurons = higher relevance
        relevance += std::min(1.0f, static_cast<float>(episode.activeNeurons.size()) / 20.0f);
        
        if (relevance >= relevanceThreshold) {
            consolidated.push_back(episode);
        }
    }
    
    pImpl->episodes = consolidated;
}

std::vector<const EpisodicMemoryItem*> EpisodicMemory::getEpisodesForReplay(size_t count) const {
    std::vector<std::tuple<float, size_t>> scored;  // (score, index)
    
    for (size_t i = 0; i < pImpl->episodes.size(); ++i) {
        float recencyScore = 1.0f / (pImpl->episodes[i].age + 1);
        float rewardScore = pImpl->episodes[i].reward;
        float score = recencyScore * 0.5f + rewardScore * 0.5f;
        scored.emplace_back(score, i);
    }
    
    std::sort(scored.begin(), scored.end(),
        [](const auto& a, const auto& b) { return std::get<0>(a) > std::get<0>(b); });
    
    std::vector<const EpisodicMemoryItem*> results;
    for (size_t i = 0; i < std::min(count, scored.size()); ++i) {
        results.push_back(&pImpl->episodes[std::get<1>(scored[i])]);
    }
    
    return results;
}

void EpisodicMemory::replayEpisode(const EpisodicMemoryItem* episode) {
    // Reactivate neurons from episode
    // In real implementation, this would inject current into brain neurons
    // For now, just log the replay
}

// Semantic Memory Implementation - stores facts and concepts
struct SemanticMemory::Impl {
    std::vector<Fact> facts;
    std::unordered_map<std::string, std::vector<float>> conceptVectors;
    
    Impl() {}
};

SemanticMemory::SemanticMemory() : pImpl(new Impl) {}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::storeFact(const std::string& subject, const std::string& predicate, const std::string& object) {
    Fact fact;
    fact.subject = subject;
    fact.predicate = predicate;
    fact.object = object;
    fact.confidence = 1.0f;
    
    // Check if fact already exists
    for (auto& existing : pImpl->facts) {
        if (existing.subject == subject && existing.predicate == predicate && existing.object == object) {
            existing.confidence = 1.0f;
            return;
        }
    }
    
    pImpl->facts.push_back(fact);
}

std::string SemanticMemory::retrieveFact(const std::string& subject, const std::string& predicate) const {
    for (const auto& fact : pImpl->facts) {
        if (fact.subject == subject && fact.predicate == predicate) {
            return fact.object;
        }
    }
    return "";
}

bool SemanticMemory::hasFact(const std::string& subject, const std::string& predicate) const {
    for (const auto& fact : pImpl->facts) {
        if (fact.subject == subject && fact.predicate == predicate) {
            return true;
        }
    }
    return false;
}

std::vector<Fact> SemanticMemory::getAllFacts() const {
    return pImpl->facts;
}

void SemanticMemory::clear() {
    pImpl->facts.clear();
    pImpl->conceptVectors.clear();
}

// Procedural Memory Implementation - stores skills and habits
struct ProceduralMemory::Impl {
    std::vector<Skill> skills;
    std::unordered_map<std::string, std::vector<NeuronId>> actionPatterns;
    
    Impl() {}
};

ProceduralMemory::ProceduralMemory() : pImpl(new Impl) {}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern, float initialProficiency) {
    Skill skill;
    skill.name = name;
    skill.neuralPattern = pattern;
    skill.proficiency = initialProficiency;
    skill.lastUsed = 0;
    
    // Check if skill already exists
    for (auto& existing : pImpl->skills) {
        if (existing.name == name) {
            existing.neuralPattern = pattern;
            existing.proficiency = initialProficiency;
            return;
        }
    }
    
    pImpl->skills.push_back(skill);
    pImpl->actionPatterns[name] = pattern;
}

Skill* ProceduralMemory::getSkill(const std::string& name) {
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            return &skill;
        }
    }
    return nullptr;
}

const Skill* ProceduralMemory::getSkill(const std::string& name) const {
    for (const auto& skill : pImpl->skills) {
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
            skill.lastUsed = 0; // Reset last used when proficiency changes
            return;
        }
    }
}

void ProceduralMemory::recordUsage(const std::string& name) {
    Skill* skill = getSkill(name);
    if (skill) {
        skill->lastUsed = 0; // Increment in real implementation
    }
}

void ProceduralMemory::clear() {
    pImpl->skills.clear();
    pImpl->actionPatterns.clear();
}

// Associative Memory Implementation - stores connections between concepts
struct AssociativeMemory::Impl {
    std::vector<Association> associations;
    std::unordered_map<NeuronId, std::vector<Association>> neuronToAssociations;
    
    Impl() {}
};

AssociativeMemory::AssociativeMemory() : pImpl(new Impl) {}

AssociativeMemory::~AssociativeMemory() = default;

void AssociativeMemory::associate(NeuronId a, NeuronId b, float strength) {
    // Check if association already exists
    for (auto& assoc : pImpl->associations) {
        if ((assoc.neuronA == a && assoc.neuronB == b) || 
            (assoc.neuronA == b && assoc.neuronB == a)) {
            assoc.strength = strength;
            return;
        }
    }
    
    Association assoc;
    assoc.neuronA = a;
    assoc.neuronB = b;
    assoc.strength = strength;
    assoc.lastActivated = 0;
    
    pImpl->associations.push_back(assoc);
    
    // Update reverse index
    pImpl->neuronToAssociations[a].push_back(assoc);
    pImpl->neuronToAssociations[b].push_back(assoc);
}

std::vector<NeuronId> AssociativeMemory::getAssociations(NeuronId neuron) const {
    std::vector<NeuronId> result;
    for (const auto& assoc : pImpl->associations) {
        if (assoc.neuronA == neuron) {
            result.push_back(assoc.neuronB);
        } else if (assoc.neuronB == neuron) {
            result.push_back(assoc.neuronA);
        }
    }
    return result;
}

float AssociativeMemory::getAssociationStrength(NeuronId a, NeuronId b) const {
    for (const auto& assoc : pImpl->associations) {
        if ((assoc.neuronA == a && assoc.neuronB == b) || 
            (assoc.neuronA == b && assoc.neuronB == a)) {
            return assoc.strength;
        }
    }
    return 0.0f;
}

void AssociativeMemory::updateAssociation(NeuronId a, NeuronId b, float delta) {
    for (auto& assoc : pImpl->associations) {
        if ((assoc.neuronA == a && assoc.neuronB == b) || 
            (assoc.neuronA == b && assoc.neuronB == a)) {
            assoc.strength = std::clamp(assoc.strength + delta, 0.0f, 1.0f);
            assoc.lastActivated = 0;
            return;
        }
    }
}

void AssociativeMemory::recordActivation(NeuronId a, NeuronId b) {
    for (auto& assoc : pImpl->associations) {
        if ((assoc.neuronA == a && assoc.neuronB == b) || 
            (assoc.neuronA == b && assoc.neuronB == a)) {
            assoc.lastActivated = 0; // Increment in real implementation
        }
    }
}

void AssociativeMemory::clear() {
    pImpl->associations.clear();
    pImpl->neuronToAssociations.clear();
}

} // namespace nlm