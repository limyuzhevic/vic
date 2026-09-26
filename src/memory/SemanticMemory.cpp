#include "SemanticMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <ctime>
#include <sstream>

namespace nlm {

struct SemanticMemory::Impl {
    std::vector<std::pair<std::string, std::string>> facts;
    size_t maxFacts;
    
    Impl(size_t max) : maxFacts(max) {}
};

SemanticMemory::SemanticMemory() : pImpl(new Impl(1000)) {}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::storeFact(const std::string& key, const std::string& value) {
    // TODO PHASE 2: Implement fact storage with validation and consolidation
    for (auto& fact : pImpl->facts) {
        if (fact.first == key) {
            fact.second = value;
            return;
        }
    }
    
    if (pImpl->facts.size() < pImpl->maxFacts) {
        pImpl->facts.emplace_back(key, value);
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
}

} // namespace nlm