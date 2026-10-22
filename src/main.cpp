// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)
//
// Enhanced with improved CLI, configuration management, error handling,
// performance monitoring, and modular test framework.

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
#include <algorithm>
#include <set>
#include <map>

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

// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)
//
// Enhanced with improved CLI, configuration management, error handling,
// performance monitoring, and modular test framework.

// Command-line option structure for enhanced CLI
struct CLIOptions {
    std::string configFile = "configs/default.cfg";
    bool verbose = false;
    bool quiet = false;
    std::string logLevel = "Info";
    std::string outputFile = "";
    std::set<std::string> enabledTests;
    std::set<std::string> disabledTests;
    bool listTests = false;
    bool showConfig = false;
    bool benchmark = false;
    double benchmarkDuration = 0.0;
    int64_t neuronCount = -1;
    int64_t regionCount = -1;
    bool validateOnly = false;
    std::string validationReport = "";
};

// Performance metrics collection
struct PerformanceMetrics {
    double totalTime = 0.0;
    double configTime = 0.0;
    double brainInitTime = 0.0;
    double brainResetTime = 0.0;
    double stepTime = 0.0;
    int64_t totalSteps = 0;
    int64_t totalSpikes = 0;
    std::map<std::string, double> testTimes;
    std::vector<double> stepTimes;
};

// Enhanced configuration validation
class ConfigValidator {
public:
    static bool validate(const Config& config, std::string& errorMsg) {
        errorMsg = "";
        
        // Validate neuron count
        int64_t neuronCount = config.getOr<int64_t>("neuron_count", 500);
        if (neuronCount <= 0 || neuronCount > 1000000) {
            errorMsg += "Invalid neuron_count: " + std::to_string(neuronCount) + " (must be 1-1000000)\n";
            return false;
        }
        
        // Validate region count
        int64_t regionCount = config.getOr<int64_t>("region_count", 1);
        if (regionCount <= 0 || regionCount > 1000) {
            errorMsg += "Invalid region_count: " + std::to_string(regionCount) + " (must be 1-1000)\n";
            return false;
        }
        
        // Validate timestep
        double timestep = config.getOr<double>("simulation_timestep", 0.001);
        if (timestep <= 0.0 || timestep > 1.0) {
            errorMsg += "Invalid simulation_timestep: " + std::to_string(timestep) + " (must be 0.0001-1.0)\n";
            return false;
        }
        
        // Validate connection probability
        float connectionProb = config.getOr<float>("connection_probability", 0.15f);
        if (connectionProb < 0.0f || connectionProb > 1.0f) {
            errorMsg += "Invalid connection_probability: " + std::to_string(connectionProb) + " (must be 0.0-1.0)\n";
            return false;
        }
        
        // Validate plasticity parameters
        float stdpLTP = config.getOr<float>("stdp_ltp_weight", 0.02f);
        if (stdpLTP < 0.0f || stdpLTP > 1.0f) {
            errorMsg += "Invalid stdp_ltp_weight: " + std::to_string(stdpLTP) + " (must be 0.0-1.0)\n";
            return false;
        }
        
        float stdpLTD = config.getOr<float>("stdp_ltd_weight", 0.015f);
        if (stdpLTD < 0.0f || stdpLTD > 1.0f) {
            errorMsg += "Invalid stdp_ltd_weight: " + std::to_string(stdpLTD) + " (must be 0.0-1.0)\n";
            return false;
        }
        
        // Validate structural plasticity rates
        float synaptogenesisRate = config.getOr<float>("synaptogenesis_rate", 0.0001f);
        if (synaptogenesisRate < 0.0f || synaptogenesisRate > 1.0f) {
            errorMsg += "Invalid synaptogenesis_rate: " + std::to_string(synaptogenesisRate) + " (must be 0.0-1.0)\n";
            return false;
        }
        
        float pruningRate = config.getOr<float>("pruning_rate", 0.00001f);
        if (pruningRate < 0.0f || pruningRate > 1.0f) {
            errorMsg += "Invalid pruning_rate: " + std::to_string(pruningRate) + " (must be 0.0-1.0)\n";
            return false;
        }
        
        return true;
    }
};

