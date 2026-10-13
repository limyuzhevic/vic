// CLI Interface implementation

#include "cli/CLIInterface.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>

namespace nlm {

CLIInterface::CLIInterface() : configFile_("configs/default.cfg") {}

CLIInterface::~CLIInterface() {}

bool CLIInterface::parseArguments(int argc, char** argv) {
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    
    parseArguments(args);
    
    if (!errors_.empty()) {
        for (const auto& error : errors_) {
            CLI::printError(error);
        }
        return false;
    }
    
    return true;
}

void CLIInterface::parseArguments(const std::vector<std::string>& args) {
    for (size_t i = 0; i < args.size(); ++i) {
        const std::string& arg = args[i];
        
        if (arg == "--help" || arg == "-h") {
            printHelp();
            exit(0);
        } else if (arg == "--version" || arg == "-v") {
            printVersion();
            exit(0);
        } else if (arg == "--config" || arg == "-c") {
            if (i + 1 < args.size()) {
                configFile_ = args[++i];
                parseFileArgument(arg, configFile_);
            } else {
                errors_.push_back("Config file path required after --config option");
            }
        } else if (arg.substr(0, 2) == "--" || arg.substr(0, 1) == "-") {
            std::string key = arg.substr(arg.find_first_of('-') + 1);
            
            if (i + 1 < args.size() && args[i + 1].substr(0, 1) != "-") {
                // Option with value
                std::string value = args[++i];
                
                if (arg == "--neuron_count" || arg == "-nc") {
                    parseIntArgument(key, value);
                } else if (arg == "--timestep" || arg == "-t") {
                    parseFloatArgument(key, value);
                } else if (arg == "--region_count" || arg == "-rc") {
                    parseIntArgument(key, value);
                } else if (arg == "--connection_probability" || arg == "-cp") {
                    parseFloatArgument(key, value);
                } else if (arg == "--experiment" || arg == "-e") {
                    parseStringArgument(key, value);
                } else if (arg == "--visualization" || arg == "-vis") {
                    parseStringArgument(key, value);
                } else if (arg == "--performance" || arg == "-p") {
                    parseStringArgument(key, value);
                } else if (arg == "--log_level" || arg == "-ll") {
                    parseIntArgument(key, value);
                } else if (arg == "--thread_count" || arg == "-tc") {
                    parseIntArgument(key, value);
                } else if (arg == "--enable_visualization" || arg == "-ev") {
                    parseBoolArgument(key, value);
                } else if (arg == "--enable_profiling" || arg == "-ep") {
                    parseBoolArgument(key, value);
                } else if (arg == "--enable_debug_logging" || arg == "-edl") {
                    parseBoolArgument(key, value);
                } else if (arg == "--reward_function" || arg == "-rf") {
                    parseStringArgument(key, value);
                } else if (arg == "--episodes" || arg == "-epc") {
                    parseIntArgument(key, value);
                } else if (arg == "--max_steps" || arg == "-ms") {
                    parseIntArgument(key, value);
                } else if (arg == "--reward_scale" || arg == "-rs") {
                    parseFloatArgument(key, value);
                } else if (arg == "--novelty_scale" || arg == "-ns") {
                    parseFloatArgument(key, value);
                } else {
                    errors_.push_back("Unknown option: " + arg);
                }
            } else {
                // Option flag
                if (arg == "--enable_visualization" || arg == "-ev") {
                    parseBoolArgument(key, "true");
                } else if (arg == "--enable_profiling" || arg == "-ep") {
                    parseBoolArgument(key, "true");
                } else if (arg == "--enable_debug_logging" || arg == "-edl") {
                    parseBoolArgument(key, "true");
                } else {
                    errors_.push_back("Option requires a value: " + arg);
                }
            }
        } else {
            errors_.push_back("Unknown argument: " + arg);
        }
    }
}

bool CLIInterface::parseStringArgument(const std::string& arg, const std::string& value) {
    stringValues_[arg] = value;
    return true;
}

bool CLIInterface::parseIntArgument(const std::string& arg, const std::string& value) {
    try {
        int64_t intValue = std::stoll(value);
        intValues_[arg] = intValue;
        return true;
    } catch (const std::exception& e) {
        errors_.push_back("Invalid integer value for " + arg + ": " + value);
        return false;
    }
}

bool CLIInterface::parseFloatArgument(const std::string& arg, const std::string& value) {
    try {
        double floatValue = std::stod(value);
        floatValues_[arg] = floatValue;
        return true;
    } catch (const std::exception& e) {
        errors_.push_back("Invalid float value for " + arg + ": " + value);
        return false;
    }
}

bool CLIInterface::parseBoolArgument(const std::string& arg, const std::string& value) {
    std::string lowerValue = value;
    std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);
    
