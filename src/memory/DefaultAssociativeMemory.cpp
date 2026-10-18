#pragma once

#include "IMemorySystem.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace nlm {

struct DefaultAssociativeMemory::Impl {
    Brain* brain_;
    std::vector<std::pair<NeuronId, std::vector<float>>> patternNeurons_;
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations_;
    
    DefaultAssociativeMemory::Impl() : brain_(nullptr) {}
};

DefaultAssociativeMemory::DefaultAssociativeMemory() : pImpl(std::make_unique<Impl>()), enabled_(true) {}

DefaultAssociativeMemory::~DefaultAssociativeMemory() = default;

void DefaultAssociativeMemory::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize DefaultAssociativeMemory with null brain");
        return;
    }
    
    pImpl->brain_ = brain;
    NLM_LOG_INFO("DefaultAssociativeMemory initialized");
}

void DefaultAssociativeMemory::associate(const std::vector<float>& patternA,
                                        const std::vector<float>& patternB,
                                        float strength) {
    if (!enabled_) return;
    
    // Find or create neurons for patterns
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    // Add association
    pImpl->associations_.emplace_back(neuronA, neuronB, strength);
    
    NLM_LOG_INFO("Created association between patterns with strength " + std::to_string(strength));
}

std::vector<std::vector<float>> DefaultAssociativeMemory::retrieve(const std::vector<float>& queryPattern,
                                                                  size_t maxResults) const {
    if (!enabled_) return std::vector<std::vector<float>>();
    
    std::vector<std::vector<float>> results;
    NeuronId queryNeuron = findPatternNeuron(queryPattern);
    
    // Find associated patterns
    for (const auto& association : pImpl->associations_) {
        NeuronId neuronA = std::get<0>(association);
        NeuronId neuronB = std::get<1>(association);
        float strength = std::get<2>(association);
        
        std::vector<float> pattern;
        if (neuronA == queryNeuron) {
            pattern = retrievePatternByNeuron(neuronB);
        } else if (neuronB == queryNeuron) {
            pattern = retrievePatternByNeuron(neuronA);
        }
        
        if (!pattern.empty()) {
            results.push_back(pattern);
        }
        
        if (results.size() >= maxResults) break;
    }
    
    return results;
}

float DefaultAssociativeMemory::getAssociationStrength(const std::vector<float>& patternA,
                                                      const std::vector<float>& patternB) const {
    if (!enabled_) return 0.0f;
    
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    for (const auto& association : pImpl->associations_) {
        NeuronId assocA = std::get<0>(association);
        NeuronId assocB = std::get<1>(association);
        float strength = std::get<2>(association);
        
        if ((assocA == neuronA && assocB == neuronB) || (assocA == neuronB && assocB == neuronA)) {
            return strength;
        }
    }
    
    return 0.0f;
}

void DefaultAssociativeMemory::clear() {
    pImpl->patternNeurons_.clear();
    pImpl->associations_.clear();
}

size_t DefaultAssociativeMemory::getAssociationCount() const {
    if (!enabled_) return 0;
    return pImpl->associations_.size();
}

NeuronId DefaultAssociativeMemory::findPatternNeuron(const std::vector<float>& pattern) {
    // Look for existing pattern
    for (size_t i = 0; i < pImpl->patternNeurons_.size(); ++i) {
        const auto& existingPattern = pImpl->patternNeurons_[i].second;
        if (comparePatterns(pattern, existingPattern)) {
            return pImpl->patternNeurons_[i].first;
        }
    }
    
    // Create new pattern neuron
    NeuronId neuronId = static_cast<NeuronId>(pImpl->patternNeurons_.size());
    pImpl->patternNeurons_.emplace_back(neuronId, pattern);
    return neuronId;
}

std::vector<float> DefaultAssociativeMemory::retrievePatternByNeuron(NeuronId neuron) const {
    for (const auto& patternNeuron : pImpl->patternNeurons_) {
        if (patternNeuron.first == neuron) {
            return patternNeuron.second;
        }
    }
    return std::vector<float>();
}

bool DefaultAssociativeMemory::comparePatterns(const std::vector<float>& a,
                                               const std::vector<float>& b) const {
    if (a.size() != b.size()) return false;
    
    for (size_t i = 0; i < a.size(); ++i) {
        if (std::abs(a[i] - b[i]) > 0.01f) { // Tolerance for floating point comparison
            return false;
        }
    }
    
    return true;
}

} // namespace nlm