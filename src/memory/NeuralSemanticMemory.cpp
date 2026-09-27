#include "NeuralSemanticMemory.hpp"
#include <algorithm>

namespace nlm {

struct NeuralSemanticMemory::Impl {
    struct Fact {
        std::string key;
        std::string value;
        
        Fact(const std::string& key, const std::string& value) 
            : key(key), value(value) {}
    };
    
    std::vector<Fact> facts;
    Brain* brain;
};

NeuralSemanticMemory::NeuralSemanticMemory() : pImpl(std::make_unique<Impl>()) {}

NeuralSemanticMemory::~NeuralSemanticMemory() = default;

void NeuralSemanticMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
}

void NeuralSemanticMemory::storeFact(const std::string& key, const std::string& value) {
    // Check if fact already exists
    auto it = std::find_if(pImpl->facts.begin(), pImpl->facts.end(),
                          [&key](const Fact& fact) { return fact.key == key; });
    
    if (it != pImpl->facts.end()) {
        it->value = value;
    } else {
        pImpl->facts.emplace_back(key, value);
    }
}

std::string NeuralSemanticMemory::retrieveFact(const std::string& key) const {
    auto it = std::find_if(pImpl->facts.begin(), pImpl->facts.end(),
                          [&key](const Fact& fact) { return fact.key == key; });
    
    return (it != pImpl->facts.end()) ? it->value : "";
}

bool NeuralSemanticMemory::hasFact(const std::string& key) const {
    return std::any_of(pImpl->facts.begin(), pImpl->facts.end(),
                      [&key](const Fact& fact) { return fact.key == key; });
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
}

} // namespace nlm
