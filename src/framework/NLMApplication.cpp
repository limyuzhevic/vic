#include "NLMApplication.hpp"
#include "framework/TestFramework.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "exceptions/NLMExceptions.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>

namespace nlm {

NLMApplication::NLMApplication() : continueOnError_(false), testFramework_(std::make_shared<DefaultTestFramework>()) {
    status_.currentPhase = ApplicationPhase::Initialization;
    status_.currentTask = "Creating application components";
    status_.progress = 0.0;
}

NLMApplication::~NLMApplication() {
    // Cleanup will be handled by executePhase
}

NLMApplication::NLMApplication(NLMApplication&&) noexcept = default;
NLMApplication& NLMApplication::operator=(NLMApplication&&) noexcept = default;

ApplicationResult NLMApplication::run(int argc, char** argv) {
    try {
        status_.currentPhase = ApplicationPhase::Initialization;
        onPhaseChanged(ApplicationPhase::Initialization, ApplicationPhase::Initialization);
        
        onInitialization();
        
        status_.currentPhase = ApplicationPhase::Configuration;
        onPhaseChanged(ApplicationPhase::Initialization, ApplicationPhase::Configuration);
        
        auto result = executeConfigurationPhase(argc, argv);
        if (result != ApplicationResult::Success) {
            return result;
        }
        
        status_.currentPhase = ApplicationPhase::BrainSetup;
        onPhaseChanged(ApplicationPhase::Configuration, ApplicationPhase::BrainSetup);
        
        result = executeBrainSetupPhase();
        if (result != ApplicationResult::Success) {
            return result;
        }
        
        status_.currentPhase = ApplicationPhase::Testing;
        onPhaseChanged(ApplicationPhase::BrainSetup, ApplicationPhase::Testing);
        
        result = executeTestingPhase(brain_);
        if (result != ApplicationResult::Success) {
            return result;
        }
        
        status_.currentPhase = ApplicationPhase::Cleanup;
        onPhaseChanged(ApplicationPhase::Testing, ApplicationPhase::Cleanup);
        
        result = executeCleanupPhase();
        if (result != ApplicationResult::Success) {
            return result;
        }
        
        status_.currentPhase = ApplicationPhase::Shutdown;
        onPhaseChanged(ApplicationPhase::Cleanup, ApplicationPhase::Shutdown);
        
        result = onShutdown();
        if (result != ApplicationResult::Success) {
            return result;
        }
        
        return ApplicationResult::Success;
        
    } catch (const std::exception& e) {
        return handleError("Application run", e);
    }
}

ApplicationResult NLMApplication::run(const std::vector<std::string>& args) {
    // Convert vector to char* array for compatibility
    int argc = static_cast<int>(args.size()) + 1;
    std::vector<char*> argv_copy(argc);
    argv_copy[0] = const_cast<char*>("nlm_app");
    
    for (size_t i = 0; i < args.size(); ++i) {
        argv_copy[i + 1] = const_cast<char*>(args[i].c_str());
    }
    
    return run(argc, argv_copy.data());
}

void NLMApplication::registerTest(const std::string& name, std::function<TestResult(std::shared_ptr<Brain>)> test) {
    testFramework_->registerTest(name, test);
}

std::vector<TestResult> NLMApplication::runAllTests(std::shared_ptr<Brain> brain) {
    return testFramework_->runAllTests(brain);
}

ApplicationResult NLMApplication::executePhase(ApplicationPhase phase) {
    switch (phase) {
        case ApplicationPhase::Initialization:
            return onInitialization();
        case ApplicationPhase::Configuration:
            return onConfiguration();
        case ApplicationPhase::BrainSetup:
            return onBrainSetup();
        case ApplicationPhase::Testing:
            return onTesting(brain_);
        case ApplicationPhase::Cleanup:
            return onCleanup();
        case ApplicationPhase::Shutdown:
            return onShutdown();
        default:
            return ApplicationResult::UnknownError;
    }
}

ApplicationResult NLMApplication::executeConfigurationPhase(int argc, char** argv) {
    onProgress(0.0, "Parsing configuration");
    
    try {
        config_ = std::make_shared<Config>();
        
        // Load default configuration first
        loadDefaultConfig();
        
        // Load from file if specified
        std::string configFile = "configs/default.cfg";
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            if (arg.substr(0, 7) == "--config") {
                if (arg.find('=') != std::string::npos) {
                    configFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    configFile = argv[++i];
                }
            }
        }
        
        if (config_->loadFromFile(configFile)) {
            onProgress(0.5, "Loaded configuration from: " + configFile);
        } else {
            onProgress(0.5, "Using default configuration (file not found)");
        }
        
        // Override with command line arguments
        config_->loadFromArgs(argc, argv);
        
        // Apply phase-specific settings
        loadPhase2Config();
        
        // Validate configuration
        if (!validateConfiguration()) {
            auto errors = getValidationErrors();
            std::string errorMsg = "Configuration validation failed:\n";
            for (const auto& error : errors) {
                errorMsg += "  " + error + "\n";
            }
            throw ConfigurationException(errorMsg);
        }
        
        onProgress(1.0, "Configuration complete");
        return ApplicationResult::Success;
        
    } catch (const std::exception& e) {
        return handleError("Configuration phase", e);
    }
}

