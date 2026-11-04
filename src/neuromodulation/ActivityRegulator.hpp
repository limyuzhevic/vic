#pragma once

namespace nlm {

class ActivityRegulator {
public:
    ActivityRegulator();
    
    // Update activity regulation based on current firing rate
    void update(float currentFiringRate, float targetRate = 10.0f, double dt = 0.001);
    
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