#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <unordered_map>

namespace nlm {

// MetaLearning: Learning to learn mechanisms
// Implements meta-learning capabilities where the brain learns how to learn
// more efficiently and adaptively based on experience
// 
// This mechanism enables:
// - Adaptive learning rates based on performance
// - Meta-plasticities that modulate learning processes
// - Transfer learning across related tasks
// - Curriculum learning and progressive complexity
// - Learning strategy optimization
// - Fast adaptation to new environments
// - Meta-memory for learning patterns
//
// The meta-learning framework consists of:
// 1. Learning rate adaptation: Adjusting how fast the brain learns
// 2. Transfer learning: Applying learned strategies to new tasks
// 3. Curriculum learning: Progressive complexity introduction
// 4. Meta-memory: Storing learning patterns and strategies
// 5. Meta-plasticities: Higher-level plasticity control

class MetaLearning {
public:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    MetaLearning();
    ~MetaLearning();
    
    // Initialize with brain reference and meta-learning parameters
    void initialize(class Brain* brain, float initialLearningRate = 0.01f,
                    size_t memorySize = 1000);
    
    // Update meta-learning based on learning performance
    void update(float learningPerformance, const std::vector<float>& state);
    
    // Get current adaptive learning rate
    float getAdaptiveLearningRate() const;
    
    // Get learning strategy recommendations
    std::vector<float> getLearningStrategy(const std::vector<float>& state) const;
    
    // Get meta-learning statistics
    float getLearningEfficiency() const;
    size_t getMetaMemoryUsage() const;
    float getTransferLearningScore() const;
    
    // Get meta-memory contents
    const std::vector<std::vector<float>>& getMetaMemory() const;
    
    // Store learning pattern in meta-memory
    void storeLearningPattern(const std::vector<float>& pattern, float performance);
    
    // Retrieve best learning pattern for current state
    std::vector<float> retrieveLearningPattern(const std::vector<float>& state) const;
    
    // Enable/disable meta-learning
    void enable(bool enable);
    bool isEnabled() const;
    
    // Configure meta-learning parameters
    void setInitialLearningRate(float rate);
    void setMemorySize(size_t size);
    void setTransferThreshold(float threshold);
    
    // Learning strategy management
    void setLearningStrategy(const std::string& strategy);
    void enableTransferLearning(bool enable);
    void enableCurriculumLearning(bool enable);
    
    // Get learning progress metrics
    float getLearningProgress() const;
    size_t getSamplesProcessed() const;
    float getAdaptationSpeed() const;
    
private:
    MetaLearning(const MetaLearning&) = delete;
    MetaLearning& operator=(const MetaLearning&) = delete;
};

} // namespace nlm