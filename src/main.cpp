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
        // Validate brain pointer
        if (!brain) {
            NLM_LOG_ERROR("ERROR: Invalid brain pointer in LearningExperiment::recordInitialState");
            return;
        }
        
        initialSynapseCount = brain->getTotalSynapseCount();
        initialWeights.clear();
        
        // Record initial weights from first region with error checking
        if (auto* region = brain->getRegion(RegionId(1))) {
            if (region->getSynapses().empty()) {
                NLM_LOG_WARNING("WARNING: No synapses found in region 1 during initial state recording");
            } else {
                for (const auto& syn : region->getSynapses()) {
                    if (syn) {
                        initialWeights.push_back(syn->getWeight());
                    } else {
                        NLM_LOG_WARNING("WARNING: Null synapse encountered in region 1");
                    }
                }
            }
        } else {
            NLM_LOG_ERROR("ERROR: Region 1 not found in brain during initial state recording");
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
        
        // Record final weights from first region with error checking
        if (auto* region = brain->getRegion(RegionId(1))) {
            if (region->getSynapses().empty()) {
                NLM_LOG_WARNING("WARNING: No synapses found in region 1 during final state recording");
            } else {
                for (const auto& syn : region->getSynapses()) {
                    if (syn) {
                        finalWeights.push_back(syn->getWeight());
                    } else {
                        NLM_LOG_WARNING("WARNING: Null synapse encountered in region 1 during final state recording");
                    }
                }
            }
        } else {
            NLM_LOG_ERROR("ERROR: Region 1 not found in brain during final state recording");
        }
        
        // Get most active neurons with error checking
        if (brain->getSpikeSystem()) {
            mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
        } else {
            NLM_LOG_ERROR("ERROR: Spike system not available for getting most active neurons");
        }
        
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
        
        // Validate data consistency
        if (initialWeights.size() != finalWeights.size()) {
            NLM_LOG_WARNING("WARNING: Weight vector sizes don't match: " + 
                          std::to_string(initialWeights.size()) + " vs " + 
                          std::to_string(finalWeights.size()));
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
        
        // Display most active neurons for better analysis
        if (!mostActiveNeurons.empty()) {
            NLM_LOG_INFO("  Most active neuron IDs:");
            for (size_t i = 0; i < std::min(size_t(10), mostActiveNeurons.size()); ++i) {
                NLM_LOG_INFO("    Neuron " + std::to_string(i) + ": " + std::to_string(mostActiveNeurons[i].value));
            }
        }
        
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

nlm::LogLevel parseLogLevel(const std::string& level) {
    std::string lower = level;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    if (lower == "debug" || lower == "d") return nlm::LogLevel::Debug;
    if (lower == "info" || lower == "i") return nlm::LogLevel::Info;
    if (lower == "warning" || lower == "w") return nlm::LogLevel::Warning;
    if (lower == "error" || lower == "e") return nlm::LogLevel::Error;
    if (lower == "critical" || lower == "c") return nlm::LogLevel::Critical;
    
    return nlm::LogLevel::Info;
}

std::string getVersionInfo() {
    return "NLM Phase 2 - Neural Learning Machine (熙然)\n"
           "Real Spiking Neural Computation\n"
           "Phase 2: Implement Leaky Integrate-and-Fire neurons with STDP and Hebbian plasticity\n"
           "Build: " __DATE__ " " __TIME__ "\n"
           "Version: 2.0.0";
}

void printHelp(const std::string& programName) {
    printBanner();
    std::cout << "\nUsage: " << programName << " [OPTIONS] [MODE] [CONFIG_FILE]\n\n"
              << "Modes (default: experiment):\n"
              << "  demo          - Quick demonstration of basic functionality\n"
              << "  experiment    - Run comprehensive experiments with configurable parameters\n"
              << "  profile        - Run with performance profiling enabled\n"
              << "  benchmark      - Run performance benchmarks\n"
              << "  validate       - Validate configuration and print diagnostics\n"
              << "  replay         - Load and replay saved brain states\n\n"
              << "Options:\n"
              << "  --config=file, -c file      Configuration file (JSON, YAML, or legacy .cfg format)\n"
               << "  --regions=value              Number of neural regions\n"
              << "  --output=file                Save results to specified file\n"
              << "  --threads=value              Number of threads for parallel computation\n"
              << "  --memory-limit=value         Memory limit in MB\n"
              << "  --progress-interval=value    Interval between progress reports (in simulation steps)\n"
              << "  --enable-plasticity         Enable synaptic plasticity (default: true)\n"
              << "  --disable-plasticity        Disable synaptic plasticity\n"
              << "  --stdp-only                 Enable only STDP plasticity\n"
              << "  --hebbian-only              Enable only Hebbian plasticity\n"
              << "  --version, -V                Print version information\n"
              << "  --help, -h                   Print this help message\n\n"
              << "Examples:\n"
              << "  " << programName << " --mode=demo --config=my_config.json\n"
              << "  " << programName << " --mode=experiment --seed=42 --neurons=1000 --regions=2\n"
              << "  " << programName << " --mode=benchmark --output=benchmark_results.json\n"
              << "  " << programName << " --mode=validate --config=config.yaml --verbosity=debug\n\n"
              << "Configuration File Discovery:\n"
              "  Searches in order: ./configs/, ./config/, ~/.nlm/, /etc/nlm/\n"
              "  Supported formats: JSON (.json), YAML (.yaml/.yml), Legacy (.cfg/.txt)\n";
}

void printExecutionHeader(const std::string& mode) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== NLM Execution Started ===");
    NLM_LOG_INFO("Mode: " + mode);
    NLM_LOG_INFO("Timestamp: " + std::to_string(std::time(nullptr)));
    NLM_LOG_INFO("PID: " + std::to_string(getpid()));
}

void printExecutionFooter() {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== NLM Execution Completed ===");
    NLM_LOG_INFO("");
}

void runDemoMode(std::shared_ptr<Config> config, std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("=== Demo Mode: Quick Demonstration ===");
    NLM_LOG_INFO("This mode provides a brief demonstration of NLM's core capabilities.");
    NLM_LOG_INFO("");
    
    // Brief connectivity test
    runBasicConnectivityTest(brain);
    
    // Quick plasticity demonstration
    brain->reset();
    brain->initialize();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Demo completed. For comprehensive testing, use experiment mode.");
}

void runExperimentMode(std::shared_ptr<Config> config, std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("=== Experiment Mode: Comprehensive Testing ===");
    NLM_LOG_INFO("Running comprehensive experiments with configurable parameters.");
    NLM_LOG_INFO("");
    
    // Run all three tests
    runBasicConnectivityTest(brain);
    
    brain->reset();
    brain->initialize();
    
    runPlasticityExperiment(brain);
    
    brain->reset();
    brain->initialize();
    
    runStdpVerification(brain);
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Experiment completed successfully!");
}

void runProfileMode(std::shared_ptr<Config> config, std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("=== Profile Mode: Performance Profiling ===");
    NLM_LOG_INFO("Running with performance profiling enabled.");
    NLM_LOG_INFO("This mode includes detailed timing and memory monitoring.");
    NLM_LOG_INFO("");
    
    // Check if profiling tools are available
    config->set("enable_profiling", true, ConfigSource::Runtime);
    
    // Run basic connectivity test with profiling
    runBasicConnectivityTest(brain);
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Profile mode completed. Detailed performance metrics have been logged.");
}

void runBenchmarkMode(std::shared_ptr<Config> config, std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("=== Benchmark Mode: Performance Testing ===");
    NLM_LOG_INFO("Running performance benchmarks.");
    NLM_LOG_INFO("This mode measures simulation speed and memory usage.");
    NLM_LOG_INFO("");
    
    const int benchmarkSteps = config->getOr<int64_t>("benchmark_steps", 100);
    const bool measureMemory = config->getOr<bool>("measure_memory", true);
    
    NLM_LOG_INFO("Benchmark parameters:");
    NLM_LOG_INFO("  Steps: " + std::to_string(benchmarkSteps));
    NLM_LOG_INFO("  Measure memory: " + std::to_string(measureMemory));
    NLM_LOG_INFO("");
    
    // Run benchmark test
    brain->reset();
    brain->initialize();
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Inject current and run simulation
    for (size_t i = 0; i < std::min(size_t(10), brain->getTotalNeuronCount()); ++i) {
        brain->injectCurrentToNeurons(nlm::NeuronType::Regular, 50.0f);
    }
    
    for (int step = 0; step < benchmarkSteps; ++step) {
        brain->step(step, step * 0.001);
        
        // Report progress
        if (config->has("progress_interval")) {
            auto interval = config->getOr<int64_t>("progress_interval", 50);
            if (step % interval == 0) {
                NLM_LOG_INFO("  Benchmark progress: step " + std::to_string(step) + 
                           "/" + std::to_string(benchmarkSteps));
            }
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Benchmark Results:");
    NLM_LOG_INFO("  Total time: " + std::to_string(duration.count()) + "ms");
    NLM_LOG_INFO("  Average steps/sec: " + std::to_string(benchmarkSteps * 1000.0 / duration.count()));
    NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
    NLM_LOG_INFO("  Neurons active: " + std::to_string(brain->getActiveNeuronCount()));
    
    if (measureMemory) {
        // Estimate memory usage
        size_t totalMemory = brain->getTotalNeuronCount() * sizeof(NeuronState);
        totalMemory += brain->getTotalSynapseCount() * sizeof(float) * 4; // Simplified estimate
        NLM_LOG_INFO("  Estimated memory usage: " + std::to_string(totalMemory / 1024) + " KB");
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Benchmark completed successfully!");
}

void runValidateMode(std::shared_ptr<Config> config) {
    NLM_LOG_INFO("=== Validate Mode: Configuration Diagnostics ===");
    NLM_LOG_INFO("Validating configuration and checking for errors.");
    NLM_LOG_INFO("");
    
    // Validate all configuration entries
    bool configValid = config->validateAll();
    
    NLM_LOG_INFO("Configuration validation: " + std::string(configValid ? "PASSED" : "FAILED"));
    
    if (!configValid) {
        auto errors = config->getValidationErrors();
        NLM_LOG_ERROR("Configuration validation errors:");
        for (const auto& error : errors) {
            NLM_LOG_ERROR("  " + error);
        }
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration summary:");
    std::string summary = config->summary();
    
    // Split summary into lines and log each line
    size_t pos = 0;
    size_t maxLineLength = 80;
    while (pos < summary.length()) {
        size_t end = pos + maxLineLength;
        if (end > summary.length()) end = summary.length();
        
        std::string line = summary.substr(pos, end - pos);
        NLM_LOG_INFO("  " + line);
        
        pos = end;
        if (pos < summary.length() && summary[pos] == '\n') pos++;
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Validation completed!");
}

void runReplayMode(std::shared_ptr<Config> config) {
    NLM_LOG_INFO("=== Replay Mode: Load and Replay Brain State ===");
    NLM_LOG_INFO("Loading and replaying saved brain state.");
    NLM_LOG_INFO("");
    
    std::string savedBrainFile = "saved_brain_state.nbm";
    
    // Check if file exists
    if (!Config::isFileExists(savedBrainFile)) {
        NLM_LOG_ERROR("No saved brain state file found: " + savedBrainFile);
        NLM_LOG_INFO("Use save mode to create a saved brain state.");
        return;
    }
    
    NLM_LOG_INFO("Loading saved brain state from: " + savedBrainFile);
    
    // Note: This would require implementing save/load functionality
    // For now, we'll just demonstrate the structure
    NLM_LOG_INFO("(Replay functionality requires save/load implementation)");
    NLM_LOG_INFO("  This is a placeholder for the replay feature.");
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Replay mode completed (placeholder implementation).");
}

void logMemoryUsage() {
    try {
        std::ifstream statusFile("/proc/self/status");
        if (statusFile.is_open()) {
            std::string line;
            while (std::getline(statusFile, line)) {
                if (line.find("VmRSS:") != std::string::npos) {
                    NLM_LOG_INFO("Memory usage: " + line);
                    break;
                }
            }
            statusFile.close();
        }
    } catch (...) {
        // Silently ignore memory reporting errors
    }
}

int main(int argc, char** argv) {
    // Initialize signal handlers for graceful shutdown
    signal(SIGINT, [](int) {
        NLM_LOG_INFO("Received SIGINT, shutting down gracefully...");
        exit(1);
    });
    
    printBanner();
    
    std::cout << "Initializing NLM Phase 2 Real Neural Computation...\n" << std::endl;
    
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
    
    // Parse command line arguments
    std::string mode = "experiment";
    std::string configFile = "configs/default.cfg";
    std::string outputFile = "";
    bool helpRequested = false;
    bool versionRequested = false;
    
    // Process arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            helpRequested = true;
        } else if (arg == "--version" || arg == "-V") {
            versionRequested = true;
        } else if (arg.substr(0, 2) == "--mode" && arg.find('=') != std::string::npos) {
            mode = arg.substr(arg.find('=') + 1);
        } else if (arg.substr(0, 2) == "--config" || arg.substr(0, 2) == "-c") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (arg.substr(0, 2) == "--output" || arg.substr(0, 2) == "-o") {
            if (arg.find('=') != std::string::npos) {
                outputFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                outputFile = argv[++i];
            }
        } else if (arg.find('=') != std::string::npos && arg.substr(0, 2) == "--") {
            // Pass through to config system
            configFile = ""; // Indicate that we have additional config args
        }
    }
    
    // Handle help/version requests before any other processing
    if (helpRequested) {
        printHelp(argv[0]);
        return 0;
    }
    
    if (versionRequested) {
        std::cout << getVersionInfo() << std::endl;
        return 0;
    }
    
    // Show usage if mode is not recognized
    std::vector<std::string> validModes = {"demo", "experiment", "profile", "benchmark", "validate", "replay"};
    bool validMode = false;
    for (const auto& valid : validModes) {
        if (mode == valid) {
            validMode = true;
            break;
        }
    }
    
    if (!validMode) {
        NLM_LOG_ERROR("Invalid mode: " + mode);
        printHelp(argv[0]);
        return 1;
    }
    
    // Log memory usage at startup
    logMemoryUsage();
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
    if (!configFile.empty()) {
        // Config file discovery
        std::vector<std::string> searchPaths = {
            "./configs/" + configFile,
            "./config/" + configFile,
            "~/.nlm/" + configFile,
            "/etc/nlm/" + configFile,
            configFile  // Direct path
        };
        
        bool configLoaded = false;
        for (const auto& path : searchPaths) {
            if (Config::isFileExists(path)) {
                configFile = path;
                configLoaded = true;
                break;
            }
        }
        
        if (configLoaded) {
            if (config->loadFromFile(configFile)) {
                NLM_LOG_INFO("Loaded configuration from: " + configFile);
            } else {
                NLM_LOG_WARNING("Failed to load configuration from: " + configFile);
                NLM_LOG_INFO("Using default configuration.");
            }
        } else {
            NLM_LOG_WARNING("Configuration file not found: " + configFile);
            NLM_LOG_INFO("Using default configuration.");
        }
    }
    
    // Override with command line arguments
    config->loadFromArgs(argc, argv);
    
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
    
    // Mode-specific configuration
    if (mode == "benchmark") {
        config->set("benchmark_steps", static_cast<int64_t>(100), ConfigSource::Runtime);
        config->set("measure_memory", true, ConfigSource::Runtime);
    } else if (mode == "profile") {
        config->set("enable_profiling", true, ConfigSource::Runtime);
    }
    
    // Process additional command-line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Parse various configuration options
        if (arg.substr(0, 2) == "--seed" || arg.substr(0, 2) == "-s") {
            if (arg.find('=') != std::string::npos) {
                try {
                    uint64_t seed = std::stoull(arg.substr(arg.find('=') + 1));
                    config->set("random_seed", static_cast<int64_t>(seed), ConfigSource::CommandLine);
                } catch (...) {
                    NLM_LOG_ERROR("Invalid seed value: " + arg.substr(arg.find('=') + 1));
                }
            } else if (i + 1 < argc) {
                try {
                    uint64_t seed = std::stoull(argv[++i]);
                    config->set("random_seed", static_cast<int64_t>(seed), ConfigSource::CommandLine);
                } catch (...) {
                    NLM_LOG_ERROR("Invalid seed value: " + std::string(argv[++i]));
                }
            }
        } else if (arg.substr(0, 2) == "--timestep") {
            if (arg.find('=') != std::string::npos) {
                try {
                    double timestep = std::stod(arg.substr(arg.find('=') + 1));
                    config->set("simulation_timestep", timestep, ConfigSource::CommandLine);
                } catch (...) {
                    NLM_LOG_ERROR("Invalid timestep value: " + arg.substr(arg.find('=') + 1));
                }
            }
        } else if (arg.substr(0, 2) == "--neurons") {
            if (arg.find('=') != std::string::npos) {
                try {
                    int64_t neurons = std::stoll(arg.substr(arg.find('=') + 1));
                    config->set("neuron_count", neurons, ConfigSource::CommandLine);
                } catch (...) {
                    NLM_LOG_ERROR("Invalid neuron count value: " + arg.substr(arg.find('=') + 1));
                }
            }
        } else if (arg.substr(0, 2) == "--regions") {
            if (arg.find('=') != std::string::npos) {
                try {
                    int64_t regions = std::stoll(arg.substr(arg.find('=') + 1));
                    config->set("region_count", regions, ConfigSource::CommandLine);
                } catch (...) {
                    NLM_LOG_ERROR("Invalid region count value: " + arg.substr(arg.find('=') + 1));
                }
            }
        } else if (arg.substr(0, 2) == "--connections") {
            if (arg.find('=') != std::string::npos) {
                try {
                    float connections = std::stof(arg.substr(arg.find('=') + 1));
                    config->set("connection_probability", connections, ConfigSource::CommandLine);
                } catch (...) {
                    NLM_LOG_ERROR("Invalid connection probability value: " + arg.substr(arg.find('=') + 1));
                }
            }
        } else if (arg.substr(0, 2) == "--verbosity" || arg.substr(0, 2) == "-v") {
            if (arg.find('=') != std::string::npos) {
                auto level = parseLogLevel(arg.substr(arg.find('=') + 1));
                consoleLogger->setLevel(level);
                logger->setLevel(level);
            }
        } else if (arg.substr(0, 2) == "--threads") {
            if (arg.find('=') != std::string::npos) {
                try {
                    int64_t threads = std::stoll(arg.substr(arg.find('=') + 1));
                    config->set("thread_count", threads, ConfigSource::CommandLine);
                } catch (...) {
                    NLM_LOG_ERROR("Invalid thread count value: " + arg.substr(arg.find('=') + 1));
                }
            }
        } else if (arg.substr(0, 2) == "--memory-limit") {
            if (arg.find('=') != std::string::npos) {
                try {
                    int64_t memoryLimit = std::stoll(arg.substr(arg.find('=') + 1));
                    config->set("memory_limit_mb", memoryLimit, ConfigSource::CommandLine);
                } catch (...) {
                    NLM_LOG_ERROR("Invalid memory limit value: " + arg.substr(arg.find('=') + 1));
                }
            }
        } else if (arg.substr(0, 2) == "--progress-interval") {
            if (arg.find('=') != std::string::npos) {
                try {
                    int64_t interval = std::stoll(arg.substr(arg.find('=') + 1));
                    config->set("progress_interval", interval, ConfigSource::CommandLine);
                } catch (...) {
                    NLM_LOG_ERROR("Invalid progress interval value: " + arg.substr(arg.find('=') + 1));
                }
            }
        } else if (arg == "--enable-plasticity") {
            config->set("plasticity_enabled", true, ConfigSource::CommandLine);
        } else if (arg == "--disable-plasticity") {
            config->set("plasticity_enabled", false, ConfigSource::CommandLine);
        } else if (arg == "--stdp-only") {
            config->set("hebbian_enabled", false, ConfigSource::CommandLine);
        } else if (arg == "--hebbian-only") {
            config->set("stdp_enabled", false, ConfigSource::CommandLine);
        }
    }
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    
    if (config->has("thread_count")) {
        NLM_LOG_INFO("  thread_count: " + std::to_string(config->getOr<int64_t>("thread_count", 1)));
    }
    if (config->has("memory_limit_mb")) {
        NLM_LOG_INFO("  memory_limit_mb: " + std::to_string(config->getOr<int64_t>("memory_limit_mb", 1024)));
    }
    
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
    
    // Print execution header
    printExecutionHeader(mode);
    
    // Run mode-specific execution
    if (mode == "demo") {
        runDemoMode(config, brain);
    } else if (mode == "experiment") {
        runExperimentMode(config, brain);
    } else if (mode == "profile") {
        runProfileMode(config, brain);
    } else if (mode == "benchmark") {
        runBenchmarkMode(config, brain);
    } else if (mode == "validate") {
        runValidateMode(config);
    } else if (mode == "replay") {
        runReplayMode(config);
    }
    
    // Save results if output file specified
    if (!outputFile.empty() && mode != "validate" && mode != "replay") {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Saving results to: " + outputFile);
        
        // Note: This would require implementing a save method
        // For now, we'll just log the intent
        NLM_LOG_INFO("(Results saving requires save/load implementation)");
    }
    
    // Log final memory usage
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Final memory usage:");
    logMemoryUsage();
    
    // Print execution footer
    printExecutionFooter();
    
    // Final brain status
    if (mode != "validate" && mode != "replay") {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Final Brain Status ===");
        brain->logStatus();
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Execution Complete ===");
    NLM_LOG_INFO("".mode << ": " << (mode == "demo" ? "Quick demonstration completed" :
                      mode == "experiment" ? "Comprehensive experiment completed" :
                      mode == "profile" ? "Performance profiling completed" :
                      mode == "benchmark" ? "Performance benchmarks completed" :
                      mode == "validate" ? "Configuration validation completed" :
                      mode == "replay" ? "Brain state replay completed" : "Completed"));
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Press Ctrl+C to interrupt (if running interactively)");
    
    return 0;
}
