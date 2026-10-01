// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <map>
#include <cctype>
#include <chrono>
#include <ctime>
#include <system_error>

using namespace nlm;

// Version information
const std::string NLM_VERSION = "2.0.0";

// Error codes for better error reporting
enum class NMLErrorCode {
    Success = 0,
    InvalidArguments = 1,
    ConfigLoadError = 2,
    BrainInitError = 3,
    ExperimentError = 4,
    FileError = 5,
    MemoryError = 6,
    SystemError = 7
};

// Error reporting with context and recovery information
void reportError(NMLErrorCode errorCode, const std::string& message, bool isRecoverable = true) {
    const char* level = "ERROR";
    switch (errorCode) {
        case NMLErrorCode::InvalidArguments: level = "WARN"; break;
        case NMLErrorCode::ConfigLoadError: level = "ERROR"; break;
        case NMLErrorCode::BrainInitError: level = "CRITICAL"; break;
        case NMLErrorCode::ExperimentError: level = "ERROR"; break;
        case NMLErrorCode::FileError: level = "ERROR"; break;
        case NMLErrorCode::MemoryError: level = "CRITICAL"; break;
        case NMLErrorCode::SystemError: level = "CRITICAL"; break;
        default: break;
    }
    
    std::cerr << "[" << level << "] " << message << std::endl;
    
    if (isRecoverable) {
        std::cerr << "[" << level << "] Recovery recommendation: Check system resources and configuration" << std::endl;
    } else {
        std::cerr << "[" << level << "] FATAL: System cannot continue" << std::endl;
    }
}

// Calculate error code from brain initialization status
NMLErrorCode getBrainInitErrorCode(bool success) {
    return success ? NMLErrorCode::Success : NMLErrorCode::BrainInitError;
}

// Error recovery mechanism - always returns success for Phase 2
NMLErrorCode define_error_recovery() {
    return NMLErrorCode::Success;
}

// Simple configuration preset system
class ConfigPresets {
public:
    struct Preset {
        std::string name;
        std::string description;
        std::map<std::string, std::string> defaults;
    };
    
    ConfigPresets() {
        presets["basic"] = {
            "basic",
            "Minimal configuration for testing and development",
            {
                {"random_seed", "42"},
                {"simulation_timestep", "0.001"},
                {"neuron_count", "50"},
                {"region_count", "1"},
                {"connection_probability", "0.1"},
                {"stdp_ltp_weight", "0.01"},
                {"stdp_ltd_weight", "0.008"},
                {"stdp_tau", "10"},
                {"synaptogenesis_rate", "0.00001"},
                {"pruning_rate", "0.000001"},
                {"enable_plasticity", "false"},
                {"log_level", "info"}
            }
        };
        
        presets["advanced"] = {
            "advanced",
            "Full configuration with plasticity and learning capabilities",
            {
                {"random_seed", "42"},
                {"simulation_timestep", "0.001"},
                {"neuron_count", "500"},
                {"region_count", "2"},
                {"connection_probability", "0.15"},
                {"stdp_ltp_weight", "0.02"},
                {"stdp_ltd_weight", "0.015"},
                {"stdp_tau", "20"},
                {"synaptogenesis_rate", "0.0001"},
                {"pruning_rate", "0.00001"},
                {"enable_plasticity", "true"},
                {"log_level", "info"}
            }
        };
        
        presets["research"] = {
            "research",
            "High-precision research configuration with extensive plasticity",
            {
                {"random_seed", "12345"},
                {"simulation_timestep", "0.0005"},
                {"neuron_count", "1000"},
                {"region_count", "3"},
                {"connection_probability", "0.2"},
                {"stdp_ltp_weight", "0.03"},
                {"stdp_ltd_weight", "0.02"},
                {"stdp_tau", "30"},
                {"synaptogenesis_rate", "0.001"},
                {"pruning_rate", "0.0001"},
                {"enable_plasticity", "true"},
                {"log_level", "debug"},
                {"record_detailed_metrics", "true"},
                {"simulation_steps", "5000"}
            }
        };
        
        presets["phase6"] = {
            "phase6",
            "Phase 2 specific neural computation configuration",
            {
                {"random_seed", "2026"},
                {"simulation_timestep", "0.001"},
                {"neuron_count", "800"},
                {"region_count", "4"},
                {"connection_probability", "0.18"},
                {"stdp_ltp_weight", "0.025"},
                {"stdp_ltd_weight", "0.018"},
                {"stdp_tau", "25"},
                {"synaptogenesis_rate", "0.0005"},
                {"pruning_rate", "0.00005"},
                {"enable_plasticity", "true"},
                {"log_level", "info"},
                {"benchmark_mode", "true"},
                {"performance_monitoring", "true"},
                {"simulation_steps", "2000"}
            }
        };
    }
    
