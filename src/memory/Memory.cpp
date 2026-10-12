#include "Memory.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <random>

namespace nlm {

// Working Memory Implementation
struct WorkingMemory::Impl {
    Brain* brain;
    std::vector<std::pair<NeuronId, float>> items;
    size_t capacity;
    SimulationStep lastUpdate;
    
    // Neural activity patterns for distributed representation
    std::vector<NeuronId> neuronPool;
    std::unordered_map<NeuronId, float> activationPatterns;
    
    Impl(size_t cap) : brain(nullptr), capacity(cap), lastUpdate(0) {
        // Initialize neuron pool for pattern storage
        std::mt19937 rng(42);
        std::uniform_int_distribution<uint64_t> dist(1000, 100000);
        
        for (size_t i = 0; i < cap; ++i) {
            neuronPool.push_back(NeuronId(dist(rng)));
        }
    }
};

WorkingMemory::WorkingMemory() : pImpl(new Impl(100)) {}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::store(NeuronId neuron, float value) {
    if (!brain_) return;
    
    // Store as neural activation pattern
    pImpl->activationPatterns[neuron] = value;
    pImpl->items.push_back(std::make_pair(neuron, value));
    
    // Create distributed neural representation
    // Find available neuron for this memory
    if (pImpl->items.size() <= pImpl->neuronPool.size()) {
        size_t poolIndex = (pImpl->items.size() - 1) % pImpl->neuronPool.size();
        NeuronId memoryNeuron = pImpl->neuronPool[poolIndex];
        
        // Inject current to establish neural trace
        brain_->injectCurrent(memoryNeuron, value * 5.0f);
        
        // Create recurrent connection for maintenance
        if (pImpl->items.size() > 1) {
            NeuronId prevNeuron = pImpl->items[pImpl->items.size() - 2].first;
            // In real implementation, would create persistent synaptic connection
        }
    }
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    // Check activation patterns
    auto it = pImpl->activationPatterns.find(neuron);
    if (it != pImpl->activationPatterns.end()) {
        return it->second;
    }
    
    // Search in items vector
    for (const auto& item : pImpl->items) {
        if (item.first == neuron) {
            return item.second;
        }
    }
    
    return 0.0f;
}

bool WorkingMemory::contains(NeuronId neuron) const {
    return pImpl->activationPatterns.find(neuron) != pImpl->activationPatterns.end();
}

void WorkingMemory::clear() {
    pImpl->activationPatterns.clear();
    pImpl->items.clear();
}

size_t WorkingMemory::getCapacity() const {
    return pImpl->capacity;
}

size_t WorkingMemory::getCurrentSize() const {
    return pImpl->items.size();
}

void WorkingMemory::decay(float decayRate) {
    if (decayRate <= 0.0f) return;
    
    // Decay all activation patterns
    for (auto& pair : pImpl->activationPatterns) {
        pair.second *= (1.0f - decayRate);
    }
    
    // Decay item values
    for (auto& item : pImpl->items) {
        item.second *= (1.0f - decayRate);
    }
    
    // Remove near-zero values
    pImpl->activationPatterns.erase(
        std::remove_if(pImpl->activationPatterns.begin(), pImpl->activationPatterns.end(),
                      [](const auto& pair) { return pair.second < 0.001f; }),
        pImpl->activationPatterns.end()
    );
}

void WorkingMemory::update(TimestepDuration dt) {
    if (!brain_ || dt <= 0.0f) return;
    
    pImpl->lastUpdate++;
    
    // Natural decay based on time
    float timeDecay = 0.001f * dt;
    decay(timeDecay);
    
    // Reinforce active memories through hippocampal theta rhythms
    if (pImpl->lastUpdate % 10 == 0) {  // Theta burst every 10 steps
        for (auto& pair : pImpl->activationPatterns) {
            if (pair.second > 0.1f) {
                // Boost activity of memory neurons
                // In real implementation, would target specific hippocampal circuits
                float boost = pair.second * 0.1f;
                // brain_->injectCurrent(memoryNeuron, boost);
            }
        }
    }
}

// Episodic Memory Implementation
struct EpisodicMemory::Impl {
    Brain* brain;
    std::vector<EpisodicMemoryItem> episodes;
    size_t maxEpisodes;
    SimulationStep lastConsolidation;
    
