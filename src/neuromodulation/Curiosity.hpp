#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Forward declarations
class Brain;
class NeuralWorkingMemory;
class PredictionSystem;
class NeuralAttentionalSelection;
class NeuralPlanner;

// Curiosity drive: exploration motivation based on novelty and prediction error
// Extended with effects on working memory, learning rate, attention, and planning

class Curiosity {
public:
    Curiosity();
    ~Curiosity();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get curiosity level
    float getLevel() const;
    
    // Update curiosity based on novelty and prediction error
    void update(float novelty, float predictionError, TimestepDuration dt);
    
    // Get exploration drive (same as level)
    float getExplorationDrive() const;
    
    // Set curiosity parameters
    void setNoveltyWeight(float weight);
    void setPredictionErrorWeight(float weight);
    
    // Reset
    void reset();
    
    // Enhanced curiosity effects on cognitive systems
    void applyWorkingMemoryEnhancement(NeuralWorkingMemory* workingMemory, float deltaTime);
    void applyLearningRateEnhancement(PredictionSystem* predictionSystem, float deltaTime);
    void applyAttentionGuidance(NeuralAttentionalSelection* attention, float deltaTime);
    void applyPlanningDepthEnhancement(NeuralPlanner* planner, float deltaTime);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
