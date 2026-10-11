#include "SemanticMemory.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <ctime>

namespace nlm {

struct SemanticMemory::Impl {
    struct Fact {
        std::string key;
        std::string value;
        SimulationStep timestamp;
        float confidence;
        
        Fact() : timestamp(0), confidence(0.8f) {}
        Fact(const std::string& k, const std::string& v) 
            : key(k), value(v), timestamp(0), confidence(0.8f) {}
    };
    
    std::unordered_map<std::string, Fact> facts;
    SimulationStep nextTimestamp;
    
    Impl() : nextTimestamp(0) {}
};

SemanticMemory::SemanticMemory() : pImpl(new Impl), brain_(nullptr) {}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("SemanticMemory initialized");
}

void SemanticMemory::storeFact(const std::string& key, const std::string& value) {
    if (key.empty()) return;
    
    auto it = pImpl->facts.find(key);
    if (it != pImpl->facts.end()) {
        // Update existing fact
        it->second.value = value;
        it->second.confidence = std::min(1.0f, it->second.confidence + 0.1f);
        it->second.timestamp = pImpl->nextTimestamp++;
    } else {
        // Create new fact
        pImpl->facts[key] = Impl::Fact(key, value);
        pImpl->facts[key].timestamp = pImpl->nextTimestamp++;
    }
    
    NLM_LOG_INFO("Stored fact: " + key + " = " + value);
}

std::string SemanticMemory::retrieveFact(const std::string& key) const {
    auto it = pImpl->facts.find(key);
    if (it != pImpl->facts.end()) {
        return it->second.value;
    }
    return "";
}

bool SemanticMemory::hasFact(const std::string& key) const {
    return pImpl->facts.find(key) != pImpl->facts.end();
}

std::vector<std::pair<std::string, std::string>> SemanticMemory::getAllFacts() const {
    std::vector<std::pair<std::string, std::string>> result;
    result.reserve(pImpl->facts.size());
    
    for (const auto& pair : pImpl->facts) {
        result.push_back(std::make_pair(pair.first, pair.second.value));
    }
    
    return result;
}

void SemanticMemory::clear() {
    pImpl->facts.clear();
    NLM_LOG_INFO("SemanticMemory cleared");
}

size_t SemanticMemory::getFactCount() const {
    return pImpl->facts.size();
}

void SemanticMemory::updateFact(const std::string& key, const std::string& newValue) {
    auto it = pImpl->facts.find(key);
    if (it != pImpl->facts.end()) {
        it->second.value = newValue;
        it->second.confidence = std::min(1.0f, it->second.confidence + 0.2f);
        it->second.timestamp = pImpl->nextTimestamp++;
    }
}

void SemanticMemory::removeFact(const std::string& key) {
    pImpl->facts.erase(key);
}

std::vector<std::pair<std::string, std::string>> SemanticMemory::search(const std::string& keyword) const {
    std::vector<std::pair<std::string, std::string>> results;
    
    for (const auto& pair : pImpl->facts) {
        if (pair.first.find(keyword) != std::string::npos || 
            pair.second.value.find(keyword) != std::string::npos) {
            results.push_back(std::make_pair(pair.first, pair.second.value));
        }
    }
    
    return results;
}

std::vector<std::pair<std::string, std::string>> SemanticMemory::getRecentFacts(size_t maxAge) const {
    std::vector<std::pair<std::string, std::string>> results;
    
    SimulationStep currentTime = pImpl->nextTimestamp;
    for (const auto& pair : pImpl->facts) {
        if (currentTime - pair.second.timestamp <= maxAge) {
            results.push_back(std::make_pair(pair.first, pair.second.value));
        }
    }
    
    return results;
}

float SemanticMemory::getFactConfidence(const std::string& key) const {
    auto it = pImpl->facts.find(key);
    if (it != pImpl->facts.end()) {
        return it->second.confidence;
    }
    return 0.0f;
}

void SemanticMemory::updateFactConfidence(const std::string& key, float delta) {
    auto it = pImpl->facts.find(key);
    if (it != pImpl->facts.end()) {
        it->second.confidence = std::clamp(it->second.confidence + delta, 0.0f, 1.0f);
    }
}

SimulationStep SemanticMemory::getFactTimestamp(const std::string& key) const {
    auto it = pImpl->facts.find(key);
    if (it != pImpl->facts.end()) {
        return it->second.timestamp;
    }
    return 0;
}

} // namespace nlm
