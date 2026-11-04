#include "Neuromodulator.hpp"
#include <algorithm>
#include <numeric>

namespace nlm {

FiringRateRegulator::FiringRateRegulator() {
    recentFiringRates_.reserve(historySize_);
}

void FiringRateRegulator::updateRegulation(float currentFiringRate, float targetRate, TimestepDuration dt) {
    (void)dt;
    
    // Add current firing rate to history
    recentFiringRates_.push_back(currentFiringRate);
    if (recentFiringRates_.size() > historySize_) {
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
        regulationState_ += regulationGain_ * deviation * dt;
        // Clamp regulation state
        regulationState_ = std::clamp(regulationState_, -1.0f, 1.0f);
    } else {
        // Gradually decay regulation state towards zero
        regulationState_ *= (1.0f - adaptationRate_ * dt);
    }
    
    // Convert regulation state to inhibition signal
    // Negative regulationState_ means less excitation (more inhibition)
    inhibitionSignal_ = -regulationState_ * 2.0f;  // Scale to reasonable range
    
    // Ensure inhibition is within reasonable bounds
    inhibitionSignal_ = std::clamp(inhibitionSignal_, -1.0f, 1.0f);
}

} // namespace nlm