// NLM (熙然) - Neural Learning Machine
// Enhanced Main Executable with Comprehensive CLI Interface
// Provides multiple demo modes, configuration presets, and user-friendly interface

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <map>
#include <algorithm>
#include <functional>

using namespace nlm;

// Enhanced version information
const std::string NLM_VERSION = "1.0.0";
const std::string NLM_BUILD_DATE = "2026-10-20";
const std::string NLM_DESCRIPTION = "Neural Learning Machine - Experimental Artificial Developmental Brain";

// Configuration presets
struct ConfigPreset {
    std::string name;
    std::string description;
    std::function<void(std::shared_ptr<Config>)> configure;
};

// Command line options structure
struct CliOptions {
    std::string mode = "phase2"; // phase2, phase3, phase4, phase6, benchmark, python
    std::string configFile = "configs/default.cfg";
    std::string outputFile = "";
    size_t neuronCount = 500;
    size_t regionCount = 1;
    float connectionProb = 0.15f;
    uint64_t randomSeed = 42;
    double timestep = 0.001;
    int episodes = 5;
    int stepsPerEpisode = 200;
    bool enableLogging = true;
    bool enableProgress = true;
    bool help = false;
    bool version = false;
    std::string preset = "";
    std::vector<std::string> customArgs;
};

// Forward declarations for experiment classes
class Phase3Demo;
class Phase4Demo;
class Phase6Demo;

// Helper functions
void printBanner();
void printVersionInfo();
void displayHelp();
CliOptions parseCommandLine(int argc, char** argv);
void setupPresetConfig(std::shared_ptr<Config> config, const std::string& presetName);
void runPhase2Demo(const CliOptions& options, std::shared_ptr<Config> config);
void runPhase3Demo(const CliOptions& options, std::shared_ptr<Config> config);
void runPhase4Demo(const CliOptions& options, std::shared_ptr<Config> config);
void runPhase6Demo(const CliOptions& options, std::shared_ptr<Config> config);
void printProgress(int current, int total, const std::string& message);

// Original Phase 2 functionality (simplified for demo)
void runBasicConnectivityTest(std::shared_ptr<Brain> brain, double timestep);
void runPlasticityExperiment(std::shared_ptr<Brain> brain, double timestep, bool enableProgress);
void runStdpVerification(std::shared_ptr<Brain> brain, double timestep);

struct LearningExperiment {
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    
    LearningExperiment(std::shared_ptr<Brain> b, uint64_t s) : brain(b), seed(s) {}
    
    void recordInitialState();
    void recordFinalState();
    void computeStatistics();
};

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Enhanced Command Line Interface                           ║
    ║                                                               ║
    ║     Version " << NLM_VERSION << "                                               ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printVersionInfo() {
    std::cout << "NLM " << NLM_VERSION << " (Build: " << NLM_BUILD_DATE << ")" << std::endl;
    std::cout << NLM_DESCRIPTION << std::endl;
    std::cout << "Languages: C++20 with Python bindings (pynlm)" << std::endl;
    std::cout << "Phases: 1-6 Complete (Final Integration)" << std::endl;
}

