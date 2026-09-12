#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

class PredictionError {
public:
    PredictionError();
    ~PredictionError();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get error value
    float getError() const;
    
    // Get error magnitude for neuromodulation
    float getMagnitude() const;
    
    // Compute prediction error
    void computeError(float predicted, float actual);
    
    // Update prediction
    void updatePrediction(float newPrediction);
    
    // Get history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm