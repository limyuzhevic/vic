#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "Neuromodulator.hpp"
#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace nlm {

// Forward declarations
class NeuromodulationEngine;

// Plasticity control parameters
struct PlasticityControlParams {
    // Gated plasticity settings
    bool enableGatedPlasticity;           // Enable neuromodulator-gated plasticity
    float gatedPlasticityThreshold;       // Threshold for plasticity gating
    
    // Scaled plasticity settings
    bool enableScaledPlasticity;          // Enable neuromodulator-scaled plasticity
    float scaledPlasticityFactor;         // Scaling factor for plasticity magnitude
    float plasticitySaturation;           // Maximum plasticity scaling
    
    // Timing-dependent plasticity settings
    bool enableTDPlasticity;              // Enable neuromodulator-timing dependent plasticity
    float tdPlasticityWindow;              // Time window for TD plasticity
    float tdPlasticityWeight;              // Weight for timing dependence
    
    // Type-specific plasticity settings
    bool enableTypeSpecificPlasticity;    // Enable neuromodulator-type specific plasticity
    std::vector<float> typeSpecificWeights;  // Weights for different neuromodulator types
    
    // Learning rate modulation
    float baseLearningRate;               // Base learning rate
    float neuromodulatorModulation;       // Overall neuromodulator modulation
    float homeostaticLearningRate;       // Learning rate for homeostasis
    
    PlasticityControlParams()
        : enableGatedPlasticity(true), gatedPlasticityThreshold(0.5f),
          enableScaledPlasticity(true), scaledPlasticityFactor(1.0f),
          plasticitySaturation(2.0f), enableTDPlasticity(true),
          tdPlasticityWindow(0.1f), tdPlasticityWeight(0.5f),
          enableTypeSpecificPlasticity(true), baseLearningRate(0.01f),
          neuromodulatorModulation(1.0f), homeostaticLearningRate(0.001f) {
        
        typeSpecificWeights.resize(10, 1.0f);  // Default 10 neuromodulators
    }
};

// Plasticity state for synapse
struct SynapsePlasticityState {
    float currentWeight;                  // Current synaptic weight
    float targetWeight;                   // Target synaptic weight
    float eligibilityTrace;               // Eligibility trace for STDP
    float neuromodulatorInfluence;         // Current neuromodulator influence
    float learningHistory;                 // Learning history
    float homeostaticPressure;            // Homeostatic pressure
    float temporalEligibility;            // Temporal eligibility trace
    float burstEligibility;               // Burst eligibility for TD learning
    
    SynapsePlasticityState()
        : currentWeight(0.5f), targetWeight(0.5f), eligibilityTrace(0.0f),
          neuromodulatorInfluence(1.0f), learningHistory(0.0f),
          homeostaticPressure(1.0f), temporalEligibility(0.0f),
          burstEligibility(0.0f) {}
};

// Hebbian plasticity state
struct HebbianPlasticityState {
    float activationProduct;              // Product of pre- and post-synaptic activity
    float noveltyBonus;                   // Bonus for novel connections
    float predictionErrorBonus;           // Bonus for prediction error
    float neuromodulatorModulation;        // Neuromodulator modulation
    float synapticTag;                     // Synaptic tag for synaptic tagging and capture
    
    HebbianPlasticityState()
        : activationProduct(0.0f), noveltyBonus(0.0f), predictionErrorBonus(0.0f),
          neuromodulatorModulation(1.0f), synapticTag(0.0f) {}
};

// STDP (Spike-Timing-Dependent Plasticity) state
struct STDPState {
    std::vector<float> tracePre;          // Pre-synaptic trace
    std::vector<float> tracePost;         // Post-synaptic trace
    std::vector<float> deltaWeights;      // Weight changes
    float stdpLearningRate;               // STDP learning rate
    float neuromodulatorSTDPModulation;    // Neuromodulator modulation of STDP
    float temporalWindow;                 // STDP temporal window
    float burstModulation;                // Burst modulation factor
    
    STDPState()
        : stdpLearningRate(0.001f), neuromodulatorSTDPModulation(1.0f),
          temporalWindow(0.02f), burstModulation(1.0f) {
        
        tracePre.resize(1000, 0.0f);
        tracePost.resize(1000, 0.0f);
        deltaWeights.resize(1000, 0.0f);
    }
};

// Structural plasticity state
struct StructuralPlasticityState {
    float newDendriteFormation;           // Rate of new dendrite formation
    float spineDensity;                   // Spine density
    float spineElimination;               // Spine elimination rate
    float arborExpansion;                 // Arbor expansion rate
    float neurotrophinModulation;          // Neurotrophin modulation
    float neuromodulatorStructural;        // Neuromodulator modulation
    
    StructuralPlasticityState()
        : newDendriteFormation(0.001f), spineDensity(1000.0f),
          spineElimination(0.0001f), arborExpansion(0.01f),
          neurotrophinModulation(1.0f), neuromodulatorStructural(1.0f) {}
};

// Plasticity controller: Neuromodulator-controlled plasticity
class PlasticityController {
public:
    PlasticityController();
    ~PlasticityController();
    
    // Disable copying, enable moving
    PlasticityController(const PlasticityController&) = delete;
    PlasticityController& operator=(const PlasticityController&) = delete;
    PlasticityController(PlasticityController&&) noexcept;
    PlasticityController& operator=(PlasticityController&&) noexcept;
    
    // Initialization
    bool initialize(std::shared_ptr<Brain> brain, std::shared_ptr<NeuromodulationEngine> engine);
    
    // Main update step
    void update(TimestepDuration dt, SimulationStep step);
    
    // Plasticity computation
    void computeHebbianPlasticity(const class NeuralConnection& connection,
                                 float preActivation, float postActivation);
    void computeSTDPPlasticity(const class NeuralConnection& connection,
                              float preSpikeTime, float postSpikeTime);
    void computeTDPlasticity(const class NeuralConnection& connection,
                           float tdError, float reward);
    void computeNeuromodulatorGatedPlasticity(const class NeuralConnection& connection,
                                            const Neuromodulator& modulator);
    void computeNeuromodulatorScaledPlasticity(const class NeuralConnection& connection,
                                              const Neuromodulator& modulator);
    void computeNeuromodulatorTDPlasticity(const class NeuralConnection& connection,
                                         const Neuromodulator& modulator,
                                         float spikeTimeDifference);
    void computeNeuromodulatorTypeSpecificPlasticity(const class NeuralConnection& connection,
                                                   const Neuromodulator& modulator);
    
    // State management
    void updateState(TimestepDuration dt);
    void reset();
    
    // Configuration
    void setPlasticityParams(const PlasticityControlParams& params);
    void enableHebbianPlasticity(bool enable) { enableHebbianPlasticity_ = enable; }
    void enableSTDP(bool enable) { enableSTDP_ = enable; }
    void enableTDPlasticity(bool enable) { enableTDPlasticity_ = enable; }
    void enableStructuralPlasticity(bool enable) { enableStructuralPlasticity_ = enable; }
    
    // Integration with other systems
    void integrateWithNeuromodulators(const std::vector<const Neuromodulator*>& modulators);
    void integrateWithLearningSystems();
    void integrateWithHomeostaticSystems();
    
    // Plasticity application
    void applyPlasticityToSynapse(SynapseId synapseId, float weightChange);
    void updateConnectionWeight(SynapseId synapseId, float newWeight);
    void updateStructuralProperties(SynapseId synapseId, float structuralChange);
    
    // Accessors
    float getHebbianLearningRate() const { return pImpl->state.hebbianLearningRate; }
    float getSTDPLearningRate() const { return pImpl->state.stdpLearningRate; }
    float getTDLearningRate() const { return pImpl->state.tdLearningRate; }
    float getTotalPlasticityChange() const { return pImpl->state.totalPlasticityChange; }
    
    // History and statistics
    const std::vector<float>& getHebbianHistory() const { return pImpl->history.hebbianHistory; }
    const std::vector<float>& getSTDPHistory() const { return pImpl->history.stdpHistory; }
    const std::vector<float>& getTDHistory() const { return pImpl->history.tdHistory; }
    void clearHistory();
    
    // Safety and validation
    bool validatePlasticityConstraints() const;
    void enforceStabilityConstraints();
    bool checkNumericalStability() const;
    
private:
    struct Impl;
    Impl* pImpl;
    
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<NeuromodulationEngine> engine_;
    
    bool enableHebbianPlasticity_;
    bool enableSTDP_;
    bool enableTDPlasticity_;
    bool enableStructuralPlasticity_;
    
    // Plasticity state
    struct PlasticityState {
        float hebbianLearningRate;            // Hebbian learning rate
        float stdpLearningRate;              // STDP learning rate
        float tdLearningRate;                // TD learning rate
        float totalPlasticityChange;          // Total plasticity change
        float homeostaticPressure;           // Overall homeostatic pressure
        float neuromodulatorModulation;       // Overall neuromodulator modulation
        
        PlasticityState()
            : hebbianLearningRate(0.01f), stdpLearningRate(0.001f),
              tdLearningRate(0.01f), totalPlasticityChange(0.0f),
              homeostaticPressure(1.0f), neuromodulatorModulation(1.0f) {}
    };
    
    // History tracking
    struct PlasticityHistory {
        std::vector<float> hebbianHistory;
        std::vector<float> stdpHistory;
        std::vector<float> tdHistory;
        std::vector<float> structuralHistory;
        
        PlasticityHistory() {
            hebbianHistory.reserve(1000);
            stdpHistory.reserve(1000);
            tdHistory.reserve(1000);
            structuralHistory.reserve(100);
        }
    };
};

} // namespace nlm