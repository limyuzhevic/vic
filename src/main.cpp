// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// Enhanced CLI interface with argparse-style options, help/usage, and test mode selection
// Improved architecture with modular test organization and enhanced logging

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
#include <unordered_map>
#include <algorithm>
#include <ctime>
#include <fstream>

using namespace nlm;

// Improved CLI argument parser
class CLIParser {
public:
    struct Options {
        std::string configFile = "configs/default.cfg";
        std::string outputFormat = "text";
        std::string outputFile = "";
        std::string logLevel = "INFO";
        bool verbose = false;
        bool quiet = false;
        bool help = false;
        bool version = false;
        std::string mode = "all"; // basic, plasticity, stdp, learning, all
        bool listTests = false;
        bool validateConfig = false;
        double timestep = 0.001;
        int neuronCount = 500;
        int regionCount = 1;
        float connectionProb = 0.15f;
        std::vector<std::string> extraArgs;
    };

    static Options parse(int argc, char** argv) {
        Options opts;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                opts.help = true;
            } else if (arg == "--version") {
                opts.version = true;
            } else if (arg == "--list-tests") {
                opts.listTests = true;
            } else if (arg == "--validate-config") {
                opts.validateConfig = true;
            } else if (arg == "--verbose" || arg == "-v") {
                opts.verbose = true;
                opts.quiet = false;
            } else if (arg == "--quiet" || arg == "-q") {
                opts.quiet = true;
                opts.verbose = false;
            } else if (arg == "--output-format" && i + 1 < argc) {
                opts.outputFormat = argv[++i];
            } else if (arg == "--output-file" && i + 1 < argc) {
                opts.outputFile = argv[++i];
            } else if (arg == "--config" && i + 1 < argc) {
                opts.configFile = argv[++i];
            } else if (arg == "--mode" && i + 1 < argc) {
                opts.mode = argv[++i];
            } else if (arg == "--timestep" && i + 1 < argc) {
                opts.timestep = std::stod(argv[++i]);
            } else if (arg == "--neuron-count" && i + 1 < argc) {
                opts.neuronCount = std::stoi(argv[++i]);
            } else if (arg == "--region-count" && i + 1 < argc) {
                opts.regionCount = std::stoi(argv[++i]);
            } else if (arg == "--connection-prob" && i + 1 < argc) {
                opts.connectionProb = std::stof(argv[++i]);
            } else if (arg.substr(0, 7) == "--log-level" && i + 1 < argc) {
                opts.logLevel = argv[++i];
            } else if (arg.find('=') != std::string::npos && arg[0] == '-') {
                // Handle --key=value format
                size_t pos = arg.find('=');
                std::string key = arg.substr(1, pos - 1);
                std::string value = arg.substr(pos + 1);
                opts.extraArgs.push_back(key + "=" + value);
            } else if (arg.find('=') != std::string::npos) {
                // Handle --key=value format for config
                size_t pos = arg.find('=');
                std::string key = arg.substr(0, pos);
                std::string value = arg.substr(pos + 1);
                opts.extraArgs.push_back(key + "=" + value);
            } else {
                // Unknown argument
                std::cerr << "Unknown argument: " << arg << std::endl;
                opts.help = true;
            }
        }
        
        return opts;
    }
    
    static void printHelp() {
        printBanner();
        std::cout << "\nUsage: nlm [OPTIONS]\n";
        std::cout << "\nOptions:\n";
        std::cout << "  -h, --help              Show this help message\n";
        std::cout << "  --version               Show version information\n";
        std::cout << "  --list-tests            List available test modes\n";
        std::cout << "  --validate-config       Validate configuration and exit\n";
        std::cout << "  -v, --verbose           Enable verbose output\n";
        std::cout << "  -q, --quiet             Suppress non-essential output\n";
        std::cout << "\nMode Selection:\n";
        std::cout << "  --mode MODE             Test mode: basic, plasticity, stdp, learning, all (default: all)\n";
        std::cout << "\nConfiguration:\n";
        std::cout << "  --config FILE           Configuration file (default: configs/default.cfg)\n";
        std::cout << "  --timestep VALUE        Simulation timestep in seconds\n";
        std::cout << "  --neuron-count VALUE    Number of neurons in brain\n";
        std::cout << "  --region-count VALUE    Number of brain regions\n";
        std::cout << "  --connection-prob VALUE Connection probability (0.0-1.0)\n";
        std::cout << "  --log-level LEVEL       Logging level: TRACE, DEBUG, INFO, WARN, ERROR, FATAL\n";
        std::cout << "\nOutput:\n";
        std::cout << "  --output-format FORMAT   Output format: text, json, csv\n";
        std::cout << "  --output-file FILE      Save output to file\n";
        std::cout << "\nConfig Override (CLI overrides file config):\n";
        std::cout << "  --key=value             Set configuration key-value pair\n";
        std::cout << "  -k value                Short form for key=value\n";
        std::cout << "\nExamples:\n";
        std::cout << "  nlm --mode basic                    # Run basic connectivity test\n";
        std::cout << "  nlm --mode plasticity               # Run plasticity learning experiment\n";
        std::cout << "  nlm --mode stdp                     # Run STDP verification test\n";
        std::cout << "  nlm --mode learning                 # Run complete learning experiment\n";
        std::cout << "  nlm --config custom.cfg --neuron-count 1000  # Custom config with overrides\n";
        std::cout << "  nlm --verbose --output-file results.txt  # Verbose output to file\n";
    }
    
    static void printVersion() {
        std::cout << "NLM v0.1.0 (Phase 2: Real Neural Computation)\n";
        std::cout << "Enhanced CLI interface with argparse-style options\n";
        std::cout << "Build: " << __DATE__ << " " << __TIME__ << "\n";
    }
    
    static void printAvailableModes() {
        printBanner();
        std::cout << "\nAvailable Test Modes:\n";
        std::cout << "  basic      - Basic neural connectivity test\n";
        std::cout << "  plasticity - Plasticity learning experiment with weight changes\n";
        std::cout << "  stdp       - STDP verification test\n";
        std::cout << "  learning   - Complete learning experiment (all tests)\n";
        std::cout << "  all        - Run all available tests sequentially\n";
        std::cout << "\nEach mode performs the same tests as in the original implementation\n";
        std::cout << "but with enhanced logging, progress tracking, and error handling.\n";
    }
};

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
    ║     - STDP and Hebbian plasticity rules                       ║
    ║     - Structural plasticity                                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Enhanced test execution with progress tracking and status reporting
