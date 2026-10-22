// NLM (熙然) - Neural Learning Machine
// Enhanced Command Line Interface with Advanced Control
//
// This provides a comprehensive CLI for controlling the NLM brain simulation
// with support for:
// - Multiple execution modes (basic, learning, stdp, full)
// - Configuration file and command-line override
// - Performance profiling and monitoring
// - Checkpoint management
// - Experiment control and result export

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

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <fstream>
#include <map>
#include <algorithm>
#include <ctime>

using namespace nlm;

// Command line options structure
struct CLIConfig {
    std::string configFile = "configs/default.cfg";
    std::string outputFile = "";
    std::string checkpointFile = "";
    std::string experimentFile = "";
    
    std::string mode = "basic"; // basic, learning, stdp, full, profile
    size_t steps = 1000;
    size_t neurons = 500;
    size_t regions = 1;
    float connectionProbability = 0.15f;
    
    bool enableLogging = true;
    bool enableProfiling = false;
    bool enableCheckpoints = false;
    bool enableExport = false;
    
    float stdpLtpWeight = 0.02f;
    float stdpLtdWeight = 0.015f;
    float stdpTau = 20.0f;
    
    float synaptogenesisRate = 0.0001f;
    float pruningRate = 0.00001f;
    
    // Performance monitoring
    bool monitorMemory = false;
    bool monitorCPU = false;
    bool monitorSpikes = true;
    
    // Experiment settings
    size_t learningSteps = 1000;
    size_t plasticitySteps = 500;
    size_t stdpTrials = 50;
    
    void printUsage() const {
        std::cout << "NLM Neural Learning Machine - Advanced CLI Interface\n";
        std::cout << "===================================================\n\n";
        
        std::cout << "Modes:\n";
        std::cout << "  basic     - Run basic connectivity test\n";
        std::cout << "  learning  - Run plasticity learning experiment\n";
        std::cout << "  stdp      - Run STDP verification experiment\n";
        std::cout << "  full      - Run all experiments sequentially\n";
        std::cout << "  profile    - Run with performance profiling\n\n";
        
        std::cout << "Options:\n";
        std::cout << "  --config file.cfg              Load configuration from file\n";
        std::cout << "  --mode MODE                   Set execution mode\n";
        std::cout << "  --steps N                      Number of simulation steps\n";
        std::cout << "  --neurons N                    Number of neurons (default: 500)\n";
        std::cout << "  --regions N                    Number of regions (default: 1)\n";
        std::cout << "  --connection-prob P            Connection probability (default: 0.15)\n";
        std::cout << "  --output file.csv              Export results to CSV\n";
        stdcout << "  --checkpoint file.bin          Enable checkpointing\n";
        stdcout << "  --experiment file.cfg          Load experiment configuration\n";
        stdcout << "  --enable-logging              Enable detailed logging\n";
        stdcout << "  --disable-logging             Disable logging\n";
        stdcout << "  --profile                      Enable performance profiling\n";
        stdcout << "  --help                         Show this help message\n\n";
        
        stdcout << "STDP Parameters:\n";
        stdcout << "  --stdp-ltp-weight W            STDP LTP weight (default: 0.02)\n";
        stdcout << "  --stdp-ltd-weight W            STDP LTD weight (default: 0.015)\n";
        stdcout << "  --stdp-tau T                   STDP time constant (default: 20)\n\n";
        
        stdcout << "Plasticity Parameters:\n";
        stdcout << "  --synaptogenesis-rate R        Synaptogenesis rate (default: 0.0001)\n";
        stdcout << "  --pruning-rate R               Pruning rate (default: 0.00001)\n\n";
    }
    
