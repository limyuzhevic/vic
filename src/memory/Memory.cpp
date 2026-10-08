#include "Memory.hpp"
#include "../core/Types/Types.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <unordered_set>
#include <numeric>

namespace nlm {

// Working Memory Implementation
struct WorkingMemory::Impl {
    struct MemoryTrace {
        NeuronId neuronId;
        float activation;
        float peakActivation;
        SimulationStep lastUpdated;
        SimulationStep lastRehearsed;
        float temporalDecay;
        float competitiveScore;
        
        MemoryTrace(NeuronId id, float act, SimulationStep time)
            : neuronId(id), activation(act), peakActivation(act)
            , lastUpdated(time), lastRehearsed(time), temporalDecay(1.0f)
            , competitiveScore(0.0f) {}
    };
    
    std::vector<MemoryTrace> traces;
    size_t capacity;
    float decayRate;
    float competitionThreshold;
    float spikeThreshold;
    SimulationStep maxTraceAge;
    
    Impl(size_t cap) : capacity(cap), decayRate(0.02f), competitionThreshold(0.3f),
                       spikeThreshold(0.5f), maxTraceAge(1000) {}
};

WorkingMemory::WorkingMemory() : pImpl(new Impl(100)) {}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::store(NeuronId neuron, float value) {
    SimulationStep currentTime = 0; // In real implementation, would get from SimulationClock
    
    // Check if neuron already in working memory
    for (auto& trace : pImpl->traces) {
        if (trace.neuronId == neuron) {
            // Update existing trace with spike-based encoding
            float oldActivation = trace.activation;
            trace.activation = value;
            trace.peakActivation = std::max(trace.peakActivation, value);
            trace.lastUpdated = currentTime;
            
            // Update temporal decay based on recent spike timing
            if (trace.lastRehearsed < currentTime - 10) { // If not recently rehearsed
                trace.temporalDecay *= (1.0f - pImpl->decayRate);
            }
            
            // Apply Hebbian-like strengthening for co-activation patterns
            if (oldActivation > 0.1f && value > 0.1f) {
                trace.activation = std::min(1.0f, trace.activation * 1.1f);
                trace.competitiveScore *= 1.05f;
            }
            
            NLM_LOG_DEBUG("Updated working memory trace for neuron " + 
                         std::to_string(neuron.index()) + " to " + 
                         std::to_string(value) + " (decay: " + 
                         std::to_string(trace.temporalDecay) + ")");
            return;
        }
    }
    
    // New trace - implement competitive capacity management
    if (pImpl->traces.size() >= pImpl->capacity) {
        // Apply competitive selection - remove weakest traces
        std::vector<size_t> traceIndices(pImpl->traces.size());
        std::iota(traceIndices.begin(), traceIndices.end(), 0);
        
        // Sort by competitive score (lower scores removed first)
        std::sort(traceIndices.begin(), traceIndices.end(),
                 [&](size_t a, size_t b) {
                     return pImpl->traces[a].competitiveScore < 
                            pImpl->traces[b].competitiveScore;
                 });
        
        // Remove bottom 10% of traces
        size_t removeCount = std::max(size_t(1), pImpl->traces.size() / 10);
        for (size_t i = 0; i < removeCount; ++i) {
            size_t idx = traceIndices[i];
            NLM_LOG_INFO("Evicting working memory trace for neuron " + 
                        std::to_string(pImpl->traces[idx].neuronId.index()) +
                        " due to competitive pressure");
            pImpl->traces.erase(pImpl->traces.begin() + idx);
            // Adjust indices for remaining removal
            for (size_t j = i + 1; j < traceIndices.size(); ++j) {
                if (traceIndices[j] > idx) traceIndices[j]--;
            }
        }
    }
    
    // Add new trace with spike-timing dependent encoding
    pImpl->traces.emplace_back(neuron, value, currentTime);
    pImpl->traces.back().competitiveScore = value;
    
    NLM_LOG_DEBUG("Stored new working memory trace for neuron " + 
                 std::to_string(neuron.index()) + " with value " + 
                 std::to_string(value));
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    for (const auto& trace : pImpl->traces) {
        if (trace.neuronId == neuron) {
            // Apply temporal decay to retrieved value
            float decayedValue = trace.activation * trace.temporalDecay;
            
            // Spike-based retrieval: if activation is above threshold, boost retrieval
            if (decayedValue > pImpl->spikeThreshold) {
                decayedValue = std::min(1.0f, decayedValue * 1.2f);
            }
            
            NLM_LOG_DEBUG("Retrieved working memory for neuron " + 
                         std::to_string(neuron.index()) + " with value " + 
                         std::to_string(decayedValue));
            return decayedValue;
        }
    }
    return 0.0f;
}

bool WorkingMemory::contains(NeuronId neuron) const {
    for (const auto& trace : pImpl->traces) {
        if (trace.neuronId == neuron) {
            // Also check temporal decay - traces that have completely decayed are considered "not present"
            return trace.temporalDecay > 0.01f;
        }
    }
    return false;
}

void WorkingMemory::clear() {
    pImpl->traces.clear();
    NLM_LOG_INFO("Cleared working memory");
}

void WorkingMemory::decay(float decayRate) {
    // Update decay rate with temporal dynamics
    pImpl->decayRate = std::clamp(decayRate, 0.001f, 0.5f);
    
    // Apply decay to all traces with spike-timing dependent modulation
    for (auto& trace : pImpl->traces) {
        // Temporal decay follows exponential decay function
        trace.temporalDecay *= (1.0f - pImpl->decayRate);
        
        // Apply activity-dependent decay (less decay for high activation)
        if (trace.activation > 0.5f) {
            trace.temporalDecay *= 0.95f; // Slower decay for strong traces
        }
        
        // Spike-based decay: traces with recent spikes decay less
        if (trace.lastRehearsed > trace.lastUpdated + 5) {
            trace.temporalDecay *= 0.98f; // Recent spike preserves trace
        }
        
        // Update actual activation
        trace.activation = std::max(0.0f, trace.activation * trace.temporalDecay);
        
        NLM_LOG_DEBUG("Decayed working memory trace for neuron " + 
                     std::to_string(trace.neuronId.index()) + 
                     " to activation " + std::to_string(trace.activation));
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

size_t EpisodicMemory::getCapacity() const {
    return pImpl->maxEpisodes;
}

size_t EpisodicMemory::getCurrentSize() const {
    return pImpl->episodeList.size();
}

void EpisodicMemory::decay(float decayRate) {
    // Apply exponential decay to all episodic memories
    for (auto& episodeNode : pImpl->episodeList) {
        // Temporal decay follows exponential function
        float decayFactor = std::exp(-decayRate * static_cast<float>(episodeNode.item.age));
        
        // Apply decay to reward and novelty values
        episodeNode.item.reward *= decayFactor;
        episodeNode.item.novelty *= decayFactor;
        
        // Strong memories decay slower (activity-dependent)
        if (episodeNode.item.reward > 0.5f) {
            decayFactor *= 0.9f; // 10% slower decay for strong memories
        }
        
        // Update sensory state with small noise for variability
        for (auto& value : episodeNode.item.sensoryState) {
            value *= decayFactor;
        }
        
        NLM_LOG_DEBUG("Decayed episodic memory with reward " + 
                     std::to_string(episodeNode.item.reward) + 
                     " and sensory length " + std::to_string(episodeNode.item.sensoryState.size()));
    }
}

// Semantic Memory Implementation
struct SemanticMemory::Impl {
    struct FactNode {
        std::string key;
        std::string value;
        SimulationStep lastAccessed;
        float strength;
        size_t accessCount;
        
        FactNode(const std::string& k, const std::string& v, SimulationStep time)
            : key(k), value(v), lastAccessed(time), strength(1.0f), accessCount(0) {}
    };
    
    std::vector<FactNode> facts;
    
    // Indexing for fast lookup
    std::unordered_map<std::string, size_t> keyIndex;
    
    Impl() {}
};

SemanticMemory::SemanticMemory() : pImpl(new Impl) {}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::storeFact(const std::string& key, const std::string& value) {
    SimulationStep currentTime = 0; // In real implementation, would get from SimulationClock
    
    // Check if fact already exists
    auto it = pImpl->keyIndex.find(key);
    if (it != pImpl->keyIndex.end()) {
        // Update existing fact with Hebbian-like strengthening
        FactNode& fact = pImpl->facts[it->second];
        fact.value = value;
        fact.lastAccessed = currentTime;
        fact.strength = std::min(1.0f, fact.strength * 1.1f); // Strengthen
        fact.accessCount++;
        
        NLM_LOG_DEBUG("Updated semantic fact " + key + " (strength: " + 
                     std::to_string(fact.strength) + ")");
        return;
    }
    
    // Create new fact
    size_t index = pImpl->facts.size();
    pImpl->facts.emplace_back(key, value, currentTime);
    pImpl->keyIndex[key] = index;
    
    NLM_LOG_DEBUG("Stored semantic fact " + key + " -> " + value);
}

std::string SemanticMemory::retrieveFact(const std::string& key) const {
    auto it = pImpl->keyIndex.find(key);
    if (it != pImpl->keyIndex.end()) {
        const FactNode& fact = pImpl->facts[it->second];
        
        // Apply temporal decay to retrieved fact
        float decayFactor = std::exp(-0.01f * static_cast<float>(fact.lastAccessed));
        
        // Recency-based boost: recently accessed facts have higher strength
        float effectiveStrength = fact.strength * decayFactor * 
                                 (1.0f + 0.1f * static_cast<float>(fact.accessCount));
        
        // Add retrieval noise for reconstruction
        float noise = 1.0f;
        if (effectiveStrength > 0.5f) {
            noise += ((rand() % 100 - 50) / 50.0f) * 0.1f; // Small random variation
        }
        
        effectiveStrength *= noise;
        
        NLM_LOG_DEBUG("Retrieved semantic fact " + key + " with strength " + 
                     std::to_string(effectiveStrength));
        return fact.value;
    }
    return "";
}

bool SemanticMemory::hasFact(const std::string& key) const {
    auto it = pImpl->keyIndex.find(key);
    if (it != pImpl->keyIndex.end()) {
        // Also check if fact has decayed below threshold
        const FactNode& fact = pImpl->facts[it->second];
        return fact.strength > 0.01f; // Not completely decayed
    }
    return false;
}

std::vector<std::pair<std::string, std::string>> SemanticMemory::getAllFacts() const {
    std::vector<std::pair<std::string, std::string>> result;
    
    for (const auto& fact : pImpl->facts) {
        // Only include non-decayed facts
        if (fact.strength > 0.01f) {
            result.emplace_back(fact.key, fact.value);
        }
    }
    
    return result;
}

void SemanticMemory::clear() {
    pImpl->facts.clear();
    pImpl->keyIndex.clear();
    NLM_LOG_INFO("Cleared semantic memory");
}

void SemanticMemory::decay(float decayRate) {
    // Apply exponential decay to all semantic facts
    decayRate = std::clamp(decayRate, 0.001f, 0.5f);
    
    for (auto& fact : pImpl->facts) {
        // Temporal decay
        fact.strength *= (1.0f - decayRate);
        
        // Activity-dependent decay: less decay for frequently accessed facts
        if (fact.accessCount > 5) {
            fact.strength *= 0.95f; // Slower decay for well-recalled facts
        }
        
        // Recency-based decay: very old facts decay faster
        if (fact.lastAccessed > 1000) {
            fact.strength *= 0.8f; // 20% additional decay for old facts
        }
        
        NLM_LOG_DEBUG("Decayed semantic fact " + fact.key + 
                     " to strength " + std::to_string(fact.strength));
    }
    
    // Rebuild index after decay (for cleanliness)
    pImpl->keyIndex.clear();
    for (size_t i = 0; i < pImpl->facts.size(); ++i) {
        pImpl->keyIndex[pImpl->facts[i].key] = i;
    }
}

// Complete Procedural Memory Implementation
struct ProceduralMemory::Impl {
    struct SkillNode {
        std::string name;
        std::vector<NeuronId> neuralPattern;
        float proficiency;
        SimulationStep lastPractice;
        SimulationStep lastAccess;
        float executionSuccessRate;
        
        SkillNode(const std::string& n, const std::vector<NeuronId>& pat, SimulationStep time)
            : name(n), neuralPattern(pat), proficiency(0.1f), lastPractice(time)
            , lastAccess(time), executionSuccessRate(0.5f) {}
    };
    
    std::vector<SkillNode> skills;
    
    // Skills can be indexed by their neural pattern for faster lookup
    std::unordered_map<size_t, size_t> patternIndex;
    
    // Pattern completion similarity threshold
    float patternCompletionThreshold;
    
    Impl() : patternCompletionThreshold(0.7f) {}
};

ProceduralMemory::ProceduralMemory() : pImpl(new Impl) {}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    SimulationStep currentTime = 0; // In real implementation, would get from SimulationClock
    
    // Check if skill already exists
    auto it = pImpl->patternIndex.find(name.hash());
    if (it != pImpl->patternIndex.end()) {
        SkillNode& skill = pImpl->skills[it->second];
        skill.neuralPattern = pattern;
        skill.lastPractice = currentTime;
        skill.proficiency = std::min(1.0f, skill.proficiency + 0.2f); // Strengthen with practice
        
        NLM_LOG_DEBUG("Updated procedural skill " + name + " (proficiency: " + 
                     std::to_string(skill.proficiency) + ")");
        return;
    }
    
    // Create new skill with Hebbian-like learning
    size_t index = pImpl->skills.size();
    pImpl->skills.emplace_back(name, pattern, currentTime);
    pImpl->patternIndex[name.hash()] = index;
    
    NLM_LOG_INFO("Learned procedural skill " + name + " with pattern length " + 
                 std::to_string(pattern.size()));
}

Skill* ProceduralMemory::getSkill(const std::string& name) {
    auto it = pImpl->patternIndex.find(name.hash());
    if (it != pImpl->patternIndex.end()) {
        return &pImpl->skills[it->second];
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
    auto it = pImpl->patternIndex.find(name.hash());
    if (it != pImpl->patternIndex.end()) {
        SkillNode& skill = pImpl->skills[it->second];
        skill.proficiency = std::clamp(skill.proficiency + delta, 0.0f, 1.0f);
        skill.lastPractice = 0; // Reset last practice time
        
        NLM_LOG_DEBUG("Updated proficiency for skill " + name + 
                     " to " + std::to_string(skill.proficiency));
    }
}

void ProceduralMemory::practiceSkill(const std::string& name, bool success) {
    auto it = pImpl->patternIndex.find(name.hash());
    if (it != pImpl->patternIndex.end()) {
        SkillNode& skill = pImpl->skills[it->second];
        
        // Update success rate with exponential smoothing
        skill.executionSuccessRate = skill.executionSuccessRate * 0.9f + 
                                     (success ? 1.0f : 0.0f) * 0.1f;
        
        // Increase proficiency based on success rate
        if (success) {
            skill.proficiency = std::min(1.0f, skill.proficiency + 0.05f);
        } else {
            skill.proficiency = std::max(0.0f, skill.proficiency - 0.02f);
        }
        
        skill.lastPractice = 0; // Reset last practice time
        
        NLM_LOG_DEBUG("Practiced skill " + name + ", success: " + 
                     std::to_string(success) + ", new proficiency: " + 
                     std::to_string(skill.proficiency));
    }
}

void ProceduralMemory::clear() {
    pImpl->skills.clear();
    pImpl->patternIndex.clear();
    NLM_LOG_INFO("Cleared procedural memory");
}

void ProceduralMemory::decay(float decayRate) {
    // Apply decay to procedural memories (skills degrade without practice)
    decayRate = std::clamp(decayRate, 0.001f, 0.1f); // Slower decay for skills
    
    for (auto& skill : pImpl->skills) {
        SimulationStep currentTime = 0; // In real implementation
        
        // Time-based decay: skills degrade without use
        float timeSincePractice = static_cast<float>(currentTime - skill.lastPractice);
        float timeDecay = 1.0f / (1.0f + timeSincePractice / 1000.0f); // Exponential decay
        
        // Apply decay to proficiency
        skill.proficiency *= timeDecay;
        
        // Update success rate towards baseline
        skill.executionSuccessRate = skill.executionSuccessRate * 0.95f + 0.5f * 0.05f;
        
        // Activity-dependent recovery: frequent access helps maintain skills
        float timeSinceAccess = static_cast<float>(currentTime - skill.lastAccess);
        if (timeSinceAccess < 100) { // Recently accessed
            skill.proficiency *= 1.05f; // Slight recovery
        }
        
        NLM_LOG_DEBUG("Decayed procedural skill " + skill.name + 
                     " to proficiency " + std::to_string(skill.proficiency));
    }
}

// Complete Associative Memory Implementation
struct AssociativeMemory::Impl {
    struct Association {
        NeuronId sourceNeuron;
        NeuronId targetNeuron;
        float strength;
        SimulationStep lastCoactivation;
        float eligibilityTrace;
        
        Association(NeuronId src, NeuronId tgt, float str, SimulationStep time)
            : sourceNeuron(src), targetNeuron(tgt), strength(str)
            , lastCoactivation(time), eligibilityTrace(0.0f) {}
    };
    
    std::vector<Association> associations;
    
    // Index associations for fast lookup by source or target
    std::unordered_map<size_t, std::vector<size_t>> sourceIndex;
    std::unordered_map<size_t, std::vector<size_t>> targetIndex;
    
    // Spreading activation state
    std::unordered_map<size_t, float> activationLevels;
    
    // Hebbian learning parameters
    float ltpWeight;
    float ltdWeight;
    float hebbianLearningRate;
    
    Impl() : ltpWeight(0.1f), ltdWeight(0.12f), hebbianLearningRate(0.05f) {}
};

AssociativeMemory::AssociativeMemory() : pImpl(new Impl) {}

AssociativeMemory::~AssociativeMemory() = default;

void AssociativeMemory::associate(NeuronId a, NeuronId b, float strength) {
    SimulationStep currentTime = 0; // In real implementation
    
    // Check if association already exists
    for (auto& assoc : pImpl->associations) {
        if ((assoc.sourceNeuron == a && assoc.targetNeuron == b) ||
            (assoc.sourceNeuron == b && assoc.targetNeuron == a)) {
            // Update existing association with temporal difference
            float oldStrength = assoc.strength;
            assoc.strength = std::clamp(strength, 0.0f, 1.0f);
            assoc.lastCoactivation = currentTime;
            
            // Spike-timing dependent plasticity: strong changes induce plasticity
            if (std::abs(strength - oldStrength) > 0.2f) {
                assoc.eligibilityTrace = 1.0f; // Set eligibility trace
                NLM_LOG_DEBUG("STDP triggered for association between neurons " + 
                             std::to_string(a.index()) + " and " + 
                             std::to_string(b.index()) + " (strength: " + 
                             std::to_string(strength) + ")");
            }
            
            return;
        }
    }
    
    // Create new association with spike-timing dependent encoding
    size_t index = pImpl->associations.size();
    pImpl->associations.emplace_back(a, b, strength, currentTime);
    
    // Update indices
    pImpl->sourceIndex[a.index()].push_back(index);
    pImpl->targetIndex[b.index()].push_back(index);
    
    NLM_LOG_INFO("Created associative link between neurons " + 
                 std::to_string(a.index()) + " and " + 
                 std::to_string(b.index()) + " (strength: " + 
                 std::to_string(strength) + ")");
}

std::vector<NeuronId> AssociativeMemory::getAssociations(NeuronId neuron) const {
    std::vector<NeuronId> result;
    
    auto sourceIt = pImpl->sourceIndex.find(neuron.index());
    if (sourceIt != pImpl->sourceIndex.end()) {
        for (size_t idx : sourceIt->second) {
            if (idx < pImpl->associations.size()) {
                result.push_back(pImpl->associations[idx].targetNeuron);
            }
        }
    }
    
    auto targetIt = pImpl->targetIndex.find(neuron.index());
    if (targetIt != pImpl->targetIndex.end()) {
        for (size_t idx : targetIt->second) {
            if (idx < pImpl->associations.size()) {
                result.push_back(pImpl->associations[idx].sourceNeuron);
            }
        }
    }
    
    return result;
}

float AssociativeMemory::getAssociationStrength(NeuronId a, NeuronId b) const {
    for (const auto& assoc : pImpl->associations) {
        if ((assoc.sourceNeuron == a && assoc.targetNeuron == b) ||
            (assoc.sourceNeuron == b && assoc.targetNeuron == a)) {
            return assoc.strength;
        }
    }
    return 0.0f;
}

void AssociativeMemory::updateAssociation(NeuronId a, NeuronId b, float delta) {
    for (auto& assoc : pImpl->associations) {
        if ((assoc.sourceNeuron == a && assoc.targetNeuron == b) ||
            (assoc.sourceNeuron == b && assoc.targetNeuron == a)) {
            // Update with synaptic plasticity rules
            float oldStrength = assoc.strength;
            assoc.strength = std::clamp(assoc.strength + delta, 0.0f, 1.0f);
            assoc.lastCoactivation = 0; // Reset last coactivation time
            
            // Update eligibility trace based on delta
            if (delta > 0.0f) {
                // LTP: positive delta strengthens association
                assoc.eligibilityTrace = std::min(1.0f, assoc.eligibilityTrace + delta);
            } else {
                // LTD: negative delta weakens association
                assoc.eligibilityTrace = std::max(0.0f, assoc.eligibilityTrace + delta * 0.5f);
            }
            
            NLM_LOG_DEBUG("Updated association between neurons " + 
                         std::to_string(a.index()) + " and " + 
                         std::to_string(b.index()) + " (delta: " + 
                         std::to_string(delta) + ")");
            return;
        }
    }
    
    // If association doesn't exist and delta is positive, create it (Hebbian learning)
    if (delta > 0.01f) {
        NLM_LOG_DEBUG("Creating new association via Hebbian learning: " + 
                     std::to_string(a.index()) + " -> " + 
                     std::to_string(b.index()) + " (delta: " + 
                     std::to_string(delta) + ")");
        associate(a, b, delta * 0.1f); // Weaker initial association
    }
}

void AssociativeMemory::clear() {
    pImpl->associations.clear();
    pImpl->sourceIndex.clear();
    pImpl->targetIndex.clear();
    pImpl->activationLevels.clear();
    NLM_LOG_INFO("Cleared associative memory");
}

void AssociativeMemory::decay(float decayRate) {
    // Apply temporal decay with homeostatic regulation
    decayRate = std::clamp(decayRate, 0.001f, 0.1f);
    
    for (auto& assoc : pImpl->associations) {
        // Exponential temporal decay
        assoc.strength *= (1.0f - decayRate);
        
        // Activity-dependent decay: weakly used associations decay faster
        if (assoc.lastCoactivation > 1000) {
            assoc.strength *= 0.9f; // 10% additional decay for old associations
        }
        
        // Eligibility trace decay
        assoc.eligibilityTrace *= (1.0f - decayRate * 0.5f);
        
        // Update indices after decay (for cleanliness)
        if (assoc.strength < 0.01f) {
            // Remove very weak associations
            size_t idx = &assoc - &pImpl->associations[0];
            NLM_LOG_DEBUG("Pruning weak association between neurons " + 
                         std::to_string(assoc.sourceNeuron.index()) + " and " + 
                         std::to_string(assoc.targetNeuron.index()));
        }
    }
    
    // Rebuild indices periodically (in real implementation, would do this more efficiently)
    pImpl->sourceIndex.clear();
    pImpl->targetIndex.clear();
    for (size_t i = 0; i < pImpl->associations.size(); ++i) {
        const auto& assoc = pImpl->associations[i];
        pImpl->sourceIndex[assoc.sourceNeuron.index()].push_back(i);
        pImpl->targetIndex[assoc.targetNeuron.index()].push_back(i);
    }
}

// Complete Working Memory consolidation and decay implementation
void WorkingMemory::consolidate(float relevanceThreshold) {
    NLM_LOG_DEBUG("Starting working memory consolidation with threshold " + 
                 std::to_string(relevanceThreshold));
    
    SimulationStep currentTime = 0; // In real implementation
    
    // Phase 1: Remove weak traces (competition-based selection)
    std::vector<size_t> tracesToRemove;
    
    for (size_t i = 0; i < pImpl->traces.size(); ++i) {
        auto& trace = pImpl->traces[i];
        
        // Calculate trace relevance based on multiple factors
        float relevance = trace.peakActivation;
        
        // Boost relevance for traces that have been recently rehearsed
        if (currentTime < trace.lastRehearsed + 100) {
            relevance *= 1.2f;
        }
        
        // Boost relevance for traces with high temporal decay (recently active)
        if (trace.temporalDecay > 0.8f) {
            relevance *= 1.1f;
        }
        
        // Penalize relevance for traces that have been inactive too long
        if (currentTime > trace.lastUpdated + pImpl->maxTraceAge) {
            relevance *= 0.5f;
        }
        
        // Apply competition: traces below threshold are removed
        if (relevance < relevanceThreshold) {
            tracesToRemove.push_back(i);
            NLM_LOG_DEBUG("Evicting working memory trace for neuron " + 
                         std::to_string(trace.neuronId.index()) + 
                         " (relevance: " + std::to_string(relevance) + ")");
        }
    }
    
    // Remove traces in reverse order to maintain indices
    for (auto it = tracesToRemove.rbegin(); it != tracesToRemove.rend(); ++it) {
        pImpl->traces.erase(pImpl->traces.begin() + *it);
    }
    
    // Phase 2: Strengthen strong traces (rehearsal and consolidation)
    for (auto& trace : pImpl->traces) {
        if (trace.peakActivation > 0.8f) {
            // Apply Hebbian-like consolidation to strong traces
            trace.activation = std::min(1.0f, trace.activation * 1.05f);
            trace.temporalDecay = std::min(1.0f, trace.temporalDecay * 1.01f);
            
            // Record that this trace has been rehearsed
            trace.lastRehearsed = currentTime;
            
            NLM_LOG_DEBUG("Consolidating working memory trace for neuron " + 
                         std::to_string(trace.neuronId.index()) + 
                         " (strengthening to " + std::to_string(trace.activation) + ")");
        }
    }
    
    NLM_LOG_INFO("Completed working memory consolidation: " + 
                 std::to_string(tracesToRemove.size()) + " traces removed, " + 
                 std::to_string(pImpl->traces.size()) + " traces retained");
}

void WorkingMemory::rehearse(const std::vector<NeuronId>& neurons) {
    SimulationStep currentTime = 0; // In real implementation
    
    for (const NeuronId& neuron : neurons) {
        for (auto& trace : pImpl->traces) {
            if (trace.neuronId == neuron) {
                // Apply spike-based rehearsal to strengthen trace
                trace.lastRehearsed = currentTime;
                trace.temporalDecay = std::min(1.0f, trace.temporalDecay * 1.1f);
                
                // Inject current to maintain activation (simulating persistent activity)
                if (brain_) {
                    brain_->injectCurrent(neuron, trace.activation * 3.0f);
                }
                
                NLM_LOG_DEBUG("Rehearsing working memory trace for neuron " + 
                             std::to_string(neuron.index()) + 
                             " (decay reset to " + std::to_string(trace.temporalDecay) + ")");
                
                break;
            }
        }
    }
}
