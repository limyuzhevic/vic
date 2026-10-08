// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
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

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>

using namespace nlm;

void printBanner();

void printUsage();

void printUsage() {
    std::cout << R"(
NLM Usage:
  nlm [OPTIONS] [ARGUMENTS]

Core Options:
  --config=<file>                    Load configuration from JSON or key=value file
  --neuron_count=<int>               Override number of neurons (default: 500)
  --region_count=<int>                Override number of regions (default: 1)
  --connection_probability=<float>    Override connection probability (default: 0.15)
  --simulation_timestep=<float>       Override timestep in seconds (default: 0.001)

Output Options:
  --log=<level>                      Set log level: DEBUG, INFO, WARNING, ERROR, CRITICAL
  --log_file=<file>                  Log output to file
  --performance                      Enable performance profiling
  --export=<format>                  Export results: json, csv, xml, or all
  --output_dir=<dir>                 Directory for all outputs

Experiment Options:
  --experiment=<name>                Run specific experiment: basic, plasticity, stdp, learning, scaling
  --experiment_file=<file>            Run experiment from file
  --experiment_args=<args>            Additional arguments for experiment

Debug & Development Options:
  --checkpoint=<file>                 Save brain state to file at checkpoints
  --checkpoint_interval=<int>         Save checkpoint every N steps
  --verbose                           Enable verbose logging
  --debug_neurons=<int>               Debug specific neuron IDs
  --debug_synapses=<int>              Debug specific synapse IDs
  --profile=<type>                    Profile type: memory, time, network
  --validate_config                   Validate configuration and exit

Visualization Options:
  --viz=<type>                        Enable visualization: network, activity, heatmap, spike_raster
  --viz_file=<file>                   Save visualization to file
  --viz_interval=<int>                Visualization update interval in steps

Advanced Options:
  --enable_checkpointing              Enable automatic checkpointing
  --enable_profiling                  Enable detailed profiling
  --enable_validation                 Enable configuration validation
  --memory_limit=<bytes>              Set memory usage limit
  --thread_count=<int>                Number of threads for parallel processing
  --random_seed=<int>                 Override random seed for reproducible results
  --reproduce                         Run with previous experiment seed/reproduction ID
  --compare=<type>                    Compare against baseline: evolution, performance, accuracy
  --runs=<int>                        Number of runs for comparative experiments (default: 1)
  --config_set=<name>                 Use predefined configuration set: standard, optimized, minimal

Utility Options:
  --help                              Show this help message
  --version                           Show version information
  --create_config                     Create example configuration file and exit
  --validate_config_file=<file>       Validate specific configuration file
  --config_schema                    Show configuration schema and exit

Examples:
  # Basic run with custom config
  ./nlm --config=my_config.json --experiment=learning --export=json

  # Performance benchmark with profiling
  ./nlm --performance --profile=time --experiment=scaling --runs=10

  # Visual debugging
  ./nlm --viz=network --viz_file=brain_network.png --debug_neurons=123

  # Advanced comparative experiment
  ./nlm --compare=evolution --runs=5 --config_set=optimized --export=results.csv

For more information, visit: https://github.com/your-repo/nlm
  )" << std::endl;
}

