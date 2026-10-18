#pragma once

#include "Experiment.hpp"
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <chrono>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <iomanip>
#include <sstream>

namespace nlm {

// Experiment parameter configuration
struct ExperimentParams {
    std::string name;
    uint64_t seed;
    SimulationStep maxSteps;
    double timestep;
    bool enableLogging;
    bool enableCheckpointing;
    bool enableDevelopment;
    bool enableNeuromodulation;
    bool enableMemorySystems;
    bool enablePrediction;
    bool enableCognition;
    
    ExperimentParams() : seed(42), maxSteps(1000), timestep(0.001), 
                         enableLogging(true), enableCheckpointing(false),
                         enableDevelopment(true), enableNeuromodulation(true),
                         enableMemorySystems(true), enablePrediction(true), 
                         enableCognition(true) {}
};

// Experiment result statistics
struct ExperimentResult {
    std::string experimentName;
    SimulationStep stepsCompleted;
    double totalReward;
    double avgFiringRate;
    double stdFiringRate;
    double minFiringRate;
    double maxFiringRate;
    size_t neuronCount;
    size_t synapseCount;
    size_t activeNeuronCount;
    size_t firingNeuronCount;
    size_t totalSpikes;
    float dopamineLevel;
    float curiosityLevel;
    float noveltyLevel;
    float predictionError;
    SimulationStep developmentalStage;
    size_t memoryEpisodesStored;
    size_t workingMemoryActiveTraces;
    double wallClockTime;
    std::chrono::system_clock::time_point completionTime;
    
    // Statistical analysis
    double coefficientOfVariation() const {
        if (avgFiringRate > 0) return stdFiringRate / avgFiringRate;
        return 0.0;
    }
    
    double firingRateEfficiency() const {
        if (totalSpikes > 0 && avgFiringRate > 0) {
            return static_cast<double>(firingNeuronCount) / activeNeuronCount;
        }
        return 0.0;
    }
};

// Parameter sweep configuration
struct ParameterSweep {
    std::string parameterName;
    std::vector<double> values;
    std::string distribution; // "linear", "log", "random", "exponential"
    double minValue;
    double maxValue;
    size_t sampleCount;
    
    ParameterSweep() : minValue(0.0), maxValue(1.0), sampleCount(10), distribution("linear") {}
};

// Advanced experiment runner with comprehensive features
class ExperimentRunner {
public:
    ExperimentRunner();
    ~ExperimentRunner();
    
    // Create new experiment with enhanced configuration
    std::shared_ptr<Experiment> createExperiment(const std::string& name);
    std::shared_ptr<Experiment> createExperiment(const ExperimentParams& params);
    
    // Run single experiment with enhanced error handling and logging
    bool runExperiment(std::shared_ptr<Experiment> experiment,
                       std::shared_ptr<Brain> brain,
                       std::shared_ptr<Environment> environment,
                       SimulationStep maxSteps,
                       bool detailedLogging = true);
    
    // Enhanced batch processing with parameter sweeping
    std::vector<ExperimentResult> runBatch(
        const std::vector<std::shared_ptr<Experiment>>& experiments,
        std::shared_ptr<Brain> brainTemplate,
        std::shared_ptr<Environment> environmentTemplate,
        SimulationStep maxSteps,
        const std::map<std::string, ParameterSweep>& parameterSweeps = {});
    
    // Parameter sweep analysis
    std::vector<ExperimentResult> runParameterSweep(
        const std::string& experimentTemplate,
        const std::vector<ParameterSweep>& sweeps,
        std::shared_ptr<Brain> brainTemplate,
        std::shared_ptr<Environment> environmentTemplate,
        SimulationStep maxSteps);
    
    // Statistical analysis of experiment results
    struct AnalysisResults {
        double meanTotalReward;
        double stdTotalReward;
        double meanFiringRate;
        double stdFiringRate;
        double meanEfficiency;
        double correlationMatrix[3][3]; // Reward vs Firing vs Efficiency
        std::string bestConfiguration;
        std::string worstConfiguration;
    };
    
    AnalysisResults analyzeResults(const std::vector<ExperimentResult>& results);
    
    // Result visualization data export
    std::string exportResultsCSV(const std::vector<ExperimentResult>& results) const;
    std::string exportResultsJSON(const std::vector<ExperimentResult>& results) const;
    
    // Get all experiments
    const std::vector<std::shared_ptr<Experiment>>& getExperiments() const;
    
    // Get experiment by name
    std::shared_ptr<Experiment> getExperiment(const std::string& name) const;
    
    // Get experiment results by name
    ExperimentResult getExperimentResult(const std::string& name) const;
    
    // Clear all experiments
    void clearExperiments();
    
    // Set random seed for reproducibility
    void setGlobalSeed(uint64_t seed);
    
    // Save experiment results in multiple formats
    bool saveResults(const std::string& filepath, const std::string& format = "csv") const;
    bool saveResultsJSON(const std::string& filepath, const std::vector<ExperimentResult>& results) const;
    bool saveResultsCSV(const std::string& filepath, const std::vector<ExperimentResult>& results) const;
    
    // Advanced configuration management
    void configureExperiment(const std::string& experimentName, const std::map<std::string, std::string>& config);
    std::map<std::string, std::string> getExperimentConfig(const std::string& experimentName) const;
    
    // Results filtering and querying
    std::vector<ExperimentResult> filterResults(const std::function<bool(const ExperimentResult&)>& predicate) const;
    ExperimentResult getBestResult(const std::vector<ExperimentResult>& results, const std::string& metric = "totalReward") const;
    ExperimentResult getWorstResult(const std::vector<ExperimentResult>& results, const std::string& metric = "totalReward") const;
    
    // Experiment quality assessment
    double assessExperimentQuality(const ExperimentResult& result) const;
    std::string getQualityLabel(double quality) const;
    
    // Integration with configuration system
    void applyConfigToBrain(std::shared_ptr<Brain> brain, const ExperimentParams& params) const;
    void applyConfigToEnvironment(std::shared_ptr<Environment> environment, const ExperimentParams& params) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper methods
    ExperimentResult runSingleExperimentInternal(
        std::shared_ptr<Experiment> experiment,
        std::shared_ptr<Brain> brain,
        std::shared_ptr<Environment> environment,
        SimulationStep maxSteps,
        const ExperimentParams& params);
    
    std::vector<double> generateParameterValues(const ParameterSweep& sweep) const;
    double interpolate(double x, double x0, double x1, double y0, double y1) const;
    std::string resultToString(const ExperimentResult& result) const;
};

} // namespace nlm