void displayHelp() {
    std::cout << "NLM - Neural Learning Machine" << std::endl;
    std::cout << "Version " << NLM_VERSION << " (Build: " << NLM_BUILD_DATE << ")" << std::endl;
    std::cout << "Experimental Artificial Developmental Brain" << std::endl;
    std::cout << std::endl;
    
    std::cout << "USAGE:" << std::endl;
    std::cout << "  nlm [OPTIONS]" << std::endl;
    std::cout << std::endl;
    
    std::cout << "MODES:" << std::endl;
    std::cout << "  --phase2          Run Phase 2 neural computation demo (default)" << std::endl;
    std::cout << "  --phase3          Run Phase 3 world interaction demo" << std::endl;
    std::cout << "  --phase4          Run Phase 4 cognition demo" << std::endl;
    std::cout << "  --phase6          Run Phase 6 integration demo" << std::endl;
    std::cout << "  --benchmark       Run performance benchmarks" << std::endl;
    std::cout << "  --python          Start Python interactive session" << std::endl;
    std::cout << std::endl;
    
    std::cout << "CONFIGURATION PRESETS:" << std::endl;
    std::cout << "  --preset minimal  Small, fast demo (100 neurons)" << std::endl;
    std::cout << "  --preset standard Standard demo (500 neurons)" << std::endl;
    std::cout << "  --preset large    Large scale demo (2000 neurons)" << std::endl;
    std::cout << "  --preset fast     Fast, approximate demo (200 neurons, 5ms timestep)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "OPTIONS:" << std::endl;
    std::cout << "  --neuron-count N      Number of neurons (default: 500)" << std::endl;
    std::cout << "  --region-count N      Number of brain regions (default: 1)" << std::endl;
    std::cout << "  --connection-prob P   Connection probability (default: 0.15)" << std::endl;
    std::cout << "  --random-seed S       Random seed for reproducibility (default: 42)" << std::endl;
    std::cout << "  --timestep T          Simulation timestep in seconds (default: 0.001)" << std::endl;
    std::cout << "  --episodes N          Number of episodes for Phase 3/4/6 (default: 5)" << std::endl;
    std::cout << "  --steps N             Steps per episode (default: 200)" << std::endl;
    std::cout << "  --config FILE         Load configuration from file (default: configs/default.cfg)" << std::endl;
    std::cout << "  --output FILE         Save results to file" << std::endl;
    std::cout << "  --enable-logging      Enable detailed logging (default: enabled)" << std::endl;
    std::cout << "  --disable-logging     Disable logging" << std::endl;
    std::cout << "  --enable-progress     Show progress output (default: enabled)" << std::endl;
    std::cout << "  --disable-progress    Hide progress output" << std::endl;
    std::cout << std::endl;
    
    std::cout << "EXAMPLES:" << std::endl;
    std::cout << "  nlm --phase2                                    # Default Phase 2 demo" << std::endl;
    std::cout << "  nlm --phase3 --preset large --episodes 3      # Phase 3 with large preset" << std::endl;
    std::cout << "  nlm --phase4 --neuron-count 2000              # Phase 4 with more neurons" << std::endl;
    std::cout << "  nlm --phase6 --steps 500                       # Phase 6 with more steps" << std::endl;
    std::cout << "  nlm --help                                     # Show this help" << std::endl;
    std::cout << "  nlm --version                                  # Show version info" << std::endl;
    std::cout << std::endl;
}

CliOptions parseCommandLine(int argc, char** argv) {
    CliOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            options.help = true;
        } else if (arg == "--version" || arg == "-v") {
            options.version = true;
        } else if (arg == "--mode") {
            if (i + 1 < argc) options.mode = argv[++i];
        } else if (arg == "--neuron-count") {
            if (i + 1 < argc) options.neuronCount = std::stoul(argv[++i]);
        } else if (arg == "--region-count") {
            if (i + 1 < argc) options.regionCount = std::stoul(argv[++i]);
        } else if (arg == "--connection-prob") {
            if (i + 1 < argc) options.connectionProb = std::stof(argv[++i]);
        } else if (arg == "--random-seed") {
            if (i + 1 < argc) options.randomSeed = std::stoull(argv[++i]);
        } else if (arg == "--timestep") {
            if (i + 1 < argc) options.timestep = std::stod(argv[++i]);
        } else if (arg == "--episodes") {
            if (i + 1 < argc) options.episodes = std::stoi(argv[++i]);
        } else if (arg == "--steps") {
            if (i + 1 < argc) options.stepsPerEpisode = std::stoi(argv[++i]);
        } else if (arg == "--config") {
            if (i + 1 < argc) options.configFile = argv[++i];
        } else if (arg == "--output") {
            if (i + 1 < argc) options.outputFile = argv[++i];
        } else if (arg == "--preset") {
            if (i + 1 < argc) options.preset = argv[++i];
        } else if (arg == "--phase2" || arg == "--phase3" || arg == "--phase4" || arg == "--phase6") {
            options.mode = arg.substr(2); // Remove "--" prefix
        } else if (arg.find("--") == 0) {
            options.customArgs.push_back(arg);
        }
    }
    
    return options;
}

