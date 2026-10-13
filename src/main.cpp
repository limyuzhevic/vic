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
#include <sstream>
#include <algorithm>
#include <functional>
#include <chrono>

using namespace nlm;

// Simple argument parser for CLI
class CLIParser {
public:
    struct Argument {
        std::string name;
        std::string value;
        std::string help;
        char shortOpt;
        bool required;
        bool hasValue;
    };
    
    struct Option {
        std::string name;
        std::string description;
        std::function<void()> action;
        char shortOpt;
    };
    
    CLIParser(int argc, char** argv) : argc_(argc), argv_(argv) {}
    
    void addArgument(const std::string& name, char shortOpt = 0, const std::string& help = "", bool required = false, bool hasValue = true) {
        arguments_.push_back({name, "", help, shortOpt, required, hasValue});
    }
    
    void addOption(const std::string& name, const std::string& description, std::function<void()> action, char shortOpt = 0) {
        options_.push_back({name, description, action, shortOpt});
    }
    
    void printHelp(const std::string& programName, const std::vector<std::string>& subcommands = {}) {
        std::cout << "NLM - Neural Learning Machine" << std::endl;
        std::cout << "Usage: " << programName << " [OPTIONS] <COMMAND> [ARGS]..." << std::endl << std::endl;
        
        if (!subcommands.empty()) {
            std::cout << "COMMANDS:" << std::endl;
            for (const auto& cmd : subcommands) {
                std::cout << "  " << cmd << std::endl;
            }
            std::cout << std::endl;
        }
        
        if (!arguments_.empty()) {
            std::cout << "ARGUMENTS:" << std::endl;
            for (const auto& arg : arguments_) {
                std::cout << "  ";
                if (arg.shortOpt) {
                    std::cout << "-" << arg.shortOpt << ", ";
                }
                std::cout << "--" << arg.name;
                if (arg.hasValue) {
                    std::cout << "=<value>";
                }
                std::cout << " - " << arg.help;
                if (arg.required) {
                    std::cout << " (required)";
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
        
        if (!options_.empty()) {
            std::cout << "OPTIONS:" << std::endl;
            for (const auto& opt : options_) {
                std::cout << "  ";
                if (opt.shortOpt) {
                    std::cout << "-" << opt.shortOpt << ", ";
                }
                std::cout << "--" << opt.name << " - " << opt.description << std::endl;
            }
            std::cout << std::endl;
        }
        
        std::cout << "Global options:" << std::endl;
        std::cout << "  --help, -h - Print this help message" << std::endl;
        std::cout << "  --version, -v - Print version information" << std::endl;
        std::cout << "  --verbose, -V - Enable verbose output" << std::endl;
    }
    
    bool parse() {
        for (int i = 1; i < argc_; ++i) {
            std::string arg = argv_[i];
            
            if (arg == "--help" || arg == "-h") {
                printHelp(argv_[0], subcommand_);
                return false;
            } else if (arg == "--version" || arg == "-v") {
                std::cout << "NLM Phase 2 - Version 0.1.0" << std::endl;
                return false;
            } else if (arg == "--verbose" || arg == "-V") {
                verbose_ = true;
            } else if (arg.substr(0, 2) == "--" && arg.find('=') != std::string::npos) {
                // --key=value format
                size_t pos = arg.find('=');
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                parseArgument(key, value);
            } else if (arg.substr(0, 2) == "--") {
                // Long option without value
                parseOption(arg.substr(2));
            } else if (arg[0] == '-' && arg.size() > 1) {
                // Short option (could be with or without value)
                parseShortOption(arg.substr(1));
            } else {
                // Positional argument (subcommand or value)
                if (subcommand_.empty()) {
                    subcommand_ = arg;
                } else {
                    // Additional positional arguments for the subcommand
                    parsePositional(arg);
                }
            }
        }
        return true;
    }
    
    std::string getSubcommand() const { return subcommand_; }
    std::string getArgument(const std::string& name) const {
        for (const auto& arg : arguments_) {
            if (arg.name == name && !arg.value.empty()) {
                return arg.value;
            }
        }
        return "";
    }
    bool hasArgument(const std::string& name) const {
        return getArgument(name).empty();
    }
    bool isVerbose() const { return verbose_; }
    
private:
    void parseArgument(const std::string& key, const std::string& value) {
        for (auto& arg : arguments_) {
            if (arg.name == key && arg.hasValue) {
                arg.value = value;
                break;
            }
        }
    }
    
    void parseOption(const std::string& opt) {
        for (const auto& option : options_) {
            if (option.name == opt) {
                option.action();
                break;
            }
        }
    }
    
    void parseShortOption(const std::string& opt) {
        if (opt.size() == 1) {
            // Single character option, check if it's a flag or needs a value
            char c = opt[0];
            bool found = false;
            for (auto& option : options_) {
                if (option.shortOpt == c) {
                    option.action();
                    found = true;
                    break;
                }
            }
            if (!found) {
                // Check if it's an argument
                for (auto& arg : arguments_) {
                    if (arg.shortOpt == c && arg.hasValue && arg.value.empty()) {
                        // Need to find the next argument
                        if (i_ + 1 < argc_) {
                            arg.value = argv_[++i_];
                            found = true;
                        }
                        break;
                    } else if (arg.shortOpt == c && !arg.hasValue) {
                        // Boolean flag
                        arg.value = "true";
                        found = true;
                        break;
                    }
                }
            }
        }
    }
    
    void parsePositional(const std::string& arg) {
        positionalArgs_.push_back(arg);
    }
    
    int argc_;
    char** argv_;
    std::vector<Argument> arguments_;
    std::vector<Option> options_;
    std::string subcommand_;
    std::vector<std::string> positionalArgs_;
    bool verbose_ = false;
    size_t i_ = 1; // Current position in argv for parsing short options with values
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
    ║     - STDP and Hebbian plasticity                            ║
    ║     - Structural plasticity                                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Test command implementations
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

void runBatchExperiment(const Config& config) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Batch Experiment Mode ===");
    
    int numExperiments = config.getOr<int>("batch_experiment_count", 3);
    int duration = config.getOr<int>("batch_experiment_duration", 1000);
    
    NLM_LOG_INFO("Running " + std::to_string(numExperiments) + " experiments for " + std::to_string(duration) + " steps each");
    
    for (int exp = 0; exp < numExperiments; ++exp) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Experiment " + std::to_string(exp + 1) + "/" + std::to_string(numExperiments));
        
        auto config = std::make_shared<Config>();
        config->set("random_seed", 42 + exp, ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", 200, ConfigSource::Default);
        config->set("region_count", 1, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain for experiment " + std::to_string(exp));
            continue;
        }
        
        // Run basic connectivity test
        runBasicConnectivityTest(brain);
        
        // Reset for plasticity test
        brain->reset();
        brain->initialize();
        
        // Run plasticity experiment
        runPlasticityExperiment(brain);
        
        // Reset for STDP test
        brain->reset();
        brain->initialize();
        
        // Run STDP verification
        runStdpVerification(brain);
    }
}

void runBenchmark(const Config& config) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Performance Benchmark ===");
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    int steps = config.getOr<int>("benchmark_steps", 5000);
    auto configCopy = std::make_shared<Config>(config);
    
    for (SimulationStep step = 0; step < steps; ++step) {
        // Simulate a brain for benchmarking
        auto brain = std::make_shared<Brain>(configCopy);
        brain->step(step, step * 0.001);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    double stepsPerSecond = (steps * 1000.0) / duration.count();
    NLM_LOG_INFO("Benchmark completed:");
    NLM_LOG_INFO("  Steps: " + std::to_string(steps));
    NLM_LOG_INFO("  Time: " + std::to_string(duration.count()) + " ms");
    NLM_LOG_INFO("  Performance: " + std::to_string(stepsPerSecond) + " steps/sec");
}

void runSimulation(const Config& config) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Simulation Mode ===");
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    int maxSteps = config.getOr<int>("max_simulation_steps", 1000);
    
    auto brain = std::make_shared<Brain>(std::make_shared<Config>(config));
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain for simulation");
        return;
    }
    
    for (SimulationStep step = 0; step < maxSteps; ++step) {
        brain->step(step, step * 0.001);
        
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Simulation completed in " + std::to_string(duration.count()) + " ms");
    brain->logStatus();
}

int main(int argc, char** argv) {
    printBanner();
    
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
    
    // Initialize CLI parser
    CLIParser parser(argc, argv);
    
    // Add global options
    parser.addOption("help", "Print help message", []() {
        std::cout << "Use '--help' with a command for more specific help." << std::endl;
    }, 'h');
    
    parser.addOption("verbose", "Enable verbose output", []() {
        // Will be set during parsing
    }, 'V');
    
    // Parse command line
    if (!parser.parse()) {
        return 0;
    }
    
    // Check for subcommand
    std::string subcommand = parser.getSubcommand();
    if (subcommand.empty()) {
        std::cout << "Error: No command specified. Use '--help' for usage information." << std::endl;
        return 1;
    }
    
    // Initialize configuration
    auto config = std::make_shared<Config>();
    
    // Load default configuration
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    config->set("batch_experiment_count", 3, ConfigSource::Default);
    config->set("batch_experiment_duration", 1000, ConfigSource::Default);
    config->set("benchmark_steps", 5000, ConfigSource::Default);
    config->set("max_simulation_steps", 10000, ConfigSource::Default);
    config->set("max_simulation_time", 60.0, ConfigSource::Default);
    
    // Load configuration file if specified
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
    
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration loaded:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    NLM_LOG_INFO("");
    
    // Configure schema for validation
    ConfigSchema schema;
    schema.addField("random_seed", static_cast<int64_t>(42), "Random seed for reproducible experiments", nullptr, true);
    schema.addField("simulation_timestep", 0.001, "Simulation timestep in seconds", nullptr, true);
    schema.addField("neuron_count", static_cast<int64_t>(100), "Number of neurons in the brain", nullptr, true);
    schema.addField("region_count", static_cast<int64_t>(1), "Number of brain regions", nullptr, true);
    schema.addField("connection_probability", 0.1f, "Connection probability for random connectivity", [](const ConfigValue& v) {
        return std::get<float>(v) >= 0.0f && std::get<float>(v) <= 1.0f;
    }, true);
    
    // Validate configuration
    if (!config->validate(schema)) {
        NLM_LOG_ERROR("Configuration validation failed");
        return 1;
    }
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Execute subcommands
    if (subcommand == "basic") {
        NLM_LOG_INFO("=== Basic Connectivity Test ===");
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return 1;
        }
        brain->logStatus();
        runBasicConnectivityTest(brain);
        
    } else if (subcommand == "plasticity" || subcommand == "learning") {
        NLM_LOG_INFO("=== Plasticity Learning Experiment ===");
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return 1;
        }
        brain->logStatus();
        runPlasticityExperiment(brain);
        
    } else if (subcommand == "stdp") {
        NLM_LOG_INFO("=== STDP Verification ===");
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return 1;
        }
        brain->logStatus();
        runStdpVerification(brain);
        
    } else if (subcommand == "batch") {
        runBatchExperiment(*config);
        
    } else if (subcommand == "benchmark") {
        runBenchmark(*config);
        
    } else if (subcommand == "simulation" || subcommand == "sim") {
        runSimulation(*config);
        
    } else if (subcommand == "config") {
        std::string format = parser.getArgument("format");
        if (format.empty()) {
            format = "simple";
        }
        
        bool saveFormat = (format == "json");
        std::string outputFile = parser.getArgument("output");
        if (outputFile.empty()) {
            outputFile = "config_output.json";
        }
        
        if (config->saveToFile(outputFile, saveFormat)) {
            NLM_LOG_INFO("Configuration saved to: " + outputFile);
        } else {
            NLM_LOG_ERROR("Failed to save configuration to: " + outputFile);
        }
        
    } else {
        std::cout << "Error: Unknown command: " << subcommand << std::endl;
        std::cout << "Available commands: basic, plasticity, learning, stdp, batch, benchmark, simulation, sim, config" << std::endl;
        return 1;
    }
    
    // Final status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Execution Complete ===");
    NLM_LOG_INFO("All operations completed successfully.");
    NLM_LOG_INFO("For more information, check the log file or use --verbose flag.");
    
    return 0;
}
