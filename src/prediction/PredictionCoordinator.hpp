#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../prediction/PredictionSystem.hpp"
#include <memory>

namespace nlm {

class PredictionCoordinator {
public:
    PredictionCoordinator(std::shared_ptr<Config> config);
    ~PredictionCoordinator();
    
    // Initialize prediction system
    bool initialize(Brain* brain);
    
    // Update prediction system
    void update(Timestamp time, TimestepDuration timestep);
    
    // Reset prediction system
    void reset();
    
    // Get component access
    PredictionSystem* getPredictionSystem() { return predictionSystem.get(); }
    
    // Prediction statistics
    float getPredictionError() const;
    float getPredictionConfidence() const;
    
    // Configuration methods
    void setPredictionHorizon(size_t horizon) { predictionHorizon = horizon; }
    void setPredictionThreshold(float threshold) { predictionThreshold = threshold; }
    
private:
    struct Impl;
    Impl* pImpl;
    
    size_t predictionHorizon;
    float predictionThreshold;
};

} // namespace nlm