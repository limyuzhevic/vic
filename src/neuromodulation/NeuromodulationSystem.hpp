#pragma once

#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "Brain.hpp"
#include <memory>
#include <string>

namespace nlm {

class NeuromodulationSystem {
public:
    NeuromodulationSystem();
    ~NeuromodulationSystem();
    
    // Disable copying, enable moving
    NeuromodulationSystem(const NeuromodulationSystem&) = delete;
    NeuromodulationSystem& operator=(const NeuromodulationSystem&) = delete;
    NeuromodulationSystem(NeuromodulationSystem&&) noexcept;
    NeuromodulationSystem& operator=(NeuromodulationSystem&&) noexcept;
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Neuromodulator access
    Dopamine* getDopamine();
    const Dopamine* getDopamine() const;
    
    Curiosity* getCuriosity();
    const Curiosity* getCuriosity() const;
    
    Novelty* getNovelty();
    const Novelty* getNovelty() const;
    
    PredictionError* getPredictionError();
    const PredictionError* getPredictionError() const;
    
    // Neuromodulation updates
    void updateNeuromodulators(float timestep);
    void applyNeuromodulationToBrain();
    
    // Neuromodulation signals
    void emitReward(float reward, float predictionError = 0.0f);
    void detectNovelty(const class SensoryInput& input);
    void updateCuriosity(float explorationValue);
    void processPredictionError(float predictedValue, float actualValue);
    
    // Neuromodulation statistics
    float getDopamineLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPlasticityModulator() const;
    
    // Configuration
    void setRewardSensitivity(float sensitivity);
    void setExplorationRate(float rate);
    void setNoveltyThreshold(float threshold);
    void setPredictionLearningRate(float rate);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm