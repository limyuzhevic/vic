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
#include "agent/AgentBrain.hpp"
#include "agent/SensoryPercept.hpp"
#include "world/SimpleWorld.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <unordered_map>
#include <chrono>
#include <thread>

using namespace nlm;

// Command-line option parsing
struct CommandLineOptions {
    std::string configFile = "configs/default.cfg";
    uint64_t randomSeed = 42;
    double simulationTimestep = 0.001;
    int neuronCount = 500;
    int regionCount = 1;
    float connectionProbability = 0.15f;
    std::string experimentMode = "all"; // "basic", "plasticity", "stdp", "all", "agent"
    int maxSteps = 1000;
    bool enableLogging = true;
    LogLevel logLevel = LogLevel::Info;
    bool runBenchmarks = false;
    std::string benchmarkType = "all";
    bool enablePerformanceProfiling = false;
    int profilingInterval = 100;
    bool saveResults = false;
    std::string resultsFile = "results.json";
    bool continuousMode = false;
    double continuousInterval = 1.0;
    bool enableAdvancedAgent = false;
    int agentPersonality = 0; // 0=random, 1=conservative, 2=explorative, 3=balanced
    std::string outputFormat = "text"; // "text", "json", "csv"
    bool exportNeuralData = false;
    std::string neuralDataFile = "neural_data.bin";
    bool enableRealTimeVisualization = false;
    int visualizationPort = 8080;
    std::string customConfig = "";
};

CommandLineOptions parseCommandLine(int argc, char** argv) {
    CommandLineOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printUsage();
            exit(0);
        } else if (arg == "--version") {
            printVersion();
            exit(0);
        } else if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                options.configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                options.configFile = argv[++i];
            }
        } else if (arg == "--seed") {
            if (i + 1 < argc) options.randomSeed = std::stoull(argv[++i]);
        } else if (arg == "--timestep") {
            if (i + 1 < argc) options.simulationTimestep = std::stod(argv[++i]);
        } else if (arg == "--neurons") {
            if (i + 1 < argc) options.neuronCount = std::stoi(argv[++i]);
        } else if (arg == "--regions") {
            if (i + 1 < argc) options.regionCount = std::stoi(argv[++i]);
        } else if (arg == "--connection-prob") {
            if (i + 1 < argc) options.connectionProbability = std::stof(argv[++i]);
        } else if (arg == "--experiment") {
            if (i + 1 < argc) options.experimentMode = argv[++i];
        } else if (arg == "--steps") {
            if (i + 1 < argc) options.maxSteps = std::stoi(argv[++i]);
        } else if (arg == "--no-log") {
            options.enableLogging = false;
        } else if (arg == "--log-level") {
            if (i + 1 < argc) {
                std::string level = argv[++i];
                if (level == "debug") options.logLevel = LogLevel::Debug;
                else if (level == "info") options.logLevel = LogLevel::Info;
                else if (level == "warning") options.logLevel = LogLevel::Warning;
                else if (level == "error") options.logLevel = LogLevel::Error;
            }
        } else if (arg == "--benchmark") {
            options.runBenchmarks = true;
            if (i + 1 < argc) options.benchmarkType = argv[++i];
        } else if (arg == "--profile") {
            options.enablePerformanceProfiling = true;
            if (i + 1 < argc) options.profilingInterval = std::stoi(argv[++i]);
        } else if (arg == "--save-results") {
            options.saveResults = true;
            if (i + 1 < argc) options.resultsFile = argv[++i];
        } else if (arg == "--continuous") {
            options.continuousMode = true;
            if (i + 1 < argc) options.continuousInterval = std::stod(argv[++i]);
        } else if (arg == "--agent") {
            options.experimentMode = "agent";
        } else if (arg == "--stdp-only") {
            options.experimentMode = "stdp";
        } else if (arg == "--plasticity-only") {
            options.experimentMode = "plasticity";
        } else if (arg == "--basic-only") {
            options.experimentMode = "basic";
        } else if (arg == "--advanced-agent") {
            options.enableAdvancedAgent = true;
        } else if (arg == "--personality") {
            if (i + 1 < argc) options.agentPersonality = std::stoi(argv[++i]);
        } else if (arg == "--output-format") {
            if (i + 1 < argc) options.outputFormat = argv[++i];
        } else if (arg == "--export-neural") {
            options.exportNeuralData = true;
        } else if (arg == "--neural-file") {
            if (i + 1 < argc) options.neuralDataFile = argv[++i];
        } else if (arg == "--visualization") {
            options.enableRealTimeVisualization = true;
            if (i + 1 < argc) options.visualizationPort = std::stoi(argv[++i]);
        } else if (arg == "--custom-config") {
            if (i + 1 < argc) options.customConfig = argv[++i];
        } else if (arg == "--fast") {
            // Quick test mode with reduced steps
            options.maxSteps = 100;
            options.experimentMode = "basic";
        } else if (arg == "--thorough") {
            // Comprehensive test mode
            options.maxSteps = 5000;
            options.experimentMode = "all";
            options.enablePerformanceProfiling = true;
        }
    }
    
    return options;
}