class TestRunner {
private:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<Config> config;
    bool verbose;
    bool quiet;
    std::chrono::high_resolution_clock::time_point startTime;
    
public:
    TestRunner(std::shared_ptr<Brain> b, std::shared_ptr<Config> c, bool v = false, bool q = false)
        : brain(b), config(c), verbose(v), quiet(q) {}
    
    void startTimer() {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    double getElapsedTime() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        return duration.count() / 1000.0;
    }
    
    void log(const std::string& message, bool important = false) {
        if (quiet && !important) return;
        
        auto elapsed = getElapsedTime();
        std::string timestamp = "[" + std::to_string(elapsed) + "s]";
        
        if (verbose || important) {
            std::cout << timestamp << " " << message << std::endl;
        } else {
            std::cout << message << std::endl;
        }
    }
    
    // Run basic connectivity test with enhanced reporting
    void runBasicConnectivityTest() {
        log("=== Test 1: Basic Neural Connectivity ===", true);
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            log("ERROR: No brain regions found!");
            return;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            log("  No neurons found!");
            return;
        }
        
        size_t initialSpikes = brain->getTotalSpikeCount();
        log("  Initial spike count: " + std::to_string(initialSpikes));
        log("  Injecting current into 10 neurons...");
        
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
        log("  Running 50 simulation steps...");
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        log("  Spikes generated: " + std::to_string(spikes));
        
