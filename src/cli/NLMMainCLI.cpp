#include "NLMMainCLI.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <chrono>
#include <ctime>

namespace nlm {
namespace cli {

NLMMainCLI::NLMMainCLI() : configFile_("") {
    // Initialize components
    setupLogger();
    setupDefaultConfiguration();
    setupCommandHandlers();
    
    LOG_INFO("NLM CLI initialized successfully");
}

void NLMMainCLI::setupLogger() {
    logger_ = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger_->addLogger(consoleLogger);
    Logger::setGlobal(logger_);
}

void NLMMainCLI::setupDefaultConfiguration() {
    config_ = std::make_shared<Config>();
    
    // Set default values
    config_->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config_->set("simulation_timestep", 0.001, ConfigSource::Default);
    config_->set("neuron_count", static_cast<size_t>(1000), ConfigSource::Default);
    config_->set("region_count", static_cast<size_t>(1), ConfigSource::Default);
    config_->set("connection_probability", 0.1f, ConfigSource::Default);
    config_->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config_->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config_->set("stdp_tau", 20.0f, ConfigSource::Default);
    config_->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config_->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Set paths
    config_->set("checkpoint_dir", "./checkpoints", ConfigSource::Default);
    config_->set("replay_interval", static_cast<size_t>(100), ConfigSource::Default);
    config_->set("consolidation_interval", static_cast<size_t>(1000), ConfigSource::Default);
}

void NLMMainCLI::setupCommandHandlers() {
    // Create command instances
    runCommand_ = std::make_unique<RunCommand>();
    configCommand_ = std::make_unique<ConfigCommand>();
    experimentCommand_ = std::make_unique<ExperimentCommand>();
    utilsCommand_ = std::make_unique<UtilsCommand>();
    
    // Register commands
    registerCommand(std::move(runCommand_));
    registerSubcommand(std::move(configCommand_));
    registerSubcommand(std::move(experimentCommand_));
    registerSubcommand(std::move(utilsCommand_));
}

Result NLMMainCLI::initialize() const {
    LOG_INFO("Initializing NLM CLI system...");
    
    try {
        // Load configuration if specified
        if (!configFile_.empty()) {
            if (!loadConfiguration(configFile_)) {
                return Result::failure(1, "Failed to load configuration file: " + configFile_);
            }
        }
        
        LOG_INFO("NLM CLI system initialized successfully");
        return Result::success("NLM CLI system ready");
        
    } catch (const std::exception& e) {
        handleFatalError(std::string("Initialization failed: ") + e.what());
        return Result::failure(1, "Initialization failed: " + std::string(e.what()));
    }
}

Result NLMMainCLI::cleanup() const {
    LOG_INFO("Cleaning up NLM CLI system...");
    
    // Save configuration if needed
    if (!configFile_.empty()) {
        if (!saveConfiguration(configFile_)) {
            LOG_WARNING("Failed to save configuration file: " + configFile_);
        }
    }
    
    LOG_INFO("NLM CLI system cleaned up successfully");
    return Result::success("Cleanup completed");
}

bool NLMMainCLI::loadConfiguration(const std::string& configFile) {
    if (configFile.empty()) {
        return true; // No file to load
    }
    
    std::ifstream file(configFile);
    if (!file.good()) {
        LOG_WARNING("Configuration file not found: " + configFile);
        return false; // File not found is not a fatal error
    }
    
    try {
        // Load configuration from file
        // In a real implementation, this would parse the configuration
        configFile_ = configFile;
        LOG_INFO("Configuration loaded from: " + configFile_);
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR(std::string("Failed to load configuration: ") + e.what());
        return false;
    }
}

bool NLMMainCLI::saveConfiguration(const std::string& configFile) const {
    if (configFile.empty()) {
        return true; // No file to save
    }
    
    try {
        // Save configuration to file
        std::ofstream file(configFile);
        if (!file.good()) {
            LOG_ERROR("Failed to create configuration file: " + configFile);
            return false;
        }
        
        // In a real implementation, this would serialize the configuration
        LOG_INFO("Configuration saved to: " + configFile);
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR(std::string("Failed to save configuration: ") + e.what());
        return false;
    }
}

std::string NLMMainCLI::getDefaultConfigPath() {
    return "configs/default.cfg";
}

std::string NLMMainCLI::getDefaultLogPath() {
    return "logs/nlm.log";
}

std::string NLMMainCLI::getDefaultExperimentPath() {
    return "experiments";
}

void NLMMainCLI::handleFatalError(const std::string& message) {
    std::cerr << "FATAL ERROR: " << message << std::endl;
    std::cerr << "The NLM CLI system encountered a fatal error and cannot continue." << std::endl;
}

void NLMMainCLI::handleError(const CLIError& error) {
    std::cerr << "ERROR: " << error.what() << std::endl;
}

} // namespace cli
} // namespace nlm