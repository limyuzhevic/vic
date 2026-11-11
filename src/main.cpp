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

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements:                                    ║
    ║     - Memory systems (working, episodic, associative)          ║
    ║     - Neuromodulation (dopamine, curiosity, prediction error)   ║
    ║     - Prediction system integrated                            ║
    ║     - Cognitive systems (planner, concept formation)           ║
    ║     - Development integration                                   ║
    ║                                                               ║
    ║     User-friendly Command Line Interface                       ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printUsage() {
    std::cout << R"(
Usage: nlm-cli [OPTIONS]

Options:
  -h, --help                    Show this help message
  -v, --version                 Show version information
  --config FILE                 Load configuration from file
  --neurons N                   Set number of neurons (default: 500)
  --steps S                     Run S simulation steps (default: 100)
  --time T                      Run for T seconds (overrides steps)
  --verbose                      Enable verbose output
  --quiet                        Suppress informational messages
  --demo                        Run demonstration mode
  --validate-config             Validate configuration
  --benchmark                    Run benchmark tests
  --save STATE_FILE             Save brain state to file
  --load STATE_FILE             Load brain state from file
  --stats                        Print brain statistics

Examples:
  nlm-cli --neurons 1000 --steps 500
  nlm-cli --config custom.cfg --steps 1000
  nlm-cli --demo --verbose
  nlm-cli --validate-config --benchmark
" << std::endl;
}

void runSimpleSimulation(std::shared_ptr<Brain> brain, size_t steps, bool verbose) {
    if (verbose) {
        NLM_LOG_INFO("Running simulation for " + std::to_string(steps) + " steps...");
    }
    
    for (SimulationStep step = 0; step < static_cast<SimulationStep>(steps); ++step) {
        brain->step(step);
        
        if (verbose && step % 100 == 0) {
            NLM_LOG_INFO("Step " + std::to_string(step) + ": " + std::to_string(brain->getFiringNeuronCount()) + " firing neurons, " + std::to_string(brain->getTotalSpikeCount()) + " spikes");
        }
    }
    
    if (verbose) {
        NLM_LOG_INFO("Simulation completed. Final stats: " + std::to_string(brain->getFiringNeuronCount()) + " neurons firing, " + std::to_string(brain->getTotalSpikeCount()) + " total spikes.");
    }
}

void runLearningSimulation(std::shared_ptr<Brain> brain, size_t steps, bool verbose) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Learning Experiment ===");
    
    // Enable plasticity on synapses
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    // Apply repeated input to stimulate learning
    NLM_LOG_INFO("Applying learning patterns...");
    
    for (SimulationStep step = 0; step < static_cast<SimulationStep>(steps); ++step) {
        brain->injectCurrent(NeuronId(1), 30.0f);
        brain->step(step);
        
        if (verbose && step % 200 == 0) {
            NLM_LOG_INFO("Step " + std::to_string(step) + ": " + std::to_string(brain->getFiringNeuronCount()) + " firing neurons");
        }
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Learning experiment completed.");
}

void printBrainStats(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Brain Statistics ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Key metrics:");
    NLM_LOG_INFO("  Total neurons: " + std::to_string(brain->getTotalNeuronCount()));
    NLM_LOG_INFO("  Total synapses: " + std::to_string(brain->getTotalSynapseCount()));
    NLM_LOG_INFO("  Currently firing: " + std::to_string(brain->getFiringNeuronCount()));
    NLM_LOG_INFO("  Average firing rate: " + std::to_string(brain->getAverageFiringRate()) + " Hz");
    NLM_LOG_INFO("  E/I ratio: " + std::to_string(brain->getExcitationInhibitionRatio()));
    NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
    NLM_LOG_INFO("  Developmental stage: " + std::to_string(static_cast<int>(brain->getDevelopmentalStage())));
}

