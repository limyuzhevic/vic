#include "Memory.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Working Memory Implementation
struct WorkingMemory::Impl {
    // Neural-like memory traces with persistent activity
    struct MemoryTrace {
        NeuronId neuron;
        float activation;
        float strength;
        float age;
        float decayAccumulator;
        SimulationStep lastUpdate;
        float competitiveRank;
        
        MemoryTrace() : neuron(), activation(0.0f), strength(0.0f), age(0.0f),
                       decayAccumulator(0.0f), lastUpdate(0), competitiveRank(0.0f) {}
        MemoryTrace(NeuronId n, float act) : neuron(n), activation(act), strength(act), age(0.0f),
                                           decayAccumulator(0.0f), lastUpdate(0), competitiveRank(act) {}
    };
    
    std::vector<MemoryTrace> traces;
    size_t capacity;
    float globalInhibition;
    float LTP_threshold;
    float LTD_threshold;
    float maxTraceAge;
    
    Impl(size_t cap) : capacity(cap), globalInhibition(0.5f), LTP_threshold(0.7f), 
                       LTD_threshold(0.3f), maxTraceAge(1000.0f) {}
};

WorkingMemory::WorkingMemory() : pImpl(new Impl(100)) {}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::store(NeuronId neuron, float value) {
    // Implement neural-like storage with capacity limits and competition
    auto currentStep = SimulationStep(0); // Would come from clock in real usage
    
    // Find existing trace
    auto it = std::find_if(pImpl->traces.begin(), pImpl->traces.end(),
                          [neuron](const Impl::MemoryTrace& t) { return t.neuron == neuron; });
    
    if (it != pImpl->traces.end()) {
        // Update existing trace with activity-dependent learning
        float oldActivation = it->activation;
        it->activation = std::clamp(value, 0.0f, 1.0f);
        
        // Apply Hebbian-like learning
        if (it->activation > pImpl->LTP_threshold && oldActivation < pImpl->LTP_threshold) {
            // Long-term potentiation
            it->strength = std::min(1.0f, it->strength + 0.1f * value);
        } else if (it->activation < pImpl->LTD_threshold && oldActivation > pImpl->LTD_threshold) {
            // Long-term depression
            it->strength = std::max(0.0f, it->strength - 0.1f * (1.0f - value));
        }
        
        // Update trace age and decay accumulator
        it->age = 0.0f;
        it->lastUpdate = currentStep;
        it->decayAccumulator = 0.0f;
    } else {
        // Create new trace if within capacity
        if (pImpl->traces.size() < pImpl->capacity) {
            Impl::MemoryTrace newTrace(neuron, value);
            newTrace.strength = value;
            newTrace.lastUpdate = currentStep;
            pImpl->traces.push_back(newTrace);
            
            // Trigger competitive learning
            runCompetition();
        } else {
            // Apply competitive eviction - remove weakest trace
            std::vector<size_t> tracesAboveThreshold;
            float minStrength = 1.0f;
            
            for (size_t i = 0; i < pImpl->traces.size(); ++i) {
                if (pImpl->traces[i].strength > pImpl->globalInhibition) {
                    tracesAboveThreshold.push_back(i);
                    if (pImpl->traces[i].strength < minStrength) {
                        minStrength = pImpl->traces[i].strength;
                    }
                }
            }
            
            if (!tracesAboveThreshold.empty()) {
                // Evict the weakest trace
                size_t weakestIdx = 0;
                for (size_t i : tracesAboveThreshold) {
                    if (pImpl->traces[i].strength < pImpl->traces[weakestIdx].strength) {
                        weakestIdx = i;
                    }
                }
                pImpl->traces.erase(pImpl->traces.begin() + weakestIdx);
                
                // Add new trace
                Impl::MemoryTrace newTrace(neuron, value);
                newTrace.strength = value;
                newTrace.lastUpdate = currentStep;
                pImpl->traces.push_back(newTrace);
                
                // Run competition again
                runCompetition();
            }
        }
    }
}

void WorkingMemory::runCompetition() {
    // Implement competitive neural inhibition for memory trace selection
    if (pImpl->traces.empty()) return;
    
    // Compute competitive rankings based on strength and activity
    std::vector<float> ranks(pImpl->traces.size(), 0.0f);
    float totalStrength = 0.0f;
    
    for (size_t i = 0; i < pImpl->traces.size(); ++i) {
        totalStrength += pImpl->traces[i].strength;
    }
    
    if (totalStrength > 0.0f) {
        for (size_t i = 0; i < pImpl->traces.size(); ++i) {
            ranks[i] = pImpl->traces[i].strength / totalStrength;
        }
    }
    
    // Apply lateral inhibition
    for (size_t i = 0; i < pImpl->traces.size(); ++i) {
        float sumBelow = 0.0f;
        float sumAbove = 0.0f;
        
        for (size_t j = 0; j < pImpl->traces.size(); ++j) {
            if (i != j) {
                if (ranks[j] < ranks[i]) {
                    sumBelow += ranks[j];
                } else {
                    sumAbove += ranks[j];
                }
            }
        }
        
        // Apply competition - neurons below threshold get inhibited
        if (sumBelow > pImpl->globalInhibition) {
            pImpl->traces[i].competitiveRank *= 0.5f;
        } else if (sumAbove > pImpl->globalInhibition) {
            // Strengthen based on being above threshold
            pImpl->traces[i].competitiveRank = std::min(1.0f, pImpl->traces[i].competitiveRank * 1.2f);
        }
    }
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    for (const auto& trace : pImpl->traces) {
        if (trace.neuron == neuron) {
            return trace.activation;
        }
    }
    return 0.0f;
}

bool WorkingMemory::contains(NeuronId neuron) const {
    for (const auto& trace : pImpl->traces) {
        if (trace.neuron == neuron) {
            return true;
        }
    }
    return false;
}

void WorkingMemory::clear() {
    pImpl->traces.clear();
}

size_t WorkingMemory::getCapacity() const {
    return pImpl->capacity;
}

size_t WorkingMemory::getCurrentSize() const {
    return pImpl->traces.size();
}

void WorkingMemory::decay(float decayRate) {
    for (auto& trace : pImpl->traces) {
        trace.activation *= (1.0f - decayRate);
        trace.strength *= (1.0f - decayRate * 0.1f); // Slower strength decay
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
    // Implement real consolidation with neural mechanisms
    std::vector<size_t> toConsolidate;
    std::vector<size_t> toDelete;
    
    // Select episodes for consolidation based on multiple factors
    for (size_t i = 0; i < pImpl->episodes.size(); ++i) {
        const auto& episode = pImpl->episodes[i];
        
        // Calculate relevance score based on:
        // 1. Recentness (older episodes decay)
        // 2. Reward value
        // 3. Novelty
        // 4. Recency of access
        
        float recencyScore = 1.0f / (episode.age + 1.0f);
        float rewardScore = std::min(1.0f, episode.reward * 2.0f);
        float noveltyScore = episode.novelty;
        
        float relevance = (recencyScore * 0.3f) + (rewardScore * 0.4f) + (noveltyScore * 0.3f);
        
        if (relevance >= relevanceThreshold) {
            toConsolidate.push_back(i);
        } else if (relevance < 0.1f) {
            // Very low relevance - mark for deletion
            toDelete.push_back(i);
        }
    }
    
    // Process consolidation
    for (size_t i : toDelete) {
        if (i < pImpl->episodes.size()) {
            pImpl->episodes.erase(pImpl->episodes.begin() + i);
        }
    }
    
    // In a real implementation, consolidate would move episodes to semantic memory
    // and create synaptic connections representing the experience
    // For now, we just keep the high-relevance episodes
    // TODO: Integrate with semantic memory system
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
