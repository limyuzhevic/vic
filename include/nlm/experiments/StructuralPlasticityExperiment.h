#pragma once

#include "AdvancedExperiment.h"
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <vector>
#include <memory>
#include <chrono>
#include <random>
#include <unordered_map>

namespace nlm {

/**
 * Structural Plasticity Experiment
 * 
 * Advanced experiment that tests structural plasticity and brain development
 * with dynamic synaptogenesis, pruning, and neural network remodeling.
 * Demonstrates how neural networks can reorganize themselves through
 * activity-dependent structural changes.
 */
class StructuralPlasticityExperiment : public AdvancedExperiment {
public:
    // Structural plasticity configuration
    struct PlasticityConfig {
        // Synaptogenesis parameters
        float synaptogenesisRate;
        float initialConnectionProbability;
        size_t initialMaxNeurons;
        size_t maxNeurons;
        
        // Pruning parameters
        float pruningRate;
        float pruningThreshold;
        float excitationInhibitionRatio;
        
        // Development parameters
        uint64_t synaptogenesisPhase;
        uint64_t pruningPhase;
        uint64_t stabilizationPhase;
        
        // Activity-dependent parameters
        float activityDependentPlasticity;
        float spikeTimingDependentPlasticity;
        float homeostaticPlasticity;
        
        // Experiment parameters
        size_t numTrials;
        float timeLimit;
        bool trackStructuralChanges;
        bool trackNeuralDynamics;
        
        PlasticityConfig()
            : synaptogenesisRate(0.0001f), initialConnectionProbability(0.1f),
              initialMaxNeurons(1000), maxNeurons(5000), pruningRate(0.00001f),
              pruningThreshold(0.01f), excitationInhibitionRatio(4.0f),
              synaptogenesisPhase(1000), pruningPhase(5000), stabilizationPhase(10000),
              activityDependentPlasticity(1.0f), spikeTimingDependentPlasticity(0.5f),
              homeostaticPlasticity(0.8f), numTrials(50), timeLimit(300.0f),
              trackStructuralChanges(true), trackNeuralDynamics(true) {}
    };
    
    // Synaptic connection information
    struct SynapticConnection {
        size_t preNeuronId;
        size_t postNeuronId;
        float weight;
        float plasticity;
        uint64_t lastUpdate;
        bool isActive;
        
        SynapticConnection() : preNeuronId(0), postNeuronId(0), weight(0.0f),
                             plasticity(0.0f), lastUpdate(0), isActive(true) {}
    };
    
    // Neural development stage
    struct NeuralDevelopmentStage {
        std::string stageName;
        uint64_t startStep;
        uint64_t endStep;
        size_t neuronCount;
        size_t synapseCount;
        float averageFiringRate;
        float averageWeight;
        float connectionDensity;
        
        NeuralDevelopmentStage() : startStep(0), endStep(0), neuronCount(0),
                                   synapseCount(0), averageFiringRate(0.0f),
                                   averageWeight(0.0f), connectionDensity(0.0f) {}
    };
    
    StructuralPlasticityExperiment();
    ~StructuralPlasticityExperiment() = default;
    
    // Experiment lifecycle implementation
    bool initialize() override;
    void setup() override;
    void run() override;
    void analyzeResults() override;
    void cleanup() override;
    
    // Configuration management
    void setPlasticityConfig(const PlasticityConfig& config);
    const PlasticityConfig& getPlasticityConfig() const;
    
    // Experiment parameters
    void setNumNeurons(size_t neurons) override;
    void setNumSynapses(size_t synapses) override;
    void setNumTrials(size_t trials) override;
    void setTimeLimit(double limit) override;
    
    // Structural plasticity specific methods
    void runStructuralPlasticityTrial(size_t trialNumber);
    void simulateSynaptogenesis(size_t trialNumber);
    void simulatePruning(size_t trialNumber);
    void simulateStabilization(size_t trialNumber);
    void updateStructuralConnectivity();
    
    // Results management
    const std::vector<SynapticConnection>& getSynapticConnections() const;
    const std::vector<NeuralDevelopmentStage>& getDevelopmentStages() const;
    double calculateSynapticStability() const;
    double calculateNetworkEfficiency() const;
    double calculateStructuralComplexity() const;
    
    // Brain and environment setup
    std::shared_ptr<Brain> createBrain() override;
    std::shared_ptr<Environment> createEnvironment() override;
    
    // Experiment execution
    bool runExperiment(std::shared_ptr<Brain> brain, 
                       std::shared_ptr<Environment> environment) override;
    
    // Results saving/loading
    bool saveResults(const std::string& filepath) override;
    bool loadResults(const std::string& filepath) override;
    
    // Progress reporting
    double getProgress() const override;
    std::string getStatus() const override;
    
    // Structural plasticity specific getters
    size_t getNeuronCount() const;
    size_t getSynapseCount() const;
    float getAverageConnectionStrength() const;
    size_t getActiveConnections() const;
    
private:
    // Configuration
    PlasticityConfig config;
    
    // State tracking
    std::vector<SynapticConnection> synapticConnections;
    std::vector<NeuralDevelopmentStage> developmentStages;
    std::shared_ptr<Brain> currentBrain;
    std::shared_ptr<Environment> currentEnvironment;
    
    // Structural plasticity state
    size_t currentNeuronCount;
    size_t currentSynapseCount;
    float currentConnectionProbability;
    uint64_t currentStep;
    
    // Random number generation
    std::mt19937_64 rng;
    std::uniform_real_distribution<double> weightDist;
    std::uniform_real_distribution<double> plasticityDist;
};

} // namespace nlm