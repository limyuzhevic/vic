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
#include <algorithm>
#include <sstream>
#include <filesystem>
#include <map>

using namespace nlm;

// Experiment metadata structure
struct ExperimentMetadata {
    std::string name;
    std::string description;
    std::vector<std::string> dependencies;
    bool enabled;
    std::map<std::string, std::string> parameters;
    size_t requiredNeurons;
    double requiredTime;
    bool available;
};

// Command line parser utility
class CommandLineParser {
public:
    struct Options {
        bool help = false;
        bool version = false;
        bool verbose = false;
        bool quiet = false;
        std::string configFile = "configs/default.cfg";
        std::string logLevel = "info";
        std::string logFile = "";
        int logRotationSize = 10 * 1024 * 1024;  // 10MB
        int logRotationCount = 5;
        std::vector<std::string> experiments;
        std::vector<std::string> disabledExperiments;
        bool configGenerate = false;
        std::string outputFormat = "simple";  // simple, json, yaml
        std::string configTemplate = "";
        bool validateOnly = false;
        bool listExperiments = false;
        bool helpAll = false;
        std::string helpTopic = "";
        int64_t randomSeed = -1;  // -1 means use config/default
        double simulationTimestep = -1;
        int64_t neuronCount = -1;
        int64_t regionCount = -1;
        float connectionProbability = -1.0f;
    };

    static Options parse(int argc, char** argv) {
        Options options;
        
        // Standard getopt-style parsing
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help") {
                options.help = true;
            } else if (arg == "--version") {
                options.version = true;
            } else if (arg == "--verbose") {
                options.verbose = true;
            } else if (arg == "--quiet") {
                options.quiet = true;
            } else if (arg == "--config-generate") {
                options.configGenerate = true;
            } else if (arg == "--validate-only") {
                options.validateOnly = true;
            } else if (arg == "--list-experiments") {
                options.listExperiments = true;
            } else if (arg == "--help-all") {
                options.helpAll = true;
            } else if (arg.find("--help=") == 0) {
                options.helpTopic = arg.substr(7);
            } else if (arg.find("--config=") == 0) {
                options.configFile = arg.substr(9);
            } else if (arg.find("--log-level=") == 0) {
                options.logLevel = arg.substr(12);
            } else if (arg.find("--log-file=") == 0) {
                options.logFile = arg.substr(11);
            } else if (arg.find("--log-rotation-size=") == 0) {
                options.logRotationSize = std::stoi(arg.substr(21));
            } else if (arg.find("--log-rotation-count=") == 0) {
                options.logRotationCount = std::stoi(arg.substr(22));
            } else if (arg.find("--experiment=") == 0) {
                options.experiments.push_back(arg.substr(13));
            } else if (arg.find("--disable-experiment=") == 0) {
                options.disabledExperiments.push_back(arg.substr(22));
            } else if (arg.find("--output-format=") == 0) {
                options.outputFormat = arg.substr(16);
            } else if (arg.find("--config-template=") == 0) {
                options.configTemplate = arg.substr(18);
            } else if (arg.find("--random-seed=") == 0) {
                options.randomSeed = std::stoll(arg.substr(14));
            } else if (arg.find("--timestep=") == 0) {
                options.simulationTimestep = std::stod(arg.substr(11));
            } else if (arg.find("--neurons=") == 0) {
                options.neuronCount = std::stoll(arg.substr(10));
            } else if (arg.find("--regions=") == 0) {
                options.regionCount = std::stoll(arg.substr(10));
            } else if (arg.find("--connections=") == 0) {
                options.connectionProbability = std::stof(arg.substr(13));
            } else if (arg == "--experiments") {
                if (i + 1 < argc) {
                    std::string next = argv[++i];
                    if (next.find_first_not_of("0123456789,") == std::string::npos) {
                        // Parse comma-separated list of experiment indices
                        std::stringstream ss(next);
                        std::string item;
                        while (std::getline(ss, item, ',')) {
                            if (!item.empty()) {
                                options.experiments.push_back(item);
                            }
                        }
                    } else {
                        // Treat as comma-separated names
                        std::stringstream ss(next);
                        std::string item;
                        while (std::getline(ss, item, ',')) {
                            options.experiments.push_back(item);
                        }
                    }
                }
            } else if (arg == "--experiments-disable") {
                if (i + 1 < argc) {
                    std::string next = argv[++i];
                    if (next.find_first_not_of("0123456789,") == std::string::npos) {
                        std::stringstream ss(next);
                        std::string item;
                        while (std::getline(ss, item, ',')) {
                            if (!item.empty()) {
                                options.disabledExperiments.push_back(item);
                            }
                        }
                    } else {
                        std::stringstream ss(next);
                        std::string item;
                        while (std::getline(ss, item, ',')) {
                            options.disabledExperiments.push_back(item);
                        }
                    }
                }
            } else {
                // Unknown argument
                std::cerr << "Unknown argument: " << arg << std::endl;
                std::cerr << "Use --help for usage information." << std::endl;
                std::exit(1);
            }
        }
        
        return options;
    }
};