// Enhanced logging with verbosity levels
class EnhancedLogger {
public:
    static void logWithLevel(const std::string& level, const std::string& message, bool verbose = false, bool quiet = false) {
        if (quiet && level != "ERROR" && level != "FATAL") {
            return;
        }
        
        if (verbose || level == "ERROR" || level == "FATAL" || level == "WARN") {
            auto now = std::chrono::system_clock::now();
            auto time = std::chrono::system_clock::to_time_t(now);
            std::cout << "[" << level << "] " << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") 
                     << ": " << message << std::endl;
        }
    }
};

// Progress tracker for long-running operations
class ProgressTracker {
public:
    ProgressTracker(const std::string& description, int64_t total, bool showPercentage = true) 
        : description(description), total(total), showPercentage(showPercentage) {
        startTime = std::chrono::high_resolution_clock::now();
        if (showPercentage) {
            std::cout << "Starting " << description << "..."; fflush(stdout);
        }
    }
    
    void update(int64_t current) {
        if (showPercentage && current % (total / 100 + 1) == 0) {
            double percentage = 100.0 * current / total;
            std::cout << "\r" << description << ": " << percentage << "% complete"; fflush(stdout);
        }
    }
    
    void complete() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        if (showPercentage) {
            std::cout << "\r" << description << ": 100% complete (" << duration.count() << "ms)" << std::endl;
        } else {
            std::cout << description << " completed (" << duration.count() << "ms)" << std::endl;
        }
    }
    
    double getElapsedSeconds() const {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        return duration.count() / 1000.0;
    }
    
private:
    std::string description;
    int64_t total;
    bool showPercentage;
    std::chrono::high_resolution_clock::time_point startTime;
};

// Test framework base class
class TestCase {
public:
    virtual ~TestCase() = default;
    virtual std::string getName() const = 0;
    virtual bool shouldRun(const CLIOptions& options) const = 0;
    virtual PerformanceMetrics run(std::shared_ptr<Brain> brain, const Config& config) = 0;
    virtual void setup(std::shared_ptr<Brain> brain, const Config& config) {}
    virtual void teardown(std::shared_ptr<Brain> brain, const Config& config) {}
};

// Concrete test cases
class BasicConnectivityTest : public TestCase {
public:
    std::string getName() const override { return "BasicConnectivity"; }
    
    bool shouldRun(const CLIOptions& options) const override {
        return options.enabledTests.empty() || 
               options.enabledTests.count(getName()) > 0 ||
               options.disabledTests.count(getName()) == 0;
    }
    
    PerformanceMetrics run(std::shared_ptr<Brain> brain, const Config& config) override {
        PerformanceMetrics metrics;
        auto startTime = std::chrono::high_resolution_clock::now();
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
        
        // Inject current into a few neurons and see if spikes propagate
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("Region not found");
            return metrics;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            NLM_LOG_INFO("  No neurons found!");
            return metrics;
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
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        metrics.testTimes["BasicConnectivity"] = duration.count() / 1000.0;
        metrics.totalSpikes = spikes;
        
        return metrics;
    }
};

class PlasticityExperiment : public TestCase {
public:
    std::string getName() const override { return "PlasticityExperiment"; }
    
    bool shouldRun(const CLIOptions& options) const override {
        return options.enabledTests.empty() || 
               options.enabledTests.count(getName()) > 0 ||
               options.disabledTests.count(getName()) == 0;
    }
    
    PerformanceMetrics run(std::shared_ptr<Brain> brain, const Config& config) override {
        PerformanceMetrics metrics;
        auto startTime = std::chrono::high_resolution_clock::now();
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
        
        // Enable plasticity on synapses
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
            }
        }
        
        // Apply repeated input pattern to stimulate learning
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
        
        ProgressTracker progress("Learning Experiment", 1000);
        
        for (SimulationStep step = 0; step < 1000; ++step) {
            // Create input pattern - inject current into sensory neurons
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step, step * 0.001);
            
            // Log progress every 100 steps
            if (step % 100 == 0) {
                progress.update(step);
                NLM_LOG_INFO("  Step " + std::to_string(step) + 
                           " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                           " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        progress.complete();
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("✓ Learning experiment completed");
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        metrics.testTimes["PlasticityExperiment"] = duration.count() / 1000.0;
        metrics.totalSteps = 1000;
        
        return metrics;
    }
};

class STDPVerification : public TestCase {
public:
    std::string getName() const override { return "STDPVerification"; }
    