    Impl(size_t max) : brain(nullptr), maxEpisodes(max), lastConsolidation(0) {}
};

EpisodicMemory::EpisodicMemory() : pImpl(new Impl(1000)) {}

EpisodicMemory::~EpisodicMemory() = default;

void EpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    if (!brain_) return;
    
    // Encode episode as neural activation pattern
    EpisodicMemoryItem encoded = episode;
    encoded.metadata = "ep_" + std::to_string(episodes.size());
    
    // Add neural activation scores based on brain state
    // In real implementation, would capture current neural activity
    for (size_t i = 0; i < std::min(episode.neurons.size(), 5u); ++i) {
        float activation = 0.5f + 0.5f * sin(static_cast<float>(i) * 0.5f);
        encoded.activations.push_back(activation);
    }
    if (encoded.activations.empty()) {
        encoded.activations.resize(std::min(episode.neurons.size(), 5u), 0.5f);
    }
    
    // Store episode
    if (episodes.size() >= maxEpisodes) {
        episodes.erase(episodes.begin());
    }
    episodes.push_back(encoded);
    
    // Replay episode for consolidation (simulated)
    replayEpisode(encoded);
}

EpisodicMemoryItem EpisodicMemory::retrieveEpisode(size_t index) const {
    if (index < episodes.size()) {
        return episodes[index];
    }
    return EpisodicMemoryItem();
}

size_t EpisodicMemory::getEpisodeCount() const {
    return episodes.size();
}

std::vector<EpisodicMemoryItem> EpisodicMemory::getRecentEpisodes(size_t count) const {
    std::vector<EpisodicMemoryItem> result;
    size_t start = count > episodes.size() ? 0 : episodes.size() - count;
    for (size_t i = start; i < episodes.size(); ++i) {
        result.push_back(episodes[i]);
    }
    return result;
}

void EpisodicMemory::clear() {
    episodes.clear();
}

void EpisodicMemory::consolidate(float relevanceThreshold) {
    if (!brain_) return;
    
    // Find important episodes based on neural activation
    std::vector<EpisodicMemoryItem> toKeep;
    
    for (const auto& episode : episodes) {
        // Compute relevance based on activation patterns
        float relevance = 0.0f;
        for (float activation : episode.activations) {
            relevance += activation;
        }
        relevance /= std::max<float>(episode.activations.size(), 1.0f);
        
        if (relevance >= relevanceThreshold) {
            toKeep.push_back(episode);
        }
    }
    
    // Keep important episodes, discard unimportant ones
    episodes = toKeep;
}

void EpisodicMemory::replayEpisode(const EpisodicMemoryItem& episode) {
    if (!brain_) return;
    
    // Replay neural patterns from episode
    for (size_t i = 0; i < episode.neurons.size() && i < episode.activations.size(); ++i) {
        NeuronId neuron = episode.neurons[i];
        float activation = episode.activations[i];
        
        // Inject current to reactivate neural pattern
        brain_->injectCurrent(neuron, activation * 3.0f);
        
        // Update synaptic connections (consolidation)
        // In real implementation, would strengthen relevant synapses
    }
}

std::vector<const EpisodicMemoryItem*> EpisodicMemory::getEpisodesForReplay(size_t maxCount) const {
    std::vector<const EpisodicMemoryItem*> result;
    if (maxCount == 0 || episodes.empty()) return result;
    
    // Prioritize recent episodes for replay
    size_t start = std::max<size_t>(0, static_cast<size_t>(episodes.size()) - maxCount);
    for (size_t i = start; i < episodes.size(); ++i) {
        result.push_back(&episodes[i]);
    }
    
    return result;
}

