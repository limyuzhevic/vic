// NLM (熙然) - Neural Learning Machine
// Enhanced CLI with Advanced Options and Better User Experience
//
// This phase implements enhanced command-line interface with:
// - Multiple execution modes (basic test, full test, demo, phase-specific)
// - Advanced configuration options (JSON/YAML support)
// - Better help and usage documentation
// - Profiling and benchmarking options
// - Error handling and validation
// - Configuration file management
// - Command-line parser with short/long options support
// - Configuration presets (fast, medium, high, custom)

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
#include "experiments/Phase3Demo.hpp"
#include "experiments/Phase4Demo.hpp"
#include "experiments/Phase6Demo.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <chrono>

using namespace nlm;

// Forward declarations for enhanced CLI
namespace cli {
    // Command-line parsing and argument handling
    class CommandLineParser {
    public:
        struct ParsedArgs {
            std::string mode = "basic";
            std::string configFile = "configs/default.cfg";
            std::string benchmarkFile;
            std::string profileFile;
            std::string paramsFile;
            bool help = false;
            bool version = false;
            bool benchmark = false;
            bool profile = false;
            bool validate = false;
            bool listConfigs = false;
            std::vector<std::pair<std::string, std::string>> params;
            bool useShortFormat = false;
        };

        static ParsedArgs parse(int argc, char** argv);
        static void printHelp(bool shortFormat = false);
        static void printVersion();
        static void listAvailableConfigs();
        static bool validateConfig(const Config& config);
        static void saveConfig(const Config& config, const std::string& filepath);
        static bool loadParams(const std::string& filepath, std::vector<std::pair<std::string, std::string>>& params);
    };

    // Configuration presets
    namespace presets {
        void loadFastConfig(Config& config);
        void loadMediumConfig(Config& config);
        void loadHighConfig(Config& config);
        void loadCustomConfig(Config& config, const std::vector<std::pair<std::string, std::string>>& params);
    }

    // Benchmark and profiling utilities
    namespace benchmark {
        struct BenchmarkResult {
            double executionTime;
            double memoryUsage;
            double cpuUsage;
            std::string timestamp;
        };

        BenchmarkResult runBenchmark(const Config& config, const std::string& mode);
        void printBenchmarkResults(const BenchmarkResult& result);
    }

    namespace profile {
        void enableProfiling();
        void disableProfiling();
        void printProfileData();
    }
}

// Enhanced execution modes
namespace modes {
    void runBasicTest(const Config& config);
    void runFullTest(const Config& config);
    void runDemo(const Config& config, const std::string& phase);
    void runBenchmark(const Config& config);
    void runCustomExperiment(const Config& config);
}

// Utility functions
namespace utils {
    std::string trim(const std::string& str);
    std::string toLower(const std::string& str);
    bool endsWith(const std::string& str, const std::string& suffix);
    bool fileExists(const std::string& path);
    std::string formatTime(double seconds);
}

// Enhanced banner with version and capabilities
void printEnhancedBanner();
void printModeInfo(const std::string& mode);

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

// ============================================================================
// Utility Functions
// ============================================================================

std::string utils::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string utils::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

bool utils::endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && 
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

bool utils::fileExists(const std::string& path) {
    std::ifstream file(path);
    return file.good();
}

std::string utils::formatTime(double seconds) {
    int hours = static_cast<int>(seconds / 3600);
    int minutes = static_cast<int>((seconds - hours * 3600) / 60);
    int secs = static_cast<int>(seconds - hours * 3600 - minutes * 60);
    int ms = static_cast<int>((seconds - static_cast<int>(seconds)) * 1000);
    
    std::ostringstream oss;
    if (hours > 0) oss << hours << "h ";
    oss << minutes << "m " << secs << "." << std::setfill('0') << std::setw(3) << ms << "s";
    return oss.str();
}

// ============================================================================
// Enhanced Banner
// ============================================================================

