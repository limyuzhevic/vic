#pragma once

#include "../core/Config/Config.hpp"
#include "../prediction/PredictionSystem.hpp"

namespace nlm {

class Brain;

class PredictionSystemManager {
public:
    PredictionSystemManager();
    ~PredictionSystemManager();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Update prediction system
    void update();
    
    // Get prediction system
    PredictionSystem* get() { return predictionSystem_.get(); }
    const PredictionSystem* get() const { return predictionSystem_.get(); }
    
    // Configure from config
    void configureFromConfig(const Config& config);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::unique_ptr<PredictionSystem> predictionSystem_;
};

} // namespace nlm