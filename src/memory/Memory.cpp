#include "Memory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <set>
#include <numeric>

namespace nlm {

// WorkingMemory Implementation
struct WorkingMemory::Impl {
    struct Trace {
        NeuronId neuronId;
        float activation;
        SimulationStep timestamp;
        SimulationStep age;
        float neuromodulation;
        float predictionError;
    };
    
    std::vector<Trace> traces;
    size_t capacity;
    
    std::vector<NeuronId> winners;
    std::unordered_map<uint64_t, float> neuronInhibition;
    
    Impl(size_t cap) : capacity(cap) {}
};

WorkingMemory::WorkingMemory() : pImpl(new Impl(100)) {}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::store(NeuronId neuron, float value) {
    auto it = std::find_if(pImpl->traces.begin(), pImpl->traces.end(),
                          [neuron](const Impl::Trace& t) { return t.neuronId == neuron; });
    
    if (it != pImpl->traces.end()) {
        it->activation = value;
        it->timestamp = 0;
        it->age = 0;
        if (it->neuromodulation > 1.0f) {
            it->activation *= it->neuromodulation;
        }
    } else {
        Impl::Trace trace;
        trace.neuronId = neuron;
        trace.activation = value;
        trace.timestamp = 0;
        trace.age = 0;
        trace.neuromodulation = 1.0f;
        trace.predictionError = 0.0f;
        
        if (pImpl->traces.size() >= pImpl->capacity) {
            size_t weakestIdx = 0;
            float weakestScore = std::numeric_limits<float>::max();
            
            for (size_t i = 0; i < pImpl->traces.size(); ++i) {
                float activationAgeFactor = pImpl->traces[i].activation * (1.0f / (pImpl->traces[i].age + 1.0f));
                float predictionErrorFactor = 1.0f - std::abs(pImpl->traces[i].predictionError);
                float score = activationAgeFactor * predictionErrorFactor;
                if (score < weakestScore) {
                    weakestScore = score;
                    weakestIdx = i;
                }
            }
            pImpl->traces.erase(pImpl->traces.begin() + weakestIdx);
            pImpl->neuronInhibition.erase(pImpl->traces[weakestIdx].neuronId.value);
        }
        
        pImpl->traces.push_back(trace);
    }
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    auto it = std::find_if(pImpl->traces.begin(), pImpl->traces.end(),
                          [neuron](const Impl::Trace& t) { return t.neuronId == neuron; });
    
    if (it != pImpl->traces.end()) {
        return it->activation;
    }
    return 0.0f;
}

bool WorkingMemory::contains(NeuronId neuron) const {
    return std::any_of(pImpl->traces.begin(), pImpl->traces.end(),
                      [neuron](const Impl::Trace& t) { return t.neuronId == neuron; });
}

void WorkingMemory::clear() {
    pImpl->traces.clear();
    pImpl->winners.clear();
    pImpl->neuronInhibition.clear();
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
        trace.age++;
        
        if (trace.neuromodulation > 1.0f) {
            trace.activation *= trace.neuromodulation;
        }
    }
}

void WorkingMemory::applyNeuromodulation(float level) {
    for (auto& trace : pImpl->traces) {
        trace.neuromodulation = 1.0f + level * 0.5f;
    }
}

const std::vector<NeuronId>& WorkingMemory::getWinners() const {
    return pImpl->winners;
}

float WorkingMemory::getInhibitionFor(NeuronId neuron) const {
    auto it = pImpl->neuronInhibition.find(neuron.value);
    if (it != pImpl->neuronInhibition.end()) {
        return it->second;
    }
    return 0.0f;
}

void WorkingMemory::rehearse() {
    // Reinforce all traces with exponential decay
    for (auto& trace : pImpl->traces) {
        trace.activation *= 1.1f;
        trace.activation = std::min(trace.activation, 1.0f);
    }
}

// EpisodicMemory Implementation
struct EpisodicMemory::Impl {
    std::vector<EpisodicMemoryItem> episodes;
    size_t maxEpisodes;
    
    Impl(size_t max) : maxEpisodes(max) {}
};

EpisodicMemory::EpisodicMemory() : pImpl(new Impl(1000)) {}

EpisodicMemory::~EpisodicMemory() = default;

void EpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    // Check if this episode is very similar to recent ones
    bool isDuplicate = false;
    for (const auto& existing : pImpl->episodes) {
        if (isEpisodeSimilar(episode, existing)) {
            // Replace existing episode with updated one
            for (auto it = pImpl->episodes.begin(); it != pImpl->episodes.end(); ++it) {
                if (it->timestamp == existing.timestamp) {
                    *it = episode;
                    return;
                }
            }
            isDuplicate = true;
            break;
        }
    }
    
    if (!isDuplicate) {
        if (pImpl->episodes.size() >= pImpl->maxEpisodes) {
            // Remove lowest priority episode
            size_t worstIdx = 0;
            float worstPriority = std::numeric_limits<float>::max();
            for (size_t i = 0; i < pImpl->episodes.size(); ++i) {
                if (pImpl->episodes[i].priority < worstPriority) {
                    worstPriority = pImpl->episodes[i].priority;
                    worstIdx = i;
                }
            }
            pImpl->episodes.erase(pImpl->episodes.begin() + worstIdx);
        }
        
        EpisodicMemoryItem stored = episode;
        stored.age = 0;
        stored.priority = getEpisodePriority(stored);
        
        pImpl->episodes.push_back(stored);
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
    if (count == 0) return result;
    
    size_t startIdx = count > pImpl->episodes.size() ? 0 : pImpl->episodes.size() - count;
    for (size_t i = startIdx; i < pImpl->episodes.size(); ++i) {
        result.push_back(pImpl->episodes[i]);
    }
    
    return result;
}

void EpisodicMemory::clear() {
    pImpl->episodes.clear();
}

void EpisodicMemory::consolidate(float relevanceThreshold) {
    // Find episodes with high relevance for consolidation
    std::vector<size_t> episodesToKeep;
    
    for (size_t i = 0; i < pImpl->episodes.size(); ++i) {
        if (getEpisodePriority(pImpl->episodes[i]) > relevanceThreshold) {
            episodesToKeep.push_back(i);
        }
    }
    
    // Keep only high-priority episodes
    if (!episodesToKeep.empty()) {
        std::vector<EpisodicMemoryItem> consolidated;
        consolidated.reserve(episodesToKeep.size());
        
        for (size_t idx : episodesToKeep) {
            consolidated.push_back(pImpl->episodes[idx]);
        }
        
        pImpl->episodes = consolidated;
    } else {
        pImpl->episodes.clear();
    }
}

float EpisodicMemory::getEpisodePriority(const EpisodicMemoryItem& episode) const {
    float priority = 0.0f;
    
    // Higher priority for more recent episodes
    priority += (1.0f - static_cast<float>(episode.age) / 1000.0f) * 0.3f;
    
    // Higher priority for episodes with many active neurons
    priority += episode.activeNeurons.size() * 0.1f;
    
    // Higher priority for episodes with high activation values
    float totalActivation = 0.0f;
    for (float val : episode.neuronActivations) {
        totalActivation += val;
    }
    priority += totalActivation * 0.4f;
    
    // Higher priority for episodes with higher rewards
    priority += episode.reward * 0.2f;
    
    return std::min(priority, 1.0f);
}

bool EpisodicMemory::isEpisodeSimilar(const EpisodicMemoryItem& episode1, const EpisodicMemoryItem& episode2) const {
    if (episode1.timestamp == episode2.timestamp) {
        return true;
    }
    
    // Check if episodes have similar active neurons
    std::unordered_map<uint64_t, int> neuronCounts;
    for (const auto& neuronId : episode1.activeNeurons) {
        neuronCounts[neuronId.value]++;
    }
    
    for (const auto& neuronId : episode2.activeNeurons) {
        auto it = neuronCounts.find(neuronId.value);
        if (it != neuronCounts.end()) {
            it->second--;
            if (it->second < 0) {
                return false;
            }
        } else {
            return false;
        }
    }
    
    // Check if episodes have similar activation patterns
    if (episode1.neuronActivations.size() != episode2.neuronActivations.size()) {
        return false;
    }
    
    for (size_t i = 0; i < episode1.neuronActivations.size(); ++i) {
        if (std::abs(episode1.neuronActivations[i] - episode2.neuronActivations[i]) > 0.1f) {
            return false;
        }
    }
    
    return true;
}

