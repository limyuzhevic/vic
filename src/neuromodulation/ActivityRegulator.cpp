#include <algorithm>
#include <numeric>
#include <cmath>

namespace nlm {

class ActivityRegulator {
public:
    ActivityRegulator() {
        recentFiringRates_.reserve(10);
        inhibitionSignals_.reserve(10);
    }
    
    // Update activity regulation based on current firing rate
    void update(float currentFiringRate, float targetRate = 10.0f, double dt = 0.001) {
        // Add current firing rate to history
        recentFiringRates_.push_back(currentFiringRate);
        if (recentFiringRates_.size() > 10) {
            recentFiringRates_.erase(recentFiringRates_.begin());
        }
        
        // Calculate average firing rate from recent history
        float avgFiringRate = 0.0f;
        if (!recentFiringRates_.empty()) {
            avgFiringRate = std::accumulate(recentFiringRates_.begin(), 
                                          recentFiringRates_.end(), 0.0f) / 
                          static_cast<float>(recentFiringRates_.size());
        }
        
        // Calculate deviation from target
        float deviation = avgFiringRate - targetRate;
        
        // Update regulation state based on deviation
        if (std::abs(deviation) > targetRate * 0.1f) {  // If deviation > 10% of target
            // Stronger regulation when deviation is large
            regulationState_ += 0.1f * deviation * static_cast<float>(dt);
            // Clamp regulation state
            regulationState_ = std::clamp(regulationState_, -1.0f, 1.0f);
        } else {
            // Gradually decay regulation state towards zero
            regulationState_ *= (1.0f - 0.01f * static_cast<float>(dt));
        }
        
        // Convert regulation state to inhibition signal
        // Negative regulationState_ means less excitation (more inhibition)
        inhibitionSignal_ = -regulationState_ * 2.0f;  // Scale to reasonable range
        
        // Ensure inhibition is within reasonable bounds
        inhibitionSignal_ = std::clamp(inhibitionSignal_, -1.0f, 1.0f);
        
        // Store in history for tracking
        if (inhibitionSignals_.size() > 10) {
            inhibitionSignals_.erase(inhibitionSignals_.begin());
        }
        inhibitionSignals_.push_back(inhibitionSignal_);
    }
    
    // Get current inhibition signal
    float getInhibitionSignal() const { return inhibitionSignal_; }
    
    // Get regulation state
    float getRegulationState() const { return regulationState_; }
    
    // Reset regulator
    void reset() {
        inhibitionSignal_ = 0.0f;
        regulationState_ = 0.0f;
        recentFiringRates_.clear();
        inhibitionSignals_.clear();
    }
    
    // Check if regulation is active
    bool isActive() const {
        return std::abs(regulationState_) > 0.01f;
    }
    
private:
    float inhibitionSignal_ = 0.0f;
    float regulationState_ = 0.0f;
    std::vector<float> recentFiringRates_;
    std::vector<float> inhibitionSignals_;
};

} // namespace nlm