#pragma once

#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../prediction/PredictionSystem.hpp"
#include <memory>

namespace nlm {

// ==========================================
// PREDICTION ENGINE - Prediction Systems
// ==========================================
// 
// Manages: prediction system
// 
// Contains: PredictionSystem
// Handles: sensory prediction and error computation
// Manages: prediction error signals

class PredictionEngine {
public:
    PredictionEngine(std::shared_ptr<Config> config);
    ~PredictionEngine();
    
    // Disable copying, enable moving
    PredictionEngine(const PredictionEngine&) = delete;
    PredictionEngine& operator=(const PredictionEngine&) = delete;
    PredictionEngine(PredictionEngine&&) noexcept;
    PredictionEngine& operator=(PredictionEngine&&) noexcept;
    
    // Initialize prediction engine with brain reference
    bool initialize(Brain* brain);
    
    // Get prediction system
    class PredictionSystem* getPredictionSystem() { return predictionSystem_.get(); }
    const class PredictionSystem* getPredictionSystem() const { return predictionSystem_.get(); }
    
    // Get brain reference
    Brain* getBrain() { return brain_; }
    const Brain* getBrain() const { return brain_; }
    
    // Update prediction system
    void update(const class SensoryInput& input, TimestepDuration dt);
    
    // Get prediction error signal
    float getPredictionError() const;
    
    // Reset prediction engine
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    std::shared_ptr<Config> config_;
    
    std::unique_ptr<class PredictionSystem> predictionSystem_;
};

} // namespace nlm