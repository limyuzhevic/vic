#pragma once

#include "NeuralSemanticMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct NeuralSemanticMemory::Impl {
    Brain* brain;
    
    // For capacity management
    std::vector<std::string> factHistory;
    std::vector<float> factWeights;
    
    Impl() : brain(nullptr) {}
};

NeuralSemanticMemory::NeuralSemanticMemory()
    : pImpl(std::make_unique<Impl>())
    , brain_(nullptr)
    , capacity_(1000)
{
}

NeuralSemanticMemory::~NeuralSemanticMemory() = default;

void NeuralSemanticMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralSemanticMemory initialized");
}

void NeuralSemanticMemory::storeFact(const std::string& key, const std::string& value) {
    if (key.empty() || value.empty() || !brain_) return;
    
    // Check if fact already exists
    auto it = facts_.find(key);
    if (it != facts_.end()) {
        // Update existing fact
        it->second.value = value;
        it->second.timestamp = 0;  // Reset for freshness
        it->second.relevance = 1.0f;
    } else {
        // Add new fact
        if (facts_.size() >= capacity_) {
            // Remove least relevant fact
            auto minIt = std::min_element(facts_.begin(), facts_.end(),
                [](const auto& a, const auto& b) { return a.second.relevance < b.second.relevance; });
            facts_.erase(minIt);
        }
        
        facts_[key] = SemanticFact(key, value, 0);
        
        // Create association in neural network
        // In real implementation, this would create Hebbian connections
    }
}

std::string NeuralSemanticMemory::retrieveFact(const std::string& key) const {
    auto it = facts_.find(key);
    if (it != facts_.end()) {
        // Update relevance when accessed
        if (it->second.relevance < 1.0f) {
            const_cast<SemanticFact&>(it->second).relevance = 1.0f;
        }
        return it->second.value;
    }
    return "";
}

bool NeuralSemanticMemory::hasFact(const std::string& key) const {
    return facts_.find(key) != facts_.end();
}

std::vector<std::pair<std::string, std::string>> NeuralSemanticMemory::getAllFacts() const {
    std::vector<std::pair<std::string, std::string>> result;
    result.reserve(facts_.size());
    
    for (const auto& pair : facts_) {
        result.emplace_back(pair.first, pair.second.value);
    }
    
    return result;
}

std::vector<std::pair<std::string, std::string>> NeuralSemanticMemory::getRelevantFacts(float threshold) const {
    std::vector<std::pair<std::string, std::string>> result;
    
    for (const auto& pair : facts_) {
        if (pair.second.relevance >= threshold) {
            result.emplace_back(pair.first, pair.second.value);
        }
    }
    
    return result;
}

void NeuralSemanticMemory::updateFactRelevance(const std::string& key, float delta) {
    auto it = facts_.find(key);
    if (it != facts_.end()) {
        it->second.relevance = std::clamp(it->second.relevance + delta, 0.0f, 1.0f);
    }
}

void NeuralSemanticMemory::clear() {
    facts_.clear();
}

size_t NeuralSemanticMemory::getActiveFactCount() const {
    size_t count = 0;
    for (const auto& pair : facts_) {
        if (pair.second.relevance > 0.5f) {
            count++;
        }
    }
    return count;
}

void NeuralSemanticMemory::updateRelevanceBasedOnUsage() {
    // Decay relevance over time
    for (auto& pair : facts_) {
        pair.second.relevance *= 0.99f;
    }
}

} // namespace nlm