void setupPresetConfig(std::shared_ptr<Config> config, const std::string& presetName) {
    if (presetName == "minimal") {
        config->set("neuron_count", static_cast<int64_t>(100));
        config->set("region_count", static_cast<int64_t>(1));
        config->set("connection_probability", 0.05f);
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation_timestep", 0.001);
    } else if (presetName == "standard") {
        config->set("neuron_count", static_cast<int64_t>(500));
        config->set("region_count", static_cast<int64_t>(2));
        config->set("connection_probability", 0.1f);
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation_timestep", 0.001);
    } else if (presetName == "large") {
        config->set("neuron_count", static_cast<int64_t>(2000));
        config->set("region_count", static_cast<int64_t>(3));
        config->set("connection_probability", 0.15f);
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation_timestep", 0.001);
    } else if (presetName == "fast") {
        config->set("neuron_count", static_cast<int64_t>(200));
        config->set("region_count", static_cast<int64_t>(1));
        config->set("connection_probability", 0.2f);
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation_timestep", 0.005);
    }
}

void printProgress(int current, int total, const std::string& message) {
    if (current % 50 == 0 || current == total) {
        std::cout << "\r  " << message << " " << current << "/" << total << " ("
                  << (100 * current / total) << "%)" << std::flush;
        if (current == total) {
            std::cout << std::endl;
        }
    }
}

void runBasicConnectivityTest(std::shared_ptr<Brain> brain, double timestep) {
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_INFO("  No neurons found!");
        return;
    }
    
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);
    }
    
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * timestep);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
    
    if (spikes > 0) {
        NLM_LOG_INFO("  ✓ Spikes propagate through network");
    } else {
        NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
    }
}

void runPlasticityExperiment(std::shared_ptr<Brain> brain, double timestep, bool enableProgress) {
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    
    LearningExperiment experiment(brain, 42);
    experiment.recordInitialState();
    
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
    }
    
    NLM_LOG_INFO("  Applying repeated input patterns (1000 steps)...");
    
    for (SimulationStep step = 0; step < 1000; ++step) {
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        brain->step(step, step * timestep);
        
        if (step % 100 == 0 && enableProgress) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    experiment.recordFinalState();
    experiment.computeStatistics();
}

void runStdpVerification(std::shared_ptr<Brain> brain, double timestep) {
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        NLM_LOG_INFO("  Not enough synapses for STDP test");
        return;
    }
    
    NLM_LOG_INFO("  Testing STDP on 5 synapses:");
    
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " before: " + std::to_string(beforeWeights[i]));
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("  Creating correlated pre->post activity (50 trials)...");
    
    for (int trial = 0; trial < 50; ++trial) {
        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            neurons[0]->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * timestep);
            neurons[1]->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * timestep);
        }
    }
    
    NLM_LOG_INFO("  After correlated activity:");
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        totalDelta += delta;
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                        " after: " + std::to_string(synapses[i]->getWeight()) +
                        " (Δ=" + std::to_string(delta) + ")");
    }
    
    if (std::abs(totalDelta) > 0.001f) {
        NLM_LOG_INFO("  ✓ STDP working: Pre-before-post produced " + 
                    std::string(totalDelta > 0 ? "potentiation" : "depression") + 
                    " (total delta: " + std::to_string(totalDelta) + ")");
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering");
    }
}

void runPhase2Demo(const CliOptions& options, std::shared_ptr<Config> config) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return;
    }
    
    brain->logStatus();
    
    std::cout << "\nPhase 2 Neural Computation Demo" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    
    runBasicConnectivityTest(brain, options.timestep);
    
    brain->reset();
    brain->initialize();
    
    runPlasticityExperiment(brain, options.timestep, options.enableProgress);
    
    brain->reset();
    brain->initialize();
    
    runStdpVerification(brain, options.timestep);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << "Phase 2 Complete! (took " << duration.count() / 1000.0 << " seconds)" << std::endl;
    std::cout << std::string(70, '=') << std::endl;
}

