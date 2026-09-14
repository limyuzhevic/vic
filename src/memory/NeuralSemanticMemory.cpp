#include "NeuralSemanticMemory.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct NeuralSemanticMemory::Impl {
    class Brain* brain;
    std::vector<std::pair<std::string, std::pair<std::string, float>>> facts;  // key -> (value, strength)
    std::vector<float> decayRates;
    
    Impl() : brain(nullptr) {}
};

NeuralSemanticMemory::NeuralSemanticMemory() : pImpl(new Impl) {}

NeuralSemanticMemory::~NeuralSemanticMemory() = default;

void NeuralSemanticMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Semantic memory initialized");
}

void NeuralSemanticMemory::storeFact(const std::string& key, const std::string& value, float strength, float decayRate) {
    // Find existing fact
    for (auto& factPair : pImpl->facts) {
        if (factPair.first == key) {
            // Update existing fact
            factPair.second.first = value;
            factPair.second.second = strength;
            // Find and update decay rate
            for (size_t i = 0; i < pImpl->facts.size(); ++i) {
                if (i < pImpl->decayRates.size() && pImpl->facts[i].first == key) {
                    pImpl->decayRates[i] = decayRate;
                    break;
                }
            }
            return;
        }
    }
    
    // Add new fact
    pImpl->facts.push_back({key, {value, strength}});
    pImpl->decayRates.push_back(decayRate);
}

std::string NeuralSemanticMemory::retrieveFact(const std::string& key) const {
    for (const auto& factPair : pImpl->facts) {
        if (factPair.first == key) {
            return factPair.second.first;
        }
    }
    return "";
}

bool NeuralSemanticMemory::hasFact(const std::string& key) const {
    for (const auto& factPair : pImpl->facts) {
        if (factPair.first == key) {
            return true;
        }
    }
    return false;
}

std::vector<std::pair<std::string, std::string>> NeuralSemanticMemory::getAllFacts() const {
    std::vector<std::pair<std::string, std::string>> result;
    for (const auto& factPair : pImpl->facts) {
        result.push_back({factPair.first, factPair.second.first});
    }
    return result;
}

void NeuralSemanticMemory::updateFactStrength(const std::string& key, float delta) {
    for (auto& factPair : pImpl->facts) {
        if (factPair.first == key) {
            factPair.second.second += delta;
            if (factPair.second.second < 0.0f) {
                factPair.second.second = 0.0f;
            }
            break;
        }
    }
}

void NeuralSemanticMemory::decay(float decayRate) {
    for (size_t i = 0; i < pImpl->facts.size(); ++i) {
        pImpl->decayRates[i] *= decayRate;
        if (pImpl->decayRates[i] < 0.01f) {
            // Remove weak facts
            pImpl->facts.erase(pImpl->facts.begin() + i);
            if (i < pImpl->decayRates.size()) {
                pImpl->decayRates.erase(pImpl->decayRates.begin() + i);
            }
            --i;
        }
    }
}

} // namespace nlm