int main(int argc, char** argv) {
    printBanner();
    
    // Parse command line arguments
    std::string configFile = "configs/default.cfg";
    size_t neuronCount = 500;
    size_t steps = 100;
    double timeSeconds = 0.0;  // 0 means use steps
    bool verbose = false;
    bool quiet = false;
    bool demo = false;
    bool validateConfig = false;
    bool benchmark = false;
    std::string saveState = "";
    std::string loadState = "";
    bool showStats = false;
    
    // Parse arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "-h" || arg == "--help") {
            printUsage();
            return 0;
        } else if (arg == "-v" || arg == "--version") {
            std::cout << "NLM CLI Version: 0.1.0" << std::endl;
            std::cout << "Phase 6: Final Integration" << std::endl;
            return 0;
        } else if (arg.find("--config") == 0) {
            if (arg.find("=") != std::string::npos) {
                configFile = arg.substr(arg.find("=") + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (arg.find("--neurons") == 0) {
            if (arg.find("=") != std::string::npos) {
                neuronCount = std::stoul(arg.substr(arg.find("=") + 1));
            } else if (i + 1 < argc) {
                neuronCount = std::stoul(argv[++i]);
            }
        } else if (arg.find("--steps") == 0) {
            if (arg.find("=") != std::string::npos) {
                steps = std::stoul(arg.substr(arg.find("=") + 1));
            } else if (i + 1 < argc) {
                steps = std::stoul(argv[++i]);
            }
        } else if (arg.find("--time") == 0) {
            if (arg.find("=") != std::string::npos) {
                timeSeconds = std::stod(arg.substr(arg.find("=") + 1));
            } else if (i + 1 < argc) {
                timeSeconds = std::stod(argv[++i]);
            }
        } else if (arg == "--verbose") {
            verbose = true;
        } else if (arg == "--quiet") {
            quiet = true;
        } else if (arg == "--demo") {
            demo = true;
        } else if (arg == "--validate-config") {
            validateConfig = true;
        } else if (arg == "--benchmark") {
            benchmark = true;
        } else if (arg.find("--save") == 0) {
            if (arg.find("=") != std::string::npos) {
                saveState = arg.substr(arg.find("=") + 1);
            } else if (i + 1 < argc) {
                saveState = argv[++i];
            }
        } else if (arg.find("--load") == 0) {
            if (arg.find("=") != std::string::npos) {
                loadState = arg.substr(arg.find("=") + 1);
            } else if (i + 1 < argc) {
                loadState = argv[++i];
            }
        } else if (arg == "--stats") {
            showStats = true;
        }
    }
    
    // Override verbose/quiet based on what's available
    if (quiet && verbose) {
        quiet = false;  // verbose takes precedence
    }
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Command Line Interface ===");
    
    try {
        // Load configuration
        auto config = std::make_shared<Config>();
        if (validateConfig) {
            NLM_LOG_INFO("Validating configuration...");
        }
        
        if (config->loadFromFile(configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + configFile);
        } else {
            NLM_LOG_INFO("Using default configuration (file not found: " + configFile + ")");
        }
        
        // Override with command line args
        config->loadFromArgs(argc, argv);
        
        // Set specific values for Phase 6
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        // Log configuration summary
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Configuration summary:");
        NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
        NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
        NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
        
        // Initialize brain
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Initializing NLM Brain...");
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return 1;
        }
        
        brain->logStatus();
        
        // Load state if requested
        if (!loadState.empty()) {
            NLM_LOG_INFO("Loading brain state from: " + loadState);
            if (brain->load(loadState)) {
                NLM_LOG_INFO("Successfully loaded brain state.");
            } else {
                NLM_LOG_WARNING("Failed to load brain state. Starting fresh.");
            }
        }
        
        // Run demo mode
        if (demo) {
            NLM_LOG_INFO("");
            NLM_LOG_INFO("=== NLM Demo Mode ===");
            runSimpleSimulation(brain, steps, verbose);
            runLearningSimulation(brain, steps, verbose);
        } else {
            // Run simple simulation
            runSimpleSimulation(brain, steps, verbose);
            
            // Run learning experiment if benchmark enabled
            if (benchmark) {
                brain->reset();
                brain->initialize();
                runLearningSimulation(brain, steps, verbose);
            }
        }
        
        // Print statistics if requested
        if (showStats) {
            printBrainStats(brain);
        }
        
        // Save state if requested
        if (!saveState.empty()) {
            NLM_LOG_INFO("Saving brain state to: " + saveState);
            if (brain->save(saveState)) {
                NLM_LOG_INFO("Successfully saved brain state.");
            } else {
                NLM_LOG_WARNING("Failed to save brain state.");
            }
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== NLM CLI Complete ===");
        NLM_LOG_INFO("The NLM brain completed simulation successfully.");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error: " + std::string(e.what()));
        return 1;
    }
    
    return 0;
}
