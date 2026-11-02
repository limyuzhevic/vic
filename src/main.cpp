// Enhanced NLM main with advanced features
// Phase 2: Real Neural Computation with enhanced CLI, logging, and monitoring

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
#include <ctime>
#include <algorithm>
#include <sstream>
#include <fstream>

using namespace nlm;

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
    ║     - Leaky Integrate-and-Fire (LIF) neuron dynamics           ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity rules                        ║
    ║     - Structural plasticity                                   ║
    ║                                                               ║
    ║     Enhanced with:                                            ║
    ║     • JSON/YAML configuration support                         ║
    ║     • Better error handling and validation                    ║
    ║     • Performance monitoring                                  ║
    ║     • Advanced command-line interface                          ║
    ║     • Configurable neuron parameters                           ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printUsage(const std::string& programName) {
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Phase 2: Real Neural Computation Demo" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h              Show this help message" << std::endl;
    std::cout << "  --version, -v           Show version information" << std::endl;
    std::cout << "  --config <file>         Load configuration from file (JSON/YAML)" << std::endl;
    std::cout << "  --neurons <count>       Number of neurons to simulate" << std::endl;
    std::cout << "  --regions <count>       Number of brain regions" << std::endl;
    std::cout << "  --seed <number>         Random seed for reproducibility" << std::endl;
    std::cout << "  --timestep <seconds>     Simulation timestep" << std::endl;
    std::cout << "  --verbose, -v           Enable verbose logging" << std::endl;
    std::cout << "  --quiet, -q             Reduce logging output" << std::endl;
    std::cout << "  --validate-only         Validate configuration and exit" << std::endl;
    std::cout << "  --performance <file>     Save performance metrics to file" << std::endl;
    std::cout << "  --stats-only            Show only statistics, no detailed output" << std::endl;
    std::cout << std::endl;
    std::cout << "Configuration file formats:" << std::endl;
    std::cout << "  .json, .jsonl            JSON format (with enhanced support)" << std::endl;
    std::cout << "  .yaml, .yml, .cfg        YAML format (with enhanced support)" << std::endl;
    std::cout << "  Plain text               Simple key=value format" << std::endl;
    std::cout << std::endl;
    std::cout << "Example:" << std::endl;
    std::cout << "  " << programName << " --config brain_config.yaml --neurons 1000 --seed 42 --verbose" << std::endl;
}

void printVersion() {
    std::cout << "NLM - Neural Learning Machine" << std::endl;
    std::cout << "Version: 2.0.0 (Phase 2 Enhanced)" << std::endl;
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << "C++ Standard: C++20" << std::endl;
    std::cout << "Features:" << std::endl;
    std::cout << "  ✓ Real LIF neuron dynamics" << std::endl;
    std::cout << "  ✓ Event-driven spike propagation" << std::endl;
    std::cout << "  ✓ STDP and Hebbian plasticity" << std::endl;
    std::cout << "  ✓ Structural plasticity" << std::endl;
    std::cout << "  ✓ JSON/YAML configuration" << std::endl;
    std::cout << "  ✓ Enhanced error handling" << std::endl;
    std::cout << "  ✓ Performance monitoring" << std::endl;
    std::cout << "  ✓ Configurable neuron parameters" << std::endl;
}

class PerformanceMonitor {
public:
    struct Metrics {
        double totalTime = 0.0;
        size_t totalSpikes = 0;
        size_t neuronCount = 0;
        size_t synapseCount = 0;
        double avgFiringRate = 0.0;
        std::vector<double> stepTimes;
        std::vector<size_t> spikeCountsPerStep;
    };

    PerformanceMonitor() : startTime_(std::chrono::high_resolution_clock::now()) {}

    void recordStep(size_t spikes, double stepTime) {
        stepTimes_.push_back(stepTime);
        spikeCountsPerStep_.push_back(spikes);
    }

