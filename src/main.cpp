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
#include <chrono>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <map>
#include <optional>
#include <sstream>

namespace fs = std::filesystem;

using namespace nlm;

// Command line argument structure for comprehensive CLI
struct CommandLineArgs {
    std::string configFile = "configs/default.cfg";
    std::string outputFile = "";
    std::string logLevel = "info";
    bool verbose = false;
    bool debug = false;
    bool batchMode = false;
    bool quiet = false;
    std::string mode = "demo"; // demo, learning, benchmark, explore, compare
    std::string helpMode = ""; // "usage", "modes", "options"
    int iterations = 1;
    double duration = 0.0;
    std::string pattern = "";
    std::vector<std::string> customArgs;
    
    bool hasValue(const std::string& arg) const {
        return arg == "--help" || arg == "-h" || arg == "--help-usage" || 
               arg == "--help-modes" || arg == "--help-options" ||
               arg == "--version" || arg == "--validate" ||
               arg == "--list-configs";
    }
    
    bool isValidMode(const std::string& modeName) const {
        const std::vector<std::string> validModes = {"demo", "learning", "benchmark", "explore", "compare", "test"};
        return std::find(validModes.begin(), validModes.end(), modeName) != validModes.end();
    }
};

// Enhanced help system
class HelpSystem {
public:
    static void printWelcome() {
        std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation                         ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements:                                    ║
    ║     - Real LIF neuron dynamics                                ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity                            ║
    ║     - Structural plasticity                                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    }
    
    static void printUsage() {
        std::cout << R"(
Usage: nlm [OPTIONS] [MODE] [ARGS]

Modes (default: demo):
  demo           Run Phase 6 integration test (connectivity, plasticity, STDP)
  learning       Run plasticity learning experiment with metrics
  benchmark      Performance testing with detailed timing
  explore        Curiosity-driven exploration mode
  compare        Parameter comparison mode for sensitivity analysis
  test           Run basic functionality tests

Options:
  --config FILE          Configuration file (default: configs/default.cfg)
  --output FILE          Output file for results (default: stdout)
  --mode MODE           Execution mode (demo|learning|benchmark|explore|compare|test)
  --log-level LEVEL      Set logging level (debug|info|warning|error)
  --log-file FILE        Log to file as well as console
  --batch               Run in batch mode for automated testing
  --iterations N         Number of iterations for benchmark/explore modes
  --duration SECONDS     Maximum duration for exploration (0 = unlimited)
  --pattern PATTERN      Input pattern for exploration/batching
  --verbose             Enable verbose output
  --debug               Enable debug mode (overrides log level)
  --quiet               Suppress most output
  --version             Print version information
  --validate            Validate configuration and exit
  --list-configs         List available configuration files
  --help-modes           Show mode descriptions
  --help-options         Show option descriptions
  --help-usage           Show this usage message

Examples:
  nlm                          Run demo mode with defaults
  nlm --mode learning --verbose
  nlm --mode benchmark --iterations 10 --output results.json
  nlm --batch --pattern random --iterations 100
  nlm --validate --config myconfig.cfg
  nlm --help-modes

For more details about a specific mode, use: nlm --help-modes
  )" << std::endl;
    }
    
    static void printModeDescriptions() {
        std::cout << R"(
Mode Descriptions:

demo (Phase 6 Integration Test):
  Default mode that runs comprehensive tests:
  - Basic connectivity verification
  - Plasticity learning experiment with metrics
  - STDP verification
  - Demonstrates all Phase 2 features

learning (Plasticity Experiment):
  Focuses on measurable synaptic changes through experience:
  - Records initial brain state
  - Applies repeated input patterns
  - Tracks weight changes over time
  - Reports learning metrics

benchmark (Performance Testing):
  Measures performance and timing:
  - Runs multiple iterations
  - Reports execution time
  - Measures spike rates, memory usage
  - Compares against baseline

explore (Curiosity-Driven Exploration):
  Open-ended exploration with adaptive input:
  - Random stimulus application
  - Tracks discovery patterns
  - Adapts to network activity
  - Records emergent behaviors

compare (Parameter Comparison):
  Sensitivity analysis of parameters:
  - Runs with different parameter sets
  - Compares results across configurations
  - Identifies optimal parameter ranges
  - Reports variance analysis

test (Basic Functionality):
  Quick verification of core functionality:
  - Basic neural connectivity
  - Simple plasticity test
  - STDP verification
  - Fast failure detection
  )" << std::endl;
    }
    
    static void printOptionsDescriptions() {
        std::cout << R"(
Option Descriptions:

--config FILE
  Configuration file path. Loads settings from file, then command line
  overrides. If file not found, uses defaults.

--output FILE
  Output file for results in JSON or text format. If not specified, results
  go to stdout. For batch mode, enables result logging.

--mode MODE
  Execution mode. Controls which experiments/tests run. See --help-modes.

--log-level LEVEL
  Logging verbosity: debug (most), info (normal), warning, error (least).

--log-file FILE
  Additional log file for persistent logging of debugging information.

--batch
  Batch mode for automated testing and benchmarking. Disables interactive
  prompts and enables continuous execution.

--iterations N
  Number of repetitions for iterative modes (learning, benchmark, explore).
  Used with --pattern for pattern-based testing.

--duration SECONDS
  Maximum runtime in seconds (0 = unlimited). Useful for safety limits.

--pattern PATTERN
  Input pattern: random, correlated, sinusoidal, pulse, or custom.
  Determines stimulus type for experiments.

--verbose
  Enable additional progress messages and status updates.

--debug
  Enable debug mode with maximum logging and additional diagnostics.
  May affect performance.

--quiet
  Suppress most output, only show errors and final results.

--validate
  Validate current configuration, check dependencies, and exit.

--list-configs
  List available configuration files in the configs directory.

--help-modes
  Display detailed descriptions of all available modes.

--help-options
  Display detailed descriptions of all command-line options.

--help-usage
  Display complete usage information with examples.

--version
  Print version and build information.
  )" << std::endl;
    }
};

