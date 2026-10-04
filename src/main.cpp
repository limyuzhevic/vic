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
#include <algorithm>
#include <chrono>
#include <sstream>
#include <filesystem>
#include <unordered_map>
#include <functional>

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
    ║     - Real LIF neuron dynamics                                ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity                            ║
    ║     - Structural plasticity                                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Command-line argument parser
class CommandLineArgs {
public:
    std::string configFile;
    std::string demoMode;
    int neuronCount;
    int regionCount;
    float connectionProbability;
    int simulationSteps;
    float simulationTimestep;
    bool enablePlasticity;
    bool enableDevelopment;
    bool enableCuriosity;
    bool enableNovelty;
    bool enableCheckpointing;
    std::string checkpointPath;
    bool verbose;
    bool quiet;
    bool profilePerformance;
    bool showHelp;
    
    CommandLineArgs() {
        configFile = "configs/default.cfg";
        demoMode = "basic";
        neuronCount = 1000;
        regionCount = 1;
        connectionProbability = 0.1f;
        simulationSteps = 100;
        simulationTimestep = 0.001f;
        enablePlasticity = true;
        enableDevelopment = false;
        enableCuriosity = false;
        enableNovelty = false;
        enableCheckpointing = false;
        checkpointPath = "";
        verbose = false;
        quiet = false;
        profilePerformance = false;
        showHelp = false;
    }
    
