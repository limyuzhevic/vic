#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

// Curiosity drive: exploration motivation based on novelty and prediction error
// Computed as weighted combination of novelty and prediction error
class Curiosity {
public:
    Curiosity();
    ~Curiosity();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Get curiosity level
    float getLevel() const;
    
    // Get exploration drive (same as level)
    float getExplorationDrive() const;
    
    // Update curiosity based on novelty and prediction error
    void update(float novelty, float predictionError, TimestepDuration dt);
    
    // Set curiosity parameters
    void setNoveltyWeight(float weight);
    float getNoveltyWeight() const;
    
    void setPredictionErrorWeight(float weight);
    float getPredictionErrorWeight() const;
    
    // Get learning rate (affects how curiosity adapts)
    float getLearningRate() const;
    void setLearningRate(float rate);
    
    // Reset curiosity
    void reset();
    
    // Get curiosity history for analysis
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Check if curiosity is high (triggers exploration)
    bool shouldExplore(float threshold) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