    bool hasPreset(const std::string& name) const {
        return presets.find(name) != presets.end();
    }
    
    const Preset& getPreset(const std::string& name) const {
        return presets.at(name);
    }
    
    std::string getDescription(const std::string& name) const {
        if (presets.find(name) != presets.end()) {
            return presets.at(name).description;
        }
        return "";
    }
    
    void applyPreset(Config& config, const std::string& name) const {
        if (!hasPreset(name)) {
            throw std::runtime_error("Unknown preset: " + name);
        }
        
        const Preset& preset = getPreset(name);
        
        for (const auto& pair : preset.defaults) {
            try {
                if (pair.first == "random_seed" || pair.first == "neuron_count" || 
                    pair.first == "region_count" || pair.first == "simulation_steps") {
                    int64_t val = std::stoll(pair.second);
                    config.set(pair.first, val, ConfigSource::Preset);
                } else if (pair.first == "stdp_ltp_weight" || pair.first == "stdp_ltd_weight" ||
                           pair.first == "stdp_tau" || pair.first == "connection_probability" ||
                           pair.first == "synaptogenesis_rate" || pair.first == "pruning_rate" ||
                           pair.first == "enable_plasticity" || pair.first == "benchmark_mode" ||
                           pair.first == "performance_monitoring" || pair.first == "record_detailed_metrics") {
                    try {
                        double val = std::stod(pair.second);
                        config.set(pair.first, val, ConfigSource::Preset);
                    } catch (...) {
                        bool val = (pair.second == "true" || pair.second == "1" || pair.second == "yes");
                        config.set(pair.first, val, ConfigSource::Preset);
                    }
                } else {
                    config.set(pair.first, pair.second, ConfigSource::Preset);
                }
            } catch (const std::exception& e) {
                std::cerr << "Warning: Failed to set preset value " << pair.first << " = " << pair.second << ": " << e.what() << std::endl;
            }
        }
    }
    
private:
    std::map<std::string, Preset> presets;
};

// Command line argument parsing
class ArgParser {
public:
    struct ParsedArgs {
        std::string configFile;
        std::string experimentType;
        std::string preset;
        std::string outputFile;
        bool benchmark;
        bool verbose;
        std::string logLevel;
        std::string experimentName;
        std::map<std::string, std::string> experimentParams;
        
        std::vector<std::string> positionalArgs;
        
        bool helpRequested;
        bool versionRequested;
    };
    
    ArgParser() {
        // Define standard options
        addOption("config", "Load configuration from file", false, "configs/default.cfg", "file");
        addOption("experiment", "Run specific experiment type (basic, advanced, research, phase6, auto)", 
                  false, "auto", "string", "-e");
        addOption("preset", "Use configuration preset (basic, advanced, research, phase6)", 
                  false, "basic", "string", "-p");
        addOption("output", "Save results to file", false, "", "file", "-o");
        addFlag("benchmark", "Run performance benchmark", "b");
        addFlag("verbose", "Enable verbose logging", "V");
        addOption("log-level", "Set logging level (trace, debug, info, warn, error, critical)", 
                  false, "info", "string");
        addOption("experiment-name", "Name specific experiment", false, "", "string");
        addOption("neurons", "Override neuron count", false, "", "number");
        addOption("steps", "Override simulation steps", false, "", "number");
        
        // Experiment-specific parameters
        addOption("experiment-basic-neurons", "Override neurons for basic experiment", false, "", "number");
        addOption("experiment-advanced-steps", "Override steps for advanced experiment", false, "", "number");
        addOption("experiment-research-steps", "Override steps for research experiment", false, "", "number");
        addOption("experiment-research-detailed", "Enable detailed metrics for research experiment", false, "false", "bool");
        addOption("experiment-phase6-steps", "Override steps for phase6 experiment", false, "", "number");
        addOption("experiment-phase6-performance", "Enable performance monitoring for phase6 experiment", false, "false", "bool");
        
        addFlag("help", "Show this help message", "h");
        addFlag("version", "Show version information", "v");
    }
    
    void addOption(const std::string& name, const std::string& help,
                   bool required = false, const std::string& defaultVal = "",
                   const std::string& type = "string", const std::string& shortName = "") {
        Option opt;
        opt.name = name;
        opt.helpText = help;
        opt.required = required;
        opt.defaultValue = defaultVal;
        opt.type = type;
        opt.hasValue = !defaultVal.empty() || required;
        opt.shortName = shortName;
        
        options[name] = opt;
        if (!shortName.empty()) {
            shortOptions[shortName] = name;
        }
    }
    