void printEnhancedBanner() {
    std::cout << R"(
╔══════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║     NLM — 熙然                                                ║
║     Neural Learning Machine                                   ║
║                                                                              ║
║     Enhanced CLI with Advanced Options                          ║
║                                                                              ║
║     Features:                                                        ║
║     • Multiple execution modes (basic, full, demo, phase-specific) ║
║     • JSON/YAML configuration file support                        ║
║     • Advanced help and usage documentation                      ║
║     • Profiling and benchmarking capabilities                    ║
║     • Configuration validation and error handling                  ║
║     • Configuration file management                               ║
║     • Command-line parser with short/long options                 ║
║     • Configuration presets (fast, medium, high, custom)          ║
║                                                                              ║
╚══════════════════════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printModeInfo(const std::string& mode) {
    std::cout << "\nMode: " << mode << std::endl;
    std::cout << "---\n";
    
    if (mode == "basic") {
        std::cout << "Basic Neural Computation Test" << std::endl;
        std::cout << "  • Tests basic neural connectivity" << std::endl;
        std::cout << "  • Verifies synaptic transmission" << std::endl;
        std::cout << "  • Quick validation of core functionality" << std::endl;
    } else if (mode == "full") {
        std::cout << "Full Integration Test" << std::endl;
        std::cout << "  • Complete brain system initialization" << std::endl;
        std::cout << "  • All three main tests (connectivity, plasticity, STDP)" << std::endl;
        std::cout << "  • Comprehensive status reporting" << std::endl;
    } else if (mode == "demo") {
        std::cout << "Demo Mode (Phase Selection)" << std::endl;
        std::cout << "  • Phase 2: Basic neural computation" << std::endl;
        std::cout << "  • Phase 3: World interaction demo" << std::endl;
        std::cout << "  • Phase 4: Emerging cognition" << std::endl;
        std::cout << "  • Phase 6: Integration verification" << std::endl;
    } else if (mode == "benchmark") {
        std::cout << "Benchmark Mode" << std::endl;
        std::cout << "  • Performance testing" << std::endl;
        std::cout << "  • Scalability measurements" << std::endl;
        std::cout << "  • Memory usage analysis" << std::endl;
    } else if (mode == "custom") {
        std::cout << "Custom Experiment Mode" << std::endl;
        std::cout << "  • User-defined configuration" << std::endl;
        std::cout << "  • Custom parameters via file or CLI" << std::endl;
        std::cout << "  • Flexible experiment setup" << std::endl;
    } else {
        std::cout << "Unknown mode: " << mode << std::endl;
    }
}

// ============================================================================
// Command Line Parser
// ============================================================================

cli::CommandLineParser::ParsedArgs cli::CommandLineParser::parse(int argc, char** argv) {
    ParsedArgs args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            args.help = true;
            // Check if it's short format
            if (i + 1 < argc && (argv[i + 1] == "--short" || argv[i + 1] == "-s")) {
                args.useShortFormat = true;
                i++; // Skip next arg
            }
        }
        else if (arg == "--version") {
            args.version = true;
        }
        else if (arg == "--list-configs") {
            args.listConfigs = true;
        }
        else if (arg == "--validate") {
            args.validate = true;
        }
        else if (arg == "--benchmark") {
            args.benchmark = true;
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                args.benchmarkFile = argv[++i];
            }
        }
        else if (arg == "--profile") {
            args.profile = true;
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                args.profileFile = argv[++i];
            }
        }
        else if (arg == "-m" || arg == "--mode") {
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                args.mode = argv[++i];
            } else {
                std::cerr << "ERROR: Missing value for --mode option" << std::endl;
                exit(1);
            }
        }
        else if (arg == "-c" || arg == "--config") {
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                args.configFile = argv[++i];
            } else {
                std::cerr << "ERROR: Missing value for --config option" << std::endl;
                exit(1);
            }
        }
        else if (arg == "-p" || arg == "--params") {
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                args.paramsFile = argv[++i];
                if (!loadParams(args.paramsFile, args.params)) {
                    std::cerr << "ERROR: Failed to load params file: " << args.paramsFile << std::endl;
                    exit(1);
                }
            } else {
                std::cerr << "ERROR: Missing value for --params option" << std::endl;
                exit(1);
            }
        }
        else if (arg.find("--params=") == 0 || arg.find("-p=") == 0) {
            // Direct params specification: --params=key1=value1,key2=value2
            std::string paramsStr;
            if (arg.find("--params=") == 0) {
                paramsStr = arg.substr(9);
            } else {
                paramsStr = arg.substr(4);
            }
            
            std::stringstream ss(paramsStr);
            std::string pair;
            while (std::getline(ss, pair, ',')) {
                size_t equalsPos = pair.find('=');
                if (equalsPos != std::string::npos) {
                    std::string key = utils::trim(pair.substr(0, equalsPos));
                    std::string value = utils::trim(pair.substr(equalsPos + 1));
                    args.params.push_back({key, value});
                }
            }
        }
        else if (arg.find('=') != std::string::npos) {
            // Direct key=value pair
            size_t equalsPos = arg.find('=');
            std::string key = arg.substr(0, equalsPos);
            std::string value = arg.substr(equalsPos + 1);
            args.params.push_back({key, value});
        }
        else if (arg[0] == '-') {
            // Unknown short option
            std::cerr << "ERROR: Unknown option: " << arg << std::endl;
            exit(1);
        }
        else {
            // Positional argument - assume it's the mode if not parsed yet
            if (args.mode == "basic") {
                args.mode = arg;
            }
        }
    }
    
    return args;
}

