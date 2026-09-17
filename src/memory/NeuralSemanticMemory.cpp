#include "NeuralSemanticMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <chrono>

namespace nlm {

struct NeuralSemanticMemory::Impl {
    struct Fact {
        std::string key;
        std::string value;
        float age;  // Time since fact was stored
        std::chrono::steady_clock::time_point timestamp;
    };
    
    std::vector<Fact> facts;
    float decayRate;
    
    Impl() : decayRate(0.005f) {}
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
    NLM_LOG_INFO("NeuralSemanticMemory initialized");
}

void NeuralSemanticMemory::storeFact(const std::string& key, const std::string& value) {
    // Check if fact already exists
    for (auto& fact : pImpl->facts) {
        if (fact.key == key) {
            fact.value = value;
            fact.age = 0.0f;
            fact.timestamp = std::chrono::steady_clock::now();
            NLM_LOG_INFO("Updated semantic fact: " + key);
            return;
        }
    }
    
    // Add new fact
    Impl::Fact fact;
    fact.key = key;
    fact.value = value;
    fact.age = 0.0f;
    fact.timestamp = std::chrono::steady_clock::now();
    pImpl->facts.push_back(fact);
    
    // Also store in episodic memory if available
    if (brain_) {
        if (auto* episodicMem = brain_->getEpisodicMemory()) {
            // Create a simple episodic record
            EpisodicMemoryItem item;
            item.timestamp = 0;  // Will be set by brain if needed
            item.metadata = "semantic_fact:" + key;
            episodicMem->storeEpisode(item);
        }
    }
    
    NLM_LOG_INFO("Stored semantic fact: " + key);
}

std::string NeuralSemanticMemory::retrieveFact(const std::string& key) const {
    for (const auto& fact : pImpl->facts) {
        if (fact.key == key) {
            return fact.value;
        }
    }
    return "";
}

bool NeuralSemanticMemory::hasFact(const std::string& key) const {
    for (const auto& fact : pImpl->facts) {
        if (fact.key == key) {
            return true;
        }
    }
    return false;
}

std::vector<std::pair<std::string, std::string>> NeuralSemanticMemory::getAllFacts() const {
    std::vector<std::pair<std::string, std::string>> result;
    result.reserve(pImpl->facts.size());
    
    for (const auto& fact : pImpl->facts) {
        result.emplace_back(fact.key, fact.value);
    }
    
    return result;
}

void NeuralSemanticMemory::clear() {
    pImpl->facts.clear();
    NLM_LOG_INFO("NeuralSemanticMemory cleared");
}

void NeuralSemanticMemory::update(float dt) {
    // Update fact ages
    auto now = std::chrono::steady_clock::now();
    for (auto& fact : pImpl->facts) {
        fact.age += dt;
        
        // Decay older facts
        if (fact.age > 1000.0f) {
            // Very old facts have reduced weight
            // In a real implementation, this would affect retrieval probability
            fact.value = "[STALE] " + fact.value;
        }
    }
}

} // namespace nlm
