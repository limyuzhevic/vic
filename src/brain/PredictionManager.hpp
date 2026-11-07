#pragma once

#include "Brain.hpp"
#include <memory>

namespace nlm {

class PredictionManager {
public:
    PredictionManager(Brain* brain);
    ~PredictionManager();
    
    bool initialize();
    void step(double timestep);
    void reset();
    
    // Prediction system access
    PredictionSystem* getPredictionSystem() { return predictionSystem.get(); }
    
    // Prediction processing
    void updatePredictionSystem(double timestep);
    void updatePredictionError(double timestep);
    
private:
    Brain* brain_;
    std::unique_ptr<PredictionSystem> predictionSystem;
};

} // namespace nlm
