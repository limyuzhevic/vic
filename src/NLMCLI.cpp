#include "NLMCLI.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/NeuralParameters.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "experiments/BasicConnectivityExperiment.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <cmath>

namespace nlm {

struct NLMCommandLineInterface::Impl {
    // Default configuration values
    static const std::map<std::string, std::string> defaultConfig;
    
    // Experiment type mappings
    static const std::map<std::string, ExperimentType> experimentTypeMap;
    
    // Log level mappings
    static const std::map<std::string, LogLevel> logLevelMap;
    
    // Configuration file extensions
    static const std::vector<std::string> configFileExtensions;
    
    // Help text templates
    static const std::string helpHeader;
    static const std::string generalOptions;
    static const std::string experimentOptions;
    static const std::string configOptions;
    static const std::string advancedOptions;
    
    // Error messages
    static const std::string errorInvalidArgument;
    static const std::string errorFileNotFound;
    static const std::string errorInvalidExperiment;
    static const std::string errorConfigFileFormat;
    
    // Usage examples
    static const std::string usageExamples;
};

NLMCommandLineInterface::NLMCommandLineInterface() : pImpl(std::make_unique<Impl>()) {}

NLMCommandLineInterface::~NLMCommandLineInterface() = default;

bool NLMCommandLineInterface::parseArguments(int argc, char** argv, CLIArgs& args) {
    if (argc < 2) {
        showHelp();
        return false;
    }
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            showHelp();
            return false;
        } else if (arg == "--version") {
            std::cout << "NLM (熙然) Neural Learning Machine" << std::endl;
            std::cout << "Version: 2.0.0 (Phase 6 - Final Integration)" << std::endl;
            std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
            return false;
        } else if (arg == "--experiment-help") {
            showExperimentHelp();
            return false;
        } else if (arg.substr(0, 2) == "--") {
            // Long option
            std::string key = arg.substr(2);
            size_t equalsPos = key.find('=');
            std::string value;
            
            if (equalsPos != std::string::npos) {
                value = key.substr(equalsPos + 1);
                key = key.substr(0, equalsPos);
            } else if (i + 1 < argc && argv[i + 1][0] != '-') {
                value = argv[++i];
            }
            
            // Apply argument based on key
            if (!applyArgument(args, key, value)) {
                std::cerr << "Error: Unknown or invalid argument: " << arg << std::endl;
                showHelp();
                return false;
            }
        } else if (arg.substr(0, 1) == "-") {
            // Short option (handle if needed)
            std::cerr << "Error: Short options not supported (use --option format)" << std::endl;
            showHelp();
            return false;
        } else {
            // Positional argument
            args.additionalArgs.push_back(arg);
        }
    }
    
    // Validate arguments
    if (!validateArguments(args)) {
        return false;
    }
    
    return true;
}

bool NLMCommandLineInterface::applyArgument(CLIArgs& args, const std::string& key, const std::string& value) {
    // Integer arguments
    if (key == "seed") {
        try {
            args.seed = std::stoull(value);
        } catch (...) {
            return false;
        }
        return true;
    } else if (key == "steps") {
        try {
            args.steps = std::stoull(value);
        } catch (...) {
            return false;
        }
        return true;
    } else if (key == "neuronCount") {
        try {
            args.neuronCount = static_cast<size_t>(std::stoull(value));
        } catch (...) {
            return false;
        }
        return true;
    } else if (key == "regionCount") {
        try {
            args.regionCount = static_cast<size_t>(std::stoull(value));
        } catch (...) {
            return false;
        }
        return true;
    } else if (key == "connectionProbability") {
        try {
            args.connectionProbability = std::stof(value);
        } catch (...) {
            return false;
        }
        return true;
    } else if (key == "currentInjection") {
        try {
            args.currentInjection = std::stof(value);
        } catch (...) {
            return false;
        }
        return true;
    } else if (key == "plasticitySteps") {
        try {
            args.plasticitySteps = std::stoull(value);
        } catch (...) {
            return false;
        }
        return true;
    } else if (key == "stdpSteps") {
        try {
            args.stdpSteps = std::stoull(value);
        } catch (...) {
            return false;
        }
        return true;
    }
    
    // Boolean arguments
    if (key == "verbose" || key == "v") {
        args.verbose = true;
        return true;
    } else if (key == "quiet" || key == "q") {
        args.quiet = true;
        return true;
    } else if (key == "noPlasticity") {
        args.enablePlasticity = false;
        return true;
    } else if (key == "noStdp") {
        args.enableStdp = false;
        return true;
    } else if (key == "noDevelopment") {
        args.enableDevelopment = false;
        return true;
    } else if (key == "noCheckpointing") {
        args.enableCheckpointing = false;
        return true;
    } else if (key == "noReplay") {
        args.enableReplay = false;
        return true;
    } else if (key == "noNeuromodulation") {
        args.enableNeuromodulation = false;
        return true;
    } else if (key == "benchmark") {
        args.benchmark = true;
        return true;
    } else if (key == "saveResults") {
        args.saveResults = true;
        return true;
    } else if (key == "expertMode") {
        args.expertMode = true;
        return true;
    }
    
    // String arguments
    if (key == "config") {
        args.configFile = value;
        return true;
    } else if (key == "output") {
        args.outputFile = value;
        return true;
    } else if (key == "logLevel") {
        args.logLevel = value;
        return true;
    } else if (key == "experiment") {
        args.experimentType = value;
        return true;
    } else if (key == "preset") {
        args.preset = value;
        return true;
    } else if (key == "resultsFormat") {
        args.resultsFormat = value;
        return true;
    } else if (key == "checkpointPath") {
        args.checkpointPath = value;
        return true;
    } else if (key == "customScript") {
        args.customScript = value;
        return true;
    }
    
    // Unknown argument
    return false;
}