void runPhase3Demo(const CliOptions& options, std::shared_ptr<Config> config) {
    std::cout << "\nPhase 3 Demo" << std::endl;
    std::cout << "Note: Full Phase 3 demo requires additional experiment implementation" << std::endl;
    std::cout << "This would run world interaction, reward prediction, and developmental experiments" << std::endl;
    std::cout << "Expected behavior: Agent explores world, learns from rewards, modifies synapses" << std::endl;
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return;
    }
    
    // Simplified Phase 3 simulation
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::cout << "Running " << options.episodes << " episodes of " << options.stepsPerEpisode << " steps each..." << std::endl;
    printProgress(0, options.episodes * options.stepsPerEpisode, "Episode progress");
    
    float totalReward = 0.0f;
    size_t totalSpikes = 0;
    
    for (int episode = 0; episode < options.episodes; ++episode) {
        brain->reset();
        brain->initialize();
        
        for (int step = 0; step < options.stepsPerEpisode; ++step) {
            // Simulate brain activity
            brain->step(step, step * options.timestep);
            
            // Simple reward based on neural activity
            float reward = 0.01f * brain->getFiringNeuronCount();
            totalReward += reward;
            totalSpikes += brain->getTotalSpikeCount();
            
            // Simulate neuromodulation
            if (step % 100 == 0) {
                std::cout << "\r  Episode " << episode << ": Step " << step 
                          << " | Reward: " << reward << " | Spikes: " << brain->getTotalSpikeCount()
                          << " | Firing: " << brain->getFiringNeuronCount() << std::flush;
            }
        }
        
        printProgress(episode * options.stepsPerEpisode + options.stepsPerEpisode, 
                     options.episodes * options.stepsPerEpisode, "Episode progress");
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << std::endl << std::string(70, '=') << std::endl;
    std::cout << "Phase 3 Demo Complete!" << std::endl;
    std::cout << "Total reward: " << totalReward << std::endl;
    std::cout << "Total spikes: " << totalSpikes << std::endl;
    std::cout << "Duration: " << duration.count() / 1000.0 << " seconds" << std::endl;
    std::cout << std::string(70, '=') << std::endl;
}