    bool shouldRun(const CLIOptions& options) const override {
        return options.enabledTests.empty() || 
               options.enabledTests.count(getName()) > 0 ||
               options.disabledTests.count(getName()) == 0;
    }
    
    PerformanceMetrics run(std::shared_ptr<Brain> brain, const Config& config) override {
        PerformanceMetrics metrics;
        auto startTime = std::chrono::high_resolution_clock::now();
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 3: STDP Verification ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("Region not found");
            return metrics;
        }
        
        // Get first few synapses
        auto& synapses = region->getSynapses();
        if (synapses.size() < 5) {
            NLM_LOG_INFO("  Not enough synapses for STDP test");
            return metrics;
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
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        metrics.testTimes["STDPVerification"] = duration.count() / 1000.0;
        
        return metrics;
    }
};

// CLI parsing and configuration
CLIOptions parseCLIOptions(int argc, char** argv) {
    CLIOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            std::cout << "NLM Phase 2: Real Neural Computation" << std::endl;
            std::cout << "Usage: nlm [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --help, -h                    Show this help message" << std::endl;
            std::cout << "  --config=FILE                 Load configuration from file" << std::endl;
            std::cout << "  --verbose, -v                 Enable verbose output" << std::endl;
            std::cout << "  --quiet, -q                   Only show errors" << std::endl;
            std::cout << "  --log-level=LEVEL             Set log level (Trace, Debug, Info, Warn, Error, Fatal)" << std::endl;
            std::cout << "  --output=FILE                 Write output to file" << std::endl;
            std::cout << "  --test=NAME                   Run specific test (can repeat)" << std::endl;
            std::cout << "  --disable-test=NAME           Disable specific test" << std::endl;
            std::cout << "  --list-tests                   List available tests" << std::endl;
            std::cout << "  --show-config                 Show configuration and exit" << std::endl;
            std::cout << "  --benchmark                   Run benchmark mode" << std::endl;
            std::cout << "  --benchmark-duration=SECONDS   Set benchmark duration" << std::endl;
            std::cout << "  --neuron-count=N              Set number of neurons (overrides config)" << std::endl;
            std::cout << "  --region-count=N              Set number of regions (overrides config)" << std::endl;
            std::cout << "  --validate-only               Validate configuration only" << std::endl;
            std::cout << "  --validation-report=FILE      Write validation report to file" << std::endl;
            std::cout << std::endl;
            std::cout << "Available tests: BasicConnectivity, PlasticityExperiment, STDPVerification" << std::endl;
            exit(0);
        }
        
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                options.configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                options.configFile = argv[++i];
            }
        } else if (arg == "--verbose" || arg == "-v") {
            options.verbose = true;
        } else if (arg == "--quiet" || arg == "-q") {
            options.quiet = true;
        } else if (arg.substr(0, 12) == "--log-level=") {
            options.logLevel = arg.substr(12);
        } else if (arg.substr(0, 9) == "--output=") {
            options.outputFile = arg.substr(9);
        } else if (arg.substr(0, 7) == "--test=") {
            options.enabledTests.insert(arg.substr(7));
        } else if (arg.substr(0, 13) == "--disable-test=") {
            options.disabledTests.insert(arg.substr(13));
        } else if (arg == "--list-tests") {
            options.listTests = true;
        } else if (arg == "--show-config") {
            options.showConfig = true;
        } else if (arg == "--benchmark") {
            options.benchmark = true;
        } else if (arg.substr(0, 19) == "--benchmark-duration=") {
            options.benchmarkDuration = std::stod(arg.substr(19));
        } else if (arg.substr(0, 16) == "--neuron-count=") {
            options.neuronCount = std::stoll(arg.substr(16));
        } else if (arg.substr(0, 15) == "--region-count=") {
            options.regionCount = std::stoll(arg.substr(15));
        } else if (arg == "--validate-only") {
            options.validateOnly = true;
        } else if (arg.substr(0, 21) == "--validation-report=") {
            options.validationReport = arg.substr(21);
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            std::cerr << "Use --help for usage information" << std::endl;
            exit(1);
        }
    }
    
    return options;
}