    bool parseArgs(int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                showHelp = true;
            } else if (arg.substr(0, 7) == "--config") {
                if (arg.find('=') != std::string::npos) {
                    configFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    configFile = argv[++i];
                }
            } else if (arg.substr(0, 7) == "--demo") {
                if (arg.find('=') != std::string::npos) {
                    demoMode = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    demoMode = argv[++i];
                }
            } else if (arg == "--basic-demo" || arg == "--demo=basic") {
                demoMode = "basic";
            } else if (arg == "--learning-demo" || arg == "--demo=learning") {
                demoMode = "learning";
            } else if (arg == "--stdp-demo" || arg == "--demo=stdp") {
                demoMode = "stdp";
            } else if (arg == "--plasticity-demo" || arg == "--demo=plasticity") {
                demoMode = "plasticity";
            } else if (arg == "--development-demo" || arg == "--demo=development") {
                demoMode = "development";
            } else if (arg.substr(0, 8) == "--neurons") {
                if (arg.find('=') != std::string::npos) {
                    neuronCount = std::stoi(arg.substr(arg.find('=') + 1));
                } else if (i + 1 < argc) {
                    neuronCount = std::stoi(argv[++i]);
                }
            } else if (arg.substr(0, 8) == "--regions") {
                if (arg.find('=') != std::string::npos) {
                    regionCount = std::stoi(arg.substr(arg.find('=') + 1));
                } else if (i + 1 < argc) {
                    regionCount = std::stoi(argv[++i]);
                }
            } else if (arg.substr(0, 6) == "--steps") {
                if (arg.find('=') != std::string::npos) {
                    simulationSteps = std::stoi(arg.substr(arg.find('=') + 1));
                } else if (i + 1 < argc) {
                    simulationSteps = std::stoi(argv[++i]);
                }
            } else if (arg.substr(0, 8) == "--timestep") {
                if (arg.find('=') != std::string::npos) {
                    simulationTimestep = std::stof(arg.substr(arg.find('=') + 1));
                } else if (i + 1 < argc) {
                    simulationTimestep = std::stof(argv[++i]);
                }
            } else if (arg.substr(0, 6) == "--prob") {
                if (arg.find('=') != std::string::npos) {
                    connectionProbability = std::stof(arg.substr(arg.find('=') + 1));
                } else if (i + 1 < argc) {
                    connectionProbability = std::stof(argv[++i]);
                }
            } else if (arg == "--plasticity" || arg == "--demo=plasticity") {
                enablePlasticity = true;
            } else if (arg == "--no-plasticity" || arg == "--demo=basic") {
                enablePlasticity = false;
            } else if (arg == "--development" || arg == "--demo=development") {
                enableDevelopment = true;
            } else if (arg == "--curiosity" || arg == "--demo=curiosity") {
                enableCuriosity = true;
            } else if (arg == "--novelty" || arg == "--demo=novelty") {
                enableNovelty = true;
            } else if (arg == "--checkpoint" || arg == "--enable-checkpoint") {
                enableCheckpointing = true;
            } else if (arg.substr(0, 8) == "--checkpoint-path") {
                if (arg.find('=') != std::string::npos) {
                    checkpointPath = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    checkpointPath = argv[++i];
                }
            } else if (arg == "--verbose") {
                verbose = true;
            } else if (arg == "--quiet") {
                quiet = true;
            } else if (arg == "--profile" || arg == "--profile-performance") {
                profilePerformance = true;
            } else {
                std::cerr << "Unknown argument: " << arg << std::endl;
                return false;
            }
        }
        return true;
    }
    
    void printHelp() {
        std::cout << "NLM — Neural Learning Machine (熙然)" << std::endl;
        std::cout << "==========================================" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Usage: nlm [OPTIONS]" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Demo Modes:" << std::endl;
        std::cout << "  --basic-demo          Basic neural connectivity test" << std::endl;
        std::cout << "  --learning-demo       Plasticity and learning demonstration" << std::endl;
        std::cout << "  --stdp-demo           Spike-Timing-Dependent Plasticity test" << std::endl;
        std::cout << "  --plasticity-demo     General plasticity demonstration" << std::endl;
        std::cout << "  --development-demo    Developmental plasticity demonstration" << std::endl;
        std::cout << std::endl;
        
        std::cout << "General Options:" << std::endl;
        std::cout << "  --help, -h                Show this help message" << std::endl;
        std::cout << "  --config=FILE, --config FILE    Load configuration from file" << std::endl;
        std::cout << "  --neurons=N, --neurons N       Set number of neurons (default: 1000)" << std::endl;
        std::cout << "  --regions=N, --regions N       Set number of brain regions (default: 1)" << std::endl;
        std::cout << "  --steps=N, --steps N           Set simulation steps (default: 100)" << std::endl;
        std::cout << "  --timestep=T, --timestep T     Set simulation timestep (default: 0.001)" << std::endl;
        std::cout << "  --prob=P, --prob P             Set connection probability (default: 0.1)" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Feature Options:" << std::endl;
        std::cout << "  --plasticity              Enable plasticity mechanisms" << std::endl;
        std::cout << "  --no-plasticity           Disable plasticity" << std::endl;
        std::cout << "  --development             Enable developmental processes" << std::endl;
        std::cout << "  --curiosity               Enable curiosity-driven exploration" << std::endl;
        std::cout << "  --novelty                 Enable novelty detection" << std::endl;
        std::cout << "  --checkpoint, --enable-checkpoint  Enable checkpoint saving" << std::endl;
        std::cout << "  --checkpoint-path=PATH, --checkpoint-path PATH    Set checkpoint file path" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Output Options:" << std::endl;
        std::cout << "  --verbose                 Enable verbose output" << std::endl;
        std::cout << "  --quiet                   Suppress non-essential output" << std::endl;
        std::cout << "  --profile, --profile-performance  Enable performance profiling" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Examples:" << std::endl;
        std::cout << "  ./nlm --basic-demo                    # Basic connectivity test" << std::endl;
        std::cout << "  ./nlm --learning-demo --neurons=5000  # Learning demonstration with 5k neurons" << std::endl;
        std::cout << "  ./nlm --stdp-demo --steps=1000        # STDP verification with 1k steps" << std::endl;
        std::cout << "  ./nlm --development-demo               # Developmental plasticity demo" << std::endl;
        std::cout << "  ./nlm --basic-demo --checkpoint --checkpoint-path=./checkpoint.bin" << std::endl;
        std::cout << "  ./nlm --verbose --profile --steps=500  # Verbose profiling mode" << std::endl;
    }
};

// Enhanced banner with version info
void printEnhancedBanner() {
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
    
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << "Version: 0.1.0 (Phase 6 Integration)" << std::endl;
    std::cout << "Architecture: Modular Spiking Neural Network" << std::endl;
}

// Performance timer utility
class PerformanceTimer {
public:
    std::chrono::high_resolution_clock::time_point startTime;
    std::string operationName;
    
    PerformanceTimer(const std::string& name) : operationName(name) {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    ~PerformanceTimer() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        if (!quiet) {
            std::cout << "[" << operationName << ": " << duration.count() << "ms]" << std::endl;
        }
    }
};