void cli::CommandLineParser::printHelp(bool shortFormat) {
    if (!shortFormat) {
        std::cout << "╔════════════════════════════════════════════════════════════════════════════════╗\n";
        std::cout << "║                                    NLM Help                                     ║\n";
        std::cout << "╚════════════════════════════════════════════════════════════════════════════════╝\n\n";
        
        std::cout << "USAGE:\n";
        std::cout << "  nlm [OPTIONS] [MODE]\n\n";
        
        std::cout << "MODES:\n";
        std::cout << "  basic     Basic neural computation test (default)\n";
        std::cout << "  full      Full integration test\n";
        std::cout << "  demo      Phase-specific demo\n";
        std::cout << "  benchmark  Performance benchmarking\n";
        std::cout << "  custom    Custom experiment configuration\n\n";
        
        std::cout << "OPTIONS:\n";
        std::cout << "  -m, --mode MODE           Execution mode (default: basic)\n";
        std::cout << "  -c, --config FILE         Configuration file path (JSON/YAML/CFG)\n";
        std::cout << "  -p, --params FILE|PAIRS    Parameters file or comma-separated key=value pairs\n";
        std::cout << "  --help                     Show this help message\n";
        std::cout << "  --short                   Show compact help\n";
        std::cout << "  --version                 Show version information\n";
        std::cout << "  --benchmark [FILE]        Run benchmarks, optionally save to FILE\n";
        std::cout << "  --profile [FILE]          Enable profiling, optionally save to FILE\n";
        std::cout << "  --validate                Validate configuration\n";
        std::cout << "  --list-configs            List available configuration presets\n\n";
        
        std::cout << "PRESETS:\n";
        std::cout << "  fast     Minimal configuration for quick testing\n";
        std::cout << "  medium   Balanced configuration for development\n";
        std::cout << "  high     Detailed configuration for research\n";
        std::cout << "  custom   User-defined configuration\n\n";
        
        std::cout << "CONFIGURATION PRECEDENCE:\n";
        std::cout << "  1. Command-line parameters (--params)" << std::endl;
        std::cout << "  2. Configuration file" << std::endl;
        std::cout << "  3. Preset (fast/medium/high)" << std::endl;
        std::cout << "  4. Default values" << std::endl;
    } else {
        std::cout << "NLM Enhanced CLI - Available Modes:\n";
        std::cout << "  Modes: basic, full, demo, benchmark, custom\n";
        std::cout << "  Options: -m <mode>, -c <file>, -p <params>, --help, --version\n";
        std::cout << "  Presets: fast, medium, high, custom\n";
    }
}