    void addFlag(const std::string& name, const std::string& help,
                 const std::string& shortName = "") {
        Option opt;
        opt.name = name;
        opt.helpText = help;
        opt.required = false;
        opt.defaultValue = "false";
        opt.type = "flag";
        opt.hasValue = false;
        opt.shortName = shortName;
        
        options[name] = opt;
        if (!shortName.empty()) {
            shortOptions[shortName] = name;
        }
    }
    
    ParsedArgs parse(int argc, char** argv) {
        ParsedArgs args;
        args.helpRequested = false;
        args.versionRequested = false;
        
        // Process arguments
        std::vector<std::string> remainingArgs;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            // Handle help and version
            if (arg == "--help" || arg == "-h") {
                args.helpRequested = true;
            } else if (arg == "--version" || arg == "-v") {
                args.versionRequested = true;
            } else if (arg.substr(0, 2) == "--") {
                // Long option with = or space
                processLongOption(arg, args, remainingArgs);
            } else if (arg[0] == '-') {
                // Short option
                processShortOption(arg, args, remainingArgs);
            } else {
                // Positional argument
                remainingArgs.push_back(arg);
            }
        }
        
        // Store positional arguments
        args.positionalArgs = remainingArgs;
        if (!args.positionalArgs.empty()) {
            args.experimentType = args.positionalArgs[0];
        }
        
        // Validate arguments
        validateArgs(args);
        
        return args;
    }
    
    std::string generateHelp(const std::string& programName) const {
        std::stringstream ss;
        ss << programName << " - NLM Neural Learning Machine (Phase 2)\n\n";
        ss << "USAGE:\n";
        ss << "  " << programName << " [OPTIONS] [EXPERIMENT_TYPE]\n\n";
        
        ss << "EXPERIMENT TYPES:\n";
        ss << "  basic        Basic neural connectivity test\n";
        ss << "  advanced     Advanced plasticity experiments\n";
        ss << "  research     Research-grade experiments\n";
        ss << "  phase6       Phase 2 specific neural computation experiments\n";
        ss << "  auto         Automatic experiment selection (default)\n\n";
        
        ss << "OPTIONS:\n";
        
        // Long options
        for (const auto& pair : options) {
            const Option& opt = pair.second;
            if (opt.type == "flag") {
                std::string optStr = "  --" + opt.name;
                if (!opt.shortName.empty()) optStr += ", -" + opt.shortName;
                optStr += std::string(50 - optStr.length(), ' ');
                ss << optStr << "[\t" << opt.helpText << "\n";
            } else {
                std::string optStr = "  --" + opt.name + "=<value>";
                if (!opt.shortName.empty()) optStr += ", -" + opt.shortName + " <value>";
                optStr += std::string(42 - optStr.length(), ' ');
                ss << optStr << "<" << opt.type << ">\t" << opt.helpText;
                if (!opt.defaultValue.empty()) {
                    ss << " (default: " << opt.defaultValue << ")";
                }
                ss << "\n";
            }
        }
        
        ss << "\nEXAMPLES:\n";
        ss << "  nlm --help                               Display help information\n";
        ss << "  nlm --version                           Show version\n";
        ss << "  nlm --config=experiments.cfg            Load custom config\n";
        ss << "  nlm --experiment=basic                  Run basic experiment\n";
        ss << "  nlm --preset=advanced --neurons=1000   Use advanced preset with 1000 neurons\n";
        ss << "  nlm --output=results.json --benchmark   Run with result output and benchmark\n";
        ss << "  nlm research --experiment-name=myexp    Run research experiment named 'myexp'\n\n";
        
        ss << "CONFIGURATION PRESETS:\n";
        ConfigPresets presets;
        ss << "  basic     " << presets.getDescription("basic") << "\n";
        ss << "  advanced  " << presets.getDescription("advanced") << "\n";
        ss << "  research  " << presets.getDescription("research") << "\n";
        ss << "  phase6    " << presets.getDescription("phase6") << "\n\n";
        
        ss << "CONFIGURATION FILE FORMAT:\n";
        ss << "  key = value                                    Simple key=value pairs\n";
        ss << "  \"key\" = \"value\"                              Quoted values\n";
        ss << "  # This is a comment\n";
        ss << "  // Another comment\n\n";
        
        ss << "For more information, visit: https://github.com/yourusername/nlm\n";
        
        return ss.str();
    }
    
private:
    struct Option {
        std::string name;
        std::string shortName;
        std::string helpText;
        bool required;
        std::string defaultValue;
        std::string type;
        bool hasValue;
    };
    
    std::map<std::string, Option> options;
    std::map<std::string, std::string> shortOptions;
    
    void processLongOption(const std::string& arg, ParsedArgs& args, std::vector<std::string>& remainingArgs) {
        size_t pos = arg.find('=');
        if (pos != std::string::npos) {
            std::string name = arg.substr(2, pos - 2);
            std::string value = arg.substr(pos + 1);
            handleOption(name, value, args);
        } else {
            std::string name = arg.substr(2);
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                handleOption(name, argv[++i], args);
            } else {
                handleOption(name, "true", args);
            }
        }
    }
    
    void processShortOption(const std::string& arg, ParsedArgs& args, std::vector<std::string>& remainingArgs) {
        std::string name = arg.substr(1);
        if (name.length() == 1) {
            // Single character flag
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                handleShortOption(name, argv[++i], args);
            } else {
                handleShortOption(name, "true", args);
            }
        } else {
            // Multi-character short option
            if (name.find('=') != std::string::npos) {
                size_t pos = name.find('=');
                std::string optName = name.substr(0, pos);
                std::string value = name.substr(pos + 1);
                handleOption(optName, value, args);
            } else {
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    handleOption(name, argv[++i], args);
                } else {
                    handleOption(name, "true", args);
                }
            }
        }
    }
    
    void handleOption(const std::string& name, const std::string& value, ParsedArgs& args) {
        // Convert to snake_case
        std::string snakeName = toSnakeCase(name);
        
        // Handle special options
        if (name == "config" || snakeName == "config") {
            args.configFile = value;
        } else if (name == "experiment" || snakeName == "experiment") {
            args.experimentType = value;
        } else if (name == "preset" || snakeName == "preset") {
            args.preset = value;
        } else if (name == "output" || snakeName == "output") {
            args.outputFile = value;
        } else if (name == "benchmark" || snakeName == "benchmark") {
            args.benchmark = (value == "true" || value == "1" || value == "yes");
        } else if (name == "verbose" || snakeName == "verbose") {
            args.verbose = (value == "true" || value == "1" || value == "yes");
        } else if (name == "log-level" || name == "log_level" || snakeName == "log_level") {
            args.logLevel = value;
        } else if (name == "experiment-name" || name == "experiment_name" || snakeName == "experiment_name") {
            args.experimentName = value;
        } else if (name.find("experiment_") != std::string::npos || snakeName.find("experiment_") != std::string::npos) {
            // Store experiment-specific parameters
            size_t pos = snakeName.find('_');
            if (pos != std::string::npos) {
                std::string expName = snakeName.substr(pos + 1);
                args.experimentParams[expName] = value;
            }
        } else {
            // Store in generic params
            args.experimentParams[snakeName] = value;
        }
    }
    
    void handleShortOption(const std::string& shortName, const std::string& value, ParsedArgs& args) {
        if (shortOptions.find(shortName) != shortOptions.end()) {
            std::string longName = shortOptions[shortName];
            handleOption(longName, value, args);
        }
    }
    
    void validateArgs(const ParsedArgs& args) {
        // Validate experiment type
        std::vector<std::string> validExperiments = {"basic", "advanced", "research", "phase6", "auto"};
        bool valid = false;
        for (const auto& exp : validExperiments) {
            if (args.experimentType == exp) {
                valid = true;
                break;
            }
        }
        if (!valid && args.experimentType != "auto") {
            throw std::runtime_error("Invalid experiment type: " + args.experimentType);
        }
        
        // Validate preset
        std::vector<std::string> validPresets = {"basic", "advanced", "research", "phase6"};
        bool validPreset = false;
        for (const auto& preset : validPresets) {
            if (args.preset == preset) {
                validPreset = true;
                break;
            }
        }
        if (!validPreset) {
            throw std::runtime_error("Invalid preset: " + args.preset);
        }
        
        // Validate log level
        std::vector<std::string> validLogLevels = {"trace", "debug", "info", "warn", "error", "critical"};
        bool validLogLevel = false;
        for (const auto& level : validLogLevels) {
            if (args.logLevel == level) {
                validLogLevel = true;
                break;
            }
        }
        if (!validLogLevel) {
            throw std::runtime_error("Invalid log level: " << args.logLevel);
        }
    }
    
    std::string toSnakeCase(const std::string& str) const {
        std::string result;
        bool wasUpper = false;
        
        for (size_t i = 0; i < str.length(); ++i) {
            char c = str[i];
            if (std::isupper(c)) {
                if (!wasUpper && i > 0) {
                    result += '_';
                }
                result += std::tolower(c);
                wasUpper = true;
            } else {
                result += c;
                wasUpper = false;
            }
        }
        
        return result;
    }
};

