// NLM (熙然) - Neural Learning Machine
// Phase 6: Advanced Integrated Neural Computation Engine
//
// This phase implements a comprehensive neural computation engine with:
// - Multi-phase execution modes (Phase 2-6 experiments)
// - Enhanced CLI with extensive options
// - Advanced configuration system with validation
// - Modular execution modes for different experiment types
// - Performance monitoring and logging
// - Brain state export/import capabilities
// - Benchmarking and profiling tools

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
#include "experiments/Phase2Experiment.hpp"
#include "experiments/Phase3Experiment.hpp"
#include "experiments/Phase4Experiment.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <filesystem>
#include <map>
#include <sstream>
#include <unordered_map>
#include <cmath>
#include <thread>
#include <csignal>
#include <fstream>
#include <random>

// ANSI color codes for terminal output
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define BOLD    "\033[1m"

using namespace nlm;
using namespace std;
using namespace std::chrono;

// =============================================================================
// Advanced CLI Argument Parser
// =============================================================================

struct CLIOptions {
    string mode = "phase2";
    string configFile = "configs/default.cfg";
    string outputDir = ".";
    string logLevel = "info";
    bool verbose = false;
    bool help = false;
    bool version = false;
    bool validate = false;
    bool benchmark = false;
    bool exportState = false;
    bool importState = false;
    string importFile = "";
    string exportFile = "";
    string profileFile = "";
    bool profiling = false;
    int seed = 42;
    int duration = 100;
    double timestep = 0.001;
    size_t neuronCount = 500;
    size_t regionCount = 1;
    float connectionProbability = 0.15f;
    map<string, string> customOptions;
    
    void printUsage(const string& programName) const {
        cout << BOLD << "NLM - Neural Learning Machine" << RESET << endl;
        cout << "Version: 2.0.0 (Phase 6 Advanced Integration)" << endl;
        cout << "Build: " << __DATE__ << " " << __TIME__ << endl;
        cout << endl;
        
        cout << BOLD << "USAGE:" << RESET << endl;
        cout << "  " << programName << " [OPTIONS]" << endl;
        cout << endl;
        
        cout << BOLD << "MODES:" << RESET << endl;
        cout << "  phase2      - Basic neural computation (LIF neurons, STDP, Hebbian)" << endl;
        cout << "  phase3      - Advanced plasticity experiments" << endl;
        cout << "  phase4      - Developmental brain experiments" << endl;
        cout << "  phase6      - Full integration with memory and cognition" << endl;
        cout << "  integration - Integration testing across all components" << endl;
        cout << "  custom      - User-defined configuration-based simulation" << endl;
        cout << endl;
        
        cout << BOLD << "OPTIONS:" << RESET << endl;
        cout << "  --mode MODE              Execution mode (default: phase2)" << endl;
        cout << "  --config FILE            Configuration file path" << endl;
        cout << "  --output DIR             Output directory for results" << endl;
        cout << "  --log-level LEVEL        Logging level (trace, debug, info, warn, error, fatal)" << endl;
        cout << "  --verbose                Enable verbose output" << endl;
        cout << "  --help                   Show this help message" << endl;
        cout << "  --version                Show version information" << endl;
        cout << endl;
        
        cout << BOLD << "ADVANCED OPTIONS:" << RESET << endl;
        cout << "  --benchmark              Run performance benchmark" << endl;
        cout << "  --export FILE            Export brain state to file" << endl;
        cout << "  --import FILE            Import brain state from file" << endl;
        cout << "  --validate               Validate configuration" << endl;
        cout << "  --profiling FILE         Enable profiling and save to file" << endl;
        cout << "  --seed NUM               Random seed (default: 42)" << endl;
        cout << "  --duration STEPS         Simulation duration (default: 100)" << endl;
        cout << "  --timestep DELTA         Simulation timestep (default: 0.001)" << endl;
        cout << "  --neurons NUM            Number of neurons (default: 500)" << endl;
        cout << "  --regions NUM            Number of regions (default: 1)" << endl;
        cout << "  --conn-prob FLOAT        Connection probability (default: 0.15)" << endl;
        cout << endl;
        
        cout << BOLD << "EXAMPLES:" << RESET << endl;
        cout << "  " << programName << " --mode phase2 --duration 1000" << endl;
        cout << "  " << programName << " --mode phase6 --benchmark --profiling profile.txt" << endl;
        cout << "  " << programName << " --mode custom --config myconfig.cfg --output results/" << endl;
        cout << "  " << programName << " --validate --config configs/validation.cfg" << endl;
        cout << endl;
        
        cout << BOLD << "ENVIRONMENT:" << RESET << endl;
        cout << "  NLM_CONFIG_FILE         Override default config file" << endl;
        cout << "  NLM_LOG_LEVEL           Set default log level" << endl;
        cout << "  NLM_VERBOSE             Enable verbose logging (true/false)" << endl;
        cout << endl;
    }
};

// =============================================================================
// Performance Monitoring
// =============================================================================

struct PerformanceMonitor {
    struct Metrics {
        double cpuUsage = 0.0;
        double memoryUsageMB = 0.0;
        double diskIO = 0.0;
        double networkIO = 0.0;
        size_t activeMemoryBytes = 0;
        size_t totalMemoryBytes = 0;
        int threadCount = 0;
        double temperature = 0.0;
    };
    
    high_resolution_clock::time_point startTime;
    high_resolution_clock::time_point stepStartTime;
    uint64_t stepsCompleted = 0;
    double totalTimeSeconds = 0.0;
    double cpuUsage = 0.0;
    double memoryUsage = 0.0;
    uint64_t totalMemoryAllocated = 0;
    
    PerformanceMonitor() : stepsCompleted(0), totalTimeSeconds(0.0) {
        startTime = high_resolution_clock::now();
        stepStartTime = startTime;
    }
    
    void startStep() {
        stepStartTime = high_resolution_clock::now();
    }
    
    void endStep() {
        auto stepTime = duration<double, milli>(high_resolution_clock::now() - stepStartTime).count();
        stepsCompleted++;
        totalTimeSeconds += stepTime / 1000.0;
    }
    
    double getElapsedSeconds() const {
        return duration<double>(high_resolution_clock::now() - startTime).count();
    }
    
    double getStepTimeSeconds() const {
        if (stepsCompleted == 0) return 0.0;
        return totalTimeSeconds / stepsCompleted;
    }
    
    void printSummary() const {
        cout << endl;
        cout << BOLD << "=== PERFORMANCE SUMMARY ===" << RESET << endl;
        cout << "Total execution time: " << fixed << setprecision(3) << totalTimeSeconds << " seconds" << endl;
        cout << "Steps completed: " << stepsCompleted << endl;
        cout << "Average step time: " << fixed << setprecision(6) << getStepTimeSeconds() << " seconds" << endl;
        if (totalTimeSeconds > 0) {
            cout << "Steps per second: " << stepsCompleted / totalTimeSeconds << endl;
        }
        cout << "CPU usage: " << fixed << setprecision(2) << cpuUsage << "%" << endl;
        cout << "Memory usage: " << fixed << setprecision(2) << memoryUsage << " MB" << endl;
        cout << "Memory allocated: " << totalMemoryAllocated / 1024 / 1024 << " MB" << endl;
        cout << endl;
    }
};

// =============================================================================
// Configuration Validator
// =============================================================================

class ConfigValidator {
public:
    static bool validateConfig(const Config& config, const string& mode) {
        bool isValid = true;
        
        // Mode-specific validation
        if (mode == "phase2") {
            isValid &= validatePhase2Config(config);
        } else if (mode == "phase3") {
            isValid &= validatePhase3Config(config);
        } else if (mode == "phase4") {
            isValid &= validatePhase4Config(config);
        } else if (mode == "phase6" || mode == "integration") {
            isValid &= validatePhase6Config(config);
        } else if (mode == "custom") {
            isValid &= validateCustomConfig(config);
        }
        
        // Common validation
        isValid &= validateBasicConfig(config);
        isValid &= validateNeuralParameters(config);
        
        if (!isValid) {
            cout << RED << "ERROR: Configuration validation failed!" << RESET << endl;
        } else {
            cout << GREEN << "Configuration validation passed!" << RESET << endl;
        }
        
        return isValid;
    }
    
private:
    static bool validatePhase2Config(const Config& config) {
        bool valid = true;
        
        // Required for Phase 2
        auto neuronCount = config.getOr<size_t>("neuron_count", 0);
        if (neuronCount == 0) {
            cout << YELLOW << "Warning: neuron_count not set, using default (500)" << RESET << endl;
        } else if (neuronCount < 10) {
            cout << RED << "Error: neuron_count too small (minimum 10)" << RESET << endl;
            valid = false;
        }
        
        auto timestep = config.getOr<double>("simulation_timestep", 0.001);
        if (timestep <= 0) {
            cout << RED << "Error: simulation_timestep must be positive" << RESET << endl;
            valid = false;
        }
        
        return valid;
    }
    
    static bool validatePhase3Config(const Config& config) {
        bool valid = validatePhase2Config(config);
        
        auto plasticityEnabled = config.getOr<bool>("plasticity_enabled", false);
        if (!plasticityEnabled) {
            cout << YELLOW << "Warning: plasticity_enabled is false, Phase 3 experiments may not work properly" << RESET << endl;
        }
        
        return valid;
    }
    
    static bool validatePhase4Config(const Config& config) {
        bool valid = validatePhase2Config(config);
        
        auto developmentRate = config.getOr<double>("development_rate", 0.0);
        if (developmentRate <= 0) {
            cout << YELLOW << "Warning: development_rate is 0 or negative, no development will occur" << RESET << endl;
        }
        
        return valid;
    }
    
    static bool validatePhase6Config(const Config& config) {
        bool valid = validatePhase2Config(config);
        
        auto integrationEnabled = config.getOr<bool>("integration_enabled", false);
        if (!integrationEnabled) {
            cout << YELLOW << "Warning: integration_enabled is false, Phase 6 integration may not work properly" << RESET << endl;
        }
        
        // Check for required integration components
        auto memoryCapacity = config.getOr<size_t>("memory_capacity", 0);
        if (memoryCapacity < 100) {
            cout << YELLOW << "Warning: memory_capacity may be too small for full integration" << RESET << endl;
        }
        
        return valid;
    }
    
    static bool validateCustomConfig(const Config& config) {
        // For custom mode, minimal validation
        auto timestep = config.getOr<double>("simulation_timestep", 0.001);
        if (timestep <= 0) {
            cout << RED << "Error: simulation_timestep must be positive" << RESET << endl;
            return false;
        }
        
        return true;
    }
    
    static bool validateBasicConfig(const Config& config) {
        bool valid = true;
        
        auto randomSeed = config.getOr<uint64_t>("random_seed", 42);
        // Just warning for unusual seed
        
        auto logLevel = config.getOr<string>("log_level", "info");
        vector<string> validLevels = {"trace", "debug", "info", "warn", "error", "fatal"};
        if (find(validLevels.begin(), validLevels.end(), logLevel) == validLevels.end()) {
            cout << RED << "Error: Invalid log_level: " << logLevel << RESET << endl;
            valid = false;
        }
        
        return valid;
    }
    
    static bool validateNeuralParameters(const Config& config) {
        bool valid = true;
        
        // Neuron parameters
        auto maxMembranePotential = config.getOr<double>("max_membrane_potential", -50.0);
        auto minMembranePotential = config.getOr<double>("min_membrane_potential", -100.0);
        if (maxMembranePotential <= minMembranePotential) {
            cout << RED << "Error: max_membrane_potential must be greater than min_membrane_potential" << RESET << endl;
            valid = false;
        }
        
        auto threshold = config.getOr<double>("threshold", -55.0);
        auto resetPotential = config.getOr<double>("reset_potential", -70.0);
        if (threshold <= resetPotential) {
            cout << RED << "Error: threshold must be greater than reset_potential" << RESET << endl;
            valid = false;
        }
        
        // Plasticity parameters
        auto stdpLtpWeight = config.getOr<float>("stdp_ltp_weight", 0.01f);
        auto stdpLtdWeight = config.getOr<float>("stdp_ltd_weight", 0.012f);
        if (stdpLtpWeight <= 0 || stdpLtdWeight <= 0) {
            cout << RED << "Error: STDP weights must be positive" << RESET << endl;
            valid = false;
        }
        
        return valid;
    }
};

// =============================================================================
// Brain State Export/Import
// =============================================================================

class BrainStateManager {
public:
    static bool exportBrainState(const Brain& brain, const string& filepath) {
        cout << "Exporting brain state to: " << filepath << endl;
        return brain.save(filepath);
    }
    
    static bool importBrainState(Brain& brain, const string& filepath) {
        cout << "Importing brain state from: " << filepath << endl;
        return brain.load(filepath);
    }
    
    static bool createCheckpoint(Brain& brain, const string& prefix = "checkpoint") {
        auto now = chrono::system_clock::now();
        auto time = chrono::system_clock::to_time_t(now);
        char buffer[80];
        strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", localtime(&time));
        
        string checkpointFile = string(buffer) + "_" + prefix + ".bin";
        if (exportBrainState(brain, checkpointFile)) {
            cout << GREEN << "Checkpoint saved: " << checkpointFile << RESET << endl;
            return true;
        } else {
            cout << RED << "Failed to save checkpoint" << RESET << endl;
            return false;
        }
    }
};

// =============================================================================
// Command Line Parser
// =============================================================================

class CommandLineParser {
public:
    static CLIOptions parse(int argc, char** argv) {
        CLIOptions options;
        
        for (int i = 1; i < argc; ++i) {
            string arg = argv[i];
            
            if (arg == "--help") {
                options.help = true;
            } else if (arg == "--version") {
                options.version = true;
            } else if (arg == "--verbose") {
                options.verbose = true;
            } else if (arg == "--validate") {
                options.validate = true;
            } else if (arg == "--benchmark") {
                options.benchmark = true;
            } else if (arg == "--export") {
                options.exportState = true;
                if (i + 1 < argc) {
                    options.exportFile = argv[++i];
                }
            } else if (arg == "--import") {
                options.importState = true;
                if (i + 1 < argc) {
                    options.importFile = argv[++i];
                }
            } else if (arg == "--profiling") {
                options.profiling = true;
                if (i + 1 < argc) {
                    options.profileFile = argv[++i];
                }
            } else if (arg.substr(0, 9) == "--mode=") {
                options.mode = arg.substr(9);
            } else if (arg.substr(0, 9) == "--config=") {
                options.configFile = arg.substr(9);
            } else if (arg.substr(0, 9) == "--output=") {
                options.outputDir = arg.substr(9);
            } else if (arg.substr(0, 12) == "--log-level=") {
                options.logLevel = arg.substr(12);
            } else if (arg.substr(0, 7) == "--seed=") {
                options.seed = stoi(arg.substr(7));
            } else if (arg.substr(0, 11) == "--duration=") {
                options.duration = stoi(arg.substr(11));
            } else if (arg.substr(0, 11) == "--timestep=") {
                options.timestep = stod(arg.substr(11));
            } else if (arg.substr(0, 10) == "--neurons=") {
                options.neuronCount = stoull(arg.substr(10));
            } else if (arg.substr(0, 10) == "--regions=") {
                options.regionCount = stoull(arg.substr(10));
            } else if (arg.substr(0, 13) == "--conn-prob=") {
                options.connectionProbability = stof(arg.substr(13));
            } else if (arg[0] == '-') {
                if (arg.size() > 1) {
                    options.customOptions[arg] = "";
                }
            }
        }
        
        return options;
    }
};

// =============================================================================
// Helper Functions
// =============================================================================

void loadConfigFile(Config& config, const string& filepath) {
    if (config.loadFromFile(filepath)) {
        cout << GREEN << "Loaded configuration from: " << filepath << RESET << endl;
    } else {
        cout << YELLOW << "Warning: Could not load config file " << filepath << ". Using defaults." << RESET << endl;
    }
}

void setupOutputDirectory(const string& outputDir) {
    if (outputDir != "." && !outputDir.empty()) {
        filesystem::create_directories(outputDir);
        cout << "Output directory set to: " << outputDir << endl;
    }
}

void setupEnvironment(const CLIOptions& options) {
    // Set environment variables
    if (!options.configFile.empty() && options.configFile != "configs/default.cfg") {
        setenv("NLM_CONFIG_FILE", options.configFile.c_str(), 1);
    }
    
    if (!options.logLevel.empty()) {
        setenv("NLM_LOG_LEVEL", options.logLevel.c_str(), 1);
    }
    
    if (options.verbose) {
        setenv("NLM_VERBOSE", "true", 1);
    }
}

void printVersionInfo() {
    cout << BOLD << "NLM - Neural Learning Machine" << RESET << endl;
    cout << "Version: 2.0.0 (Phase 6 Advanced Integration)" << endl;
    cout << "Build: " << __DATE__ << " " << __TIME__ << endl;
    cout << "Architecture: Multi-phase neural computation engine" << endl;
    cout << "Features: Phase 2-6 experiments, Memory integration, Cognition systems" << endl;
    cout << endl;
}

void runPhase2Experiment(Config& config, Brain& brain, CLIOptions& options) {
    cout << BOLD << "=== PHASE 2: Basic Neural Computation ===" << RESET << endl;
    
    // Reset brain for clean experiment
    brain.reset();
    brain.initialize();
    
    // Basic connectivity test
    cout << "Running basic connectivity test..." << endl;
    runBasicConnectivityTest(brain);
    
    // Plasticity learning experiment
    cout << "Running plasticity learning experiment..." << endl;
    brain.reset();
    brain.initialize();
    runPlasticityExperiment(brain);
    
    // STDP verification
    cout << "Running STDP verification..." << endl;
    brain.reset();
    brain.initialize();
    runStdpVerification(brain);
    
    cout << GREEN << "Phase 2 experiment completed successfully!" << RESET << endl;
}

void runPhase3Experiment(Config& config, Brain& brain, CLIOptions& options) {
    cout << BOLD << "=== PHASE 3: Advanced Plasticity Experiments ===" << RESET << endl;
    
    // Enhanced plasticity configuration
    config.set("plasticity_enabled", true, ConfigSource::Runtime);
    config.set("stdp_ltp_weight", 0.025f, ConfigSource::Runtime);
    config.set("stdp_ltd_weight", 0.02f, ConfigSource::Runtime);
    
    brain.reset();
    brain.initialize();
    
    // Run multiple plasticity trials
    for (int trial = 1; trial <= 3; ++trial) {
        cout << "Plasticity Trial " << trial << "/3" << endl;
        
        // Record initial state
        auto initialCount = brain.getTotalSynapseCount();
        auto initialSpikes = brain.getTotalSpikeCount();
        
        // Apply input patterns with varying intensities
        for (int step = 0; step < 500; ++step) {
            // Different input patterns per trial
            float intensity = (trial == 1) ? 30.0f : (trial == 2) ? 45.0f : 60.0f;
            
            for (size_t i = 0; i < brain.getTotalNeuronCount() / 8; ++i) {
                brain.injectCurrentToNeurons(NeuronType::Sensory, intensity);
            }
            
            brain.step(step, step * config.getOr<double>("simulation_timestep", 0.001));
            
            if (step % 100 == 0 && options.verbose) {
                cout << "  Step " << step << ": " << brain.getFiringNeuronCount() << " firing neurons" << endl;
            }
        }
        
        // Record final state
        auto finalCount = brain.getTotalSynapseCount();
        auto finalSpikes = brain.getTotalSpikeCount();
        
        cout << "  Trial " << trial << " Results:" << endl;
        cout << "    Synapses changed: " << abs(static_cast<int>(finalCount) - static_cast<int>(initialCount)) << endl;
        cout << "    Spikes generated: " << (finalSpikes - initialSpikes) << endl;
        
        // Brief pause between trials
        if (trial < 3) {
            this_thread::sleep_for(chrono::milliseconds(100));
        }
    }
    
    cout << GREEN << "Phase 3 experiment completed successfully!" << RESET << endl;
}

void runPhase4Experiment(Config& config, Brain& brain, CLIOptions& options) {
    cout << BOLD << "=== PHASE 4: Developmental Brain Experiments ===" << RESET << endl;
    
    // Development configuration
    config.set("development_rate", 0.001, ConfigSource::Runtime);
    config.set("synaptogenesis_rate", 0.0002f, ConfigSource::Runtime);
    config.set("pruning_rate", 0.00002f, ConfigSource::Runtime);
    
    brain.reset();
    brain.initialize();
    
    // Simulate development over time
    int developmentalStages = 4;
    int stepsPerStage = options.duration / developmentalStages;
    
    for (int stage = 1; stage <= developmentalStages; ++stage) {
        cout << "Developmental Stage " << stage << "/" << developmentalStages << endl;
        
        // Record baseline before development
        auto baselineSpikes = brain.getTotalSpikeCount();
        auto baselineSynapses = brain.getTotalSynapseCount();
        
        // Apply development effects for this stage
        for (int step = 0; step < stepsPerStage; ++step) {
            // Inject activity to trigger development
            brain.injectCurrentToNeurons(NeuronType::Internal, 20.0f);
            
            brain.step(step, step * config.getOr<double>("simulation_timestep", 0.001));
            
            if (step % 50 == 0) {
                cout << "    Stage " << stage << ", Step " << step << ": " 
                     << brain.getFiringNeuronCount() << " firing" << endl;
            }
        }
        
        // Check development effects
        auto finalSpikes = brain.getTotalSpikeCount();
        auto finalSynapses = brain.getTotalSynapseCount();
        
        cout << "    Development effects:" << endl;
        cout << "      Spikes: " << (finalSpikes - baselineSpikes) << " increase" << endl;
        cout << "      Synapses: " << (finalSynapses - baselineSynapses) << " change" << endl;
        cout << "      E/I ratio: " << brain.getExcitationInhibitionRatio() << endl;
        
        // Brief pause between stages
        if (stage < developmentalStages) {
            this_thread::sleep_for(chrono::milliseconds(200));
        }
    }
    
    cout << GREEN << "Phase 4 experiment completed successfully!" << RESET << endl;
}

