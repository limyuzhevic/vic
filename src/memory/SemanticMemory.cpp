// SemanticMemory.cpp
#include "SemanticMemory.hpp"
#include <algorithm>

namespace nlm {

struct SemanticMemory::Impl {
    std::vector<std::pair<std::string, std::string>> facts;
    
    // TEMPORARY: Convert facts to episodic storage for now
    std::vector<std::string> episodicStorage;
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
    
    // Also store in episodic memory for potential consolidation later
    std::string episode = "Fact: " + key + " = " + value;
    pImpl->episodicStorage.push_back(episode);
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
    pImpl->episodicStorage.clear();
}

} // namespace nlm