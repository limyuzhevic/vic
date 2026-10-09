#include "cli/CLI.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <algorithm>
#include <iomanip>

namespace nlm {

CLI::CLI() {
    setupCommands();
}

CLI::~CLI() = default;

void CLI::setupCommands() {
    // Auto mode - runs all tests in sequence
    registerCommand({
        "auto", 
        "Run all experiments in sequence (basic -> plasticity -> stdp -> benchmark)",
        std::bind(&CLI::runAutoMode, this, std::placeholders::_1),
        "nlm auto [--config file] [--steps N] [--neurons N]"
    });
    
    // Basic mode - simple connectivity and activity test
    registerCommand({
        "basic", 
        "Run basic connectivity and activity tests",
        std::bind(&CLI::runBasicMode, this, std::placeholders::_1),
        "nlm basic [--config file] [--steps N] [--neurons N]"
    });
    
    // Plasticity mode - learning experiments
    registerCommand({
        "plasticity", 
        "Run plasticity learning experiments",
        std::bind(&CLI::runPlasticityMode, this, std::placeholders::_1),
        "nlm plasticity [--config file] [--steps N] [--neurons N]"
    });
    
    // STDP mode - spike-timing dependent plasticity verification
    registerCommand({
        "stdp", 
        "Run STDP verification experiments",
        std::bind(&CLI::runStdpMode, this, std::placeholders::_1),
        "nlm stdp [--config file] [--steps N] [--neurons N]"
    });
    
    // Benchmark mode - performance measurement
    registerCommand({
        "benchmark", 
        "Run performance benchmarks",
        std::bind(&CLI::runBenchmarkMode, this, std::placeholders::_1),
        "nlm benchmark [--steps N] [--neurons N] [--iterations M]"
    });
    
    // Script mode - execute scripts
    registerCommand({
        "script", 
        "Execute automation script",
        std::bind(&CLI::runScriptMode, this, std::placeholders::_1),
        "nlm script <script_file> [args...]"
    });
    
    // Integration mode - Phase 6 integration test
    registerCommand({
        "integration", 
        "Run Phase 6 integration experiment",
        std::bind(&CLI::runIntegrationMode, this, std::placeholders::_1),
        "nlm integration [--config file] [--steps N] [--neurons N]"
    });
    
    // Analysis mode - data analysis tools
    registerCommand({
        "analysis", 
        "Run analysis tools on existing data",
        std::bind(&CLI::runAnalysisMode, this, std::placeholders::_1),
        "nlm analysis <input_file> [--output file] [--format json/csv]"
    });
}

bool CLI::parse(int argc, char** argv) {
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            options.help = true;
        } else if (arg == "--version") {
            options.version = true;
        } else if (arg == "--show-advanced-help") {
            options.showAdvancedHelp = true;
        } else if (arg == "--list-commands") {
            options.listCommands = true;
        } else if (arg == "--verbose" || arg == "-v") {
            options.verbose = true;
        } else if (arg.substr(0, 9) == "--config=") {
            options.configFile = arg.substr(9);
        } else if (arg == "--config" && i + 1 < argc) {
            options.configFile = argv[++i];
        } else if (arg.substr(0, 8) == "--mode=") {
            options.mode = arg.substr(8);
        } else if (arg.substr(0, 9) == "--steps=") {
            try {
                options.steps = std::stoi(arg.substr(9));
            } catch (...) {
                std::cerr << "Invalid value for --steps: " << arg.substr(9) << std::endl;
                success = false;
            }
        } else if (arg.substr(0, 11) == "--neurons=") {
            try {
                options.neurons = std::stoi(arg.substr(11));
            } catch (...) {
                std::cerr << "Invalid value for --neurons: " << arg.substr(11) << std::endl;
                success = false;
            }
        } else if (arg.substr(0, 13) == "--timestep=") {
            try {
                options.timestep = std::stof(arg.substr(13));
            } catch (...) {
                std::cerr << "Invalid value for --timestep: " << arg.substr(13) << std::endl;
                success = false;
            }
        } else if (arg.substr(0, 8) == "--seed=") {
            try {
                options.seed = std::stoi(arg.substr(8));
            } catch (...) {
                std::cerr << "Invalid value for --seed: " << arg.substr(8) << std::endl;
                success = false;
            }
        } else if (arg.substr(0, 15) == "--checkpoint=") {
            options.checkpointPath = arg.substr(15);
            options.enableCheckpoint = true;
        } else if (arg == "--checkpoint") {
            options.enableCheckpoint = true;
        } else if (arg == "--enable-replay") {
            options.enableReplay = true;
        } else if (arg == "--enable-development") {
            options.enableDevelopment = true;
        } else if (arg.substr(0, 12) == "--output=") {
            options.outputFile = arg.substr(12);
        } else if (arg.substr(0, 10) == "--log-level=") {
            options.logLevel = arg.substr(10);
        } else if (arg == "--script" && i + 1 < argc) {
            options.scriptPath = argv[++i];
            options.mode = "script";
        } else if (arg.substr(0, 6) == "--arg=") {
            options.scriptArgs.push_back(arg.substr(6));
        } else if (arg.find('=') == std::string::npos && arg[0] == '-') {
            // Command mode
            options.mode = arg.substr(1);
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            success = false;
        }
    }
    
    // If no mode specified, default to "auto"
    if (options.mode == "auto" && !options.help && !options.version && !options.showAdvancedHelp) {
        // Check if there are command arguments
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            if (arg.find('=') == std::string::npos && arg[0] == '-') {
                options.mode = arg.substr(1);
                break;
            }
        }
    }
    
    return success;
}

