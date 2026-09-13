#pragma once

#include <string>
#include <vector>

namespace nlm {

struct NeuralSemanticMemory::Impl {
    std::vector<std::pair<std::string, std::string>> facts;
};

NeuralSemanticMemory::NeuralSemanticMemory() : pImpl(new Impl) {}
NeuralSemanticMemory::~NeuralSemanticMemory() = default;

void NeuralSemanticMemory::storeFact(const std::string& key, const std::string& value) {
    for (auto& fact : pImpl->facts) {
        if (fact.first == key) {
            fact.second = value;
            return;
        }
    }
    pImpl->facts.emplace_back(key, value);
}

std::string NeuralSemanticMemory::retrieveFact(const std::string& key) const {
    for (const auto& fact : pImpl->facts) {
        if (fact.first == key) {
            return fact.second;
        }
    }
    return "";
}

bool NeuralSemanticMemory::hasFact(const std::string& key) const {
    for (const auto& fact : pImpl->facts) {
        if (fact.first == key) {
            return true;
        }
    }
    return false;
}

std::vector<std::pair<std::string, std::string>> NeuralSemanticMemory::getAllFacts() const {
    return pImpl->facts;
}

void NeuralSemanticMemory::clear() {
    pImpl->facts.clear();
}

} // namespace nlm
