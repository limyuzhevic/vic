#include "NLMCommandLine.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>

namespace nlm {

NLMCommandLine::NLMCommandLine() : configLoaded_(false) {
    config_ = std::make_shared<Config>();
}

NLMCommandLine::~NLMCommandLine() = default;

NLMCommandLine::NLMCommandLine(NLMCommandLine&& other) noexcept 
    : config_(std::move(other.config_)), configLoaded_(other.configLoaded_),
      configFile_(std::move(other.configFile_)), outputFile_(std::move(other.outputFile_)),
      experimentMode_(std::move(other.experimentMode_)), presetConfig_(std::move(other.presetConfig_)),
      logLevel_(std::move(other.logLevel_)), useColors_(other.useColors_),
      logToFile_(other.logToFile_), verbose_(other.verbose_), quiet_(other.quiet_),
      maxSteps_(other.maxSteps_), timestep_(other.timestep_),
      neuronCount_(other.neuronCount_), regionCount_(other.regionCount_),
      experimentName_(std::move(other.experimentName_)) {
    other.configLoaded_ = false;
}

NLMCommandLine& NLMCommandLine::operator=(NLMCommandLine&& other) noexcept {
    if (this != &other) {
        config_ = std::move(other.config_);
        configLoaded_ = other.configLoaded_;
        configFile_ = std::move(other.configFile_);
        outputFile_ = std::move(other.outputFile_);
        experimentMode_ = std::move(other.experimentMode_);
        presetConfig_ = std::move(other.presetConfig_);
        logLevel_ = std::move(other.logLevel_);
        useColors_ = other.useColors_;
        logToFile_ = other.logToFile_;
        verbose_ = other.verbose_;
        quiet_ = other.quiet_;
        maxSteps_ = other.maxSteps_;
        timestep_ = other.timestep_;
        neuronCount_ = other.neuronCount_;
        regionCount_ = other.regionCount_;
        experimentName_ = std::move(other.experimentName_);
        
        other.configLoaded_ = false;
    }
    return *this;
}

int NLMCommandLine::parseArgs(int argc, char** argv) {
    // Set default values
    experimentMode_ = "phase6";
    presetConfig_ = "default";
    logLevel_ = "info";
    useColors_ = true;
    logToFile_ = false;
    verbose_ = false;
    quiet_ = false;
    maxSteps_ = 1000;
    timestep_ = 0.001;
    neuronCount_ = 500;
    regionCount_ = 1;
    
    // Parse arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        }
        else if (arg == "--version") {
            printVersion();
            return 0;
        }
        else if (arg.substr(0, 2) == "--") {
            std::string key = arg.substr(2);
            std::string value = "";
            
            size_t pos = key.find('=');
            if (pos != std::string::npos) {
                value = key.substr(pos + 1);
                key = key.substr(0, pos);
            } else if (i + 1 < argc && argv[i + 1][0] != '-') {
                value = argv[++i];
            }
            
            // Handle special options
            if (key == "config") {
                configFile_ = value;
            } else if (key == "mode") {
                experimentMode_ = value;
            } else if (key == "preset") {
                presetConfig_ = value;
            } else if (key == "log-level") {
                logLevel_ = value;
            } else if (key == "verbose") {
                verbose_ = true;
            } else if (key == "quiet") {
                quiet_ = true;
            } else if (key == "max-steps") {
                maxSteps_ = std::stoi(value);
            } else if (key == "timestep") {
                timestep_ = std::stof(value);
            } else if (key == "neuron-count") {
                neuronCount_ = std::stoi(value);
            } else if (key == "region-count") {
                regionCount_ = std::stoi(value);
            } else if (key == "experiment-name") {
                experimentName_ = value;
            } else {
                config_->set(key, value, ConfigSource::CommandLine);
            }
        }
        else if (arg.substr(0, 1) == "-") {
            std::string key = arg.substr(1);
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                std::string value = argv[++i];
                config_->set(key, value, ConfigSource::CommandLine);
            }
        }
    }
    
    // Load config file if specified
    if (!configFile_.empty()) {
        loadConfigFile(configFile_);
    }
    
    // Apply preset configuration
    applyPreset(presetConfig_);
    
    return 0;
}

void NLMCommandLine::printHelp() const {
    std::cout << "=== NLM Command Line Interface ===" << std::endl;
    std::cout << "Usage: ./nlm [options]" << std::endl << std::endl;
    
    std::cout << "Basic Options:" << std::endl;
    std::cout << "  --help, -h                Show this help message" << std::endl;
    std::cout << "  --version                 Show version information" << std::endl;
    std::cout << "  --config <file>           Load configuration from file" << std::endl;
    std::cout << "  --mode <mode>             Experiment mode: phase2, phase4, phase6, custom" << std::endl;
    std::cout << "  --preset <preset>         Configuration preset: default, performance, research, minimal" << std::endl;
    std::cout << "  --log-level <level>       Logging level: debug, info, warning, error, critical" << std::endl;
    std::cout << "  --verbose                 Enable verbose output" << std::endl;
    std::cout << "  --quiet                   Suppress output" << std::endl;
    std::cout << "  --max-steps <n>           Maximum simulation steps" << std::endl;
    std::cout << "  --timestep <ms>           Simulation timestep in milliseconds" << std::endl;
    std::cout << "  --neuron-count <n>        Number of neurons" << std::endl;
    std::cout << "  --region-count <n>        Number of brain regions" << std::endl;
    std::cout << "  --experiment-name <name>  Name for the experiment" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Examples:" << std::endl;
    std::cout << "  ./nlm --mode phase6 --preset performance --max-steps 5000" << std::endl;
    std::cout << "  ./nlm --mode phase2 --config my_config.cfg --verbose" << std::endl;
    std::cout << "  ./nlm --mode custom --neuron-count 100 --region-count 2" << std::endl;
    std::cout << std::endl;
}

