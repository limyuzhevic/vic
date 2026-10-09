#pragma once

/**
 * ConfigurationDrivenNeuralParameters
 * 
 * This module provides a configuration-driven approach for neural system parameters,
 * allowing runtime configuration of neuron properties, synaptic dynamics, and
 * plasticity rules through configuration files or command-line arguments.
 * 
 * This improves modularity, testability, and reproducibility of experiments
 * by making all parameters configurable rather than hard-coded.
 */

#include "../core/Config/Config.hpp"
#include <string>
#include <vector>
#include <map>
#include <unordered_map>

namespace nlm {

/**
 * Neural parameter sets for different neuron types and developmental stages
 */
struct NeuralParameters {
    // Basic neuron properties
    struct NeuronParams {
        float membraneCapacitance;     // nF
        float timeConstant;            // ms
        float restingPotential;       // mV
        float threshold;               // mV
        float resetPotential;          // mV
        float adaptationConstant;      // Adaptation after spikes
        float maxMembranePotential;    // Safety clamp
        float minMembranePotential;    // Safety clamp
        float leakConductance;         // Leak rate
        float refractoryPeriod;        // Refractory duration (ms)
        
        NeuronParams() : membraneCapacitance(1.0f),
                         timeConstant(20.0f),
                         restingPotential(-70.0f),
                         threshold(-55.0f),
                         resetPotential(-65.0f),
                         adaptationConstant(0.1f),
                         maxMembranePotential(50.0f),
                         minMembranePotential(-100.0f),
                         leakConductance(0.05f),
                         refractoryPeriod(10.0f) {}  // 10ms refractory
    };
    
    // Synapse properties
    struct SynapseParams {
        float initialExcitatoryWeight;    // 0.1 - 0.4
        float initialInhibitoryWeight;   // -0.1 to -0.4
        float initialOtherWeight;         // -0.1 to 0.1
        float efficacyRangeMin;           // 0.0 - 2.0
        float efficacyRangeMax;           // 0.0 - 2.0
        float minWeight;                  // -1.0
        float maxWeight;                  // 1.0
        float depressionTimeConstant;    // ms
        float facilitationTimeConstant;  // ms
        float maxUtilization;             // 0.0 - 1.0
        
        SynapseParams() : initialExcitatoryWeight(0.2f),
                          initialInhibitoryWeight(-0.2f),
                          initialOtherWeight(0.0f),
                          efficacyRangeMin(0.8f),
                          efficacyRangeMax(1.0f),
                          minWeight(-1.0f),
                          maxWeight(1.0f),
                          depressionTimeConstant(200.0f),
                          facilitationTimeConstant(100.0f),
                          maxUtilization(1.0f) {}
    };
    
    // Plasticity rule parameters
    struct PlasticityParams {
        // STDP parameters
        float stdpLTPWeight;             // Long-term potentiation
        float stdpLTDWeight;            // Long-term depression
        float stdpTau;                  // Time constant (ms)
        float stdpLearningRate;         // Overall learning rate
        
        // Hebbian parameters
        float hebbianLearningRate;
        float hebbianTimeConstant;
        
        // Neuromodulation effects
        float dopaminePlasticityMod;    // Modulation factor
        float rewardPredictionError;    // RPE signal scaling
        
        PlasticityParams() : stdpLTPWeight(0.02f),
                            stdpLTDWeight(0.015f),
                            stdpTau(20.0f),
                            stdpLearningRate(1.0f),
                            hebbianLearningRate(0.01f),
                            hebbianTimeConstant(100.0f),
                            dopaminePlasticityMod(1.0f),
                            rewardPredictionError(1.0f) {}
    };
    
    // Structural plasticity parameters
    struct StructuralParams {
        float synaptogenesisRate;        // Rate of new synapse formation
        float pruningRate;              // Rate of synapse elimination
        float competitionThreshold;    // Competitive threshold
        float ageDependentDecay;       // How fast synapses decay
        float criticalPeriodEnd;       // End of critical period (simulated age)
        
        StructuralParams() : synaptogenesisRate(0.0001f),
                             pruningRate(0.00001f),
                             competitionThreshold(0.5f),
                             ageDependentDecay(0.9f),
                             criticalPeriodEnd(100.0f) {}  // Simulated age
    };
    
    // Neuromodulation parameters
    struct NeuromodulationParams {
        float dopamineBaseline;         // Baseline dopamine level
        float dopamineGain;             // Signal amplification
        float curiosityDecay;           // Decay of curiosity signal
        float noveltyThreshold;         // Novelty detection threshold
        float predictionErrorGain;     // RPE signal strength
        