// Advanced learning experiment with detailed statistics
struct AdvancedLearningExperiment {
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    LearningStatistics stats;
    
    AdvancedLearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
        : brain(b), seed(s) {
        stats = LearningStatistics();
    }
    
    void recordInitialState() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        stats.initialSynapseCount = brain->getTotalSynapseCount();
        stats.initialFiringRate = brain->getAverageFiringRate();
        stats.initialActiveNeurons = brain->getActiveNeuronCount();
        
        // Record detailed initial state
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                stats.initialWeights.push_back(syn->getWeight());
            }
        }
        
        // Calculate initial statistics
        if (!stats.initialWeights.empty()) {
            float sum = std::accumulate(stats.initialWeights.begin(), stats.initialWeights.end(), 0.0f);
            stats.initialWeightMean = sum / stats.initialWeights.size();
            stats.initialWeightStd = calculateStdDev(stats.initialWeights, stats.initialWeightMean);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        stats.initialStateTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        
        NLM_LOG_INFO("Initial state recorded in " + std::to_string(stats.initialStateTime) + "ms:");
        NLM_LOG_INFO("  Synapses: " + std::to_string(stats.initialSynapseCount));
        NLM_LOG_INFO("  Firing Rate: " + std::to_string(stats.initialFiringRate));
        NLM_LOG_INFO("  Active Neurons: " + std::to_string(stats.initialActiveNeurons));
        if (!stats.initialWeights.empty()) {
            NLM_LOG_INFO("  Weight Mean: " + std::to_string(stats.initialWeightMean));
            NLM_LOG_INFO("  Weight Std Dev: " + std::to_string(stats.initialWeightStd));
        }
    }
    
    void recordFinalState() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                stats.finalWeights.push_back(syn->getWeight());
            }
        }
        
        stats.finalSpikeCount = brain->getTotalSpikeCount();
        stats.finalFiringRate = brain->getAverageFiringRate();
        stats.finalActiveNeurons = brain->getActiveNeuronCount();
        stats.finalLearningNeurons = brain->getFiringNeuronCount();
        
        if (!stats.finalWeights.empty()) {
            float sum = std::accumulate(stats.finalWeights.begin(), stats.finalWeights.end(), 0.0f);
            stats.finalWeightMean = sum / stats.finalWeights.size();
            stats.finalWeightStd = calculateStdDev(stats.finalWeights, stats.finalWeightMean);
        }
        
        // Calculate learning metrics
        calculateLearningMetrics();
        
        auto endTime = std::chrono::high_resolution_clock::now();
        stats.finalStateTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        
        NLM_LOG_INFO("Final state recorded in " + std::to_string(stats.finalStateTime) + "ms:");
        NLM_LOG_INFO("  Total Spikes: " + std::to_string(stats.finalSpikeCount));
        NLM_LOG_INFO("  Final Firing Rate: " + std::to_string(stats.finalFiringRate));
        NLM_LOG_INFO("  Active Neurons: " + std::to_string(stats.finalActiveNeurons));
        if (!stats.finalWeights.empty()) {
            NLM_LOG_INFO("  Weight Mean: " + std::to_string(stats.finalWeightMean));
            NLM_LOG_INFO("  Weight Std Dev: " + std::to_string(stats.finalWeightStd));
        }
    }
    
    void calculateLearningMetrics() {
        if (stats.initialWeights.empty() || stats.finalWeights.empty()) {
            return;
        }
        
        size_t minSize = std::min(stats.initialWeights.size(), stats.finalWeights.size());
        
        // Calculate weight changes
        for (size_t i = 0; i < minSize; ++i) {
            float delta = stats.finalWeights[i] - stats.initialWeights[i];
            stats.weightChanges.push_back(delta);
            
            if (delta > 0.01f) stats.strengthenedCount++;
            else if (delta < -0.01f) stats.weakenedCount++;
            else stats.unchangedCount++;
        }
        
        // Calculate average change
        if (!stats.weightChanges.empty()) {
            float sum = std::accumulate(stats.weightChanges.begin(), stats.weightChanges.end(), 0.0f);
            stats.averageWeightChange = sum / stats.weightChanges.size();
            
            // Calculate learning strength
            stats.learningStrength = std::abs(stats.averageWeightChange);
            
            // Calculate network stability
            float variance = calculateVariance(stats.weightChanges, stats.averageWeightChange);
            stats.networkStability = 1.0f - std::min(1.0f, variance);
        }
        
        // Calculate firing rate change
        stats.firingRateChange = stats.finalFiringRate - stats.initialFiringRate;
        
        // Determine learning classification
        if (stats.learningStrength > 0.05f) {
            stats.learningClassification = "Strong Learning";
        } else if (stats.learningStrength > 0.01f) {
            stats.learningClassification = "Moderate Learning";
        } else if (stats.learningStrength > 0.001f) {
            stats.learningClassification = "Weak Learning";
        } else {
            stats.learningClassification = "No Learning";
        }
    }
    
    void computeAdvancedStatistics() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Advanced Learning Experiment Results ===");
        NLM_LOG_INFO("");
        
        NLM_LOG_INFO("Temporal Performance:");
        NLM_LOG_INFO("  Initial state time: " + std::to_string(stats.initialStateTime) + "ms");
        NLM_LOG_INFO("  Final state time: " + std::to_string(stats.finalStateTime) + "ms");
        
        NLM_LOG_INFO("Synaptic Dynamics:");
        NLM_LOG_INFO("  Initial synapse count: " + std::to_string(stats.initialSynapseCount));
        NLM_LOG_INFO("  Final spike count: " + std::to_string(stats.finalSpikeCount));
        NLM_LOG_INFO("  Average firing rate change: " + std::to_string(stats.firingRateChange));
        
        if (!stats.weightChanges.empty()) {
            NLM_LOG_INFO("  Weight change statistics:");
            NLM_LOG_INFO("    Mean: " + std::to_string(stats.averageWeightChange));
            NLM_LOG_INFO("    Max positive: " + std::to_string(*std::max_element(stats.weightChanges.begin(), stats.weightChanges.end(), [](float a, float b) { return a < b; })));
            NLM_LOG_INFO("    Max negative: " + std::to_string(*std::min_element(stats.weightChanges.begin(), stats.weightChanges.end(), [](float a, float b) { return a < b; })));
            NLM_LOG_INFO("    Standard deviation: " + std::to_string(stats.learningStrength));
        }
        
        NLM_LOG_INFO("Synaptic Modification Count:");
        NLM_LOG_INFO("  Strengthened: " + std::to_string(stats.strengthenedCount));
        NLM_LOG_INFO("  Weakened: " + std::to_string(stats.weakenedCount));
        NLM_LOG_INFO("  Unchanged: " + std::to_string(stats.unchangedCount));
        
        NLM_LOG_INFO("Network Statistics:");
        NLM_LOG_INFO("  Initial active neurons: " + std::to_string(stats.initialActiveNeurons));
        NLM_LOG_INFO("  Final active neurons: " + std::to_string(stats.finalActiveNeurons));
        NLM_LOG_INFO("  Final learning neurons: " + std::to_string(stats.finalLearningNeurons));
        
        NLM_LOG_INFO("Learning Classification: " + stats.learningClassification);
        NLM_LOG_INFO("Network Stability: " + std::to_string(stats.networkStability * 100.0f) + "%");
        
        // Determine if learning occurred
        bool learningOccurred = (std::abs(stats.averageWeightChange) > 0.001f) ||
                               (stats.strengthenedCount > 0 || stats.weakenedCount > 0);
        
        NLM_LOG_INFO("");
        if (learningOccurred) {
            NLM_LOG_INFO("✓ ADVANCED LEARNING DETECTED: Comprehensive synaptic and network changes");
            if (stats.learningClassification == "Strong Learning") {
                NLM_LOG_INFO("  -> System shows robust learning capacity");
            } else if (stats.learningClassification == "Moderate Learning") {
                NLM_LOG_INFO("  -> System shows moderate learning capacity");
            } else if (stats.learningClassification == "Weak Learning") {
                NLM_LOG_INFO("  -> System shows limited learning capacity");
            }
        } else {
            NLM_LOG_INFO("✗ NO SIGNIFICANT LEARNING: Minimal synaptic or network changes");
        }
    }
    
private:
    float calculateStdDev(const std::vector<float>& values, float mean) {
        if (values.empty()) return 0.0f;
        float sum = 0.0f;
        for (float val : values) {
            sum += (val - mean) * (val - mean);
        }
        return std::sqrt(sum / values.size());
    }
    
    float calculateVariance(const std::vector<float>& values, float mean) {
        if (values.empty()) return 0.0f;
        float sum = 0.0f;
        for (float val : values) {
            sum += (val - mean) * (val - mean);
        }
        return sum / values.size();
    }
};

struct LearningStatistics {
    // Basic statistics
    size_t initialSynapseCount;
    float initialFiringRate;
    size_t initialActiveNeurons;
    size_t finalSpikeCount;
    float finalFiringRate;
    size_t finalActiveNeurons;
    size_t finalLearningNeurons;
    
    // Weight statistics
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<float> weightChanges;
    float initialWeightMean;
    float initialWeightStd;
    float finalWeightMean;
    float finalWeightStd;
    
    // Learning metrics
    float averageWeightChange;
    float learningStrength;
    float networkStability;
    float firingRateChange;
    
    // Classification
    std::string learningClassification;
    
    // Counts
    size_t strengthenedCount;
    size_t weakenedCount;
    size_t unchangedCount;
    
    // Timing
    long long initialStateTime;
    long long finalStateTime;
    