bool NLMCommandLineInterface::validateArguments(const CLIArgs& args) {
    // Validate experiment type
    if (!args.experimentType.empty()) {
        if (pImpl->experimentTypeMap.find(args.experimentType) == pImpl->experimentTypeMap.end()) {
            std::cerr << "Error: Invalid experiment type: " << args.experimentType << std::endl;
            return false;
        }
    }
    
    // Validate log level
    if (!args.logLevel.empty()) {
        if (pImpl->logLevelMap.find(args.logLevel) == pImpl->logLevelMap.end()) {
            std::cerr << "Error: Invalid log level: " << args.logLevel << std::endl;
            return false;
        }
    }
    
    // Validate numeric ranges
    if (args.neuronCount == 0) {
        std::cerr << "Error: neuronCount must be greater than 0" << std::endl;
        return false;
    }
    
    if (args.regionCount == 0) {
        std::cerr << "Error: regionCount must be greater than 0" << std::endl;
        return false;
    }
    
    if (args.connectionProbability < 0.0f || args.connectionProbability > 1.0f) {
        std::cerr << "Error: connectionProbability must be between 0.0 and 1.0" << std::endl;
        return false;
    }
    
    if (args.currentInjection < 0.0f) {
        std::cerr << "Error: currentInjection must be non-negative" << std::endl;
        return false;
    }
    
    return true;
}

