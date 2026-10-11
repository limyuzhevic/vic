#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "world/SimpleWorld.hpp"

using namespace nlm;
using namespace std;

namespace {

    struct CommandLineOptions {
        string configFile;
        string outputFile;
        string logFile;
        string experimentFile;
        string checkpointFile;
        string replayFile;
        bool verbose;
        bool quiet;
        bool list;
        bool runDemo;
        bool runTests;
        bool saveConfig;
        bool loadConfig;
        bool generateConfig;
        bool benchmark;
        bool simulate;
        int maxSteps;
        int neuronCount;
        int regionCount;
        int randomSeed;
        float timestep;
        vector<string> customArgs;
    };

    CommandLineOptions parseArguments(int argc, char** argv) {
        CommandLineOptions options;
        options.verbose = false;
        options.quiet = false;
        options.list = false;
        options.runDemo = false;
        options.runTests = false;
        options.saveConfig = false;
        options.loadConfig = false;
        options.generateConfig = false;
        options.benchmark = false;
        options.simulate = false;
        options.maxSteps = 1000;
        options.neuronCount = 500;
        options.regionCount = 1;
        options.randomSeed = 42;
        options.timestep = 0.001f;

        for (int i = 1; i < argc; ++i) {
            string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                options.list = true;
            } else if (arg == "--verbose") {
                options.verbose = true;
            } else if (arg == "--quiet") {
                options.quiet = true;
            } else if (arg == "--demo") {
                options.runDemo = true;
            } else if (arg == "--tests") {
                options.runTests = true;
            } else if (arg == "--save-config") {
                options.saveConfig = true;
            } else if (arg == "--load-config") {
                options.loadConfig = true;
            } else if (arg == "--generate-config") {
                options.generateConfig = true;
            } else if (arg == "--benchmark") {
                options.benchmark = true;
            } else if (arg == "--simulate") {
                options.simulate = true;
            } else if (arg == "--output" || arg == "-o") {
                if (i + 1 < argc) options.outputFile = argv[++i];
            } else if (arg == "--log-file") {
                if (i + 1 < argc) options.logFile = argv[++i];
            } else if (arg == "--config" || arg == "-c") {
                if (i + 1 < argc) options.configFile = argv[++i];
            } else if (arg == "--experiment") {
                if (i + 1 < argc) options.experimentFile = argv[++i];
            } else if (arg == "--checkpoint") {
                if (i + 1 < argc) options.checkpointFile = argv[++i];
            } else if (arg == "--replay") {
                if (i + 1 < argc) options.replayFile = argv[++i];
            } else if (arg == "--max-steps") {
                if (i + 1 < argc) options.maxSteps = stoi(argv[++i]);
            } else if (arg == "--neuron-count") {
                if (i + 1 < argc) options.neuronCount = stoi(argv[++i]);
            } else if (arg == "--region-count") {
                if (i + 1 < argc) options.regionCount = stoi(argv[++i]);
            } else if (arg == "--seed") {
                if (i + 1 < argc) options.randomSeed = stoi(argv[++i]);
            } else if (arg == "--timestep") {
                if (i + 1 < argc) options.timestep = stof(argv[++i]);
            } else if (arg.substr(0, 7) == "--set=") {
                string value = arg.substr(7);
                options.customArgs.push_back("--" + value);
            } else if (arg.substr(0, 2) == "--") {
                options.customArgs.push_back(arg);
            }
        }
        