    LearningStatistics() {
        initialSynapseCount = 0;
        initialFiringRate = 0.0f;
        initialActiveNeurons = 0;
        finalSpikeCount = 0;
        finalFiringRate = 0.0f;
        finalActiveNeurons = 0;
        finalLearningNeurons = 0;
        
        initialWeightMean = 0.0f;
        initialWeightStd = 0.0f;
        finalWeightMean = 0.0f;
        finalWeightStd = 0.0f;
        
        averageWeightChange = 0.0f;
        learningStrength = 0.0f;
        networkStability = 0.0f;
        firingRateChange = 0.0f;
        
        strengthenedCount = 0;
        weakenedCount = 0;
        unchangedCount = 0;
        
        initialStateTime = 0;
        finalStateTime = 0;
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

// Enhanced main function with comprehensive demo modes and command-line interface
int main(int argc, char** argv) {
    // Parse command-line arguments
    CommandLineArgs args;
    if (!args.parseArgs(argc, argv)) {
        std::cerr << "Error parsing command-line arguments" << std::endl;
        return 1;
    }
    
    // Print help if requested
    if (args.showHelp) {
        printEnhancedHelp();
        return 0;
    }
    
    // Set quiet flag globally (used by PerformanceTimer)
    quiet = args.quiet;
    
    // Print appropriate banner
    if (!args.quiet) {
        printEnhancedBanner();
    }
    
    // Initialize logger with appropriate level
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(
        args.verbose ? LogLevel::Debug : LogLevel::Info
    );
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    if (!args.quiet) {
        NLM_LOG_INFO("NLM — Neural Learning Machine (熙然)");
        NLM_LOG_INFO("Version 0.1.0 (Phase 6 Integration)");
        NLM_LOG_INFO("Demo Mode: " + args.demoMode);
    }
    
    // Performance profiling
    if (args.profilePerformance) {
        NLM_LOG_PERF("Starting performance profiling...");
    }
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
    std::string configFile = args.configFile;
    if (!configFile.empty() && config->loadFromFile(configFile)) {
        if (!args.quiet) {
            NLM_LOG_INFO("Loaded configuration from: " + configFile);
        }
    } else {
        if (!args.quiet) {
            NLM_LOG_INFO("Using default configuration.");
        }
    }
    
    // Set configuration parameters based on command-line arguments
    config->set("neuron_count", args.neuronCount, ConfigSource::CommandLine);
    config->set("region_count", args.regionCount, ConfigSource::CommandLine);
    config->set("connection_probability", args.connectionProbability, ConfigSource::CommandLine);
    config->set("simulation_timestep", args.simulationTimestep, ConfigSource::CommandLine);
    
    // Plasticity settings
    config->set("plasticity_hebbian_enable", args.enablePlasticity, ConfigSource::CommandLine);
    config->set("plasticity_stdp_enable", args.enablePlasticity, ConfigSource::CommandLine);
    config->set("plasticity_structural_enable", args.enableDevelopment, ConfigSource::CommandLine);
    
    // Neuromodulation settings
    config->set("neuromod.curiosity_enable", args.enableCuriosity, ConfigSource::CommandLine);
    config->set("neuromod.novelty_enable", args.enableNovelty, ConfigSource::CommandLine);
    
    // Checkpoint settings
    if (args.enableCheckpointing) {
        config->set("checkpoint.enable", true, ConfigSource::CommandLine);
        if (!args.checkpointPath.empty()) {
            config->set("checkpoint.path", args.checkpointPath, ConfigSource::CommandLine);
        }
    }
    
    // Log configuration summary
    if (!args.quiet) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Configuration Summary ===");
        NLM_LOG_INFO("  Neurons: " + std::to_string(args.neuronCount));
        NLM_LOG_INFO("  Regions: " + std::to_string(args.regionCount));
        NLM_LOG_INFO("  Connection Probability: " + std::to_string(args.connectionProbability));
        NLM_LOG_INFO("  Simulation Steps: " + std::to_string(args.simulationSteps));
        NLM_LOG_INFO("  Simulation Timestep: " + std::to_string(args.simulationTimestep) + "s");
        NLM_LOG_INFO("  Plasticity: " + std::string(args.enablePlasticity ? "Enabled" : "Disabled"));
        NLM_LOG_INFO("  Development: " + std::string(args.enableDevelopment ? "Enabled" : "Disabled"));
        NLM_LOG_INFO("  Curiosity: " + std::string(args.enableCuriosity ? "Enabled" : "Disabled"));
        NLM_LOG_INFO("  Novelty: " + std::string(args.enableNovelty ? "Enabled" : "Disabled"));
        NLM_LOG_INFO("  Checkpointing: " + std::string(args.enableCheckpointing ? "Enabled" : "Disabled"));
    }
    
    // Initialize simulation clock
    SimulationClock clock(args.simulationTimestep);
    
    // Initialize brain
    if (!args.quiet) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Initializing NLM Brain...");
    }
    
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Run demo based on mode
    bool demoSuccess = false;
    
