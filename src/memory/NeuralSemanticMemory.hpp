#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <algorithm>

namespace nlm {

class SemanticMemory {
public:
    SemanticMemory();
    ~SemanticMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Store a pattern and associate it with a concept
    void storeConcept(const std::string& conceptName, const std::vector<float>& pattern);
    
    // Retrieve a concept by name
    std::vector<float> retrieveConcept(const std::string& conceptName) const;
    
    // Get all concept names
    std::vector<std::string> getConceptNames() const;
    
    // Update concept based on neural activity patterns
    void updateConcepts(const std::vector<std::vector<float>>& patterns);
    
    // Find concepts similar to a pattern (clustering)
    std::vector<std::string> findSimilarConcepts(const std::vector<float>& pattern, float threshold = 0.7f) const;
    
    // Get concept statistics
    float getConceptStrength(const std::string& conceptName) const;
    void setConceptStrength(const std::string& conceptName, float strength);
    
    // Clear all concepts
    void clear();
    
    // Get number of stored concepts
    size_t getConceptCount() const;
    
    // Decay concepts over time
    void decayConcepts(float decayRate = 0.01f);
    
private:
    struct Concept {
        std::string name;
        std::vector<float> pattern;
        float strength;
        size_t lastUpdated;
        float coherence;
    };
    
    std::vector<Concept> concepts_;
    Brain* brain_;
    std::unordered_map<std::string, size_t> nameToIndex_;
};

} // namespace nlm