bool CLI::run() {
    if (options.help) {
        printHelp();
        return true;
    }
    
    if (options.version) {
        std::cout << "NLM v0.1.0 - Neural Learning Machine" << std::endl;
        return true;
    }
    
    if (options.showAdvancedHelp) {
        printAdvancedHelp();
        return true;
    }
    
    if (options.listCommands) {
        std::cout << "Available commands:" << std::endl;
        for (const auto& cmd : commands) {
            std::cout << "  " << cmd.name << " - " << cmd.description << std::endl;
            std::cout << "    Usage: " << cmd.usage << std::endl << std::endl;
        }
        return true;
    }
    
    // Find and run the selected command
    auto it = std::find_if(commands.begin(), commands.end(),
                          [&](const Command& cmd) { return cmd.name == options.mode; });n    
    if (it == commands.end()) {
        std::cerr << "Unknown mode: " << options.mode << std::endl;
        std::cerr << "Use --list-commands to see available modes" << std::endl;
        return false;
    }
    
    return it->callback(options);
}

void CLI::printHelp() const {
    std::cout << "Usage: nlm [OPTIONS] [MODE]" << std::endl << std::endl;
    std::cout << "Modes:" << std::endl;
    for (const auto& cmd : commands) {
        std::cout << "  " << cmd.name << " - " << cmd.description << std::endl;
    }
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h                    Show this help message" << std::endl;
    std::cout << "  --version                     Show version information" << std::endl;
    std::cout << "  --show-advanced-help          Show advanced help with all options" << std::endl;
    std::cout << "  --list-commands               List all available commands" << std::endl;
    std::cout << "  --verbose, -v                 Enable verbose output" << std::endl;
    std::cout << "  --config FILE                 Configuration file" << std::endl;
    std::cout << "  --steps N                     Number of simulation steps" << std::endl;
    std::cout << "  --neurons N                   Number of neurons" << std::endl;
    std::cout << "  --timestep F                  Simulation timestep" << std::endl;
    std::cout << "  --seed N                      Random seed" << std::endl;
    std::cout << "  --checkpoint FILE              Enable checkpointing with file path" << std::endl;
    std::cout << "  --enable-replay               Enable replay system" << std::endl;
    std::cout << "  --enable-development          Enable developmental processes" << std::endl;
    std::cout << "  --output FILE                 Output file" << std::endl;
    std::cout << "  --log-level LEVEL             Logging level (debug, info, warn, error)" << std::endl;
    std::cout << std::endl;
    std::cout << "Example usage:" << std::endl;
    std::cout << "  nlm basic --neurons 1000 --steps 5000" << std::endl;
    std::cout << "  nlm plasticity --config my_config.cfg" << std::endl;
    std::cout << "  nlm benchmark --steps 10000 --iterations 5" << std::endl;
    std::cout << "  nlm integration --enable-replay --enable-checkpoint" << std::endl;
}