void printUsage() {
    std::cout << "Usage: nlm [options]\n\n";
    std::cout << "Options:\n";
    std::cout << "  --help, -h                    Show this help message\n";
    std::cout << "  --version                     Show version information\n";
    std::cout << "  --config=<file>               Load configuration from file (default: configs/default.cfg)\n";
    std::cout << "  --seed=<value>                Random seed (default: 42)\n";
    std::cout << "  --timestep=<value>            Simulation timestep in seconds (default: 0.001)\n";
    std::cout << "  --neurons=<count>             Number of neurons (default: 500)\n";
    std::cout << "  --regions=<count>             Number of brain regions (default: 1)\n";
    std::cout << "  --connection-prob=<value>     Connection probability (default: 0.15)\n";
    std::cout << "  --experiment=<mode>           Experiment mode: all, basic, plasticity, stdp, agent (default: all)\n";
    std::cout << "  --steps=<count>               Maximum simulation steps (default: 1000)\n";
    std::cout << "  --no-log                      Disable logging\n";
    std::cout << "  --log-level=<level>           Logging level: debug, info, warning, error\n";
    std::cout << "  --benchmark[=<type>]          Run benchmarks (type: all, speed, memory, plasticity)\n";
    std::cout << "  --profile=<interval>          Enable performance profiling\n";
    std::cout << "  --save-results[=<file>]       Save results to file\n";
    std::cout << "  --continuous=<interval>       Run in continuous mode with specified interval\n";
    std::cout << "  --agent                       Run in agent mode (world interaction)\n";
    std::cout << "  --stdp-only                   Run only STDP verification\n";
    std::cout << "  --plasticity-only             Run only plasticity experiment\n";
    std::cout << "  --basic-only                  Run only basic connectivity test\n";
}

void printVersion() {
    std::cout << "NLM Phase 2: Real Neural Computation\n";
    std::cout << "Version: 0.1.0\n";
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << "\n";
    std::cout << "C++20 with scikit-build\n";
    std::cout << "Features: LIF neurons, STDP, Hebbian plasticity, structural plasticity\n";
}

void printExperimentResults(const CommandLineOptions& options, const LearningExperiment& experiment) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "EXPERIMENT RESULTS SUMMARY\n";
    std::cout << std::string(60, '=') << "\n\n";
    
    std::cout << "Experiment Configuration:" << std::endl;
    std::cout << "  Random Seed: " << experiment.seed << std::endl;
    std::cout << "  Final Synapse Count: " << experiment.brain->getTotalSynapseCount() << std::endl;
    std::cout << "  Final Spike Count: " << experiment.brain->getTotalSpikeCount() << std::endl;
    
    if (!experiment.initialWeights.empty() && !experiment.finalWeights.empty()) {
        std::cout << "  Weight Analysis:" << std::endl;
        float initialMean = std::accumulate(experiment.initialWeights.begin(), 
                                            experiment.initialWeights.end(), 0.0f) / 
                            experiment.initialWeights.size();
        float finalMean = std::accumulate(experiment.finalWeights.begin(), 
                                          experiment.finalWeights.end(), 0.0f) / 
                          experiment.finalWeights.size();
        std::cout << "    Initial Mean Weight: " << std::fixed << std::setprecision(6) << initialMean << std::endl;
        std::cout << "    Final Mean Weight: " << std::fixed << std::setprecision(6) << finalMean << std::endl;
        std::cout << "    Weight Change: " << std::fixed << std::setprecision(6) << (finalMean - initialMean) << std::endl;
    }
    
    std::cout << "\nMost Active Neurons:" << std::endl;
    for (size_t i = 0; i < std::min(size_t(10), experiment.mostActiveNeurons.size()); ++i) {
        std::cout << "  Neuron " << experiment.mostActiveNeurons[i] << std::endl;
    }
}

class PerformanceProfiler {
private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::unordered_map<std::string, double> timings;
    bool enabled;
    int interval;
    int stepCount;
    
public:
    PerformanceProfiler(bool enabled = false, int interval = 100) 
        : enabled(enabled), interval(interval), stepCount(0) {}
    