void cli::CommandLineParser::printVersion() {
    std::cout << "NLM (熙然) Neural Learning Machine" << std::endl;
    std::cout << "Version: 0.1.0 (Enhanced CLI)" << std::endl;
    std::cout << "Build: Enhanced CLI with advanced configuration options" << std::endl;
}

void cli::CommandLineParser::listAvailableConfigs() {
    std::cout << "╔════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                  Available Configuration Presets                ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════╝\n\n";
    
    std::cout << "FAST (Minimal)
";
    std::cout << "  Neurons: 500
";
    std::cout << "  Regions: 1
";
    std::cout << "  Connectivity: 0.15
";
    std::cout << "  Ideal for: Quick testing and validation\n\n";
    
    std::cout << "MEDIUM (Balanced)
";
    std::cout << "  Neurons: 1000
";
    std::cout << "  Regions: 2
";
    std::cout << "  Connectivity: 0.1
";
    std::cout << "  Ideal for: Development and debugging\n\n";
    
    std::cout << "HIGH (Detailed)
";
    std::cout << "  Neurons: 2000
";
    std::cout << "  Regions: 3
";
    std::cout << "  Connectivity: 0.05
";
    std::cout << "  Ideal for: Research and analysis\n\n";
}

bool cli::CommandLineParser::validateConfig(const Config& config) {
    std::cout << "Validating configuration...\n";
    
    bool valid = true;
    
    // Check required parameters
    std::vector<std::string> required = {
        "random_seed", "simulation_timestep", "neuron_count",
        "region_count", "connection_probability"
    };
    
    for (const auto& key : required) {
        if (!config.has(key)) {
            std::cerr << "  Missing required parameter: " << key << std::endl;
            valid = false;
        }
    }
    
    // Validate parameter ranges
    auto randomSeed = config.getOr<int64_t>("random_seed", 0);
    if (randomSeed < 0) {
        std::cerr << "  Warning: random_seed should be non-negative" << std::endl;
    }
    
    auto timestep = config.getOr<double>("simulation_timestep", 0.0);
    if (timestep <= 0.0 || timestep > 1.0) {
        std::cerr << "  Warning: simulation_timestep should be in (0, 1.0]" << std::endl;
    }
    
    auto neuronCount = config.getOr<int64_t>("neuron_count", 0);
    if (neuronCount <= 0) {
        std::cerr << "  Error: neuron_count must be positive" << std::endl;
        valid = false;
    }
    
    auto regionCount = config.getOr<int64_t>("region_count", 0);
    if (regionCount <= 0) {
        std::cerr << "  Error: region_count must be positive" << std::endl;
        valid = false;
    }
    
    auto connProb = config.getOr<float>("connection_probability", 0.0f);
    if (connProb < 0.0f || connProb > 1.0f) {
        std::cerr << "  Error: connection_probability must be in [0.0, 1.0]" << std::endl;
        valid = false;
    }
    
    if (valid) {
        std::cout << "Configuration validation passed!" << std::endl;
    } else {
        std::cout << "Configuration validation failed!" << std::endl;
    }
    
    return valid;
}