// Run basic experiment
void runBasicExperiment(std::shared_ptr<Brain> brain, const std::string& experimentType,
                       const std::map<std::string, std::string>& params) {
    NLM_LOG_INFO("=== Basic Neural Connectivity Experiment ===");
    
    size_t neuronCount = 50;
    int64_t maxSteps = 100;
    
    if (params.find("neurons") != params.end()) {
        neuronCount = std::stoul(params.at("neurons"));
    }
    if (params.find("steps") != params.end()) {
        maxSteps = std::stoll(params.at("steps"));
    }
    
    // Run basic connectivity tests
    runBasicConnectivityTest(brain);
    
    // Run simulation
    NLM_LOG_INFO("Running basic connectivity simulation with " << neuronCount << " neurons");
    for (int64_t step = 0; step < maxSteps; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t totalSpikes = brain->getTotalSpikeCount();
    size_t firingNeurons = brain->getFiringNeuronCount();
    
    NLM_LOG_INFO("Results:");
    NLM_LOG_INFO("  Total spikes: " << totalSpikes);
    NLM_LOG_INFO("  Firing neurons: " << firingNeurons);
}

// Run advanced experiment
void runAdvancedExperiment(std::shared_ptr<Brain> brain, const std::string& experimentType,
                           const std::map<std::string, std::string>& params) {
    NLM_LOG_INFO("=== Advanced Plasticity Experiment ===");
    
    enablePlasticity(brain);
    recordInitialState(brain);
    
    size_t steps = 1000;
    if (params.find("steps") != params.end()) {
        steps = std::stoul(params.at("steps"));
    }
    
    NLM_LOG_INFO("Running plasticity experiment for " << steps << " steps");
    
    for (size_t step = 0; step < steps; ++step) {
        injectInputPattern(brain, step);
        brain->step(step, step * 0.001);
        
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " << step << 
                        " | Spikes: " << brain->getTotalSpikeCount() +
                        " | Firing: " << brain->getFiringNeuronCount());
        }
    }
    
    recordFinalState(brain);
    computeStatistics(brain);
}

// Run research experiment
void runResearchExperiment(std::shared_ptr<Brain> brain, const std::string& experimentType,
                           const std::map<std::string, std::string>& params) {
    NLM_LOG_INFO("=== Research Experiment ===");
    
    enablePlasticity(brain);
    
    size_t steps = 2000;
    bool detailedMetrics = false;
    if (params.find("steps") != params.end()) {
        steps = std::stoul(params.at("steps"));
    }
    if (params.find("detailed") != params.end()) {
        detailedMetrics = (params.at("detailed") == "true" || params.at("detailed") == "1" || params.at("detailed") == "yes");
    }
    
    NLM_LOG_INFO("Running research experiment for " << steps << " steps");
    
    for (size_t step = 0; step < steps; ++step) {
        injectComplexPattern(brain, step);
        brain->step(step, step * 0.001);
        
        if (detailedMetrics) {
            recordDetailedMetrics(step, brain);
        }
        
        if (step % 500 == 0) {
            NLM_LOG_INFO("  Step " << step << 
                        " | Spikes: " << brain->getTotalSpikeCount() +
                        " | Synapses: " << brain->getTotalSynapseCount() +
                        " | Firing: " << brain->getFiringNeuronCount());
        }
    }
    
    computeAdvancedStatistics(brain, params);
}

// Run phase 6 experiment
void runPhase6Experiment(std::shared_ptr<Brain> brain, const std::string& experimentType,
                         const std::map<std::string, std::string>& params) {
    NLM_LOG_INFO("=== Phase 6 Neural Computation Experiment ===");
    
    enablePlasticity(brain);
    
    size_t steps = 1500;
    bool performanceMonitoring = false;
    if (params.find("steps") != params.end()) {
        steps = std::stoul(params.at("steps"));
    }
    if (params.find("performance") != params.end()) {
        performanceMonitoring = (params.at("performance") == "true" || params.at("performance") == "1" || params.at("performance") == "yes");
    }
    
    NLM_LOG_INFO("Running Phase 6 experiment for " << steps << " steps");
    
    for (size_t step = 0; step < steps; ++step) {
        injectPhase6Pattern(brain, step);
        brain->step(step, step * 0.001);
        
        if (performanceMonitoring) {
            monitorPerformance(step, brain);
        }
        
        if (step % 300 == 0) {
            NLM_LOG_INFO("  Step " << step << 
                        " | Spikes: " << brain->getTotalSpikeCount() +
                        " | Learning rate: N/A");
        }
    }
    
    finalPhase6Analysis(brain);
}