ApplicationResult NLMApplication::executeBrainSetupPhase() {
    onProgress(0.0, "Setting up NLM brain");
    
    try {
        brain_ = std::make_shared<Brain>(config_);
        
        if (!brain_->initialize()) {
            throw BrainInitializationException("Brain failed to initialize");
        }
        
        onProgress(1.0, "Brain setup complete");
        return ApplicationResult::Success;
        
    } catch (const std::exception& e) {
        return handleError("Brain setup phase", e);
    }
}

ApplicationResult NLMApplication::executeTestingPhase(std::shared_ptr<Brain> brain) {
    onProgress(0.0, "Running tests");
    
    try {
        auto testResults = runAllTests(brain);
        
        // Report test results
        size_t passed = 0, failed = 0, skipped = 0;
        for (const auto& result : testResults) {
            switch (result.getStatus()) {
                case TestResult::Status::PASSED: passed++; break;
                case TestResult::Status::FAILED: failed++; break;
                case TestResult::Status::SKIPPED: skipped++; break;
            }
        }
        
        onProgress(1.0, "Testing complete: " + std::to_string(passed) + " passed, " + 
                    std::to_string(failed) + " failed, " + std::to_string(skipped) + " skipped");
        
        // Log test results
        logStatus();
        
        return ApplicationResult::Success;
        
    } catch (const std::exception& e) {
        return handleError("Testing phase", e);
    }
}

ApplicationResult NLMApplication::executeCleanupPhase() {
    onProgress(0.0, "Cleaning up resources");
    
    try {
        if (brain_) {
            brain_->reset();
            brain_.reset();
        }
        
        config_.reset();
        logger_.reset();
        
        onProgress(1.0, "Cleanup complete");
        return ApplicationResult::Success;
        
    } catch (const std::exception& e) {
        return handleError("Cleanup phase", e);
    }
}

ApplicationResult NLMApplication::handleError(const std::string& context, const std::exception& e) {
    onError(context, e);
    
    if (!continueOnError_) {
        logStatus();
        return ApplicationResult::ExecutionError;
    }
    
    return ApplicationResult::Success; // Continue despite error
}

ApplicationResult NLMApplication::loadDefaultConfig() {
    config_->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config_->set("simulation_timestep", 0.001, ConfigSource::Default);
    config_->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config_->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config_->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config_->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config_->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config_->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    return ApplicationResult::Success;
}

ApplicationResult NLMApplication::loadPhase2Config() {
    // Additional Phase 2 specific settings
    config_->set("plasticity_enabled", true, ConfigSource::Runtime);
    config_->set("structural_plasticity", true, ConfigSource::Runtime);
    
    return ApplicationResult::Success;
}

ApplicationResult NLMApplication::loadExperimentConfig() {
    // Experiment-specific settings
    config_->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Runtime);
    config_->set("region_count", static_cast<int64_t>(2), ConfigSource::Runtime);
    config_->set("max_simulation_steps", 50000, ConfigSource::Runtime);
    
    return ApplicationResult::Success;
}

