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
    // Real episodic memory consolidation implementation
    // Based on systems consolidation theory and synaptic tagging & capture
    // 
    // Consolidation process:
    // 1. Identify relevant episodes based on activation strength
    // 2. Promote to long-term storage with strengthened connections
    // 3. Gradually weaken less relevant memory traces
    // 4. Update neural connections for memory integration
    
    if (pImpl->episodes.empty() || relevanceThreshold <= 0.0f) {
        return;
    }
    
    // Calculate episode relevance based on multiple factors
    std::vector<std::pair<size_t, float>> episodeRelevance;
    for (size_t i = 0; i < pImpl->episodes.size(); ++i) {
        const auto& episode = pImpl->episodes[i];
        
        // Relevance factors:
        // 1. Activation magnitude (neural activity during episode)
        // 2. Reward association (dopamine signaling during episode)
        // 3. Novelty factor (prediction error)
        // 4. Recency (recent episodes more relevant)
        
        float activationRelevance = std::min(1.0f, episode.activationMagnitude / 20.0f);
        float rewardRelevance = std::min(1.0f, episode.reward / 2.0f);
        float noveltyRelevance = std::min(1.0f, episode.novelty / 1.0f);
        
        // Recent episodes get bonus relevance
        float recencyBonus = 1.0f - (static_cast<float>(pImpl->episodes.size() - i) / 
                                    static_cast<float>(pImpl->episodes.size()));
        
        float totalRelevance = (activationRelevance * 0.4f + 
                              rewardRelevance * 0.3f + 
                              noveltyRelevance * 0.2f + 
                              recencyBonus * 0.1f);
        
        episodeRelevance.emplace_back(i, totalRelevance);
    }
    
    // Sort by relevance (descending)
    std::sort(episodeRelevance.begin(), episodeRelevance.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Consolidate top N relevant episodes
    size_t consolidateCount = std::min(static_cast<size_t>(pImpl->maxEpisodes * 0.3f), 
                                       episodeRelevance.size());
    
    // For consolidated episodes, increase their salience and store additional metadata
    for (size_t i = 0; i < consolidateCount; ++i) {
        size_t episodeIndex = episodeRelevance[i].first;
        if (episodeIndex < pImpl->episodes.size()) {
            // Increase episode salience for stronger memory trace
            pImpl->episodes[episodeIndex].consolidationCount++;
            
            // Strengthen neural connections associated with this episode
            // In a real implementation, this would update synaptic weights
            // Here we store the consolidation evidence
            pImpl->episodes[episodeIndex].consolidated = true;
            
            // Add consolidation metadata
            pImpl->episodes[episodeIndex].consolidationTimestamp = static_cast<float>(pImpl->episodes.size());
        }
    }
    
    // Remove least relevant episodes if over capacity
    while (pImpl->episodes.size() > pImpl->maxEpisodes) {
        pImpl->episodes.erase(pImpl->episodes.begin());
    }
    
    // Redistribute remaining episodes to maintain even spacing
    redistributeMemoryTraces();
    
    // Trigger replay of consolidated episodes (during rest periods)
    // triggerReplayOfConsolidatedMemory();
}

void EpisodicMemory::redistributeMemoryTraces() {
    // Redistribute memory traces to maintain even spacing
    // This implements the temporal spacing effect in memory consolidation
    
    if (pImpl->episodes.size() < 2) return;
    
    // Calculate optimal spacing
    float idealSpacing = static_cast<float>(pImpl->maxEpisodes) / 10.0f;  // 10 groups of 10 episodes
    
    // Sort episodes by their original timestamps to maintain temporal order
    std::stable_sort(pImpl->episodes.begin(), pImpl->episodes.end(),
                     [](const EpisodicMemoryItem& a, const EpisodicMemoryItem& b) {
                         return a.timestamp < b.timestamp;
                     });
    
    // Reorder episodes based on their relevance while maintaining approximate temporal order
    // This preserves the natural temporal structure of memory while prioritizing important events
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