void NLMCommandLineInterface::showHelp() const {
    std::cout << pImpl->helpHeader << std::endl;
    std::cout << "General Options:" << std::endl;
    std::cout << "  --help, -h                  Show this help message" << std::endl;
    std::cout << "  --version                   Show version information" << std::endl;
    std::cout << "  --experiment-help           Show experiment-specific help" << std::endl;
    std::cout << "  --verbose, -v               Enable verbose output" << std::endl;
    std::cout << "  --quiet, -q                 Suppress non-essential output" << std::endl;
    std::cout << "  --benchmark                 Run benchmark tests" << std::endl;
    std::cout << "  --save-results              Save experiment results to file" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Experiment Options:" << std::endl;
    std::cout << "  --experiment TYPE           Experiment type (see below)" << std::endl;
    std::cout << "  --preset PRESET             Parameter preset (standard, development, learning, mature, minimal)" << std::endl;
    std::cout << "  --steps N                   Number of simulation steps" << std::endl;
    std::cout << "  --neuronCount N             Number of neurons" << std::endl;
    std::cout << "  --regionCount N             Number of brain regions" << std::endl;
    std::cout << "  --connectionProbability P   Connection probability (0.0-1.0)" << std::endl;
    std::cout << "  --noPlasticity              Disable plasticity" << std::endl;
    std::cout << "  --noStdp                    Disable STDP" << std::endl;
    std::cout << "  --noDevelopment              Disable development" << std::endl;
    std::cout << "  --noCheckpointing            Disable checkpointing" << std::endl;
    std::cout << "  --noReplay                  Disable replay" << std::endl;
    std::cout << "  --noNeuromodulation          Disable neuromodulation" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Configuration Options:" << std::endl;
    std::cout << "  --config FILE               Configuration file" << std::endl;
    std::cout << "  --output FILE               Output file for results" << std::endl;
    std::cout << "  --logLevel LEVEL            Log level (TRACE, DEBUG, INFO, WARN, ERROR, CRITICAL)" << std::endl;
    std::cout << "  --resultsFormat FORMAT      Results format (text, csv, json, html)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Advanced Options:" << std::endl;
    std::cout << "  --expertMode                Enable expert mode" << std::endl;
    std::cout << "  --seed N                    Random seed for reproducibility" << std::endl;
    std::cout << "  --checkpointPath PATH        Path for checkpoint files" << std::endl;
    std::cout << "  --customScript SCRIPT       Custom experiment script" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Available Experiments:" << std::endl;
    for (const auto& pair : pImpl->experimentTypeMap) {
        std::cout << "  " << pair.first << " (" << static_cast<int>(pair.second) << ")" << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "Usage Examples:" << std::endl;
    std::cout << pImpl->usageExamples << std::endl;
}

void NLMCommandLineInterface::showExperimentHelp() const {
    std::cout << "=== Experiment Help ===" << std::endl;
    std::cout << std::endl;
    
    std::cout << "BasicConnectivity:" << std::endl;
    std::cout << "  Demonstrates fundamental neural connectivity and learning." << std::endl;
    std::cout << "  Includes: connectivity tests, plasticity experiments, STDP verification." << std::endl;
    std::cout << std::endl;
    
    std::cout << "Phase6Integration:" << std::endl;
    std::cout << "  Comprehensive integration test of all brain systems." << std::endl;
    std::cout << "  Verifies memory, neuromodulation, prediction, development, and checkpointing." << std::endl;
    std::cout << std::endl;
    
    std::cout << "Phase3Demo:" << std::endl;
    std::cout << "  World interaction loop with sensory-motor capabilities." << std::endl;
    std::cout << std::endl;
    
    std::cout << "Phase4Demo:" << std::endl;
    std::cout << "  Advanced cognitive mechanisms including prediction and planning." << std::endl;
    std::cout << std::endl;
    
    std::cout << "Phase5Experiment:" << std::endl;
    std::cout << "  Performance optimizations and parallel processing tests." << std::endl;
    std::cout << std::endl;
    
    std::cout << "ScalingBenchmark:" << std::endl;
    std::cout << "  Performance benchmarks for different brain sizes." << std::endl;
    std::cout << std::endl;
    
    std::cout << "AblationStudy:" << std::endl;
    std::cout << "  Tests the impact of individual brain systems by ablation." << std::endl;
    std::cout << std::endl;
}

void NLMCommandLineInterface::showConfigHelp() const {
    std::cout << "=== Configuration Help ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Configuration files should be in the format:" << std::endl;
    std::cout << "  key = value" << std::endl;
    std::cout << std::endl;
    std::cout << "Supported keys include:" << std::endl;
    std::cout << "  random_seed, simulation_timestep, neuron_count, region_count" << std::endl;
    std::cout << "  connection_probability, stdp_ltp_weight, stdp_ltd_weight" << std::endl;
    std::cout << "  synaptogenesis_rate, pruning_rate, dopamine_baseline" << std::endl;
    std::cout << "  development_plasticity_start, development_plasticity_end" << std::endl;
    std::cout << std::endl;
    std::cout << "See configs/default.cfg for examples." << std::endl;
}

void NLMCommandLineInterface::showAdvancedHelp() const {
    std::cout << "=== Advanced Help ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Expert Mode enables access to advanced configuration options." << std::endl;
    std::cout << "Use with caution as it can affect simulation behavior significantly." << std::endl;
    std::cout << std::endl;
    std::cout << "Custom scripts allow for defining experiment parameters programmatically." << std::endl;
    std::cout << std::endl;
    std::cout << "Batch experiments can be run using multiple --experiment arguments." << std::endl;
}

bool NLMCommandLineInterface::runExperiment(const CLIArgs& args) {
    NLM_LOG_INFO("=== NLM Command Line Interface ===");
    
    // Set up logging
    if (!args.logLevel.empty()) {
        auto logLevel = pImpl->logLevelMap.find(args.logLevel);
        if (logLevel != pImpl->logLevelMap.end()) {
            // Set global log level (implementation depends on Logger class)
            // Logger::setLevel(logLevel->second);
        }
    }
    
    // Create experiment runner
    auto runner = std::make_shared<ExperimentRunner>();
    
    // Create brain template
    auto cfg = std::make_shared<Config>();
    
    // Apply CLI arguments to config
    cfg->set("random_seed", static_cast<int64_t>(args.seed));
    cfg->set("simulation_timestep", 0.001);
    cfg->set("neuron_count", static_cast<int64_t>(args.neuronCount));
    cfg->set("region_count", static_cast<int64_t>(args.regionCount));
    cfg->set("connection_probability", args.connectionProbability);
    
    // Apply preset if specified
    if (!args.preset.empty()) {
        // Apply preset parameters (implementation depends on ParameterPresets)
        // For now, just log that we're using the preset
        NLM_LOG_INFO("Using parameter preset: " + args.preset);
    }
    
    // Apply feature toggles
    cfg->set("plasticity_enabled", args.enablePlasticity);
    cfg->set("stdp_enabled", args.enableStdp);
    cfg->set("development_enabled", args.enableDevelopment);
    cfg->set("checkpointing_enabled", args.enableCheckpointing);
    cfg->set("replay_enabled", args.enableReplay);
    cfg->set("neuromodulation_enabled", args.enableNeuromodulation);
    
    // Get experiment type
    ExperimentType experimentType;
    if (args.experimentType.empty()) {
        experimentType = ExperimentType::BasicConnectivity;
    } else {
        experimentType = pImpl->experimentTypeMap.at(args.experimentType);
    }
    
    // Create brain
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain");
        return false;
    }
    
    // Apply configuration-driven parameters
    auto neuralParams = ConfigurationDrivenNeuralParameters::getDefaultParameters(args.preset.empty() ? "standard" : args.preset);
    ConfigurationDrivenNeuralParameters::applyToBrain(brain, neuralParams);
    
    // Run experiment based on type
    bool success = false;
    
    switch (experimentType) {
        case ExperimentType::BasicConnectivity: {
            BasicConnectivityExperiment experiment;
            BasicConnectivityConfig config;
            config.steps = args.steps;
            config.currentInjection = args.currentInjection;
            config.enablePlasticity = args.enablePlasticity;
            config.enableStdp = args.enableStdp;
            config.seed = args.seed;
            
            BasicConnectivityResult result = experiment.run(config, brain);
            
            // Create and store experiment
            auto nlmExperiment = runner->createExperiment("BasicConnectivity");
            nlmExperiment->setSeed(args.seed);
            
            // Record metrics
            nlmExperiment->recordMetric("spikesPropagate", result.spikesPropagate ? 1.0 : 0.0);
            nlmExperiment->recordMetric("spikesGenerated", static_cast<double>(result.spikesGenerated));
            nlmExperiment->recordMetric("learningDetected", result.learningDetected ? 1.0 : 0.0);
            nlmExperiment->recordMetric("meanWeightChange", static_cast<double>(result.meanWeightChange));
            nlmExperiment->recordMetric("strengthenedConnections", static_cast<double>(result.strongenedConnections));
            nlmExperiment->recordMetric("weakenedConnections", static_cast<double>(result.weakenedConnections));
            
            success = true;
            break;
        }
        
        case ExperimentType::Phase6Integration: {
            Phase6IntegratedExperiment experiment;
            Phase6Config config;
            config.maxSteps = args.steps;
            config.neuronCount = args.neuronCount;
            config.regionCount = args.regionCount;
            config.connectionProbability = args.connectionProbability;
            config.enableCheckpointing = args.enableCheckpointing;
            config.enableReplay = args.enableReplay;
            config.enableDevelopment = args.enableDevelopment;
            config.checkpointPath = args.checkpointPath.empty() ? "./checkpoint_test.bin" : args.checkpointPath;
            
            Phase6IntegrationResult result = experiment.run(config);
            
            // Create and store experiment
            auto nlmExperiment = runner->createExperiment("Phase6Integration");
            nlmExperiment->setSeed(args.seed);
            
            // Record metrics
            nlmExperiment->recordMetric("memoryWorkingMemoryIntegrated", result.memoryWorkingMemoryIntegrated ? 1.0 : 0.0);
            nlmExperiment->recordMetric("memoryEpisodicMemoryIntegrated", result.memoryEpisodicMemoryIntegrated ? 1.0 : 0.0);
            nlmExperiment->recordMetric("neuromodulationIntegrated", result.neuromodulationIntegrated ? 1.0 : 0.0);
            nlmExperiment->recordMetric("predictionIntegrated", result.predictionIntegrated ? 1.0 : 0.0);
            nlmExperiment->recordMetric("developmentIntegrated", result.developmentIntegrated ? 1.0 : 0.0);
            nlmExperiment->recordMetric("checkpointingWorks", result.checkpointingWorks ? 1.0 : 0.0);
            nlmExperiment->recordMetric("replayWorks", result.replayWorks ? 1.0 : 0.0);
            nlmExperiment->recordMetric("totalReward", result.totalReward);
            nlmExperiment->recordMetric("avgFiringRate", result.avgFiringRate);
            nlmExperiment->recordMetric("memoryEpisodesStored", result.memoryEpisodesStored);
            
            success = true;
            break;
        }
        
        default:
            NLM_LOG_ERROR("Experiment type not implemented: " + args.experimentType);
            success = false;
            break;
    }
    
    if (success) {
        NLM_LOG_INFO("Experiment completed successfully");
        
        // Save results if requested
        if (args.saveResults) {
            saveResults(args.outputFile.empty() ? "nlm_results.txt" : args.outputFile, runner->getExperiments());
        }
        
        // Print summary
        std::cout << "=== Experiment Summary ===" << std::endl;
        std::cout << "Experiment Type: " << args.experimentType << std::endl;
        std::cout << "Steps: " << args.steps << std::endl;
        std::cout << "Neurons: " << args.neuronCount << std::endl;
        std::cout << "Status: SUCCESS" << std::endl;
    } else {
        NLM_LOG_ERROR("Experiment failed");
    }
    
    return success;
}