void EpisodicMemory::update(float timestep) {
    // Increment age for all episodes
    for (auto& episode : pImpl->episodes) {
        episode.age++;
        episode.timestamp += static_cast<SimulationStep>(timestep * 1000);
        episode.priority = getEpisodePriority(episode);
    }
}

const EpisodicMemoryItem* EpisodicMemory::getEpisodeForReplay(size_t count) const {
    if (pImpl->episodes.empty()) {
        return nullptr;
    }
    
    // Sort by priority (highest priority first for replay)
    std::vector<const EpisodicMemoryItem*> candidates;
    for (const auto& episode : pImpl->episodes) {
        candidates.push_back(&episode);
    }
    
    std::sort(candidates.begin(), candidates.end(),
        [this](const EpisodicMemoryItem* a, const EpisodicMemoryItem* b) {
            return a->priority > b->priority;
        });
    
    if (count == 0 || count > candidates.size()) {
        count = candidates.size();
    }
    
    // Return episodes in priority order - need to be careful with static allocation
    static std::vector<EpisodicMemoryItem> temp;
    temp.clear();
    for (size_t i = 0; i < count; ++i) {
        temp.push_back(*candidates[i]);
    }
    
    return temp.empty() ? nullptr : &temp[0];
}

// Semantic Memory Implementation
struct SemanticMemory::Impl {
    std::unordered_map<std::string, std::tuple<std::string, float, float>> facts;
    std::unordered_map<std::string, float> accessTimes;
};

SemanticMemory::SemanticMemory() : pImpl(new Impl) {}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::storeFact(const std::string& key, const std::string& value, float confidence, float accessTime) {
    auto it = pImpl->facts.find(key);
    if (it != pImpl->facts.end()) {
        std::get<0>(it->second) = value;
        std::get<1>(it->second) = confidence;
    } else {
        pImpl->facts[key] = std::make_tuple(value, confidence, accessTime);
    }
    pImpl->accessTimes[key] = accessTime;
}

std::string SemanticMemory::retrieveFact(const std::string& key) const {
    auto it = pImpl->facts.find(key);
    if (it != pImpl->facts.end()) {
        return std::get<0>(it->second);
    }
    return "";
}

bool SemanticMemory::hasFact(const std::string& key) const {
    return pImpl->facts.find(key) != pImpl->facts.end();
}

std::vector<std::tuple<std::string, std::string, float>> SemanticMemory::getAllFacts() const {
    std::vector<std::tuple<std::string, std::string, float>> result;
    result.reserve(pImpl->facts.size());
    for (const auto& pair : pImpl->facts) {
        result.emplace_back(pair.first, std::get<0>(pair.second), std::get<1>(pair.second));
    }
    return result;
}

float SemanticMemory::getFactStrength(const std::string& key) const {
    auto it = pImpl->facts.find(key);
    if (it != pImpl->facts.end()) {
        return std::get<1>(it->second);
    }
    return 0.0f;
}

void SemanticMemory::decayUnusedFacts(float decayRate, float currentTime) {
    for (auto it = pImpl->facts.begin(); it != pImpl->facts.end(); ) {
        float timeSinceAccess = currentTime - pImpl->accessTimes[it->first];
        if (timeSinceAccess > 0.0f) {
            std::get<1>(it->second) *= std::pow(1.0f - decayRate, timeSinceAccess);
        }
        it++;
    }
}

void SemanticMemory::clear() {
    pImpl->facts.clear();
    pImpl->accessTimes.clear();
}

// ProceduralMemory Implementation
struct ProceduralMemory::Impl {
    std::vector<Skill> skills;
    std::unordered_map<std::string, size_t> skillEffectiveness;
};