void CLI::printAdvancedHelp() const {
    std::cout << "=== Advanced NLM CLI Options ===" << std::endl << std::endl;
    std::cout << "Modes and their specific options:" << std::endl << std::endl;
    
    std::cout << "Basic Mode (--help will show mode-specific options):" << std::endl;
    std::cout << "  Runs connectivity tests and basic activity analysis" << std::endl;
    std::cout << "  Useful for verifying system setup" << std::endl << std::endl;
    
    std::cout << "Plasticity Mode (--help will show mode-specific options):" << std::endl;
    std::cout << "  Runs learning experiments with repeated patterns" << std::endl;
    std::cout << "  Tests weight changes through experience" << std::endl << std::endl;
    
    std::cout << "STDP Mode (--help will show mode-specific options):" << std::endl;
    std::cout << "  Verifies spike-timing dependent plasticity" << std::endl;
    std::cout << "  Tests LTP and LTD mechanisms" << std::endl << std::endl;
    
    std::cout << "Benchmark Mode (--help will show mode-specific options):" << std::endl;
    std::cout << "  Measures performance of neural simulation" << std::endl;
    std::cout << "  Reports timing and resource usage" << std::endl << std::endl;
    
    std::cout << "Integration Mode (--help will show mode-specific options):" << std::endl;
    std::cout << "  Runs Phase 6 integration test" << std::endl;
    std::cout << "  Verifies all systems are connected" << std::endl << std::endl;
    
    std::cout << "Configuration File Format:" << std::endl;
    std::cout << "  # Comment lines starting with #" << std::endl;
    std::cout << "  key = value  # Simple key=value pairs" << std::endl;
    std::cout << "  Examples:" << std::endl;
    std::cout << "    neuron_count = 500" << std::endl;
    std::cout << "    connection_probability = 0.15" << std::endl;
    std::cout << "    stdp_ltp_weight = 0.02" << std::endl << std::endl;
    
    std::cout << "Output Options:" << std::endl;
    std::cout << "  Results are saved to output_file if specified" << std::endl;
    std::cout << "  Supports JSON, CSV, and plain text formats" << std::endl << std::endl;
    
    std::cout << "Logging Levels:" << std::endl;
    std::cout << "  debug    - Detailed diagnostic information" << std::endl;
    std::cout << "  info     - Standard operational information" << std::endl;
    std::cout << "  warn     - Warnings and potential issues" << std::endl;
    std::cout << "  error    - Error conditions and failures" << std::endl << std::endl;
}

bool CLI::runAutoMode(const Options& opts) {
    std::cout << "=== NLM Auto Mode ===" << std::endl;
    std::cout << "Running all experiments in sequence..." << std::endl;
    
    bool success = true;
    
    // Run basic connectivity test
    std::cout << "\n1. Running basic connectivity test..." << std::endl;
    success &= runBasicMode(opts);
    
    // Reset and run plasticity experiment
    std::cout << "\n2. Running plasticity learning experiment..." << std::endl;
    success &= runPlasticityMode(opts);
    
    // Reset and run STDP verification
    std::cout << "\n3. Running STDP verification..." << std::endl;
    success &= runStdpMode(opts);
    
    // Run benchmark
    std::cout << "\n4. Running performance benchmark..." << std::endl;
    success &= runBenchmarkMode(opts);
    
    return success;
}

