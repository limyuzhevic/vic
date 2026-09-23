#include "NeuralAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <random>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    Brain* brain;
    size_t capacity;
    float learningRate;
    float interferenceThreshold;
    
    // Association storage
    std::unordered_map<size_t, std::vector<float>> patterns;
    std::unordered_map<size_t, std::unordered_map<size_t, float>> associations;
    std::unordered_map<size_t, float> accessTimes;
    
    // Similarity cache
    std::unordered_map<size_t, std::vector<std::pair<size_t, float>>> similarityCache;
    
    // Random generator for pattern generation
    std::mt19937 rng;
    std::uniform_real_distribution<float> dist;
    
    Impl() : brain(nullptr), capacity(1000), learningRate(0.01f), 
             interferenceThreshold(0.8f), rng(std::random_device{}()), dist(0.0f, 1.0f) {}
};

NeuralAssociativeMemory::NeuralAssociativeMemory() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("Associative memory system created");
}

NeuralAssociativeMemory::~NeuralAssociativeMemory() = default;

void NeuralAssociativeMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Associative memory system initialized");
}

bool NeuralAssociativeMemory::createAssociation(const std::vector<float>& pattern1, 
                                                const std::vector<float>& pattern2,
                                                float strength) {
    if (pattern1.empty() || pattern2.empty()) {
        return false;
    }
    
    size_t hash1 = patternHash(pattern1);
    size_t hash2 = patternHash(pattern2);
    
    // Check if patterns already exist
    if (pImpl->patterns.find(hash1) == pImpl->patterns.end()) {
        pImpl->patterns[hash1] = pattern1;
    }
    if (pImpl->patterns.find(hash2) == pImpl->patterns.end()) {
        pImpl->patterns[hash2] = pattern2;
    }
    
    // Add association
    pImpl->associations[hash1][hash2] = strength;
    pImpl->associations[hash2][hash1] = strength;
    
    // Update access time
    pImpl->accessTimes[hash1] = pImpl->accessTimes[hash2] = 0.0f;
    
    // Manage capacity
    manageCapacity();
    
    return true;
}

std::vector<float> NeuralAssociativeMemory::retrieveAssociation(const std::vector<float>& pattern,
                                                                 float threshold) const {
    if (pattern.empty()) {
        return {};
    }
    
    size_t hash = patternHash(pattern);
    auto it = pImpl->associations.find(hash);
    if (it == pImpl->associations.end()) {
        return {};
    }
    
    // Find associated patterns with strength above threshold
    std::vector<std::pair<size_t, float>> strongAssociations;
    for (const auto& assoc : it->second) {
        if (assoc.second >= threshold) {
            strongAssociations.push_back(assoc);
        }
    }
    
    if (strongAssociations.empty()) {
        return {};
    }
    
    // Average the associated patterns
    std::vector<float> result(pattern.size(), 0.0f);
    float totalWeight = 0.0f;
    
    for (const auto& assoc : strongAssociations) {
        auto patternIt = pImpl->patterns.find(assoc.first);
        if (patternIt != pImpl->patterns.end()) {
            float weight = assoc.second;
            for (size_t i = 0; i < pattern.size() && i < patternIt->second.size(); ++i) {
                result[i] += patternIt->second[i] * weight;
            }
            totalWeight += weight;
        }
    }
    
    if (totalWeight > 0.0f) {
        for (size_t i = 0; i < result.size(); ++i) {
            result[i] /= totalWeight;
        }
    }
    
    return result;
}

bool NeuralAssociativeMemory::hasAssociation(const std::vector<float>& pattern) const {
    size_t hash = patternHash(pattern);
    return pImpl->associations.find(hash) != pImpl->associations.end();
}

float NeuralAssociativeMemory::getAssociationStrength(const std::vector<float>& pattern1,
                                                     const std::vector<float>& pattern2) const {
    size_t hash1 = patternHash(pattern1);
    size_t hash2 = patternHash(pattern2);
    
    auto it1 = pImpl->associations.find(hash1);
    if (it1 == pImpl->associations.end()) {
        return 0.0f;
    }
    
    auto it2 = it1->second.find(hash2);
    if (it2 == it1->second.end()) {
        return 0.0f;
    }
    
    return it2->second;
}

bool NeuralAssociativeMemory::removeAssociation(const std::vector<float>& pattern1,
                                               const std::vector<float>& pattern2) {
    size_t hash1 = patternHash(pattern1);
    size_t hash2 = patternHash(pattern2);
    
    auto it1 = pImpl->associations.find(hash1);
    if (it1 == pImpl->associations.end()) {
        return false;
    }
    
    auto it2 = it1->second.find(hash2);
    if (it2 == it1->second.end()) {
        return false;
    }
    
    it1->second.erase(it2);
    
    // Also remove reverse association
    auto it3 = pImpl->associations.find(hash2);
    if (it3 != pImpl->associations.end()) {
        it3->second.erase(hash1);
    }
    
    return true;
}

void NeuralAssociativeMemory::clear() {
    pImpl->patterns.clear();
    pImpl->associations.clear();
    pImpl->accessTimes.clear();
    pImpl->similarityCache.clear();
    NLM_LOG_INFO("Associative memory cleared");
}

std::vector<std::vector<float>> NeuralAssociativeMemory::getAllPatterns() const {
    std::vector<std::vector<float>> allPatterns;
    allPatterns.reserve(pImpl->patterns.size());
    
    for (const auto& pair : pImpl->patterns) {
        allPatterns.push_back(pair.second);
    }
    
    return allPatterns;
}

size_t NeuralAssociativeMemory::patternHash(const std::vector<float>& pattern) const {
    std::hash<float> hasher;
    size_t hash = 0;
    for (float val : pattern) {
        hash ^= hasher(val) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }
    return hash;
}

void NeuralAssociativeMemory::update(TimestepDuration dt) {
    // Update access times
    float decay = std::exp(-dt / 1000.0f);  // Decay over time
    for (auto& pair : pImpl->accessTimes) {
        pair.second *= decay;
    }
    
    // Consolidate weak associations
    consolidateWeakAssociations(0.1f);
    
    // Manage capacity
    manageCapacity();
}

void NeuralAssociativeMemory::manageCapacity() {
    if (pImpl->patterns.size() <= pImpl->capacity) {
        return;
    }
    
    // Remove least recently used associations
    std::vector<std::pair<size_t, float>> accessList;
    for (const auto& pair : pImpl->accessTimes) {
        accessList.emplace_back(pair.first, pair.second);
    }
    
    std::sort(accessList.begin(), accessList.end(),
              [](const auto& a, const auto& b) { return a.second < b.second; });
    
    size_t toRemove = pImpl->patterns.size() - pImpl->capacity;
    for (size_t i = 0; i < toRemove && i < accessList.size(); ++i) {
        size_t hashToRemove = accessList[i].first;
        
        // Remove all associations involving this pattern
        auto it = pImpl->associations.find(hashToRemove);
        if (it != pImpl->associations.end()) {
            for (const auto& assoc : it->second) {
                auto revIt = pImpl->associations.find(assoc.first);
                if (revIt != pImpl->associations.end()) {
                    revIt->second.erase(hashToRemove);
                }
            }
            pImpl->associations.erase(it);
        }
        
        pImpl->patterns.erase(hashToRemove);
        pImpl->accessTimes.erase(hashToRemove);
        pImpl->similarityCache.erase(hashToRemove);
    }
}

bool NeuralAssociativeMemory::wouldExceedInterference(const std::vector<float>& pattern1,
                                                     const std::vector<float>& pattern2) const {
    size_t hash1 = patternHash(pattern1);
    size_t hash2 = patternHash(pattern2);
    
    float totalSimilarity = 0.0f;
    
    // Check for similar patterns
    auto similar1 = getSimilarPatterns(pattern1);
    auto similar2 = getSimilarPatterns(pattern2);
    
    for (const auto& sim1 : similar1) {
        for (const auto& sim2 : similar2) {
            if (sim1.first != sim2.first) {
                totalSimilarity += sim1.second * sim2.second;
            }
        }
    }
    
    return totalSimilarity > interferenceThreshold_;
}

void NeuralAssociativeMemory::consolidateWeakAssociations(float threshold) {
    for (auto& pair : pImpl->associations) {
        std::unordered_map<size_t, float>& associations = pair.second;
        
        std::vector<size_t> weakToRemove;
        for (const auto& assoc : associations) {
            if (assoc.second < threshold) {
                weakToRemove.push_back(assoc.first);
            }
        }
        
        for (size_t weakHash : weakToRemove) {
            associations.erase(weakHash);
            
            // Also remove reverse association
            auto revIt = pImpl->associations.find(weakHash);
            if (revIt != pImpl->associations.end()) {
                revIt->second.erase(pair.first);
            }
        }
    }
}

void NeuralAssociativeMemory::updatePatternRecency(const std::vector<float>& pattern) {
    size_t hash = patternHash(pattern);
    if (pImpl->accessTimes.find(hash) != pImpl->accessTimes.end()) {
        pImpl->accessTimes[hash] = 0.0f;  // Reset to most recent
    }
}

float NeuralAssociativeMemory::getPatternLastAccess(const std::vector<float>& pattern) const {
    size_t hash = patternHash(pattern);
    auto it = pImpl->accessTimes.find(hash);
    if (it != pImpl->accessTimes.end()) {
        return it->second;
    }
    return -1.0f;
}

std::vector<std::pair<size_t, float>> NeuralAssociativeMemory::getSimilarPatterns(const std::vector<float>& pattern) const {
    size_t hash = patternHash(pattern);
    
    auto it = pImpl->similarityCache.find(hash);
    if (it != pImpl->similarityCache.end()) {
        return it->second;
    }
    
    std::vector<std::pair<size_t, float>> similarities;
    float maxSimilarity = 0.0f;
    
    for (const auto& pair : pImpl->patterns) {
        if (pair.first == hash) continue;
        
        float similarity = calculatePatternSimilarity(pattern, pair.second);
        if (similarity > 0.01f) {
            similarities.emplace_back(pair.first, similarity);
            if (similarity > maxSimilarity) maxSimilarity = similarity;
        }
    }
    
    // Normalize similarities
    if (maxSimilarity > 0.0f) {
        for (auto& sim : similarities) {
            sim.second /= maxSimilarity;
        }
    }
    
    pImpl->similarityCache[hash] = similarities;
    return similarities;
}

float NeuralAssociativeMemory::calculatePatternSimilarity(const std::vector<float>& pattern1,
                                                         const std::vector<float>& pattern2) const {
    if (pattern1.size() != pattern2.size()) {
        return 0.0f;
    }
    
    float dotProduct = 0.0f;
    float norm1 = 0.0f;
    float norm2 = 0.0f;
    
    for (size_t i = 0; i < pattern1.size(); ++i) {
        dotProduct += pattern1[i] * pattern2[i];
        norm1 += pattern1[i] * pattern1[i];
        norm2 += pattern2[i] * pattern2[i];
    }
    
    norm1 = std::sqrt(norm1);
    norm2 = std::sqrt(norm2);
    
    if (norm1 == 0.0f || norm2 == 0.0f) {
        return 0.0f;
    }
    
    return std::abs(dotProduct / (norm1 * norm2));
}

} // namespace nlm