    bool parseArgs(int argc, char* argv[]) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help") {
                printUsage();
                return false; // Exit after showing help
            }
            else if (arg == "--config" && i + 1 < argc) {
                configFile = argv[++i];
            }
            else if (arg == "--mode" && i + 1 < argc) {
                mode = argv[++i];
                // Validate mode
                if (mode != "basic" && mode != "learning" && mode != "stdp" && 
                    mode != "full" && mode != "profile") {
                    std::cerr << "Error: Invalid mode '" << mode << "'.\n";
                    return false;
                }
            }
            else if (arg == "--steps" && i + 1 < argc) {
                steps = std::stoul(argv[++i]);
            }
            else if (arg == "--neurons" && i + 1 < argc) {
                neurons = std::stoul(argv[++i]);
            }
            else if (arg == "--regions" && i + 1 < argc) {
                regions = std::stoul(argv[++i]);
            }
            else if (arg == "--connection-prob" && i + 1 < argc) {
                connectionProbability = std::stof(argv[++i]);
            }
            else if (arg == "--output" && i + 1 < argc) {
                outputFile = argv[++i];
                enableExport = true;
            }
            else if (arg == "--checkpoint" && i + 1 < argc) {
                checkpointFile = argv[++i];
                enableCheckpoints = true;
            }
            else if (arg == "--experiment" && i + 1 < argc) {
                experimentFile = argv[++i];
            }
            else if (arg == "--enable-logging") {
                enableLogging = true;
            }
            else if (arg == "--disable-logging") {
                enableLogging = false;
            }
            else if (arg == "--profile") {
                enableProfiling = true;
            }
            else if (arg == "--stdp-ltp-weight" && i + 1 < argc) {
                stdpLtpWeight = std::stof(argv[++i]);
            }
            else if (arg == "--stdp-ltd-weight" && i + 1 < argc) {
                stdpLtdWeight = std::stof(argv[++i]);
            }
            else if (arg == "--stdp-tau" && i + 1 < argc) {
                stdpTau = std::stof(argv[++i]);
            }
            else if (arg == "--synaptogenesis-rate" && i + 1 < argc) {
                synaptogenesisRate = std::stof(argv[++i]);
            }
            else if (arg == "--pruning-rate" && i + 1 < argc) {
                pruningRate = std::stof(argv[++i]);
            }
            else {
                std::cerr << "Warning: Unknown argument '" << arg << "' (ignored)\n";
            }
        }
        return true;
    }
};

void printSystemInfo(const CLIConfig& config) {
    std::cout << "NLM Neural Learning Machine - System Configuration\n";
    std::cout << "===================================================\n\n";
    
    std::cout << "Execution Mode: " << config.mode << "\n";
    std::cout << "Simulation Steps: " << config.steps << "\n";
    std::cout << "Neurons: " << config.neurons << "\n";
    std::cout << "Regions: " << config.regions << "\n";
    std::cout << "Connection Probability: " << config.connectionProbability << "\n";
    
    std::cout << "\nSTDP Parameters:\n";
    std::cout << "  LTP Weight: " << config.stdpLtpWeight << "\n";
    std::cout << "  LTD Weight: " << config.stdpLtdWeight << "\n";
    std::cout << "  Time Constant: " << config.stdpTau << " ms\n";
    
    std::cout << "\nPlasticity Parameters:\n";
    std::cout << "  Synaptogenesis Rate: " << config.synaptogenesisRate << "\n";
    std::cout << "  Pruning Rate: " << config.pruningRate << "\n";
    
    std::cout << "\nFeatures Enabled:\n";
    std::cout << "  Logging: " << (config.enableLogging ? "YES" : "NO") << "\n";
    std::cout << "  Profiling: " << (config.enableProfiling ? "YES" : "NO") << "\n";
    std::cout << "  Checkpoints: " << (config.enableCheckpoints ? "YES" : "NO") << "\n";
    std::cout << "  Export: " << (config.enableExport ? "YES" : "NO") << "\n";
}

void exportResults(const std::string& filename, const std::vector<std::map<std::string, std::string>>& data) {
    if (filename.empty() || data.empty()) return;
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open output file '" << filename << "'\n";
        return;
    }
    
    // Write header
    const auto& header = data[0];
    for (auto it = header.begin(); it != header.end(); ++it) {
        file << it->first;
        if (std::next(it) != header.end()) file << ",";
    }
    file << "\n";
    
    // Write data rows
    for (const auto& row : data) {
        for (auto it = row.begin(); it != row.end(); ++it) {
            file << it->second;
            if (std::next(it) != row.end()) file << ",";
        }
        file << "\n";
    }
    
    file.close();
    std::cout << "Results exported to '" << filename << "'\n";
}