void NLMCommandLine::printVersion() const {
    std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
    std::cout << "Version: 2.0.0 (Phase 6 Final Integration)" << std::endl;
    std::cout << "Architecture: Experimental Artificial Developmental Brain" << std::endl;
    std::cout << "License: MIT" << std::endl;
}

bool NLMCommandLine::loadConfigFile(const std::string& filename) {
    if (config_->loadFromFile(filename)) {
        configLoaded_ = true;
        NLM_LOG_INFO("Loaded configuration from: " + filename);
        return true;
    } else {
        NLM_LOG_WARNING("Failed to load configuration from: " + filename);
        return false;
    }
}

void NLMCommandLine::applyPreset(const std::string& preset) {
    if (preset == "performance") {
        config_->set("random_seed", static_cast<int64_t>(123), ConfigSource::Runtime);
        config_->set("simulation_timestep", 0.0005, ConfigSource::Runtime);
        config_->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Runtime);
        config_->set("region_count", static_cast<int64_t>(2), ConfigSource::Runtime);
        config_->set("connection_probability", 0.1f, ConfigSource::Runtime);
        config_->set("enable_checkpointing", true, ConfigSource::Runtime);
        config_->set("enable_replay", true, ConfigSource::Runtime);
    } else if (preset == "research") {
        config_->set("random_seed", static_cast<int64_t>(456), ConfigSource::Runtime);
        config_->set("simulation_timestep", 0.001, ConfigSource::Runtime);
        config_->set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Runtime);
        config_->set("region_count", static_cast<int64_t>(4), ConfigSource::Runtime);
        config_->set("connection_probability", 0.05f, ConfigSource::Runtime);
        config_->set("enable_development", true, ConfigSource::Runtime);
        config_->set("enable_plasticity", true, ConfigSource::Runtime);
    } else if (preset == "minimal") {
        config_->set("random_seed", static_cast<int64_t>(789), ConfigSource::Runtime);
        config_->set("simulation_timestep", 0.002, ConfigSource::Runtime);
        config_->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Runtime);
        config_->set("region_count", static_cast<int64_t>(1), ConfigSource::Runtime);
        config_->set("connection_probability", 0.2f, ConfigSource::Runtime);
        config_->set("enable_checkpointing", false, ConfigSource::Runtime);
        config_->set("enable_replay", false, ConfigSource::Runtime);
    } else {
        // Default preset
        config_->set("random_seed", static_cast<int64_t>(42), ConfigSource::Runtime);
        config_->set("simulation_timestep", 0.001, ConfigSource::Runtime);
        config_->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Runtime);
        config_->set("region_count", static_cast<int64_t>(1), ConfigSource::Runtime);
        config_->set("connection_probability", 0.15f, ConfigSource::Runtime);
    }
}

void NLMCommandLine::setupLogging(const std::string& level, bool useColors, bool logToFile) {
    useColors_ = useColors;
    
    LogLevel logLevel = LogLevel::Info;
    if (level == "debug") logLevel = LogLevel::Debug;
    else if (level == "warning") logLevel = LogLevel::Warning;
    else if (level == "error") logLevel = LogLevel::Error;
    else if (level == "critical") logLevel = LogLevel::Critical;
    
    auto logger = Logger::getGlobal();
    auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel);
    consoleLogger->setUseColors(useColors_);
    
    if (logToFile && !outputFile_.empty()) {
        consoleLogger->setOutputFile(outputFile_);
    }
    
    logger->clearLoggers();
    logger->addLogger(consoleLogger);
    
    NLM_LOG_INFO("Logging initialized with level: " + level);
    if (useColors_) {
        NLM_LOG_INFO("Colored logging enabled");
    }
    if (logToFile) {
        NLM_LOG_INFO("Logging to file: " + outputFile_);
    }
}

void NLMCommandLine::printExperimentHeader(const std::string& title) const {
    if (quiet_) return;
    
    std::cout << "=== " << title << " ===" << std::endl;
    std::cout << "Configuration:" << std::endl;
    std::cout << "  Mode: " << experimentMode_ << std::endl;
    std::cout << "  Steps: " << maxSteps_ << std::endl;
    std::cout << "  Timestep: " << timestep_ << "s" << std::endl;
    std::cout << "  Neurons: " << neuronCount_ << std::endl;
    std::cout << "  Regions: " << regionCount_ << std::endl;
    if (!experimentName_.empty()) {
        std::cout << "  Experiment Name: " << experimentName_ << std::endl;
    }
    std::cout << std::endl;
}

int NLMCommandLine::runExperiment() {
    // For now, fall back to simple execution
    std::cout << "Running NLM with enhanced configuration..." << std::endl;
    return 0;
}

} // namespace nlm