ProceduralMemory::ProceduralMemory() : pImpl(new Impl) {}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern, float initialProficiency) {
    auto it = pImpl->skillEffectiveness.find(name);
    if (it != pImpl->skillEffectiveness.end()) {
        it->second = initialProficiency;
    } else {
        Skill skill;
        skill.name = name;
        skill.neuralPattern = pattern;
        skill.proficiency = initialProficiency;
        skill.lastUsed = 0.0f;
        skill.successCount = 0;
        pImpl->skills.push_back(skill);
        pImpl->skillEffectiveness[name] = initialProficiency;
    }
}

Skill* ProceduralMemory::getSkill(const std::string& name) {
    auto it = pImpl->skillEffectiveness.find(name);
    if (it != pImpl->skillEffectiveness.end()) {
        size_t index = 0;
        for (size_t i = 0; i < pImpl->skills.size(); ++i) {
            if (pImpl->skills[i].name == name) {
                index = i;
                break;
            }
        }
        return &pImpl->skills[index];
    }
    return nullptr;
}

std::vector<Skill*> ProceduralMemory::getAllSkills() {
    std::vector<Skill*> result;
    result.reserve(pImpl->skills.size());
    for (auto& skill : pImpl->skills) {
        result.push_back(&skill);
    }
    return result;
}

void ProceduralMemory::updateProficiency(const std::string& name, float delta, bool success) {
    auto it = pImpl->skillEffectiveness.find(name);
    if (it != pImpl->skillEffectiveness.end()) {
        float proficiency = it->second;
        if (success) {
            proficiency += delta * 0.5f;
        } else {
            proficiency += delta * 0.1f;
        }
        it->second = std::clamp(proficiency, 0.0f, 1.0f);
        
        // Find the skill to update
        for (auto& skill : pImpl->skills) {
            if (skill.name == name) {
                skill.proficiency = it->second;
                if (success) {
                    skill.successCount++;
                    skill.lastUsed = 0.0f;
                }
                break;
            }
        }
    }
}

float ProceduralMemory::getSkillEffectiveness(const std::string& name) const {
    auto it = pImpl->skillEffectiveness.find(name);
    if (it != pImpl->skillEffectiveness.end()) {
        return it->second;
    }
    return 0.0f;
}

std::string ProceduralMemory::selectBestSkill(const std::vector<float>& neuralPattern) const {
    std::string bestSkill = "";
    float bestMatch = 0.0f;
    
    for (const auto& skill : pImpl->skills) {
        float matchScore = computeNeuralPatternSimilarity(neuralPattern, skill.neuralPattern);
        if (matchScore > bestMatch) {
            bestMatch = matchScore;
            bestSkill = skill.name;
        }
    }
    
    return bestSkill;
}

void ProceduralMemory::clear() {
    pImpl->skills.clear();
    pImpl->skillEffectiveness.clear();
}

float ProceduralMemory::computeNeuralPatternSimilarity(const std::vector<float>& pattern1, const std::vector<NeuronId>& pattern2) const {
    if (pattern1.empty() || pattern2.empty()) return 0.0f;
    
    float dotProduct = 0.0f;
    float norm1 = 0.0f;
    float norm2 = 0.0f;
    
    for (size_t i = 0; i < pattern1.size(); ++i) {
        dotProduct += pattern1[i];
        norm1 += pattern1[i] * pattern1[i];
    }
    
    for (const auto& neuronId : pattern2) {
        float val = neuronId.value * 0.1f;
        dotProduct += val;
        norm2 += val * val;
    }
    
    if (norm1 > 0.0f && norm2 > 0.0f) {
        return dotProduct / (std::sqrt(norm1) * std::sqrt(norm2));
    }
    return 0.0f;
}

// AssociativeMemory Implementation
struct AssociativeMemory::Impl {
    std::vector<std::tuple<NeuronId, NeuronId, float, SimulationStep>> associations;
    std::unordered_map<NeuronId, std::vector<size_t>> neuronToAssocIndices;
};

AssociativeMemory::AssociativeMemory() : pImpl(new Impl) {}

AssociativeMemory::~AssociativeMemory() = default;

