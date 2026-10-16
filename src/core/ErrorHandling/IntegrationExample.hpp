// **Error Handling Integration Example**
//
// Comprehensive example demonstrating how to integrate the NLM error handling
// system into existing NLM components and applications.
// This example shows best practices for error handling, recovery, and configuration.

#include "core/ErrorHandling/ErrorHandling.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include <iostream>
#include <memory>
#include <fstream>
#include <vector>

// Forward declarations for demonstration
class SimulationSystem;
class DevelopmentSystem;
class NeuromodulationSystem;

// **Error Handling Strategy Class**
class ErrorHandlingStrategy {
public:
    enum class Mode {
        Strict,           // Fail fast on errors
        Resilient,        // Continue with degraded functionality
        Autonomous,       // Self-repairing with automatic recovery
        Hybrid           // Combine strategies based on error type
    };
    
    struct Configuration {
        Mode mode;
        nlm::ErrorReportLevel reportLevel;
        nlm::ExceptionStrategy exceptionStrategy;
        bool enableRecovery;
        bool enableGracefulDegradation;
        bool enableCheckpoints;
        std::chrono::milliseconds checkpointInterval;
        int maxRetryAttempts;
        std::chrono::milliseconds retryDelay;
        std::vector<nlm::ErrorCode> criticalErrors;
        std::vector<nlm::ErrorCode> recoverableErrors;
        std::vector<nlm::ErrorCode> degradableErrors;
    };
    
    explicit ErrorHandlingStrategy(const Configuration& config)
        : config(config) {
        initialize();
    }
    
    void initialize() {
        // Configure global error handling
        nlm::ErrorHandler::initialize(config.reportLevel, config.exceptionStrategy);
        
        // Add error reporters
        auto& handler = nlm::ErrorHandler::getInstance();
        
        // Add console reporter with colors
        handler.addReporter(std::make_shared<nlm::ConsoleErrorReporter>(
            config.reportLevel, true));
        
        // Add error recovery components
        if (config.enableRecovery) {
            setupRecovery();
        }
        
        if (config.enableGracefulDegradation) {
            setupGracefulDegradation();
        }
        
        if (config.enableCheckpoints) {
            setupCheckpointing();
        }
    }
    
    void setupRecovery() {
        auto& recovery = nlm::getGlobalErrorRecovery();
        
        // Register recovery actions
        recovery.getRetryManager().getRetryConfig().maxAttempts = config.maxRetryAttempts;
        recovery.getRetryManager().getRetryConfig().initialDelay = config.retryDelay;
        
        for (const auto& errorCode : config.recoverableErrors) {
            nlm::ErrorHandler::getInstance().registerRecoveryAction(
                std::to_string(static_cast<int>(errorCode)),
                [this, errorCode]() { return handleRecovery(errorCode); }
            );
        }
    }
    
    void setupGracefulDegradation() {
        auto& recovery = nlm::getGlobalErrorRecovery();
        
        // Register degraded configurations
        nlm::ErrorHandler::getInstance().registerCleanupAction(
            "system_init",
            []() {
                // Cleanup system resources
                nlm::logWarning("System cleanup completed");
            }
        );
    }
    
    void setupCheckpointing() {
        auto& recovery = nlm::getGlobalErrorRecovery();
        
        // Configure checkpointing
        auto& checkpoint = recovery.getCheckpointManager();
        // TODO: Set checkpoint interval
    }
    
    bool handleRecovery(nlm::ErrorCode errorCode) {
        switch (errorCode) {
            case nlm::ErrorCode::MemoryAllocationFailure:
                return handleMemoryAllocationRecovery();
            case nlm::ErrorCode::SimulationRuntimeError:
                return handleSimulationRecovery();
            case nlm::ErrorCode::NeuronStateError:
                return handleNeuronRecovery();
            case nlm::ErrorCode::SynapseConnectionError:
                return handleSynapseRecovery();
            default:
                return false;
        }
    }
    
