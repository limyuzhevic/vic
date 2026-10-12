// Experiment framework improvements for better experiment management
// This provides a more robust experiment system with configurable parameters,
// result export, and analysis capabilities

#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>

namespace nlm {

// Improved ExperimentConfig for better parameter management
struct ExperimentConfig {
    std::string name;
    std::string description;
    uint64_t seed;
    
    // Simulation parameters
    double timestep = 0.001;
    size_t neuronCount = 500;
    size_t regionCount = 1;
    size_t maxSteps = 1000;
    float connectionProbability = 0.15f;
    
    // Experiment-specific parameters
    std::map<std::string, std::string> parameters;
    
    // Output configuration
    std::string resultFile;
    bool saveResults = false;
    bool exportData = false;
    
    // Analysis settings
    bool computeStatistics = true;
    bool computeNetworkMetrics = true;
    bool computePlasticityMetrics = true;
    
    ExperimentConfig(const std::string& name = "experiment") 
        : name(name), seed(42), timestep(0.001), maxSteps(1000) {}
};

// ExperimentResult for comprehensive result storage
struct ExperimentResult {
    std::string experimentName;
    uint64_t seed;
    SimulationStep startStep;
    SimulationStep endStep;
    
    // Network metrics
    size_t neuronCount;
    size_t synapseCount;
    size_t regionCount;
    float avgFiringRate;
    float eiRatio;
    size_t totalSpikes;
    
    // Plasticity metrics
    float initialMeanWeight;
    float finalMeanWeight;
    float weightChange;
    size_t strengthenedSynapses;
    size_t weakenedSynapses;
    size_t unchangedSynapses;
    
    // Activity metrics
    size_t firingNeurons;
    size_t maxFiringNeurons;
    size_t minFiringNeurons;
    
    // Experimental observations
    bool learningOccurred;
    bool connectivityWorks;
    bool plasticityActive;
    bool stdpWorking;
    
    // Timestamps and metadata
    std::string startTime;
    std::string endTime;
    std::vector<std::string> observations;
    
    ExperimentResult(const std::string& name) : experimentName(name), startStep(0), endStep(0),
                                               neuronCount(0), synapseCount(0), regionCount(0),
                                               avgFiringRate(0.0f), eiRatio(0.0f), totalSpikes(0),
                                               initialMeanWeight(0.0f), finalMeanWeight(0.0f),
                                               weightChange(0.0f), strengthenedSynapses(0),
                                               weakenedSynapses(0), unchangedSynapses(0),
                                               firingNeurons(0), maxFiringNeurons(0),
                                               minFiringNeurons(0), learningOccurred(false),
                                               connectivityWorks(false), plasticityActive(false),
                                               stdpWorking(false) {}
};

// Advanced ExperimentManager for better experiment orchestration
class ExperimentManager {
public:
    ExperimentManager();
    ~ExperimentManager();
    
    // Configure experiment
    void configure(const ExperimentConfig& config);
    ExperimentConfig& getConfig();
    
    // Set up brain state
    bool initializeBrain(std::shared_ptr<Brain> brain);
    
    // Run experiment phase
    enum ExperimentPhase {
        PHASE_BASIC_CONNECTIVITY,
        PHASE_PLASTICITY_LEARNING,
        PHASE_STDP_VERIFICATION,
        PHASE_ADVANCED_ANALYSIS
    };
    
    ExperimentResult runPhase(std::shared_ptr<Brain> brain, ExperimentPhase phase);
    
    // Batch experiment execution
    std::vector<ExperimentResult> runAllPhases(std::shared_ptr<Brain> brain);
    
    // Experiment state management
    void resetBrainState(std::shared_ptr<Brain> brain);
    void prepareForPhase(std::shared_ptr<Brain> brain, ExperimentPhase phase);
    
    // Result management
    void saveResults(const std::string& filepath) const;
    void exportAnalysisData(const std::string& filepath) const;
    void generateReport(const std::string& filepath) const;
    
    // Analysis utilities
    static std::string generateExperimentReport(const ExperimentResult& result);
    static std::string formatMetrics(const std::map<std::string, double>& metrics);
    static std::string getPhaseName(ExperimentPhase phase);
    
    // Validation and checks
    bool validateConfiguration() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Progress tracking
    void setProgressCallback(std::function<void(float)> callback);
    void updateProgress(float progress);
    
    // Experiment statistics
    size_t getCompletedPhases() const;
    float getAverageScore() const;
    const std::vector<ExperimentResult>& getAllResults() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Predefined experiment types for easy configuration
class StandardExperiments {
public:
    static ExperimentConfig createBasicConnectivityExperiment(const std::string& name = "basic_connectivity");
    static ExperimentConfig createPlasticityExperiment(const std::string& name = "plasticity_learning");
    static ExperimentConfig createStdpExperiment(const std::string& name = "stdp_verification");
    static ExperimentConfig createAdvancedAnalysisExperiment(const std::string& name = "advanced_analysis");
    static ExperimentConfig createAllExperiments(const std::string& name = "comprehensive");
    
    static std::vector<ExperimentConfig> getDefaultExperimentSet();
    static ExperimentConfig createCustomExperiment(const std::string& name,
                                                   const std::map<std::string, std::string>& parameters);
};