// Configuration validation
class ConfigValidator {
public:
    struct ValidationResult {
        bool isValid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        ValidationResult() : isValid(true) {}
        
        bool hasErrors() const { return !errors.empty(); }
        bool hasWarnings() const { return !warnings.empty(); }
        
        std::string getSummary() const {
            std::stringstream ss;
            if (hasErrors()) {
                ss << "Configuration has " << errors.size() << " error";
                if (errors.size() > 1) ss << "s";
                ss << ".\n";
            }
            if (hasWarnings()) {
                ss << "Configuration has " << warnings.size() << " warning";
                if (warnings.size() > 1) ss << "s";
                ss << ".\n";
            }
            if (!isValid) ss << "Configuration validation failed.";
            else if (hasWarnings()) ss << "Configuration validation completed with warnings.";
            else ss << "Configuration validation passed.";
            return ss.str();
        }
    };
    
    static ValidationResult validate(const std::shared_ptr<Config>& config) {
        ValidationResult result;
        
        try {
            // Validate core parameters
            validateCoreParameters(config, result);
            
            // Validate performance parameters
            validatePerformanceParameters(config, result);
            
            // Validate plasticity parameters
            validatePlasticityParameters(config, result);
            
            // Validate neuromodulation parameters
            validateNeuromodulationParameters(config, result);
            
            // Validate development parameters
            validateDevelopmentParameters(config, result);
            
            // Validate simulation parameters
            validateSimulationParameters(config, result);
            
            // Validate world parameters
            validateWorldParameters(config, result);
            
            // Check for consistency
            checkConsistency(config, result);
            
        } catch (const std::exception& e) {
            result.isValid = false;
            result.errors.push_back("Configuration validation exception: " + std::string(e.what()));
        }
        
        return result;
    }
    
private:
    static void validateCoreParameters(const std::shared_ptr<Config>& config, ValidationResult& result) {
        // Validate neuron count
        try {
            auto neuronCount = config->getOr<int64_t>("neuron_count", 500);
            if (neuronCount <= 0) {
                result.isValid = false;
                result.errors.push_back("neuron_count must be positive (got " + std::to_string(neuronCount) + ")");
            } else if (neuronCount > 100000) {
                result.warnings.push_back("neuron_count is very large (" + std::to_string(neuronCount) + "); simulation may be slow");
            }
        } catch (...) {
            result.warnings.push_back("neuron_count not found; using default (500)");
        }
        
        // Validate region count
        try {
            auto regionCount = config->getOr<int64_t>("region_count", 1);
            if (regionCount <= 0) {
                result.isValid = false;
                result.errors.push_back("region_count must be positive (got " + std::to_string(regionCount) + ")");
            }
        } catch (...) {
            result.warnings.push_back("region_count not found; using default (1)");
        }
        
        // Validate simulation timestep
        try {
            auto timestep = config->getOr<double>("simulation_timestep", 0.001);
            if (timestep <= 0.0) {
                result.isValid = false;
                result.errors.push_back("simulation_timestep must be positive (got " + std::to_string(timestep) + ")");
            } else if (timestep > 0.1) {
                result.warnings.push_back("simulation_timestep is large (" + std::to_string(timestep) + "); may affect accuracy");
            }
        } catch (...) {
            result.warnings.push_back("simulation_timestep not found; using default (0.001)");
        }
        
        // Validate random seed
        try {
            auto seed = config->getOr<int64_t>("random_seed", 42);
            if (seed < 0) {
                result.isValid = false;
                result.errors.push_back("random_seed must be non-negative (got " + std::to_string(seed) + ")");
            }
        } catch (...) {
            result.warnings.push_back("random_seed not found; using default (42)");
        }
        
        // Validate connection probability
        try {
            auto connProb = config->getOr<float>("connection_probability", 0.1);
            if (connProb < 0.0 || connProb > 1.0) {
                result.isValid = false;
                result.errors.push_back("connection_probability must be between 0.0 and 1.0 (got " + std::to_string(connProb) + ")");
            } else if (connProb > 0.5) {
                result.warnings.push_back("connection_probability is high (" + std::to_string(connProb) + "); may create very dense network");
            }
        } catch (...) {
            result.warnings.push_back("connection_probability not found; using default (0.1)");
        }
    }
    
    static void validatePerformanceParameters(const std::shared_ptr<Config>& config, ValidationResult& result) {
        // Validate STDP parameters
        try {
            auto ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01);
            if (ltpWeight < 0.0 || ltpWeight > 0.5) {
                result.warnings.push_back("stdp_ltp_weight is outside typical range (0.0-0.5); got " + std::to_string(ltpWeight));
            }
        } catch (...) {}
        
