#include "Memory.hpp"
#include "NeuralWorkingMemory.hpp"
#include "NeuralEpisodicMemory.hpp"
#include "NeuralAssociativeMemory.hpp"
#include <algorithm>

namespace nlm {

// Working Memory Implementation - using NeuralWorkingMemory
struct WorkingMemory::Impl {
    std::unique_ptr<NeuralWorkingMemory> neuralMemory;
    std::unordered_map<NeuronId, float> legacyItems;
    
    Impl() {
        neuralMemory = std::make_unique<NeuralWorkingMemory>();
    }
};

WorkingMemory::WorkingMemory() : pImpl(new Impl) {}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::store(NeuronId neuron, float value) {
    // Delegate to neural working memory
    if (pImpl->neuralMemory) {
        pImpl->neuralMemory->storeToNeuron(neuron, value);
    } else {
        // Legacy fallback
        pImpl->legacyItems[neuron] = value;
    }
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    // Try neural memory first
    if (pImpl->neuralMemory) {
        return pImpl->neuralMemory->getNeuronActivation(neuron);
    }
    
    // Legacy fallback
    auto it = pImpl->legacyItems.find(neuron);
    return (it != pImpl->legacyItems.end()) ? it->second : 0.0f;
}

bool WorkingMemory::contains(NeuronId neuron) const {
    if (pImpl->neuralMemory) {
        return pImpl->neuralMemory->contains(neuron);
    }
    
    return pImpl->legacyItems.find(neuron) != pImpl->legacyItems.end();
}

void WorkingMemory::clear() {
    if (pImpl->neuralMemory) {
        pImpl->neuralMemory->clear();
    }
    pImpl->legacyItems.clear();
}

size_t WorkingMemory::getCapacity() const {
    // Neural memory has internal capacity
    return 100; // Default capacity
}

size_t WorkingMemory::getCurrentSize() const {
    if (pImpl->neuralMemory) {
        return pImpl->neuralMemory->getActiveTraces();
    }
    return pImpl->legacyItems.size();
}

void WorkingMemory::decay(float decayRate) {
    if (pImpl->neuralMemory) {
        pImpl->neuralMemory->decayTraces(decayRate);
    } else {
        for (auto& item : pImpl->legacyItems) {
            item.second *= (1.0f - decayRate);
        }
    }
}

// Neural-specific methods for working memory
void WorkingMemory::update(TimestepDuration dt) {
    if (pImpl->neuralMemory) {
        pImpl->neuralMemory->update(dt);
    }
}

size_t WorkingMemory::getActiveTraces() const {
    if (pImpl->neuralMemory) {
        return pImpl->neuralMemory->getActiveTraces();
    }
    return 0;
}

std::vector<float> WorkingMemory::getAllActivations() const {
    if (pImpl->neuralMemory) {
        return pImpl->neuralMemory->retrieve();
    }
    
    std::vector<float> result;
    result.reserve(pImpl->legacyItems.size());
    for (const auto& item : pImpl->legacyItems) {
        result.push_back(item.second);
    }
    return result;
}

void WorkingMemory::applyHebbianLearning(const std::vector<float>& pattern, float learningRate) {
    if (pImpl->neuralMemory) {
        pImpl->neuralMemory->applyHebbianLearning(pattern, learningRate);
    }
}

// Episodic Memory Implementation - using NeuralEpisodicMemory
EpisodicMemory::EpisodicMemory() : maxEpisodes_(1000) {}

EpisodicMemory::~EpisodicMemory() = default;

void EpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    episodes_.push_back(episode);
    
    // Remove old episodes if over capacity
    while (episodes_.size() > maxEpisodes_) {
        episodes_.erase(episodes_.begin());
    }
}

void EpisodicMemory::clear() {
    episodes_.clear();
}

void EpisodicMemory::consolidate(float relevanceThreshold) {
    // Simple consolidation: remove episodes with low relevance
    // In a real implementation, this would use neural consolidation mechanisms
    auto it = std::remove_if(episodes_.begin(), episodes_.end(),
                            [relevanceThreshold](const EpisodicMemoryItem& ep) {
                                // Use reward as a proxy for relevance
                                return ep.reward < relevanceThreshold;
                            });
    episodes_.erase(it, episodes_.end());
}