// Learning Experiment: Demonstrates measurable synaptic changes through experience
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
    // Handle help flag early
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h") {
            printBanner();
            printUsage();
            return 0;
        }
    }
    
    printBanner();
    
    std::cout << "Initializing NLM Phase 2 Real Neural Computation...\n" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation ===");
    NLM_LOG_INFO("Implementing:");
    NLM_LOG_INFO("  - Leaky Integrate-and-Fire (LIF) neuron dynamics");
    NLM_LOG_INFO("  - Event-driven spike propagation with delays");
    NLM_LOG_INFO("  - STDP and Hebbian plasticity rules");
    NLM_LOG_INFO("  - Structural plasticity (synaptogenesis/pruning)");
    NLM_LOG_INFO("");
    
    // Parse command line options
    std::string configFile = "configs/default.cfg";
    std::string logLevel = "INFO";
    std::string logFile = "";
    bool enablePerformance = false;
    std::string exportFormat = "";
    std::string outputDir = ".";
    std::string experimentName = "";
    std::string experimentFile = "";
    std::string experimentArgs = "";
    bool validateConfig = false;
    bool enableVerbose = false;
    int debugNeuronIds = -1;
    int debugSynapseIds = -1;
    std::string profileType = "";
    bool enableCheckpointing = false;
    std::string checkpointFile = "";
    int checkpointInterval = -1;
    std::string vizType = "";
    std::string vizFile = "";
    int vizInterval = -1;
    bool enableProfiling = false;
    bool enableValidation = false;
    long long memoryLimit = -1;
    int threadCount = -1;
    int64_t randomSeed = -1;
    bool reproduce = false;
    std::string compareType = "";
    int runs = 1;
    std::string configSet = "";
    
    // Parse all command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (arg.substr(0, 8) == "--log=") {
            logLevel = arg.substr(8);
        } else if (arg.substr(0, 10) == "--log_file=") {
            logFile = arg.substr(11);
        } else if (arg == "--performance") {
            enablePerformance = true;
        } else if (arg.substr(0, 10) == "--export=") {
            exportFormat = arg.substr(10);
        } else if (arg.substr(0, 13) == "--output_dir=") {
            outputDir = arg.substr(14);
        } else if (arg.substr(0, 14) == "--experiment=") {
            experimentName = arg.substr(15);
        } else if (arg.substr(0, 19) == "--experiment_file=") {
            experimentFile = arg.substr(20);
        } else if (arg.substr(0, 20) == "--experiment_args=") {
            experimentArgs = arg.substr(21);
        } else if (arg == "--validate_config") {
            validateConfig = true;
        } else if (arg == "--verbose") {
            enableVerbose = true;
        } else if (arg.substr(0, 17) == "--debug_neurons=") {
            debugNeuronIds = std::stoi(arg.substr(17));
        } else if (arg.substr(0, 18) == "--debug_synapses=") {
            debugSynapseIds = std::stoi(arg.substr(18));
        } else if (arg.substr(0, 11) == "--profile=") {
            profileType = arg.substr(11);
        } else if (arg == "--enable_checkpointing") {
            enableCheckpointing = true;
        } else if (arg.substr(0, 14) == "--checkpoint=") {
            checkpointFile = arg.substr(15);
        } else if (arg.substr(0, 24) == "--checkpoint_interval=") {
            checkpointInterval = std::stoi(arg.substr(24));
        } else if (arg.substr(0, 7) == "--viz=") {
            vizType = arg.substr(7);
        } else if (arg.substr(0, 11) == "--viz_file=") {
            vizFile = arg.substr(12);
        } else if (arg.substr(0, 16) == "--viz_interval=") {
            vizInterval = std::stoi(arg.substr(16));
        } else if (arg == "--enable_profiling") {
            enableProfiling = true;
        } else if (arg == "--enable_validation") {
            enableValidation = true;
        } else if (arg.substr(0, 17) == "--memory_limit=") {
            memoryLimit = std::stoll(arg.substr(17));
        } else if (arg.substr(0, 17) == "--thread_count=") {
            threadCount = std::stoi(arg.substr(18));
        } else if (arg.substr(0, 16) == "--random_seed=") {
            randomSeed = std::stoll(arg.substr(16));
        } else if (arg == "--reproduce") {
            reproduce = true;
        } else if (arg.substr(0, 13) == "--compare=") {
            compareType = arg.substr(13);
        } else if (arg.substr(0, 9) == "--runs=") {
            runs = std::stoi(arg.substr(9));
        } else if (arg.substr(0, 15) == "--config_set=") {
            configSet = arg.substr(16);
        } else if (arg == "--version") {
            std::cout << "NLM Version 0.1.0 - Phase 2: Real Neural Computation" << std::endl;
            return 0;
        } else if (arg == "--create_config") {
            std::cout << "Creating example configuration file..." << std::endl;
            // Create example config file
            std::ofstream exampleConfig("configs/example_config.json");
            if (exampleConfig.is_open()) {
                exampleConfig << R"({
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "plasticity_learning_rate": 0.01,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012,
    "stdp_time_constant": 20.0,
    "development_synaptogenesis_rate": 0.001,
    "development_pruning_rate": 0.0001,
    "dopamine_baseline": 0.1,
    "reward_discount_factor": 0.99,
    "environment_name": "GridWorld",
    "environment_width": 10,
    "environment_height": 10,
    "log_level": "INFO",
    "log_to_file": false,
    "log_filename": "nlm.log",
    "max_simulation_steps": 10000,
    "simulation_time_limit": 0.0,
    "visualization_enabled": false,
    "visualization_update_rate": 30.0
})" << std::endl;
                exampleConfig.close();
                std::cout << "Created: configs/example_config.json" << std::endl;
            }
            return 0;
        } else if (arg.substr(0, 26) == "--validate_config_file=") {
            std::string configPath = arg.substr(27);
            std::cout << "Validating configuration file: " << configPath << "..." << std::endl;
            auto tempConfig = std::make_shared<Config>();
            if (tempConfig->loadFromFile(configPath)) {
                std::cout << "✓ Configuration file is valid" << std::endl;
                std::cout << "Configuration summary:" << std::endl;
                std::cout << tempConfig->summary() << std::endl;
            } else {
                std::cout << "✗ Configuration file is invalid" << std::endl;
            }
            return 0;
        } else if (arg == "--config_schema") {
            std::cout << "NLM Configuration Schema:" << std::endl;
            std::cout << R"({
    "type": "object",
    "properties": {
        "random_seed": {"type": "integer", "default": 42},
        "simulation_timestep": {"type": "number", "default": 0.001},
        "neuron_count": {"type": "integer", "default": 500},
        "region_count": {"type": "integer", "default": 1},
        "connection_probability": {"type": "number", "default": 0.15},
        "plasticity_learning_rate": {"type": "number", "default": 0.01},
        "stdp_ltp_weight": {"type": "number", "default": 0.01},
        "stdp_ltd_weight": {"type": "number", "default": 0.012},
        "stdp_time_constant": {"type": "number", "default": 20.0},
        "development_synaptogenesis_rate": {"type": "number", "default": 0.001},
        "development_pruning_rate": {"type": "number", "default": 0.0001},
        "dopamine_baseline": {"type": "number", "default": 0.1},
        "reward_discount_factor": {"type": "number", "default": 0.99},
        "environment_name": {"type": "string", "default": "GridWorld"},
        "environment_width": {"type": "integer", "default": 10},
        "environment_height": {"type": "integer", "default": 10},
        "log_level": {"type": "string", "default": "INFO"},
        "log_to_file": {"type": "boolean", "default": false},
        "log_filename": {"type": "string", "default": "nlm.log"},
        "max_simulation_steps": {"type": "integer", "default": 10000},
        "simulation_time_limit": {"type": "number", "default": 0.0},
        "visualization_enabled": {"type": "boolean", "default": false},
        "visualization_update_rate": {"type": "number", "default": 30.0}
    },
    "required": []
})" << std::endl;
            return 0;
        } else if (arg.substr(0, 5) == "--neuron_count") {
            if (arg.find('=') != std::string::npos) {
                // Already parsed above
            } else if (i + 1 < argc) {
                // Skip the next argument for --neuron_count value
                i++;
            }
        } else if (arg.substr(0, 5) == "--region_count") {
            if (arg.find('=') != std::string::npos) {
                // Already parsed above
            } else if (i + 1 < argc) {
                i++;
            }
        } else if (arg.substr(0, 23) == "--connection_probability") {
            if (arg.find('=') != std::string::npos) {
                // Already parsed above
            } else if (i + 1 < argc) {
                i++;
            }
        } else if (arg.substr(0, 24) == "--simulation_timestep") {
            if (arg.find('=') != std::string::npos) {
                // Already parsed above
            } else if (i + 1 < argc) {
                i++;
            }
        } else if (arg.substr(0, 17) == "--synaptogenesis_rate") {
            if (arg.find('=') != std::string::npos) {
                // Already parsed above
            } else if (i + 1 < argc) {
                i++;
            }
        } else if (arg.substr(0, 15) == "--pruning_rate") {
            if (arg.find('=') != std::string::npos) {
                // Already parsed above
            } else if (i + 1 < argc) {
                i++;
            }
        } else if (arg.substr(0, 20) == "--dopamine_baseline") {
            if (arg.find('=') != std::string::npos) {
                // Already parsed above
            } else if (i + 1 < argc) {
                i++;
            }
        } else if (arg.substr(0, 25) == "--reward_discount_factor") {
            if (arg.find('=') != std::string::npos) {
                // Already parsed above
            } else if (i + 1 < argc) {
                i++;
            }
        } else if (arg.substr(0, 22) == "--environment_name") {
            if (arg.find('=') != std::string::npos) {
                // Already parsed above
            } else if (i + 1 < argc) {
                i++;
            }
        } else if (arg.substr(0, 21) == "--environment_width") {
            if (arg.find('=') != std::string::npos) {
                // Already parsed above
            } else if (i + 1 < argc) {
                i++;
            }
        } else if (arg.substr(0, 22) == "--environment_height") {
            if (arg.find('=') != std::string::npos) {
                // Already parsed above
            } else if (i + 1 < argc) {
                i++;
            }
        } else if (arg.substr(0, 13) == "--log_level=") {
            // Already parsed as --log
            if (arg.find('=') != std::string::npos) {
                // Already parsed above
            } else if (i + 1 < argc) {
                i++;
            }
        } else if (arg.substr(0, 13) == "--stdp_ltp_weight") {
            if (arg.find('=') != std::string::npos) {
                // Already parsed above
            } else if (i + 1 < argc) {
                i++;
            }
        } else if (arg.substr(0, 14) == "--stdp_ltd_weight") {
            if (arg.find('=') != std::string::npos) {
                // Already parsed above
            } else if (i + 1 < argc) {
                i++;
            }
        } else if (arg.substr(0, 16) == "--stdp_time_constant") {
            if (arg.find('=') != std::string::npos) {
                // Already parsed above
            } else if (i + 1 < argc) {
                i++;
            }
        } else if (arg.substr(0, 16) == "--max_simulation_steps") {
            if (arg.find('=') != std::string::npos) {
                // Already parsed above
            } else if (i + 1 < argc) {
                i++;
            }
        } else {
            // Unknown argument, could be --key=value or -key
            if (arg.substr(0, 2) == "--") {
                if (arg.find('=') != std::string::npos) {
                    // Extract key and value from --key=value
                    size_t pos = arg.find('=');
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    
                    // Set additional configuration options
                    if (key == "log_level") {
                        logLevel = value;
                    } else if (key == "environment_name") {
                        // This would need to be set in the config
                    } else {
                        std::cout << "Warning: Unknown option --" << key << std::endl;
                    }
                } else {
                    std::cout << "Warning: Option --" << arg.substr(2) << " requires a value" << std::endl;
                }
            } else if (arg.substr(0, 1) == "-") {
                // Handle short options
                std::cout << "Warning: Short option -" << arg.substr(1) << " is not supported" << std::endl;
            }
        }
    }
    
    // Update log level
    LogLevel level = LogLevel::Info;
    if (logLevel == "DEBUG") level = LogLevel::Debug;
    else if (logLevel == "WARNING") level = LogLevel::Warning;
    else if (logLevel == "ERROR") level = LogLevel::Error;
    else if (logLevel == "CRITICAL") level = LogLevel::Critical;
    
    auto updatedConsoleLogger = std::make_shared<ConsoleLogger>(level);
    logger->addLogger(updatedConsoleLogger);
    
    NLM_LOG_INFO("Log level set to: " + logLevel);
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
    std::string configFile = "configs/default.cfg";
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        }
    }
    
    // Load config from file (ignore if not found)
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Set default values for Phase 2
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);  // Smaller for faster test
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    NLM_LOG_INFO("");
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Run basic tests (could be replaced with specific experiments based on command line)
    if (experimentName == "basic") {
        runBasicConnectivityTest(brain);
    } else if (experimentName == "plasticity") {
        runPlasticityExperiment(brain);
    } else if (experimentName == "stdp") {
        runStdpVerification(brain);
    } else if (experimentName == "learning") {
        runPlasticityExperiment(brain);  // Learning experiment includes plasticity
    } else {
        // Default tests for unknown experiment names
        NLM_LOG_INFO("Running default tests (unknown experiment: " << experimentName << ")");
        runBasicConnectivityTest(brain);
        
        // Reset brain for plasticity experiment
        brain->reset();
        brain->initialize();
        
        runPlasticityExperiment(brain);
        
        // Reset and run STDP verification
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
    }
    
    // Simple export and monitoring utilities