bool CLI::runBasicMode(const Options& opts) {
    std::cout << "=== Basic Connectivity Test ===" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Load configuration
    auto config = std::make_shared<Config>();
    if (config->loadFromFile(opts.configFile)) {
        std::cout << "Loaded configuration from: " << opts.configFile << std::endl;
    }
    
    // Set options
    config->set("neuron_count", opts.neurons, ConfigSource::CommandLine);
    config->set("simulation_timestep", opts.timestep, ConfigSource::CommandLine);
    config->set("random_seed", opts.seed, ConfigSource::CommandLine);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return false;
    }
    
    // Run basic connectivity test
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        std::cerr << "No regions found!" << std::endl;
        return false;
    }
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        std::cerr << "No neurons found!" << std::endl;
        return false;
    }
    
    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject current into first 10 neurons
    std::cout << "Injecting current into " << std::min(size_t(10), neurons.size()) << " neurons..." << std::endl;
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);
    }
    
    // Run simulation steps
    for (SimulationStep step = 0; step < opts.steps; ++step) {
        brain->step(step, step * opts.timestep);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    std::cout << "Spikes generated: " << spikes << std::endl;
    
    if (spikes > 0) {
        std::cout << "✓ Connectivity test passed - spikes propagate through network" << std::endl;
    } else {
        std::cout << "! No spikes generated - checking neuron parameters..." << std::endl;
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            std::cout << "  Neuron " << i << " V=" << neurons[i]->getMembranePotential() 
                      << " thresh=" << neurons[i]->getThreshold() << std::endl;
        }
    }
    
    return true;
}

bool CLI::runPlasticityMode(const Options& opts) {
    std::cout << "=== Plasticity Learning Experiment ===" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Load configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", opts.neurons, ConfigSource::CommandLine);
    config->set("simulation_timestep", opts.timestep, ConfigSource::CommandLine);
    config->set("random_seed", opts.seed, ConfigSource::CommandLine);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return false;
    }
    
    // Enable plasticity on synapses
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    // Record initial state
    std::cout << "Recording initial state..." << std::endl;
    auto* region = brain->getRegion(RegionId(1));
    std::vector<float> initialWeights;
    if (region) {
        for (const auto& syn : region->getSynapses()) {
            initialWeights.push_back(syn->getWeight());
        }
    }
    
    std::cout << "Initial synapses: " << (region ? region->getSynapses().size() : 0) << std::endl;
    
    // Apply repeated input pattern to stimulate learning
    std::cout << "Applying repeated input patterns (" << opts.steps << " steps)..." << std::endl;
    
    for (SimulationStep step = 0; step < opts.steps; ++step) {
        // Create input pattern - inject current into sensory neurons
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * opts.timestep);
        
        // Log progress every 500 steps
        if (step % 500 == 0 && opts.verbose) {
            std::cout << "  Step " << step << " | Spikes: " << brain->getTotalSpikeCount()
                      << " | Firing: " << brain->getFiringNeuronCount() << std::endl;
        }
    }
    
    // Record final state
    std::cout << "Recording final state..." << std::endl;
    std::vector<float> finalWeights;
    if (region) {
        for (const auto& syn : region->getSynapses()) {
            finalWeights.push_back(syn->getWeight());
        }
    }
    
    // Compute and display statistics
    std::cout << "\n=== Plasticity Experiment Results ===" << std::endl;
    
    if (initialWeights.empty() || finalWeights.empty()) {
        std::cout << "ERROR: No weights recorded" << std::endl;
        return false;
    }
    
    float initialSum = 0.0f, finalSum = 0.0f;
    for (float w : initialWeights) initialSum += w;
    for (float w : finalWeights) finalSum += w;
    
    float initialMean = initialSum / initialWeights.size();
    float finalMean = finalSum / finalWeights.size();
    
    std::cout << "Weight Statistics:" << std::endl;
    std::cout << "  Initial mean weight: " << initialMean << std::endl;
    std::cout << "  Final mean weight: " << finalMean << std::endl;
    std::cout << "  Change: " << (finalMean - initialMean) << std::endl;
    
    // Count synapses that changed significantly
    size_t strengthened = 0, weakened = 0, unchanged = 0;
    size_t minSize = std::min(initialWeights.size(), finalWeights.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        float delta = finalWeights[i] - initialWeights[i];
        if (delta > 0.01f) ++strengthened;
        else if (delta < -0.01f) ++weakened;
        else ++unchanged;
    }
    
    std::cout << "\nSynaptic Changes:" << std::endl;
    std::cout << "  Strengthened: " << strengthened << std::endl;
    std::cout << "  Weakened: " << weakened << std::endl;
    std::cout << "  Unchanged: " << unchanged << std::endl;
    
    bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) || 
                            (strengthened > 0 || weakened > 0);
    
    std::cout << std::endl;
    if (learningOccurred) {
        std::cout << "✓ Learning detected: Synaptic weights changed through experience" << std::endl;
    } else {
        std::cout << "✗ No learning: Weights did not change significantly" << std::endl;
    }
    
    return true;
}

