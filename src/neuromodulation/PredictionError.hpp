#pragma once

#include "../core/Types/Types.hpp"
#include "Neuromodulator.hpp"
#include "NeuromodulationConstants.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

// PredictionError implementation - temporal difference learning
class PredictionError : public Neuromodulator {
public:
    PredictionError();
    ~PredictionError() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void reset() override;
    bool isActive() const override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Compute prediction error
    void computeError(float predicted, float actual);
    
    // Update prediction
    void updatePrediction(float newPrediction);
    
    // Temporal difference learning
    float computeTD(float reward, float currentValue, float nextValue);
    
    // Value function learning
    void learnValueFunction(float stateValue, float targetValue);
    
    // Get prediction statistics
    float getPredictedValue() const;
    float getValuePredictionError() const;
    float getMagnitude() const;
    
    // Error propagation
    void propagateError(float factor);
    
    // Error history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Set prediction parameters
    void setLearningRate(float rate);
    void setDiscountFactor(float gamma);
    void setErrorThreshold(float threshold);
    
    // Integration with other systems
    void integrateWithDopamine(class Dopamine* dopamine);
    void integrateWithCuriosity(class Curiosity* curiosity);
    
    // Connect to plasticity systems
    void connectToSTDP(class STDP* stdp);
    void connectToHebbian(class Hebbian* hebbian);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
