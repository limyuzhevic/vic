#include "Memory.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace nlm {

// Working Memory Implementation
struct WorkingMemory::Impl {
    std::vector<std::pair<NeuronId, float>> items;
    size_t capacity;
    float decayRate;
    std::unordered_map<NeuronId, float> priorityScores;
    
    Impl(size_t cap) : capacity(cap), decayRate(0.01f) {}
};

WorkingMemory::WorkingMemory() : pImpl(new Impl(100)) {}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::store(NeuronId neuron, float value) {
    // TODO PHASE 2: Implement real storage with capacity limits
    // More sophisticated implementation with priority-based storage
    
    // Calculate priority score based on neuron activity and value
    float priority = calculatePriority(neuron, value);
    
    // Check if neuron already exists
    for (auto& item : pImpl->items) {
        if (item.first == neuron) {
            item.second = value;
            pImpl->priorityScores[neuron] = priority;
            return;
        }
    }
    
    // Check capacity
    if (pImpl->items.size() >= pImpl->capacity) {
        // Remove lowest priority item
        removeLowestPriorityItem();
    }
    
    // Add new item
    pImpl->items.emplace_back(neuron, value);
    pImpl->priorityScores[neuron] = priority;
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
    pImpl->priorityScores.clear();
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
        item.second *= (1.0f - pImpl->decayRate);
    }
}

float WorkingMemory::calculatePriority(NeuronId neuron, float value) const {
    // Calculate priority based on multiple factors
    float priority = std::abs(value);  // Magnitude-based priority
    
    // Add decay-based priority (items that haven't been retrieved have lower priority)
    // This would require tracking retrieval times in a full implementation
    
    // Add novelty-based priority (larger changes have higher priority)
    // This would require tracking history in a full implementation
    
    return priority;
}

void WorkingMemory::removeLowestPriorityItem() {
    if (pImpl->items.empty()) return;
    
    size_t lowestIndex = 0;
    float lowestPriority = pImpl->priorityScores[pImpl->items[0].first];
    
    for (size_t i = 1; i < pImpl->items.size(); ++i) {
        float currentPriority = pImpl->priorityScores[pImpl->items[i].first];
        if (currentPriority < lowestPriority) {
            lowestPriority = currentPriority;
            lowestIndex = i;
        }
    }
    
    // Remove the item with lowest priority
    pImpl->items.erase(pImpl->items.begin() + lowestIndex);
    pImpl->priorityScores.erase(pImpl->items[lowestIndex].first);
}

// Episodic Memory Implementation
struct EpisodicMemory::Impl {
    std::vector<EpisodicMemoryItem> episodes;
    size_t maxEpisodes;
    float consolidationThreshold;
    std::vector<size_t> consolidatedIndices;
    
    Impl(size_t max) : maxEpisodes(max), consolidationThreshold(0.7f) {}
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
    pImpl->consolidatedIndices.clear();
}

void EpisodicMemory::consolidate(float relevanceThreshold) {
    // TODO PHASE 2: Implement real consolidation
    // More sophisticated consolidation based on memory relevance
    
    pImpl->consolidationThreshold = relevanceThreshold;
    pImpl->consolidatedIndices.clear();
    
    // Calculate relevance scores for each episode
    std::vector<std::pair<size_t, float>> relevanceScores;
    
    for (size_t i = 0; i < pImpl->episodes.size(); ++i) {
        const auto& episode = pImpl->episodes[i];
        float relevance = calculateEpisodeRelevance(episode);
        relevanceScores.emplace_back(i, relevance);
    }
    
    // Sort by relevance
    std::sort(relevanceScores.begin(), relevanceScores.end(),
              [](const std::pair<size_t, float>& a, const std::pair<size_t, float>& b) {
                  return a.second > b.second;
              });
    
    // Mark episodes for consolidation
    for (size_t i = 0; i < relevanceScores.size(); ++i) {
        if (relevanceScores[i].second >= pImpl->consolidationThreshold) {
            pImpl->consolidatedIndices.push_back(relevanceScores[i].first);
        }
    }
    
    // Consolidate high-relevance episodes
    for (size_t index : pImpl->consolidatedIndices) {
        if (index < pImpl->episodes.size()) {
            consolidateEpisode(pImpl->episodes[index]);
        }
    }
}

float EpisodicMemory::calculateEpisodeRelevance(const EpisodicMemoryItem& episode) const {
    // Calculate relevance based on multiple factors
    float relevance = 0.0f;
    
    // Factor 1: Recency (recent episodes are more relevant)
    size_t age = pImpl->episodes.size() - 1;
    relevance += (1.0f - std::min(1.0f, age / 100.0f));
    
    // Factor 2: Reward (episodes with high reward are more relevant)
    relevance += std::min(1.0f, std::abs(episode.reward) / 10.0f);
    
    // Factor 3: Novelty (episodes with novel sensory states are more relevant)
    float novelty = 0.0f;
    for (float state : episode.sensoryState) {
        novelty += std::abs(state);
    }
    relevance += std::min(1.0f, novelty / 10.0f);
    
    // Factor 4: Action outcome (successful actions are more relevant)
    relevance += episode.resultingReward;
    
    return relevance;
}