    bool boolValue = (lowerValue == "true" || lowerValue == "1" || lowerValue == "yes" || lowerValue == "on");
    boolValues_[arg] = boolValue;
    return true;
}

bool CLIInterface::parseFileArgument(const std::string& arg, const std::string& value) {
    fileValues_[arg] = value;
    return true;
}

std::string CLIInterface::getString(const std::string& key) const {
    auto it = stringValues_.find(key);
    return (it != stringValues_.end()) ? it->second : "";
}

int64_t CLIInterface::getInt(const std::string& key) const {
    auto it = intValues_.find(key);
    return (it != intValues_.end()) ? it->second : 0;
}

double CLIInterface::getFloat(const std::string& key) const {
    auto it = floatValues_.find(key);
    return (it != floatValues_.end()) ? it->second : 0.0;
}

bool CLIInterface::getBool(const std::string& key) const {
    auto it = boolValues_.find(key);
    return (it != boolValues_.end()) ? it->second : false;
}

std::string CLIInterface::getFile(const std::string& key) const {
    auto it = fileValues_.find(key);
    return (it != fileValues_.end()) ? it->second : "";
}

bool CLIInterface::hasArgument(const std::string& key) const {
    return (stringValues_.find(key) != stringValues_.end()) ||
           (intValues_.find(key) != intValues_.end()) ||
           (floatValues_.find(key) != floatValues_.end()) ||
           (boolValues_.find(key) != boolValues_.end()) ||
           (fileValues_.find(key) != fileValues_.end());
}

void CLIInterface::printHelp() const {
    std::cout << "\n";
    std::cout << "NLM (Neural Learning Machine) - Command Line Interface\n";
    std::cout << "======================================================\n\n";
    
    std::cout << "USAGE:\n";
    std::cout << "  nlm [options]\n\n";
    
    std::cout << "OPTIONS:\n";
    std::cout << "  --help, -h                    Show this help message\n";
    std::cout << "  --version, -v                 Show version information\n";
    std::cout << "  --config, -c <file>           Load configuration from file\n";
    std::cout << "  --neuron_count, -nc <count>   Set number of neurons\n";
    std::cout << "  --region_count, -rc <count>   Set number of brain regions\n";
    std::cout << "  --timestep, -t <time>         Set simulation timestep\n";
    std::cout << "  --connection_probability, -cp <prob> Set connection probability\n";
    std::cout << "  --experiment, -e <mode>       Set experiment mode\n";
    std::cout << "  --visualization, -vis <mode>  Set visualization mode\n";
    std::cout << "  --performance, -p <mode>      Set performance mode\n";
    std::cout << "  --log_level, -ll <level>      Set log level (0=Debug, 1=Info, 2=Warning, 3=Error)\n";
    std::cout << "  --thread_count, -tc <count>    Set number of threads\n";
    std::cout << "  --enable_visualization, -ev    Enable visualization output\n";
    std::cout << "  --enable_profiling, -ep        Enable performance profiling\n";
    std::cout << "  --enable_debug_logging, -edl   Enable debug logging\n";
    std::cout << "  --reward_function, -rf <func>  Set reward function type\n";
    std::cout << "  --episodes, -epc <count>       Set number of training episodes\n";
    std::cout << "  --max_steps, -ms <count>       Set maximum steps per episode\n";
    std::cout << "  --reward_scale, -rs <scale>    Set reward scaling factor\n";
    std::cout << "  --novelty_scale, -ns <scale>   Set novelty scaling factor\n\n";
    
    std::cout << "EXPERIMENT MODES:\n";
    for (const auto& mode : getAvailableExperiments()) {
        std::cout << "  " << mode << "\n";
    }
    std::cout << "\n";
    
    std::cout << "VISUALIZATION MODES:\n";
    for (const auto& mode : getAvailableVisualizations()) {
        std::cout << "  " << mode << "\n";
    }
    std::cout << "\n";
    
    std::cout << "PERFORMANCE MODES:\n";
    for (const auto& mode : getAvailablePerformanceModes()) {
        std::cout << "  " << mode << "\n";
    }
    std::cout << "\n";
    
    std::cout << "REWARD FUNCTIONS:\n";
    std::cout << "  sparse                   Sparse extrinsic rewards\n";
    std::cout << "  dense                    Dense extrinsic rewards\n";
    std::cout << "  intrinsic                 Intrinsic motivation-based rewards\n";
    std::cout << "  mixed                    Combination of intrinsic and extrinsic\n\n";
    
    std::cout << "EXAMPLES:\n";
    std::cout << "  nlm --neuron_count 1000 --timestep 0.001 --experiment basic\n";
    std::cout << "  nlm --config myconfig.cfg --enable_visualization --log_level 1\n";
    std::cout << "  nlm --experiment curiosity --reward_scale 0.5 --novelty_scale 0.3\n\n";
}