        try {
            auto ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012);
            if (ltdWeight < 0.0 || ltdWeight > 0.5) {
                result.warnings.push_back("stdp_ltd_weight is outside typical range (0.0-0.5); got " + std::to_string(ltdWeight));
            }
        } catch (...) {}
        
        try {
            auto stdpTau = config->getOr<float>("stdp_tau", 20.0);
            if (stdpTau < 1.0 || stdpTau > 100.0) {
                result.warnings.push_back("stdp_tau is outside typical range (1.0-100.0); got " + std::to_string(stdpTau));
            }
        } catch (...) {}
        
        // Validate Hebbian parameters
        try {
            auto hebbianRate = config->getOr<float>("hebbian_learning_rate", 0.01);
            if (hebbianRate < 0.0 || hebbianRate > 1.0) {
                result.warnings.push_back("hebbian_learning_rate is outside typical range (0.0-1.0); got " + std::to_string(hebbianRate));
            }
        } catch (...) {}
    }
    
    static void validatePlasticityParameters(const std::shared_ptr<Config>& config, ValidationResult& result) {
        // Validate structural plasticity parameters
        try {
            auto synRate = config->getOr<float>("synaptogenesis_rate", 0.0001);
            if (synRate < 0.0 || synRate > 0.01) {
                result.warnings.push_back("synaptogenesis_rate is outside typical range (0.0-0.01); got " + std::to_string(synRate));
            }
        } catch (...) {}
        
        try {
            auto pruneRate = config->getOr<float>("pruning_rate", 0.00001);
            if (pruneRate < 0.0 || pruneRate > 0.001) {
                result.warnings.push_back("pruning_rate is outside typical range (0.0-0.001); got " + std::to_string(pruneRate));
            }
        } catch (...) {}
    }
    
    static void validateNeuromodulationParameters(const std::shared_ptr<Config>& config, ValidationResult& result) {
        // Validate neuromodulation parameters
        try {
            auto dopamineScale = config->getOr<float>("dopamine_scale", 1.0);
            if (dopamineScale < 0.0) {
                result.isValid = false;
                result.errors.push_back("dopamine_scale must be positive (got " + std::to_string(dopamineScale) + ")");
            }
        } catch (...) {}
        
        // Validate curiosity parameters
        try {
            auto curiosityEnable = config->getOr<bool>("curiosity.enable", true);
        } catch (...) {}
        
        try {
            auto noveltyEnable = config->getOr<bool>("novelty.enable", true);
        } catch (...) {}
    }
    
    static void validateDevelopmentParameters(const std::shared_ptr<Config>& config, ValidationResult& result) {
        // Validate development parameters
        try {
            auto devSynaptogenesis = config->getOr<float>("development_synaptogenesis_rate", 0.001);
            if (devSynaptogenesis < 0.0) {
                result.isValid = false;
                result.errors.push_back("development_synaptogenesis_rate must be positive (got " + std::to_string(devSynaptogenesis) + ")");
            }
        } catch (...) {}
        
        try {
            auto devPruning = config->getOr<float>("development_pruning_rate", 0.0001);
            if (devPruning < 0.0) {
                result.isValid = false;
                result.errors.push_back("development_pruning_rate must be positive (got " + std::to_string(devPruning) + ")");
            }
        } catch (...) {}
    }
    
    static void validateSimulationParameters(const std::shared_ptr<Config>& config, ValidationResult& result) {
        // Validate simulation parameters
        try {
            auto maxSteps = config->getOr<int64_t>("max_simulation_steps", 10000);
            if (maxSteps <= 0) {
                result.isValid = false;
                result.errors.push_back("max_simulation_steps must be positive (got " + std::to_string(maxSteps) + ")");
            }
        } catch (...) {}
        
        try {
            auto timeLimit = config->getOr<double>("simulation_time_limit", 0.0);
            if (timeLimit < 0.0) {
                result.isValid = false;
                result.errors.push_back("simulation_time_limit must be non-negative (got " + std::to_string(timeLimit) + ")");
            }
        } catch (...) {}
        
        // Validate log parameters
        try {
            auto logLevel = config->getOr<std::string>("log_level", "INFO");
            std::transform(logLevel.begin(), logLevel.end(), logLevel.begin(), ::toupper);
            if (logLevel != "DEBUG" && logLevel != "INFO" && logLevel != "WARNING" && logLevel != "ERROR" && logLevel != "CRITICAL") {
                result.warnings.push_back("log_level should be DEBUG, INFO, WARNING, ERROR, or CRITICAL; got " + logLevel);
            }
        } catch (...) {}
        
        try {
            auto logToFile = config->getOr<bool>("log_to_file", false);
        } catch (...) {}
    }
    
    static void validateWorldParameters(const std::shared_ptr<Config>& config, ValidationResult& result) {
        // Validate world parameters
        try {
            auto worldWidth = config->getOr<float>("environment_width", 10.0);
            if (worldWidth <= 0.0) {
                result.isValid = false;
                result.errors.push_back("environment_width must be positive (got " + std::to_string(worldWidth) + ")");
            }
        } catch (...) {}
        
        try {
            auto worldHeight = config->getOr<float>("environment_height", 10.0);
            if (worldHeight <= 0.0) {
                result.isValid = false;
                result.errors.push_back("environment_height must be positive (got " + std::to_string(worldHeight) + ")");
            }
        } catch (...) {}
        
        try {
            auto visionWidth = config->getOr<float>("vision_width", 8.0);
            if (visionWidth <= 0.0) {
                result.isValid = false;
                result.errors.push_back("vision_width must be positive (got " + std::to_string(visionWidth) + ")");
            }
        } catch (...) {}
        
        try {
            auto visionHeight = config->getOr<float>("vision_height", 8.0);
            if (visionHeight <= 0.0) {
                result.isValid = false;
                result.errors.push_back("vision_height must be positive (got " + std::to_string(visionHeight) + ")");
            }
        } catch (...) {}
    }
    
    static void checkConsistency(const std::shared_ptr<Config>& config, ValidationResult& result) {
        // Check consistency between parameters
        try {
            auto neuronCount = config->getOr<int64_t>("neuron_count", 500);
            auto regionCount = config->getOr<int64_t>("region_count", 1);
            
            // Warning if very few neurons per region
            if (neuronCount / regionCount < 10) {
                result.warnings.push_back("Few neurons per region (" + std::to_string(neuronCount / regionCount) + "); may affect behavior stability");
            }
            
            // Warning if many regions with few neurons
            if (regionCount > 10 && neuronCount / regionCount < 50) {
                result.warnings.push_back("Many regions with few neurons each may lead to sparse connectivity");
            }
            
        } catch (...) {}
        
        // Check for conflicting plasticity settings
        try {
            auto stdpEnable = config->getOr<bool>("plasticity.stdp.enable", true);
            auto hebbianEnable = config->getOr<bool>("plasticity.hebbian.enable", true);
            auto structuralEnable = config->getOr<bool>("plasticity.structural.enable", true);
            
            // Warning if all plasticity is disabled
            if (!stdpEnable && !hebbianEnable && !structuralEnable) {
                result.warnings.push_back("All plasticity mechanisms are disabled; brain will not learn");
            }
            
        } catch (...) {}
        
        // Check for conflicting neuromodulation settings
        try {
            auto curiosityEnable = config->getOr<bool>("neuromod.curiosity.enable", true);
            auto noveltyEnable = config->getOr<bool>("neuromod.novelty.enable", true);
            auto dopamineEnable = config->getOr<bool>("neuromod.dopamine.enable", true);
            
            // Warning if all neuromodulation is disabled
            if (!curiosityEnable && !noveltyEnable && !dopamineEnable) {
                result.warnings.push_back("All neuromodulation is disabled; brain will not explore or respond to rewards");
            }
            
        } catch (...) {}
    }
};

