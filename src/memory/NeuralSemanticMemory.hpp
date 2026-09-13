#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <unordered_map>

namespace nlm {

// Semantic memory: gradually acquired knowledge
class NeuralSemanticMemory {
public:
    NeuralSemanticMemory();
    ~NeuralSemanticMemory();
    
    // Store fact
    void storeFact(const std::string& key, const std::string& value);
    
    // Retrieve fact
    std::string retrieveFact(const std::string& key) const;
    
    // Check if fact exists
    bool hasFact(const std::string& key) const;
    
    // Get all facts
    std::vector<std::pair<std::string, std::string>> getAllFacts() const;
    
    // Clear all
    void clear();
    
    // Initialize with brain reference
    void initialize(class Brain* brain) {}
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm