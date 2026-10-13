#include "SemanticMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct SemanticMemory::Impl {
    float decayRate = 0.01f;
    float minActivation = 0.1f;
};

SemanticMemory::SemanticMemory() : pImpl(new Impl) {}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::storeFact(const std::string& key, const std::string& value) {
    facts_[key] = value;
    activations_[key] = 1.0f;  // New fact has full activation
    
    NLM_LOG_INFO("SemanticMemory stored fact: " + key + " = " + value);
}

std::string SemanticMemory::retrieveFact(const std::string& key) const {
    auto it = facts_.find(key);
    if (it != facts_.end()) {
        return it->second;
    }
    return "";
}

bool SemanticMemory::hasFact(const std::string& key) const {
    return facts_.find(key) != facts_.end();
}

std::vector<std::pair<std::string, std::string>> SemanticMemory::getAllFacts() const {
    std::vector<std::pair<std::string, std::string>> result;
    result.reserve(facts_.size());
    for (const auto& fact : facts_) {
        result.emplace_back(fact.first, fact.second);
    }
    return result;
}

void SemanticMemory::clear() {
    facts_.clear();
    activations_.clear();
    NLM_LOG_INFO("SemanticMemory cleared");
}

void SemanticMemory::setActivation(const std::string& key, float activation) {
    if (hasFact(key)) {
        activations_[key] = std::clamp(activation, 0.0f, 1.0f);
    }
}

float SemanticMemory::getActivation(const std::string& key) const {
    auto it = activations_.find(key);
    if (it != activations_.end()) {
        return it->second;
    }
    return 0.0f;
}

} // namespace nlm