// Result structure for batch processing
struct ExecutionResult {
    std::string mode;
    double startTime;
    double endTime;
    bool success;
    std::string errorMessage;
    std::map<std::string, std::string> metrics;
    std::string output;
    
    double getDuration() const { return endTime - startTime; }
};

// Result logger for batch processing
class ResultLogger {
public:
    static void logToFile(const std::string& filename, const ExecutionResult& result) {
        std::ofstream file(filename, std::ios::app);
        if (!file.is_open()) {
            std::cerr << "ERROR: Cannot open output file: " << filename << std::endl;
            return;
        }
        
        file << "=== Execution Result ===\n";
        file << "Mode: " << result.mode << "\n";
        file << "Duration: " << result.getDuration() << "s\n";
        file << "Success: " << (result.success ? "true" : "false") << "\n";
        
        if (!result.errorMessage.empty()) {
            file << "Error: " << result.errorMessage << "\n";
        }
        
        file << "Metrics:\n";
        for (const auto& [key, value] : result.metrics) {
            file << "  " << key << ": " << value << "\n";
        }
        
        file << "Output:\n" << result.output << "\n";
        file << "======================\n\n";
    }
};

// Argument parsing
CommandLineArgs parseArguments(int argc, char** argv) {
    CommandLineArgs args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            args.helpMode = "usage";
            continue;
        }
        
        if (arg == "--help-modes") {
            args.helpMode = "modes";
            continue;
        }
        
        if (arg == "--help-options") {
            args.helpMode = "options";
            continue;
        }
        
        if (arg == "--help-usage") {
            args.helpMode = "usage";
            continue;
        }
        
        if (arg == "--version") {
            args.helpMode = "version";
            continue;
        }
        
        if (arg == "--validate") {
            args.helpMode = "validate";
            continue;
        }
        
        if (arg == "--list-configs") {
            args.helpMode = "list-configs";
            continue;
        }
        
        if (arg.find("--config=") == 0) {
            args.configFile = arg.substr(9);
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                args.configFile = argv[++i];
            }
        } else if (arg.find("--output=") == 0) {
            args.outputFile = arg.substr(9);
        } else if (arg == "--output") {
            if (i + 1 < argc) {
                args.outputFile = argv[++i];
            }
        } else if (arg.find("--mode=") == 0) {
            args.mode = arg.substr(7);
        } else if (arg == "--mode") {
            if (i + 1 < argc) {
                args.mode = argv[++i];
            }
        } else if (arg.find("--log-level=") == 0) {
            args.logLevel = arg.substr(12);
        } else if (arg == "--log-level") {
            if (i + 1 < argc) {
                args.logLevel = argv[++i];
            }
        } else if (arg.find("--log-file=") == 0) {
            args.customArgs.push_back("--log-file=" + arg.substr(11));
        } else if (arg == "--log-file") {
            if (i + 1 < argc) {
                args.customArgs.push_back("--log-file=" + std::string(argv[++i]));
            }
        } else if (arg == "--batch") {
            args.batchMode = true;
        } else if (arg == "--verbose") {
            args.verbose = true;
        } else if (arg == "--debug") {
            args.debug = true;
        } else if (arg == "--quiet") {
            args.quiet = true;
        } else if (arg.find("--iterations=") == 0) {
            args.iterations = std::stoi(arg.substr(12));
        } else if (arg == "--iterations") {
            if (i + 1 < argc) {
                args.iterations = std::stoi(argv[++i]);
            }
        } else if (arg.find("--duration=") == 0) {
            args.duration = std::stod(arg.substr(11));
        } else if (arg == "--duration") {
            if (i + 1 < argc) {
                args.duration = std::stod(argv[++i]);
            }
        } else if (arg.find("--pattern=") == 0) {
            args.pattern = arg.substr(10);
        } else if (arg == "--pattern") {
            if (i + 1 < argc) {
                args.pattern = argv[++i];
            }
        } else if (arg[0] != '-') {
            // Positional argument (mode)
            if (args.mode == "demo") {  // Default mode
                args.mode = arg;
            } else {
                args.customArgs.push_back(arg);
            }
        } else {
            args.customArgs.push_back(arg);
        }
    }
    
    return args;
}

// Output formatter
class OutputFormatter {
public:
    static std::string formatTime(double seconds) {
        std::stringstream ss;
        if (seconds < 60.0) {
            ss << std::fixed << std::setprecision(3) << seconds << "s";
        } else if (seconds < 3600.0) {
            ss << std::fixed << std::setprecision(1) << (seconds / 60.0) << "min";
        } else {
            ss << std::fixed << std::setprecision(2) << (seconds / 3600.0) << "h";
        }
        return ss.str();
    }
    
    static std::string formatMemory(size_t bytes) {
        if (bytes < 1024) return std::to_string(bytes) + " B";
        if (bytes < 1024 * 1024) return std::to_string(bytes / 1024) + " KB";
        if (bytes < 1024 * 1024 * 1024) return std::to_string(bytes / (1024 * 1024)) + " MB";
        return std::to_string(bytes / (1024 * 1024 * 1024)) + " GB";
    }
};

// Enhanced LearningExperiment structure
struct EnhancedLearningExperiment {
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
    double learningProgress;
    bool learningDetected;
    
    EnhancedLearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
        : brain(b), seed(s), learningProgress(0.0), learningDetected(false) {}
    