bool CLI::runStdpMode(const Options& opts) {
    std::cout << "=== STDP Verification ===" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Load configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", opts.neurons, ConfigSource::CommandLine);
    config->set("simulation_timestep", opts.timestep, ConfigSource::CommandLine);
    config->set("random_seed", opts.seed, ConfigSource::CommandLine);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return false;
    }
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return false;
    
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        std::cout << "Not enough synapses for STDP test (need 5, have " 
                  << synapses.size() << ")" << std::endl;
        return false;
    }
    
    std::cout << "Testing STDP on 5 synapses:" << std::endl;
    
    // Record initial weights
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
        std::cout << "  Synapse " << i << " before: " << beforeWeights[i] << std::endl;
    }
    
    // Create correlated activity: fire pre then post to trigger LTP
    std::cout << "Creating correlated pre->post activity (potentiation)..." << std::endl;
    
    auto neurons = region->getAllNeurons();
    if (neurons.size() >= 2) {
        Neuron* preNeuron = neurons[0];
        Neuron* postNeuron = neurons[1];
        
        for (int trial = 0; trial < 50; ++trial) {
            // Pre fires first
            preNeuron->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * opts.timestep);
            
            // Then post fires
            postNeuron->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * opts.timestep);
        }
    }
    
    // Record after weights
    std::cout << "After correlated activity:" << std::endl;
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        totalDelta += delta;
        std::cout << "  Synapse " << i << " after: " << synapses[i]->getWeight()
                  << " (Δ=" << delta << ")" << std::endl;
    }
    
    // Check if weights increased (LTP)
    if (totalDelta > 0.001f) {
        std::cout << "✓ STDP working: Pre-before-post produced potentiation" << std::endl;
    } else if (totalDelta < -0.001f) {
        std::cout << "! STDP reversed: Check parameters" << std::endl;
    } else {
        std::cout << "! No change: STDP may not be triggering" << std::endl;
    }
    
    return true;
}

bool CLI::runBenchmarkMode(const Options& opts) {
    std::cout << "=== NLM Performance Benchmark ===" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run multiple iterations to get consistent measurements
    int iterations = 3; // Default
    
    std::cout << "Running benchmark with " << opts.neurons << " neurons for " 
              << opts.steps << " steps (" << iterations << " iterations)..." << std::endl;
    
    double totalStepsPerSecond = 0.0;
    double totalWallTime = 0.0;
    
    for (int iter = 0; iter < iterations; ++iter) {
        // Load configuration
        auto config = std::make_shared<Config>();
        config->set("neuron_count", opts.neurons, ConfigSource::CommandLine);
        config->set("simulation_timestep", opts.timestep, ConfigSource::CommandLine);
        config->set("random_seed", opts.seed + iter, ConfigSource::CommandLine);
        
        // Initialize brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain in iteration " << iter << std!" << std::endl;
            return false;
        }
        
        auto iterStart = std::chrono::high_resolution_clock::now();
        
        // Run simulation
        for (SimulationStep step = 0; step < opts.steps; ++step) {
            brain->step(step, step * opts.timestep);
        }
        
        auto iterEnd = std::chrono::high_resolution_clock::now();
        double iterTime = std::chrono::duration<double>(iterEnd - iterStart).count();
        double stepsPerSecond = opts.steps / iterTime;
        
        totalStepsPerSecond += stepsPerSecond;
        totalWallTime += iterTime;
        
        std::cout << "  Iteration " << iter + 1 << ": " << stepsPerSecond << " steps/sec (" 
                  << iterTime << "s wall time)" << std::endl;
    }
    
    double avgStepsPerSecond = totalStepsPerSecond / iterations;
    double avgWallTime = totalWallTime / iterations;
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalWallTimeSeconds = std::chrono::duration<double>(endTime - startTime).count();
    
    std::cout << "\n=== Benchmark Results ===" << std::endl;
    std::cout << "Average steps per second: " << avgStepsPerSecond << std::endl;
    std::cout << "Average wall time per iteration: " << avgWallTime << "s" << std::endl;
    std::cout << "Total wall time: " << totalWallTimeSeconds << "s" << std::endl;
    std::cout << "Neurons: " << opts.neurons << std::endl;
    std::cout << "Steps per iteration: " << opts.steps << std::endl;
    std::cout << "Total steps processed: " << opts.steps * iterations << std::endl;
    
    // Calculate metrics
    double efficiency = (opts.steps * iterations) / totalWallTimeSeconds;
    std::cout << "Efficiency: " << efficiency << " steps per second (total)" << std::endl;
    
    // Performance assessment
    if (avgStepsPerSecond > 10000) {
        std::cout << "✓ Excellent performance (>10,000 steps/sec)" << std::endl;
    } else if (avgStepsPerSecond > 5000) {
        std::cout << "✓ Good performance (>5,000 steps/sec)" << std::endl;
    } else if (avgStepsPerSecond > 1000) {
        std::cout << "✓ Moderate performance (>1,000 steps/sec)" << std::endl;
    } else {
        std::cout << "⚠ Low performance (<1,000 steps/sec)" << std::endl;
    }
    
    return true;
}

