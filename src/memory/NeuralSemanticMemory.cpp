#include "NeuralSemanticMemory.hpp"

namespace nlm {

// NeuralSemanticMemory Implementation
struct NeuralSemanticMemory::Impl {
    // Pattern storage for each fact
    std::unordered_map<std::string, std::vector<float>> patterns;
    std::unordered_map<std::string, std::vector<NeuronId>> patternNeurons;
    
    // Recurrent connections for pattern maintenance
    std::vector<std::pair<NeuronId, NeuronId>> recurrentConnections;
    
    // Interference tracking
    std::vector<std::pair<std::string, std::string>> interferencePairs;
    
    // Pattern activation history for consolidation
    std::unordered_map<std::string, SimulationStep> lastAccessTime;
    
    // Pattern similarity cache for faster retrieval
    std::unordered_map<std::string, float> activationStrengths;
    
    // Memory capacity
    size_t capacity_;
    
    Impl() : capacity_(1000) {}
};

NeuralSemanticMemory::NeuralSemanticMemory() : pImpl(new Impl), brain_(nullptr) {
    // Set initial capacity
    pImpl->capacity_ = 1000;
}

NeuralSemanticMemory::~NeuralSemanticMemory() = default;

void NeuralSemanticMemory::initialize(Brain* brain) {
    brain_ = brain;
}

void NeuralSemanticMemory::storeFact(const std::string& key, const std::vector<float>& pattern) {
    storeNeuralPattern(key, pattern);
}

void NeuralSemanticMemory::storeNeuralPattern(const std::string& key, const std::vector<float>& pattern) {
    // Store pattern if within capacity
    if (pImpl->patterns.size() >= pImpl->capacity_) {
        decayWeakFacts();
    }
    
    // Convert pattern to neural activity
    pImpl->patterns[key] = pattern;
    
    // Create neurons for pattern representation
    std::vector<NeuronId> neurons;
    for (float activation : pattern) {
        if (activation > 0.5f) {
            // Create a neuron for this pattern
            size_t neuronIndex = 0;
            if (pImpl->patternNeurons.count(key) > 0) {
                neuronIndex = pImpl->patternNeurons[key].size();
            }
            NeuronId neuronId = NeuronId(neuronIndex + 1000);
            neurons.push_back(neuronId);
            
            // Store activation strength
            std::string activationKey = key + "_" + std::to_string(neuronId.index());
            pImpl->activationStrengths[activationKey] = activation;
        }
    }
    
    pImpl->patternNeurons[key] = neurons;
    
    // Create recurrent connections for pattern maintenance
    createFactRecurrentConnection(key);
    
    // Resolve interference
    resolveInterference();
    
    // Update access time
    pImpl->lastAccessTime[key] = SimulationStep(0);
}

std::vector<float> NeuralSemanticMemory::retrieveFact(const std::string& key) const {
    auto it = pImpl->patterns.find(key);
    if (it != pImpl->patterns.end()) {
        return it->second;
    }
    return std::vector<float>();
}

bool NeuralSemanticMemory::hasFact(const std::string& key) const {
    return pImpl->patterns.find(key) != pImpl->patterns.end();
}

std::vector<std::pair<std::string, std::vector<float>>> NeuralSemanticMemory::getAllFacts() const {
    std::vector<std::pair<std::string, std::vector<float>>> result;
    result.reserve(pImpl->patterns.size());
    for (const auto& fact : pImpl->patterns) {
        result.emplace_back(fact.first, fact.second);
    }
    return result;
}

void NeuralSemanticMemory::updateFact(const std::string& key, const std::vector<float>& pattern) {
    if (hasFact(key)) {
        pImpl->patterns[key] = pattern;
        pImpl->lastAccessTime[key] = SimulationStep(0);
    }
}

void NeuralSemanticMemory::deleteFact(const std::string& key) {
    pImpl->patterns.erase(key);
    pImpl->patternNeurons.erase(key);
    pImpl->lastAccessTime.erase(key);
}

std::vector<NeuronId> NeuralSemanticMemory::getFactNeurons(const std::string& key) const {
    auto it = pImpl->patternNeurons.find(key);
    if (it != pImpl->patternNeurons.end()) {
        return it->second;
    }
    return std::vector<NeuronId>();
}

float NeuralSemanticMemory::getFactActivation(const std::string& key, NeuronId neuron) const {
    auto factIt = pImpl->patterns.find(key);
    if (factIt != pImpl->patterns.end()) {
        size_t neuronIndex = neuron.index();
        if (neuronIndex < factIt->second.size()) {
            return factIt->second[neuronIndex];
        }
    }
    return 0.0f;
}

void NeuralSemanticMemory::createFactRecurrentConnection(const std::string& key) {
    auto it = pImpl->patternNeurons.find(key);
    if (it != pImpl->patternNeurons.end() && !it->second.empty()) {
        // Create a recurrent connection for each neuron to maintain pattern
        for (size_t i = 0; i < it->second.size(); ++i) {
            NeuronId from = it->second[i];
            NeuronId to = (i + 1 < it->second.size()) ? it->second[i + 1] : it->second[0];
            pImpl->recurrentConnections.emplace_back(from, to);
        }
    }
}

void NeuralSemanticMemory::decayWeakFacts() {
    // Decay facts that haven't been accessed recently
    auto currentTime = SimulationStep(0);
    std::vector<std::string> toRemove;
    
    for (const auto& fact : pImpl->lastAccessTime) {
        // Simple decay based on access time
        if (fact.second > SimulationStep(10000)) {
            toRemove.push_back(fact.first);
        }
    }
    
    for (const auto& key : toRemove) {
        pImpl->patterns.erase(key);
        pImpl->patternNeurons.erase(key);
        pImpl->lastAccessTime.erase(key);
    }
}

void NeuralSemanticMemory::resolveInterference() {
    // Simple interference resolution - remove the weaker of similar patterns
    // This is a placeholder implementation
    for (const auto& pair : pImpl->interferencePairs) {
        // Compare patterns and keep the more recently accessed
        if (pImpl->lastAccessTime.count(pair.first) && pImpl->lastAccessTime.count(pair.second)) {
            if (pImpl->lastAccessTime[pair.first] < pImpl->lastAccessTime[pair.second]) {
                pImpl->patterns.erase(pair.first);
                pImpl->patternNeurons.erase(pair.first);
                pImpl->lastAccessTime.erase(pair.first);
            } else {
                pImpl->patterns.erase(pair.second);
                pImpl->patternNeurons.erase(pair.second);
                pImpl->lastAccessTime.erase(pair.second);
            }
        }
    }
}

size_t NeuralSemanticMemory::getFactCount() const {
    return pImpl->patterns.size();
}

// Get capacity
size_t NeuralSemanticMemory::getCapacity() const {
    return pImpl->capacity_;
}

// Set capacity
void NeuralSemanticMemory::setCapacity(size_t cap) {
    pImpl->capacity_ = cap;
}