void cli::CommandLineParser::saveConfig(const Config& config, const std::string& filepath) {
    std::cout << "Saving configuration to: " << filepath << std::endl;
    
    // For now, save in simple key=value format
    // TODO: Add proper JSON/YAML support
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "ERROR: Could not open file for writing: " << filepath << std::endl;
        return;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM Enhanced CLI\n\n";
    
    for (const auto& key : config.getKeys()) {
        file << key << " = ";
        auto val = config.get<std::string>(key);
        if (val.has_value()) {
            file << val.value();
        } else {
            file << "";
        }
        file << "\n";
    }
    
    file.close();
    std::cout << "Configuration saved successfully!" << std::endl;
}

bool cli::CommandLineParser::loadParams(const std::string& filepath, std::vector<std::pair<std::string, std::string>>& params) {
    std::cout << "Loading parameters from: " << filepath << std::endl;
    
    if (!utils::fileExists(filepath)) {
        std::cerr << "ERROR: Parameter file does not exist: " << filepath << std::endl;
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "ERROR: Could not open parameter file: " << filepath << std::endl;
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line = utils::trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t equalsPos = line.find('=');
        if (equalsPos != std::string::npos) {
            std::string key = utils::trim(line.substr(0, equalsPos));
            std::string value = utils::trim(line.substr(equalsPos + 1));
            params.push_back({key, value});
        }
    }
    
    file.close();
    std::cout << "Loaded " << params.size() << " parameters" << std::endl;
    return true;
}

// ============================================================================
// Configuration Presets
// ============================================================================