        return options;
    }

    void printHelp() {
        cout << "NLM — Neural Learning Machine" << endl;
        cout << "Version 0.1.0" << endl;
        cout << endl;
        cout << "Usage: nlm [OPTIONS] [SUBCOMMAND] [ARGS]" << endl;
        cout << endl;
        cout << "Commands:" << endl;
        cout << "  demo          Run the basic demonstration (Phase 2)" << endl;
        cout << "  tests         Run unit tests" << endl;
        cout << "  experiment    Run a named experiment" << endl;
        cout << "  benchmark     Run performance benchmark" << endl;
        cout << "  simulate      Run simulation from checkpoint" << endl;
        cout << endl;
        cout << "Options:" << endl;
        cout << "  --help, -h               Show this help message" << endl;
        cout << "  --config <file>, -c <file>  Load configuration from file" << endl;
        cout << "  --output <file>, -o <file>  Save output to file" << endl;
        cout << "  --log-file <file>         Log to file in addition to console" << endl;
        cout << "  --verbose                 Enable verbose output" << endl;
        cout << "  --quiet                   Suppress non-essential output" << endl;
        cout << "  --save-config             Save current configuration" << endl;
        cout << "  --load-config             Load configuration" << endl;
        cout << "  --generate-config         Generate default configuration" << endl;
        cout << "  --max-steps <n>           Set maximum simulation steps" << endl;
        cout << "  --neuron-count <n>        Set number of neurons" << endl;
        cout << "  --region-count <n>        Set number of regions" << endl;
        cout << "  --seed <n>                Set random seed" << endl;
        cout << "  --timestep <t>            Set simulation timestep" << endl;
        cout << "  --checkpoint <file>       Save checkpoint to file" << endl;
        cout << "  --replay <file>           Replay from checkpoint" << endl;
        cout << "  --set=<key>=<value>        Set configuration parameter" << endl;
        cout << endl;
        cout << "Examples:" << endl;
        cout << "  nlm --demo --config myconfig.cfg" << endl;
        cout << "  nlm --benchmark --neuron-count 1000 --max-steps 5000" << endl;
        cout << "  nlm experiment --name my_exp --config config.json" << endl;
    }

    void initializeSystem(const CommandLineOptions& options) {
        // Initialize logger with file output if specified
        auto logger = make_shared<Logger>();
        auto consoleLogger = make_shared<ConsoleLogger>(options.quiet ? LogLevel::Warning : LogLevel::Info);
        
        if (!options.logFile.empty()) {
            consoleLogger->setOutputFile(options.logFile);
        }
        
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        if (!options.quiet) {
            NLM_LOG_INFO("=== NLM System Initialized ===");
        }
    }

    void generateDefaultConfig() {
        auto config = make_shared<Config>();
        
        // Set default values
        config->set("random_seed", 42, ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", 500, ConfigSource::Default);
        config->set("region_count", 1, ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        // STDP parameters
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
        
        // Structural plasticity parameters
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
        
        // Save to JSON file
        if (config->saveToJsonFile("configs/default.json")) {
            NLM_LOG_INFO("Default configuration saved to configs/default.json");
        }
        
        // Save to YAML file
        if (config->saveToYamlFile("configs/default.yaml")) {
            NLM_LOG_INFO("Default configuration saved to configs/default.yaml");
        }
        
        // Save to simple format
        if (config->saveToFile("configs/default.cfg")) {
            NLM_LOG_INFO("Default configuration saved to configs/default.cfg");
        }
    }

    bool runDemo(const CommandLineOptions& options) {
        NLM_LOG_INFO("=== NLM Phase 2 Demo ===");
        
        // Create and initialize brain
        auto config = make_shared<Config>();
        
        // Load config if specified
        if (!options.configFile.empty()) {
            if (!options.configFile.empty() && 
                (options.configFile.ends_with(".json") || options.configFile.ends_with(".yaml") || 
                 options.configFile.ends_with(".yml"))) {
                if (!config->loadFromJsonFile(options.configFile)) {
                    NLM_LOG_ERROR("Failed to load JSON/YAML config, using defaults");
                }
            } else {
                if (!config->loadFromFile(options.configFile)) {
                    NLM_LOG_ERROR("Failed to load config file, using defaults");
                }
            }
        }
        
        // Override with command line options
        config->set("random_seed", options.randomSeed, ConfigSource::CommandLine);
        config->set("simulation_timestep", options.timestep, ConfigSource::CommandLine);
        config->set("neuron_count", options.neuronCount, ConfigSource::CommandLine);
        config->set("region_count", options.regionCount, ConfigSource::CommandLine);
        
        // Process custom arguments
        for (const auto& arg : options.customArgs) {
            if (arg.find('=') != string::npos) {
                size_t pos = arg.find('=');
                string key = arg.substr(0, pos);
                string value = arg.substr(pos + 1);
                config->set(key, value, ConfigSource::CommandLine);
            }
        }
        
        // Create brain
        auto brain = make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain");
            return false;
        }
        
        NLM_LOG_INFO("Brain initialized with " + to_string(brain->getTotalNeuronCount()) + " neurons");
        
        // Run basic connectivity test
        runBasicConnectivityTest(brain);
        
        // Reset and run plasticity experiment
        brain->reset();
        brain->initialize();
        runPlasticityExperiment(brain);
        
        // Reset and run STDP verification
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
        
        // Final brain status
        NLM_LOG_INFO("=== Demo Complete ===");
        return true;
    }

    bool runBenchmark(const CommandLineOptions& options) {
        NLM_LOG_INFO("=== NLM Performance Benchmark ===");
        
        auto startTime = chrono::high_resolution_clock::now();
        
        // Create and initialize brain
        auto config = make_shared<Config>();
        config->set("random_seed", options.randomSeed, ConfigSource::Default);
        config->set("simulation_timestep", options.timestep, ConfigSource::Default);
        config->set("neuron_count", options.neuronCount, ConfigSource::Default);
        config->set("region_count", options.regionCount, ConfigSource::Default);
        
        auto brain = make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain for benchmark");
            return false;
        }
        
        // Run benchmark simulation
        NLM_LOG_INFO("Running benchmark with " + to_string(options.neuronCount) + " neurons for " + 
                    to_string(options.maxSteps) + " steps");
        
        for (SimulationStep step = 0; step < options.maxSteps; ++step) {
            brain->step(step, options.timestep);
            
            if (step % 100 == 0) {
                NLM_LOG_INFO("Step " + to_string(step) + ": " + to_string(brain->getFiringNeuronCount()) + 
                            " active neurons, " + to_string(brain->getTotalSpikeCount()) + " spikes");
            }
        }
        
        auto endTime = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);
        
        NLM_LOG_INFO("=== Benchmark Results ===");
        NLM_LOG_INFO("Duration: " + to_string(duration.count() / 1000.0) + " seconds");
        NLM_LOG_INFO("Neurons: " + to_string(options.neuronCount));
        NLM_LOG_INFO("Steps: " + to_string(options.maxSteps));
        NLM_LOG_INFO("Total spikes: " + to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("Average firing rate: " + to_string(brain->getAverageFiringRate()) + " Hz");
        
        return true;
    }

    bool runExperiment(const CommandLineOptions& options) {
        NLM_LOG_INFO("=== NLM Experiment Runner ===");
        
        if (options.experimentFile.empty()) {
            NLM_LOG_ERROR("Experiment file not specified");
            return false;
        }
        
        // Load experiment configuration
        auto config = make_shared<Config>();
        if (!options.configFile.empty()) {
            if (!options.configFile.empty() && 
                (options.configFile.ends_with(".json") || options.configFile.ends_with(".yaml") || 
                 options.configFile.ends_with(".yml"))) {
                config->loadFromJsonFile(options.configFile);
            } else {
                config->loadFromFile(options.configFile);
            }
        }
        
        // Override with command line options
        config->set("random_seed", options.randomSeed, ConfigSource::CommandLine);
        config->set("simulation_timestep", options.timestep, ConfigSource::CommandLine);
        config->set("neuron_count", options.neuronCount, ConfigSource::CommandLine);
        config->set("region_count", options.regionCount, ConfigSource::CommandLine);
        
        // Create and initialize brain
        auto brain = make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain for experiment");
            return false;
        }
        
        // Create experiment
        auto experiment = make_shared<Experiment>(options.experimentFile);
        
        // Run experiment
        if (options.benchmark) {
            return runBenchmark(options);
        } else {
            return runDemo(options);
        }
    }

    void runTests() {
        NLM_LOG_INFO("=== NLM Unit Tests ===");
        
        // Note: This would require implementing proper unit test framework
        // For now, just run the basic demo
        NLM_LOG_INFO("Unit tests not yet implemented - running demo instead");
        CommandLineOptions options;
        options.runDemo = true;
        runDemo(options);
    }

} // namespace