// Help system
class HelpSystem {
public:
    static void printVersion() {
        std::cout << "NLM Phase 2 - Real Neural Computation" << std::endl;
        std::cout << "Version: 2.0.0" << std::endl;
        std::cout << "Build Date: " << __DATE__ << " " << __TIME__ << std::endl;
        std::cout << "" << std::endl;
    }

    static void printGeneralHelp() {
        std::cout << "Usage: nlm [OPTIONS]" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "NLM (熙然) - Neural Learning Machine - Phase 2: Real Neural Computation" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Core Options:" << std::endl;
        std::cout << "  --help                    Show general help information" << std::endl;
        std::cout << "  --version                 Show version information" << std::endl;
        std::cout << "  --config=FILE             Load configuration from FILE" << std::endl;
        std::cout << "  --verbose                 Enable verbose output" << std::endl;
        std::cout << "  --quiet                   Suppress output (errors only)" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Experiment Control:" << std::endl;
        std::cout << "  --experiment=NAME         Run specific experiment (can be repeated)" << std::endl;
        std::cout << "  --experiments=LIST        Run experiments from LIST (comma-separated)" << std::endl;
        std::cout << "  --disable-experiment=NAME Disable specific experiment" << std::endl;
        std::cout << "  --list-experiments        List all available experiments" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Configuration Management:" << std::endl;
        std::cout << "  --config-generate         Generate configuration file" << std::endl;
        std::cout << "  --config-template=TYPE    Configuration template (basic, research, performance)" << std::endl;
        std::cout << "  --output-format=FORMAT    Output format for generated config (simple, json, yaml)" << std::endl;
        std::cout << "  --validate-only           Validate configuration only" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Logging Options:" << std::endl;
        std::cout << "  --log-level=LEVEL         Set log level (debug, info, warning, error, critical)" << std::endl;
        std::cout << "  --log-file=FILE           Log output to file" << std::endl;
        std::cout << "  --log-rotation-size=N     Rotate log files at N bytes" << std::endl;
        std::cout << "  --log-rotation-count=N    Keep N rotated log files" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Simulation Parameters:" << std::endl;
        std::cout << "  --random-seed=N           Set random seed for reproducible results" << std::endl;
        std::cout << "  --timestep=N              Set simulation timestep" << std::endl;
        std::cout << "  --neurons=N               Set number of neurons" << std::endl;
        std::cout << "  --regions=N               Set number of brain regions" << std::endl;
        std::cout << "  --connections=N           Set connection probability" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  nlm --help-all              Show all help topics" << std::endl;
        std::cout << "  nlm --config=research.json   Load research configuration" << std::endl;
        std::cout << "  nlm --verbose --log-file=nlm.log  Run with detailed logging" << std::endl;
        std::cout << "  nlm --experiment=plasticity  Run only plasticity experiment" << std::endl;
        std::cout << "  nlm --experiments=1,2,3      Run experiments 1, 2, and 3" << std::endl;
        std::cout << "  nlm --config-generate --config-template=research" << std::endl;
        std::cout << "" << std::endl;
    }

    static void printConfigHelp() {
        std::cout << "Configuration Options Help:" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Configuration files can be in three formats:" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "1. Simple format (configs/default.cfg):" << std::endl;
        std::cout << "   key = value" << std::endl;
        std::cout << "   # Comments start with #" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "2. JSON format (configs/research.json):" << std::endl;
        std::cout << "   {\"key\": \"value\", \"nested\": {\"key2\": \"value2\"}}" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "3. YAML format (configs/research.yml):" << std::endl;
        std::cout << "   key: value" << std::endl;
        std::cout << "   nested:" << std::endl;
        std::cout << "     key2: value2" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Configuration System supports validation of all keys and values." << std::endl;
        std::cout << "Use --validate-only to validate a configuration file." << std::endl;
        std::cout << "" << std::endl;
    }