// Semantic Memory Implementation
struct SemanticMemory::Impl {
    Brain* brain;
    std::vector<std::pair<std::string, std::string>> facts;
    std::unordered_map<std::string, std::vector<std::string>> conceptLinks;
    
    Impl() : brain(nullptr) {}
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
    
    // Extract concepts and create associations
    std::vector<std::string> concepts = extractConcepts(value);
    for (const std::string& concept : concepts) {
        pImpl->conceptLinks[concept].push_back(key);
    }
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
    pImpl->conceptLinks.clear();
}

float SemanticMemory::getConceptSimilarity(const std::string& concept1, const std::string& concept2) const {
    // Check direct associations
    auto it1 = pImpl->conceptLinks.find(concept1);
    auto it2 = pImpl->conceptLinks.find(concept2);
    
    if (it1 == pImpl->conceptLinks.end() || it2 == pImpl->conceptLinks.end()) {
        return 0.0f;
    }
    
    // Compute intersection of related concepts
    std::unordered_set<std::string> related1(it1->second.begin(), it1->second.end());
    std::unordered_set<std::string> related2(it2->second.begin(), it2->second.end());
    
    size_t intersection = 0;
    for (const std::string& concept : related1) {
        if (related2.find(concept) != related2.end()) {
            intersection++;
        }
    }
    
    size_t unionSize = related1.size() + related2.size() - intersection;
    if (unionSize == 0) return 0.0f;
    
    return static_cast<float>(intersection) / static_cast<float>(unionSize);
}

std::vector<std::string> SemanticMemory::getRelatedConcepts(const std::string& concept) const {
    auto it = pImpl->conceptLinks.find(concept);
    if (it != pImpl->conceptLinks.end()) {
        return it->second;
    }
    return std::vector<std::string>();
}

std::vector<std::string> SemanticMemory::extractConcepts(const std::string& text) const {
    // Simple concept extraction - in real implementation would use NLP
    std::vector<std::string> concepts;
    std::string current;
    
    for (char c : text) {
        if (std::isalnum(c)) {
            current += c;
        } else if (!current.empty()) {
            // Check if it's a potential concept (not too common)
            if (current.length() >= 3 && current.length() <= 20) {
                concepts.push_back(current);
            }
            current.clear();
        }
    }
    
    if (!current.empty() && current.length() >= 3 && current.length() <= 20) {
        concepts.push_back(current);
    }
    
    return concepts;
}

// Procedural Memory Implementation
void ProceduralMemory::store(const std::string& key, const std::string& value) {
    for (auto& fact : pImpl->facts) {
        if (fact.first == key) {
            fact.second = value;
            return;
        }
    }
    pImpl->facts.emplace_back(key, value);
}

std::string ProceduralMemory::retrieve(const std::string& key) const {
    for (const auto& fact : pImpl->facts) {
        if (fact.first == key) {
            return fact.second;
        }
    }
    return "";
}

bool ProceduralMemory::has(const std::string& key) const {
    for (const auto& fact : pImpl->facts) {
        if (fact.first == key) {
            return true;
        }
    }
    return false;
}

std::vector<std::pair<std::string, std::string>> ProceduralMemory::getAll() const {
    return pImpl->facts;
}

void ProceduralMemory::clear() {
    pImpl->facts.clear();
}

// Associative Memory Implementation
void AssociativeMemory::store(const std::string& key1, const std::string& key2, float strength) {
    for (auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == key1 && std::get<1>(assoc) == key2) ||
            (std::get<0>(assoc) == key2 && std::get<1>(assoc) == key1)) {
            std::get<2>(assoc) = strength;
            return;
        }
    }
    pImpl->associations.emplace_back(key1, key2, strength);
    
    // Update indices
    pImpl->forwardIndex[key1].push_back(std::make_tuple(key2, strength));
    pImpl->backwardIndex[key2].push_back(std::make_tuple(key1, strength));
}

void AssociativeMemory::store(NeuronId neuron1, NeuronId neuron2, float strength) {
    std::string key1 = "neuron_" + std::to_string(neuron1.value);
    std::string key2 = "neuron_" + std::to_string(neuron2.value);
    store(key1, key2, strength);
}