    bool handleMemoryAllocationRecovery() {
        nlm::logWarning("Attempting memory allocation recovery");
        // Try to free up memory
        std::vector<void*> allocations;
        // Allocate and then free memory
        for (int i = 0; i < 100; ++i) {
            void* ptr = malloc(1024 * 1024); // 1MB
            if (ptr) allocations.push_back(ptr);
        }
        
        for (auto ptr : allocations) {
            free(ptr);
        }
        
        return true;
    }
    
    bool handleSimulationRecovery() {
        nlm::logWarning("Attempting simulation recovery");
        // Reset simulation state
        // TODO: Implement simulation state reset
        return true;
    }
    
    bool handleNeuronRecovery() {
        nlm::logWarning("Attempting neuron recovery");
        // Reset neuron states
        // TODO: Implement neuron state reset
        return true;
    }
    
    bool handleSynapseRecovery() {
        nlm::logWarning("Attempting synapse recovery");
        // Reset synapse states
        // TODO: Implement synapse state reset
        return true;
    }
    
    nlm::ErrorHandling::GracefulDegradation::DegradedValue<std::shared_ptr<nlm::Config>>
    createDegradedConfig(const std::string& reason) {
        return nlm::ErrorHandling::GracefulDegradation::degradedConfig(reason);
    }
    
    nlm::ErrorHandling::GracefulDegradation::DegradedValue<std::unique_ptr<nlm::Brain>>
    createDegradedBrain(const std::string& reason) {
        return nlm::ErrorHandling::GracefulDegradation::degradedBrain(reason);
    }
    
    // **Safe Operation Wrappers**
    
    static bool safeConfigLoad(std::shared_ptr<nlm::Config>& config,
                              const std::string& filepath,
                              bool createIfMissing = false) {
        try {
            config = std::make_shared<nlm::Config>();
            if (!config->loadFromFile(filepath) && createIfMissing) {
                // Create default configuration
                config->set("neuron_count", static_cast<int64_t>(1000));
                config->set("simulation_timestep", 0.001);
                config->set("region_count", static_cast<int64_t>(1));
                config->set("connection_probability", 0.1);
                config->set("learning_rate", 0.01);
            } else if (!config->loadFromFile(filepath)) {
                nlm::logError("Failed to load configuration from " + filepath);
                return false;
            }
            
            // Validate configuration
            auto result = config->validate();
            if (!result.isValid) {
                nlm::logWarning("Configuration validation issues: " + result.toString());
                // Continue with degraded config for now
            }
            
            return true;
        } catch (const nlm::NLMException& e) {
            nlm::logError(e.getMessage(), "ConfigLoad");
            config = nullptr;
            return false;
        }
    }
    
    static bool safeBrainStep(nlm::Brain& brain, nlm::SimulationStep step,
                             const std::string& operation = "simulation_step") {
        try {
            brain.step(step);
            return true;
        } catch (const nlm::NLMException& e) {
            nlm::logError(e.getMessage(), operation);
            return false;
        }
    }
    
    static bool safeNeuronOperation(nlm::Neuron& neuron,
                                    nlm::Timestamp currentTime,
                                    nlm::TimestepDuration dt,
                                    const std::string& operation = "neuron_step") {
        try {
            bool result = neuron.stepLIF(currentTime, dt);
            return result;
        } catch (const nlm::NLMException& e) {
            nlm::logError(e.getMessage(), operation);
            return false;
        }
    }
    
    static bool safeSynapseOperation(nlm::Synapse& synapse,
                                     const std::string& operation = "update_weight") {
        try {
            if (operation == "update_weight") {
                // TODO: Implement synapse weight update
                return true;
            } else if (operation == "reset") {
                synapse = nlm::Synapse(synapse.getId(), synapse.getSourceNeuron(),
                                     synapse.getDestinationNeuron());
                return true;
            }
            return false;
        } catch (const nlm::NLMException& e) {
            nlm::logError(e.getMessage(), operation);
            return false;
        }
    }
    
    // **System Management**
    
    class SystemManager {
    private:
        std::shared_ptr<SimulationSystem> simulationSystem;
        std::shared_ptr<DevelopmentSystem> developmentSystem;
        std::shared_ptr<NeuromodulationSystem> neuromodulationSystem;
        bool initialized = false;
        
    public:
        SystemManager() = default;
        
        bool initialize(std::shared_ptr<nlm::Config> config) {
            try {
                nlm::ErrorHandling::ErrorContext context("system_init");
                context.addStep("Initializing simulation system");
                
                // Initialize simulation system
                simulationSystem = std::make_shared<SimulationSystem>(config);
                
                context.addStep("Initializing development system");
                developmentSystem = std::make_shared<DevelopmentSystem>(config);
                
                context.addStep("Initializing neuromodulation system");
                neuromodulationSystem = std::make_shared<NeuromodulationSystem>(config);
                
                initialized = true;
                nlm::logInfo("System initialized successfully");
                return true;
                
            } catch (const nlm::NLMException& e) {
                nlm::logError("System initialization failed: " + e.getMessage(), "system_init");
                cleanup();
                return false;
            }
        }
        
        void cleanup() {
            nlm::ErrorHandling::ErrorContext context("system_cleanup");
            context.addStep("Cleaning up neuromodulation system");
            neuromodulationSystem.reset();
            
            context.addStep("Cleaning up development system");
            developmentSystem.reset();
            
            context.addStep("Cleaning up simulation system");
            simulationSystem.reset();
            
            initialized = false;
            nlm::logInfo("System cleanup completed");
        }
        
        bool runStep(nlm::SimulationStep step) {
            if (!initialized) {
                nlm::logError("System not initialized");
                return false;
            }
            
            try {
                nlm::ErrorHandling::ErrorContext context("system_step");
                context.addStep(std::string("Running simulation step ") + std::to_string(step));
                
                // Run systems in order
                bool success = true;
                
                context.addStep("Running neuromodulation");
                success &= neuromodulationSystem->update(step);
                
                context.addStep("Running development");
                success &= developmentSystem->update(step);
                
                context.addStep("Running simulation");
                success &= simulationSystem->update(step);
                
                if (!success) {
                    nlm::logWarning("System step completed with warnings");
                }
                
                return success;
                
            } catch (const nlm::NLMException& e) {
                nlm::logError("System step failed: " + e.getMessage(), "system_step");
                
                // Attempt recovery
                if (attemptRecovery()) {
                    nlm::logWarning("System recovery attempted");
                    return true; // Recovery successful
                }
                
                return false;
            }
        }
        
        bool attemptRecovery() {
            // Try to recover from errors
            if (developmentSystem) {
                nlm::logWarning("Recovering development system");
                // TODO: Implement development system recovery
            }
            
            if (neuromodulationSystem) {
                nlm::logWarning("Recovering neuromodulation system");
                // TODO: Implement neuromodulation system recovery
            }
            
            if (simulationSystem) {
                nlm::logWarning("Recovering simulation system");
                // TODO: Implement simulation system recovery
            }
            
            return true; // Assume recovery works for now
        }
        
        bool saveState(const std::string& filepath) {
            try {
                nlm::ErrorHandling::ErrorContext context("save_state");
                context.addStep(std::string("Saving state to ") + filepath);
                
                std::ofstream file(filepath);
                if (!file) {
                    throw nlm::ResourceAllocationException("Failed to open state file for writing");
                }
                
                // TODO: Serialize system state
                // For now, just write a simple marker
                file << "NLM System State v1.0\n";
                file << "Simulation System: " << (simulationSystem ? "Active" : "Inactive") << "\n";
                file << "Development System: " << (developmentSystem ? "Active" : "Inactive") << "\n";
                file << "Neuromodulation System: " << (neuromodulationSystem ? "Active" : "Inactive") << "\n";
                
                nlm::logInfo("State saved successfully to " + filepath);
                return true;
                
            } catch (const nlm::NLMException& e) {
                nlm::logError("Failed to save state: " + e.getMessage(), "save_state");
                return false;
            }
        }
        
