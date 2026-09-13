#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

class PredictionSystem {
public:
    PredictionSystem() = default;
    ~PredictionSystem() = default;
    
    // Core prediction system operations
    void initialize(class Brain* brain);
    void update(float timestep);
    float getPredictionError() const;
    void reset();
    
private:
    // Prediction state
    class Brain* brainRef;
    float predictionError;
    float predictionAccuracy;
};

} // namespace nlm