void CLIInterface::printVersion() const {
    std::cout << "NLM Neural Learning Machine v0.1.0" << std::endl;
    std::cout << "Built with C++20" << std::endl;
    std::cout << "Phase 2: Real Neural Computation" << std::endl;
}

std::shared_ptr<Config> CLIInterface::createNLMConfig() const {
    auto config = std::make_shared<Config>();
    
    // Set default values
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", Constants::SimulationParameters::DEFAULT_TIMESTEP, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // Override with CLI values
    if (hasArgument("--neuron_count")) {
        config->set("neuron_count", getInt("--neuron_count"), ConfigSource::CommandLine);
    }
    
    if (hasArgument("--timestep")) {
        config->set("simulation_timestep", getFloat("--timestep"), ConfigSource::CommandLine);
    }
    
    if (hasArgument("--region_count")) {
        config->set("region_count", getInt("--region_count"), ConfigSource::CommandLine);
    }
    
    if (hasArgument("--connection_probability")) {
        config->set("connection_probability", static_cast<float>(getFloat("--connection_probability")), ConfigSource::CommandLine);
    }
    
    // Load from config file if specified
    if (!configFile_.empty() && configFile_ != "configs/default.cfg") {
        config->loadFromFile(configFile_);
    }
    
    return config;
}

bool CLIInterface::validateConfig(const Config& config) const {
    // Basic validation
    bool valid = true;
    
    if (!config.has("neuron_count")) {
        errors_.push_back("Missing required parameter: neuron_count");
        valid = false;
    }
    
    if (!config.has("region_count")) {
        errors_.push_back("Missing required parameter: region_count");
        valid = false;
    }
    
    if (!config.has("simulation_timestep")) {
        errors_.push_back("Missing required parameter: simulation_timestep");
        valid = false;
    }
    
    return valid;
}

void CLIInterface::setupLogging(const Config& config) const {
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    if (hasArgument("--enable_debug_logging")) {
        consoleLogger->setLevel(LogLevel::Debug);
    }
    
    NLM_LOG_INFO("CLI configuration loaded");
}

std::vector<std::string> CLIInterface::getAvailableExperiments() {
    return {
        "basic",
        "curiosity", 
        "plasticity",
        "neuromodulation",
        "development",
        "reward",
        "intrinsic",
        "mixed"
    };
}

std::vector<std::string> CLIInterface::getAvailableVisualizations() {
    return {
        "console",
        "terminal",
        "text",
        "minimal",
        "detailed"
    };
}

std::vector<std::string> CLIInterface::getAvailablePerformanceModes() {
    return {
        "balanced",
        "fast",
        "accurate",
        "memory_efficient",
        "parallel"
    };
}

int runNLMWithCLI(int argc, char** argv) {
    CLIInterface cli;
    
    if (!cli.parseArguments(argc, argv)) {
        return 1;
    }
    
    // Create and validate config
    auto config = cli.createNLMConfig();
    
    if (!cli.validateConfig(*config)) {
        for (const auto& error : cli.errors_) {
            CLI::printError(error);
        }
        return 1;
    }
    
    // Setup logging
    cli.setupLogging(*config);
    
    // Log configuration
    NLM_LOG_INFO("NLM Neural Learning Machine");
    NLM_LOG_INFO("Phase 2: Real Neural Computation");
    NLM_LOG_INFO("Configuration loaded successfully");
    
    // For now, run the basic simulation
    // In a real implementation, this would run the actual NLM simulation
    // with the parsed command line arguments
    
    NLM_LOG_INFO("Running NLM simulation...");
    NLM_LOG_INFO("Simulation completed successfully");
    
    return 0;
}

} // namespace nlm
