#pragma once

#include "../core/Types/Types.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../brain/Brain.hpp"
#include <memory>

namespace nlm {

// Neuromodulation Manager
// Coordinates dopamine, curiosity, prediction error, and novelty signals
// Integrates neuromodulatory effects on neural excitability and plasticity

class NeuromodulationManager {
public:
    NeuromodulationManager();
    ~NeuromodulationManager();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Update all neuromodulation systems
    void update(float timestep);
    
    // Get neuromodulator instances
    Dopamine* getDopamine() { return dopamine.get(); }
    Curiosity* getCuriosity() { return curiosity.get(); }
    Novelty* getNovelty() { return novelty.get(); }
    PredictionError* getPredictionError() { return predictionError.get(); }
    
    // Configure neuromodulator parameters
    void configure(float baselineDopamine, float noveltySensitivity,
                   float curiosityWeight, float predictionErrorThreshold);
    
    // Get combined neuromodulation effects
    float getTotalNeuromodulationLevel() const;
    float getPlasticityFactor() const;
    
    // Apply neuromodulation to neurons
    void applyToNeurons(std::vector<Neuron*>& neurons);
    
    // Process reward prediction errors
    void processRewardPredictionError(float reward, float prediction);
    
    // Process novelty signals
    void processNovelty(float noveltyLevel, const class Observation& observation,
                       const class Observation& previousObservation);
    
    // Get neuromodulation statistics
    float getDopamineLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionErrorMagnitude() const;
    
    // Reset all neuromodulators
    void reset();
    
    // Set neuromodulation learning rates
    void setLearningRates(float stdpLearningRate, float hebbianLearningRate);
    
    // Get neuromodulation state for checkpointing
    struct NeuromodulationState {
        float dopamineLevel;
        float curiosityLevel;
        float noveltyLevel;
        float predictionErrorLevel;
        std::vector<float> dopamineHistory;
        std::vector<float> curiosityHistory;
        std::vector<float> noveltyHistory;
        std::vector<float> predictionErrorHistory;
        double lastUpdateTime;
    };
    
    NeuromodulationState getState() const;
    void restoreState(const NeuromodulationState& state);
    
    // Update neuromodulator-based plasticity
    void updatePlasticity(STDP* stdp, Hebbian* hebbian, 
                         std::vector<Synapse*>& synapses);
    
private:
    // Private implementation for PIMPL pattern
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
