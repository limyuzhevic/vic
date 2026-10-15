#pragma once

#include "../core/Types/Types.hpp"
#include "prediction/PredictionSystem.hpp"
#include "neuromodulation/PredictionError.hpp"

namespace nlm {

class PredictionModule {
public:
    PredictionModule(std::shared_ptr<PredictionSystem> predictionSystem,
                    std::shared_ptr<PredictionError> predictionError);
    
    PredictionSystem* getPredictionSystem();
    PredictionError* getPredictionErrorSignal();
    
    void update(SimulationStep currentStep, Timestamp currentTime,
                const std::vector<float>& sensoryInput);
    void computePredictionError();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
