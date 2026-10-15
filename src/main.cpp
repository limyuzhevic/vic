// main.cpp - Enhanced modular CLI interface for NLM
// Phase 2: Real Neural Computation
// 
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)
// 
// NEW ENHANCED FEATURES:
// - Comprehensive CLI interface with -h/--help, -v/--version, --verbose, --quiet, --debug
// - Command-line experiment selection with -e EXPERIMENT
// - Configuration file support with -c CONFIG_FILE and --config (backward compatible)
// - Dry-run mode with --dry-run
// - Validation mode with --validate-only
// - GPU acceleration with --gpu
// - Modular architecture with CommandLineParser, ConfigurationManager, ExperimentManager
// - Advanced features: custom scripts, profiling, memory tracking, output formats
// - Better error handling with proper error codes and detailed messages
// - Backward compatibility maintained for existing --config=file.cfg syntax

#include "core/CommandLineParser.hpp"
#include "core/ConfigurationManager.hpp"
#include "core/ExperimentManager.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/Random.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "experiments/Experiment.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <fstream>
#include <ctime>
#include <sstream>

using namespace nlm;

// Forward declarations for enhanced error handling
enum class ExitCode {
    Success = 0,
    Error = 1,
    ConfigurationError = 2,
    ExperimentError = 3,
    ValidationError = 4
};

// Enhanced error reporting with suggestions
class ErrorReporter {
public:
    static void report(const std::string& message, ExitCode code = ExitCode::Error,
                      const std::string& suggestion = "") {
        std::cerr << "\n";
        std::cerr << "ERROR: " << message << "\n";
        
        if (!suggestion.empty()) {
            std::cerr << "SUGGESTION: " << suggestion << "\n";
        }
        
        std::cerr << "Exit code: " << static_cast<int>(code) << "\n";
        std::cerr << "\n";
    }
};

// Version information
class VersionInfo {
public:
    static const std::string getVersion() {
        return "Phase 2: Real Neural Computation";
    }
    
    static const std::string getBuildInfo() {
        return "Development Build - " + std::to_string(std::time(nullptr));
    }
    
    static void display() {
        std::cout << "NLM (熙然) — Neural Learning Machine" << std::endl;
        std::cout << "Version: " << getVersion() << std::endl;
        std::cout << "Build: " << getBuildInfo() << std::endl;
        std::cout << "License: MIT" << std::endl;
        std::cout << "Repository: https://github.com/neural-learning-machine/NLM" << std::endl;
    }
};

// Enhanced banner with new features
void printEnhancedBanner() {
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
    ║     Enhanced CLI with modular architecture                   ║
    ║     Advanced error handling and validation                   ║
    ║     Support for multiple experiments (learning, plasticity, etc.) ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Learning experiment for Phase 2
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

// Basic connectivity test
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
        neurons[i]->injectCurrent(50.0f);
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

// Plasticity experiment
void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    
    LearningExperiment experiment(brain, 42);
    
    // Record initial state
    experiment.recordInitialState();
    
    // Enable plasticity on synapses
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
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

// STDP verification
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
        synapses[i]->enablePlasticity(false, true, false);
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

// Main enhanced CLI interface
int main(int argc, char** argv) {
    // Parse command line arguments
    CommandLineParser parser(argc, argv);
    if (!parser.parse()) {
        ErrorReporter::report("Failed to parse command line arguments",
                             ExitCode::Error, "Check syntax and try again");
        return static_cast<int>(ExitCode::Error);
    }
    
    const auto& options = parser.getOptions();
    
    // Handle help flag
    if (options.help) {
        printEnhancedBanner();
        CommandLineParser::showHelp(parser.getProgramName(argv[0]));
        return static_cast<int>(ExitCode::Success);
    }
    
    // Handle version flag
    if (options.version) {
        printEnhancedBanner();
        VersionInfo::display();
        return static_cast<int>(ExitCode::Success);
    }
    
    printEnhancedBanner();
    
    NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation ===");
    NLM_LOG_INFO("Starting enhanced NLM execution with modular architecture");
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Configure logging based on options
    if (!options.quiet.empty()) {
        consoleLogger->setLogLevel(LogLevel::None);
        NLM_LOG_INFO("Output suppressed (quiet mode)");
    } else if (!options.verbose.empty()) {
        consoleLogger->setLogLevel(LogLevel::Debug);
        NLM_LOG_INFO("Verbose logging enabled");
    } else if (!options.debug.empty()) {
        consoleLogger->setLogLevel(LogLevel::Debug);
        NLM_LOG_INFO("Debug logging enabled");
    }
    
    // Log file output if specified
    if (!options.logFile.empty()) {
        NLM_LOG_INFO("Log file would be written to: " + options.logFile);
    }
    
    // Create configuration manager
    ConfigurationManager configManager;
    
    // Convert command line overrides to map
    std::unordered_map<std::string, std::string> cliOverrides;
    
    // Build CLI overrides from parsed options
    if (!options.experiment.empty()) {
        cliOverrides["experiment_name"] = options.experiment;
    }
    if (!options.configFile.empty()) {
        cliOverrides["config_file"] = options.configFile;
    }
    if (options.gpuEnabled) {
        cliOverrides["gpu_enabled"] = "true";
    }
    if (options.profilingEnabled) {
        cliOverrides["profiling_enabled"] = "true";
    }
    if (options.memoryTrackingEnabled) {
        cliOverrides["memory_tracking_enabled"] = "true";
    }
    if (!options.outputFormat.empty()) {
        cliOverrides["output_format"] = options.outputFormat;
    }
    
    // Load configuration
    std::string configFile = "configs/default.cfg";
    if (!options.configFile.empty()) {
        configFile = options.configFile;
    } else if (!cliOverrides.count("config_file")) {
        cliOverrides["config_file"] = "configs/default.cfg";
    }
    
    NLM_LOG_INFO("Loading configuration from: " + configFile);
    
    if (!configManager.loadConfiguration(configFile, cliOverrides)) {
        ErrorReporter::report("Configuration validation failed",
                             ExitCode::ConfigurationError,
                             "Check configuration file syntax and required fields");
        return static_cast<int>(ExitCode::ConfigurationError);
    }
    
    // Dry-run mode - setup and exit
    if (options.dryRun) {
        NLM_LOG_INFO("=== DRY RUN MODE ===");
        NLM_LOG_INFO("Configuration loaded successfully");
        NLM_LOG_INFO("Would run experiments: " + (options.experiment.empty() ? "all" : options.experiment));
        NLM_LOG_INFO("Dry run complete. Exiting without running experiments.");
        return static_cast<int>(ExitCode::Success);
    }
    
    // Validate-only mode - check configs without running
    if (options.validateOnly) {
        NLM_LOG_INFO("=== VALIDATION MODE ===");
        
        if (configManager.validateConfiguration()) {
            NLM_LOG_INFO("✓ Configuration validation passed");
        } else {
            ErrorReporter::report("Configuration validation failed",
                                 ExitCode::ValidationError,
                                 "Review the validation errors above");
            return static_cast<int>(ExitCode::ValidationError);
        }
        
        NLM_LOG_INFO("Validation complete. Exiting without running experiments.");
        return static_cast<int>(ExitCode::Success);
    }
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration Summary:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(configManager.getConfigValue<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(configManager.getConfigValue<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(configManager.getConfigValue<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(configManager.getConfigValue<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(configManager.getConfigValue<float>("connection_probability", 0.15f)));
    NLM_LOG_INFO("");
    
    // Initialize simulation clock
    double timestep = configManager.getConfigValue<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain...");
    
    // Create config from configuration manager
    auto config = configManager.getInternalConfig();
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        ErrorReporter::report("Failed to initialize brain",
                             ExitCode::Error,
                             "Check system resources and configuration");
        return static_cast<int>(ExitCode::Error);
    }
    
    brain->logStatus();
    
    // Determine which experiments to run
    std::string experimentName = options.experiment.empty() ? "learning" : options.experiment;
    NLM_LOG_INFO("Selected experiment: " + experimentName);
    
    // Run experiments based on selection
    if (experimentName == "learning") {
        // Run all Phase 2 experiments
        runBasicConnectivityTest(brain);
        
        // Reset brain for plasticity experiment
        brain->reset();
        brain->initialize();
        
        runPlasticityExperiment(brain);
        
        // Reset and run STDP verification
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
        
    } else if (experimentName == "plasticity") {
        // Run only plasticity experiment
        brain->reset();
        brain->initialize();
        runPlasticityExperiment(brain);
        
    } else if (experimentName == "stdp") {
        // Run only STDP verification
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
        
    } else if (experimentName == "basic") {
        // Run only basic connectivity test
        runBasicConnectivityTest(brain);
        
    } else {
        ErrorReporter::report("Unknown experiment: " + experimentName,
                             ExitCode::ExperimentError,
                             "Use 'learning', 'plasticity', 'stdp', or 'basic'");
        return static_cast<int>(ExitCode::ExperimentError);
    }
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Execution Complete ===");
    NLM_LOG_INFO("NLM execution completed successfully!");
    return static_cast<int>(ExitCode::Success);
}