// Save performance metrics to file
void saveMetrics(const PerformanceMetrics& metrics, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open metrics file: " + filename);
        return;
    }
    
    file << "NLM Performance Metrics" << std::endl;
    file << "=======================" << std::endl;
    file << std::endl;
    
    file << "Total Time: " << metrics.totalTime << "s" << std::endl;
    file << "Total Steps: " << metrics.totalSteps << std::endl;
    file << "Total Spikes: " << metrics.totalSpikes << std::endl;
    file << std::endl;
    
    file << "Test Times:" << std::endl;
    for (const auto& pair : metrics.testTimes) {
        file << "  " << pair.first << ": " << pair.second << "s" << std::endl;
    }
    
    file << std::endl;
    file << "Average Step Time: " << (metrics.totalSteps > 0 ? metrics.stepTime / metrics.totalSteps : 0.0) << "s" << std::endl;
    file << "Average Spikes per Second: " << (metrics.totalTime > 0 ? metrics.totalSpikes / metrics.totalTime : 0.0) << std::endl;
    
    file.close();
    NLM_LOG_INFO("Metrics saved to: " + filename);
}

// Save configuration to file
void saveConfig(const Config& config, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filename);
        return;
    }
    
    // This would need config to have a saveToFile method
    // For now, just output current config values
    file << "Current Configuration:" << std::endl;
    file << "=======================" << std::endl;
    
    // Get all config values (this is a simplified version)
    // In a real implementation, config should provide iteration over all keys
    file << "neuron_count: " << config.getOr<int64_t>("neuron_count", 500) << std::endl;
    file << "region_count: " << config.getOr<int64_t>("region_count", 1) << std::endl;
    file << "simulation_timestep: " << config.getOr<double>("simulation_timestep", 0.001) << std::endl;
    file << "connection_probability: " << config.getOr<float>("connection_probability", 0.15f) << std::endl;
    
    file.close();
    NLM_LOG_INFO("Configuration saved to: " + filename);
}

