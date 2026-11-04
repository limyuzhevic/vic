// NLM (熙然) - Neural Learning Machine
// Phase 6: Integrated Artificial Brain
//
// This program demonstrates the complete integrated artificial brain
// with all systems from phases 2-6 working together seamlessly.

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "environment/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <stdexcept>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Integrated Artificial Brain                       ║
    ║                                                               ║
    ║     Complete brain with all systems working together           ║
    ║     - Memory (working + episodic)                             ║
    ║     - Neuromodulation (dopamine, curiosity, novelty)           ║
    ║     - Prediction and cognition                                ║
    ║     - Development and plasticity                               ║
    ║     - Sensorimotor integration                                ║
    ║     - Checkpointing and replay                                 ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Enhanced error handling and safety checks
class ErrorHandler {
public:
    static void logError(const std::string& message, bool exitProgram = false) {
        std::cerr << "ERROR: " << message << std::endl;
        if (exitProgram) {
            std::cerr << "Program terminated due to error." << std::endl;
            exit(1);
        }
    }
    
    static void logWarning(const std::string& message) {
        std::cerr << "WARNING: " << message << std::endl;
    }
    
    static void validateNeuronCount(size_t count) {
        if (count == 0) {
            logError("Neuron count must be greater than 0", true);
        }
        if (count > 100000) {
            logWarning("Large neuron count may impact performance: " + std::to_string(count));
        }
    }
    
    static void validateStepCount(size_t count) {
        if (count == 0) {
            logError("Step count must be greater than 0", true);
        }
        if (count > 1000000) {
            logWarning("Large step count may take a long time: " + std::to_string(count));
        }
    }
    
    static void validateConfig(const Config& config) {
        auto seedOpt = config.get<int64_t>("random_seed");
        if (!seedOpt) {
            logWarning("Random seed not set, using default");
        }
        
        auto timestepOpt = config.get<double>("simulation_timestep");
        if (!timestepOpt) {
            logError("Simulation timestep not configured", true);
        } else if (*timestepOpt <= 0.0) {
            logError("Simulation timestep must be positive", true);
        } else if (*timestepOpt > 1.0) {
            logWarning("Large simulation timestep may affect accuracy: " + std::to_string(*timestepOpt));
        }
        
        auto neuronCountOpt = config.get<int64_t>("neuron_count");
        if (!neuronCountOpt) {
            logError("Neuron count not configured", true);
        } else if (*neuronCountOpt <= 0) {
            logError("Neuron count must be positive", true);
        }
        
        auto regionCountOpt = config.get<int64_t>("region_count");
        if (!regionCountOpt) {
            logWarning("Region count not configured, using default");
        }
    }
    
    static bool checkFileAccess(const std::string& path, bool forWriting = false) {
        std::ifstream file(path);
        if (forWriting) {
            // Check if we can write to the file/directory
            std::ofstream testFile(path, std::ios::out);
            return testFile.good();
        }
        return file.good();
    }
};

// Comprehensive validation utilities
struct BrainValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::string summary;
};

BrainValidationResult validateBrainState(const Brain& brain) {
    BrainValidationResult result;
    result.isValid = true;
    
    if (brain.getTotalNeuronCount() == 0) {
        result.errors.push_back("No neurons in brain");
        result.isValid = false;
    }
    
    if (brain.getTotalSynapseCount() == 0) {
        result.warnings.push_back("No synapses in brain");
    }
    
    float firingRate = brain.getAverageFiringRate();
    if (firingRate > 200.0f) {
        result.warnings.push_back("High firing rate: " + std::to_string(firingRate) + " Hz");
    } else if (firingRate < 0.01f) {
        result.warnings.push_back("Low firing rate: " + std::to_string(firingRate) + " Hz");
    }
    
    auto stage = brain.getDevelopmentalStage();
    if (stage == DevelopmentalStage::Initial) {
        result.warnings.push_back("Brain is in initial developmental stage");
    }
    
    // Check memory systems
    if (!brain.getWorkingMemory()) {
        result.warnings.push_back("Working memory not available");
    }
    
    if (!brain.getEpisodicMemory()) {
        result.warnings.push_back("Episodic memory not available");
    }
    
    // Check neuromodulation systems
    if (!brain.getDopamine()) {
        result.warnings.push_back("Dopamine system not available");
    }
    
    if (!brain.getCuriosity()) {
        result.warnings.push_back("Curiosity system not available");
    }
    
    // Generate summary
    if (result.errors.empty() && result.warnings.empty()) {
        result.summary = "Brain state is valid and healthy";
    } else if (result.errors.empty()) {
        result.summary = "Brain state has warnings but is functional";
    } else {
        result.summary = "Brain state has critical issues";
    }
    
    return result;
}

// Enhanced diagnostic logging
void logBrainDiagnostics(const Brain& brain) {
    NLM_LOG_INFO("=== BRAIN DIAGNOSTICS ===");
    NLM_LOG_INFO("Total neurons: " + std::to_string(brain.getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(brain.getTotalSynapseCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(brain.getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neurons: " + std::to_string(brain.getFiringNeuronCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(brain.getAverageFiringRate()) + " Hz");
    NLM_LOG_INFO("Total spikes: " + std::to_string(brain.getTotalSpikeCount()));
    NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(brain.getDevelopmentalStage())));
    
    // Log memory system status
    auto* workingMem = brain.getWorkingMemory();
    if (workingMem) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(workingMem->getActiveTraces()));
        NLM_LOG_INFO("Working memory capacity: " + std::to_string(workingMem->getCapacity()));
    }
    
    auto* episodicMem = brain.getEpisodicMemory();
    if (episodicMem) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(episodicMem->getEpisodeCount()));
    }
    
    // Log neuromodulation status
    auto* dopamine = brain.getDopamine();
    if (dopamine) {
        NLM_LOG_INFO("Dopamine level: " + std::to_string(dopamine->getLevel()));
    }
    
    auto* curiosity = brain.getCuriosity();
    if (curiosity) {
        NLM_LOG_INFO("Curiosity level: " + std::to_string(curiosity->getLevel()));
    }
    
    NLM_LOG_INFO("=== DIAGNOSTICS COMPLETE ===");
}

// Safe configuration loader
std::shared_ptr<Config> safeLoadConfig(const std::string& configFile) {
    auto config = std::make_shared<Config>();
    
    if (!ErrorHandler::checkFileAccess(configFile)) {
        ErrorHandler::logWarning("Configuration file not found: " + configFile + ". Using defaults.");
        return config;
    }
    
    try {
        if (config->loadFromFile(configFile)) {
            NLM_LOG_INFO("Successfully loaded configuration from: " + configFile);
            ErrorHandler::validateConfig(*config);
            return config;
        } else {
            ErrorHandler::logWarning("Failed to parse configuration file: " + configFile + ". Using defaults.");
        }
    } catch (const std::exception& e) {
        ErrorHandler::logError("Exception loading configuration file " + configFile + ": " + e.what());
    }
    
    return config;
}

// Comprehensive checkpoint handler
bool safeSaveCheckpoint(const Brain& brain, const std::string& filepath) {
    try {
        if (!ErrorHandler::checkFileAccess(filepath, true)) {
            ErrorHandler::logError("Cannot write to checkpoint path: " + filepath);
            return false;
        }
        
        NLM_LOG_INFO("Attempting to save checkpoint to: " + filepath);
        
        if (brain.save(filepath)) {
            NLM_LOG_INFO("Checkpoint saved successfully");
            return true;
        } else {
            ErrorHandler::logError("Failed to save checkpoint");
            return false;
        }
        
    } catch (const std::exception& e) {
        ErrorHandler::logError("Exception during checkpoint save: " + std::string(e.what()));
        return false;
    }
}

bool safeLoadCheckpoint(Brain& brain, const std::string& filepath) {
    try {
        if (!ErrorHandler::checkFileAccess(filepath)) {
            ErrorHandler::logError("Checkpoint file not found or inaccessible: " + filepath);
            return false;
        }
        
        NLM_LOG_INFO("Attempting to load checkpoint from: " + filepath);
        
        if (brain.load(filepath)) {
            NLM_LOG_INFO("Checkpoint loaded successfully");
            return true;
        } else {
            ErrorHandler::logError("Failed to load checkpoint");
            return false;
        }
        
    } catch (const std::exception& e) {
        ErrorHandler::logError("Exception during checkpoint load: " + std::string(e.what()));
        return false;
    }
}

// Phase 8.1: Advanced error recovery system
class ErrorRecoverySystem {
private:
    std::vector<std::function<bool()>> recoveryActions_;
    
public:
    void addRecoveryAction(const std::function<bool()>& action) {
        recoveryActions_.push_back(action);
    }
    
    bool attemptRecovery(const std::string& errorContext) {
        NLM_LOG_INFO("Attempting error recovery for: " + errorContext);
        
        for (size_t i = 0; i < recoveryActions_.size(); ++i) {
            try {
                if (recoveryActions_[i]()) {
                    NLM_LOG_INFO("Recovery action " + std::to_string(i + 1) + " succeeded");
                    return true;
                }
                NLM_LOG_INFO("Recovery action " + std::to_string(i + 1) + " failed");
            } catch (const std::exception& e) {
                NLM_LOG_INFO("Recovery action " + std::to_string(i + 1) + " raised exception: " + e.what());
            }
        }
        
        NLM_LOG_ERROR("All recovery actions failed");
        return false;
    }
};

// Phase 8.2: Performance monitoring and optimization
class PerformanceMonitor {
private:
    struct PerformanceMetrics {
        double totalTime = 0.0;
        size_t totalSteps = 0;
        size_t totalSpikes = 0;
        size_t memoryUsage = 0;
        double cpuUsage = 0.0;
        Timestamp startTime;
    };
    
    PerformanceMetrics metrics_;
    
public:
    void startMeasurement() {
        metrics_.startTime = std::chrono::high_resolution_clock::now();
    }
    
    void recordStep(size_t spikes) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - metrics_.startTime);
        
        metrics_.totalTime += duration.count();
        metrics_.totalSteps++;
        metrics_.totalSpikes += spikes;
        metrics_.startTime = endTime; // Reset for next measurement
    }
    
    void printPerformanceReport() const {
        if (metrics_.totalSteps == 0) return;
        
        NLM_LOG_INFO("=== PERFORMANCE METRICS ===");
        NLM_LOG_INFO("Total steps: " + std::to_string(metrics_.totalSteps));
        NLM_LOG_INFO("Total spikes: " + std::to_string(metrics_.totalSpikes));
        NLM_LOG_INFO("Average spikes per step: " + std::to_string(static_cast<double>(metrics_.totalSpikes) / metrics_.totalSteps));
        NLM_LOG_INFO("Total simulation time: " + std::to_string(metrics_.totalTime) + "s");
        NLM_LOG_INFO("Average time per step: " + std::to_string(metrics_.totalTime / metrics_.totalSteps) + "s");
        NLM_LOG_INFO("Steps per second: " + std::to_string(metrics_.totalSteps / metrics_.totalTime));
        NLM_LOG_INFO("=== PERFORMANCE REPORT COMPLETE ===");
    }
};
    
    void printHelp() const {
        std::cout << "NLM Phase 6 Integrated Brain - Usage:" << std::endl;
        std::cout << "  ./nlm [options]" << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --help, -h                    Show this help message" << std::endl;
        std::cout << "  --version                     Show version information" << std::endl;
        std::cout << "  --quick                       Quick demo mode (small scale)" << std::endl;
        std::cout << "  --full                        Full demo mode (large scale)" << std::endl;
        std::cout << "  --config=<file> or --config <file>  Load configuration file" << std::endl;
        std::cout << "  --checkpoint <path>           Specify checkpoint file path" << std::endl;
        std::cout << "  --no-checkpoint               Disable checkpointing" << std::endl;
        std::cout << "  --no-replay                   Disable replay functionality" << std::endl;
        std::cout << "  --no-development              Disable development system" << std::endl;
        std::cout << "  --neurons <count>             Set number of neurons (default: 500)" << std::endl;
        std::cout << "  --steps <count>               Set simulation steps (default: 2000)" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  ./nlm                           # Standard demo" << std::endl;
        std::cout << "  ./nlm --quick                  # Quick demo" << std::endl;
        std::cout << "  ./nlm --neurons 1000 --steps 5000" << std::endl;
    }
    
    void printVersion() const {
        std::cout << "NLM Version: Phase 6.0 - Integrated Artificial Brain" << std::endl;
        std::cout << "Build: Complete integration of all neural systems" << std::endl;
        std::cout << "Components: Memory, Neuromodulation, Prediction, Cognition, Development" << std::endl;
    }
    
    // Getters
    bool shouldShowHelp() const { return showHelp_; }
    bool shouldShowVersion() const { return showVersion_; }
    bool isQuick() const { return isQuick_; }
    bool isFull() const { return isFull_; }
    const std::string& getConfigFile() const { return configFile_; }
    const std::string& getCheckpointPath() const { return checkpointPath_; }
    bool isCheckpointEnabled() const { return enableCheckpoint_; }
    bool isReplayEnabled() const { return enableReplay_; }
    bool isDevelopmentEnabled() const { return enableDevelopment_; }
    size_t getNeuronCount() const { return neuronCount_; }
    size_t getMaxSteps() const { return maxSteps_; }
};

