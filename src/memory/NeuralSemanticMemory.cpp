#include "NeuralSemanticMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace nlm {

struct NeuralSemanticMemory::Impl {
    Brain* brain;
    
    // Concept storage
    std::unordered_map<std::string, std::vector<float>> conceptPatterns;
    std::unordered_map<std::string, float> conceptActivations;
    std::unordered_map<std::string, std::vector<float>> conceptLearnRates;
    
    // Concept stability (decay of old knowledge)
    std::unordered_map<std::string, float> conceptStability;
    
    // Integration with episodic memory
    bool conceptUpdatingEnabled;
    
    Impl() : brain(nullptr), conceptUpdatingEnabled(true) {}
};

NeuralSemanticMemory::NeuralSemanticMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
{
}

NeuralSemanticMemory::~NeuralSemanticMemory() = default;

void NeuralSemanticMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    pImpl->conceptUpdatingEnabled = true;
    
    NLM_LOG_INFO("NeuralSemanticMemory initialized");
}

void NeuralSemanticMemory::storeFact(const std::string& key, const std::vector<float>& pattern, float strength) {
    if (!brain_ || pattern.empty()) return;
    
    // Store the pattern
    pImpl->conceptPatterns[key] = pattern;
    
    // Initialize activation based on pattern strength
    pImpl->conceptActivations[key] = strength;
    pImpl->conceptLearnRates[key] = 0.01f; // Initial learning rate
    pImpl->conceptStability[key] = 1.0f; // Full stability for new concepts
    
    // Create neural pattern in brain
    NeuronId conceptNeuron(keyNeuronId(key));
    createConceptNeuralPattern(conceptNeuron, pattern);
    
    NLM_LOG_INFO("Stored concept fact: " + key + " with pattern size " + std::to_string(pattern.size()) + " strength " + std::to_string(strength));
}

std::vector<float> NeuralSemanticMemory::retrieveFact(const std::string& key) const {
    auto it = pImpl->conceptPatterns.find(key);
    if (it != pImpl->conceptPatterns.end()) {
        return it->second;
    }
    return std::vector<float>();
}

bool NeuralSemanticMemory::hasFact(const std::string& key) const {
    return pImpl->conceptPatterns.find(key) != pImpl->conceptPatterns.end();
}

std::vector<std::pair<std::string, std::vector<float>>> NeuralSemanticMemory::getAllFacts() const {
    std::vector<std::pair<std::string, std::vector<float>>> result;
    result.reserve(pImpl->conceptPatterns.size());
    
    for (const auto& kv : pImpl->conceptPatterns) {
        result.push_back(kv);
    }
    
    return result;
}

void NeuralSemanticMemory::clear() {
    pImpl->conceptPatterns.clear();
    pImpl->conceptActivations.clear();
    pImpl->conceptLearnRates.clear();
    pImpl->conceptStability.clear();
}

float NeuralSemanticMemory::getConceptActivation(const std::string& conceptName) const {
    auto it = pImpl->conceptActivations.find(conceptName);
    if (it != pImpl->conceptActivations.end()) {
        return it->second;
    }
    return 0.0f;
}

void NeuralSemanticMemory::updateConcept(const std::string& conceptName, float activation, float learningRate) {
    if (!pImpl->conceptUpdatingEnabled) return;
    
    auto patternIt = pImpl->conceptPatterns.find(conceptName);
    if (patternIt == pImpl->conceptPatterns.end()) {
        // Create new concept
        storeFact(conceptName, std::vector<float>(256, 0.0f), activation);
        return;
    }
    
    // Update existing concept
    std::vector<float>& pattern = patternIt->second;
    
    // Gradually modify pattern based on new activation
    for (size_t i = 0; i < pattern.size() && i < 100; ++i) { // Update first 100 neurons
        pattern[i] = pattern[i] * (1.0f - pImpl->conceptLearnRates[conceptName]) + 
                    activation * learningRate;
        pattern[i] = std::clamp(pattern[i], 0.0f, 1.0f);
    }
    
    // Update activation and learning rate
    pImpl->conceptActivations[conceptName] = activation;
    pImpl->conceptLearnRates[conceptName] = learningRate;
    
    // Update concept neural pattern
    NeuronId conceptNeuron = findConceptNeuron(conceptName);
    if (brain_ && conceptNeuron != INVALID_NEURON_ID) {
        // Update neuron activation based on concept
        brain_->injectCurrent(conceptNeuron, activation * 2.0f);
    }
}

void NeuralSemanticMemory::createConceptNeuralPattern(NeuronId neuron, const std::vector<float>& pattern) {
    if (!brain_) return;
    
    // Encode pattern in neural population
    size_t neuronsPerConcept = std::min(size_t(10), pattern.size() / 25);
    
    if (neuronsPerConcept > 0) {
        // Create new population for this concept
        RegionId regionId = brain_->getRegion(RegionId(1)); // Use first region
        if (regionId != INVALID_REGION_ID) {
            NeuralRegion* region = brain_->getRegion(regionId);
            if (region) {
                // Note: In real implementation, we would add neurons to population
                // This is a simplified approach
            }
        }
    }
}

NeuronId NeuralSemanticMemory::findConceptNeuron(const std::string& conceptName) {
    // In a full implementation, this would map concepts to specific neurons
    // For now, use deterministic hashing
    size_t hash = std::hash<std::string>{}(conceptName);
    return NeuronId(10000 + (hash % 10000)); // Create neuron ID in concept range
}

std::string NeuralSemanticMemory::keyNeuronId(const std::string& key) {
    // Convert concept name to a deterministic neuron ID for reference
    size_t hash = std::hash<std::string>{}(key);
    return "concept_" + std::to_string(hash % 10000);
}

// SemanticMemory Implementation
struct SemanticMemory::Impl {
    std::unordered_map<std::string, std::string> facts;
    
    Impl() {}
};

SemanticMemory::SemanticMemory() : pImpl(new Impl) {}
SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::storeFact(const std::string& key, const std::string& value) {
    pImpl->facts[key] = value;
}

std::string SemanticMemory::retrieveFact(const std::string& key) const {
    auto it = pImpl->facts.find(key);
    if (it != pImpl->facts.end()) {
        return it->second;
    }
    return "";
}

bool SemanticMemory::hasFact(const std::string& key) const {
    return pImpl->facts.find(key) != pImpl->facts.end();
}

std::vector<std::pair<std::string, std::string>> SemanticMemory::getAllFacts() const {
    std::vector<std::pair<std::string, std::string>> result;
    result.reserve(pImpl->facts.size());
    
    for (const auto& kv : pImpl->facts) {
        result.push_back(kv);
    }
    
    return result;
}

void SemanticMemory::clear() {
    pImpl->facts.clear();
}

} // namespace nlm