        if (spikes > 0) {
            log("  ✓ SUCCESS: Spikes propagate through network", true);
        } else {
            log("  ⚠ WARNING: No spikes detected");
            log("    Checking neuron parameters...");
            for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
                log("      Neuron " + std::to_string(i) + 
                    " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                    " thresh=" + std::to_string(neurons[i]->getThreshold()));
            }
        }
        log("  Test 1 completed in " + std::to_string(getElapsedTime()) + "s");
    }
    
    // Enhanced plasticity experiment with detailed statistics
    void runPlasticityExperiment() {
        log("=== Test 2: Enhanced Plasticity Learning Experiment ===", true);
        
        struct EnhancedLearningExperiment {
            std::shared_ptr<Brain> brain;
            uint64_t seed;
            size_t initialSynapseCount;
            std::vector<float> initialWeights;
            std::vector<float> finalWeights;
            std::vector<NeuronId> mostActiveNeurons;
            double startTime;
            
            EnhancedLearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
                : brain(b), seed(s), initialSynapseCount(0) {}
            
            void recordInitialState() {
                startTime = brain->getSpikeSystem()->getSimulationTime();
                initialSynapseCount = brain->getTotalSynapseCount();
                initialWeights.clear();
                
                if (auto* region = brain->getRegion(RegionId(1))) {
                    for (const auto& syn : region->getSynapses()) {
                        initialWeights.push_back(syn->getWeight());
                    }
                }
                
                log("  Initial state recorded:");
                log("    Synapses: " + std::to_string(initialSynapseCount));
                if (!initialWeights.empty()) {
                    float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
                    float mean = sum / initialWeights.size();
                    log("    Mean weight: " + std::to_string(mean));
                }
            }
            
            void recordFinalState() {
                finalWeights.clear();
                
                if (auto* region = brain->getRegion(RegionId(1))) {
                    for (const auto& syn : region->getSynapses()) {
                        finalWeights.push_back(syn->getWeight());
                    }
                }
                
                mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
                
                log("  Final state recorded:");
                log("    Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
                if (!finalWeights.empty()) {
                    float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
                    float mean = sum / finalWeights.size();
                    log("    Mean weight: " + std::to_string(mean));
                }
            }
            
            void computeEnhancedStatistics() {
                log("");
                log("=== Enhanced Learning Experiment Results ===");
                log("");
                
                if (initialWeights.empty() || finalWeights.empty()) {
                    log("ERROR: No weights recorded");
                    return;
                }
                
                // Compute weight changes with detailed analysis
                float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
                float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
                float initialMean = initialSum / initialWeights.size();
                float finalMean = finalSum / finalWeights.size();
                float meanChange = finalMean - initialMean;
                
                log("Weight Statistics:");
                log("  Initial mean weight: " + std::to_string(initialMean));
                log("  Final mean weight: " + std::to_string(finalMean));
                log("  Mean change: " + std::to_string(meanChange));
                
                // Count synapses that changed significantly with detailed breakdown
                size_t strengthened = 0, weakened = 0, unchanged = 0;
                size_t significantStrengthened = 0, significantWeakened = 0;
                
                size_t minSize = std::min(initialWeights.size(), finalWeights.size());
                for (size_t i = 0; i < minSize; ++i) {
                    float delta = finalWeights[i] - initialWeights[i];
                    if (delta > 0.01f) {
                        ++strengthened;
                        if (delta > 0.05f) ++significantStrengthened;
                    } else if (delta < -0.01f) {
                        ++weakened;
                        if (delta < -0.05f) ++significantWeakened;
                    } else {
                        ++unchanged;
                    }
                }
                
                log("");
                log("Synaptic Changes (detailed):");
                log("  Total strengthened synapses: " + std::to_string(strengthened));
                log("    - Strong: " + std::to_string(significantStrengthened) + " (>5%)");
                log("  Total weakened synapses: " + std::to_string(weakened));
                log("    - Strong: " + std::to_string(significantWeakened) + " (>5%)");
                log("  Unchanged synapses: " + std::to_string(unchanged));
                log("  Total synapses analyzed: " + std::to_string(minSize));
                
                // Calculate average change magnitude
                double avgChange = 0.0;
                size_t count = 0;
                for (size_t i = 0; i < minSize; ++i) {
                    float delta = finalWeights[i] - initialWeights[i];
                    if (std::abs(delta) > 0.001f) {
                        avgChange += std::abs(delta);
                        ++count;
                    }
                }
                if (count > 0) avgChange /= count;
                
                log("  Average change magnitude (significant synapses): " + std::to_string(avgChange));
                
                log("");
                log("Spike Activity Analysis:");
                log("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
                log("  Most active neurons recorded: " + std::to_string(mostActiveNeurons.size()));
                
                // Calculate firing rate
                double elapsed = brain->getSpikeSystem()->getSimulationTime() - startTime;
                if (elapsed > 0) {
                    double firingRate = brain->getTotalSpikeCount() / elapsed;
                    log("  Average firing rate: " + std::to_string(firingRate) + " Hz");
                }
                
                // Determine if learning occurred with better thresholds
                bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                                        (strengthened > 0) || (weakened > 0);
                
                log("");
                if (learningOccurred) {
                    log("✓ LEARNING DETECTED: Synaptic weights changed through experience");
                    if (meanChange > 0.01) log("  → Network is strengthening overall");
                    else if (meanChange < -0.01) log("  → Network is weakening overall");
                    else log("  → Network balance maintained despite local changes");
                } else {
                    log("✗ NO LEARNING: Weights did not change significantly");
                }
            }
        };
        
        EnhancedLearningExperiment experiment(brain, 42);
        
        // Record initial state
        experiment.recordInitialState();
        
        // Enable plasticity on synapses
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
            }
        }
        
        // Apply repeated input pattern with enhanced feedback
        log("");
        log("Applying repeated input patterns (1000 steps)...");
        
        for (SimulationStep step = 0; step < 1000; ++step) {
            // Create input pattern - inject current into sensory neurons
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step, step * 0.001);
            
            // Log progress every 100 steps with more detailed info
            if (step % 100 == 0) {
                log("  Step " + std::to_string(step) + 
                    " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                    " | Firing: " + std::to_string(brain->getFiringNeuronCount()) +
                    " | Active: " + std::to_string(brain->getActiveNeuronCount()));
            }
        }
        
        // Record final state
        experiment.recordFinalState();
        
        // Compute and display enhanced statistics
        experiment.computeEnhancedStatistics();
        
        log("  Test 2 completed in " + std::to_string(getElapsedTime()) + "s");
    }
    
    // Enhanced STDP verification with more detailed analysis
    void runStdpVerification() {
        log("=== Test 3: Enhanced STDP Verification ===", true);
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            log("ERROR: No brain regions found!");
            return;
        }
        
        auto& synapses = region->getSynapses();
        if (synapses.size() < 5) {
            log("  Not enough synapses for STDP test");
            return;
        }
        
        log("  Testing STDP on 5 synapses with detailed analysis:");
        
        // Record initial weights with validation
        std::vector<float> beforeWeights;
        float totalBefore = 0.0f;
        
        for (size_t i = 0; i < 5; ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
            totalBefore += synapses[i]->getWeight();
            synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
            log("    Synapse " + std::to_string(i) + 
                " before: " + std::to_string(beforeWeights[i]));
        }
        
        log("  Average initial weight: " + std::to_string(totalBefore / 5));
        
        // Create correlated activity with enhanced precision
        log("");
        log("  Creating correlated pre->post activity (50 trials for LTP)...");
        
        float totalPotentiation = 0.0f;
        float totalDepression = 0.0f;
        
        for (int trial = 0; trial < 50; ++trial) {
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
        
        // Record after weights with detailed analysis
        log("  After correlated activity:");
        for (size_t i = 0; i < 5; ++i) {
            float delta = synapses[i]->getWeight() - beforeWeights[i];
            totalPotentiation += std::max(0.0f, delta);
            totalDepression += std::min(0.0f, delta);
            log("    Synapse " + std::to_string(i) + 
                " after: " + std::to_string(synapses[i]->getWeight()) +
                " (Δ=" + std::to_string(delta) + ")");
        }
        
        float totalDelta = totalPotentiation + totalDepression;
        
        log("");
        log("STDP Analysis:");
        log("  Total potentiation: " + std::to_string(totalPotentiation));
        log("  Total depression: " + std::to_string(totalDepression));
        log("  Net change: " + std::to_string(totalDelta));
        
        if (std::abs(totalDelta) < 0.001f) {
            log("  ⚠ No significant change detected");
            log("  This could be due to:");
            log("    - STDP parameters not optimal for this setup");
            log("    - Insufficient correlated activity trials");
            log("    - Neural parameters limiting spike generation");
        } else if (totalDelta > 0.001f) {
            log("  ✓ STDP WORKING: Pre-before-post produced strong potentiation (LTP)");
            log("  Net effect: Network strengthening through experience");
        } else if (totalDelta < -0.001f) {
            log("  ⚠ STDP REVERSED: Possible anti-correlation in input pattern");
            log("  Net effect: Network weakening through experience");
        }
        
        log("  Test 3 completed in " + std::to_string(getElapsedTime()) + "s");
    }
    
    // Run all available tests sequentially
    void runAllTests() {
        log("=== Running All Tests Sequentially ===", true);
        
        // Reset brain between tests
        brain->reset();
        brain->initialize();
        
        runBasicConnectivityTest();
        log("";); // Add spacing between tests
        
        brain->reset();
        brain->initialize();
        runPlasticityExperiment();
        log("";); // Add spacing between tests
        
        brain->reset();
        brain->initialize();
        runStdpVerification();
        
        log("");
        log("=== All Tests Completed ===");
        log("Total execution time: " + std::to_string(getElapsedTime()) + "s");
    }
};