void runBasicTest(std::shared_ptr<Brain> brain, const CLIConfig& config) {
    std::cout << "\n=== Basic Connectivity Test ===\n";
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        std::cout << "No neurons found!\n";
        return;
    }
    
    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject current into first 10 neurons
    std::cout << "Injecting current into 10 neurons...\n";
    for (size_t i = 0; i < std::min<size_t>(10, neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);
    }
    
    // Run steps
    for (SimulationStep step = 0; step < config.steps; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    std::cout << "Spikes generated: " << spikes << "\n";
    
    if (spikes > 0) {
        std::cout << "✓ Spikes propagate through network\n";
    } else {
        std::cout << "! No spikes - checking neuron parameters...\n";
        for (size_t i = 0; i < std::min<size_t>(3, neurons.size()); ++i) {
            std::cout << "  Neuron " << i << 
                         " V=" << neurons[i]->getMembranePotential() <<
                         " thresh=" << neurons[i]->getThreshold() << "\n";
        }
    }
}

void runLearningExperiment(std::shared_ptr<Brain> brain, const CLIConfig& config) {
    std::cout << "\n=== Learning Experiment ===\n";
    
    // Enable plasticity on synapses
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    std::cout << "Applying repeated input patterns (" << config.learningSteps << " steps)...\n";
    
    for (SimulationStep step = 0; step < config.learningSteps; ++step) {
        // Create input pattern
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        if (step % 200 == 0) {
            std::cout << "  Step " << step << 
                         " | Spikes: " << brain->getTotalSpikeCount() <<
                         " | Firing: " << brain->getFiringNeuronCount() << "\n";
        }
    }
    
    std::cout << "Learning experiment completed\n";
}

void runSTDPVerification(std::shared_ptr<Brain> brain, const CLIConfig& config) {
    std::cout << "\n=== STDP Verification ===\n";
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        std::cout << "Not enough synapses for STDP test\n";
        return;
    }
    
    std::cout << "Testing STDP on 5 synapses:\n";
    
    // Record initial weights
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
        std::cout << "  Synapse " << i << " before: " << beforeWeights[i] << "\n";
    }
    
    // Create correlated activity for LTP
    std::cout << "\nCreating correlated pre->post activity (potentiation)...\n";
    
    auto neurons = region->getAllNeurons();
    if (neurons.size() >= 2) {
        Neuron* preNeuron = neurons[0];
        Neuron* postNeuron = neurons[1];
        
        for (int trial = 0; trial < config.stdpTrials; ++trial) {
            // Pre fires first
            preNeuron->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            
            // Then post fires
            postNeuron->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
    }
    
    std::cout << "After correlated activity:\n";
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        std::cout << "  Synapse " << i << 
                     " after: " << synapses[i]->getWeight() << 
                     " (Δ=" << delta << ")\n";
        totalDelta += delta;
    }
    
    std::cout << "\n";
    if (totalDelta > 0.001f) {
        std::cout << "✓ STDP WORKING: Pre-before-post produced potentiation\n";
    } else if (totalDelta < -0.001f) {
        std::cout << "! STDP reversed: Check parameters\n";
    } else {
        std::cout << "! No change: STDP may not be triggering\n";
    }
}