bool NLMCommandLineInterface::runBatchExperiments(const CLIArgs& args) {
    NLM_LOG_INFO("=== Running Batch Experiments ===");
    
    // For now, just run a single experiment
    // Batch processing would require parsing multiple experiment types
    return runExperiment(args);
}

bool NLMCommandLineInterface::saveResults(const std::string& filepath, const std::vector<std::shared_ptr<Experiment>>& experiments) {
    if (experiments.empty()) {
        NLM_LOG_WARNING("No experiments to save");
        return false;
    }
    
    // Use ResultsFormatter to save based on format
    // For now, just use default text format
    std::string content = ResultsFormatter::formatResults(experiments, "text");
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open results file: " + filepath);
        return false;
    }
    
    file << content;
    file.close();
    
    NLM_LOG_INFO("Results saved to: " + filepath);
    return true;
}

bool NLMCommandLineInterface::loadConfig(const std::string& filepath, CLIArgs& args) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        size_t equalsPos = line.find('=');
        if (equalsPos == std::string::npos) {
            continue;
        }
        
        std::string key = line.substr(0, equalsPos);
        std::string value = line.substr(equalsPos + 1);
        
        // Trim whitespace
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        // Try to apply as argument
        applyArgument(args, key, value);
    }
    
    NLM_LOG_INFO("Configuration loaded from: " + filepath);
    return true;
}