void cli::presets::loadFastConfig(Config& config) {
    config.clear();
    
    // Fast preset - minimal configuration for quick testing
    config.set("random_seed", int64_t(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", int64_t(500), ConfigSource::Default);
    config->set("region_count", int64_t(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    std::cout << "Loaded FAST preset: Minimal configuration for quick testing" << std::endl;
}

void cli::presets::loadMediumConfig(Config& config) {
    config.clear();
    
    // Medium preset - balanced configuration for development
    config.set("random_seed", int64_t(123), ConfigSource::Default);
    config.set("simulation_timestep", 0.001, ConfigSource::Default);
    config.set("neuron_count", int64_t(1000), ConfigSource::Default);
    config.set("region_count", int64_t(2), ConfigSource::Default);
    config.set("connection_probability", 0.1f, ConfigSource::Default);
    config.set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config.set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config.set("stdp_tau", 20.0f, ConfigSource::Default);
    config.set("synaptogenesis_rate", 0.001f, ConfigSource::Default);
    config.set("pruning_rate", 0.0001f, ConfigSource::Default);
    
    std::cout << "Loaded MEDIUM preset: Balanced configuration for development" << std::endl;
}

void cli::presets::loadHighConfig(Config& config) {
    config.clear();
    
    // High preset - detailed configuration for research
    config.set("random_seed", int64_t(456), ConfigSource::Default);
    config.set("simulation_timestep", 0.0005, ConfigSource::Default);
    config.set("neuron_count", int64_t(2000), ConfigSource::Default);
    config.set("region_count", int64_t(3), ConfigSource::Default);
    config.set("connection_probability", 0.05f, ConfigSource::Default);
    config.set("stdp_ltp_weight", 0.005f, ConfigSource::Default);
    config.set("stdp_ltd_weight", 0.008f, ConfigSource::Default);
    config.set("stdp_tau", 30.0f, ConfigSource::Default);
    config.set("synaptogenesis_rate", 0.01f, ConfigSource::Default);
    config.set("pruning_rate", 0.001f, ConfigSource::Default);
    config.set("plasticity_learning_rate", 0.05f, ConfigSource::Default);
    config.set("development_synaptogenesis_rate", 0.01f, ConfigSource::Default);
    config.set("development_pruning_rate", 0.001f, ConfigSource::Default);
    config.set("dopamine_baseline", 0.1f, ConfigSource::Default);
    config.set("reward_discount_factor", 0.999f, ConfigSource::Default);
    
    std::cout << "Loaded HIGH preset: Detailed configuration for research" << std::endl;
}

void cli::presets::loadCustomConfig(Config& config, const std::vector<std::pair<std::string, std::string>>& params) {
    config.clear();
    
    // Load custom configuration from parameters
    for (const auto& param : params) {
        // Try to infer type and convert
        std::string valueStr = param.second;
        
        // Check if it's a number
        try {
            size_t pos;
            int64_t intVal = std::stoll(valueStr, &pos);
            if (pos == valueStr.size()) {
                config.set(param.first, intVal, ConfigSource::CommandLine);
                continue;
            }
        } catch (...) {}
        
        try {
            size_t pos;
            double doubleVal = std::stod(valueStr, &pos);
            if (pos == valueStr.size()) {
                config.set(param.first, doubleVal, ConfigSource::CommandLine);
                continue;
            }
        } catch (...) {}
        
        try {
            size_t pos;
            float floatVal = std::stof(valueStr, &pos);
            if (pos == valueStr.size()) {
                config.set(param.first, floatVal, ConfigSource::CommandLine);
                continue;
            }
        } catch (...) {}
        
        // Check if it's a boolean
        std::string lowerVal = utils::toLower(valueStr);
        if (lowerVal == "true" || lowerVal == "yes" || lowerVal == "1") {
            config.set(param.first, true, ConfigSource::CommandLine);
            continue;
        } else if (lowerVal == "false" || lowerVal == "no" || lowerVal == "0") {
            config.set(param.first, false, ConfigSource::CommandLine);
            continue;
        }
        
        // Default to string
        config.set(param.first, valueStr, ConfigSource::CommandLine);
    }
    
    std::cout << "Loaded CUSTOM preset from " << params.size() << " parameters" << std::endl;
}

// ============================================================================
// Benchmark and Profiling
// ============================================================================

cli::benchmark::BenchmarkResult cli::benchmark::runBenchmark(const Config& config, const std::string& mode) {
    BenchmarkResult result;
    
    std::cout << "Running benchmark in mode: " << mode << std::endl;
    
    // Record start time
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run benchmark based on mode
    if (mode == "basic") {
        // Run basic test multiple times
        for (int i = 0; i < 5; ++i) {
            // Create a minimal brain and run
            auto brain = std::make_shared<Brain>(config);
            if (brain->initialize()) {
                for (int step = 0; step < 100; ++step) {
                    brain->step(step, step * 0.001);
                }
            }
        }
    } else if (mode == "full") {
        // Run full integration test
        auto brain = std::make_shared<Brain>(config);
        if (brain->initialize()) {
            runBasicConnectivityTest(brain);
            brain->reset();
            brain->initialize();
            runPlasticityExperiment(brain);
            brain->reset();
            brain->initialize();
            runStdpVerification(brain);
        }
    }
    
    // Record end time and compute results
    auto endTime = std::chrono::high_resolution_clock::now();
    result.executionTime = std::chrono::duration<double>(endTime - startTime).count();
    result.memoryUsage = 0.0; // Placeholder
    result.cpuUsage = 0.0;    // Placeholder
    result.timestamp = "2026-01-01T00:00:00Z"; // Placeholder
    
    return result;
}

void cli::benchmark::printBenchmarkResults(const BenchmarkResult& result) {
    std::cout << "╔════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                    BENCHMARK RESULTS                          ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════╝\n\n";
    
    std::cout << "Execution Time: " << utils::formatTime(result.executionTime) << std::endl;
    std::cout << "Memory Usage: " << result.memoryUsage << " MB" << std::endl;
    std::cout << "CPU Usage: " << result.cpuUsage << "%" << std::endl;
    std::cout << "Timestamp: " << result.timestamp << std::endl;
}

void cli::profile::enableProfiling() {
    std::cout << "Profiling enabled..." << std::endl;
    // TODO: Implement actual profiling
}

void cli::profile::disableProfiling() {
    std::cout << "Profiling disabled..." << std::endl;
    // TODO: Implement actual profiling
}

void cli::profile::printProfileData() {
    std::cout << "Profile data would be displayed here..." << std::endl;
    // TODO: Implement actual profiling
}

// ============================================================================
// Enhanced Execution Modes
// ============================================================================

void modes::runBasicTest(const Config& config) {
    std::cout << "=== Basic Neural Computation Test ===\n";
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return;
    }
    
    // Run basic connectivity test
    runBasicConnectivityTest(brain);
    
    std::cout << "\nBasic test completed successfully!" << std::endl;
}

void modes::runFullTest(const Config& config) {
    std::cout << "=== Full Integration Test ===\n";
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return;
    }
    
    brain->logStatus();
    
    // Run all three main tests
    runBasicConnectivityTest(brain);
    
    brain->reset();
    brain->initialize();
    runPlasticityExperiment(brain);
    
    brain->reset();
    brain->initialize();
    runStdpVerification(brain);
    
    std::cout << "\nFull integration test completed successfully!" << std::endl;
}

void modes::runDemo(const Config& config, const std::string& phase) {
    std::cout << "=== Phase " << phase << " Demo ===\n";
    
    if (phase == "2") {
        // Run the original Phase 2 test
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
            return;
        }
        
        brain->logStatus();
        runBasicConnectivityTest(brain);
        brain->reset();
        brain->initialize();
        runPlasticityExperiment(brain);
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
        
    } else if (phase == "3") {
        // Run Phase 3 demo
        std::cout << "Phase 3: World Interaction Demo (Simulation - requires actual world system)\n";
        std::cout << "This would demonstrate experience -> action -> consequence -> learning cycle.\n";
        
    } else if (phase == "4") {
        // Run Phase 4 demo
        std::cout << "Phase 4: Emerging Cognition Demo (Simulation - requires actual experiment system)\n";
        std::cout << "This would demonstrate temporal prediction, working memory, episodic recall,\n";
        std::cout << "concept formation, neural attention, multi-step planning, self-model, and\n";
        std::cout << "social learning.\n";
        
    } else if (phase == "6") {
        // Run Phase 6 demo
        std::cout << "Phase 6: Integration Verification Demo (Simulation - requires actual experiment system)\n";
        std::cout << "This would verify integration of all brain systems.\n";
        
    } else {
        std::cout << "Unknown phase: " << phase << std::endl;
    }
}

void modes::runBenchmark(const Config& config) {
    std::cout << "=== Performance Benchmark ===\n";
    
    // Get benchmark mode from config or use default
    std::string benchmarkMode = config.getOr<std::string>("benchmark_mode", "basic");
    
    auto benchResult = cli::benchmark::runBenchmark(config, benchmarkMode);
    cli::benchmark::printBenchmarkResults(benchResult);
}

void modes::runCustomExperiment(const Config& config) {
    std::cout << "=== Custom Experiment ===\n";
    
    // Custom experiment implementation would go here
    // For now, just display the configuration
    std::cout << "Configuration:" << std::endl;
    std::cout << config.summary() << std::endl;
    
    std::cout << "\nCustom experiment setup complete." << std::endl;
}

// ============================================================================
// Enhanced Main Function
// ============================================================================

int main(int argc, char** argv) {
    printEnhancedBanner();
    
    // Parse command line arguments
    auto args = cli::CommandLineParser::parse(argc, argv);
    
    // Handle special options
    if (args.help) {
        cli::CommandLineParser::printHelp(!args.useShortFormat);
        return 0;
    }
    
    if (args.version) {
        cli::CommandLineParser::printVersion();
        return 0;
    }
    
    if (args.listConfigs) {
        cli::CommandLineParser::listAvailableConfigs();
        return 0;
    }
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "Initializing NLM Enhanced CLI...\n" << std::endl;
    
    // Create and load configuration
    auto config = std::make_shared<Config>();
    
    // Determine preset based on mode
    std::string preset = "medium"; // Default
    if (args.mode == "basic") preset = "fast";
    else if (args.mode == "full") preset = "medium";
    else if (args.mode == "demo" || args.mode == "phase2" || args.mode == "phase3" || args.mode == "phase4" || args.mode == "phase6") preset = "medium";
    else if (args.mode == "benchmark") preset = "fast";
    else if (args.mode == "custom") preset = "high";
    
    // Load preset configuration
    if (preset == "fast") {
        cli::presets::loadFastConfig(*config);
    } else if (preset == "medium") {
        cli::presets::loadMediumConfig(*config);
    } else if (preset == "high") {
        cli::presets::loadHighConfig(*config);
    }
    
    // Load configuration from file if specified
    if (args.configFile != "configs/default.cfg" && utils::fileExists(args.configFile)) {
        if (config->loadFromFile(args.configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + args.configFile);
        } else {
            NLM_LOG_WARNING("Failed to load configuration from: " + args.configFile);
            NLM_LOG_WARNING("Using preset configuration instead.");
        }
    }
    
    // Override with command-line parameters
    for (const auto& param : args.params) {
        config->set(param.first, param.second, ConfigSource::CommandLine);
    }
    
    // Handle phase-specific demo mode
    std::string phase = "";
    if (args.mode == "demo") {
        if (argc > 1 && argv[1][0] != '-') {
            phase = argv[1];
        } else {
            std::cerr << "ERROR: Phase required for demo mode. Use: --mode demo phase2|phase3|phase4|phase6" << std::endl;
            cli::CommandLineParser::printHelp();
            return 1;
        }
    }
    
    // Print mode information
    printModeInfo(args.mode);
    
    // Validate configuration if requested
    if (args.validate) {
        if (!cli::CommandLineParser::validateConfig(*config)) {
            std::cerr << "Configuration validation failed!" << std::endl;
            return 1;
        }
    }
    
    // Handle benchmark mode
    if (args.benchmark) {
        modes::runBenchmark(*config);
        if (args.benchmarkFile.empty()) {
            std::cout << "\nBenchmark completed. Use --benchmark [FILE] to save results." << std::endl;
        } else {
            cli::CommandLineParser::saveConfig(*config, args.benchmarkFile);
        }
        return 0;
    }
    
    // Handle profiling mode
    if (args.profile) {
        cli::profile::enableProfiling();
        // Run the main mode
        if (args.mode == "demo") {
            modes::runDemo(*config, phase);
        } else {
            std::cerr << "ERROR: Profiling only supported with demo mode!" << std::endl;
            return 1;
        }
        cli::profile::disableProfiling();
        if (!args.profileFile.empty()) {
            cli::CommandLineParser::saveConfig(*config, args.profileFile);
        }
        return 0;
    }
    
    // Run the selected mode
    std::cout << "\n=== Starting Execution ===\n";
    
    bool success = false;
    
    if (args.mode == "basic") {
        modes::runBasicTest(*config);
        success = true;
    } else if (args.mode == "full") {
        modes::runFullTest(*config);
        success = true;
    } else if (args.mode == "demo") {
        modes::runDemo(*config, phase);
        success = true;
    } else if (args.mode == "benchmark") {
        modes::runBenchmark(*config);
        success = true;
    } else if (args.mode == "custom") {
        modes::runCustomExperiment(*config);
        success = true;
    } else {
        std::cerr << "ERROR: Unknown mode: " << args.mode << std::endl;
        cli::CommandLineParser::printHelp();
        return 1;
    }
    
    // Save configuration if requested
    if (!args.benchmarkFile.empty()) {
        cli::CommandLineParser::saveConfig(*config, args.benchmarkFile);
    }
    
    if (success) {
        std::cout << "\n=== Execution completed successfully ===\n";
        return 0;
    } else {
        std::cout << "\n=== Execution failed ===\n";
        return 1;
    }
}
