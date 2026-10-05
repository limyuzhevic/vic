#pragma once

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <unordered_map>
#include <unordered_set>

namespace nlm {

/**
 * ExperimentResult - Standardized result structure for all experiments
 */
struct ExperimentResult {
    std::string experimentName;
    std::string description;
    bool success;
    std::string errorMessage;
    double executionTime;
    std::unordered_map<std::string, double> metrics;
    std::unordered_map<std::string, std::string> info;
    
    ExperimentResult(const std::string& name, const std::string& desc)
        : experimentName(name), description(desc), success(false), executionTime(0.0) {}
};

/**
 * LearningExperiment - Structured learning experiment with advanced analysis
 */
class LearningExperiment {
public:
    LearningExperiment(std::shared_ptr<Brain> brain, uint64_t seed)
        : brain(brain), seed(seed) {}
    
    void recordInitialState();
    void recordFinalState();
    void computeStatistics();
    double calculateLearningScore() const;
    std::string generateReport() const;
    
    // Accessors
    const std::vector<float>& getInitialWeights() const { return initialWeights; }
    const std::vector<float>& getFinalWeights() const { return finalWeights; }
    uint64_t getSeed() const { return seed; }
    
private:
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
};

/**
 * ExperimentManager - Manages multiple experiment runs and results
 */
class ExperimentManager {
public:
    ExperimentManager(std::shared_ptr<Brain> brain) : brain(brain) {}
    
    ExperimentResult runBasicConnectivityTest();
    ExperimentResult runPlasticityExperiment();
    ExperimentResult runStdpVerification();
    ExperimentResult runIntegrationVerification();
    ExperimentResult runPerformanceBenchmark();
    ExperimentResult runMemoryAnalysis();
    
    // Advanced experiment capabilities
    ExperimentResult runLearningExperiment(uint64_t seed = 42);
    ExperimentResult runDevelopmentalExperiment(const std::string& stage);
    ExperimentResult runNeuromodulationExperiment();
    ExperimentResult runReplayExperiment();
    
    std::vector<ExperimentResult> runExperimentSuite(const std::vector<std::string>& experimentNames);
    std::string exportResults(const std::vector<ExperimentResult>& results) const;
    
private:
    std::shared_ptr<Brain> brain;
    std::chrono::high_resolution_clock::time_point startTime;
};

/**
 * CommandLineInterface - Advanced CLI for NLM with multiple modes
 */
class CommandLineInterface {
public:
    CommandLineInterface() {}
    
    int run(int argc, char** argv);
    void printHelp() const;
    void printVersion() const;
    
private:
    struct CLIConfig {
        std::string experimentMode;
        std::string configFile;
        std::string outputFile;
        std::string checkpointPath;
        bool verbose;
        bool quiet;
        bool jsonOutput;
        uint64_t seed;
        size_t neuronCount;
        size_t maxSteps;
        bool enableDevelopment;
        bool enableCheckpointing;
        bool enableReplay;
        
        CLIConfig() : verbose(false), quiet(false), jsonOutput(false), seed(42), 
                     neuronCount(1000), maxSteps(10000), enableDevelopment(true),
                     enableCheckpointing(true), enableReplay(true) {}
    };
    
    CLIConfig parseArguments(int argc, char** argv);
    void executeExperiment(const CLIConfig& config);
    void saveResults(const std::vector<ExperimentResult>& results, const std::string& filename);
};

} // namespace nlm