void AssociativeMemory::associate(NeuronId a, NeuronId b, float strength) {
    // Check if association exists
    for (size_t i = 0; i < pImpl->associations.size(); ++i) {
        auto& assoc = pImpl->associations[i];
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            // Update existing association with Hebbian-like strengthening
            float newStrength = std::get<2>(assoc) + strength * 0.5f;
            std::get<2>(assoc) = std::clamp(newStrength, 0.0f, 1.0f);
            std::get<3>(assoc) = 0; // Reset age
            
            // Update reverse lookup
            pImpl->neuronToAssocIndices[a].push_back(i);
            pImpl->neuronToAssocIndices[b].push_back(i);
            
            return;
        }
    }
    
    // Create new association
    size_t index = pImpl->associations.size();
    auto assoc = std::make_tuple(a, b, strength, 0);
    pImpl->associations.push_back(assoc);
    
    // Update reverse lookup
    pImpl->neuronToAssocIndices[a].push_back(index);
    pImpl->neuronToAssocIndices[b].push_back(index);
}

std::vector<NeuronId> AssociativeMemory::getAssociations(NeuronId neuron, float minStrength) const {
    std::vector<NeuronId> result;
    
    auto it = pImpl->neuronToAssocIndices.find(neuron);
    if (it == pImpl->neuronToAssocIndices.end()) {
        return result;
    }
    
    for (size_t index : it->second) {
        if (index < pImpl->associations.size()) {
            auto& assoc = pImpl->associations[index];
            
            // Check if association strength is above threshold
            if (std::get<2>(assoc) >= minStrength) {
                NeuronId partner = (std::get<0>(assoc) == neuron) ? 
                    std::get<1>(assoc) : std::get<0>(assoc);
                result.push_back(partner);
            }
        }
    }
    
    return result;
}

float AssociativeMemory::getAssociationStrength(NeuronId a, NeuronId b) const {
    for (const auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            float strength = std::get<2>(assoc);
            float age = std::get<3>(assoc);
            return strength * (1.0f - age * 0.001f);
        }
    }
    return 0.0f;
}

void AssociativeMemory::updateAssociation(NeuronId a, NeuronId b, float delta) {
    for (auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + delta, 0.0f, 1.0f);
            std::get<3>(assoc) = 0;
            return;
        }
    }
}

std::vector<NeuronId> AssociativeMemory::retrievePattern(const std::vector<NeuronId>& partialPattern) const {
    std::vector<NeuronId> result = partialPattern; // Start with partial input
    
    if (result.empty()) {
        return result;
    }
    
    // Use partial pattern to find associated neurons
    for (const NeuronId& seed : result) {
        auto it = pImpl->neuronToAssocIndices.find(seed);
        if (it != pImpl->neuronToAssocIndices.end()) {
            for (size_t index : it->second) {
                if (index < pImpl->associations.size()) {
                    auto& assoc = pImpl->associations[index];
                    
                    // Check if association strength is above threshold
                    if (std::get<2>(assoc) > 0.1f) {
                        // Add partner neuron
                        NeuronId partner = (std::get<0>(assoc) == seed) ? 
                            std::get<1>(assoc) : std::get<0>(assoc);
                        
                        // Only add if not already present
                        bool alreadyPresent = false;
                        for (const auto& existing : result) {
                            if (existing == partner) {
                                alreadyPresent = true;
                                break;
                            }
                        }
                        
                        if (!alreadyPresent) {
                            result.push_back(partner);
                        }
                    }
                }
            }
        }
    }
    
    return result;
}

std::vector<std::tuple<NeuronId, NeuronId, float>> AssociativeMemory::getAllAssociations() const {
    std::vector<std::tuple<NeuronId, NeuronId, float>> result;
    result.reserve(pImpl->associations.size());
    
    for (const auto& assoc : pImpl->associations) {
        result.emplace_back(std::get<0>(assoc), std::get<1>(assoc), std::get<2>(assoc));
    }
    
    return result;
}

void AssociativeMemory::clear() {
    pImpl->associations.clear();
    pImpl->neuronToAssocIndices.clear();
}

} // namespace nlm