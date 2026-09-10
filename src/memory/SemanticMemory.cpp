#include "SemanticMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct SemanticMemory::Impl {
    Brain* brain;
    
    // Fact activation levels
    std::vector<float> activationLevels;
    
    // Related fact indices for spreading activation
    std::vector<std::vector<size_t>> relatedIndices;
    
    Impl() : brain(nullptr) {}
};

SemanticMemory::SemanticMemory()
    : pImpl(new Impl)
    , brain_(nullptr) {
}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("SemanticMemory initialized");
}

void SemanticMemory::storeFact(const std::string& key, const std::string& value, float confidence) {
    ensureFactEntry(key);
    
    size_t index = 0;
    for (size_t i = 0; i < facts_.size(); ++i) {
        if (facts_[i].first == key) {
            index = i;
            break;
        }
    }
    
    facts_[index].second = value;
    confidences_[index] = std::clamp(confidence, 0.0f, 1.0f);
    
    // Update related facts if key already existed
    if (!relatedFacts_[index].empty()) {
        for (size_t relatedIdx : relatedFacts_[index]) {
            if (relatedIdx < confidences_.size()) {
                confidences_[relatedIdx] = std::min(confidences_[relatedIdx], confidence);
            }
        }
    }
}

std::string SemanticMemory::retrieveFact(const std::string& key) const {
    for (size_t i = 0; i < facts_.size(); ++i) {
        if (facts_[i].first == key) {
            return facts_[i].second;
        }
    }
    return "";
}

bool SemanticMemory::hasFact(const std::string& key) const {
    for (const auto& fact : facts_) {
        if (fact.first == key) {
            return true;
        }
    }
    return false;
}

std::vector<std::pair<std::string, std::string>> SemanticMemory::getAllFacts() const {
    return facts_;
}

float SemanticMemory::getFactConfidence(const std::string& key) const {
    for (size_t i = 0; i < facts_.size(); ++i) {
        if (facts_[i].first == key) {
            return confidences_[i];
        }
    }
    return 0.0f;
}

void SemanticMemory::updateFact(const std::string& key, const std::string& value) {
    ensureFactEntry(key);
    
    for (auto& fact : facts_) {
        if (fact.first == key) {
            fact.second = value;
            break;
        }
    }
}

void SemanticMemory::clear() {
    facts_.clear();
    confidences_.clear();
    relatedFacts_.clear();
    pImpl->activationLevels.clear();
    pImpl->relatedIndices.clear();
}

void SemanticMemory::ensureFactEntry(const std::string& key) {
    for (size_t i = 0; i < facts_.size(); ++i) {
        if (facts_[i].first == key) {
            return;
        }
    }
    
    facts_.emplace_back(key, "");
    confidences_.push_back(1.0f);
    pImpl->activationLevels.push_back(0.0f);
    relatedFacts_.emplace_back();
    pImpl->relatedIndices.emplace_back();
}

} // namespace nlm