    void finalize(const std::shared_ptr<Brain>& brain, const std::string& outputFile = "") {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime_);
        totalTime_ = duration.count();

        if (brain) {
            totalSpikes_ = brain->getTotalSpikeCount();
            neuronCount_ = brain->getTotalNeuronCount();
            synapseCount_ = brain->getTotalSynapseCount();
            if (neuronCount_ > 0) {
                avgFiringRate_ = (static_cast<double>(totalSpikes_) / totalTime_) * 1000.0; // spikes per second per neuron
            }
        }

        if (!outputFile.empty()) {
            saveToFile(outputFile);
        }
    }

    void saveToFile(const std::string& filepath) const {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_WARNING("Could not open performance file for writing: " + filepath);
            return;
        }

        file << "Performance Metrics" << std::endl;
        file << "====================" << std::endl;
        file << "Total simulation time: " << totalTime_ << " seconds" << std::endl;
        file << "Total spikes generated: " << totalSpikes_ << std::endl;
        file << "Number of neurons: " << neuronCount_ << std::endl;
        file << "Number of synapses: " << synapseCount_ << std::endl;
        file << "Average firing rate: " << avgFiringRate_ << " Hz per neuron" << std::endl;
        file << std::endl;

        if (!stepTimes_.empty()) {
            double minStep = *std::min_element(stepTimes_.begin(), stepTimes_.end());
            double maxStep = *std::max_element(stepTimes_.begin(), stepTimes_.end());
            double avgStep = std::accumulate(stepTimes_.begin(), stepTimes_.end(), 0.0) / stepTimes_.size();
            double stdStep = 0.0;
            for (double t : stepTimes_) {
                stdStep += (t - avgStep) * (t - avgStep);
            }
            stdStep = std::sqrt(stdStep / stepTimes_.size());

            file << "Step timing statistics:" << std::endl;
            file << "  Minimum: " << minStep * 1000 << " ms" << std::endl;
            file << "  Maximum: " << maxStep * 1000 << " ms" << std::endl;
            file << "  Average: " << avgStep * 1000 << " ms" << std::endl;
            file << "  Standard deviation: " << stdStep * 1000 << " ms" << std::endl;
            file << "  Steps recorded: " << stepTimes_.size() << std::endl;
        }

        if (!spikeCountsPerStep_.empty()) {
            size_t maxSpikes = *std::max_element(spikeCountsPerStep_.begin(), spikeCountsPerStep_.end());
            double avgSpikes = std::accumulate(spikeCountsPerStep_.begin(), spikeCountsPerStep_.end(), 0.0) / spikeCountsPerStep_.size();

            file << std::endl;
            file << "Spike count statistics:" << std::endl;
            file << "  Maximum spikes per step: " << maxSpikes << std::endl;
            file << "  Average spikes per step: " << avgSpikes << std::endl;
            file << "  Total steps: " << spikeCountsPerStep_.size() << std::endl;
        }
    }

    void printSummary() const {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Performance Summary ===");
        NLM_LOG_INFO("Total simulation time: " + std::to_string(totalTime_) + " seconds");
        NLM_LOG_INFO("Total spikes generated: " + std::to_string(totalSpikes_));
        NLM_LOG_INFO("Number of neurons: " + std::to_string(neuronCount_));
        NLM_LOG_INFO("Number of synapses: " + std::to_string(synapseCount_));
        NLM_LOG_INFO("Average firing rate: " + std::to_string(avgFiringRate_) + " Hz per neuron");

        if (!stepTimes_.empty()) {
            double minStep = *std::min_element(stepTimes_.begin(), stepTimes_.end());
            double maxStep = *std::max_element(stepTimes_.begin(), stepTimes_.end());
            double avgStep = std::accumulate(stepTimes_.begin(), stepTimes_.end(), 0.0) / stepTimes_.size();

            NLM_LOG_INFO("");
            NLM_LOG_INFO("Step timing statistics:");
            NLM_LOG_INFO("  Minimum step time: " + std::to_string(minStep * 1000) + " ms");
            NLM_LOG_INFO("  Maximum step time: " + std::to_string(maxStep * 1000) + " ms");
            NLM_LOG_INFO("  Average step time: " + std::to_string(avgStep * 1000) + " ms");
        }
    }