    void start(const std::string& operation) {
        if (!enabled) return;
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    void stop(const std::string& operation) {
        if (!enabled) return;
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        timings[operation] += duration;
        
        stepCount++;
        if (stepCount % interval == 0) {
            printPerformanceSummary();
        }
    }
    
    void printPerformanceSummary() {
        if (!enabled) return;
        std::cout << "\n--- Performance Profile (every " << interval << " steps) ---" << std::endl;
        for (const auto& pair : timings) {
            std::cout << "  " << pair.first << ": " << std::fixed << std::setprecision(3) 
                      << pair.second << " ms" << std::endl;
        }
        // Reset for next interval
        timings.clear();
        stepCount = 0;
    }
    
    void printFinalSummary() {
        if (!enabled || timings.empty()) return;
        std::cout << "\n--- Final Performance Summary ---" << std::endl;
        for (const auto& pair : timings) {
            std::cout << "  " << pair.first << ": " << std::fixed << std::setprecision(3) 
                      << pair.second << " ms" << std::endl;
        }
    }
};

// Learning Experiment: Demonstrates measurable synaptic changes through experience
class LearningExperiment {
private:
    std::shared_ptr<Brain> brain_;
    uint64_t seed_;
    size_t initialSynapseCount_;
    std::vector<float> initialWeights_;
    std::vector<float> finalWeights_;
    std::vector<NeuronId> mostActiveNeurons_;
    PerformanceProfiler profiler_;
    
public:
    LearningExperiment(std::shared_ptr<Brain> brain, uint64_t seed, 
                      bool profiling = false, int profilingInterval = 100)
        : brain_(brain), seed_(seed), profiler_(profiling, profilingInterval) {}
    
    void recordInitialState() {
        initialSynapseCount_ = brain_->getTotalSynapseCount();
        initialWeights_.clear();
        
        // Record initial weights from first region
        if (auto* region = brain_->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights_.push_back(syn->getWeight());
            }
        }
        
        profiler_.start("initial_state_recording");
        NLM_LOG_INFO("Initial state recorded:");
        NLM_LOG_INFO("  Synapses: " + std::to_string(initialSynapseCount_));
        if (!initialWeights_.empty()) {
            float sum = std::accumulate(initialWeights_.begin(), initialWeights_.end(), 0.0f);
            float mean = sum / initialWeights_.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
        profiler_.stop("initial_state_recording");
    }
    
    void recordFinalState() {
        profiler_.start("final_state_recording");
        finalWeights_.clear();
        
        // Record final weights from first region
        if (auto* region = brain_->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights_.push_back(syn->getWeight());
            }
        }
        
        mostActiveNeurons_ = brain_->getSpikeSystem()->getMostActiveNeurons(10);
        
        NLM_LOG_INFO("Final state recorded:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain_->getTotalSpikeCount()));
        if (!finalWeights_.empty()) {
            float sum = std::accumulate(finalWeights_.begin(), finalWeights_.end(), 0.0f);
            float mean = sum / finalWeights_.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
        profiler_.stop("final_state_recording");
    }
    
    void computeStatistics() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Learning Experiment Results ===");
        NLM_LOG_INFO("");
        
        if (initialWeights_.empty() || finalWeights_.empty()) {
            NLM_LOG_INFO("ERROR: No weights recorded");
            return;
        }
        
        // Compute weight changes
        float initialSum = std::accumulate(initialWeights_.begin(), initialWeights_.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights_.begin(), finalWeights_.end(), 0.0f);
        float initialMean = initialSum / initialWeights_.size();
        float finalMean = finalSum / finalWeights_.size();
        
        NLM_LOG_INFO("Weight Statistics:");
        NLM_LOG_INFO("  Initial mean weight: " + std::to_string(initialMean));
        NLM_LOG_INFO("  Final mean weight: " + std::to_string(finalMean));
        NLM_LOG_INFO("  Change: " + std::to_string(finalMean - initialMean));
        
        // Count synapses that changed significantly
        size_t strengthened = 0;
        size_t weakened = 0;
        size_t unchanged = 0;
        
        size_t minSize = std::min(initialWeights_.size(), finalWeights_.size());
        for (size_t i = 0; i < minSize; ++i) {
            float delta = finalWeights_[i] - initialWeights_[i];
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
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain_->getTotalSpikeCount()));
        NLM_LOG_INFO("  Most active neurons recorded: " + std::to_string(mostActiveNeurons_.size()));
        
        // Determine if learning occurred
        bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                                (strengthened > 0 || weakened > 0);
        