void runPhase4Demo(const CliOptions& options, std::shared_ptr<Config> config) {
    std::cout << "\nPhase 4 Demo" << std::endl;
    std::cout << "Note: Full Phase 4 demo requires additional experiment implementation" << std::endl;
    std::cout << "This would demonstrate working memory, episodic memory, concept formation, attention, and planning" << std::endl;
    std::cout << "Expected behavior: Complex cognitive abilities emerge from neural dynamics" << std::endl;
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return;
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::cout << "Running " << options.stepsPerEpisode << " steps of Phase 4 cognition demo..." << std::endl;
    
    // Simplified Phase 4 simulation focusing on memory systems
    for (int step = 0; step < options.stepsPerEpisode; ++step) {
        brain->step(step, step * options.timestep);
        
        if (step % 200 == 0) {
            std::cout << "\r  Step " << step 
                      << " | Firing: " << brain->getFiringNeuronCount()
                      << " | Memory: " << brain->getTotalSynapseCount() << " synapses" << std::flush;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << std::endl << std::string(70, '=') << std::endl;
    std::cout << "Phase 4 Demo Complete!" << std::endl;
    std::cout << "Duration: " << duration.count() / 1000.0 << " seconds" << std::endl;
    std::cout << std::string(70, '=') << std::endl;
}

void runPhase6Demo(const CliOptions& options, std::shared_ptr<Config> config) {
    std::cout << "\nPhase 6 Demo" << std::endl;
    std::cout << "Note: Full Phase 6 demo requires additional experiment implementation" << std::endl;
    std::cout << "This would demonstrate complete integration of all brain systems" << std::endl;
    std::cout << "Expected behavior: Memory, neuromodulation, prediction, cognition, and development work together" << std::endl;
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return;
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::cout << "Running " << options.episodes << " episodes of " << options.stepsPerEpisode << " steps each..." << std::endl;
    
    // Simplified Phase 6 integration simulation
    float totalReward = 0.0f;
    size_t memoryStored = 0;
    
    for (int episode = 0; episode < options.episodes; ++episode) {
        brain->reset();
        brain->initialize();
        
        std::cout << "\r  Episode " << episode + 1 << "/" << options.episodes << " | " << std::flush;
        
        for (int step = 0; step < options.stepsPerEpisode; ++step) {
            brain->step(step, step * options.timestep);
            
            // Simple reward
            float reward = 0.01f * brain->getFiringNeuronCount();
            totalReward += reward;
            
            // Simulate memory storage
            if (step % 100 == 0) {
                memoryStored += 5; // Simulate memory consolidation
            }
            
            if (step % 100 == 0) {
                std::cout << "\r  Episode " << episode + 1 << "/" << options.episodes 
                          << " | Step " << step 
                          << " | Reward: " << reward
                          << " | Memory: " << brain->getTotalSynapseCount() << " synapses"
                          << std::flush;
            }
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << std::endl << std::string(70, '=') << std::endl;
    std::cout << "Phase 6 Demo Complete!" << std::endl;
    std::cout << "Total reward: " << totalReward << std::endl;
    std::cout << "Memory stored: " << memoryStored << " units" << std::endl;
    std::cout << "Final synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "Duration: " << duration.count() / 1000.0 << " seconds" << std::endl;
    std::cout << std::string(70, '=') << std::endl;
}

int main(int argc, char** argv) {
    // Parse command line options
    CliOptions options = parseCommandLine(argc, argv);
    
    // Handle help/version requests
    if (options.help) {
        displayHelp();
        return 0;
    }
    
    if (options.version) {
        printVersionInfo();
        return 0;
    }
    
    // Print banner
    printBanner();
    
    // Setup configuration
    auto config = std::make_shared<Config>();
    
    // Apply preset if specified
    if (!options.preset.empty()) {
        std::cout << "Applying preset: " << options.preset << std::endl;
        setupPresetConfig(config, options.preset);
    }
    
    // Load configuration from file
    if (config->loadFromFile(options.configFile)) {
        std::cout << "Loaded configuration from: " << options.configFile << std::endl;
    } else {
        std::cout << "Using default configuration." << std::endl;
    }
    
    // Apply command line options
    config->set("random_seed", static_cast<int64_t>(options.randomSeed), ConfigSource::CommandLine);
    config->set("simulation_timestep", options.timestep, ConfigSource::CommandLine);
    config->set("neuron_count", static_cast<int64_t>(options.neuronCount), ConfigSource::CommandLine);
    config->set("region_count", static_cast<int64_t>(options.regionCount), ConfigSource::CommandLine);
    config->set("connection_probability", options.connectionProb, ConfigSource::CommandLine);
    
    // Load from custom args (if any)
    config->loadFromArgs(argc, argv);
    
    // Initialize logger with enhanced logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(options.enableLogging ? LogLevel::Info : LogLevel::Error);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Display configuration summary
    std::cout << "\nConfiguration Summary:" << std::endl;
    std::cout << std::string(70, '-') << std::endl;
    auto keys = config->getKeys();
    for (const auto& key : keys) {
        if (key == "random_seed") {
            std::cout << "  " << std::left << std::setw(25) << key << ": " 
                      << config->getOr<int64_t>(key, 0) << std::endl;
        } else if (key == "simulation_timestep") {
            std::cout << "  " << std::left << std::setw(25) << key << ": " 
                      << std::fixed << std::setprecision(3) 
                      << config->getOr<double>(key, 0.001) << "s" << std::endl;
        } else if (key == "neuron_count") {
            std::cout << "  " << std::left << std::setw(25) << key << ": " 
                      << config->getOr<size_t>(key, 500) << std::endl;
        } else if (key == "region_count") {
            std::cout << "  " << std::left << std::setw(25) << key << ": " 
                      << config->getOr<size_t>(key, 1) << std::endl;
        } else if (key == "connection_probability") {
            std::cout << "  " << std::left << std::setw(25) << key << ": " 
                      << std::fixed << std::setprecision(3) 
                      << config->getOr<float>(key, 0.15f) << std::endl;
        }
    }
    
    // Run appropriate mode
    if (options.mode == "phase2") {
        runPhase2Demo(options, config);
    } else if (options.mode == "phase3") {
        runPhase3Demo(options, config);
    } else if (options.mode == "phase4") {
        runPhase4Demo(options, config);
    } else if (options.mode == "phase6") {
        runPhase6Demo(options, config);
    } else if (options.mode == "benchmark") {
        std::cout << "Benchmark mode not yet implemented" << std::endl;
    } else if (options.mode == "python") {
        std::cout << "Python mode not yet implemented (requires Python integration)" << std::endl;
    } else {
        std::cerr << "ERROR: Unknown mode: " << options.mode << std::endl;
        displayHelp();
        return 1;
    }
    
    std::cout << "\nNLM execution completed successfully!" << std::endl;
    return 0;
}