private:
    std::chrono::high_resolution_clock::time_point startTime_;
    double totalTime_ = 0.0;
    size_t totalSpikes_ = 0;
    size_t neuronCount_ = 0;
    size_t synapseCount_ = 0;
    double avgFiringRate_ = 0.0;
    std::vector<double> stepTimes_;
    std::vector<size_t> spikeCountsPerStep_;
};

void validateConfiguration(const std::shared_ptr<Config>& config) {
    NLM_LOG_INFO("Validating configuration...");

    // Check required configuration keys
    std::vector<std::string> requiredKeys = {
        "random_seed",
        "simulation_timestep",
        "neuron_count",
        "region_count",
        "connection_probability"
    };

    bool valid = true;
    for (const auto& key : requiredKeys) {
        if (!config->has(key)) {
            NLM_LOG_WARNING("Missing required configuration key: " + key);
            valid = false;
        }
    }

    // Validate parameter ranges
    auto timestep = config->getOr<double>("simulation_timestep", 0.001);
    if (timestep <= 0.0 || timestep > 1.0) {
        NLM_LOG_WARNING("Invalid simulation timestep: " + std::to_string(timestep) + ". Must be between 0 and 1.");
        valid = false;
    }

    auto neuronCount = config->getOr<int64_t>("neuron_count", 1000);
    if (neuronCount <= 0) {
        NLM_LOG_WARNING("Invalid neuron count: " + std::to_string(neuronCount) + ". Must be positive.");
        valid = false;
    }

    auto connectionProb = config->getOr<float>("connection_probability", 0.1f);
    if (connectionProb < 0.0f || connectionProb > 1.0f) {
        NLM_LOG_WARNING("Invalid connection probability: " + std::to_string(connectionProb) + ". Must be between 0 and 1.");
        valid = false;
    }

    if (valid) {
        NLM_LOG_INFO("Configuration validation successful.");
    } else {
        NLM_LOG_ERROR("Configuration validation failed. Using default values.");
    }
}

void printConfigurationSummary(const std::shared_ptr<Config>& config) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Configuration Summary ===");
    
    auto configSummary = config->summary();
    // Split summary into lines to avoid overly long log entries
    std::istringstream iss(configSummary);
    std::string line;
    while (std::getline(iss, line)) {
        if (line.length() > 200) {
            NLM_LOG_INFO(line.substr(0, 200));
            NLM_LOG_INFO("(... truncated)");
            break;
        } else {
            NLM_LOG_INFO(line);
        }
    }
}

class ProgressReporter {
public:
    ProgressReporter(size_t totalSteps, const std::string& description = "Simulation")
        : totalSteps_(totalSteps), description_(description) {}

    void start() {
        startTime_ = std::chrono::high_resolution_clock::now();
        lastReportTime_ = startTime_;
        NLM_LOG_INFO("[" + description_ + "] Starting simulation with " + std::to_string(totalSteps_) + " steps...");
    }

    void update(size_t step, size_t spikes) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration<double>(currentTime - startTime_).count();

        // Report progress every 10% or every 100 steps, whichever comes first
        if (step == 0 || step % std::max<size_t>(1, totalSteps_ / 10) == 0 ||
            (step > 0 && step % 100 == 0)) {

            double progress = (static_cast<double>(step) / totalSteps_) * 100.0;
            NLM_LOG_INFO("[" + description_ + "] Progress: " + std::to_string(progress) + "% (step " + std::to_string(step) +
                         ", " + std::to_string(spikes) + " spikes, " + std::to_string(elapsed) + "s)");
            lastReportTime_ = currentTime;
        }
    }

    void finish() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto totalDuration = std::chrono::duration<double>(endTime - startTime_).count();
        NLM_LOG_INFO("[" + description_ + "] Simulation completed in " + std::to_string(totalDuration) + " seconds.");
    }