// Original demo functions from main.cpp
void printBanner() {
    cout << R"(
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
    )" << endl;
}

void runBasicConnectivityTest(shared_ptr<Brain> brain) {
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
    for (size_t i = 0; i < min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
    }
    
    // Run a few steps
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("  Spikes generated: " + to_string(spikes));
    
    if (spikes > 0) {
        NLM_LOG_INFO("  ✓ Spikes propagate through network");
    } else {
        NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
        for (size_t i = 0; i < min(size_t(3), neurons.size()); ++i) {
            NLM_LOG_INFO("    Neuron " + to_string(i) + 
                        " V=" + to_string(neurons[i]->getMembranePotential()) +
                        " thresh=" + to_string(neurons[i]->getThreshold()));
        }
    }
}

void runPlasticityExperiment(shared_ptr<Brain> brain) {
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
            NLM_LOG_INFO("  Step " + to_string(step) + 
                        " | Spikes: " + to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + to_string(brain->getFiringNeuronCount()));
        }
    }
    
    // Record final state
    experiment.recordFinalState();
    
    // Compute and display statistics
    experiment.computeStatistics();
}

void runStdpVerification(shared_ptr<Brain> brain) {
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
    vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
        NLM_LOG_INFO("    Synapse " + to_string(i) + 
                    " before: " + to_string(beforeWeights[i]));
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
        NLM_LOG_INFO("    Synapse " + to_string(i) + 
                    " after: " + to_string(synapses[i]->getWeight()) +
                    " (Δ=" + to_string(delta) + ")");
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
    auto options = parseArguments(argc, argv);
    
    if (options.list) {
        printHelp();
        return 0;
    }
    
    initializeSystem(options);
    
    // Handle subcommands
    if (options.runDemo) {
        return runDemo(options) ? 0 : 1;
    } else if (options.runTests) {
        runTests();
        return 0;
    } else if (!options.experimentFile.empty()) {
        return runExperiment(options) ? 0 : 1;
    } else if (options.benchmark) {
        return runBenchmark(options) ? 0 : 1;
    } else {
        // Default behavior: run demo
        return runDemo(options) ? 0 : 1;
    }
}