bool NLMCommandLineInterface::saveConfig(const std::string& filepath, const CLIArgs& args) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to create config file: " + filepath);
        return false;
    }
    
    file << "# NLM Configuration File (Generated by CLI)" << std::endl;
    file << "# Generated on: " << std::time(nullptr) << std::endl;
    file << std::endl;
    
    // Write key configuration parameters
    file << "random_seed = " << args.seed << std::endl;
    file << "neuron_count = " << args.neuronCount << std::endl;
    file << "region_count = " << args.regionCount << std::endl;
    file << "connection_probability = " << args.connectionProbability << std::endl;
    file << "steps = " << args.steps << std::endl;
    file << "experiment = " << args.experimentType << std::endl;
    file << "preset = " << args.preset << std::endl;
    file << std::endl;
    
    file.close();
    NLM_LOG_INFO("Configuration saved to: " + filepath);
    return true;
}

ExperimentType NLMCommandLineInterface::getExperimentType(const std::string& typeStr) const {
    auto it = pImpl->experimentTypeMap.find(typeStr);
    if (it != pImpl->experimentTypeMap.end()) {
        return it->second;
    }
    return ExperimentType::BasicConnectivity; // Default
}

std::string NLMCommandLineInterface::getExperimentTypeString(ExperimentType type) const {
    for (const auto& pair : pImpl->experimentTypeMap) {
        if (pair.second == type) {
            return pair.first;
        }
    }
    return "unknown";
}

