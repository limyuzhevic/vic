#pragma once

#include "../brain/Brain.hpp"
#include "../plasticity/STDP.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>

namespace nlm {

// Advanced brain configuration options
struct AdvancedBrainConfig {
    // Custom neuron types
    struct CustomNeuronType {
        std::string name;
        float threshold;
        float resetPotential;
        float decayConstant;
        float synapticWeight;
        bool isExcitatory;
        std::map<std::string, float> properties;
        
        CustomNeuronType() : threshold(-55.0f), resetPotential(-70.0f), 
                           decayConstant(0.04f), synapticWeight(1.0f), 
                           isExcitatory(true) {}
    };
    
    std::vector<CustomNeuronType> customNeuronTypes;
    
    // Custom development trajectories
    struct DevelopmentTrajectory {
        std::string name;
        std::vector<std::pair<float, float>> stages;  // (age, plasticity_modulus)
        std::vector<std::pair<float, float>> synaptogenesisRates;
        std::vector<std::pair<float, float>> pruningRates;
        std::vector<std::pair<float, float>> myelinationRates;
        
        DevelopmentalStage getStageAtAge(float age) const {
            for (auto it = stages.rbegin(); it != stages.rend(); ++it) {
                if (age >= it->first) {
                    return static_cast<DevelopmentalStage>(static_cast<int>(it->second));
                }
            }
            return DevelopmentalStage::Initial;
        }
    };
    
    std::vector<DevelopmentTrajectory> developmentTrajectories;
    std::string activeDevelopmentTrajectory;
    
    // Custom neuromodulation systems
    struct CustomNeuromodulator {
        std::string name;
        float baselineLevel;
        float maxLevel;
        float modulationStrength;
        std::map<std::string, float> effects;
        bool isEnabled;
        
        CustomNeuromodulator() : baselineLevel(0.5f), maxLevel(1.0f), 
                               modulationStrength(1.0f), isEnabled(true) {}
    };
    
    std::vector<CustomNeuromodulator> customNeuromodulators;
    
    // Multi-region architectures
    struct MultiRegionArchitecture {
        std::string name;
        std::vector<RegionId> regions;
        std::vector<std::pair<RegionId, RegionId>> longRangeConnections;
        float connectionStrength;
        std::string interactionPattern;
    };
    
    std::vector<MultiRegionArchitecture> multiRegionArchitectures;
    std::string activeArchitecture;
    
    // Advanced plasticity mechanisms
    struct PlasticityMechanism {
        std::string name;
        std::string type;  // "STDP", "Hebbian", "BCM", "Oja", "HTT", etc.
        std::map<std::string, float> parameters;
        bool isEnabled;
        
        PlasticityMechanism() : isEnabled(true) {}
    };
    
    std::vector<PlasticityMechanism> plasticityMechanisms;
    
    // Simulation modes
    enum class SimulationMode {
        RealTime,
        EventDriven,
        Batch,
        Parallel,
        MemoryEfficient
    };
    
    SimulationMode simulationMode;
    bool enableParallelProcessing;
    bool enableMemoryOptimization;
    
    // Analysis configuration
    struct AnalysisConfig {
        bool enableNeuralPatternAnalysis;
        bool enableLearningCurveAnalysis;
        bool enableStatisticalAnalysis;
        bool enableAdvancedVisualization;
        bool enableMachineLearningIntegration;
        float analysisInterval;
        std::string analysisOutputFormat;
    };
    
    AnalysisConfig analysisConfig;
    
    // Custom trajectories
    std::vector<std::string> customTrajectories;
    
    // Advanced neuromodulation parameters
    float neuromodulationTimeConstant;
    float neuromodulationStrength;
    bool enableNeuromodulationLearning;
    
    // Custom learning rules
    std::map<std::string, std::string> customLearningRules;
};

// Advanced Brain: Extends Brain with expert-level features
class AdvancedBrain : public Brain {
public:
    // Create advanced brain with configuration
    explicit AdvancedBrain(std::shared_ptr<Config> config);
    