        bool loadState(const std::string& filepath) {
            try {
                nlm::ErrorHandling::ErrorContext context("load_state");
                context.addStep(std::string("Loading state from ") + filepath);
                
                std::ifstream file(filepath);
                if (!file) {
                    throw nlm::ConfigFileNotFoundException(filepath);
                }
                
                std::string line;
                std::getline(file, line);
                if (line != "NLM System State v1.0") {
                    throw nlm::MalformedConfigException("Invalid state file format");
                }
                
                // TODO: Deserialize system state
                // For now, just acknowledge the file exists
                
                nlm::logInfo("State loaded successfully from " + filepath);
                return true;
                
            } catch (const nlm::NLMException& e) {
                nlm::logError("Failed to load state: " + e.getMessage(), "load_state");
                return false;
            }
        }
    };
};

// **Application Example**

class NLMApplication {
private:
    ErrorHandlingStrategy::Configuration errorConfig;
    std::unique_ptr<ErrorHandlingStrategy> errorStrategy;
    std::unique_ptr<nlm::Config> config;
    std::unique_ptr<ErrorHandlingStrategy::SystemManager> systemManager;
    bool running = false;
    
public:
    NLMApplication(const std::string& configFile = "config.cfg") {
        // Configure error handling
        errorConfig = {
            .mode = ErrorHandlingStrategy::Mode::Resilient,
            .reportLevel = nlm::ErrorReportLevel::Warning,
            .exceptionStrategy = nlm::ExceptionStrategy::Catch,
            .enableRecovery = true,
            .enableGracefulDegradation = true,
            .enableCheckpoints = true,
            .checkpointInterval = std::chrono::milliseconds(30000), // 30 seconds
            .maxRetryAttempts = 3,
            .retryDelay = std::chrono::milliseconds(100),
            .criticalErrors = {nlm::ErrorCode::MemoryError, nlm::ErrorCode::SystemError},
            .recoverableErrors = {nlm::ErrorCode::MemoryAllocationFailure,
                                 nlm::ErrorCode::SimulationRuntimeError},
            .degradableErrors = {nlm::ErrorCode::NeuronStateError,
                                nlm::ErrorCode::SynapseConnectionError}
        };
        
        errorStrategy = std::make_unique<ErrorHandlingStrategy>(errorConfig);
        
        // Load configuration
        bool configSuccess = ErrorHandlingStrategy::safeConfigLoad(config, configFile, true);
        if (configSuccess) {
            nlm::logInfo("Configuration loaded successfully");
        } else {
            nlm::logError("Failed to load configuration, using defaults");
            // Create default config
            config = std::make_unique<nlm::Config>();
            config->set("neuron_count", static_cast<int64_t>(100));
            config->set("simulation_timestep", 0.001);
            config->set("region_count", static_cast<int64_t>(1));
            config->set("connection_probability", 0.1);
        }
    }
    
    bool initialize() {
        try {
            nlm::ErrorHandling::ErrorContext context("app_init");
            context.addStep("Initializing system manager");
            
            systemManager = std::make_unique<ErrorHandlingStrategy::SystemManager>();
            context.addStep("System manager initialized");
            
            // Initialize system
            context.addStep("Initializing simulation system");
            if (!systemManager->initialize(config)) {
                nlm::logError("Failed to initialize simulation system");
                return false;
            }
            
            // Save initial checkpoint
            auto& recovery = nlm::getGlobalErrorRecovery();
            std::ostringstream oss;
            oss << "{" << nlm::ConfigEntry("neuron_count", config->getOr<int64_t>("neuron_count", 1000))
                << "}";
            
            recovery.getCheckpointManager().save("initial_state", oss.str());
            
            nlm::logInfo("Application initialized successfully");
            running = true;
            return true;
            
        } catch (const nlm::NLMException& e) {
            nlm::logError("Application initialization failed: " + e.getMessage(), "app_init");
            return false;
        }
    }
    
    bool runStep(nlm::SimulationStep step) {
        if (!running) {
            nlm::logError("Application not running");
            return false;
        }
        
        try {
            nlm::ErrorHandling::ErrorContext context("app_step");
            context.addStep(std::string("Running application step ") + std::to_string(step));
            
            // Run system step
            context.addStep("Running system update");
            if (!systemManager->runStep(step)) {
                nlm::logWarning("System update completed with warnings");
            }
            
            // Save periodic checkpoint
            if (step % 1000 == 0) {
                auto& recovery = nlm::getGlobalErrorRecovery();
                std::ostringstream oss;
                oss << "{" << step << "}";
                recovery.getCheckpointManager().save("periodic_step_" + std::to_string(step), oss.str());
            }
            
            nlm::logInfo("Application step " + std::to_string(step) + " completed");
            return true;
            
        } catch (const nlm::NLMException& e) {
            nlm::logError("Application step failed: " + e.getMessage(), "app_step");
            
            // Attempt recovery
            if (systemManager->attemptRecovery()) {
                nlm::logWarning("Application recovery attempted");
                return true;
            }
            
            return false;
        }
    }
    
    void shutdown() {
        try {
            nlm::ErrorHandling::ErrorContext context("app_shutdown");
            context.addStep("Saving final state");
            
            saveState();
            
            context.addStep("Cleaning up system manager");
            systemManager.reset();
            
            context.addStep("Shutting down error handling");
            nlm::ErrorHandler::cleanup();
            
            nlm::logInfo("Application shutdown completed");
            running = false;
            
        } catch (const nlm::NLMException& e) {
            nlm::logError("Application shutdown failed: " + e.getMessage(), "app_shutdown");
        }
    }
    
    bool saveState() {
        return systemManager->saveState("nlm_state.json");
    }
    
    bool loadState() {
        return systemManager->loadState("nlm_state.json");
    }
    
    bool isRunning() const { return running; }
};

// **Main Function Example**

int main(int argc, char* argv[]) {
    std::string configFile = "config.cfg";
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--config" && i + 1 < argc) {
            configFile = argv[i + 1];
            ++i;
        } else if (std::string(argv[i]) == "--help") {
            std::cout << "NLM Application" << std::endl;
            std::cout << "  --config <file>  Specify configuration file" << std::endl;
            std::cout << "  --checkpoint     Enable checkpointing" << std::endl;
            return 0;
        }
    }
    
    // Create and run application
    NLMApplication app(configFile);
    
    // Initialize
    if (!app.initialize()) {
        nlm::logError("Failed to initialize application");
        return 1;
    }
    
    // Check for saved state
    if (app.loadState()) {
        nlm::logInfo("Loaded saved state");
    }
    
    // Run simulation loop
    const nlm::SimulationStep totalSteps = 10000;
    nlm::SimulationStep currentStep = 0;
    
    nlm::logInfo(std::string("Starting simulation with ") + totalSteps + " steps");
    
    while (app.isRunning() && currentStep < totalSteps) {
        // Run step
        bool success = app.runStep(currentStep);
        
        if (!success) {
            nlm::logError("Step failed, attempting recovery");
            // Continue anyway for demonstration
        }
        
        ++currentStep;
        
        // Periodic checkpoint
        if (currentStep % 1000 == 0) {
            nlm::logInfo("Checkpoint at step " + std::to_string(currentStep));
        }
    }
    
    // Save final state
    app.saveState();
    
    // Shutdown
    app.shutdown();
    
    nlm::logInfo("Simulation completed successfully");
    return 0;
}
