// PredictionComponent.h - Prediction system with forward models
#pragma once

#include "BrainComponentBase.h"
#include "../core/Types/Types.hpp"
#include <memory>
#include <vector>

namespace nlm {

class SensoryInput;
class Brain;

class PredictionComponent : public BrainComponentBase {
public:
    PredictionComponent();
    ~PredictionComponent() override;
    
    // Initialize component with brain reference
    bool initialize(Brain* brain) override;
    
    // Update component state for current timestep
    void update(const TimestepDuration& dt) override;
    
    // Reset component to initial state
    void reset() override;
    
    // Log component status
    void logStatus() const override;
    
    // Get component name
    const char* getName() const override { return "PredictionComponent"; }
    
    // Core prediction methods
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    float getPredictionError() const;
    float getConfidence() const;
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    void train(const SensoryInput& observation);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