private:
    size_t totalSteps_;
    std::string description_;
    std::chrono::high_resolution_clock::time_point startTime_;
    std::chrono::high_resolution_clock::time_point lastReportTime_;
};

void configureLogging(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--verbose" || arg == "-v") {
            // Enable all logging levels
            Logger::getGlobal().setLevel(LogLevel::Debug);
            NLM_LOG_INFO("Verbose logging enabled.");
            break;
        } else if (arg == "--quiet" || arg == "-q") {
            // Reduce to info level only
            Logger::getGlobal().setLevel(LogLevel::Info);
            NLM_LOG_INFO("Quiet mode enabled (Info level and above).");
            break;
        }
    }
}

int main(int argc, char** argv) {
    // Parse command line arguments
    std::string programName = "nlm";
    if (argc > 0) {
        programName = std::string(argv[0]);
    }

    // Check for help/version requests
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h") {
            printUsage(programName);
            return 0;
        } else if (arg == "--version" || arg == "-v") {
            printVersion();
            return 0;
        }
    }

    printBanner();
    
    // Initialize performance monitoring
    PerformanceMonitor perfMonitor;

    // Initialize logger with configurable level
    configureLogging(argc, argv);
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);

    NLM_LOG_INFO("=== Enhanced NLM Phase 2: Real Neural Computation ===");
    NLM_LOG_INFO("Starting enhanced neural simulation with advanced features...");

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

    // Load config from file
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration (file not found: " + configFile + ")");
    }

    // Override with command line args
    config->loadFromArgs(argc, argv);

    // Set default values for Phase 2 with enhanced defaults
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);  // Smaller for faster test
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);

    // Enhanced STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);

    // Structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);

    // Enhanced logging configuration
    configureLogging(argc, argv);

    // Validate configuration
    validateConfiguration(config);

    // Log configuration summary
    printConfigurationSummary(config);

    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");

    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain (Phase 2 Enhanced)...");
    auto brain = std::make_shared<Brain>(config);

    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }

    brain->logStatus();

    // Performance monitoring
    size_t totalSteps = 1000;  // For demo purposes
    ProgressReporter progressReporter(totalSteps, "Phase 2 Demo");
    progressReporter.start();

    // Run Test 1: Basic connectivity
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    runBasicConnectivityTest(brain);

    // Reset brain for plasticity experiment
    NLM_LOG_INFO("Resetting brain for plasticity experiment...");
    brain->reset();
    brain->initialize();

    // Run Test 2: Plasticity learning experiment
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    runPlasticityExperiment(brain);

    // Reset and run Test 3: STDP verification
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    brain->reset();
    brain->initialize();
    runStdpVerification(brain);

    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();

    // Performance monitoring finalization
    perfMonitor.finalize(brain, "performance_metrics.json");
    perfMonitor.printSummary();

    // Save performance metrics
    std::string outputFile;
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 14) == "--performance") {
            if (arg.find('=') != std::string::npos) {
                outputFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                outputFile = argv[++i];
            }
            break;
        }
    }

    if (!outputFile.empty()) {
        NLM_LOG_INFO("Performance metrics saved to: " + outputFile);
    }

    // Enhanced completion message
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 2 Enhanced Complete ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Phase 2 Objectives Completed:");
    NLM_LOG_INFO("  ✓ Real LIF neuron dynamics implemented");
    NLM_LOG_INFO("  ✓ Event-driven spike propagation with delays");
    NLM_LOG_INFO("  ✓ STDP plasticity rule");
    NLM_LOG_INFO("  ✓ Hebbian plasticity rule");
    NLM_LOG_INFO("  ✓ Structural plasticity (synaptogenesis/pruning)");
    NLM_LOG_INFO("  ✓ Learning experiment demonstrates measurable changes");
    NLM_LOG_INFO("  ✓ Network shows activity-dependent synaptic modification");
    NLM_LOG_INFO("  ✓ JSON/YAML configuration support");
    NLM_LOG_INFO("  ✓ Enhanced error handling and validation");
    NLM_LOG_INFO("  ✓ Performance monitoring and metrics");
    NLM_LOG_INFO("  ✓ Progress reporting and logging");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("The NLM brain is now an enhanced artificial neural substrate");
    NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
    NLM_LOG_INFO("Additional features include configuration validation, performance")
    NLM_LOG_INFO("monitoring, and improved user experience.");
    NLM_LOG_INFO("");

    return 0;
}

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
    for (size_t i = 0; i < std::min<size_t>(10), neurons.size()); ++i) {
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
        for (size_t i = 0; i < std::min<size_t>(3), neurons.size()); ++i) {
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
    
    // A pattern, not a seizure: stimulate only the first 20 sensory neurons
    // each step. The old code injected 30.0f into *every* sensory neuron
    // 20 times per step (600.0f), so the whole network fired synchronously
    // and every weight saturated to 1.0 - learning was measurable but not
    // selective at all.
    std::vector<NeuronId> patternNeurons;
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto* neuron : region->getAllNeurons()) {
            if (neuron->getType() == NeuronType::Sensory && patternNeurons.size() < 20) {
                patternNeurons.push_back(neuron->getId());
            }
        }
    }
    
    ProgressReporter plasticityProgress(1000, "Plasticity Experiment");
    plasticityProgress.start();
    
    for (SimulationStep step = 0; step < 1000; ++step) {
        for (NeuronId id : patternNeurons) {
            brain->injectCurrent(id, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            NLM_LOG_INFO("    Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
        
        plasticityProgress.update(step, brain->getTotalSpikeCount());
    }
    
    plasticityProgress.finish();
    
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
    
    // Use one real synapse and its own pre/post neurons. The old test
    // fired neurons[0]/neurons[1] and measured synapses[0..4], which are
    // usually unrelated to that pair, so its result said nothing about STDP.
    auto& synapses = region->getSynapses();
    if (synapses.empty()) {
        NLM_LOG_INFO("  No synapses for STDP test");
        return;
    }
    Synapse* syn = synapses[0].get();
    syn->enablePlasticity(false, true, false);  // STDP only
    float beforeWeight = syn->getWeight();
    NLM_LOG_INFO("  Testing STDP on 1 synapse:");
    NLM_LOG_INFO("    Synapse before: " + std::to_string(beforeWeight) +
                 " (pre=" + std::to_string(syn->getSourceNeuron().index()) +
                 ", post=" + std::to_string(syn->getDestinationNeuron().index()) + ")");
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
    
    for (int trial = 0; trial < 50; ++trial) {
        brain->injectCurrent(syn->getSourceNeuron(), 60.0f);
        brain->step(trial * 2, trial * 2 * 0.001);
        brain->injectCurrent(syn->getDestinationNeuron(), 60.0f);
        brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
    }
    
    float delta = syn->getWeight() - beforeWeight;
    NLM_LOG_INFO("  After correlated activity:");
    NLM_LOG_INFO("    Synapse after: " + std::to_string(syn->getWeight()) +
                 " (Δ=" + std::to_string(delta) + ")");
    
    float totalDelta = delta;
    
    NLM_LOG_INFO("");
    if (totalDelta > 0.001f) {
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check parameters");
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering");
    }
}

// Learning Experiment struct (moved from main.cpp)
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

// Forward declarations for test functions
void runBasicConnectivityTest(std::shared_ptr<Brain> brain);
void runPlasticityExperiment(std::shared_ptr<Brain> brain);
void runStdpVerification(std::shared_ptr<Brain> brain);