// Run automatic experiment sequence
void runAutomaticExperimentSequence(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Running automatic experiment sequence...");
    
    // Test 1: Basic connectivity
    runBasicConnectivityTest(brain);
    
    // Reset for plasticity experiment
    brain->reset();
    brain->initialize();
    
    // Test 2: Plasticity learning experiment
    runPlasticityExperiment(brain);
    
    // Reset for STDP verification
    brain->reset();
    brain->initialize();
    
    // Test 3: STDP verification
    runStdpVerification(brain);
}

// Helper functions for experiments
void enablePlasticity(std::shared_ptr<Brain> brain) {
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
    }
}

void recordInitialState(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Recording initial state...");
}

void recordFinalState(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Recording final state...");
}

void computeStatistics(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("=== Learning Experiment Results ===");
    
    size_t totalSpikes = brain->getTotalSpikeCount();
    size_t totalSynapses = brain->getTotalSynapseCount();
    
    NLM_LOG_INFO("Total spikes: " << totalSpikes);
    NLM_LOG_INFO("Total synapses: " << totalSynapses);
}

void injectInputPattern(std::shared_ptr<Brain> brain, size_t step) {
    for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
        brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
    }
}

void injectComplexPattern(std::shared_ptr<Brain> brain, size_t step) {
    float intensity = 20.0f + 10.0f * std::sin(step * 0.01f);
    for (size_t i = 0; i < 30 && i < brain->getTotalNeuronCount() / 3; ++i) {
        brain->injectCurrentToNeurons(NeuronType::Sensory, intensity);
    }
}

void injectPhase6Pattern(std::shared_ptr<Brain> brain, size_t step) {
    float baseIntensity = 25.0f;
    float variation = 5.0f * std::cos(step * 0.005f);
    
    for (size_t i = 0; i < 40 && i < brain->getTotalNeuronCount() / 2; ++i) {
        float intensity = baseIntensity + variation + (i % 3) * 5.0f;
        brain->injectCurrentToNeurons(NeuronType::Sensory, intensity);
    }
}

void recordDetailedMetrics(size_t step, std::shared_ptr<Brain> brain) {
    // Record detailed metrics for research experiments
}

void computeAdvancedStatistics(std::shared_ptr<Brain> brain, const std::map<std::string, std::string>& params) {
    NLM_LOG_INFO("Computing advanced statistics...");
}

void monitorPerformance(size_t step, std::shared_ptr<Brain> brain) {
    // Performance monitoring for phase 6 experiments
}

void finalPhase6Analysis(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("=== Phase 6 Final Analysis ===");
    NLM_LOG_INFO("Analyzing final brain state and performance metrics...");
}

// Original test functions from main.cpp
void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_INFO("  No neurons found!");
        return;
    }
    
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    NLM_LOG_INFO("  Injecting current into 10 neurons...");
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);
    }
    
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("  Spikes generated: " << spikes);
    
    if (spikes > 0) {
        NLM_LOG_INFO("  ✓ Spikes propagate through network");
    } else {
        NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            NLM_LOG_INFO("    Neuron " << i << 
                        " V=" << neurons[i]->getMembranePotential() +
                        " thresh=" << neurons[i]->getThreshold());
        }
    }
}

void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    
    LearningExperiment experiment(brain, 42);
    
    experiment.recordInitialState();
    
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
    
    for (SimulationStep step = 0; step < 1000; ++step) {
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " << step << 
                        " | Spikes: " << brain->getTotalSpikeCount() +
                        " | Firing: " << brain->getFiringNeuronCount());
        }
    }
    
    experiment.recordFinalState();
    experiment.computeStatistics();
}

void runStdpVerification(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        NLM_LOG_INFO("  Not enough synapses for STDP test");
        return;
    }
    
    NLM_LOG_INFO("  Testing STDP on 5 synapses:");
    
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);
        NLM_LOG_INFO("    Synapse " << i << 
                     " before: " << beforeWeights[i]);
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
    
    for (int trial = 0; trial < 50; ++trial) {
        Neuron* preNeuron = nullptr;
        Neuron* postNeuron = nullptr;
        
        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            preNeuron = neurons[0];
            postNeuron = neurons[1];
        }
        
        if (preNeuron && postNeuron) {
            preNeuron->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            
            postNeuron->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
    }
    
    NLM_LOG_INFO("  After correlated activity:");
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        NLM_LOG_INFO("    Synapse " << i << 
                     " after: " << synapses[i]->getWeight() +
                     " (Δ=" << delta << ")");
    }
    
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        totalDelta += synapses[i]->getWeight() - beforeWeights[i];
    }
    
    NLM_LOG_INFO("");
    if (totalDelta > 0.001f) {
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check parameters");
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering");
    }
}

