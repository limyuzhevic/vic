#pragma once

#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

class BrainNeuromodIntegration {
public:
    BrainNeuromodIntegration();
    ~BrainNeuromodIntegration();
    
    // Initialize all neuromodulation systems
    void initializeNeuromodulationSystems(Brain* brain);
    
    // Integrate dopamine system
    void integrateDopamineSystem(Brain* brain);
    
    // Integrate curiosity system
    void integrateCuriositySystem(Brain* brain);
    
    // Integrate novelty detection system
    void integrateNoveltySystem(Brain* brain);
    
    // Integrate prediction error system
    void integratePredictionErrorSystem(Brain* brain);
    
    // Apply neuromodulatory signals to plasticity
    void applyNeuromodulationToPlasticity(Brain* brain, const Neuromodulator& signal);
    
    // Apply neuromodulation to memory
    void applyNeuromodulationToMemory(Brain* brain, const Neuromodulator& signal);
    
    // Apply neuromodulation to cognition
    void applyNeuromodulationToCognition(Brain* brain, const Neuromodulator& signal);
    
    // Update neuromodulation systems
    void updateNeuromodulationSystems(Brain* brain, TimestepDuration dt);
    
    // Calculate reward prediction error
    float calculateRewardPredictionError(Brain* brain, float reward, float prediction);
    
    // Calculate novelty value
    float calculateNoveltyValue(Brain* brain, const std::vector<float>& sensoryInput);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm