#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <cmath>

namespace nlm {

// NeuralSemanticMemory: Long-term knowledge storage
// Represents concepts, facts, and semantic relationships
struct SemanticFact {
    std::string key;
    std::string value;
    SimulationStep timestamp;
    float relevance;
    
    SemanticFact(const std::string& k = "", const std::string& v = "", SimulationStep t = 0)
        : key(k), value(v), timestamp(t), relevance(1.0f) {}
};

class NeuralSemanticMemory {
public:
    NeuralSemanticMemory();
    ~NeuralSemanticMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Store a semantic fact/knowledge
    void storeFact(const std::string& key, const std::string& value);
    
    // Retrieve a semantic fact
    std::string retrieveFact(const std::string& key) const;
    
    // Check if fact exists
    bool hasFact(const std::string& key) const;
    
    // Get all facts
    std::vector<std::pair<std::string, std::string>> getAllFacts() const;
    
    // Get facts by relevance
    std::vector<std::pair<std::string, std::string>> getRelevantFacts(float threshold) const;
    
    // Update fact relevance based on usage
    void updateFactRelevance(const std::string& key, float delta);
    
    // Clear all facts
    void clear();
    
    // Get statistics
    size_t getFactCount() const { return facts_.size(); }
    size_t getActiveFactCount() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Legacy interface for Brain.cpp
    friend class Brain;
    
    Brain* brain_;
    
    // Legacy data structures for compatibility
    std::unordered_map<std::string, SemanticFact> facts_;
    float capacity_;
    
    // Helper methods
    void updateRelevanceBasedOnUsage();
};

} // namespace nlm