std::string getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void exportResultsToCSV(std::shared_ptr<Brain> brain, const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open file for CSV export: " + filepath);
        return;
    }
    
    try {
        // Write CSV headers
        file << "Metric,Value\n";
        file << "total_neurons," << brain->getTotalNeuronCount() << "\n";
        file << "total_synapses," << brain->getTotalSynapseCount() << "\n";
        file << "total_spikes," << brain->getTotalSpikeCount() << "\n";
        file << "firing_neuron_count," << brain->getFiringNeuronCount() << "\n";
        file << "simulation_steps," << brain->getSimulationStepCount() << "\n";
        file << "avg_membrane_potential," << brain->getAverageMembranePotential() << "\n";
        file << "max_membrane_potential," << brain->getMaxMembranePotential() << "\n";
        file << "min_membrane_potential," << brain->getMinMembranePotential() << "\n";
        
        NLM_LOG_INFO("Successfully exported results to CSV: " + filepath);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error exporting results to CSV " + filepath + ": " + std::string(e.what()));
    }
}

void exportResultsToJSON(std::shared_ptr<Brain> brain, const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open file for JSON export: " + filepath);
        return;
    }
    
    try {
        file << "{\n";
        file << "  \"metadata\": {\n";
        file << "    \"timestamp\": \"" << getCurrentTime() << "\",\n";
        file << "    \"version\": \"NLM Phase 2\",\n";
        file << "    \"totalNeurons\": " << brain->getTotalNeuronCount() << ",\n";
        file << "    \"totalSynapses\": " << brain->getTotalSynapseCount() << ",\n";
        file << "    \"totalSpikes\": " << brain->getTotalSpikeCount() << "\n";
        file << "  }\n";
        file << "}\n";
        
        NLM_LOG_INFO("Successfully exported results to JSON: " + filepath);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error exporting results to JSON " + filepath + ": " + std::string(e.what()));
    }
}

void printPerformanceReport(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Performance Report ===");
    NLM_LOG_INFO("Network Statistics:");
    NLM_LOG_INFO("  Total Neurons: " << brain->getTotalNeuronCount());
    NLM_LOG_INFO("  Total Synapses: " << brain->getTotalSynapseCount());
    NLM_LOG_INFO("  Total Spikes: " << brain->getTotalSpikeCount());
    NLM_LOG_INFO("  Active Neurons: " << brain->getFiringNeuronCount());
    NLM_LOG_INFO("  Simulation Steps: " << brain->getSimulationStepCount());
    NLM_LOG_INFO("  Average Membrane Potential: " << brain->getAverageMembranePotential());
    NLM_LOG_INFO("  Max Membrane Potential: " << brain->getMaxMembranePotential());
    NLM_LOG_INFO("  Min Membrane Potential: " << brain->getMinMembranePotential());
    
    // Calculate network density
    double density = 0.0;
    if (brain->getTotalNeuronCount() > 0) {
        density = static_cast<double>(brain->getTotalSynapseCount()) / brain->getTotalNeuronCount();
    }
    NLM_LOG_INFO("  Network Density (synapses/neuron): " << density);
    
    NLM_LOG_INFO("");
}