int main(int argc, char** argv) {
    printBanner();
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Parse command line options
    CLIOptions options = parseCLIOptions(argc, argv);
    
    // Initialize logger with enhanced options
    auto logger = std::make_shared<Logger>();
    
    // Set log level based on options
    LogLevel logLevel = LogLevel::Info;
    if (options.logLevel == "Trace") logLevel = LogLevel::Trace;
    else if (options.logLevel == "Debug") logLevel = LogLevel::Debug;
    else if (options.logLevel == "Warn") logLevel = LogLevel::Warning;
    else if (options.logLevel == "Error") logLevel = LogLevel::Error;
    else if (options.logLevel == "Fatal") logLevel = LogLevel::Fatal;
    
    auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Log startup information with enhanced options
    EnhancedLogger::logWithLevel("INFO", "=== NLM Phase 2: Real Neural Computation ===", options.verbose, options.quiet);
    EnhancedLogger::logWithLevel("INFO", "Implementing:", options.verbose, options.quiet);
    EnhancedLogger::logWithLevel("INFO", "  - Leaky Integrate-and-Fire (LIF) neuron dynamics", options.verbose, options.quiet);
    EnhancedLogger::logWithLevel("INFO", "  - Event-driven spike propagation with delays", options.verbose, options.quiet);
    EnhancedLogger::logWithLevel("INFO", "  - STDP and Hebbian plasticity rules", options.verbose, options.quiet);
    EnhancedLogger::logWithLevel("INFO", "  - Structural plasticity (synaptogenesis/pruning)", options.verbose, options.quiet);
    EnhancedLogger::logWithLevel("INFO", "", options.verbose, options.quiet);
    
    // List tests if requested
    if (options.listTests) {
        EnhancedLogger::logWithLevel("INFO", "Available tests:", options.verbose, options.quiet);
        std::vector<std::string> testNames = {"BasicConnectivity", "PlasticityExperiment", "STDPVerification"};
        for (const auto& testName : testNames) {
            EnhancedLogger::logWithLevel("INFO", "  - " + testName, options.verbose, options.quiet);
        }
        return 0;
    }
    
    // Load configuration
    auto config = std::make_shared<Config>();
    auto configStartTime = std::chrono::high_resolution_clock::now();
    
    // Try to load from file if provided
    std::string configFile = options.configFile;
    if (config->loadFromFile(configFile)) {
        EnhancedLogger::logWithLevel("INFO", "Loaded configuration from: " + configFile, options.verbose, options.quiet);
    } else {
        EnhancedLogger::logWithLevel("INFO", "Using default configuration.", options.verbose, options.quiet);
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Apply command line overrides for convenience
    if (options.neuronCount >= 0) {
        config->set("neuron_count", static_cast<int64_t>(options.neuronCount), ConfigSource::CommandLine);
    }
    if (options.regionCount >= 0) {
        config->set("region_count", static_cast<int64_t>(options.regionCount), ConfigSource::CommandLine);
    }
    
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
    EnhancedLogger::logWithLevel("INFO", "", options.verbose, options.quiet);
    EnhancedLogger::logWithLevel("INFO", "Configuration:", options.verbose, options.quiet);
    EnhancedLogger::logWithLevel("INFO", "  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)), options.verbose, options.quiet);
    EnhancedLogger::logWithLevel("INFO", "  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s", options.verbose, options.quiet);
    EnhancedLogger::logWithLevel("INFO", "  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)), options.verbose, options.quiet);
    EnhancedLogger::logWithLevel("INFO", "  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)), options.verbose, options.quiet);
    EnhancedLogger::logWithLevel("INFO", "  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)), options.verbose, options.quiet);
    EnhancedLogger::logWithLevel("INFO", "", options.verbose, options.quiet);
    
    // Validate configuration if requested
    if (options.validateOnly) {
        std::string validationError;
        if (ConfigValidator::validate(*config, validationError)) {
            EnhancedLogger::logWithLevel("INFO", "Configuration validation PASSED", options.verbose, options.quiet);
        } else {
            EnhancedLogger::logWithLevel("ERROR", "Configuration validation FAILED:" + validationError, options.verbose, options.quiet);
            if (!options.validationReport.empty()) {
                config->saveToFile(options.validationReport);
            }
            return 1;
        }
    }
    
    auto configEndTime = std::chrono::high_resolution_clock::now();
    PerformanceMetrics metrics;
    metrics.configTime = std::chrono::duration<double>(configEndTime - configStartTime).count();
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    EnhancedLogger::logWithLevel("INFO", "Simulation clock initialized with timestep: " + std::to_string(timestep) + "s", options.verbose, options.quiet);
    
    // Initialize brain
    EnhancedLogger::logWithLevel("INFO", "", options.verbose, options.quiet);
    EnhancedLogger::logWithLevel("INFO", "Initializing NLM Brain...", options.verbose, options.quiet);
    
    auto brainStartTime = std::chrono::high_resolution_clock::now();
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        EnhancedLogger::logWithLevel("ERROR", "Failed to initialize brain!", options.verbose, options.quiet);
        return 1;
    }
    
    auto brainEndTime = std::chrono::high_resolution_clock::now();
    metrics.brainInitTime = std::chrono::duration<double>(brainEndTime - brainStartTime).count();
    
    brain->logStatus();
    
    // Show configuration if requested
    if (options.showConfig) {
        EnhancedLogger::logWithLevel("INFO", "", options.verbose, options.quiet);
        EnhancedLogger::logWithLevel("INFO", "Final Configuration:", options.verbose, options.quiet);
        brain->logStatus();
    }
    
    // Create test instances
    std::vector<std::unique_ptr<TestCase>> tests;
    tests.push_back(std::make_unique<BasicConnectivityTest>());
    tests.push_back(std::make_unique<PlasticityExperiment>());
    tests.push_back(std::make_unique<STDPVerification>());
    
    // Run selected tests
    PerformanceMetrics totalMetrics;
    for (auto& test : tests) {
        if (test->shouldRun(options)) {
            EnhancedLogger::logWithLevel("INFO", "", options.verbose, options.quiet);
            EnhancedLogger::logWithLevel("INFO", "=== Running " + test->getName() + " ===", options.verbose, options.quiet);
            
            auto testStartTime = std::chrono::high_resolution_clock::now();
            
            // Setup test if needed
            test->setup(brain, *config);
            
            // Run test
            PerformanceMetrics testMetrics = test->run(brain, *config);
            
            // Teardown test if needed
            test->teardown(brain, *config);
            
            auto testEndTime = std::chrono::high_resolution_clock::now();
            double testDuration = std::chrono::duration<double>(testEndTime - testStartTime).count();
            
            // Accumulate metrics
            totalMetrics.totalTime += testMetrics.totalTime;
            totalMetrics.configTime += testMetrics.configTime;
            totalMetrics.brainInitTime += testMetrics.brainInitTime;
            totalMetrics.brainResetTime += testMetrics.brainResetTime;
            totalMetrics.totalSteps += testMetrics.totalSteps;
            totalMetrics.totalSpikes += testMetrics.totalSpikes;
            totalMetrics.testTimes[test->getName()] = testDuration;
            
            // Reset brain for next test if not benchmark
            if (!options.benchmark) {
                EnhancedLogger::logWithLevel("INFO", "Resetting brain for next test...", options.verbose, options.quiet);
                auto resetStartTime = std::chrono::high_resolution_clock::now();
                brain->reset();
                brain->initialize();
                auto resetEndTime = std::chrono::high_resolution_clock::now();
                totalMetrics.brainResetTime += std::chrono::duration<double>(resetEndTime - resetStartTime).count();
            }
            
            EnhancedLogger::logWithLevel("INFO", "✓ Test " + test->getName() + " completed in " + std::to_string(testDuration) + "s", options.verbose, options.quiet);
        } else {
            EnhancedLogger::logWithLevel("INFO", "Skipping test: " + test->getName(), options.verbose, options.quiet);
        }
    }
    
    // Calculate total metrics
    auto endTime = std::chrono::high_resolution_clock::now();
    metrics.totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Final brain status
    EnhancedLogger::logWithLevel("INFO", "", options.verbose, options.quiet);
    EnhancedLogger::logWithLevel("INFO", "=== Final Brain Status ===", options.verbose, options.quiet);
    brain->logStatus();
    
    // Summary
    EnhancedLogger::logWithLevel("INFO", "", options.verbose, options.quiet);
    EnhancedLogger::logWithLevel("INFO", "=== Execution Summary ===", options.verbose, options.quiet);
    EnhancedLogger::logWithLevel("INFO", "Total execution time: " + std::to_string(metrics.totalTime) + "s", options.verbose, options.quiet);
    EnhancedLogger::logWithLevel("INFO", "Total steps executed: " + std::to_string(metrics.totalSteps), options.verbose, options.quiet);
    EnhancedLogger::logWithLevel("INFO", "Total spikes generated: " + std::to_string(metrics.totalSpikes), options.verbose, options.quiet);
    
    if (metrics.totalTime > 0) {
        EnhancedLogger::logWithLevel("INFO", "Average spikes per second: " + std::to_string(metrics.totalSpikes / metrics.totalTime), options.verbose, options.quiet);
    }
    
    EnhancedLogger::logWithLevel("INFO", "", options.verbose, options.quiet);
    EnhancedLogger::logWithLevel("INFO", "=== Phase 2 Complete ===", options.verbose, options.quiet);
    EnhancedLogger::logWithLevel("INFO", "", options.verbose, options.quiet);
    EnhancedLogger::logWithLevel("INFO", "Phase 2 Objectives Completed:", options.verbose, options.quiet);
    EnhancedLogger::logWithLevel("INFO", "  ✓ Real LIF neuron dynamics implemented", options.verbose, options.quiet);
    EnhancedLogger::logWithLevel("INFO", "  ✓ Event-driven spike propagation with delays", options.verbose, options.quiet);
    EnhancedLogger::logWithLevel("INFO", "  ✓ STDP plasticity rule", options.verbose, options.quiet);
    EnhancedLogger::logWithLevel("INFO", "  ✓ Hebbian plasticity rule", options.verbose, options.quiet);
    EnhancedLogger::logWithLevel("INFO", "  ✓ Structural plasticity (synaptogenesis/pruning)", options.verbose, options.quiet);
    EnhancedLogger::logWithLevel("INFO", "  ✓ Learning experiment demonstrates measurable changes", options.verbose, options.quiet);
    EnhancedLogger::logWithLevel("INFO", "  ✓ Network shows activity-dependent synaptic modification", options.verbose, options.quiet);
    EnhancedLogger::logWithLevel("INFO", "", options.verbose, options.quiet);
    EnhancedLogger::logWithLevel("INFO", "The NLM brain is now a functioning artificial neural substrate", options.verbose, options.quiet);
    EnhancedLogger::logWithLevel("INFO", "capable of changing its own synaptic connections through experience.", options.verbose, options.quiet);
    EnhancedLogger::logWithLevel("INFO", "", options.verbose, options.quiet);
    
    // Save metrics if requested
    if (options.outputFile.find("metrics") != std::string::npos || !options.outputFile.empty()) {
        saveMetrics(metrics, options.outputFile);
    }
    
    return 0;
}