void EpisodicMemory::consolidateEpisode(EpisodicMemoryItem& episode) {
    // Perform consolidation operations on the episode
    
    // 1. Extract salient features
    extractSalientFeatures(episode);
    
    // 2. Update episode with consolidated information
    episode.age = std::min(episode.age + 10, 100);  // Increase age
    
    // 3. Update neural activations based on consolidated memory
    updateNeuronalActivations(episode);
}

void EpisodicMemory::extractSalientFeatures(EpisodicMemoryItem& episode) {
    // Extract key features from the episode for efficient storage
    
    // Extract average sensory state
    if (!episode.sensoryState.empty()) {
        float sum = 0.0f;
        for (float state : episode.sensoryState) {
            sum += state;
        }
        episode.sensoryState.clear();
        episode.sensoryState.push_back(sum / episode.sensoryState.size());
    }
    
    // Extract average resulting sensory state
    if (!episode.resultingSensoryState.empty()) {
        float sum = 0.0f;
        for (float state : episode.resultingSensoryState) {
            sum += state;
        }
        episode.resultingSensoryState.clear();
        episode.resultingSensoryState.push_back(sum / episode.resultingSensoryState.size());
    }
    
    // Extract average neuron activations
    if (!episode.neuronActivations.empty()) {
        float sum = 0.0f;
        for (float activation : episode.neuronActivations) {
            sum += activation;
        }
        episode.neuronActivations.clear();
        episode.neuronActivations.push_back(sum / episode.neuronActivations.size());
    }
}

void EpisodicMemory::updateNeuronalActivations(const EpisodicMemoryItem& episode) {
    // Update neuronal activations based on consolidated episode
    // This would integrate with the brain's working memory
    // For simplicity, we'll skip this implementation
}

// Semantic Memory Implementation
struct SemanticMemory::Impl {
    std::vector<std::pair<std::string, std::string>> facts;
    std::unordered_map<std::string, float> activationLevels;
    float decayRate;
    
    Impl() : decayRate(0.01f) {}
};

SemanticMemory::SemanticMemory() : pImpl(new Impl) {}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::storeFact(const std::string& key, const std::string& value) {
    for (auto& fact : pImpl->facts) {
        if (fact.first == key) {
            fact.second = value;
            pImpl->activationLevels[key] = 1.0f;  // Reset activation
            return;
        }
    }
    pImpl->facts.emplace_back(key, value);
    pImpl->activationLevels[key] = 1.0f;
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
    pImpl->activationLevels.clear();
}

void SemanticMemory::decayActivations() {
    for (auto& pair : pImpl->activationLevels) {
        pair.second *= (1.0f - pImpl->decayRate);
    }
}

// Procedural Memory Implementation
struct ProceduralMemory::Impl {
    std::vector<Skill> skills;
    float decayRate;
    
    Impl() : decayRate(0.01f) {}
};

ProceduralMemory::ProceduralMemory() : pImpl(new Impl) {}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            skill.neuralPattern = pattern;
            skill.lastUsed = 0;  // Reset usage
            skill.accuracy = 1.0f;  // Set to perfect for new skill
            return;
        }
    }
    Skill skill;
    skill.name = name;
    skill.neuralPattern = pattern;
    skill.proficiency = 0.5f;  // Start with moderate proficiency
    skill.accuracy = 0.7f;  // Start with good accuracy
    skill.lastUsed = 0;
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
            skill.accuracy = std::clamp(skill.accuracy + delta * 0.5f, 0.0f, 1.0f);
            return;
        }
    }
}

void ProceduralMemory::updateSkillUsage(const std::string& name, SimulationStep step) {
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            skill.lastUsed = step;
            break;
        }
    }
}

void ProceduralMemory::decayProficiencies() {
    for (auto& skill : pImpl->skills) {
        skill.proficiency *= (1.0f - pImpl->decayRate);
        skill.accuracy *= (1.0f - pImpl->decayRate * 0.5f);
    }
}

void ProceduralMemory::clear() {
    pImpl->skills.clear();
}

// Associative Memory Implementation
struct AssociativeMemory::Impl {
    // Pairs of (a, b) associations with strength
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations;
    float decayRate;
    
    Impl() : decayRate(0.01f) {}
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

void AssociativeMemory::decayAssociations() {
    for (auto& assoc : pImpl->associations) {
        std::get<2>(assoc) *= (1.0f - pImpl->decayRate);
    }
}

void AssociativeMemory::clear() {
    pImpl->associations.clear();
}

} // namespace nlm
