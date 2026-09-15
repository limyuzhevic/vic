#pragma once

#include "../core/Types/Types.hpp"
#include <unordered_map>
#include <string>

namespace nlm {

// SemanticMemory: Gradually acquired knowledge - neural patterns representing concepts
class NeuralSemanticMemory {
public:
    NeuralSemanticMemory();
    ~NeuralSemanticMemory();
    
    // Store neural pattern for a concept
    void storeConcept(const std::string& conceptName, const std::vector<float>& pattern, float strength = 1.0f);
    
    // Retrieve pattern for a concept
    std::vector<float> retrievePattern(const std::string& conceptName) const;
    
    // Check if concept exists
    bool hasConcept(const std::string& conceptName) const;
    
    // Get all concepts
    std::vector<std::string> getAllConcepts() const;
    
    // Get concept strength
    float getConceptStrength(const std::string& conceptName) const;
    
    // Update concept strength
    void updateConceptStrength(const std::string& conceptName, float delta);
    
    // Clear all
    void clear();
    
    // Initialize with brain
    void initialize(Brain* brain);
    
    // Update with time (for consolidation)
    void update(TimestepDuration dt);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