        NeuromodulationParams() : dopamineBaseline(0.1f),
                                   dopamineGain(0.5f),
                                   curiosityDecay(0.99f),
                                   noveltyThreshold(0.5f),
                                   predictionErrorGain(1.0f) {}
    };
    
    // Developmental parameters
    struct DevelopmentParams {
        float plasticityRateStart;      // Initial plasticity rate
        float plasticityRateEnd;        // Mature plasticity rate
        float structuralPlasticityStart; // Initial structural plasticity
        float structuralPlasticityEnd;   // Mature structural plasticity
        float criticalPeriodStart;      // Start of critical period
        float maturationRate;           // Rate of maturation
        
        DevelopmentParams() : plasticityRateStart(1.0f),
                              plasticityRateEnd(0.2f),
                              structuralPlasticityStart(1.0f),
                              structuralPlasticityEnd(0.1f),
                              criticalPeriodStart(50.0f),
                              maturationRate(0.01f) {}
    };
    
    // Population-specific parameters
    struct PopulationParams {
        float firingThreshold;           // Base firing threshold
        float inputSensitivity;         // Sensitivity to inputs
        float outputGain;                // Output signal strength
        float noiseLevel;                // Intrinsic noise
        
        PopulationParams() : firingThreshold(1.0f),
                             inputSensitivity(1.0f),
                             outputGain(1.0f),
                             noiseLevel(0.1f) {}
    };
    
    // Configuration version
    int configVersion;
    
    // Current parameters
    NeuronParams neuron;
    SynapseParams synapse;
    PlasticityParams plasticity;
    StructuralParams structural;
    NeuromodulationParams neuromodulation;
    DevelopmentParams development;
    
    // Population type parameters
    std::unordered_map<NeuronType, PopulationParams> populationTypeParams;
    
    NeuralParameters() : configVersion(1) {
        // Initialize population type parameters with reasonable defaults
        populationTypeParams[NeuronType::Sensory] = PopulationParams();
        populationTypeParams[NeuronType::Internal] = PopulationParams();
        populationTypeParams[NeuronType::Motor] = PopulationParams();
        populationTypeParams[NeuronType::Excitatory] = PopulationParams();
        populationTypeParams[NeuronType::Inhibitory] = PopulationParams();
    }
};

/**
 * ConfigurationDrivenNeuralParameters - Main interface for configuration-driven parameters
 */
class ConfigurationDrivenNeuralParameters {
public:
    ConfigurationDrivenNeuralParameters();
    ~ConfigurationDrivenNeuralParameters();
    
    /**
     * Load parameters from configuration file
     */
    bool loadFromFile(const std::string& filepath, NeuralParameters& params);
    
    /**
     * Load parameters from configuration object
     */
    bool loadFromConfig(const std::shared_ptr<Config>& config, NeuralParameters& params);
    
    /**
     * Load parameters from command line arguments
     */
    bool loadFromArgs(int argc, char** argv, NeuralParameters& params);
    
    /**
     * Apply configuration parameters to neural systems
     */
    void applyToBrain(std::shared_ptr<Brain> brain, const NeuralParameters& params);
    
    /**
     * Validate parameters
     */
    bool validate(const NeuralParameters& params) const;
    
    /**
     * Get default parameters for a specific configuration
     */
    static NeuralParameters getDefaultParameters(const std::string& preset = "standard");
    
    /**
     * Get parameters for a specific developmental stage
     */
    static NeuralParameters getParametersForDevelopmentalStage(
        const DevelopmentParams& devParams, 
        DevelopmentalStage stage);
    
    /**
     * Export parameters to file
     */
    bool exportToFile(const std::string& filepath, const NeuralParameters& params) const;
    
    /**
     * Merge parameters (file + defaults)
     */
    NeuralParameters mergeParameters(const NeuralParameters& fileParams, 
                                      const NeuralParameters& defaultParams) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * ParameterPresets - Common parameter configurations
 */
class ParameterPresets {
public:
    static NeuralParameters createStandardPreset();
    static NeuralParameters createDevelopmentPreset();
    static NeuralParameters createLearningPreset();
    static NeuralParameters createMaturePreset();
    static NeuralParameters createMinimalPreset();
    static NeuralParameters createHighPlasticityPreset();
    static NeuralParameters createLowNoisePreset();
    
private:
    ParameterPresets() = delete;
    ParameterPresets(const ParameterPresets&) = delete;
    ParameterPresets& operator=(const ParameterPresets&) = delete;
};

} // namespace nlm