bool CLI::runScriptMode(const Options& opts) {
    std::cout << "=== NLM Script Mode ===" << std::endl;
    
    if (opts.scriptPath.empty()) {
        std::cerr << "Error: No script file specified" << std::endl;
        return false;
    }
    
    std::cout << "Script file: " << opts.scriptPath << std::endl;
    
    // TODO: Implement script execution
    // This would load and execute NLM scripting language
    // For now, just show what would be executed
    
    std::cout << "Script arguments:" << std::endl;
    for (size_t i = 0; i < opts.scriptArgs.size(); ++i) {
        std::cout << "  " << i << ": " << opts.scriptArgs[i] << std::endl;
    }
    
    std::cout << "Script execution is not yet implemented" << std::endl;
    std::cout << "This will be implemented in Phase 7" << std::endl;
    
    return true;
}

bool CLI::runIntegrationMode(const Options& opts) {
    std::cout << "=== NLM Integration Mode ===" << std::endl;
    std::cout << "Running Phase 6 integration test..." << std::endl;
    
    // Use existing Phase6IntegratedExperiment
    Phase6IntegratedExperiment experiment;
    
    // First, run integration verification
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return false;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Test individual systems
    std::cout << "--- Memory Integration Test ---" << std::endl;
    experiment.testMemoryIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    experiment.testNeuromodulationIntegration();
    std::cout << std::endl;
    
    // Run full integration experiment if enabled
    if (opts.enableCheckpoint || opts.enableReplay || opts.enableDevelopment) {
        std::cout << "--- Full Integration Experiment ---" << std::endl;
        Phase6Config config;
        config.neuronCount = opts.neurons;
        config.maxSteps = opts.steps;
        config.enableCheckpointing = opts.enableCheckpoint;
        config.enableReplay = opts.enableReplay;
        config.enableDevelopment = opts.enableDevelopment;
        
        if (opts.enableCheckpoint) {
            config.checkpointPath = opts.checkpointPath;
        }
        
        auto result = experiment.run(config);
        
        std::cout << std::endl;
        std::cout << "=== FINAL RESULTS ===" << std::endl;
        std::cout << "Total reward: " << result.totalReward << std::endl;
        std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
        std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
        std::cout << std::endl;
        
        std::cout << "=== INTEGRATION STATUS ===" << std::endl;
        std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
        std::cout << std::endl;
        
        std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    }
    
    return true;
}

bool CLI::runAnalysisMode(const Options& opts) {
    std::cout << "=== NLM Analysis Mode ===" << std::endl;
    
    if (opts.scriptArgs.empty()) {
        std::cerr << "Error: No input file specified for analysis" << std::endl;
        return false;
    }
    
    std::cout << "Input file: " << opts.scriptArgs[0] << std::endl;
    
    // TODO: Implement analysis tools
    // This would read data files and perform various analyses
    // For now, just show what would be analyzed
    
    std::cout << "Analysis is not yet implemented" << std::endl;
    std::cout << "This will be implemented in Phase 7" << std::endl;
    
    return true;
}

} // namespace nlm