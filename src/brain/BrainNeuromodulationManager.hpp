#pragma once

#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"

namespace nlm {

class BrainNeuromodulationManager {
public:
    // Create neuromodulation manager with configuration
    explicit BrainNeuromodulationManager(std::shared_ptr<Config> config);
    
    ~BrainNeuromodulationManager();
    
    // Initialize neuromodulation manager
    bool initialize(BrainNeuralCore* neuralCore);
    
    // Update neuromodulation systems
    void update(TimestepDuration timestep, const class Observation* observation = nullptr);
    
    // Neuromodulation operations
    void signalReward(float reward);
    void signalPredictionError(float error);
    void signalNovelty(const class Observation& observation);
    void computeCuriosity();
    
    // Get access to neuromodulation components
    Dopamine* getDopamine();
    const Dopamine* getDopamine() const;
    
    Curiosity* getCuriosity();
    const Curiosity* getCuriosity() const;
    
    Novelty* getNovelty();
    const Novelty* getNovelty() const;
    
    PredictionError* getPredictionError();
    const PredictionError* getPredictionError() const;
    
    // Neuromodulation effects
    void applyEffectsToNeuralCore(BrainNeuralCore* neuralCore);
    void applyEffectsToPlasticity(BrainPlasticityManager* plasticityManager);
    
    // Reset neuromodulation state
    void reset();
    
    // Get current configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Log neuromodulation manager status
    void logStatus() const;

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm