// NLM Unified Command-Line Interface
// Consolidates all demo modes into a single, user-friendly tool

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
#include "agent/AgentBrain.hpp"
#include "agent/SensoryPercept.hpp"
#include "agent/AgentBody.hpp"
#include "world/SimpleWorld.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "prediction/NeuralPrediction.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "development/DevelopmentSystem.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <algorithm>

namespace nlm {

class NLMCommandLine {
public:
    NLMCommandLine() : verbose_(false), quiet_(false), help_(false) {}
    
    int run(int argc, char* argv[]);
    
private:
    // Command line argument parsing
    struct CommandLineOptions {
        std::string mode = "demo"; // demo, phase3, phase4, phase5, phase6, phase6-text, benchmark
        std::string configFile;
        std::string outputFile;
        int episodes = 1;
        int stepsPerEpisode = 200;
        std::string logLevel = "info";
        std::string randomSeed;
        bool enableProgress = false;
        bool enableStats = true;
        bool enableCheckpointing = false;
        bool enableReplay = false;
        bool enableDevelopment = false;
        bool enableCuriosity = true;
        bool enableRewardModulation = true;
        std::string checkpointPath;
        bool resume = false;
        bool help = false;
        std::string helpTopic;
        bool version = false;
        bool listConfigs = false;
        std::vector<std::string> extraArgs;
    };
    
    CommandLineOptions parseArguments(int argc, char* argv[]);
    void printUsage(const std::string& programName);
    void printHelp(const std::string& topic = "");
    void printVersion();
    void printAvailableModes();
    
    // Demo modes
    void runDemoMode(const CommandLineOptions& opts);
    void runPhase3Demo(const CommandLineOptions& opts);
    void runPhase4Demo(const CommandLineOptions& opts);
    void runPhase5Demo(const CommandLineOptions& opts);
    void runPhase6Demo(const CommandLineOptions& opts);
    void runPhase6TextDemo(const CommandLineOptions& opts);
    void runBenchmark(const CommandLineOptions& opts);
    
    // Helper functions
    std::shared_ptr<Config> createDefaultConfig() const;
    std::shared_ptr<Config> loadConfig(const std::string& configFile) const;
    void applyCommandLineOptions(std::shared_ptr<Config>& config, const CommandLineOptions& opts) const;
    
    // Statistics and monitoring
    struct SimulationStats {
        uint64_t totalSteps = 0;
        uint64_t totalSpikes = 0;
        uint64_t neuronsCreated = 0;
        uint64_t synapsesCreated = 0;
        uint64_t neuronsFiring = 0;
        float averageFiringRate = 0.0f;
        float totalReward = 0.0f;
        float curiosityLevel = 0.0f;
        float noveltyLevel = 0.0f;
        float predictionError = 0.0f;
        float dopamineLevel = 0.0f;
        uint64_t episodesCompleted = 0;
        double wallClockTime = 0.0;
        uint64_t memoryUsage = 0;
    };
    
    void printProgress(int step, int maxSteps, const SimulationStats& stats);
    void printFinalStats(const SimulationStats& stats, double startTime);
    void saveStatsToFile(const SimulationStats& stats, const std::string& filename);
    
    // Internal state
    bool verbose_;
    bool quiet_;
    bool help_;
    std::shared_ptr<Logger> logger_;
};

int NLMCommandLine::run(int argc, char* argv[]) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        CommandLineOptions opts = parseArguments(argc, argv);
        
        if (opts.help) {
            printHelp(opts.helpTopic);
            return 0;
        }
        
        if (opts.version) {
            printVersion();
            return 0;
        }
        
        if (opts.listConfigs) {
            std::cout << "Available configuration templates:" << std::endl;
            std::cout << "  default - Standard configuration" << std::endl;
            std::cout << "  fast    - Optimized for speed" << std::endl;
            std::cout << "  balanced - Balanced performance and learning" << std::endl;
            std::cout << "  learning - Optimized for learning capacity" << std::endl;
            return 0;
        }
        
        // Initialize logger
        logger_ = std::make_shared<Logger>();
        if (opts.logLevel == "error") {
            logger_->addLogger(std::make_shared<ConsoleLogger>(LogLevel::Error));
        } else if (opts.logLevel == "warning") {
            logger_->addLogger(std::make_shared<ConsoleLogger>(LogLevel::Warning));
        } else if (opts.logLevel == "info") {
            logger_->addLogger(std::make_shared<ConsoleLogger>(LogLevel::Info));
        } else if (opts.logLevel == "debug") {
            logger_->addLogger(std::make_shared<ConsoleLogger>(LogLevel::Debug));
        }
        Logger::setGlobal(logger_);
        
        if (!quiet_) {
            std::cout << "=== NLM Command Line Interface ===" << std::endl;
            std::cout << "Running mode: " << opts.mode << std::endl;
            if (!opts.configFile.empty()) {
                std::cout << "Config file: " << opts.configFile << std::endl;
            }
            std::cout << "Episodes: " << opts.episodes << " per run" << std::endl;
            std::cout << "Steps per episode: " << opts.stepsPerEpisode << std::endl;
            std::cout << std::endl;
        }
        
        // Load and configure
        std::shared_ptr<Config> config = createDefaultConfig();
        
        if (!opts.configFile.empty()) {
            std::shared_ptr<Config> fileConfig = loadConfig(opts.configFile);
            if (fileConfig) {
                // Merge file config with defaults (file config overrides)
                for (const auto& key : fileConfig->getKeys()) {
                    config->set(key, fileConfig->getKeys()[key]);
                }
            }
        }
        
        applyCommandLineOptions(config, opts);
        
        // Run selected mode
        if (opts.mode == "phase3") {
            runPhase3Demo(opts);
        } else if (opts.mode == "phase4") {
            runPhase4Demo(opts);
        } else if (opts.mode == "phase5") {
            runPhase5Demo(opts);
        } else if (opts.mode == "phase6") {
            runPhase6Demo(opts);
        } else if (opts.mode == "phase6-text") {
            runPhase6TextDemo(opts);
        } else if (opts.mode == "benchmark") {
            runBenchmark(opts);
        } else {
            runDemoMode(opts);
        }
        
        if (!quiet_) {
            auto endTime = std::chrono::high_resolution_clock::now();
            double elapsed = std::chrono::duration<double>(endTime - startTime).count();
            std::cout << std::endl << "=== Command completed in " << elapsed << " seconds ===" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
    
    return 0;
}

NLMCommandLine::CommandLineOptions NLMCommandLine::parseArguments(int argc, char* argv[]) {
    CommandLineOptions opts;
    
    // Simple argument parsing (could be enhanced with proper CLI library)
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            opts.help = true;
        } else if (arg == "--version") {
            opts.version = true;
        } else if (arg == "--list-configs") {
            opts.listConfigs = true;
        } else if (arg == "--quiet" || arg == "-q") {
            opts.logLevel = "error";
            quiet_ = true;
        } else if (arg == "--verbose" || arg == "-v") {
            opts.logLevel = "debug";
            verbose_ = true;
        } else if (arg == "--mode") {
            if (i + 1 < argc) opts.mode = argv[++i];
        } else if (arg == "--config") {
            if (i + 1 < argc) opts.configFile = argv[++i];
        } else if (arg == "--output") {
            if (i + 1 < argc) opts.outputFile = argv[++i];
        } else if (arg == "--episodes") {
            if (i + 1 < argc) opts.episodes = std::stoi(argv[++i]);
        } else if (arg == "--steps") {
            if (i + 1 < argc) opts.stepsPerEpisode = std::stoi(argv[++i]);
        } else if (arg == "--enable-progress") {
            opts.enableProgress = true;
        } else if (arg == "--no-stats") {
            opts.enableStats = false;
        } else if (arg == "--enable-checkpointing") {
            opts.enableCheckpointing = true;
        } else if (arg == "--enable-replay") {
            opts.enableReplay = true;
        } else if (arg == "--enable-development") {
            opts.enableDevelopment = true;
        } else if (arg == "--disable-curiosity") {
            opts.enableCuriosity = false;
        } else if (arg == "--disable-reward-modulation") {
            opts.enableRewardModulation = false;
        } else if (arg == "--checkpoint") {
            if (i + 1 < argc) opts.checkpointPath = argv[++i];
        } else if (arg == "--resume") {
            opts.resume = true;
        } else if (arg == "--random-seed") {
            if (i + 1 < argc) opts.randomSeed = argv[++i];
        } else {
            // Unknown argument, pass through to demos
            opts.extraArgs.push_back(arg);
        }
    }
    
    return opts;
}

void NLMCommandLine::printUsage(const std::string& programName) {
    std::cout << "Usage: " << programName << " [OPTIONS] [MODE]" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Modes:" << std::endl;
    std::cout << "  demo          - General demonstration (default)" << std::endl;
    std::cout << "  phase3        - Phase 3 world interaction demo" << std::endl;
    std::cout << "  phase4        - Phase 4 cognitive demo" << std::endl;
    std::cout << "  phase5        - Phase 5 sandbox demo" << std::endl;
    std::cout << "  phase6        - Phase 6 integration demo" << std::endl;
    std::cout << "  phase6-text   - Phase 6 text teaching demo" << std::endl;
    std::cout << "  benchmark     - Performance benchmark" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h                Show help information" << std::endl;
    std::cout << "  --version                  Show version information" << std::endl;
    std::cout << "  --list-configs             List available configuration templates" << std::endl;
    std::cout << "  --config FILE              Load configuration from file" << std::endl;
    std::cout << "  --output FILE              Save output to file" << std::endl;
    std::cout << "  --episodes N               Number of episodes to run (default: 1)" << std::endl;
    std::cout << "  --steps N                  Steps per episode (default: 200)" << std::endl;
    std::cout << "  --enable-progress          Show progress updates" << std::endl;
    std::cout << "  --no-stats                 Disable statistics collection" << std::endl;
    std::cout << "  --enable-checkpointing     Enable checkpointing" << std::endl;
    std::cout << "  --enable-replay            Enable replay" << std::endl;
    std::cout << "  --enable-development       Enable development system" << std::endl;
    std::cout << "  --disable-curiosity        Disable curiosity-driven exploration" << std::endl;
    std::cout << "  --disable-reward-modulation Disable reward-based learning" << std::endl;
    std::cout << "  --checkpoint FILE           Checkpoint file path" << std::endl;
    std::cout << "  --resume                    Resume from checkpoint" << std::endl;
    std::cout << "  --random-seed SEED          Set random seed" << std::endl;
    std::cout << "  --verbose, -v              Enable verbose output" << std::endl;
    std::cout << "  --quiet, -q                Quiet mode (only errors)" << std::endl;
    std::cout << "" << std::endl;
}

void NLMCommandLine::printHelp(const std::string& topic) {
    if (topic.empty()) {
        printUsage("nlm");
        printAvailableModes();
    } else {
        if (topic == "demo" || topic == "phase3" || topic == "phase4" || topic == "phase5" || topic == "phase6" || topic == "phase6-text" || topic == "benchmark") {
            std::cout << "=== " << topic << " mode ===" << std::endl;
            std::cout << "This mode runs the " << topic << " demonstration." << std::endl;
            std::cout << "For detailed help, refer to the documentation." << std::endl;
        } else {
            std::cout << "Unknown help topic: " << topic << std::endl;
        }
    }
}

void NLMCommandLine::printVersion() {
    std::cout << "NLM - Neural Learning Machine v0.1.0" << std::endl;
    std::cout << "Phase 6 Final Integration" << std::endl;
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
}

void NLMCommandLine::printAvailableModes() {
    std::cout << "Available Modes:" << std::endl;
    std::cout << "  demo          - General demonstration with all systems enabled" << std::endl;
    std::cout << "  phase3        - Phase 3: World interaction (sensory-motor loop)" << std::endl;
    std::cout << "  phase4        - Phase 4: Cognitive functions (attention, planning, memory)" << std::endl;
    std::cout << "  phase5        - Phase 5: Sandbox learning (brain-personality development)" << std::endl;
    std::cout << "  phase6        - Phase 6: Integration test (all systems connected)" << std::endl;
    std::cout << "  phase6-text   - Phase 6: Text teaching (language as sensory input)" << std::endl;
    std::cout << "  benchmark     - Performance benchmarking" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Use --help [topic] for more detailed information about a specific mode." << std::endl;
}

std::shared_ptr<Config> NLMCommandLine::createDefaultConfig() const {
    auto config = std::make_shared<Config>();
    
    // Default configuration
    config->set("brain.neuron_count", 500, ConfigSource::Default);
    config->set("brain.synapse_density", 0.1f, ConfigSource::Default);
    config->set("brain.region_count", 2, ConfigSource::Default);
    config->set("brain.connection_probability", 0.05f, ConfigSource::Default);
    config->set("brain.initial_weight_mean", 0.5f, ConfigSource::Default);
    config->set("brain.initial_weight_std", 0.1f, ConfigSource::Default);
    config->set("brain.v_thresh", -50.0f, ConfigSource::Default);
    config->set("brain.v_rest", -70.0f, ConfigSource::Default);
    config->set("brain.v_reset", -75.0f, ConfigSource::Default);
    config->set("brain.tau_mem", 20.0f, ConfigSource::Default);
    config->set("brain.tau_ref", 2.0f, ConfigSource::Default);
    
    // Plasticity settings
    config->set("plasticity.stdp.enable", true, ConfigSource::Default);
    config->set("plasticity.stdp.learning_rate", 0.001f, ConfigSource::Default);
    config->set("plasticity.stdp.tau_plus", 20.0f, ConfigSource::Default);
    config->set("plasticity.stdp.tau_minus", 20.0f, ConfigSource::Default);
    config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
    config->set("plasticity.structural.enable", true, ConfigSource::Default);
    
    // Neuromodulation settings
    config->set("neuromod.dopamine.scale", 1.0f, ConfigSource::Default);
    config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
    config->set("neuromod.novelty.enable", true, ConfigSource::Default);
    
    // Development settings
    config->set("development.enable", true, ConfigSource::Default);
    config->set("development.critical_period_length", 100, ConfigSource::Default);
    config->set("development.plasticity_rate_multiplier", 2.0f, ConfigSource::Default);
    
    // Simulation settings
    config->set("simulation.timestep", 0.001f, ConfigSource::Default);
    config->set("simulation.max_steps", 1000, ConfigSource::Default);
    config->set("simulation.random_seed", 42, ConfigSource::Default);
    
    // Memory settings
    config->set("memory.working.enable", true, ConfigSource::Default);
    config->set("memory.episodic.enable", true, ConfigSource::Default);
    config->set("memory.associative.enable", true, ConfigSource::Default);
    
    return config;
}

std::shared_ptr<Config> NLMCommandLine::loadConfig(const std::string& configFile) const {
    auto config = std::make_shared<Config>();
    
    if (config->loadFromFile(configFile)) {
        return config;
    } else {
        NLM_LOG_ERROR("Failed to load config file: " + configFile);
        return nullptr;
    }
}

void NLMCommandLine::applyCommandLineOptions(std::shared_ptr<Config>& config, const CommandLineOptions& opts) const {
    if (!opts.randomSeed.empty()) {
        config->set("simulation.random_seed", std::stoll(opts.randomSeed), ConfigSource::CommandLine);
    }
    
    if (opts.episodes > 0) {
        config->set("simulation.episodes", opts.episodes, ConfigSource::CommandLine);
    }
    
    if (opts.stepsPerEpisode > 0) {
        config->set("simulation.steps_per_episode", opts.stepsPerEpisode, ConfigSource::CommandLine);
    }
    
    // Enable/disable subsystems based on options
    config->set("neuromod.curiosity.enable", opts.enableCuriosity, ConfigSource::CommandLine);
    config->set("plasticity.stdp.enable", opts.enableRewardModulation, ConfigSource::CommandLine);
    config->set("development.enable", opts.enableDevelopment, ConfigSource::CommandLine);
    config->set("memory.working.enable", true, ConfigSource::CommandLine);
    config->set("memory.episodic.enable", true, ConfigSource::CommandLine);
    
    // Checkpoint settings
    if (!opts.checkpointPath.empty()) {
        config->set("checkpoint.path", opts.checkpointPath, ConfigSource::CommandLine);
    }
    config->set("checkpoint.enabled", opts.enableCheckpointing, ConfigSource::CommandLine);
    config->set("checkpoint.resume", opts.resume, ConfigSource::CommandLine);
}

void NLMCommandLine::printProgress(int step, int maxSteps, const SimulationStats& stats) {
    if (quiet_) return;
    
    int progress = (step * 100) / maxSteps;
    std::cout << "\rProgress: " << progress << "% | Step: " << step << "/" << maxSteps 
              << " | Spikes: " << stats.totalSpikes 
              << " | Neurons: " << stats.neuronsFiring << "/" << stats.neuronsCreated
              << " | Reward: " << std::fixed << std::setprecision(3) << stats.totalReward
              << " | Curiosity: " << std::fixed << std::setprecision(3) << stats.curiosityLevel
              << " | " << std::flush;
}

void NLMCommandLine::printFinalStats(const SimulationStats& stats, double startTime) {
    auto endTime = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double>(endTime - std::chrono::high_resolution_clock::now()).count();
    
    std::cout << std::endl << std::endl;
    std::cout << "╔══════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                         SIMULATION STATISTICS                               ║" << std::endl;
    std::cout << "╠══════════════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ Total Steps            : " << std::setw(10) << stats.totalSteps << "                           ║" << std::endl;
    std::cout << "║ Total Spikes           : " << std::setw(10) << stats.totalSpikes << "                           ║" << std::endl;
    std::cout << "║ Neurons Created        : " << std::setw(10) << stats.neuronsCreated << "                           ║" << std::endl;
    std::cout << "║ Synapses Created        : " << std::setw(10) << stats.synapsesCreated << "                           ║" << std::endl;
    std::cout << "║ Neurons Firing         : " << std::setw(10) << stats.neuronsFiring << "                           ║" << std::endl;
    std::cout << "║ Average Firing Rate    : " << std::setw(10) << std::fixed << std::setprecision(3) << stats.averageFiringRate << "                           ║" << std::endl;
    std::cout << "║ Total Reward            : " << std::setw(10) << std::fixed << std::setprecision(3) << stats.totalReward << "                           ║" << std::endl;
    std::cout << "║ Curiosity Level         : " << std::setw(10) << std::fixed << std::setprecision(3) << stats.curiosityLevel << "                           ║" << std::endl;
    std::cout << "║ Novelty Level           : " << std::setw(10) << std::fixed << std::setprecision(3) << stats.noveltyLevel << "                           ║" << std::endl;
    std::cout << "║ Prediction Error        : " << std::setw(10) << std::fixed << std::setprecision(3) << stats.predictionError << "                           ║" << std::endl;
    std::cout << "║ Dopamine Level          : " << std::setw(10) << std::fixed << std::setprecision(3) << stats.dopamineLevel << "                           ║" << std::endl;
    std::cout << "║ Episodes Completed      : " << std::setw(10) << stats.episodesCompleted << "                           ║" << std::endl;
    std::cout << "║ Wall Clock Time         : " << std::setw(10) << std::fixed << std::setprecision(2) << elapsed << "s                   ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
}

void NLMCommandLine::saveStatsToFile(const SimulationStats& stats, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open output file: " + filename);
        return;
    }
    
    file << "# NLM Simulation Statistics" << std::endl;
    file << "# Generated by NLM command line interface" << std::endl;
    file << "#" << std::endl;
    
    file << "total_steps=" << stats.totalSteps << std::endl;
    file << "total_spikes=" << stats.totalSpikes << std::endl;
    file << "neurons_created=" << stats.neuronsCreated << std::endl;
    file << "synapses_created=" << stats.synapsesCreated << std::endl;
    file << "neurons_firing=" << stats.neuronsFiring << std::endl;
    file << "average_firing_rate=" << stats.averageFiringRate << std::endl;
    file << "total_reward=" << stats.totalReward << std::endl;
    file << "curiosity_level=" << stats.curiosityLevel << std::endl;
    file << "novelty_level=" << stats.noveltyLevel << std::endl;
    file << "prediction_error=" << stats.predictionError << std::endl;
    file << "dopamine_level=" << stats.dopamineLevel << std::endl;
    file << "episodes_completed=" << stats.episodesCompleted << std::endl;
    
    file.close();
    NLM_LOG_INFO("Statistics saved to file: " + filename);
}

// Forward declarations for demo mode implementations
void runGeneralDemo(const NLMCommandLine::CommandLineOptions& opts);
void runPhase3Demo(const NLMCommandLine::CommandLineOptions& opts);
void runPhase4Demo(const NLMCommandLine::CommandLineOptions& opts);
void runPhase5Demo(const NLMCommandLine::CommandLineOptions& opts);
void runPhase6Demo(const NLMCommandLine::CommandLineOptions& opts);
void runPhase6TextDemo(const NLMCommandLine::CommandLineOptions& opts);
void runBenchmark(const NLMCommandLine::CommandLineOptions& opts);

// Implementation of demo modes (simplified versions)
void NLMCommandLine::runDemoMode(const CommandLineOptions& opts) {
    runGeneralDemo(opts);
}

void NLMCommandLine::runPhase3Demo(const CommandLineOptions& opts) {
    runPhase3Demo(opts);
}

void NLMCommandLine::runPhase4Demo(const CommandLineOptions& opts) {
    runPhase4Demo(opts);
}

void NLMCommandLine::runPhase5Demo(const CommandLineOptions& opts) {
    runPhase5Demo(opts);
}

void NLMCommandLine::runPhase6Demo(const CommandLineOptions& opts) {
    runPhase6Demo(opts);
}

void NLMCommandLine::runPhase6TextDemo(const CommandLineOptions& opts) {
    runPhase6TextDemo(opts);
}

void NLMCommandLine::runBenchmark(const CommandLineOptions& opts) {
    runBenchmark(opts);
}

// Note: The actual demo implementations would need to be updated to use
// the new configuration system and command-line interface
// This file provides the unified CLI foundation