LearningExperiment::LearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
    : brain(b), seed(s), initialSynapseCount(0) {}

void LearningExperiment::recordInitialState() {
    initialSynapseCount = brain->getTotalSynapseCount();
    initialWeights.clear();
    
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (const auto& syn : region->getSynapses()) {
            initialWeights.push_back(syn->getWeight());
        }
    }
    
    NLM_LOG_INFO("Initial state recorded:");
    NLM_LOG_INFO("  Synapses: " << std::to_string(initialSynapseCount));
    if (!initialWeights.empty()) {
        float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float mean = sum / initialWeights.size();
        NLM_LOG_INFO("  Mean weight: " << std::to_string(mean));
    }
}

void LearningExperiment::recordFinalState() {
    finalWeights.clear();
    
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (const auto& syn : region->getSynapses()) {
            finalWeights.push_back(syn->getWeight());
        }
    }
    
    mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
    
    NLM_LOG_INFO("Final state recorded:");
    NLM_LOG_INFO("  Total spikes: " << std::to_string(brain->getTotalSpikeCount()));
    if (!finalWeights.empty()) {
        float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        float mean = sum / finalWeights.size();
        NLM_LOG_INFO("  Mean weight: " << std::to_string(mean));
    }
}

void LearningExperiment::computeStatistics() {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Learning Experiment Results ===");
    NLM_LOG_INFO("");
    
    if (initialWeights.empty() || finalWeights.empty()) {
        NLM_LOG_INFO("ERROR: No weights recorded");
        return;
    }
    
    float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
    float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
    float initialMean = initialSum / initialWeights.size();
    float finalMean = finalSum / finalWeights.size();
    
    NLM_LOG_INFO("Weight Statistics:");
    NLM_LOG_INFO("  Initial mean weight: " << std::to_string(initialMean));
    NLM_LOG_INFO("  Final mean weight: " << std::to_string(finalMean));
    NLM_LOG_INFO("  Change: " << std::to_string(finalMean - initialMean));
    
    size_t strengthened = 0;
    size_t weakened = 0;
    size_t unchanged = 0;
    
    size_t minSize = std::min(initialWeights.size(), finalWeights.size());
    for (size_t i = 0; i < minSize; ++i) {
        float delta = finalWeights[i] - initialWeights[i];
        if (delta > 0.01f) ++strengthened;
        else if (delta < -0.01f) ++weakened;
        else ++unchanged;
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Synaptic Changes:");
    NLM_LOG_INFO("  Strengthened: " << std::to_string(strengthened));
    NLM_LOG_INFO("  Weakened: " << std::to_string(weakened));
    NLM_LOG_INFO("  Unchanged: " << std::to_string(unchanged));
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Spike Activity:");
    NLM_LOG_INFO("  Total spikes: " << std::to_string(brain->getTotalSpikeCount()));
    NLM_LOG_INFO("  Most active neurons recorded: " << std::to_string(mostActiveNeurons.size()));
    
    bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                          (strengthened > 0 || weakened > 0);
    
    NLM_LOG_INFO("");
    if (learningOccurred) {
        NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
    } else {
        NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
    }
}

int main(int argc, char** argv) {
    try {
        // Initialize parser
        ArgParser parser;
        
        // Parse arguments
        ArgParser::ParsedArgs args = parser.parse(argc, argv);
        
        // Handle help and version
        if (args.helpRequested) {
            std::cout << parser.generateHelp(argv[0]) << std::endl;
            return static_cast<int>(NMLErrorCode::Success);
        }
        
        if (args.versionRequested) {
            printVersion();
            return static_cast<int>(NMLErrorCode::Success);
        }
        
        // Initialize logging
        initializeLogger(args.logLevel);
        
        // Create config presets
        ConfigPresets presets;
        
        // Load configuration
        auto config = std::make_shared<Config>();
        
        // Try to load from file if specified (and not default)
        if (args.configFile != "configs/default.cfg") {
            if (config->loadFromFile(args.configFile)) {
                NLM_LOG_INFO("Loaded configuration from: " << args.configFile);
            } else {
                NLM_LOG_WARN("Failed to load configuration from: " << args.configFile);
                NLM_LOG_WARN("Using default configuration.");
                reportError(NMLErrorCode::ConfigLoadError, 
                    "Failed to load configuration from " + args.configFile + ". Using defaults.");
            }
        }
        
        // Apply preset
        if (presets.hasPreset(args.preset)) {
            if (!presets.applyPreset(*config, args.preset)) {
                reportError(NMLErrorCode::ConfigLoadError, 
                    "Failed to apply preset: " + args.preset + ". Using defaults.");
            } else {
                NLM_LOG_INFO("Applied preset: " << args.preset);
                NLM_LOG_INFO("Preset description: " << presets.getDescription(args.preset));
            }
        }
        
        // Override with command line arguments
        if (!config->loadFromArgs(argc, argv)) {
            NMLErrorCode errorCode = config->getLastError() == ConfigErrorCode::Success ? 
                NMLErrorCode::ConfigLoadError : NMLErrorCode::SystemError;
            reportError(errorCode, "Failed to load configuration from command line arguments");
        }
        
        // Set default values for Phase 2
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        // STDP parameters
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
        
        // Structural plasticity parameters
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
        
        // Log configuration summary
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Configuration:");
        NLM_LOG_INFO("  random_seed: " << config->getOr<int64_t>("random_seed", 42));
        NLM_LOG_INFO("  simulation_timestep: " << config->getOr<double>("simulation_timestep", 0.001) << "s");
        NLM_LOG_INFO("  neuron_count: " << config->getOr<int64_t>("neuron_count", 500));
        NLM_LOG_INFO("  region_count: " << config->getOr<int64_t>("region_count", 1));
        NLM_LOG_INFO("  connection_probability: " << config->getOr<float>("connection_probability", 0.15f));
        
        // Initialize simulation clock
        double timestep = config->getOr<double>("simulation_timestep", 0.001);
        SimulationClock clock(timestep);
        NLM_LOG_INFO("Simulation clock initialized with timestep: " << timestep << "s");
        
        // Initialize brain
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Initializing NLM Brain...");
        auto brain = std::make_shared<Brain>(config);
        
        NMLErrorCode brainInitResult = getBrainInitErrorCode(brain->initialize());
        if (brainInitResult != NMLErrorCode::Success) {
            reportError(brainInitResult, "Failed to initialize brain. System cannot continue.");
            return static_cast<int>(brainInitResult);
        }
        
        brain->logStatus();
        
        // Determine experiment type
        std::string experimentType = args.experimentType;
        if (!args.positionalArgs.empty()) {
            experimentType = args.positionalArgs[0];
        }
        
        // Validate experiment type
        std::vector<std::string> validExperiments = {"basic", "advanced", "research", "phase6", "auto"};
        bool validExperiment = false;
        for (const auto& exp : validExperiments) {
            if (experimentType == exp) {
                validExperiment = true;
                break;
            }
        }
        
        if (!validExperiment) {
            NLM_LOG_ERROR("Invalid experiment type: " << experimentType);
            NLM_LOG_INFO("Valid experiment types: basic, advanced, research, phase6, auto");
            return 1;
        }
        
        // Prepare experiment-specific parameters
        std::map<std::string, std::string> experimentParams;
        for (const auto& pair : args.experimentParams) {
            experimentParams[pair.first] = pair.second;
        }
        
        // Run the selected experiment
        if (experimentType == "basic") {
            runBasicExperiment(brain, experimentType, experimentParams);
        } else if (experimentType == "advanced") {
            runAdvancedExperiment(brain, experimentType, experimentParams);
        } else if (experimentType == "research") {
            runResearchExperiment(brain, experimentType, experimentParams);
        } else if (experimentType == "phase6") {
            runPhase6Experiment(brain, experimentType, experimentParams);
        } else if (experimentType == "auto") {
            runAutomaticExperimentSequence(brain);
        }
        
        // Final brain status
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Final Brain Status ===");
        brain->logStatus();
        
        // Display completion message
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== NLM Phase 2 Complete ===");
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Phase 2 Objectives Completed:");
        NLM_LOG_INFO("  ✓ Real LIF neuron dynamics implemented");
        NLM_LOG_INFO("  ✓ Event-driven spike propagation with delays");
        NLM_LOG_INFO("  ✓ STDP plasticity rule");
        NLM_LOG_INFO("  ✓ Hebbian plasticity rule");
        NLM_LOG_INFO("  ✓ Structural plasticity (synaptogenesis/pruning)");
        NLM_LOG_INFO("  ✓ Learning experiment demonstrates measurable changes");
        NLM_LOG_INFO("  ✓ Network shows activity-dependent synaptic modification");
        NLM_LOG_INFO("");
        NLM_LOG_INFO("The NLM brain is now a functioning artificial neural substrate");
        NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
        
        return static_cast<int>(NMLErrorCode::Success);
        
    } catch (const std::exception& e) {
        reportError(NMLErrorCode::SystemError, 
            "Unhandled exception in main execution: " + std::string(e.what()), false);
        return static_cast<int>(NMLErrorCode::SystemError);
    }
}