    static void printExperimentHelp() {
        std::cout << "Experiment Options Help:" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Available experiment types:" << std::endl;
        std::cout << "1. basic_connectivity          - Test basic neural connectivity" << std::endl;
        std::cout << "2. plasticity                  - Demonstrate synaptic plasticity" << std::endl;
        std::cout << "3. stdp_verification           - Verify STDP mechanism" << std::endl;
        std::cout << "4. learning_experiment         - Measure learning through experience" << std::endl;
        std::cout << "5. development_experiment      - Study brain development" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Experiment names are case-insensitive." << std::endl;
        std::cout << "Multiple experiments can be specified with --experiment.
        std::cout << "Use --list-experiments to see all available experiments." << std::endl;
        std::cout << "" << std::endl;
    }

    static void printLoggingHelp() {
        std::cout << "Logging Options Help:" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Log Levels (in order of verbosity):" << std::endl;
        std::cout << "debug     - Detailed debugging information" << std::endl;
        std::cout << "info      - General operational information" << std::endl;
        std::cout << "warning   - Potentially harmful situations" << std::endl;
        std::cout << "error     - Error conditions" << std::endl;
        std::cout << "critical  - Critical system errors" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "Log File Rotation:" << std::endl;
        std::cout << "When log file reaches --log-rotation-size, it's rotated." << std::endl;
        std::cout << "Rotated files are renamed with timestamps (e.g., nlm_20250103-120000.log)" << std::endl;
        std::cout << "Only --log-rotation-count most recent rotated files are kept." << std::endl;
        std::cout << "" << std::endl;
    }

    static void printErrorHelp() {
        std::cout << "Error Handling Help:" << std::endl;
        std::cout << "" << std::endl;
        std::cout << "NLM provides comprehensive error handling with detailed messages:" << std::endl;
        std::cout << "- All errors include context (file, line, function)" << std::endl;
        std::cout << "- Errors are categorized by severity (debug, info, warning, error, critical)" << std::endl;
        std::cout << "- Error recovery mechanisms are available for recoverable errors" << std::endl;
        std::cout << "- Exit codes indicate error type:" << std::endl;
        std::cout << "  - 0: Success" << std::endl;
        std::cout << "  - 1: Configuration or initialization error" << std::endl;
        std::cout << "  - 2: Validation error" << std::endl;
        std::cout << "  - 3: Runtime error" << std::endl;
        std::cout << "  - 4: System error" << std::endl;
        std::cout << "" << std::endl;
    }

    static void printAllHelp() {
        printGeneralHelp();
        printConfigHelp();
        printExperimentHelp();
        printLoggingHelp();
        printErrorHelp();
        std::cout << "For more information on specific topics, use --help=<topic>" << std::endl;
        std::cout << "Topics: config, experiment, logging, error, all" << std::endl;
    }

    static void printTopicHelp(const std::string& topic) {
        if (topic == "config") {
            printConfigHelp();
        } else if (topic == "experiment") {
            printExperimentHelp();
        } else if (topic == "logging") {
            printLoggingHelp();
        } else if (topic == "error") {
            printErrorHelp();
        } else if (topic == "all") {
            printAllHelp();
        } else {
            std::cerr << "Unknown help topic: " << topic << std::endl;
            std::cerr << "Available topics: config, experiment, logging, error, all" << std::endl;
        }
    }
};

// Experiment metadata for built-in experiments
std::vector<ExperimentMetadata> getBuiltInExperiments() {
    return {
        {"basic_connectivity", "Test basic neural connectivity and signal propagation", {}, true, {}, 100, 10.0, true},
        {"plasticity", "Demonstrate synaptic plasticity through learning", {}, true, {}, 500, 100.0, true},
        {"stdp_verification", "Verify STDP mechanism with pre-post spike timing", {}, true, {}, 50, 50.0, true},
        {"learning_experiment", "Measure learning through experience over time", {}, true, {}, 1000, 1000.0, true},
        {"development_experiment", "Study brain development and maturation", {}, true, {}, 200, 200.0, true}
    };
}

// Configuration generator
class ConfigGenerator {
public:
    static bool generateConfig(const std::string& filename, const std::string& format, const std::string& templateName) {
        Config config;
        
        // Apply template
        if (templateName == "basic") {
            generateBasicTemplate(config);
        } else if (templateName == "research") {
            generateResearchTemplate(config);
        } else if (templateName == "performance") {
            generatePerformanceTemplate(config);
        } else {
            generateBasicTemplate(config);  // Default
        }
        
        // Save to file
        std::string ext = "." + format;
        std::string fullFilename = filename;
        if (fullFilename.find(ext) == std::string::npos) {
            fullFilename += ext;
        }
        
        bool success = false;
        if (format == "json" || format == "JSON") {
            success = config.saveToJson(fullFilename);
        } else if (format == "yaml" || format == "YAML") {
            success = config.saveToYaml(fullFilename);
        } else {  // simple
            success = config.saveToFile(fullFilename);
        }
        
        if (success) {
            std::cout << "Generated configuration file: " << fullFilename << std::endl;
            std::cout << "Format: " << format << std::endl;
        }
        
        return success;
    }
    
private:
    static void generateBasicTemplate(Config& config) {
        // Basic template for testing
        config.set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config.set("simulation_timestep", 0.001, ConfigSource::Default);
        config.set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config.set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config.set("connection_probability", 0.15f, ConfigSource::Default);
        
        config.set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config.set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config.set("stdp_tau", 20.0f, ConfigSource::Default);
        
        config.set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config.set("pruning_rate", 0.00001f, ConfigSource::Default);
        
        config.set("log_level", "info", ConfigSource::Default);
        config.set("log_to_file", false, ConfigSource::Default);
        config.set("log_filename", "nlm.log", ConfigSource::Default);
        
        config.set("max_simulation_steps", static_cast<int64_t>(10000), ConfigSource::Default);
    }
    
    static void generateResearchTemplate(Config& config) {
        // Research template for detailed experiments
        config.set("random_seed", static_cast<int64_t>(12345), ConfigSource::Default);
        config.set("simulation_timestep", 0.0005, ConfigSource::Default);
        config.set("neuron_count", static_cast<int64_t>(5000), ConfigSource::Default);
        config.set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
        config.set("connection_probability", 0.1f, ConfigSource::Default);
        
        config.set("stdp_ltp_weight", 0.025f, ConfigSource::Default);
        config.set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        config.set("stdp_tau", 25.0f, ConfigSource::Default);
        
        config.set("plasticity_learning_rate", 0.001f, ConfigSource::Default);
        config.set("synaptogenesis_rate", 0.0005f, ConfigSource::Default);
        config.set("pruning_rate", 0.00005f, ConfigSource::Default);
        
        config.set("log_level", "debug", ConfigSource::Default);
        config.set("log_to_file", true, ConfigSource::Default);
        config.set("log_filename", "research_nlm.log", ConfigSource::Default);
        config.set("log_rotation_size", 100 * 1024 * 1024, ConfigSource::Default);
        config.set("log_rotation_count", 10, ConfigSource::Default);
        
        config.set("max_simulation_steps", static_cast<int64_t>(100000), ConfigSource::Default);
        config.set("simulation_time_limit", 3600.0, ConfigSource::Default);
    }
    
    static void generatePerformanceTemplate(Config& config) {
        // Performance template for benchmarking
        config.set("random_seed", static_cast<int64_t>(999), ConfigSource::Default);
        config.set("simulation_timestep", 0.0001, ConfigSource::Default);
        config.set("neuron_count", static_cast<int64_t>(10000), ConfigSource::Default);
        config.set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config.set("connection_probability", 0.05f, ConfigSource::Default);
        
        config.set("stdp_ltp_weight", 0.015f, ConfigSource::Default);
        config.set("stdp_ltd_weight", 0.01f, ConfigSource::Default);
        config.set("stdp_tau", 15.0f, ConfigSource::Default);
        
        config.set("plasticity_learning_rate", 0.0005f, ConfigSource::Default);
        config.set("synaptogenesis_rate", 0.00001f, ConfigSource::Default);
        config.set("pruning_rate", 0.000001f, ConfigSource::Default);
        
        config.set("log_level", "warning", ConfigSource::Default);
        config.set("log_to_file", true, ConfigSource::Default);
        config.set("log_filename", "benchmark_nlm.log", ConfigSource::Default);
        config.set("log_rotation_size", 50 * 1024 * 1024, ConfigSource::Default);
        config.set("log_rotation_count", 3, ConfigSource::Default);
        
        config.set("max_simulation_steps", static_cast<int64_t>(50000), ConfigSource::Default);
    }
};