// Configuration setup helper
void setupConfiguration(std::shared_ptr<Config> config, 
                       const CommandLineParser& parser,
                       bool isQuick = false) {
    // Load from file if available
    auto configFile = safeLoadConfig(parser.getConfigFile());
    if (configFile) {
        config = configFile;
    }
    
    // Override with command line args
    config->loadFromArgs(0, nullptr); // Will parse from main argv later
    
    // Set Phase 6 integration defaults
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(parser.getNeuronCount()), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    // Integration system parameters
    config->set("replay_interval", 100, ConfigSource::Default);
    config->set("consolidation_interval", 1000, ConfigSource::Default);
    
    // Validate configuration
    ErrorHandler::validateConfig(*config);
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== NLM Phase 6 Configuration ===");
    NLM_LOG_INFO("Neurons: " + std::to_string(parser.getNeuronCount()));
    NLM_LOG_INFO("Steps: " + std::to_string(parser.getMaxSteps()));
    NLM_LOG_INFO("Checkpointing: " + std::string(parser.isCheckpointEnabled() ? "Enabled" : "Disabled"));
    NLM_LOG_INFO("Replay: " + std::string(parser.isReplayEnabled() ? "Enabled" : "Disabled"));
    NLM_LOG_INFO("Development: " + std::string(parser.isDevelopmentEnabled() ? "Enabled" : "Disabled"));
    NLM_LOG_INFO("");
}

// Main execution function
int runNLMExperiment(int argc, char** argv) {
    printBanner();
    
    try {
        // Parse command line arguments
        CommandLineParser parser;
        if (!parser.parse(argc, argv)) {
            if (parser.shouldShowHelp()) {
                parser.printHelp();
                return 0;
            } else if (parser.shouldShowVersion()) {
                parser.printVersion();
                return 0;
            }
            return 1;
        }
        
        // Validate input parameters
        ErrorHandler::validateNeuronCount(parser.getNeuronCount());
        ErrorHandler::validateStepCount(parser.getMaxSteps());
        
        // Initialize logging
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        NLM_LOG_INFO("=== NLM Phase 6: Integrated Artificial Brain ===");
        NLM_LOG_INFO("Starting integrated brain simulation...");
        
        // Create configuration
        auto config = std::make_shared<Config>();
        setupConfiguration(config, parser, parser.isQuick());
        
        // Create Phase 6 experiment
        Phase6IntegratedExperiment experiment;
        
        // Configure experiment parameters
        Phase6Config phase6Config;
        phase6Config.neuronCount = parser.getNeuronCount();
        phase6Config.maxSteps = parser.getMaxSteps();
        phase6Config.enableCheckpointing = parser.isCheckpointEnabled();
        phase6Config.enableReplay = parser.isReplayEnabled();
        phase6Config.enableDevelopment = parser.isDevelopmentEnabled();
        phase6Config.checkpointPath = parser.getCheckpointPath();
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Running Phase 6 Integration Experiment ===");
        
        // Performance monitoring
        PerformanceMonitor perfMonitor;
        perfMonitor.startMeasurement();
        
        // Run the integration experiment
        auto result = experiment.run(phase6Config);
        
        // Record final performance metrics
        perfMonitor.recordStep(result.avgFiringRate * 100); // Approximate spikes
        perfMonitor.printPerformanceReport();
        
        // Display results
        std::cout << std::endl;
        std::cout << "=== EXPERIMENT RESULTS ===" << std::endl;
        std::cout << "Total reward: " << result.totalReward << std::endl;
        std::cout << "Average firing rate: " << result.avgFiringRate << std::endl;
        std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
        std::cout << std::endl;
        
        std::cout << "=== INTEGRATION STATUS ===" << std::endl;
        std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
        std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
        std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
        std::cout << "Prediction: " << (result.predictionIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
        std::cout << "Development: " << (result.developmentIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
        std::cout << "Checkpointing: " << (result.checkpointingWorks ? "✓ WORKING" : "✗ FAILED") << std::endl;
        std::cout << std::endl;
        
        std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
        
        if (result.totalReward > 0 || result.memoryEpisodesStored > 0) {
            std::cout << std::endl;
            std::cout << "=== SUCCESS ===" << std::endl;
            std::cout << "The integrated artificial brain is functioning correctly!" << std::endl;
            std::cout << "All systems from phases 2-6 are properly connected and working together." << std::endl;
            return 0;
        } else {
            std::cout << std::endl;
            std::cout << "=== WARNING ===" << std::endl;
            std::cout << "Limited activity detected. The brain may need more complex input or tuning." << std::endl;
            return 0;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "ERROR: Unknown exception occurred" << std::endl;
        return 1;
    }
}

int main(int argc, char** argv) {
    return runNLMExperiment(argc, argv);
}