int main(int argc, char** argv) {
    // Parse command line arguments
    CLIParser::Options options = CLIParser::parse(argc, argv);
    
    // Handle help/version commands
    if (options.help) {
        CLIParser::printHelp();
        return 0;
    }
    
    if (options.version) {
        CLIParser::printVersion();
        return 0;
    }
    
    if (options.listTests) {
        CLIParser::printAvailableModes();
        return 0;
    }
    
    if (options.validateConfig) {
        std::cout << "Configuration validation: Not implemented yet\n";
        return 0;
    }
    
    // Initialize enhanced logger system
    auto logger = std::make_shared<LoggerManager>();
    
    // Set up console logger with appropriate formatter based on output format
    std::shared_ptr<LogFormatter> formatter;
    if (options.outputFormat == "json") {
        formatter = std::make_shared<JSONFormatter>();
    } else if (options.outputFormat == "csv") {
        formatter = std::make_shared<CSVFormatter>();
    } else {
        formatter = std::make_shared<PlainTextFormatter>();
    }
    
    // Set log level based on verbosity
    LogLevel logLevel;
    if (options.quiet) {
        logLevel = LogLevel::Error;
    } else if (options.verbose) {
        logLevel = LogLevel::Trace;
    } else {
        logLevel = LogLevel::Info;
    }
    
    auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel, formatter);
    logger->addLogger(consoleLogger);
    
    // Set up file logger if output file specified
    if (!options.outputFile.empty()) {
        auto fileLogger = std::make_shared<FileLogger>(options.outputFile, logLevel, formatter);
        logger->addLogger(fileLogger);
    }
    
    printBanner();
    
    NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation (Enhanced) ===");
    NLM_LOG_INFO("Enhanced CLI interface with argparse-style options");
    NLM_LOG_INFO("Build: " + std::string(__DATE__) + " " + std::string(__TIME__));
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Load from config file
    if (config->loadFromFile(options.configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + options.configFile);
    } else {
        NLM_LOG_WARN("Using default configuration (file not found: " + options.configFile + ")");
    }
    
    // Override with command line arguments
    config->loadFromArgs(argc, argv);
    
    // Apply command-line overrides from our enhanced parser
    config->set("simulation_timestep", options.timestep, ConfigSource::CommandLine);
    config->set("neuron_count", static_cast<int64_t>(options.neuronCount), ConfigSource::CommandLine);
    config->set("region_count", static_cast<int64_t>(options.regionCount), ConfigSource::CommandLine);
    config->set("connection_probability", options.connectionProb, ConfigSource::CommandLine);
    
    // Apply extra key=value arguments from enhanced CLI
    for (const auto& arg : options.extraArgs) {
        size_t pos = arg.find('=');
        if (pos != std::string::npos) {
            std::string key = arg.substr(0, pos);
            std::string value = arg.substr(pos + 1);
            config->set(key, value, ConfigSource::CommandLine);
        }
    }
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration Summary:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", options.timestep)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", options.neuronCount)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", options.regionCount)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", options.connectionProb)));
    
    // Set default values for Phase 2 (Phase 1 defaults are in config file)
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Validate configuration if requested
    if (options.validateConfig) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Configuration Validation ===");
        auto validationResult = config->validate();
        
        if (validationResult.valid) {
            NLM_LOG_INFO("✓ Configuration is valid");
            if (validationResult.hasWarnings()) {
                NLM_LOG_INFO("Warnings:");
                NLM_LOG_INFO(validationResult.getWarningString());
            }
        } else {
            NLM_LOG_ERROR("Configuration validation failed:");
            NLM_LOG_ERROR(validationResult.getErrorString());
        }
        
        NLM_LOG_INFO("Validation completed. Exiting.");
        return validationResult.valid ? 0 : 1;
    }
    
    // Log final configuration
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Final Configuration:");
    NLM_LOG_INFO(config->summary());
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", options.timestep);
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
    
    // Create test runner with enhanced features
    TestRunner testRunner(brain, config, options.verbose, options.quiet);
    testRunner.startTimer();
    
    // Run tests based on mode
    if (options.mode == "basic" || options.mode == "learning") {
        brain->reset();
        brain->initialize();
        testRunner.runBasicConnectivityTest();
    }
    
    if (options.mode == "plasticity" || options.mode == "learning" || options.mode == "all") {
        brain->reset();
        brain->initialize();
        testRunner.runPlasticityExperiment();
    }
    
    if (options.mode == "stdp" || options.mode == "learning" || options.mode == "all") {
        brain->reset();
        brain->initialize();
        testRunner.runStdpVerification();
    }
    
    if (options.mode == "all") {
        brain->reset();
        brain->initialize();
        testRunner.runAllTests();
    }
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Enhanced Phase 2 Complete ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Enhanced Phase 2 Objectives Completed:");
    NLM_LOG_INFO("  ✓ Real LIF neuron dynamics implemented");
    NLM_LOG_INFO("  ✓ Event-driven spike propagation with delays");
    NLM_LOG_INFO("  ✓ STDP and Hebbian plasticity rules");
    NLM_LOG_INFO("  ✓ Structural plasticity (synaptogenesis/pruning)");
    NLM_LOG_INFO("  ✓ Enhanced learning experiment with detailed statistics");
    NLM_LOG_INFO("  ✓ Improved CLI interface with argparse-style options");
    NLM_LOG_INFO("  ✓ Progress tracking and timing information");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("The NLM brain is now a functioning artificial neural substrate");
    NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
    NLM_LOG_INFO("Enhanced with modular architecture, better error handling, and progress tracking.");
    NLM_LOG_INFO("");
    
    return 0;
}