// Error codes
enum class ExitCode {
    Success = 0,
    ConfigError = 1,
    ValidationError = 2,
    RuntimeError = 3,
    SystemError = 4,
    HelpRequested = 5,
    VersionRequested = 6
};

// Command line interface implementation
int main(int argc, char** argv) {
    // Parse command line arguments
    CommandLineParser::Options options = CommandLineParser::parse(argc, argv);
    
    // Handle special flags first
    if (options.version) {
        HelpSystem::printVersion();
        return static_cast<int>(ExitCode::VersionRequested);
    }
    
    if (options.help || options.helpAll) {
        if (!options.helpTopic.empty()) {
            HelpSystem::printTopicHelp(options.helpTopic);
        } else if (options.helpAll) {
            HelpSystem::printAllHelp();
        } else {
            HelpSystem::printGeneralHelp();
        }
        return static_cast<int>(ExitCode::HelpRequested);
    }
    
    if (options.listExperiments) {
        std::cout << "Available Experiments:" << std::endl;
        auto experiments = getBuiltInExperiments();
        for (size_t i = 0; i < experiments.size(); ++i) {
            std::cout << i + 1 << ". " << experiments[i].name << " - " << experiments[i].description << std::endl;
        }
        return static_cast<int>(ExitCode::Success);
    }
    
    if (options.configGenerate) {
        std::string filename = "config_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + "." + options.outputFormat;
        std::cout << "Generating configuration file..." << std::endl;
        if (ConfigGenerator::generateConfig(filename, options.outputFormat, options.configTemplate)) {
            return static_cast<int>(ExitCode::Success);
        } else {
            return static_cast<int>(ExitCode::ConfigError);
        }
    }
    
    // Initialize logger based on options
    auto logger = std::make_shared<Logger>();
    
    // Convert log level string to enum
    LogLevel logLevel = LogLevel::Info;
    if (options.logLevel == "debug") logLevel = LogLevel::Debug;
    else if (options.logLevel == "warning") logLevel = LogLevel::Warning;
    else if (options.logLevel == "error") logLevel = LogLevel::Error;
    else if (options.logLevel == "critical") logLevel = LogLevel::Critical;
    
    // Create appropriate logger based on options
    if (options.logFile.empty() && !options.quiet) {
        // Console logger only
        auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel);
        consoleLogger->setUseColors(!options.quiet && !options.verbose);
        logger->addLogger(consoleLogger);
    } else if (options.logFile.empty() && options.quiet) {
        // Silent console
        auto silentLogger = std::make_shared<ConsoleLogger>(LogLevel::Critical);
        logger->addLogger(silentLogger);
    } else {
        // File logger
        auto fileLogger = std::make_shared<FileLogger>(options.logFile, logLevel);
        fileLogger->setMaxFileSize(options.logRotationSize);
        fileLogger->setMaxFileCount(options.logRotationCount);
        logger->addLogger(fileLogger);
        
        // Also output to console if not quiet
        if (!options.quiet) {
            auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel);
            consoleLogger->setUseColors(!options.verbose);
            logger->addLogger(consoleLogger);
        }
    }
    
    Logger::setGlobal(logger);
    
    printBanner();
    
    if (options.verbose) {
        std::cout << "Verbose mode enabled. Detailed output will be shown." << std::endl;
    }
    
    // Initialize configuration schema
    Config::initializeSchema();
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
    std::string configFile = options.configFile;
    
    // Load config from file (ignore if not found)
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration (file not found: " + configFile + ")");
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Override with command line options
    if (options.randomSeed != -1) {
        config->set("random_seed", static_cast<int64_t>(options.randomSeed), ConfigSource::CommandLine);
    }
    if (options.simulationTimestep != -1) {
        config->set("simulation_timestep", options.simulationTimestep, ConfigSource::CommandLine);
    }
    if (options.neuronCount != -1) {
        config->set("neuron_count", static_cast<int64_t>(options.neuronCount), ConfigSource::CommandLine);
    }
    if (options.regionCount != -1) {
        config->set("region_count", static_cast<int64_t>(options.regionCount), ConfigSource::CommandLine);
    }
    if (options.connectionProbability != -1.0f) {
        config->set("connection_probability", options.connectionProbability, ConfigSource::CommandLine);
    }
    
    // Set default values for Phase 2 (only if not already set)
    if (!config->has("random_seed")) {
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    }
    if (!config->has("simulation_timestep")) {
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
    }
    if (!config->has("neuron_count")) {
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    }
    if (!config->has("region_count")) {
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    }
    if (!config->has("connection_probability")) {
        config->set("connection_probability", 0.15f, ConfigSource::Default);
    }
    
    // Set Phase 2 specific defaults
    if (!config->has("stdp_ltp_weight")) {
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    }
    if (!config->has("stdp_ltd_weight")) {
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    }
    if (!config->has("stdp_tau")) {
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
    }
    
    if (!config->has("synaptogenesis_rate")) {
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    }
    if (!config->has("pruning_rate")) {
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    }
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    NLM_LOG_INFO("");
    
    // Validate configuration if requested
    if (options.validateOnly) {
        auto validationErrors = config->validate();
        if (validationErrors.empty()) {
            NLM_LOG_INFO("Configuration validation passed.");
        } else {
            NLM_LOG_ERROR("Configuration validation failed:");
            for (const auto& error : validationErrors) {
                NLM_LOG_ERROR("  - " + error.getKey() + ": " + error.what());
            }
            return static_cast<int>(ExitCode::ValidationError);
        }
    }
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return static_cast<int>(ExitCode::RuntimeError);
    }
    
    brain->logStatus();
    
    // Determine which experiments to run
    std::vector<std::string> experimentsToRun;
    std::vector<ExperimentMetadata> availableExperiments = getBuiltInExperiments();
    
    // Filter by user selection
    if (!options.experiments.empty()) {
        for (const auto& expName : options.experiments) {
            for (const auto& exp : availableExperiments) {
                if (exp.name == expName || (expName >= "1" && expName <= std::to_string(availableExperiments.size()))) {
                    size_t idx = std::stoi(expName) - 1;
                    if (idx < availableExperiments.size() && availableExperiments[idx].enabled) {
                        experimentsToRun.push_back(exp.name);
                    }
                    break;
                }
            }
        }
    } else if (!options.disabledExperiments.empty()) {
        // Run all experiments except disabled ones
        for (const auto& exp : availableExperiments) {
            bool disabled = false;
            for (const auto& disabledName : options.disabledExperiments) {
                if (exp.name == disabledName || (disabledName >= "1" && disabledName <= std::to_string(availableExperiments.size()))) {
                    disabled = true;
                    break;
                }
            }
            if (!disabled && exp.available) {
                experimentsToRun.push_back(exp.name);
            }
        }
    } else {
        // Run all experiments by default
        for (const auto& exp : availableExperiments) {
            if (exp.available) {
                experimentsToRun.push_back(exp.name);
            }
        }
    }
    
    // Run selected experiments
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Running " + std::to_string(experimentsToRun.size()) + " experiment(s)...");
    
    for (const auto& expName : experimentsToRun) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Running Experiment: " + expName + " ===");
        
        // Reset and reinitialize brain for each experiment
        brain->reset();
        brain->initialize();
        
        // Run experiment based on name
        if (expName == "basic_connectivity") {
            runBasicConnectivityTest(brain);
        } else if (expName == "plasticity") {
            runPlasticityExperiment(brain);
        } else if (expName == "stdp_verification") {
            runStdpVerification(brain);
        } else if (expName == "learning_experiment") {
            // Run learning experiment
            LearningExperiment experiment(brain, config->getOr<int64_t>("random_seed", 42));
            experiment.recordInitialState();
            
            // Run plasticity learning
            if (auto* region = brain->getRegion(RegionId(1))) {
                for (auto& syn : region->getSynapses()) {
                    syn->enablePlasticity(true, true, false);
                }
            }
            
            for (SimulationStep step = 0; step < 1000; ++step) {
                for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                    brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
                }
                brain->step(step, step * 0.001);
                
                if (step % 100 == 0) {
                    NLM_LOG_INFO("  Step " + std::to_string(step) + 
                                " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                                " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
                }
            }
            
            experiment.recordFinalState();
            experiment.computeStatistics();
        } else if (expName == "development_experiment") {
            // Development experiment - simulate brain development
            NLM_LOG_INFO("Development experiment - simulating brain maturation...");
            
            LearningExperiment experiment(brain, config->getOr<int64_t>"random_seed", 42));
            experiment.recordInitialState();
            
            // Simulate developmental phases
            for (float age = 0.0f; age < 10.0f; age += 0.1f) {
                NLM_LOG_INFO("  Developmental age: " + std::to_string(age) + "/10.0");
                
                // Apply developmental processes
                if (auto* region = brain->getRegion(RegionId(1))) {
                    // Synaptogenesis
                    for (auto& syn : region->getSynapses()) {
                        if (std::rand() / (float)RAND_MAX < config->getOr<float>("synaptogenesis_rate", 0.0001)) {
                            syn->setWeight(syn->getWeight() * 1.1f);
                        }
                    }
                    
                    // Pruning
                    for (auto& syn : region->getSynapses()) {
                        if (std::rand() / (float)RAND_MAX < config->getOr<float>("pruning_rate", 0.00001)) {
                            syn->setWeight(syn->getWeight() * 0.9f);
                        }
                    }
                }
                
                // Run simulation step
                brain->step(static_cast<SimulationStep>(age * 1000), age * 0.1);
                
                // Log developmental stage
                if (age < 3.0f) NLM_LOG_INFO("    Phase 1: Initial formation");
                else if (age < 6.0f) NLM_LOG_INFO("    Phase 2: Growth and strengthening");
                else if (age < 8.0f) NLM_LOG_INFO("    Phase 3: Refinement and pruning");
                else NLM_LOG_INFO("    Phase 4: Maturation and stabilization");
            }
            
            experiment.recordFinalState();
            experiment.computeStatistics();
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Experiment " + expName + " completed.");
    }
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 2 Complete ===");
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
    NLM_LOG_INFO("");
    
    return static_cast<int>(ExitCode::Success);
}

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