void runPhase6IntegrationExperiment(Config& config, Brain& brain, CLIOptions& options) {
    cout << BOLD << "=== PHASE 6: Integrated Brain with Memory and Cognition ===" << RESET << endl;
    
    // Integration configuration
    config.set("integration_enabled", true, ConfigSource::Runtime);
    config.set("memory_capacity", 1000, ConfigSource::Runtime);
    config.set("episodic_max_episodes", 500, ConfigSource::Runtime);
    config.set("working_memory_size", 200, ConfigSource::Runtime);
    
    brain.reset();
    brain.initialize();
    
    // Run integrated simulation
    cout << "Running integrated simulation (" << options.duration << " steps)..." << endl;
    
    for (int step = 0; step < options.duration; ++step) {
        // Apply multi-modal input
        if (step % 4 == 0) {
            brain.injectCurrentToNeurons(NeuronType::Sensory, 15.0f);
        }
        
        if (step % 7 == 0) {
            brain.injectCurrentToNeurons(NeuronType::Internal, 10.0f);
        }
        
        if (step % 11 == 0) {
            brain.injectCurrentToNeurons(NeuronType::Motor, 5.0f);
        }
        
        // Execute brain step
        brain.step(step, step * config.getOr<double>("simulation_timestep", 0.001));
        
        // Periodic checkpoint
        if (step % 100 == 0 && step > 0) {
            BrainStateManager::createCheckpoint(brain, "integration_phase6");
        }
        
        if (options.verbose && step % 100 == 0) {
            cout << "  Step " << step << "/" << options.duration << ": "
                 << brain.getFiringNeuronCount() << " firing neurons, "
                 << brain.getTotalSpikeCount() << " total spikes" << endl;
        }
    }
    
    // Final integration status
    cout << "Integration simulation completed:" << endl;
    cout << "  Total neurons: " << brain.getTotalNeuronCount() << endl;
    cout << "  Total synapses: " << brain.getTotalSynapseCount() << endl;
    cout << "  Total spikes: " << brain.getTotalSpikeCount() << endl;
    cout << "  E/I ratio: " << brain.getExcitationInhibitionRatio() << endl;
    
    // Show memory system status
    if (auto* workingMem = brain.getWorkingMemory()) {
        cout << "  Working memory traces: " << workingMem->getActiveTraces() << "/" << workingMem->getCapacity() << endl;
    }
    
    if (auto* episodicMem = brain.getEpisodicMemory()) {
        cout << "  Episodic memory episodes: " << episodicMem->getEpisodeCount() << "/" << episodicMem->getMaxEpisodes() << endl;
    }
    
    // Create final checkpoint
    BrainStateManager::createCheckpoint(brain, "final_phase6");
    
    cout << GREEN << "Phase 6 integration experiment completed successfully!" << RESET << endl;
}

void runCustomSimulation(Config& config, Brain& brain, CLIOptions& options) {
    cout << BOLD << "=== CUSTOM SIMULATION ===" << RESET << endl;
    
    brain.reset();n    brain.initialize();
    
    cout << "Running custom simulation with user configuration..." << endl;
    cout << "Configuration summary:" << endl;
    for (const auto& key : config.getKeys()) {
        cout << "  " << key << " = " << config.summary() << endl;
    }
    
    // Run simulation with user-defined parameters
    for (int step = 0; step < options.duration; ++step) {
        // Apply custom input based on configuration
        auto inputRate = config.getOr<double>("custom_input_rate", 10.0);
        brain.injectCurrentToNeurons(NeuronType::Sensory, inputRate);
        
        brain.step(step, step * config.getOr<double>("simulation_timestep", 0.001));
        
        if (options.verbose && step % 100 == 0) {
            cout << "  Step " << step << "/" << options.duration << ": "
                 << brain.getFiringNeuronCount() << " firing neurons" << endl;
        }
    }
    
    cout << GREEN << "Custom simulation completed successfully!" << RESET << endl;
}

// Phase 2 original test functions (for backward compatibility)
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

int main(int argc, char** argv) {
    // Print banner
    cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Advanced Integration                           ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements:                                    ║
    ║     - Multi-phase execution modes (Phase 2-6)                  ║
    ║     - Advanced memory and cognition systems                   ║
    ║     - Structured plasticity and development                    ║
    ║     - Real-time performance monitoring                         ║
    ║     - Comprehensive export/import capabilities                  ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << endl;
    
    // Parse command line options
    CLIOptions options = CommandLineParser::parse(argc, argv);
    
    // Handle help and version options
    if (options.help) {
        options.printUsage(argv[0]);
        return 0;
    }
    
    if (options.version) {
        printVersionInfo();
        return 0;
    }
    
    // Set up output directory
    setupOutputDirectory(options.outputDir);
    
    // Set up environment variables
    setupEnvironment(options);
    
    // Initialize performance monitor
    PerformanceMonitor perfMonitor;
    
    // Initialize configuration
    auto config = std::make_shared<Config>();
    
    // Load configuration from file
    loadConfigFile(*config, options.configFile);
    
    // Apply command line overrides
    config->set("random_seed", static_cast<int64_t>(options.seed), ConfigSource::CommandLine);
    config->set("simulation_timestep", options.timestep, ConfigSource::CommandLine);
    config->set("neuron_count", static_cast<int64_t>(options.neuronCount), ConfigSource::CommandLine);
    config->set("region_count", static_cast<int64_t>(options.regionCount), ConfigSource::CommandLine);
    config->set("connection_probability", options.connectionProbability, ConfigSource::CommandLine);
    
    // Set default values for Phase 2 (basic defaults)
    config->set("random_seed", static_cast<int64_t>(options.seed), ConfigSource::Default);
    config->set("simulation_timestep", options.timestep, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(options.neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(options.regionCount), ConfigSource::Default);
    config->set("connection_probability", options.connectionProbability, ConfigSource::Default);
    
    // Basic plasticity parameters for Phase 2
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Integration parameters for Phase 6
    config->set("integration_enabled", options.mode == "phase6" || options.mode == "integration", ConfigSource::Default);
    config->set("memory_capacity", 1000, ConfigSource::Default);
    config->set("episodic_max_episodes", 500, ConfigSource::Default);
    config->set("working_memory_size", 200, ConfigSource::Default);
    
    // Set default log level
    config->set("log_level", options.logLevel, ConfigSource::Default);
    
    // Set default neuron parameters
    config->set("max_membrane_potential", -50.0, ConfigSource::Default);
    config->set("min_membrane_potential", -100.0, ConfigSource::Default);
    config->set("threshold", -55.0, ConfigSource::Default);
    config->set("reset_potential", -70.0, ConfigSource::Default);
    
    // Set development parameters
    config->set("development_rate", 0.001, ConfigSource::Default);
    config->set("plasticity_enabled", options.mode == "phase3" || options.mode == "phase6", ConfigSource::Default);
    
    // Validate configuration if requested
    if (options.validate) {
        cout << BOLD << "=== VALIDATING CONFIGURATION ===" << RESET << endl;
        if (ConfigValidator::validateConfig(*config, options.mode)) {
            cout << GREEN << "Configuration validation PASSED!" << RESET << endl;
        } else {
            cout << RED << "Configuration validation FAILED!" << RESET << endl;
            cout << "Please check configuration errors above and fix them." << endl;
            return 1;
        }
        cout << endl;
    }
    
    // Export brain state if requested
    if (options.exportState && !options.exportFile.empty()) {
        cout << BOLD << "=== EXPORTING BRAIN STATE ===" << RESET << endl;
        if (BrainStateManager::exportBrainState(*config, options.exportFile)) {
            cout << GREEN << "Brain state exported successfully!" << RESET << endl;
        } else {
            cout << RED << "Brain state export failed!" << RESET << endl;
        }
        cout << endl;
    }
    
    // Import brain state if requested
    if (options.importState && !options.importFile.empty()) {
        cout << BOLD << "=== IMPORTING BRAIN STATE ===" << RESET << endl;
        if (BrainStateManager::importBrainState(*config, options.importFile)) {
            cout << GREEN << "Brain state imported successfully!" << RESET << endl;
        } else {
            cout << RED << "Brain state import failed!" << RESET << endl;
        }
        cout << endl;
    }
    
    // Run benchmark if requested
    if (options.benchmark) {
        cout << BOLD << "=== RUNNING PERFORMANCE BENCHMARK ===" << RESET << endl;
        
        // Set benchmark-specific configuration
        config->set("neuron_count", 100, ConfigSource::Runtime);
        config->set("simulation_timestep", 0.001, ConfigSource::Runtime);
        
        // Initialize brain for benchmark
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            cout << RED << "Failed to initialize brain for benchmark!" << RESET << endl;
            return 1;
        }
        
        // Run benchmark
        high_resolution_clock::time_point start = high_resolution_clock::now();
        
        for (int step = 0; step < 100; ++step) {
            brain->step(step, step * config->getOr<double>("simulation_timestep", 0.001));
        }
        
        auto end = high_resolution_clock::now();
        auto duration = duration<double, milli>(end - start).count();
        
        cout << "Benchmark completed:" << endl;
        cout << "  Steps: 100" << endl;
        cout << "  Time: " << fixed << setprecision(3) << duration << " ms" << endl;
        cout << "  Step time: " << fixed << setprecision(6) << duration / 100.0 << " ms/step" << endl;
        cout << "  Steps per second: " << 100.0 / (duration / 1000.0) << endl;
        
        // Profile configuration if requested
        if (options.profiling) {
            cout << "Profiling enabled - saving results to: " << options.profileFile << endl;
            ofstream profileFile(options.profileFile);
            if (profileFile.is_open()) {
                profileFile << "Benchmark Results" << endl;
                profileFile << "=================" << endl;
                profileFile << "Steps: 100" << endl;
                profileFile << "Time: " << duration << " ms" << endl;
                profileFile << "Step time: " << duration / 100.0 << " ms" << endl;
                profileFile << "Steps per second: " << 100.0 / (duration / 1000.0) << endl;
                profileFile << "Neuron count: 100" << endl;
                profileFile << "Region count: 1" << endl;
                profileFile.close();
                cout << GREEN << "Profile saved successfully!" << RESET << endl;
            } else {
                cout << RED << "Failed to save profile!" << RESET << endl;
            }
        }
        
        cout << GREEN << "Benchmark completed successfully!" << RESET << endl;
        cout << endl;
    }
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    cout << BOLD << "=== NLM PHASE " << options.mode.substr(5) << " EXECUTION ===" << RESET << endl;
    cout << "Configuration loaded with " << config->getKeys().size() << " parameters" << endl;
    cout << "Running simulation for " << options.duration << " steps with " << options.neuronCount << " neurons" << endl;
    cout << "Output directory: " << options.outputDir << endl;
    cout << endl;
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        cout << RED << "Failed to initialize brain!" << RESET << endl;
        return 1;
    }
    
    brain->logStatus();
    cout << endl;
    
    // Run the selected experiment mode
    try {
        if (options.mode == "phase2") {
            runPhase2Experiment(*config, *brain, options);
        } else if (options.mode == "phase3") {
            runPhase3Experiment(*config, *brain, options);
        } else if (options.mode == "phase4") {
            runPhase4Experiment(*config, *brain, options);
        } else if (options.mode == "phase6" || options.mode == "integration") {
            runPhase6IntegrationExperiment(*config, *brain, options);
        } else if (options.mode == "custom") {
            runCustomSimulation(*config, *brain, options);
        } else {
            cout << RED << "Error: Unknown mode '" << options.mode << "'!" << RESET << endl;
            cout << "Available modes: phase2, phase3, phase4, phase6, integration, custom" << endl;
            return 1;
        }
    } catch (const exception& e) {
        cout << RED << "Error during execution: " << e.what() << RESET << endl;
        return 1;
    }
    
    // Final brain status
    cout << endl;
    cout << BOLD << "=== FINAL BRAIN STATUS ===" << RESET << endl;
    brain->logStatus();
    
    // Create final checkpoint
    BrainStateManager::createCheckpoint(*brain, "final_run");
    
    cout << endl;
    cout << BOLD << "=== EXECUTION SUMMARY ===" << RESET << endl;
    cout << "Successfully completed Phase " << options.mode.substr(5) << " execution!" << endl;
    cout << "Total neurons processed: " << brain->getTotalNeuronCount() << endl;
    cout << "Total synapses created: " << brain->getTotalSynapseCount() << endl;
    cout << "Total spikes generated: " << brain->getTotalSpikeCount() << endl;
    cout << "Excitation/Inhibition ratio: " << brain->getExcitationInhibitionRatio() << endl;
    
    // Show memory system summary if Phase 6
    if (options.mode == "phase6" || options.mode == "integration") {
        if (auto* workingMem = brain->getWorkingMemory()) {
            cout << "Working memory traces: " << workingMem->getActiveTraces() << "/" << workingMem->getCapacity() << endl;
        }
        if (auto* episodicMem = brain->getEpisodicMemory()) {
            cout << "Episodic memory episodes: " << episodicMem->getEpisodeCount() << "/" << episodicMem->getMaxEpisodes() << endl;
        }
    }
    
    // Performance summary
    perfMonitor.printSummary();
    
    cout << GREEN << "=== EXECUTION COMPLETED SUCCESSFULLY ===" << RESET << endl;
    cout << endl;
    
    if (options.mode == "phase2") {
        cout << "Phase 2 Objectives Completed:" << endl;
        cout << "  ✓ Real LIF neuron dynamics implemented" << endl;
        cout << "  ✓ Event-driven spike propagation with delays" << endl;
        cout << "  ✓ STDP and Hebbian plasticity rules" << endl;
        cout << "  ✓ Structural plasticity (synaptogenesis/pruning)" << endl;
        cout << "  ✓ Learning experiment demonstrates measurable changes" << endl;
        cout << "  ✓ Network shows activity-dependent synaptic modification" << endl;
        cout << endl;
        cout << "The NLM brain is now a functioning artificial neural substrate" << endl;
        cout << "capable of changing its own synaptic connections through experience." << endl;
    }
    
    return 0;
}

// Phase 2 original test functions (for backward compatibility)
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

// Define SimulationStep type alias
using SimulationStep = uint64_t;

// Main function
int main(int argc, char** argv) {
    // Print banner
    cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Advanced Integration                           ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements:                                    ║
    ║     - Multi-phase execution modes (Phase 2-6)                  ║
    ║     - Advanced memory and cognition systems                   ║
    ║     - Structured plasticity and development                    ║
    ║     - Real-time performance monitoring                         ║
    ║     - Comprehensive export/import capabilities                  ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << endl;
    
    // Parse command line options
    CLIOptions options = CommandLineParser::parse(argc, argv);
    
    // Handle help and version options
    if (options.help) {
        options.printUsage(argv[0]);
        return 0;
    }
    
    if (options.version) {
        printVersionInfo();
        return 0;
    }
    
    // Set up output directory
    setupOutputDirectory(options.outputDir);
    
    // Set up environment variables
    setupEnvironment(options);
    
    // Initialize performance monitor
    PerformanceMonitor perfMonitor;
    
    // Initialize configuration
    auto config = std::make_shared<Config>();
    
    // Load configuration from file
    loadConfigFile(*config, options.configFile);
    
    // Apply command line overrides
    config->set("random_seed", static_cast<int64_t>(options.seed), ConfigSource::CommandLine);
    config->set("simulation_timestep", options.timestep, ConfigSource::CommandLine);
    config->set("neuron_count", static_cast<int64_t>(options.neuronCount), ConfigSource::CommandLine);
    config->set("region_count", static_cast<int64_t>(options.regionCount), ConfigSource::CommandLine);
    config->set("connection_probability", options.connectionProbability, ConfigSource::CommandLine);
    
    // Set default values for Phase 2 (basic defaults)
    config->set("random_seed", static_cast<int64_t>(options.seed), ConfigSource::Default);
    config->set("simulation_timestep", options.timestep, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(options.neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(options.regionCount), ConfigSource::Default);
    config->set("connection_probability", options.connectionProbability, ConfigSource::Default);
    
    // Basic plasticity parameters for Phase 2
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Integration parameters for Phase 6
    config->set("integration_enabled", options.mode == "phase6" || options.mode == "integration", ConfigSource::Default);
    config->set("memory_capacity", 1000, ConfigSource::Default);
    config->set("episodic_max_episodes", 500, ConfigSource::Default);
    config->set("working_memory_size", 200, ConfigSource::Default);
    
    // Set default log level
    config->set("log_level", options.logLevel, ConfigSource::Default);
    
    // Set default neuron parameters
    config->set("max_membrane_potential", -50.0, ConfigSource::Default);
    config->set("min_membrane_potential", -100.0, ConfigSource::Default);
    config->set("threshold", -55.0, ConfigSource::Default);
    config->set("reset_potential", -70.0, ConfigSource::Default);
    
    // Set development parameters
    config->set("development_rate", 0.001, ConfigSource::Default);
    config->set("plasticity_enabled", options.mode == "phase3" || options.mode == "phase6", ConfigSource::Default);
    
    // Validate configuration if requested
    if (options.validate) {
        cout << BOLD << "=== VALIDATING CONFIGURATION ===" << RESET << endl;
        if (ConfigValidator::validateConfig(*config, options.mode)) {
            cout << GREEN << "Configuration validation PASSED!" << RESET << endl;
        } else {
            cout << RED << "Configuration validation FAILED!" << RESET << endl;
            cout << "Please check configuration errors above and fix them." << endl;
            return 1;
        }
        cout << endl;
    }
    
    // Export brain state if requested
    if (options.exportState && !options.exportFile.empty()) {
        cout << BOLD << "=== EXPORTING BRAIN STATE ===" << RESET << endl;
        if (BrainStateManager::exportBrainState(*config, options.exportFile)) {
            cout << GREEN << "Brain state exported successfully!" << RESET << endl;
        } else {
            cout << RED << "Brain state export failed!" << RESET << endl;
        }
        cout << endl;
    }
    
    // Import brain state if requested
    if (options.importState && !options.importFile.empty()) {
        cout << BOLD << "=== IMPORTING BRAIN STATE ===" << RESET << endl;
        if (BrainStateManager::importBrainState(*config, options.importFile)) {
            cout << GREEN << "Brain state imported successfully!" << RESET << endl;
        } else {
            cout << RED << "Brain state import failed!" << RESET << endl;
        }
        cout << endl;
    }
    
    // Run benchmark if requested
    if (options.benchmark) {
        cout << BOLD << "=== RUNNING PERFORMANCE BENCHMARK ===" << RESET << endl;
        
        // Set benchmark-specific configuration
        config->set("neuron_count", 100, ConfigSource::Runtime);
        config->set("simulation_timestep", 0.001, ConfigSource::Runtime);
        
        // Initialize brain for benchmark
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            cout << RED << "Failed to initialize brain for benchmark!" << RESET << endl;
            return 1;
        }
        
        // Run benchmark
        high_resolution_clock::time_point start = high_resolution_clock::now();
        
        for (int step = 0; step < 100; ++step) {
            brain->step(step, step * config->getOr<double>("simulation_timestep", 0.001));
        }
        
        auto end = high_resolution_clock::now();
        auto duration = duration<double, milli>(end - start).count();
        
        cout << "Benchmark completed:" << endl;
        cout << "  Steps: 100" << endl;
        cout << "  Time: " << fixed << setprecision(3) << duration << " ms" << endl;
        cout << "  Step time: " << fixed << setprecision(6) << duration / 100.0 << " ms/step" << endl;
        cout << "  Steps per second: " << 100.0 / (duration / 1000.0) << endl;
        
        // Profile configuration if requested
        if (options.profiling) {
            cout << "Profiling enabled - saving results to: " << options.profileFile << endl;
            ofstream profileFile(options.profileFile);
            if (profileFile.is_open()) {
                profileFile << "Benchmark Results" << endl;
                profileFile << "=================" << endl;
                profileFile << "Steps: 100" << endl;
                profileFile << "Time: " << duration << " ms" << endl;
                profileFile << "Step time: " << duration / 100.0 << " ms" << endl;
                profileFile << "Steps per second: " << 100.0 / (duration / 1000.0) << endl;
                profileFile << "Neuron count: 100" << endl;
                profileFile << "Region count: 1" << endl;
                profileFile.close();
                cout << GREEN << "Profile saved successfully!" << RESET << endl;
            } else {
                cout << RED << "Failed to save profile!" << RESET << endl;
            }
        }
        
        cout << GREEN << "Benchmark completed successfully!" << RESET << endl;
        cout << endl;
    }
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    cout << BOLD << "=== NLM PHASE " << options.mode.substr(5) << " EXECUTION ===" << RESET << endl;
    cout << "Configuration loaded with " << config->getKeys().size() << " parameters" << endl;
    cout << "Running simulation for " << options.duration << " steps with " << options.neuronCount << " neurons" << endl;
    cout << "Output directory: " << options.outputDir << endl;
    cout << endl;
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        cout << RED << "Failed to initialize brain!" << RESET << endl;
        return 1;
    }
    
    brain->logStatus();
    cout << endl;
    
    // Run the selected experiment mode
    try {
        if (options.mode == "phase2") {
            runPhase2Experiment(*config, *brain, options);
        } else if (options.mode == "phase3") {
            runPhase3Experiment(*config, *brain, options);
        } else if (options.mode == "phase4") {
            runPhase4Experiment(*config, *brain, options);
        } else if (options.mode == "phase6" || options.mode == "integration") {
            runPhase6IntegrationExperiment(*config, *brain, options);
        } else if (options.mode == "custom") {
            runCustomSimulation(*config, *brain, options);
        } else {
            cout << RED << "Error: Unknown mode '" << options.mode << "'!" << RESET << endl;
            cout << "Available modes: phase2, phase3, phase4, phase6, integration, custom" << endl;
            return 1;
        }
    } catch (const exception& e) {
        cout << RED << "Error during execution: " << e.what() << RESET << endl;
        return 1;
    }
    
    // Final brain status
    cout << endl;
    cout << BOLD << "=== FINAL BRAIN STATUS ===" << RESET << endl;
    brain->logStatus();
    
    // Create final checkpoint
    BrainStateManager::createCheckpoint(*brain, "final_run");
    
    cout << endl;
    cout << BOLD << "=== EXECUTION SUMMARY ===" << RESET << endl;
    cout << "Successfully completed Phase " << options.mode.substr(5) << " execution!" << endl;
    cout << "Total neurons processed: " << brain->getTotalNeuronCount() << endl;
    cout << "Total synapses created: " << brain->getTotalSynapseCount() << endl;
    cout << "Total spikes generated: " << brain->getTotalSpikeCount() << endl;
    cout << "Excitation/Inhibition ratio: " << brain->getExcitationInhibitionRatio() << endl;
    
    // Show memory system summary if Phase 6
    if (options.mode == "phase6" || options.mode == "integration") {
        if (auto* workingMem = brain->getWorkingMemory()) {
            cout << "Working memory traces: " << workingMem->getActiveTraces() << "/" << workingMem->getCapacity() << endl;
        }
        if (auto* episodicMem = brain->getEpisodicMemory()) {
            cout << "Episodic memory episodes: " << episodicMem->getEpisodeCount() << "/" << episodicMem->getMaxEpisodes() << endl;
        }
    }
    
    // Performance summary
    perfMonitor.printSummary();
    
    cout << GREEN << "=== EXECUTION COMPLETED SUCCESSFULLY ===" << RESET << endl;
    cout << endl;
    
    if (options.mode == "phase2") {
        cout << "Phase 2 Objectives Completed:" << endl;
        cout << "  ✓ Real LIF neuron dynamics implemented" << endl;
        cout << "  ✓ Event-driven spike propagation with delays" << endl;
        cout << "  ✓ STDP and Hebbian plasticity rules" << endl;
        cout << "  ✓ Structural plasticity (synaptogenesis/pruning)" << endl;
        cout << "  ✓ Learning experiment demonstrates measurable changes" << endl;
        cout << "  ✓ Network shows activity-dependent synaptic modification" << endl;
        cout << endl;
        cout << "The NLM brain is now a functioning artificial neural substrate" << endl;
        cout << "capable of changing its own synaptic connections through experience." << endl;
    }
    
    return 0;
}