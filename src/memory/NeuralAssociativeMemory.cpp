#include "NeuralAssociativeMemory.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <unordered_set>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    Brain* brain = nullptr;
    
    // Create a hash key for two patterns
    std::string createKey(const std::vector<float>& pattern1, const std::vector<float>& pattern2) const {
        std::stringstream ss;
        ss << "[";
        for (size_t i = 0; i < pattern1.size(); ++i) {
            ss << pattern1[i];
            if (i < pattern1.size() - 1) ss << ",";
        }
        ss << "]-";
        for (size_t i = 0; i < pattern2.size(); ++i) {
            ss << pattern2[i];
            if (i < pattern2.size() - 1) ss << ";";
        }
        ss << "]";
        return ss.str();
    }
    
    std::string createPatternKey(const std::vector<float>& pattern) const {
        std::stringstream ss;
        ss << "[";
        for (size_t i = 0; i < pattern.size(); ++i) {
            ss << pattern[i];
            if (i < pattern.size() - 1) ss << ",";
        }
        ss << "]";
        return ss.str();
    }
    
    // Compute temporal proximity score between two episodes
    float computeTemporalProximity(const EpisodicMemoryItem& episode1, const EpisodicMemoryItem& episode2) const {
        float timeDiff = std::abs(static_cast<float>(episode1.timestamp - episode2.timestamp));
        // Exponential decay with time
        return std::exp(-timeDiff * 0.01f);
    }
    
    // Compute spatial proximity score
    float computeSpatialProximity(const EpisodicMemoryItem& episode1, const EpisodicMemoryItem& episode2) const {
        float dx = episode1.positionX - episode2.positionX;
        float dy = episode1.positionY - episode2.positionY;
        float dist = std::sqrt(dx * dx + dy * dy);
        return std::exp(-dist * 0.1f);
    }
};

NeuralAssociativeMemory::NeuralAssociativeMemory() : pImpl(std::make_unique<Impl>()) {}

NeuralAssociativeMemory::~NeuralAssociativeMemory() = default;

void NeuralAssociativeMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
}

void NeuralAssociativeMemory::associate(const std::vector<float>& patternA,
                                      const std::vector<float>& patternB,
                                      float strength) {
    if (patternA.empty() || patternB.empty()) return;
    
    std::string key = pImpl->createKey(patternA, patternB);
    
    // Update or create association
    if (associations_.find(key) != associations_.end()) {
        // Existing association - strengthen based on Hebbian rule
        float oldStrength = associations_[key];
        float newStrength = std::min(1.0f, oldStrength + strength * 0.1f);
        associations_[key] = newStrength;
    } else {
        // New association
        associations_[key] = strength;
    }
    
    // Also store reverse association (bidirectional)
    std::string reverseKey = pImpl->createKey(patternB, patternA);
    if (associations_.find(reverseKey) != associations_.end()) {
        associations_[reverseKey] = std::min(1.0f, associations_[reverseKey] + strength * 0.05f);
    } else {
        associations_[reverseKey] = strength * 0.5f;
    }
}

void NeuralAssociativeMemory::associateFromExperience(const EpisodicMemoryItem& episode) {
    if (!episode.sensoryState.empty() && !episode.resultingSensoryState.empty()) {
        // Associate current sensory state with resulting state
        associate(episode.sensoryState, episode.resultingSensoryState, 0.8f);
    }
    
    // Associate action with resulting state
    if (!episode.sensoryState.empty()) {
        // Create action-specific pattern
        std::vector<float> actionPattern(episode.sensoryState.size(), 0.0f);
        actionPattern[0] = static_cast<float>(episode.action) / 10.0f;
        
        associate(actionPattern, episode.resultingSensoryState, 0.7f);
    }
    
    // Associate location with action-outcome pairs
    std::vector<float> locationPattern{episode.positionX, episode.positionY, episode.orientation};
    if (!episode.resultingSensoryState.empty()) {
        associate(locationPattern, episode.resultingSensoryState, 0.6f);
    }
}

void NeuralAssociativeMemory::storeAssociation(const std::vector<float>& pattern1,
                                              const std::vector<float>& pattern2,
                                              float strength) {
    associate(pattern1, pattern2, strength);
}

std::vector<float> NeuralAssociativeMemory::getAssociation(const std::vector<float>& pattern) const {
    std::vector<float> bestMatch;
    float bestStrength = 0.0f;
    
    for (const auto& [key, strength] : associations_) {
        if (strength > bestStrength) {
            // Simple pattern matching - extract first pattern from key
            size_t dashPos = key.find('-');
            if (dashPos != std::string::npos) {
                std::string patternStr = key.substr(1, dashPos - 1);
                std::vector<float> candidate;
                std::stringstream ss(patternStr);
                std::string token;
                while (std::getline(ss, token, ',')) {
                    if (token.empty()) continue;
                    try {
                        candidate.push_back(std::stof(token));
                    } catch (...) {
                        // Skip invalid tokens
                    }
                }
                if (candidate.size() > bestMatch.size()) {
                    bestMatch = candidate;
                    bestStrength = strength;
                }
            }
        }
    }
    
    return bestMatch;
}

float NeuralAssociativeMemory::getAssociationStrength(const std::vector<float>& pattern1,
                                                      const std::vector<float>& pattern2) const {
    std::string key = pImpl->createKey(pattern1, pattern2);
    auto it = associations_.find(key);
    return (it != associations_.end()) ? it->second : 0.0f;
}

void NeuralAssociativeMemory::updateAssociation(const std::vector<float>& pattern1,
                                                const std::vector<float>& pattern2,
                                                float newStrength) {
    std::string key = pImpl->createKey(pattern1, pattern2);
    auto it = associations_.find(key);
    if (it != associations_.end()) {
        it->second = newStrength;
    }
}

void NeuralAssociativeMemory::clear() {
    associations_.clear();
}

std::unordered_map<std::string, float> NeuralAssociativeMemory::getAllAssociations() const {
    return associations_;
}

size_t NeuralAssociativeMemory::getAssociationCount() const {
    return associations_.size();
}

bool NeuralAssociativeMemory::hasAssociation(const std::vector<float>& pattern1,
                                            const std::vector<float>& pattern2) const {
    std::string key = pImpl->createKey(pattern1, pattern2);
    return associations_.find(key) != associations_.end();
}

std::vector<float> NeuralAssociativeMemory::retrieve(const std::vector<float>& input,
                                                     float threshold) const {
    std::vector<float> bestMatch;
    float bestStrength = 0.0f;
    
    for (const auto& [key, strength] : associations_) {
        if (strength > bestStrength && strength >= threshold) {
            // Extract pattern from key
            size_t dashPos = key.find('-');
            if (dashPos != std::string::npos) {
                std::string patternStr = key.substr(dashPos + 1);
                std::vector<float> candidate;
                std::stringstream ss(patternStr);
                std::string token;
                while (std::getline(ss, token, ',')) {
                    if (token.empty()) continue;
                    try {
                        candidate.push_back(std::stof(token));
                    } catch (...) {
                        // Skip invalid tokens
                    }
                }
                if (candidate.size() > bestMatch.size()) {
                    bestMatch = candidate;
                    bestStrength = strength;
                }
            }
        }
    }
    
    return bestMatch;
}

void NeuralAssociativeMemory::consolidate(float threshold) {
    // Remove weak associations
    for (auto it = associations_.begin(); it != associations_.end(); ) {
        if (it->second < threshold) {
            it = associations_.erase(it);
        } else {
            ++it;
        }
    }
}

size_t NeuralAssociativeMemory::memoryUsage() const {
    size_t total = sizeof(Impl);
    for (const auto& pair : associations_) {
        total += pair.first.size();
    }
    return total;
}

} // namespace nlm