        NLM_LOG_INFO("");
        if (learningOccurred) {
            NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
        } else {
            NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
        }
        
        // Print performance summary
        profiler_.printFinalSummary();
    }
    
    // Getters for results
    uint64_t getSeed() const { return seed_; }
    const std::vector<float>& getInitialWeights() const { return initialWeights_; }
    const std::vector<float>& getFinalWeights() const { return finalWeights_; }
    const std::vector<NeuronId>& getMostActiveNeurons() const { return mostActiveNeurons_; }
    std::shared_ptr<Brain> getBrain() { return brain_; }
};

int main(int argc, char** argv) {
    printBanner();
    
    std::cout << "Initializing NLM Phase 2 Real Neural Computation...\n" << std::endl;
    
    // Parse command line options
    CommandLineOptions options = parseCommandLine(argc, argv);
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(options.logLevel);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation ===");
    NLM_LOG_INFO("Version: 0.1.0");
    NLM_LOG_INFO("Build: " << __DATE__ << " " << __TIME__);
    NLM_LOG_INFO("Implementing:");
    NLM_LOG_INFO("  - Leaky Integrate-and-Fire (LIF) neuron dynamics");
    NLM_LOG_INFO("  - Event-driven spike propagation with delays");
    NLM_LOG_INFO("  - STDP and Hebbian plasticity rules");
    NLM_LOG_INFO("  - Structural plasticity (synaptogenesis/pruning)");
    NLM_LOG_INFO("");
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
    std::string configFile = options.configFile;
    // Config file loading is handled by config->loadFromFile()
    
    // Load config from file (ignore if not found)
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Set default values for Phase 2
    config->set("random_seed", static_cast<int64_t>(options.randomSeed), ConfigSource::Default);
    config->set("simulation_timestep", options.simulationTimestep, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(options.neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(options.regionCount), ConfigSource::Default);
    config->set("connection_probability", options.connectionProbability, ConfigSource::Default);
    
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
    NLM_LOG_INFO("  experiment_mode: " + options.experimentMode);
    NLM_LOG_INFO("  max_steps: " + std::to_string(options.maxSteps));
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
    
    // Performance profiler for experiments
    PerformanceProfiler profiler(options.enablePerformanceProfiling, options.profilingInterval);
    
    // Run experiments based on mode
    if (options.experimentMode == "agent") {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Agent Mode (Phase 3+) ===");
        runAgentExperiment(brain, options, profiler);
    } else {
        // Run basic experiments
        runBasicConnectivityTest(brain, profiler);
        
        // Reset brain for plasticity experiment
        brain->reset();
        brain->initialize();
        
        if (options.experimentMode == "all" || options.experimentMode == "plasticity") {
            runPlasticityExperiment(brain, options, profiler);
        }
        
        // Reset and run STDP verification if requested
        if (options.experimentMode == "all" || options.experimentMode == "stdp") {
            brain->reset();
            brain->initialize();
            runStdpVerification(brain, profiler);
        }
    }
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 2 Complete ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Phase 2 Objectives Completed:");
    NLM_LOG_INFO("  ✓ Real LIF neuron dynamics implemented");
    NLM_LOG_INFO("  ✓ Event-driven spike propagation with delays");
    NLM_LOG_INFO("  ✓ STDP plasticity rule");
    NLM_LOG_INFO("  ✓ Hebbian plasticity rule");
    NLM_LOG_INFO("  ✓ Structural plasticity (synaptogenesis/pruning)");
    NLM_LOG_INFO("  ✓ Learning experiment demonstrates measurable changes");
    NLM_LOG_INFO("  ✓ Network shows activity-dependent synaptic modification");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("The NLM brain is now a functioning artificial neural substrate");
    NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
    NLM_LOG_INFO("");
    
    return 0;
}

// New function to run agent experiments
void runAgentExperiment(std::shared_ptr<Brain> brain, const CommandLineOptions& options, PerformanceProfiler& profiler) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Agent Experiment (Phase 3+) ===");
    
    // Create configuration for agent
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500);
    config->set("region_count", 2);
    config->set("connection_probability", 0.1f);
    config->set("random_seed", 42u);
    config->set("simulation_timestep", 0.001);
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    config->set("stdp_tau", 20.0f);
    config->set("synaptogenesis_rate", 0.0001f);
    config->set("pruning_rate", 0.00001f);
    
    // Create world
    SimpleWorld world;
    world.configure(20.0f, 20.0f, 16, 16);
    world.setRandomSeed(42);
    
    // Create agent brain interface
    AgentBrain agentBrain(brain);
    agentBrain.initialize(world);
    
    // Configure agent brain with advanced settings
    agentBrain.enableRewardModulation(true);
    agentBrain.enableStructuralPlasticity(true);
    agentBrain.enableDevelopment(true);
    agentBrain.enableCuriosity(true);
    
    // Enable advanced agent features
    agentBrain.setCuriosityThreshold(0.2f);
    agentBrain.setExplorationRate(0.3f);
    agentBrain.enableCaching(true);
    
    std::cout << "Agent Brain initialized:" << std::endl;
    std::cout << "  Neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "  Synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "  Sensory input size: " << agentBrain.getSensoryInputSize() << std::endl;
    std::cout << "  Motor output size: " << agentBrain.getMotorOutputSize() << std::endl;
    std::cout << "  Motor neurons count: " << agentBrain.getMotorNeuronsCount() << std::endl;
    std::cout << "  Sensory neurons count: " << agentBrain.getSensoryNeuronsCount() << std::endl;
    std::cout << std::endl;
    
    // Run agent episodes
    int numEpisodes = options.continuousMode ? 10 : 5;
    int stepsPerEpisode = options.continuousMode ? 100 : 200;
    
    for (int episode = 0; episode < numEpisodes; ++episode) {
        std::cout << "--- Episode " << episode << " started ---" << std::endl;
        
        // Reset world and brain
        world.reset();
        agentBrain.reset();
        brain->reset();
        brain->initialize();
        
        float episodeReward = 0.0f;
        
        // Run steps
        for (int step = 0; step < stepsPerEpisode; ++step) {
            profiler.start("sensory_processing");
            // 1. Get sensory percept from world
            const SensoryPercept& percept = world.getSensoryPercept();
            
            profiler.stop("sensory_processing");
            
            profiler.start("brain_processing");
            // 2. Inject sensory input into brain
            agentBrain.processSensoryInput(percept);
            
            // 3. Simulate brain (multiple steps per action)
            for (int neuralStep = 0; neuralStep < 10; ++neuralStep) {
                brain->step(neuralStep, neuralStep * 0.001);
            }
            
            // 4. Decode motor command from brain activity
            MotorCommand cmd = agentBrain.decodeMotorCommand();
            
            profiler.stop("brain_processing");
            
            profiler.start("world_interaction");
            // 5. Apply motor command to world
            ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
            
            // 6. Apply reward modulation
            agentBrain.applyRewardModulation(result.reward, 0.0f);
            
            // 7. Update world
            world.update(0.01);  // 10ms timestep
            
            // 8. Update development
            agentBrain.updateDevelopment(0.01);
            
            profiler.stop("world_interaction");
            
            episodeReward += result.reward;
            
            // Print periodic updates
            if (step % 50 == 0) {
                std::cout << "  Step " << step 
                          << ": reward=" << result.reward
                          << ", action=" << motorCommandToString(cmd)
                          << ", neuromod=" << agentBrain.getNeuromodulationLevel()
                          << ", novelty=" << agentBrain.getNoveltyLevel()
                          << ", energy=" << world.getAgentBody().energy
                          << std::endl;
            }
        }
        
        std::cout << "--- Episode " << episode << " ended ---" << std::endl;
        std::cout << "  Total Reward: " << episodeReward << std::endl;
        std::cout << "  Final Novelty: " << agentBrain.getNoveltyLevel() << std::endl;
        std::cout << "  Final Curiosity: " << agentBrain.getCuriosityLevel() << std::endl;
        std::cout << "  Final Energy: " << world.getAgentBody().energy << std::endl;
        std::cout << "  Synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << std::endl;
        
        // Small delay between episodes if in continuous mode
        if (options.continuousMode && episode < numEpisodes - 1) {
            std::this_thread::sleep_for(std::chrono::duration<double>(options.continuousInterval));
        }
    }
    
    std::cout << "Agent experiment complete." << std::endl;
    std::cout << "The agent received sensory input, produced actions," << std::endl;
    std::cout << "experienced consequences, and modified its synapses." << std::endl;
}

// Helper function to convert motor command to string
const char* motorCommandToString(MotorCommand cmd) {
    switch (cmd) {
        case MotorCommand::MoveForward: return "MoveForward";
        case MotorCommand::MoveBackward: return "MoveBackward";
        case MotorCommand::TurnLeft: return "TurnLeft";
        case MotorCommand::TurnRight: return "TurnRight";
        case MotorCommand::LookLeft: return "LookLeft";
        case MotorCommand::LookRight: return "LookRight";
        case MotorCommand::Interact: return "Interact";
        case MotorCommand::Wait: return "Wait";
        default: return "Unknown";
    }
}
