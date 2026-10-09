#pragma once

/**
 * NLM Command Line Interface
 * 
 * This module provides a comprehensive command-line interface for NLM (Neural Learning Machine)
 * that supports multiple experiment types, configuration options, and advanced features.
 * 
 * This improves the original monolithic main.cpp by:
 * 1. Supporting multiple experiment types through a pluggable architecture
 * 2. Providing flexible configuration options
 * 3. Supporting batch experiment execution
 * 4. Adding advanced features for research and development
 * 5. Improving user experience with clear help and status reporting
 */

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/NeuralParameters.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "experiments/BasicConnectivityExperiment.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "environment/Environment.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <chrono>
#include <map>
#include <iomanip>

namespace nlm {

/**
 * Command-line arguments structure
 */
struct CLIArgs {
    // General options
    std::string configFile;
    std::string outputFile;
    bool verbose;
    bool quiet;
    std::string logLevel;
    
    // Experiment selection
    std::string experimentType;
    std::string preset;  // Parameter preset (standard, development, learning, etc.)
    
    // Experiment configuration
    uint64_t seed;
    uint64_t steps;
    size_t neuronCount;
    size_t regionCount;
    float connectionProbability;
    
    // Feature toggles
    bool enablePlasticity;
    bool enableStdp;
    bool enableDevelopment;
    bool enableCheckpointing;
    bool enableReplay;
    bool enableNeuromodulation;
    
    // Performance and output options
    bool benchmark;
    bool saveResults;
    std::string resultsFormat;
    
    // Advanced options
    bool expertMode;
    std::string customScript;
    std::vector<std::string> additionalArgs;
    
    CLIArgs() : verbose(false),
                quiet(false),
                seed(42),
                steps(1000),
                neuronCount(1000),
                regionCount(1),
                connectionProbability(0.1f),
                enablePlasticity(true),
                enableStdp(true),
                enableDevelopment(true),
                enableCheckpointing(false),
                enableReplay(false),
                enableNeuromodulation(true),
                benchmark(false),
                saveResults(false),
                resultsFormat("text"),
                expertMode(false) {}
};

/**
 * Available experiment types
 */
enum class ExperimentType {
    BasicConnectivity,
    Phase6Integration,
    Phase3Demo,
    Phase4Demo,
    Phase5Experiment,
    ScalingBenchmark,
    AblationStudy
};

/**
 * NLMCommandLineInterface - Main CLI handler
 */
class NLMCommandLineInterface {
public:
    NLMCommandLineInterface();
    ~NLMCommandLineInterface();
    
    /**
     * Parse command-line arguments
     */
    bool parseArguments(int argc, char** argv, CLIArgs& args);
    
    /**
     * Display help information
     */
    void showHelp() const;
    void showExperimentHelp() const;
    void showConfigHelp() const;
    void showAdvancedHelp() const;
    
    /**
     * Run the selected experiment
     */
    bool runExperiment(const CLIArgs& args);
    
    /**
     * Run batch of experiments
     */
    bool runBatchExperiments(const CLIArgs& args);
    
    /**
     * Save experiment results
     */
    bool saveResults(const std::string& filepath, const std::vector<std::shared_ptr<Experiment>>& experiments);
    
    /**
     * Load configuration from file
     */
    bool loadConfig(const std::string& filepath, CLIArgs& args);
    
    /**
     * Save configuration to file
     */
    bool saveConfig(const std::string& filepath, const CLIArgs& args) const;
    
    /**
     * Get experiment type from string
     */
    ExperimentType getExperimentType(const std::string& typeStr) const;
    
    /**
     * Get experiment type from string
     */
    std::string getExperimentTypeString(ExperimentType type) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * ExperimentFactory - Creates experiment instances
 */
class ExperimentFactory {
public:
    static std::shared_ptr<Experiment> createExperiment(ExperimentType type, const std::string& name);
    static std::vector<std::shared_ptr<Experiment>> createBatch(ExperimentType type, 
                                                                const std::vector<std::string>& names);
    static bool configureExperiment(std::shared_ptr<Experiment> experiment, 
                                   const CLIArgs& args, 
                                   std::shared_ptr<Brain> brainTemplate = nullptr);
    
private:
    ExperimentFactory() = delete;
    ExperimentFactory(const ExperimentFactory&) = delete;
    ExperimentFactory& operator=(const ExperimentFactory&) = delete;
};

/**
 * ResultsFormatter - Formats experiment results for different output formats
 */
class ResultsFormatter {
public:
    static std::string formatResults(const std::vector<std::shared_ptr<Experiment>>& experiments, 
                                    const std::string& format = "text");
    static std::string formatCSV(const std::vector<std::shared_ptr<Experiment>>& experiments);
    static std::string formatJSON(const std::vector<std::shared_ptr<Experiment>>& experiments);
    static std::string formatHTML(const std::vector<std::shared_ptr<Experiment>>& experiments);
    
private:
    ResultsFormatter() = delete;
    ResultsFormatter(const ResultsFormatter&) = delete;
    ResultsFormatter& operator=(const ResultsFormatter&) = delete;
};

} // namespace nlm
