#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <algorithm>
#include <numeric>
#include <cmath>

namespace nlm {

// Information gain: measures reduction in uncertainty about the world
// Computed from prediction error distributions and novelty
class InformationGainCalculator {
public:
    InformationGainCalculator() : entropyThreshold_(0.5f), maxGain_(1.0f) {}
    
    // Calculate information gain from sensory prediction errors
    float calculateGain(const std::vector<float>& predictionErrors,
                       const std::vector<float>& novelties,
                       TimestepDuration dt) const;
    
    // Get entropy of the prediction error distribution
    float calculateEntropy(const std::vector<float>& errors) const;
    
    // Determine if new information is worth exploring
    bool isInformationGainWorthExploring(float gain, float currentUncertainty) const;
    
    // Set gain calculation parameters
    void setEntropyThreshold(float threshold) { entropyThreshold_ = threshold; }
    void setMaxGain(float max) { maxGain_ = max; }
    
private:
    float entropyThreshold_;
    float maxGain_;
};

// Intrinsic motivation system based on exploration-exploitation trade-off
class IntrinsicMotivation {
public:
    IntrinsicMotivation();
    
    // Calculate intrinsic motivation level
    float calculateMotivation(const std::vector<float>& stateEntropy,
                             const std::vector<float>& predictionAccuracy,
                             TimestepDuration dt);
    
    // Get exploration bonus based on uncertainty
    float getExplorationBonus(float uncertainty) const;
    
    // Update motivation state
    void update(float currentMotivation, TimestepDuration dt);
    
    // Get current motivation level
    float getMotivationLevel() const { return motivationLevel_; }
    
    // Reset motivation
    void reset();
    
    // Configure intrinsic motivation parameters
    void setExplorationRate(float rate) { explorationRate_ = rate; }
    void setUncertaintySensitivity(float sensitivity) { uncertaintySensitivity_ = sensitivity; }
    void setDecayRate(float rate) { decayRate_ = rate; }
    
private:
    float motivationLevel_;
    float explorationRate_;
    float uncertaintySensitivity_;
    float decayRate_;
    
    // Historical motivation for smoothing
    std::deque<float> motivationHistory_;
    static constexpr size_t MOTIVATION_HISTORY_SIZE = 10;
};

// Enhanced curiosity system with information gain and intrinsic motivation
class EnhancedCuriosity {
public:
    EnhancedCuriosity();
    ~EnhancedCuriosity();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Update curiosity based on multiple signals
    void update(float novelty, float predictionError,
               const std::vector<float>& predictionErrors,
               const std::vector<float>& sensoryEntropy,
               TimestepDuration dt);
    
    // Get total curiosity level (weighted combination)
    float getLevel() const;
    
    // Get information gain component
    float getInformationGain() const { return informationGain_; }
    
    // Get intrinsic motivation component
    float getIntrinsicMotivation() const { return intrinsicMotivation_.getMotivationLevel(); }
    
    // Get exploration drive (combined)
    float getExplorationDrive() const;
    
    // Get uncertainty estimate for exploration
    float getUncertaintyEstimate() const { return uncertaintyEstimate_; }
    
    // Reset curiosity
    void reset();
    
    // Configure curiosity parameters
    void setNoveltyWeight(float weight) { noveltyWeight_ = weight; }
    void setPredictionErrorWeight(float weight) { predictionErrorWeight_ = weight; }
    void setInformationGainWeight(float weight) { informationGainWeight_ = weight; }
    void setIntrinsicMotivationWeight(float weight) { intrinsicMotivationWeight_ = weight; }
    
private:
    // Components
    InformationGainCalculator infoGainCalculator_;
    IntrinsicMotivation intrinsicMotivation_;
    
    // Current brain reference
    class Brain* brain_;
    
    // Current state
    float level_;
    float informationGain_;
    float uncertaintyEstimate_;
    
    // Weights for different components
    float noveltyWeight_;
    float predictionErrorWeight_;
    float informationGainWeight_;
    float intrinsicMotivationWeight_;
    
    // Adaptive exploration parameters
    float explorationRate_;
    float minExplorationRate_;
    float maxExplorationRate_;
    
    // Historical values for smoothing
    std::deque<float> curiosityHistory_;
    static constexpr size_t CURIOSITY_HISTORY_SIZE = 20;
};

} // namespace nlm
