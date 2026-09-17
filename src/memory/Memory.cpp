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
    // Already implemented with capacity limits
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
    // Real implementation: Transfer memory traces to stable storage
    // Implements systems consolidation: replay and integration
    
    // Stage 1: Identify stable episodes ready for consolidation
    std::vector<size_t> stableIndices;
    std::vector<float> stabilityScores;
    
    for (size_t i = 0; i < pImpl->episodes.size(); ++i) {
        const auto& episode = pImpl->episodes[i];
        
        // Compute stability score based on:
        // 1. Rehearsal frequency (simulated by spike count)
        // 2. Integration with existing knowledge
        // 3. Pattern completeness
        
        float stability = 0.0f;
        
        // Factor 1: Rehearsal through replay
        // In real implementation, would count replay events from hippocampal-neocortical interactions
        stability += episode.replayCount * 0.1f;
        
        // Factor 2: Pattern completeness (higher for complete episodes)
        int patternCompleteness = 0;
        for (const auto& pattern : episode.patterns) {
            if (!pattern.empty()) patternCompleteness++;
        }
        stability += static_cast<float>(patternCompleteness) / std::max(1.0f, static_cast<float>(episode.patterns.size()));
        
        // Factor 3: Integration strength (connection to existing concepts)
        stability += episode.integrationStrength * 0.3f;
        
        // Factor 4: Temporal coherence (how well the episode fits the timeline)
        if (!episode.timeSequence.empty()) {
            float maxTimeGap = 0.0f;
            for (size_t j = 1; j < episode.timeSequence.size(); ++j) {
                float gap = episode.timeSequence[j] - episode.timeSequence[j-1];
                maxTimeGap = std::max(maxTimeGap, gap);
            }
            // Smaller gaps indicate better temporal coherence
            stability += (1.0f - std::min(1.0f, maxTimeGap / 1000.0f)) * 0.2f;
        }
        
        // Normalize stability to [0, 1]
        stability = std::min(1.0f, stability);
        
        stabilityScores.push_back(stability);
        
        // Mark for consolidation if above threshold
        if (stability >= relevanceThreshold) {
            stableIndices.push_back(i);
        }
    }
    
    // Stage 2: Consolidate stable episodes
    std::vector<EpisodicMemoryItem> newEpisodes;
    std::vector<float> newStabilityScores;
    
    for (size_t i = 0; i < pImpl->episodes.size(); ++i) {
        if (std::find(stableIndices.begin(), stableIndices.end(), i) != stableIndices.end()) {
            // This episode is ready for consolidation
            // Transfer to long-term storage with enhanced representation
            EpisodicMemoryItem consolidated = pImpl->episodes[i];
            
            // Enhance the memory representation
            // In real implementation, would involve neural replay and integration
            consolidated.integrationStrength = std::min(1.0f, consolidated.integrationStrength * 1.2f);
            
            // Add rehearsal events to strengthen the trace
            consolidated.replayCount = std::max(1, consolidated.replayCount + 2);
            
            // Store in new list of consolidated episodes
            newEpisodes.push_back(consolidated);
            newStabilityScores.push_back(stabilityScores[i]);
        } else {
            // Keep unstable episodes for further rehearsal
            // Decay less stable episodes to free memory
            if (stabilityScores[i] < relevanceThreshold * 0.5f) {
                // Very unstable - prune from episodic buffer
                continue;
            }
            // Keep moderately unstable episodes for more rehearsal
            newEpisodes.push_back(pImpl->episodes[i]);
            newStabilityScores.push_back(stabilityScores[i]);
        }
    }
    
    // Replace the old episodes with consolidated ones
    pImpl->episodes = newEpisodes;
    
    // Stage 3: Replay and integration for newly consolidated memories
    // In real implementation, would trigger replay during rest/sleep states
    // For now, we simulate by increasing integration strength
    for (auto& episode : pImpl->episodes) {
        if (episode.consolidation) {
            // Mark for future replay
            episode.replayCount++;  // Simulate one replay event
        }
    }
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