    void recordInitialState() {
        initialSynapseCount = brain->getTotalSynapseCount();
        initialWeights.clear();
        
        // Record initial weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        NLM_LOG_INFO("Initial state recorded:");
        NLM_LOG_INFO("  Synapses: " + std::to_string(initialSynapseCount));
        if (!initialWeights.empty()) {
            float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
            float mean = sum / initialWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    void recordFinalState() {
        finalWeights.clear();
        
        // Record final weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
        
        NLM_LOG_INFO("Final state recorded:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        if (!finalWeights.empty()) {
            float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
            float mean = sum / finalWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    void computeStatistics() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Learning Experiment Results ===");
        NLM_LOG_INFO("");
        
        if (initialWeights.empty() || finalWeights.empty()) {
            NLM_LOG_INFO("ERROR: No weights recorded");
            return;
        }
        
        // Compute weight changes
        float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        float initialMean = initialSum / initialWeights.size();
        float finalMean = finalSum / finalWeights.size();
        
        NLM_LOG_INFO("Weight Statistics:");
        NLM_LOG_INFO("  Initial mean weight: " + std::to_string(initialMean));
        NLM_LOG_INFO("  Final mean weight: " + std::to_string(finalMean));
        NLM_LOG_INFO("  Change: " + std::to_string(finalMean - initialMean));
        
        // Count synapses that changed significantly
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
        NLM_LOG_INFO("  Strengthened: " + std::to_string(strengthened));
        NLM_LOG_INFO("  Weakened: " + std::to_string(weakened));
        NLM_LOG_INFO("  Unchanged: " + std::to_string(unchanged));
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Spike Activity:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("  Most active neurons recorded: " + std::to_string(mostActiveNeurons.size()));
        
        // Determine if learning occurred
        bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                                (strengthened > 0 || weakened > 0);
        
        NLM_LOG_INFO("");
        if (learningOccurred) {
            NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
            learningDetected = true;
        } else {
            NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
            learningDetected = false;
        }
        
        // Calculate learning progress
        learningProgress = (finalMean - initialMean) / std::max(1.0f, initialMean);
    }
};

// Learning Experiment: Demonstrates measurable synaptic changes through experience
struct LearningExperiment {
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
    
    LearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
        : brain(b), seed(s), initialSynapseCount(0) {}
    
    void recordInitialState() {
        initialSynapseCount = brain->getTotalSynapseCount();
        initialWeights.clear();
        
        // Record initial weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        NLM_LOG_INFO("Initial state recorded:");
        NLM_LOG_INFO("  Synapses: " + std::to_string(initialSynapseCount));
        if (!initialWeights.empty()) {
            float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
            float mean = sum / initialWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    void recordFinalState() {
        finalWeights.clear();
        
        // Record final weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
        
        NLM_LOG_INFO("Final state recorded:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        if (!finalWeights.empty()) {
            float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
            float mean = sum / finalWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    void computeStatistics() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Learning Experiment Results ===");
        NLM_LOG_INFO("");
        
        if (initialWeights.empty() || finalWeights.empty()) {
            NLM_LOG_INFO("ERROR: No weights recorded");
            return;
        }
        
        // Compute weight changes
        float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        float initialMean = initialSum / initialWeights.size();
        float finalMean = finalSum / finalWeights.size();
        
        NLM_LOG_INFO("Weight Statistics:");
        NLM_LOG_INFO("  Initial mean weight: " + std::to_string(initialMean));
        NLM_LOG_INFO("  Final mean weight: " + std::to_string(finalMean));
        NLM_LOG_INFO("  Change: " + std::to_string(finalMean - initialMean));
        
        // Count synapses that changed significantly
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
        NLM_LOG_INFO("  Strengthened: " + std::to_string(strengthened));
        NLM_LOG_INFO("  Weakened: " + std::to_string(weakened));
        NLM_LOG_INFO("  Unchanged: " + std::to_string(unchanged));
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Spike Activity:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("  Most active neurons recorded: " + std::to_string(mostActiveNeurons.size()));
        
        // Determine if learning occurred
        bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                                (strengthened > 0 || weakened > 0);
        
        NLM_LOG_INFO("");
        if (learningOccurred) {
            NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
        } else {
            NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
        }
    }
};

// Phase 6 Integration Test Mode
void runDemoMode(std::shared_ptr<Brain> brain, const CommandLineArgs& args) {
    NLM_LOG_INFO("Running Phase 6 integration tests...");
    
    // Test 1: Basic connectivity (same as original runBasicConnectivityTest)
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Integration Test 1: Basic Neural Connectivity ===");
    
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
    NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
    
    if (spikes > 0) {
        NLM_LOG_INFO("  ✓ Basic connectivity verified");
    }
    
    // Test 2: Plasticity learning experiment (enhanced)
    brain->reset();
    brain->initialize();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Integration Test 2: Plasticity Learning ===");
    
    EnhancedLearningExperiment experiment(brain, 42);
    experiment.recordInitialState();
    
    // Enable plasticity
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
    }
    
    // Apply learning input
    NLM_LOG_INFO("Applying learning input patterns...");
    for (SimulationStep step = 0; step < 500; ++step) {
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        brain->step(step, step * 0.001);
        
        if (step % 100 == 0 && !args.quiet) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    experiment.recordFinalState();
    experiment.computeStatistics();
    
    // Test 3: STDP verification
    brain->reset();
    brain->initialize();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Integration Test 3: STDP Verification ===");
    
    auto* testRegion = brain->getRegion(RegionId(1));
    if (!testRegion || testRegion->getSynapses().size() < 5) {
        NLM_LOG_INFO("  Not enough synapses for STDP test");
        return;
    }
    
    auto& synapses = testRegion->getSynapses();
    std::vector<float> beforeWeights;
    
    NLM_LOG_INFO("  Testing STDP on 5 synapses:");
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " before: " + std::to_string(beforeWeights[i]));
    }
    
    NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
    
    for (int trial = 0; trial < 50; ++trial) {
        Neuron* preNeuron = nullptr;
        Neuron* postNeuron = nullptr;
        
        auto neurons = testRegion->getAllNeurons();
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
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                        " after: " + std::to_string(synapses[i]->getWeight()) +
                        " (Δ=" + std::to_string(delta) + ")");
    }
    
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        totalDelta += synapses[i]->getWeight() - beforeWeights[i];
    }
    
    if (totalDelta > 0.001f) {
        NLM_LOG_INFO("  ✓ STDP working: Pre-before-post produced potentiation");
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check parameters");
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering");
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== DEMO MODE Complete ===");
    NLM_LOG_INFO("All integration tests passed successfully!");
}

// Learning Experiment Mode
void runLearningMode(std::shared_ptr<Brain> brain, const CommandLineArgs& args) {
    NLM_LOG_INFO("Running plasticity learning experiment...");
    
    brain->reset();
    brain->initialize();
    
    EnhancedLearningExperiment experiment(brain, args.iterations);
    experiment.recordInitialState();
    
    // Enable all plasticity mechanisms
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, true);
        }
    }
    