ApplicationStatus::Phase NLMApplication::getPhaseFromEnum(ApplicationPhase phase) {
    switch (phase) {
        case ApplicationPhase::Initialization: return ApplicationStatus::Phase::Initialization;
        case ApplicationPhase::Configuration: return ApplicationStatus::Phase::Configuration;
        case ApplicationPhase::BrainSetup: return ApplicationStatus::Phase::BrainSetup;
        case ApplicationPhase::Testing: return ApplicationStatus::Phase::Testing;
        case ApplicationPhase::Cleanup: return ApplicationStatus::Phase::Cleanup;
        case ApplicationPhase::Shutdown: return ApplicationStatus::Phase::Shutdown;
        default: return ApplicationStatus::Phase::Unknown;
    }
}

std::string NLMApplication::getStatusString() const {
    std::ostringstream oss;
    oss << "Phase: " << static_cast<int>(status_.currentPhase) << " (" << status_.currentTask << ")\n";
    oss << "Progress: " << (status_.progress * 100) << "%\n";
    oss << "Configuration keys: " << config_->getKeys().size() << "\n";
    oss << "Brain initialized: " << (brain_ ? "Yes" : "No") << "\n";
    oss << "Tests registered: " << testFramework_->getTestCount() << "\n";
    return oss.str();
}

bool NLMApplication::validateConfiguration() const {
    return true; // Placeholder - would use actual validation
}

std::vector<std::string> NLMApplication::getValidationErrors() const {
    return std::vector<std::string>(); // Placeholder - would return actual validation errors
}

void NLMApplication::logStatus() const {
    if (logger_) {
        logger_->info("=== NLM Application Status ===");
        logger_->info(getStatusString());
        if (brain_) {
            brain_->logStatus();
        }
    }
}

ApplicationResult NLMApplication::onInitialization() {
    onProgress(0.5, "Initializing application");
    onProgress(1.0, "Initialization complete");
    return ApplicationResult::Success;
}

ApplicationResult NLMApplication::onConfiguration() {
    onProgress(1.0, "Configuration complete");
    return ApplicationResult::Success;
}

ApplicationResult NLMApplication::onBrainSetup() {
    onProgress(1.0, "Brain setup complete");
    return ApplicationResult::Success;
}

ApplicationResult NLMApplication::onTesting(std::shared_ptr<Brain> brain) {
    onProgress(1.0, "Testing complete");
    return ApplicationResult::Success;
}

ApplicationResult NLMApplication::onCleanup() {
    onProgress(1.0, "Cleanup complete");
    return ApplicationResult::Success;
}

ApplicationResult NLMApplication::onShutdown() {
    onProgress(1.0, "Shutdown complete");
    return ApplicationResult::Success;
}

void NLMApplication::onPhaseChanged(ApplicationPhase oldPhase, ApplicationPhase newPhase) {
    // Default implementation - can be overridden
}

void NLMApplication::onError(const std::string& context, const std::exception& e) {
    std::cerr << "Error in " << context << ": " << e.what() << std::endl;
}

void NLMApplication::onProgress(double progress, const std::string& task) {
    status_.currentTask = task;
    status_.progress = progress;
    // For now, just output to console - could use logger in future
}

std::shared_ptr<Config> NLMApplication::createDefaultConfig() {
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    return config;
}

std::shared_ptr<Config> NLMApplication::createPhase2Config() {
    auto config = createDefaultConfig();
    config->set("plasticity_enabled", true, ConfigSource::Runtime);
    return config;
}

std::shared_ptr<Config> NLMApplication::createExperimentConfig() {
    auto config = createDefaultConfig();
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Runtime);
    return config;
}

NLMApplication::CommandLineOptions NLMApplication::parseCommandLine(int argc, char** argv) {
    CommandLineOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            options.help = true;
        } else if (arg == "--version") {
            options.version = true;
        } else if (arg == "--verbose") {
            options.verbose = true;
        } else if (arg == "--quiet") {
            options.quiet = true;
        } else if (arg.find("--config=") == 0) {
            options.configFile = arg.substr(8);
        } else if (arg.find("--config") == 0 && i + 1 < argc) {
            options.configFile = argv[++i];
        } else {
            options.customArgs.push_back(arg);
        }
    }
    
    return options;
}

} // namespace nlm
