// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
// Enhanced version with user-friendly features and improved usability
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)

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
#include "visualization/VisualizationInterface.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <map>
#include <functional>
#include <chrono>
#include <fstream>
#include <algorithm>
#include <sstream>

using namespace nlm;

// Configuration structure for improved modularity
struct NLMConfig {
    uint64_t randomSeed = 42;
    double simulationTimestep = 0.001;
    size_t neuronCount = 500;
    size_t regionCount = 1;
    float connectionProbability = 0.15f;
    float stdpLtpWeight = 0.02f;
    float stdpLtdWeight = 0.015f;
    float stdpTau = 20.0f;
    float synaptogenesisRate = 0.0001f;
    float pruningRate = 0.00001f;
    bool verboseLogging = true;
    bool enableVisualization = false;
    bool saveCheckpoints = true;
    size_t maxCheckpoints = 10;
    std::string checkpointDir = "./checkpoints";
    std::string configFile = "configs/default.cfg";
    std::string experimentMode = "all"; // "all", "basic", "plasticity", "stdp", "custom"
    size_t customExperimentSteps = 1000;
    float customInputStrength = 30.0f;
    bool validateParams = true;
    bool autoSaveConfig = true;
    std::string experimentHistoryFile = "experiment_history.json";
    
    void applyToConfig(std::shared_ptr<Config> config) const {
        config->set("random_seed", randomSeed, ConfigSource::Default);
        config->set("simulation_timestep", simulationTimestep, ConfigSource::Default);
        config->set("neuron_count", neuronCount, ConfigSource::Default);
        config->set("region_count", regionCount, ConfigSource::Default);
        config->set("connection_probability", connectionProbability, ConfigSource::Default);
        config->set("stdp_ltp_weight", stdpLtpWeight, ConfigSource::Default);
        config->set("stdp_ltd_weight", stdpLtdWeight, ConfigSource::Default);
        config->set("stdp_tau", stdpTau, ConfigSource::Default);
        config->set("synaptogenesis_rate", synaptogenesisRate, ConfigSource::Default);
        config->set("pruning_rate", pruningRate, ConfigSource::Default);
    }
    
    bool validate() const {
        bool valid = true;
        
        if (neuronCount == 0) {
            std::cerr << "Error: neuron_count must be > 0" << std::endl;
            valid = false;
        }
        
        if (regionCount == 0) {
            std::cerr << "Error: region_count must be > 0" << std::endl;
            valid = false;
        }
        
        if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
            std::cerr << "Error: connection_probability must be between 0.0 and 1.0" << std::endl;
            valid = false;
        }
        
        if (simulationTimestep <= 0.0) {
            std::cerr << "Error: simulation_timestep must be > 0" << std::endl;
            valid = false;
        }
        
        if (stdpLtpWeight < 0.0f) {
            std::cerr << "Error: stdp_ltp_weight must be >= 0.0" << std::endl;
            valid = false;
        }
        
        if (stdpLtdWeight < 0.0f) {
            std::cerr << "Error: stdp_ltd_weight must be >= 0.0" << std::endl;
            valid = false;
        }
        
        if (stdpTau <= 0.0f) {
            std::cerr << "Error: stdp_tau must be > 0.0" << std::endl;
            valid = false;
        }
        
        if (!valid && verboseLogging) {
            std::cout << "Use --help for more information." << std::endl;
        }
        
        return valid;
    }
};

