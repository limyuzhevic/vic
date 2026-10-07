#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

namespace nlm {

// Forward declarations
class Brain;

// Associative memory - stores pattern associations and relationships
class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();
    
    // Disable copying
    NeuralAssociativeMemory(const NeuralAssociativeMemory&) = delete;
    NeuralAssociativeMemory& operator=(const NeuralAssociativeMemory&) = delete;
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Store an association between two patterns
    void storeAssociation(const std::vector<float>& pattern1, 
                         const std::vector<float>& pattern2,
                         float strength = 1.0f);
    
    // Retrieve associated patterns
    std::vector<float> getAssociation(const std::vector<float>& pattern) const;
    
    // Get strength of association
    float getAssociationStrength(const std::vector<float>& pattern1,
                                 const std::vector<float>& pattern2) const;
    
    // Update association strength
    void updateAssociation(const std::vector<float>& pattern1,
                          const std::vector<float>& pattern2,
                          float newStrength);
    
    // Clear all associations
    void clear();
    
    // Get all associations
    std::unordered_map<std::string, float> getAllAssociations() const;
    
    // Get association count
    size_t getAssociationCount() const;
    
    // Check if an association exists
    bool hasAssociation(const std::vector<float>& pattern1,
                       const std::vector<float>& pattern2) const;
    
    // Get associated pattern for similar input
    std::vector<float> retrieve(const std::vector<float>& input,
                               float threshold = 0.5f) const;
    
    // Consolidate similar associations
    void consolidate(float threshold = 0.3f);
    
    // Get memory usage
    size_t memoryUsage() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm