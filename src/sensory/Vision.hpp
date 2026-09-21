#pragma once

#include "SensoryInput.hpp"

namespace nlm {

// Vision processing using NLM's neural machinery
// Implements biologically plausible visual processing pipeline:
// 1. Retinal preprocessing (center-surround filtering)
// 2. Feature extraction (edges, motion, orientation)
// 3. Neural enhancement through recurrent connections
// 4. Action potential generation for higher areas

class VisionProcessor {
public:
    VisionProcessor();
    ~VisionProcessor();
    
    // Process vision input
    void process(const Vision& input);
    
    // Get feature representation
    const std::vector<float>& getFeatures() const;
    
    // Get feature dimensionality
    size_t getFeatureDimensions() const;
    
    // Get neural firing rate pattern
    const std::vector<float>& getNeuralActivity() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper methods for neural processing
    void applyCenterSurroundFiltering(std::vector<float>& features);
    void extractVisualFeatures(std::vector<float>& features);
    void neuralEnhancement(std::vector<float>& features);
    void generateNeuralActivityPatterns();
    
    // Neural network integration
    std::shared_ptr<class RandomGenerator> brain_;
    
    friend class AgentBrain;  // Allow AgentBrain to access brain_
    
    // Initialize brain access
    void setBrainAccess(std::shared_ptr<class RandomGenerator> brain);
};

} // namespace nlm