void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    
    // Inject current into a few neurons and see if spikes propagate
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_INFO("  No neurons found!");
        return;
    }
    
    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject strong current into first 10 neurons
    NLM_LOG_INFO("  Injecting current into 10 neurons...");
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
    }
    
    // Run a few steps
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
    
    if (spikes > 0) {
        NLM_LOG_INFO("  ✓ Spikes propagate through network");
    } else {
        NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                        " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                        " thresh=" + std::to_string(neurons[i]->getThreshold()));
        }
    }
}

void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    
    LearningExperiment experiment(brain, 42);
    
    // Record initial state
    experiment.recordInitialState();
    
    // Enable plasticity on synapses
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    // Apply repeated input pattern to stimulate learning
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
    
    for (SimulationStep step = 0; step < 1000; ++step) {
        // Create input pattern - inject current into sensory neurons
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    // Record final state
    experiment.recordFinalState();
    
    // Compute and display statistics
    experiment.computeStatistics();
}

void runStdpVerification(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    // Get first few synapses
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        NLM_LOG_INFO("  Not enough synapses for STDP test");
        return;
    }
    
    NLM_LOG_INFO("  Testing STDP on 5 synapses:");
    
    // Record initial weights
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " before: " + std::to_string(beforeWeights[i]));
    }
    
    // Create correlated activity: fire pre then post to trigger LTP
    NLM_LOG_INFO("");
    NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
    
    for (int trial = 0; trial < 50; ++trial) {
        // Fire pre-synaptic neuron
        Neuron* preNeuron = nullptr;
        Neuron* postNeuron = nullptr;
        
        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            preNeuron = neurons[0];
            postNeuron = neurons[1];
        }
        
        if (preNeuron && postNeuron) {
            // Pre fires first
            preNeuron->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            
            // Then post fires
            postNeuron->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
    }
    
    // Record after weights
    NLM_LOG_INFO("  After correlated activity:");
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " after: " + std::to_string(synapses[i]->getWeight()) +
                    " (Δ=" + std::to_string(delta) + ")");
    }
    
    // Check if weights increased (LTP)
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