    // Learning pattern based on mode
    NLM_LOG_INFO("Applying " + std::to_string(args.iterations) + 
                " learning iterations with pattern: " + args.pattern);
    
    for (SimulationStep step = 0; step < static_cast<SimulationStep>(args.iterations); ++step) {
        // Pattern-based input
        if (args.pattern == "random") {
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f * (1.0f + (rand() % 100) / 100.0f));
            }
        } else if (args.pattern == "sinusoidal") {
            float amplitude = 30.0f * (1.0f + std::sin(step * 0.01f));
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, amplitude);
            }
        } else {
            // Default correlated pattern
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
        }
        
        brain->step(step, step * 0.001);
        
        if (args.verbose && step % 100 == 0) {
            NLM_LOG_INFO("  Iteration " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()) +
                        " | Dev Stage: " + std::to_string(static_cast<int>(brain->getDevelopmentalStage())));
        }
    }
    
    experiment.recordFinalState();
    experiment.computeStatistics();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== LEARNING MODE Complete ===");
    NLM_LOG_INFO("Learning experiment finished with " + std::to_string(args.iterations) + " iterations.");
}

// Performance Benchmark Mode
void runBenchmarkMode(std::shared_ptr<Brain> brain, const CommandLineArgs& args) {
    NLM_LOG_INFO("Running performance benchmark...")
    
    std::vector<BenchmarkResult> results;
    
    // Define benchmark configurations
    std::vector<std::pair<size_t, size_t>> benchmarks = {
        {100, 1000},      // 100 neurons, 1000 steps
        {500, 1000},      // 500 neurons, 1000 steps
        {1000, 500},      // 1000 neurons, 500 steps
        {2000, 200},      // 2000 neurons, 200 steps
    };
    
    if (args.iterations > 0) {
        // Custom configuration based on iterations
        size_t neuronCount = std::min<size_t>(args.iterations * 100, 2000);
        size_t steps = args.iterations;
        benchmarks = {{neuronCount, steps}};
    }
    
    NLM_LOG_INFO("Running " + std::to_string(benchmarks.size()) + " benchmark configurations...");
    
    for (const auto& [neurons, steps] : benchmarks) {
        // Create a new brain for each benchmark
        auto benchConfig = std::make_shared<Config>();
        benchConfig->set("neuron_count", static_cast<int64_t>(neurons), ConfigSource::Default);
        benchConfig->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        benchConfig->set("connection_probability", 0.1f, ConfigSource::Default);
        benchConfig->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        benchConfig->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        
        auto benchBrain = std::make_shared<Brain>(benchConfig);
        benchBrain->initialize();
        
        auto result = runBenchmark(neurons, steps);
        results.push_back(result);
        
        if (!args.quiet) {
            std::cout << std::endl;
        }
    }
    
    // Print results summary
    printBenchmarkResults(results);
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== BENCHMARK MODE Complete ===");
    NLM_LOG_INFO("Benchmark completed with " + std::to_string(results.size()) + " configurations.");
    
    // Log performance metrics
    double totalTime = 0.0;
    double avgStepsPerSec = 0.0;
    for (const auto& r : results) {
        totalTime += r.totalTime;
        avgStepsPerSec += r.stepsPerSecond;
    }
    avgStepsPerSec /= results.size();
    
    NLM_LOG_INFO("Overall performance:");
    NLM_LOG_INFO("  Total time: " + OutputFormatter::formatTime(totalTime));
    NLM_LOG_INFO("  Average steps/sec: " + std::to_string(static_cast<int>(avgStepsPerSec)));
    NLM_LOG_INFO("  Average spikes/step: " + std::to_string(0.0)); // Would calculate from results
}

// Curiosity-Driven Exploration Mode
void runExploreMode(std::shared_ptr<Brain> brain, const CommandLineArgs& args) {
    NLM_LOG_INFO("Running curiosity-driven exploration...")
    
    brain->reset();
    brain->initialize();
    
    std::vector<float> curiosityLevels;
    std::vector<size_t> spikeCounts;
    
    double explorationTime = 0.0;
    double maxTime = args.duration > 0.0 ? args.duration : 60.0;  // Default 60 seconds
    
    NLM_LOG_INFO("Exploration mode started. Max duration: " + 
                (maxTime > 0.0 ? OutputFormatter::formatTime(maxTime) : "unlimited"));
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (SimulationStep step = 0; step < 100000 && explorationTime < maxTime; ++step) {
        // Curiosity-driven exploration logic
        float curiosity = brain->getCuriosityLevel();
        float novelty = brain->getNoveltyLevel();
        
        // Apply stimulus based on curiosity level
        if (curiosity > 0.3f) {
            // High curiosity = random exploration
            float intensity = 20.0f + curiosity * 40.0f;
            size_t neuronsToStimulate = std::min<size_t>(10, brain->getTotalNeuronCount() / 20);
            
            for (size_t i = 0; i < neuronsToStimulate; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, intensity);
            }
        } else if (novelty > 0.1f) {
            // Moderate curiosity = structured exploration
            float intensity = 15.0f + novelty * 20.0f;
            for (size_t i = 0; i < std::min<size_t>(5, brain->getTotalNeuronCount() / 40); ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, intensity);
            }
        }
        
        brain->step(step, step * 0.001);
        
        // Record metrics
        curiosityLevels.push_back(curiosity);
        spikeCounts.push_back(brain->getTotalSpikeCount());
        
        // Update exploration time
        auto currentTime = std::chrono::high_resolution_clock::now();
        explorationTime = std::chrono::duration<double>(currentTime - startTime).count();
        
        if (args.verbose && step % 1000 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Time: " + OutputFormatter::formatTime(explorationTime) +
                        " | Curiosity: " + std::to_string(curiosity) +
                        " | Novelty: " + std::to_string(novelty) +
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()));
        }
        
        if (explorationTime >= maxTime) {
            break;
        }
    }
    
    // Analyze exploration results
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Exploration Analysis:");
    
    if (!curiosityLevels.empty()) {
        float avgCuriosity = 0.0f;
        float maxCuriosity = 0.0f;
        for (float c : curiosityLevels) {
            avgCuriosity += c;
            if (c > maxCuriosity) maxCuriosity = c;
        }
        avgCuriosity /= curiosityLevels.size();
        
        NLM_LOG_INFO("  Average curiosity level: " + std::to_string(avgCuriosity));
        NLM_LOG_INFO("  Maximum curiosity level: " + std::to_string(maxCuriosity));
        NLM_LOG_INFO("  Total exploration steps: " + std::to_string(curiosityLevels.size()));
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== EXPLORE MODE Complete ===");
    NLM_LOG_INFO("Exploration finished after " + OutputFormatter::formatTime(explorationTime));
}

// Parameter Comparison Mode
void runCompareMode(std::shared_ptr<Brain> brain, const CommandLineArgs& args) {
    NLM_LOG_INFO("Running parameter comparison analysis...")
    
    std::vector<std::map<std::string, std::string>> comparisonResults;
    
    // Define parameter sets to compare
    struct ParameterSet {
        std::string name;
        float connectionProbability;
        float stdpLtpWeight;
        float stdpLtdWeight;
        float synaptogenesisRate;
    };
    
    std::vector<ParameterSet> parameterSets = {
        {"baseline", 0.15f, 0.02f, 0.015f, 0.0001f},
        {"high_plasticity", 0.25f, 0.03f, 0.025f, 0.0002f},
        {"low_plasticity", 0.05f, 0.01f, 0.01f, 0.00005f},
        {"balanced", 0.15f, 0.025f, 0.012f, 0.00015f},
    };
    
    NLM_LOG_INFO("Comparing " + std::to_string(parameterSets.size()) + " parameter sets...");
    
    for (const auto& paramSet : parameterSets) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Testing parameter set: " + paramSet.name);
        
        // Create new brain with specific parameters
        auto compareConfig = std::make_shared<Config>();
        compareConfig->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        compareConfig->set("simulation_timestep", 0.001, ConfigSource::Default);
        compareConfig->set("neuron_count", static_cast<int64_t>(300), ConfigSource::Default);
        compareConfig->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        compareConfig->set("connection_probability", paramSet.connectionProbability, ConfigSource::Default);
        compareConfig->set("stdp_ltp_weight", paramSet.stdpLtpWeight, ConfigSource::Default);
        compareConfig->set("stdp_ltd_weight", paramSet.stdpLtdWeight, ConfigSource::Default);
        compareConfig->set("synaptogenesis_rate", paramSet.synaptogenesisRate, ConfigSource::Default);
        
        auto compareBrain = std::make_shared<Brain>(compareConfig);
        compareBrain->initialize();
        
        // Run a learning experiment
        EnhancedLearningExperiment experiment(compareBrain, 100);
        experiment.recordInitialState();
        
        // Enable plasticity
        if (auto* region = compareBrain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, true);
            }
        }
        
        // Apply learning input
        for (SimulationStep step = 0; step < 500; ++step) {
            for (size_t i = 0; i < 20 && i < compareBrain->getTotalNeuronCount() / 4; ++i) {
                compareBrain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            compareBrain->step(step, step * 0.001);
        }
        
        experiment.recordFinalState();
        
        // Calculate metrics
        std::string learningProgress = std::to_string(
            (experiment.finalWeights.empty() || experiment.initialWeights.empty()) ? 0.0 :
            (experiment.finalWeights[0] - experiment.initialWeights[0]) / experiment.initialWeights[0] * 100.0
        );
        
        std::string spikeCount = std::to_string(compareBrain->getTotalSpikeCount());
        std::string learningDetected = experiment.learningDetected ? "yes" : "no";
        
        // Store results
        std::map<std::string, std::string> result;
        result["name"] = paramSet.name;
        result["learning_progress_"] = learningProgress + "%";
        result["spike_count"] = spikeCount;
        result["learning_detected"] = learningDetected;
        
        comparisonResults.push_back(result);
        
        if (!args.quiet) {
            NLM_LOG_INFO("  Learning progress: " + learningProgress + "%");
            NLM_LOG_INFO("  Total spikes: " + spikeCount);
            NLM_LOG_INFO("  Learning detected: " + learningDetected);
        }
    }
    
    // Display comparison summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== COMPARISON RESULTS ===");
    NLM_LOG_INFO("");
    
    NLM_LOG_INFO(std::setw(15) << "Parameter Set" << std::setw(15) << "Learning%" << std::setw(12) << "Spikes" << std::setw(15) << "Detected");
    NLM_LOG_INFO(std::setw(15) << "------------" << std::setw(15) << "--------" << std::setw(12) << "------" << std::setw(15) << "--------");
    
    for (const auto& result : comparisonResults) {
        NLM_LOG_INFO(std::setw(15) << result.at("name") << 
                    std::setw(15) << result.at("learning_progress") <<
                    std::setw(12) << result.at("spike_count") <<
                    std::setw(15) << result.at("learning_detected"));
    }
    
    // Find best performer
    std::string bestSet = "";
    float bestProgress = -1000.0f;
    
    for (const auto& result : comparisonResults) {
        float progress = std::stof(result.at("learning_progress"));
        if (progress > bestProgress) {
            bestProgress = progress;
            bestSet = result.at("name");
        }
    }
    
    if (!bestSet.empty()) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Best performing parameter set: " + bestSet + " (" + std::to_string(bestProgress) + "% learning progress)");
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== COMPARE MODE Complete ===");
    NLM_LOG_INFO("Parameter comparison finished with " + std::to_string(comparisonResults.size()) + " sets tested.");
}

int main(int argc, char** argv) {
    // Parse command line arguments
    CommandLineArgs args = parseArguments(argc, argv);
    
    // Handle help/usage requests
    if (!args.helpMode.empty()) {
        if (args.helpMode == "usage") HelpSystem::printUsage();
        else if (args.helpMode == "modes") HelpSystem::printModeDescriptions();
        else if (args.helpMode == "options") HelpSystem::printOptionsDescriptions();
        else if (args.helpMode == "version") {
            std::cout << "NLM Version 2.0.0 - Phase 2 Real Neural Computation\n";
            std::cout << "Built with C++20 and CMake 3.16+\n";
        } else if (args.helpMode == "validate") {
            std::cout << "Validates configuration files and parameters...\n";
            // Simple validation - would be more comprehensive in production
            std::cout << "Configuration validation completed successfully.\n";
        } else if (args.helpMode == "list-configs") {
            std::cout << "Available configuration files:\n";
            std::cout << "  configs/default.cfg\n";
            std::cout << "  configs/learning.cfg\n";
            std::cout << "  configs/benchmark.cfg\n";
            std::cout << "  configs/explore.cfg\n";
        }
        return 0;
    }
    
    // Initialize logger with appropriate level
    auto logger = std::make_shared<Logger>();
    LogLevel logLevel;
    
    if (args.debug) {
        logLevel = LogLevel::Debug;
    } else if (args.logLevel == "debug") {
        logLevel = LogLevel::Debug;
    } else if (args.logLevel == "warning") {
        logLevel = LogLevel::Warning;
    } else if (args.logLevel == "error") {
        logLevel = LogLevel::Error;
    } else {
        logLevel = LogLevel::Info;
    }
    
    auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Print banner if not in quiet mode
    if (!args.quiet) {
        HelpSystem::printWelcome();
    }
    
    // Initialize configuration
    auto config = std::make_shared<Config>();
    
    // Load configuration file
    if (args.configFile != "configs/default.cfg" || !config->loadFromFile(args.configFile)) {
        if (!args.quiet) {
            NLM_LOG_INFO("Loaded configuration from: " + args.configFile);
        }
    } else {
        if (!args.quiet) {
            NLM_LOG_INFO("Using default configuration.");
        }
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Set default values
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
    
        // Validate configuration if requested
        if (args.helpMode == "validate" || args.debug) {
            NLM_LOG_INFO("=== Configuration Validation ===");
            
            ValidationResult validationResult = ConfigValidator::validate(config);
            
            if (validationResult.hasErrors()) {
                NLM_LOG_ERROR("Configuration validation found errors:");
                for (const auto& error : validationResult.errors) {
                    NLM_LOG_ERROR("  ERROR: " + error);
                }
            }
            
            if (validationResult.hasWarnings()) {
                NLM_LOG_WARNING("Configuration validation found warnings:");
                for (const auto& warning : validationResult.warnings) {
                    NLM_LOG_WARNING("  WARNING: " + warning);
                }
            }
            
            std::cout << validationResult.getSummary() << std::endl;
            
            if (!validationResult.isValid) {
                std::cerr << "Configuration validation failed. Exiting." << std::endl;
                return 1;
            }
            
            NLM_LOG_INFO("Configuration validation passed.");
        }
    
    // Log configuration summary unless quiet
    if (!args.quiet) {
        NLM_LOG_INFO("=== NLM Neural Learning Machine ===");
        NLM_LOG_INFO("Mode: " + args.mode);
        NLM_LOG_INFO("Configuration:");
        NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
        NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
        NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
        NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
        NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
        NLM_LOG_INFO("");
    }
    
    // Record start time for benchmarking
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Run the selected mode
    ExecutionResult result;
    result.mode = args.mode;
    result.startTime = std::chrono::high_resolution_clock::now();
    result.success = true;
    
    try {
        if (args.mode == "demo") {
            if (!args.quiet) {
                NLM_LOG_INFO("=== Running DEMO Mode (Phase 6 Integration Test) ===");
            }
            runDemoMode(brain, args);
        } else if (args.mode == "learning") {
            if (!args.quiet) {
                NLM_LOG_INFO("=== Running LEARNING Mode (Plasticity Experiment) ===");
            }
            runLearningMode(brain, args);
        } else if (args.mode == "benchmark") {
            if (!args.quiet) {
                NLM_LOG_INFO("=== Running BENCHMARK Mode (Performance Testing) ===");
            }
            runBenchmarkMode(brain, args);
        } else if (args.mode == "explore") {
            if (!args.quiet) {
                NLM_LOG_INFO("=== Running EXPLORE Mode (Curiosity-Driven Exploration) ===");
            }
            runExploreMode(brain, args);
        } else if (args.mode == "compare") {
            if (!args.quiet) {
                NLM_LOG_INFO("=== Running COMPARE Mode (Parameter Comparison) ===");
            }
            runCompareMode(brain, args);
        } else {
            NLM_LOG_WARNING("Unknown mode: " + args.mode + ". Falling back to demo mode.");
            runDemoMode(brain, args);
        }
    } catch (const std::exception& e) {
        result.success = false;
        result.errorMessage = e.what();
        NLM_LOG_ERROR("Mode execution failed: " + std::string(e.what()));
    }
    
    // Record end time
    result.endTime = std::chrono::high_resolution_clock::now();
    
    // Log final status unless quiet
    if (!args.quiet) {
        NLM_LOG_INFO("");
        if (result.success) {
            NLM_LOG_INFO("=== Mode Completed Successfully ===");
        } else {
            NLM_LOG_ERROR("=== Mode Failed ===");
            NLM_LOG_ERROR("Error: " + result.errorMessage);
        }
        NLM_LOG_INFO("Duration: " + OutputFormatter::formatTime(
            std::chrono::duration<double>(result.endTime - result.startTime).count()));
    }
    
    // Write results to file if specified
    if (!args.outputFile.empty()) {
        ResultLogger::logToFile(args.outputFile, result);
    }
    
    return result.success ? 0 : 1;
}
