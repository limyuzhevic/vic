#pragma once

#include "NeuromodulatorBase.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

// Prediction error signal for curiosity and learning
// PLACEHOLDER - Phase 2
class PredictionError : public NeuromodulatorBase {
public:
    PredictionError();
    ~PredictionError() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void initialize(class Brain* brain) override;
    NeuromodulatorType getType() const override;
    void loadConfig(const std::string& key, float value) override;
    
    // Prediction error-specific methods
    float getError() const;
    void computeError(float predicted, float actual);
    void updatePrediction(float newPrediction);
    float getMagnitude() const;
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm