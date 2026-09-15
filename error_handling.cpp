//!
 * Error Handling and User Experience Improvements
 * ================================================
 *
 * This file demonstrates improved error handling and user experience features
 * for the NLM framework, including better error messages, input validation,
 * progress reporting, and user-friendly interfaces.
 *
 */

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <cassert>
#include <algorithm>
#include <string>
#include <vector>

using namespace nlm;

/**
 * @brief Exception class for NLM-specific errors
 */
class NLMRuntimeError : public std::runtime_error {
public:
    explicit NLMRuntimeError(const std::string& message) 
        : std::runtime_error("NLM Runtime Error: " + message) {}
    
    explicit NLMRuntimeError(const std::string& context, const std::string& message)
        : std::runtime_error("NLM Runtime Error in " + context + ": " + message) {}
};

/**
 * @brief Improved user-friendly error messages with context
 */
class ErrorReporter {
private:
    std::string lastErrorContext;
    bool verboseMode;
    
public:
    ErrorReporter() : verboseMode(false) {}
    
    void setVerbose(bool verbose) { verboseMode = verbose; }
    
    void logError(const std::string& context, const std::string& message) {
        std::cerr << std::endl;
        std::cerr << "╔══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cerr << "║                 ❌ ERROR OCCURRED                 ║" << std::endl;
        std::cerr << "╠══════════════════════════════════════════════════════════════╣" << std::endl;
        std::cerr << "║ Context: " << std::left << std::setw(42) << context << "║" << std::endl;
        std::cerr << "║ Message: " << std::left << std::setw(42) << message << "║" << std::endl;
        std::cerr << "╚══════════════════════════════════════════════════════════════╝" << std::endl;
        
        lastErrorContext = context;
        
        if (verboseMode) {
            printDebugInformation(context, message);
        }
    }
    
    void logWarning(const std::string& context, const std::string& message) {
        std::cerr << std::endl;
        std::cerr << "⚠️  WARNING: [" << context << "] " << message << std::endl;
    }
    
    void logInfo(const std::string& context, const std::string& message) {
        if (verboseMode) {
            std::cout << "ℹ️  INFO: [" << context << "] " << message << std::endl;
        }
    }
    
    void printDebugInformation(const std::string& context, const std::string& message) {
        std::cout << std::endl;
        std::cout << "🔍 Debug Information:" << std::endl;
        std::cout << "   - System time: " << getCurrentTimestamp() << std::endl;
        std::cout << "   - Available memory: " << getAvailableMemory() << std::endl;
        std::cout << "   - Process ID: " << getProcessId() << std::endl;
        
        // Suggest possible causes and solutions
        std::cout << std::endl;
        std::cout << "💡 Suggested Solutions:" << std::endl;
        
        if (message.find("initialization") != std::string::npos) {
            std::cout << "   - Check configuration parameters" << std::endl;
            std::cout << "   - Verify required dependencies are installed" << std::endl;
            std::cout << "   - Try using default configuration" << std::endl;
        } else if (message.find("neuron") != std::string::npos) {
            std::cout << "   - Reduce neuron count if system is under memory pressure" << std::endl;
            std::cout << "   - Check system resources" << std::endl;
            std::cout << "   - Verify configuration settings" << std::endl;
        } else if (message.find("file") != std::string::npos) {
            std::cout << "   - Verify file path and permissions" << std::endl;
            std::cout << "   - Check disk space" << std::endl;
            std::cout << "   - Ensure file format is supported" << std::endl;
        } else {
            std::cout << "   - Check system logs for detailed error information" << std::endl;
            std::cout << "   - Verify all required components are initialized" << std::endl;
            std::cout << "   - Review configuration and parameters" << std::endl;
        }
        
        std::cout << "   - For additional help, refer to the documentation" << std::endl;
        std::cout << std::endl;
    }
    
private:
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        ss << "." << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }
    
    long getProcessId() {
        return static_cast<long>(getpid());
    }
    
    std::string getAvailableMemory() {
        // Simplified memory check - in production, use platform-specific calls
        return "N/A (simplified in demo)";
    }
};

/**
 * @brief Input validation and parameter checking utilities
 */
class InputValidator {
public:
    static bool validateConfig(const Config& config) {
        std::vector<std::string> errors;
        
        // Check required parameters
        if (!config.has("neuron_count")) {
            errors.push_back("Missing required parameter: 'neuron_count'");
        } else {
            auto neuronCount = config.getOr<int64_t>("neuron_count", 0);
            if (neuronCount <= 0) {
                errors.push_back("Invalid neuron_count: must be positive (> 0)");
            } else if (neuronCount > 100000) {
                errors.push_back("Neuron count too high: maximum supported is 100,000");
            }
        }
        
        if (!config.has("region_count")) {
            errors.push_back("Missing required parameter: 'region_count'");
        } else {
            auto regionCount = config->getOr<int64_t>("region_count", 0);
            if (regionCount <= 0) {
                errors.push_back("Invalid region_count: must be positive (> 0)");
            } else if (regionCount > 100) {
                errors.push_back("Region count too high: maximum supported is 100");
            }
        }
        
        if (config.has("connection_probability")) {
            auto prob = config->getOr<float>("connection_probability", 0.0f);
            if (prob < 0.0f || prob > 1.0f) {
                errors.push_back("Invalid connection_probability: must be between 0.0 and 1.0");
            }
        }
        
        if (config.has("plasticity.stdp.learning_rate")) {
            auto rate = config->getOr<float>("plasticity.stdp.learning_rate", 0.0f);
            if (rate < 0.0f || rate > 1.0f) {
                errors.push_back("Invalid STDP learning_rate: must be between 0.0 and 1.0");
            }
        }
        
        if (config.has("simulation_timestep")) {
            auto timestep = config->getOr<double>("simulation_timestep", 0.0);
            if (timestep <= 0.0) {
                errors.push_back("Invalid simulation_timestep: must be positive (> 0)");
            } else if (timestep > 0.1) {
                errors.push_back("Warning: simulation_timestep is large (> 0.1s). May affect simulation accuracy.");
            }
        }
        
        // Report errors if any
        if (!errors.empty()) {
            std::cerr << "⚠️  Configuration Validation Errors:" << std::endl;
            for (size_t i = 0; i < errors.size(); ++i) {
                std::cerr << "   " << (i + 1) << ". " << errors[i] << std::endl;
            }
            return false;
        }
        
        return true;
    }
    
    static std::vector<std::string> getSuggestions(const std::string& context) {
        std::vector<std::string> suggestions;
        
        if (context.find("neuron") != std::string::npos) {
            suggestions.push_back("Try reducing neuron count to improve performance");
            suggestions.push_back("Use connection_probability to control sparsity");
            suggestions.push_back("Consider using regional specialization");
        } else if (context.find("memory") != std::string::npos) {
            suggestions.push_back("Optimize working memory capacity settings");
            suggestions.push_back("Adjust episodic memory parameters for your use case");
            suggestions.push_back("Consider memory consolidation settings");
        } else if (context.find("plasticity") != std::string::npos) {
            suggestions.push_back("Tune learning rates for your specific task");
            suggestions.push_back("Consider adding structural plasticity for memory");
            suggestions.push_back("Balance between stability and learning rate");
        }
        
        return suggestions;
    }
};

/**
 * @brief Progress reporting and user feedback system
 */
class ProgressReporter {
private:
    bool enabled;
    std::string lastMessage;
    double lastProgress;
    
public:
    ProgressReporter() : enabled(true), lastProgress(-1.0) {}
    
    void setEnabled(bool enabled) { this->enabled = enabled; }
    
    void reportProgress(const std::string& context, double progress, const std::string& message = "") {
        if (!enabled) return;
        
        if (progress < 0.0 || progress > 1.0) {
            std::cerr << "⚠️  Invalid progress value: " << progress << std::endl;
            return;
        }
        
        // Only report if progress changed or message is provided
        if (fabs(progress - lastProgress) > 0.01 || !message.empty()) {
            std::cout << "🔄 " << context << ": " << std::fixed << std::setprecision(1) << (progress * 100) << "%";
            
            if (!message.empty()) {
                std::cout << " - " << message;
            }
            
            std::cout << std::endl;
            
            lastProgress = progress;
            lastMessage = message;
        }
    }
    
    void reportCompletion(const std::string& context, const std::string& summary = "") {
        if (!enabled) return;
        
        std::cout << "✅ " << context << " completed successfully";
        if (!summary.empty()) {
            std::cout << " - " << summary;
        }
        std::cout << std::endl;
    }
    
    void reportError(const std::string& context, const std::string& error) {
        std::cerr << "❌ " << context << " failed: " << error << std::endl;
    }
};

/**
 * @brief User-friendly parameter interface with intelligent defaults
 */
class UserFriendlyConfig {
private:
    std::shared_ptr<Config> config;
    ErrorReporter errorReporter;
    ProgressReporter progressReporter;
    
public:
    UserFriendlyConfig() : config(std::make_shared<Config>()) {
        // Set up sensible defaults for common use cases
        setupDefaults();
    }
    
    UserFriendlyConfig(const std::string& preset) : UserFriendlyConfig() {
        applyPreset(preset);
    }
    
    void setupDefaults() {
        // Application-level defaults
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("log_level", "INFO", ConfigSource::Default);
        config->set("log_to_file", false, ConfigSource::Default);
        
        // Brain structure defaults
        config->set("neuron_count", 1000, ConfigSource::Default);
        config->set("region_count", 1, ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        
        // Plasticity defaults
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.stdp.learning_rate", 0.001f, ConfigSource::Default);
        config->set("plasticity.stdp.tau_plus", 20.0f, ConfigSource::Default);
        config->set("plasticity.stdp.tau_minus", 20.0f, ConfigSource::Default);
        
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("plasticity.hebbian.learning_rate", 0.0005f, ConfigSource::Default);
        
        config->set("plasticity.structural.enable", false, ConfigSource::Default);  // Default to false for performance
        
        // Neuromodulation defaults
        config->set("neuromod.dopamine.scale", 1.0f, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, ConfigSource::Default);
        
        // Development defaults
        config->set("development.enable", false, ConfigSource::Default);  // Default to false for performance
        
        // Memory defaults
        config->set("memory.working_memory.capacity", 50, ConfigSource::Default);
        config->set("memory.episodic.memory_capacity", 500, ConfigSource::Default);
        
        progressReporter.reportCompletion("Configuration", "Default settings applied");
    }
    
    void applyPreset(const std::string& preset) {
        if (preset == "research") {
            // Optimized for research with more neurons and learning
            config->set("neuron_count", 5000, ConfigSource::Default);
            config->set("region_count", 3, ConfigSource::Default);
            config->set("connection_probability", 0.15f, ConfigSource::Default);
            config->set("plasticity.structural.enable", true, ConfigSource::Default);
            config->set("development.enable", true, ConfigSource::Default);
            progressReporter.reportCompletion("Research Preset", "Applied research-optimized settings");
        } else if (preset == "performance") {
            // Optimized for performance with fewer neurons
            config->set("neuron_count", 500, ConfigSource::Default);
            config->set("region_count", 1, ConfigSource::Default);
            config->set("connection_probability", 0.05f, ConfigSource::Default);
            config->set("plasticity.structural.enable", false, ConfigSource::Default);
            config->set("development.enable", false, ConfigSource::Default);
            config->set("simulation_timestep", 0.002, ConfigSource::Default);
            progressReporter.reportCompletion("Performance Preset", "Applied performance-optimized settings");
        } else if (preset == "education") {
            // Optimized for educational purposes with moderate complexity
            config->set("neuron_count", 1500, ConfigSource::Default);
            config->set("region_count", 2, ConfigSource::Default);
            config->set("connection_probability", 0.12f, ConfigSource::Default);
            config->set("plasticity.structural.enable", true, ConfigSource::Default);
            config->set("development.enable", true, ConfigSource::Default);
            progressReporter.reportCompletion("Education Preset", "Applied education-optimized settings");
        } else {
            errorReporter.logWarning("Configuration", "Unknown preset '" + preset + "'. Using defaults.");
        }
    }
    
    std::shared_ptr<Config> getConfig() const { return config; }
    
    bool initializeBrain(std::shared_ptr<Brain>& brain, const std::string& context = "Main") {
        progressReporter.reportProgress("Brain Initialization", 0.0, "Creating brain from configuration");
        
        brain = std::make_shared<Brain>(config);
        
        progressReporter.reportProgress("Brain Initialization", 0.3, "Validating configuration");
        
        if (!InputValidator::validateConfig(*config)) {
            errorReporter.logError(context, "Invalid configuration parameters");
            return false;
        }
        
        progressReporter.reportProgress("Brain Initialization", 0.6, "Setting up neural structures");
        
        if (!brain->initialize()) {
            errorReporter.logError(context, "Failed to initialize brain");
            return false;
        }
        
        progressReporter.reportProgress("Brain Initialization", 0.9, "Finalizing brain setup");
        
        progressReporter.reportCompletion("Brain Initialization", "Brain created successfully");
        return true;
    }
    
    bool createAgentBrain(std::shared_ptr<AgentBrain>& agent, std::shared_ptr<SimpleWorld>& world, const std::string& context = "Main") {
        progressReporter.reportProgress("Agent Setup", 0.0, "Creating agent interface");
        
        if (!brain) {
            errorReporter.logError(context, "Cannot create agent: brain is not initialized");
            return false;
        }
        
        agent = std::make_shared<AgentBrain>(brain);
        
        progressReporter.reportProgress("Agent Setup", 0.5, "Initializing agent with world");
        
        world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        
        agent->initialize(world);
        
        // Enable useful subsystems
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        
        progressReporter.reportCompletion("Agent Setup", "Agent created successfully");
        return true;
    }
    
    void printConfigurationSummary() {
        std::cout << std::endl << "=== Configuration Summary ===" << std::endl;
        std::cout << "Neuron Count: " << config->getOr<int64_t>("neuron_count", 0) << std::endl;
        std::cout << "Region Count: " << config->getOr<int64_t>("region_count", 0) << std::endl;
        std::cout << "Connection Probability: " << config->getOr<float>("connection_probability", 0.0f) << std::endl;
        std::cout << "STDP Enabled: " << (config->getOr<bool>("plasticity.stdp.enable", false) ? "Yes" : "No") << std::endl;
        std::cout << "Hebbian Enabled: " << (config->getOr<bool>("plasticity.hebbian.enable", false) ? "Yes" : "No") << std::endl;
        std::cout << "Structural Plasticity: " << (config->getOr<bool>("plasticity.structural.enable", false) ? "Yes" : "No") << std::endl;
        std::cout << "Development: " << (config->getOr<bool>("development.enable", false) ? "Yes" : "No") << std::endl;
        std::cout << "Curiosity: " << (config->getOr<bool>("neuromod.curiosity.enable", false) ? "Yes" : "No") << std::endl;
        std::cout << "Simulation Timestep: " << config->getOr<double>("simulation_timestep", 0.0) << "s" << std::endl;
        std::cout << std::endl;
    }
};

/**
 * @brief Demonstration of improved user experience features
 */
int demonstrateUserExperience() {
    std::cout << "=== NLM User Experience Improvements Demo ===" << std::endl;
    std::cout << std::endl;
    
    // Create user-friendly configuration
    UserFriendlyConfig userConfig("education");  // Can also use "research" or "performance"
    
    // Print configuration summary
    userConfig.printConfigurationSummary();
    
    // Initialize brain with error handling
    std::shared_ptr<Brain> brain;
    std::shared_ptr<AgentBrain> agent;
    std::shared_ptr<SimpleWorld> world;
    
    std::cout << "Step 1: Creating brain from configuration..." << std::endl;
    if (!userConfig.initializeBrain(brain, "Brain Creation")) {
        std::cerr << "Failed to create brain. Exiting." << std::endl;
        return 1;
    }
    
    std::cout << std::endl;
    std::cout << "Step 2: Creating agent and world..." << std::endl;
    if (!userConfig.createAgentBrain(agent, world, "Agent Creation")) {
        std::cerr << "Failed to create agent. Exiting." << std::endl;
        return 1;
    }
    
    std::cout << std::endl;
    std::cout << "Step 3: Running demonstration simulation..." << std::endl;
    
    // Run a simple demonstration
    int steps = 100;
    for (int step = 0; step < steps; ++step) {
        // Update agent with world percept
        agent->processSensoryInput(world->getSensoryPercept());
        
        // Step brain
        brain->step(step, step * 0.1);
        
        // Get action from agent
        MotorCommand action = agent->decodeMotorCommand();
        
        // Apply action to world
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Show progress every 25 steps
        if (step % 25 == 0) {
            std::cout << "  Step " << step << "/" << steps 
                      << ": " << brain->getFiringNeuronCount() << " firing neurons, "
                      << agent->getCuriosityLevel() << " curiosity level" << std::endl;
        }
    }
    
    std::cout << std::endl;
    std::cout << "=== Simulation Complete ===" << std::endl;
    std::cout << "Brain Statistics:" << std::endl;
    std::cout << "  Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "  Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "  Average Firing Rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "  Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
    
    std::cout << std::endl;
    std::cout << "Agent Statistics:" << std::endl;
    std::cout << "  Curiosity Level: " << agent->getCuriosityLevel() << std::endl;
    std::cout << "  Novelty Level: " << agent->getNoveltyLevel() << std::endl;
    std::cout << "  Neuromodulation Level: " << agent->getNeuromodulationLevel() << std::endl;
    
    std::cout << std::endl;
    std::cout << "World Statistics:" << std::endl;
    std::cout << "  World Size: " << world->getWidth() << "x" << world->getHeight() << std::endl;
    std::cout << "  Simulation Time: " << world->getSimulationTime() << "s" << std::endl;
    
    std::cout << std::endl;
    std::cout << "=== User Experience Features Demonstrated ===" << std::endl;
    std::cout << "✅ Intelligent defaults based on preset configuration" << std::endl;
    std::cout << "✅ Step-by-step progress reporting" << std::endl;
    std::cout << "✅ Error handling with user-friendly messages" << std::endl;
    std::cout << "✅ Configuration validation" << std::endl;
    std::cout << "✅ Multiple preset configurations (education, research, performance)" << std::endl;
    std::cout << "✅ Progress reporting and feedback" << std::endl;
    
    return 0;
}

int main() {
    try {
        return demonstrateUserExperience();
    } catch (const std::exception& e) {
        std::cerr << std::endl;
        std::cerr << "╔══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cerr << "║                 FATAL ERROR - UNEXPECTED EXCEPTION            ║" << std::endl;
        std::cerr << "╠══════════════════════════════════════════════════════════════╣" << std::endl;
        std::cerr << "║ Exception message: " << std::left << std::setw(42) << std::string(e.what()) << "║" << std::endl;
        std::cerr << "╚══════════════════════════════════════════════════════════════╝" << std::endl;
        std::cerr << std::endl;
        std::cerr << "An unexpected error occurred. Please check your system configuration" << std::endl;
        std::cerr << "and consult the documentation for troubleshooting information." << std::endl;
        return 1;
    } catch (...) {
        std::cerr << std::endl;
        std::cerr << "╔══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cerr << "║                 FATAL ERROR - UNKNOWN EXCEPTION              ║" << std::endl;
        std::cerr << "╠══════════════════════════════════════════════════════════════╣" << std::endl;
        std::cerr << "║ Unknown exception occurred. This may indicate a system error.  ║" << std::endl;
        std::cerr << "╚══════════════════════════════════════════════════════════════╝" << std::endl;
        std::cerr << std::endl;
        std::cerr << "An unknown error occurred. Please check your system resources" << std::endl;
        std::cerr << "and memory availability." << std::endl;
        return 1;
    }
}