// Command-line argument parser with enhanced validation
NLMConfig parseCommandLine(int argc, char** argv) {
    NLMConfig config;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            printUsage();
            exit(0);
        }
        else if (arg == "--mode" && i + 1 < argc) {
            config.experimentMode = argv[++i];
        }
        else if (arg == "--neurons" && i + 1 < argc) {
            try {
                config.neuronCount = std::stoul(argv[++i]);
            } catch (const std::exception&) {
                std::cerr << "Error: Invalid neuron count value: " << argv[i] << std::endl;
                exit(1);
            }
        }
        else if (arg == "--regions" && i + 1 < argc) {
            try {
                config.regionCount = std::stoul(argv[++i]);
            } catch (const std::exception&) {
                std::cerr << "Error: Invalid region count value: " << argv[i] << std::endl;
                exit(1);
            }
        }
        else if (arg == "--seed" && i + 1 < argc) {
            try {
                config.randomSeed = std::stoull(argv[++i]);
            } catch (const std::exception&) {
                std::cerr << "Error: Invalid seed value: " << argv[i] << std::endl;
                exit(1);
            }
        }
        else if (arg == "--timestep" && i + 1 < argc) {
            try {
                config.simulationTimestep = std::stod(argv[++i]);
            } catch (const std::exception&) {
                std::cerr << "Error: Invalid timestep value: " << argv[i] << std::endl;
                exit(1);
            }
        }
        else if (arg == "--plasticity" && i + 1 < argc) {
            try {
                config.customExperimentSteps = std::stoul(argv[++i]);
            } catch (const std::exception&) {
                std::cerr << "Error: Invalid plasticity steps value: " << argv[i] << std::endl;
                exit(1);
            }
        }
        else if (arg == "--input" && i + 1 < argc) {
            try {
                config.customInputStrength = std::stof(argv[++i]);
            } catch (const std::exception&) {
                std::cerr << "Error: Invalid input strength value: " << argv[i] << std::endl;
                exit(1);
            }
        }
        else if (arg == "--config" && i + 1 < argc) {
            config.configFile = argv[++i];
        }
        else if (arg == "--checkpoint-dir" && i + 1 < argc) {
            config.checkpointDir = argv[++i];
        }
        else if (arg == "--no-checkpoints") {
            config.saveCheckpoints = false;
        }
        else if (arg == "--quiet") {
            config.verboseLogging = false;
        }
        else if (arg == "--validate-only") {
            config.validateParams = true;
            config.verboseLogging = false;
        }
        else if (arg == "--auto-save") {
            config.autoSaveConfig = true;
        }
        else if (arg == "--no-auto-save") {
            config.autoSaveConfig = false;
        }
        else if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                config.configFile = arg.substr(arg.find('=') + 1);
            }
        }
    }
    
    // Validate experiment mode
    std::vector<std::string> validModes = {"all", "basic", "plasticity", "stdp", "custom"};
    if (std::find(validModes.begin(), validModes.end(), config.experimentMode) == validModes.end()) {
        std::cerr << "Error: Invalid experiment mode: " << config.experimentMode << std::endl;
        printUsage();
        exit(1);
    }
    
    return config;
}

void printUsage() {
    std::cout << "NLM Neural Learning Machine - Usage:" << std::endl;
    std::cout << "  ./nlm [options]" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Modes (default: all):" << std::endl;
    std::cout << "  all      Run all experiments sequentially" << std::endl;
    std::cout << "  basic    Run only basic connectivity test" << std::endl;
    std::cout << "  plasticity Run only plasticity experiment" << std::endl;
    std::cout << "  stdp     Run only STDP verification" << std::endl;
    std::cout << "  custom   Run custom experiment with specified parameters" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --neurons N          Number of neurons (default: 500)" << std::endl;
    std::cout << "  --regions R          Number of regions (default: 1)" << std::endl;
    std::cout << "  --seed S             Random seed (default: 42)" << std::endl;
    std::cout << "  --timestep T         Simulation timestep in seconds (default: 0.001)" << std::endl;
    std::cout << "  --plasticity STEPS   Number of plasticity experiment steps (default: 1000)" << std::endl;
    std::cout << "  --input STRENGTH     Input current strength (default: 30.0)" << std::endl;
    std::cout << "  --config FILE        Configuration file (default: configs/default.cfg)" << std::endl;
    std::cout << "  --checkpoint-dir DIR  Directory for checkpoints (default: ./checkpoints)" << std::endl;
    std::cout << "  --no-checkpoints      Disable checkpoint saving" << std::endl;
    std::cout << "  --quiet              Minimize logging output" << std::endl;
    std::cout << "  --validate-only      Only validate configuration, don't run experiments" << std::endl;
    std::cout << "  --auto-save          Auto-save configuration after changes" << std::endl;
    std::cout << "  --no-auto-save       Don't auto-save configuration" << std::endl;
    std::cout << "  -h, --help           Show this help message" << std::endl;
}

void printBanner() {
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
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}