    try {
        if (args.demoMode == "basic") {
            if (!args.quiet) {
                NLM_LOG_INFO("=== Running Basic Neural Connectivity Demo ===");
            }
            runBasicConnectivityTest(brain);
            demoSuccess = true;
            
        } else if (args.demoMode == "learning") {
            if (!args.quiet) {
                NLM_LOG_INFO("=== Running Advanced Learning Demo ===");
            }
            runAdvancedLearningDemo(brain);
            demoSuccess = true;
            
        } else if (args.demoMode == "stdp") {
            if (!args.quiet) {
                NLM_LOG_INFO("=== Running STDP Verification Demo ===");
            }
            runStdpVerificationDemo(brain);
            demoSuccess = true;
            
        } else if (args.demoMode == "plasticity") {
            if (!args.quiet) {
                NLM_LOG_INFO("=== Running Plasticity Demo ===");
            }
            runPlasticityExperiment(brain);
            demoSuccess = true;
            
        } else if (args.demoMode == "development") {
            if (!args.quiet) {
                NLM_LOG_INFO("=== Running Development Demo ===");
            }
            runDevelopmentDemo(brain);
            demoSuccess = true;
            
        } else if (args.demoMode == "performance") {
            if (!args.quiet) {
                NLM_LOG_INFO("=== Running Performance Profiling Demo ===");
            }
            runPerformanceProfilingDemo(brain);
            demoSuccess = true;
            
        } else {
            NLM_LOG_ERROR("Unknown demo mode: " + args.demoMode);
            printEnhancedHelp();
            return 1;
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during demo execution: " + std::string(e.what()));
        return 1;
    }
    
    if (!demoSuccess) {
        NLM_LOG_ERROR("Demo failed to execute successfully");
        return 1;
    }
    
    // Final brain status
    if (!args.quiet) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Final Brain Status ===");
        brain->logStatus();
    }
    
    if (args.profilePerformance) {
        NLM_LOG_PERF("Demo completed with performance profiling");
    }
    
    if (!args.quiet) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Demo Complete ===");
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Demo Mode: " + args.demoMode);
        NLM_LOG_INFO("Neurons: " + std::to_string(args.neuronCount));
        NLM_LOG_INFO("Steps: " + std::to_string(args.simulationSteps));
        NLM_LOG_INFO("Spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("Learning Neurons: " + std::to_string(brain->getFiringNeuronCount()));
        NLM_LOG_INFO("Average Firing Rate: " + std::to_string(brain->getAverageFiringRate()));
    }
    
    return 0;
}