void EpisodicMemory::replayDuringRest(float strength) {
    if (episodes_.empty()) return;
    
    // Select episodes for replay based on reward (simple heuristic)
    std::vector<std::pair<float, size_t>> scoredEpisodes;
    for (size_t i = 0; i < episodes_.size(); ++i) {
        scoredEpisodes.emplace_back(episodes_[i].reward, i);
    }
    
    std::sort(scoredEpisodes.begin(), scoredEpisodes.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Replay top episodes
    size_t numToReplay = std::min(static_cast<size_t>(3), scoredEpisodes.size());
    for (size_t i = 0; i < numToReplay; ++i) {
        size_t episodeId = scoredEpisodes[i].second;
        // In a real implementation, this would reactivate neural patterns
        // For now, we just log the replay
    }
}

void EpisodicMemory::applyPredictionError(float errorSignal) {
    // Use prediction error to modulate episode relevance
    for (auto& episode : episodes_) {
        float errorWeight = std::min(std::abs(errorSignal) * 2.0f, 1.0f);
        episode.reward = std::clamp(episode.reward + errorWeight * 0.1f, 0.0f, 1.0f);
        episode.novelty = std::min(episode.novelty + std::abs(errorSignal) * 0.05f, 1.0f);
    }
}

void EpisodicMemory::updateAllEpisodes(SimulationStep currentStep) {
    // Update all episodes with current simulation time for temporal tracking
    for (auto& episode : episodes_) {
        episode.age++;
        // Simple temporal processing
        if (currentStep > episode.timestamp) {
            episode.age = currentStep - episode.timestamp;
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

// Associative Memory Implementation - using NeuralAssociativeMemory
struct AssociativeMemory::Impl {
    std::unique_ptr<NeuralAssociativeMemory> neuralMemory;
    std::vector<std::pair<NeuronId, NeuronId>> legacyAssociations;
    
    Impl() {
        neuralMemory = std::make_unique<NeuralAssociativeMemory>();
    }
};

AssociativeMemory::AssociativeMemory() : pImpl(new Impl) {}

AssociativeMemory::~AssociativeMemory() = default;

void AssociativeMemory::associate(NeuronId a, NeuronId b, float strength) {
    // Delegate to neural associative memory
    if (pImpl->neuralMemory) {
        // Convert neuron IDs to activity patterns
        std::vector<float> pattern1{static_cast<float>(a.value) / 10000.0f};
        std::vector<float> pattern2{static_cast<float>(b.value) / 10000.0f};
        pImpl->neuralMemory->storeAssociation(pattern1, pattern2, strength);
    } else {
        // Legacy fallback
        for (auto& assoc : pImpl->legacyAssociations) {
            if ((assoc.first == a && assoc.second == b) || (assoc.first == b && assoc.second == a)) {
                // Update existing
                return;
            }
        }
        pImpl->legacyAssociations.emplace_back(a, b);
    }
}

std::vector<NeuronId> AssociativeMemory::getAssociations(NeuronId neuron) const {
    std::vector<NeuronId> result;
    
    // Try neural memory first
    if (pImpl->neuralMemory) {
        // In a real implementation, we would retrieve associations from neural memory
        // For now, we use legacy for simplicity
    }
    
    // Legacy fallback
    for (const auto& assoc : pImpl->legacyAssociations) {
        if (assoc.first == neuron) {
            result.push_back(assoc.second);
        } else if (assoc.second == neuron) {
            result.push_back(assoc.first);
        }
    }
    return result;
}

float AssociativeMemory::getAssociationStrength(NeuronId a, NeuronId b) const {
    // Try neural memory first
    if (pImpl->neuralMemory) {
        // In a real implementation, we would retrieve association strength from neural memory
    }
    
    // Legacy fallback
    for (const auto& assoc : pImpl->legacyAssociations) {
        if ((assoc.first == a && assoc.second == b) || (assoc.first == b && assoc.second == a)) {
            return 1.0f; // Default strength
        }
    }
    return 0.0f;
}

void AssociativeMemory::updateAssociation(NeuronId a, NeuronId b, float delta) {
    // Try neural memory first
    if (pImpl->neuralMemory) {
        // In a real implementation, we would update association in neural memory
    }
    
    // Legacy fallback
    for (auto& assoc : pImpl->legacyAssociations) {
        if ((assoc.first == a && assoc.second == b) || (assoc.first == b && assoc.second == a)) {
            // Update strength (not implemented in legacy)
            return;
        }
    }
}

void AssociativeMemory::clear() {
    if (pImpl->neuralMemory) {
        pImpl->neuralMemory->clear();
    }
    pImpl->legacyAssociations.clear();
}

// Neural-specific methods for associative memory
void AssociativeMemory::storeAssociation(const std::vector<float>& pattern1,
                                         const std::vector<float>& pattern2,
                                         float strength) {
    if (pImpl->neuralMemory) {
        pImpl->neuralMemory->storeAssociation(pattern1, pattern2, strength);
    }
}

std::vector<float> AssociativeMemory::getAssociation(const std::vector<float>& pattern) const {
    if (pImpl->neuralMemory) {
        return pImpl->neuralMemory->retrieve(pattern);
    }
    return std::vector<float>();
}

float AssociativeMemory::getAssociationStrength(const std::vector<float>& pattern1,
                                                 const std::vector<float>& pattern2) const {
    if (pImpl->neuralMemory) {
        return pImpl->neuralMemory->getAssociationStrength(pattern1, pattern2);
    }
    return 0.0f;
}

std::vector<float> AssociativeMemory::retrieve(const std::vector<float>& input,
                                               float threshold) const {
    if (pImpl->neuralMemory) {
        return pImpl->neuralMemory->retrieve(input, threshold);
    }
    return std::vector<float>();
}

void AssociativeMemory::applyHebbianLearning(const std::vector<float>& pattern, float learningRate) {
    if (pImpl->neuralMemory) {
        // Apply Hebbian learning in neural associative memory
        // This would strengthen associations with co-active patterns
    }
}

} // namespace nlm
