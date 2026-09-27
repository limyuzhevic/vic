#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Novelty.hpp"
#include <vector>
#include <memory>

namespace nlm {

// AttentionalSelection: Gating mechanism for memory storage and processing
// Determines which experiences are worth storing and processing
// NOT a full attention mechanism - a selective gating system for memory

class AttentionalSelection {
public:
    AttentionalSelection();
    ~AttentionalSelection();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Check if a neural event should be attended to
    // Returns attention score (0.0 to 1.0)
    float evaluateEvent(const std::vector<float>& neuralActivations,
                       float novelty,
                       float predictionError,
                       float timeSinceLastEpisode,
                       bool isResting);
    
    // Update attention based on neuromodulation signals
    void update(float curiosity,
                float predictionError,
                float novelty,
                TimestepDuration dt);
    
    // Get current attention level
    float getAttentionLevel() const;
    
    // Set attention parameters
    void setAttentionThreshold(float threshold);
    void setNoveltyWeight(float weight);
    void setPredictionErrorWeight(float weight);
    void setTimePressureWeight(float weight);
    void setRestingThreshold(float threshold);
    
    // Check if should enter resting state
    bool shouldEnterResting(const std::vector<float>& neuralActivations,
                           float averageFiringRate,
                           float timeSinceLastEpisode);
    
    // Check if should exit resting state
    bool shouldExitResting(float timeInResting,
                         float accumulatedStress,
                         float averageFiringRate);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