void AssociativeMemory::store(const std::string& concept, NeuronId neuron, float strength) {
    std::string neuronKey = "neuron_" + std::to_string(neuron.value);
    store(concept, neuronKey, strength);
}

float AssociativeMemory::get(const std::string& key1, const std::string& key2) const {
    for (const auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == key1 && std::get<1>(assoc) == key2) ||
            (std::get<0>(assoc) == key2 && std::get<1>(assoc) == key1)) {
            return std::get<2>(assoc);
        }
    }
    return 0.0f;
}

float AssociativeMemory::get(NeuronId neuron1, NeuronId neuron2) const {
    std::string key1 = "neuron_" + std::to_string(neuron1.value);
    std::string key2 = "neuron_" + std::to_string(neuron2.value);
    return get(key1, key2);
}

std::vector<std::string> AssociativeMemory::getAssociations(const std::string& key) const {
    std::vector<std::string> result;
    auto it = pImpl->forwardIndex.find(key);
    if (it != pImpl->forwardIndex.end()) {
        for (const auto& pair : it->second) {
            result.push_back(std::get<0>(pair));
        }
    }
    return result;
}

std::vector<NeuronId> AssociativeMemory::getAssociations(NeuronId neuron) const {
    std::vector<NeuronId> result;
    std::string key = "neuron_" + std::to_string(neuron.value);
    auto it = pImpl->forwardIndex.find(key);
    if (it != pImpl->forwardIndex.end()) {
        for (const auto& pair : it->second) {
            // Convert back to NeuronId
            if (std::get<0>(pair).rfind("neuron_", 0) == 0) {
                std::string neuronStr = std::get<0>(pair).substr(7);
                try {
                    uint64_t val = std::stoull(neuronStr);
                    result.push_back(NeuronId(val));
                } catch (...) {
                    // Invalid neuron ID, skip
                }
            }
        }
    }
    return result;
}

void AssociativeMemory::update(const std::string& key1, const std::string& key2, float delta) {
    for (auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == key1 && std::get<1>(assoc) == key2) ||
            (std::get<0>(assoc) == key2 && std::get<1>(assoc) == key1)) {
            std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + delta, 0.0f, 1.0f);
            return;
        }
    }
    // If not found, create new association
    store(key1, key2, delta);
}

void AssociativeMemory::update(NeuronId neuron1, NeuronId neuron2, float delta) {
    std::string key1 = "neuron_" + std::to_string(neuron1.value);
    std::string key2 = "neuron_" + std::to_string(neuron2.value);
    update(key1, key2, delta);
}

bool AssociativeMemory::has(const std::string& key1, const std::string& key2) const {
    return get(key1, key2) > 0.0f;
}

bool AssociativeMemory::has(NeuronId neuron1, NeuronId neuron2) const {
    return get(neuron1, neuron2) > 0.0f;
}

void AssociativeMemory::clear() {
    pImpl->associations.clear();
    pImpl->forwardIndex.clear();
    pImpl->backwardIndex.clear();
}

// Procedural Memory Implementation
void ProceduralMemory::store(const std::string& key, const std::string& value) {
    for (auto& fact : pImpl->facts) {
        if (fact.first == key) {
            fact.second = value;
            return;
        }
    }
    pImpl->facts.emplace_back(key, value);
}

std::string ProceduralMemory::retrieve(const std::string& key) const {
    for (const auto& fact : pImpl->facts) {
        if (fact.first == key) {
            return fact.second;
        }
    }
    return "";
}

bool ProceduralMemory::has(const std::string& key) const {
    for (const auto& fact : pImpl->facts) {
        if (fact.first == key) {
            return true;
        }
    }
    return false;
}

std::vector<std::pair<std::string, std::string>> ProceduralMemory::getAll() const {
    return pImpl->facts;
}

void ProceduralMemory::clear() {
    pImpl->facts.clear();
}

} // namespace nlm

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