    ~AdvancedBrain() override;
    
    // Disable copying, enable moving
    AdvancedBrain(const AdvancedBrain&) = delete;
    AdvancedBrain& operator=(const AdvancedBrain&) = delete;
    AdvancedBrain(AdvancedBrain&&) noexcept = default;
    AdvancedBrain& operator=(AdvancedBrain&&) noexcept = default;
    
    // Custom neuron types
    NeuronId addCustomNeuron(const AdvancedBrainConfig::CustomNeuronType& neuronType, 
                           RegionId region = RegionId(1));
    bool updateNeuronProperties(NeuronId neuronId, 
                               const AdvancedBrainConfig::CustomNeuronType& properties);
    
    // Multi-region architectures
    bool loadMultiRegionArchitecture(const std::string& architectureName);
    bool addRegionWithArchitecture(const std::string& architectureName,
                                  const std::string& regionName = "");
    
    // Advanced plasticity mechanisms
    bool addPlasticityMechanism(const AdvancedBrainConfig::PlasticityMechanism& mechanism);
    bool configurePlasticityRule(const std::string& ruleName, const std::map<std::string, float>& params);
    
    // Custom development trajectories
    bool loadDevelopmentTrajectory(const std::string& trajectoryName);
    void setDevelopmentTrajectory(const std::string& trajectoryName);
    DevelopmentTrajectory getCurrentDevelopmentTrajectory() const;
    
    // Advanced neuromodulation
    bool addCustomNeuromodulator(const AdvancedBrainConfig::CustomNeuromodulator& modulator);
    bool activateNeuromodulator(const std::string& name, float level = 1.0f);
    bool deactivateNeuromodulator(const std::string& name);
    float getNeuromodulatorLevel(const std::string& name) const;
    
    // Custom neural dynamics
    void setCustomDynamicsModel(const std::string& modelName, 
                               const std::map<std::string, float>& parameters);
    void configureCustomSynapticConductance(const std::string& neuronType, 
                                          float conductance, float reversalPotential);
    
    // Advanced simulation modes
    void setSimulationMode(AdvancedBrainConfig::SimulationMode mode);
    AdvancedBrainConfig::SimulationMode getSimulationMode() const;
    void enableParallelProcessing(bool enable);
    void enableMemoryOptimization(bool enable);
    
    // Advanced analysis integration
    bool enableAnalysisFeature(const std::string& featureName);
    bool disableAnalysisFeature(const std::string& featureName);
    bool isAnalysisFeatureEnabled(const std::string& featureName) const;
    
    // Custom learning trajectories
    void addLearningTrajectory(const std::string& name, const std::vector<float>& trajectory);
    std::vector<float> getLearningTrajectory(const std::string& name) const;
    void setActiveLearningTrajectory(const std::string& name);
    std::string getActiveLearningTrajectory() const;
    
    // Advanced neuromodulation systems
    void setNeuromodulationTimeConstant(float timeConstant);
    void setNeuromodulationStrength(float strength);
    void enableNeuromodulationLearning(bool enable);
    
    // Custom learning rules
    void addCustomLearningRule(const std::string& name, const std::string& rule);
    
    // Performance optimization
    void optimizeConnectivity(float targetConnectionDensity);
    void compressSynapticWeights(float compressionFactor);
    void enableSparseRepresentation(bool enable);
    
    // Export and import configurations
    bool exportConfiguration(const std::string& filepath) const;
    bool importConfiguration(const std::string& filepath);
    
    // Get advanced configuration
    const AdvancedBrainConfig& getAdvancedConfig() const;
    void updateAdvancedConfig(const AdvancedBrainConfig& config);
    
    // Advanced statistics
    struct AdvancedStats {
        float plasticityRate;
        float developmentProgress;
        float neuromodulationActivity;
        float learningEfficiency;
        float memoryUtilization;
        std::map<std::string, float> customMetrics;
    };
    
    AdvancedStats calculateAdvancedStats() const;
    
    // Logging
    void logAdvancedStatus() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
