// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)
//
// ARCHITECTURE: Main execution demonstrates the complete brain loop:
// 1. Configuration loading and setup
// 2. Brain initialization with random connectivity
// 3. Three experimental tests to verify functionality:
//    - Test 1: Basic connectivity (spike propagation)
//    - Test 2: Plasticity (learning from experience)
//    - Test 3: STDP verification (pre-post spike timing rules)
//
// The LIF neuron model implements the differential equation:
//   dV/dt = (V_rest - V)/τ + I/C
// Where:
// - V is membrane potential
// - τ (tau) is time constant (20ms)
// - C is membrane capacitance (1nF)
// - I is synaptic input current
// - V_rest is resting potential (-70mV)
// - V_reset is reset potential after spike

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

// Performance optimization flags for different execution modes
#define ENABLE_PERFORMANCE_MEASUREMENTS 1
#define ENABLE_DETAILED_LOGGING 1
#define ENABLE_BRAIN_VISUALIZATION 0

// Experimental test configurations
struct TestConfig {
    SimulationStep steps;
    float currentInjection;
    bool enablePlasticity;
    const char* description;
};

// Define test scenarios for different neural behaviors
static const TestConfig TEST_CONFIGS[] = {
    {50, 50.0f, false, "Basic connectivity test - inject current into sensory neurons"},
    {1000, 30.0f, true, "Plasticity experiment - repeated input patterns for learning"},
    {50, 60.0f, false, "STDP verification - create correlated pre-post spike timing"}
};

// Random seed for reproducible experiments
static const uint64_t DEFAULT_RANDOM_SEED = 42;

// Enhanced header for better architecture visualization
const char* getVersionString() {
    return "NLM v2.0.0 - Phase 2: Real Neural Computation";
}

void printAdvancedBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation                         ║
    ║                                                               ║
    ║     Advanced Scientific Brain Simulator                       ║
    ║                                                               ║
    ║     ARCHITECTURE:                                             ║
    ║     - LIF neurons with membrane dynamics                       ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity                              ║
    ║     - Structural plasticity                                    ║
    ║     - Neuromodulation integration                              ║
    ║                                                               ║
    ║     PERFORMANCE MODE AVAILABLE:                               ║
    ║     - Fast simulation with optimizations                      ║
    ║     - Reduced logging for speed                               ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    
    std::cout << "[INFO] NLM Phase 2 Real Neural Computation" << std::endl;
    std::cout << "[INFO] Building biologically-inspired artificial brains" << std::endl;
    std::cout << "[INFO] Version: " << getVersionString() << std::endl;
    std::cout << "[INFO] Press --help for command-line options" << std::endl << std::endl;
}

using namespace nlm;

// Performance measurement utilities for benchmarking neural computation
class PerformanceTimer {
private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::string name;
    
public:
    PerformanceTimer(const std::string& timerName) : name(timerName) {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    ~PerformanceTimer() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime);
        
        if (ENABLE_PERFORMANCE_MEASUREMENTS) {
            std::cout << "[PERF] " << name << ": " 
                     << std::fixed << std::setprecision(6) << duration.count() 
                     << " seconds" << std::endl;
        }
    }
};

// Enhanced command-line argument parsing with validation and help
bool parseAdvancedCommandLineArgs(int argc, char** argv, 
                                 std::string& configFile, 
                                 bool& enableDebug,
                                 bool& enablePerformanceMode) {
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle configuration file
        if (arg.find("--config") == 0) {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        }
        // Enable debug mode
        else if (arg == "--debug" || arg == "-d") {
            enableDebug = true;
            Logger::getGlobal().setLevel(LogLevel::Debug);
        }
        // Performance mode for faster simulation
        else if (arg == "--performance" || arg == "--fast") {
            enablePerformanceMode = true;
            // Optimization hints
            std::cout << "[INFO] Performance mode enabled - optimizations applied" << std::endl;
        }
        // Help/usage
        else if (arg == "--help" || arg == "-h") {
            std::cout << "=== NLM Phase 2 Help ===" << std::endl;
            std::cout << "--config=file.cfg     Load configuration from file" << std::endl;
            std::cout << "--debug              Enable debug logging" << std::endl;
            std::cout << "--performance        Fast simulation mode" << std::endl;
            std::cout << "--help, -h           Show this help message" << std::endl;
            success = false;  // Exit after showing help
        }
        // Unknown argument
        else if (arg.find("-") == 0) {
            std::cout << "[WARN] Unknown command line argument: " << arg << std::endl;
            success = false;
        }
    }
    
    return success;
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
    ║     An experimental artificial developmental brain.          ║
    ║     This phase implements:                                    ║
    ║     - Real LIF neuron dynamics                                ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity                            ║
    ║     - Structural plasticity                                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
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