int main(int argc, char* argv[]) {
    CLIConfig config;
    
    // Parse command line arguments
    if (!config.parseArgs(argc, argv)) {
        return 1; // Error or help requested
    }
    
    // Print system info
    printSystemInfo(config);
    
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    if (config.enableLogging) {
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
    }
    Logger::setGlobal(logger);
    
    // Initialize configuration
    auto cfg = std::make_shared<Config>();
    
    // Try to load from config file if provided
    if (config.configFile != "configs/default.cfg") {
        if (cfg->loadFromFile(config.configFile)) {
            std::cout << "Loaded configuration from: " << config.configFile << "\n";
        } else {
            std::cout << "Warning: Could not load configuration from file\n";
        }
    }
    
    // Override with CLI arguments
    cfg->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    cfg->set("simulation_timestep", 0.001, ConfigSource::Default);
    cfg->set("neuron_count", static_cast<int64_t>(config.neurons), ConfigSource::Default);
    cfg->set("region_count", static_cast<int64_t>(config.regions), ConfigSource::Default);
    cfg->set("connection_probability", config.connectionProbability, ConfigSource::Default);
    
    // STDP parameters
    cfg->set("stdp_ltp_weight", config.stdpLtpWeight, ConfigSource::Default);
    cfg->set("stdp_ltd_weight", config.stdpLtdWeight, ConfigSource::Default);
    cfg->set("stdp_tau", config.stdpTau, ConfigSource::Default);
    
    // Structural plasticity parameters
    cfg->set("synaptogenesis_rate", config.synaptogenesisRate, ConfigSource::Default);
    cfg->set("pruning_rate", config.pruningRate, ConfigSource::Default);
    
    std::cout << "\n=== Initializing NLM Brain ===\n";
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!\n";
        return 1;
    }
    
    brain->logStatus();
    
    // Performance monitoring setup
    auto startTime = std::chrono::high_resolution_clock::now();
    size_t peakMemory = 0;
    
    // Run experiments based on mode
    if (config.mode == "basic") {
        runBasicTest(brain, config);
    }
    else if (config.mode == "learning") {
        runLearningExperiment(brain, config);
    }
    else if (config.mode == "stdp") {
        runSTDPVerification(brain, config);
    }
    else if (config.mode == "full") {
        std::cout << "\n=== Running Full Experiment Suite ===\n";
        
        // Reset and run basic test
        brain->reset();
        brain->initialize();
        runBasicTest(brain, config);
        
        // Reset and run learning experiment
        brain->reset();
        brain->initialize();
        runLearningExperiment(brain, config);
        
        // Reset and run STDP verification
        brain->reset();
        brain->initialize();
        runSTDPVerification(brain, config);
    }
    else if (config.mode == "profile") {
        std::cout << "\n=== Running with Performance Profiling ===\n";
        
        // Run learning experiment with monitoring
        runLearningExperiment(brain, config);
        
        // Collect performance metrics
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime).count();
        
        std::cout << "\n=== Performance Metrics ===\n";
        std::cout << "Total execution time: " << duration << "s\n";
        std::cout << "Average firing rate: " << brain->getAverageFiringRate() << " Hz\n";
        std::cout << "Total spikes: " << brain->getTotalSpikeCount() << "\n";
        std::cout << "Active neurons: " << brain->getActiveNeuronCount() << "\n";
        std::cout << "E/I ratio: " << brain->getExcitationInhibitionRatio() << "\n";
    }
    
    // Final brain status
    std::cout << "\n=== Final Brain Status ===\n";
    brain->logStatus();
    
    // Save checkpoint if requested
    if (config.enableCheckpoints && !config.checkpointFile.empty()) {
        std::cout << "\nSaving checkpoint to: " << config.checkpointFile << "\n";
        if (brain->save(config.checkpointFile)) {
            std::cout << "Checkpoint saved successfully\n";
        } else {
            std::cerr << "Error: Failed to save checkpoint\n";
        }
    }
    
    // Export results if requested
    if (config.enableExport && !config.outputFile.empty()) {
        std::vector<std::map<std::string, std::string>> exportData;
        
        std::map<std::string, std::string> row;
        row["mode"] = config.mode;
        row["steps"] = std::to_string(config.steps);
        row["neurons"] = std::to_string(config.neurons);
        row["regions"] = std::to_string(config.regions);
        row["total_spikes"] = std::to_string(brain->getTotalSpikeCount());
        row["avg_firing_rate"] = std::to_string(brain->getAverageFiringRate());
        row["e_i_ratio"] = std::to_string(brain->getExcitationInhibitionRatio());
        
        exportData.push_back(row);
        
        exportResults(config.outputFile, exportData);
    }
    
    std::cout << "\n=== Execution Complete ===\n";
    std::cout << "Mode: " << config.mode << "\n";
    std::cout << "Success: All tasks completed successfully\n";
    
    return 0;
}