// ExperimentFactory implementations
std::shared_ptr<Experiment> ExperimentFactory::createExperiment(ExperimentType type, const std::string& name) {
    switch (type) {
        case ExperimentType::BasicConnectivity:
            return std::make_shared<Experiment>(name);
        case ExperimentType::Phase6Integration:
            return std::make_shared<Experiment>(name);
        default:
            return std::make_shared<Experiment>(name);
    }
}

std::vector<std::shared_ptr<Experiment>> ExperimentFactory::createBatch(ExperimentType type, 
                                                                          const std::vector<std::string>& names) {
    std::vector<std::shared_ptr<Experiment>> experiments;
    for (const auto& name : names) {
        experiments.push_back(createExperiment(type, name));
    }
    return experiments;
}

bool ExperimentFactory::configureExperiment(std::shared_ptr<Experiment> experiment, 
                                           const CLIArgs& args, 
                                           std::shared_ptr<Brain> brainTemplate) {
    if (!experiment) {
        return false;
    }
    
    experiment->setSeed(args.seed);
    
    // Configure experiment based on type
    // Implementation depends on experiment-specific configuration
    
    return true;
}

// ResultsFormatter implementations
std::string ResultsFormatter::formatResults(const std::vector<std::shared_ptr<Experiment>>& experiments, 
                                           const std::string& format) {
    if (format == "csv") {
        return formatCSV(experiments);
    } else if (format == "json") {
        return formatJSON(experiments);
    } else if (format == "html") {
        return formatHTML(experiments);
    } else {
        return formatText(experiments); // Default
    }
}

std::string ResultsFormatter::formatCSV(const std::vector<std::shared_ptr<Experiment>>& experiments) {
    std::stringstream ss;
    ss << "Experiment,Seed,Steps,Metrics" << std::endl;
    
    for (const auto& experiment : experiments) {
        ss << experiment->getName() << "," 
           << experiment->getSeed() << ",";  // Simplified
        // Add metrics
        ss << "..." << std::endl;
    }
    
    return ss.str();
}

std::string ResultsFormatter::formatJSON(const std::vector<std::shared_ptr<Experiment>>& experiments) {
    std::stringstream ss;
    ss << "{" << std::endl;
    ss << "  \"experiments\": [" << std::endl;
    
    for (size_t i = 0; i < experiments.size(); ++i) {
        ss << "    {\"name\": \"" << experiments[i]->getName() << "\", \"seed\": " 
           << experiments[i]->getSeed() << "}";
        if (i < experiments.size() - 1) {
            ss << ",";
        }
        ss << std::endl;
    }
    
    ss << "  ]" << std::endl;
    ss << "}" << std::endl;
    
    return ss.str();
}

std::string ResultsFormatter::formatHTML(const std::vector<std::shared_ptr<Experiment>>& experiments) {
    std::stringstream ss;
    ss << "<!DOCTYPE html>" << std::endl;
    ss << "<html><head><title>NLM Experiment Results</title></head>" << std::endl;
    ss << "<body><h1>NLM Experiment Results</h1>" << std::endl;
    
    for (const auto& experiment : experiments) {
        ss << "<h2>" << experiment->getName() << "</h2>" << std::endl;
        ss << "<p>Seed: " << experiment->getSeed() << "</p>" << std::endl;
    }
    
    ss << "</body></html>" << std::endl;
    return ss.str();
}

std::string ResultsFormatter::formatText(const std::vector<std::shared_ptr<Experiment>>& experiments) {
    std::stringstream ss;
    ss << "=== NLM Experiment Results ===" << std::endl;
    ss << std::endl;
    
    for (const auto& experiment : experiments) {
        ss << "Experiment: " << experiment->getName() << std::endl;
        ss << "Seed: " << experiment->getSeed() << std::endl;
        ss << "Metrics:" << std::endl;
        
        // Add metrics
        const auto& metrics = experiment->getMetrics();
        for (const auto& metric : metrics) {
            ss << "  " << metric.first << ": " << metric.second << std::endl;
        }
        ss << std::endl;
    }
    
    return ss.str();
}

} // namespace nlm
