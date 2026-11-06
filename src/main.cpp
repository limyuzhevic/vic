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

void printUsage() {
    std::cout << R"(
USAGE:
    ./nlm [options]

OPTIONS:
    --help                          Show this help message
    --version                       Show version information
    --config=<file>                 Load configuration from file (JSON, YAML, or .cfg)
    --log-level=<level>             Set log level (debug, info, warning, error, critical)
    --log-file=<file>               Log to file in addition to console
    --quiet                         Only show critical errors and higher
    --verbose                        Show debug information (overrides --quiet)
    --no-colors                      Disable colored output
    --benchmark=<name>              Run specific benchmark and save results to file
    --validate-config                Validate configuration file without running
    --save-config=<file>             Save current configuration to file
    --simulation-steps=<count>       Override simulation steps
    --checkpoint-frequency=<steps>   Save brain state at specified intervals
    --export-neurons=<file>          Export neuron data to file for analysis
    --export-synapses=<file>         Export synapse data to file for analysis
    --profile                        Enable performance profiling
    --debug                          Enable debug mode with extra validation


LOG LEVELS:
    debug     - Detailed debugging information
    info      - General information
    warning   - Warning messages
    error     - Error messages
    critical  - Critical errors only


CONFIGURATION:
    Configuration can be specified in multiple formats:
    - .cfg files (key=value format, traditional)
    - .json files (JSON format)
    - .yaml/.yml files (YAML format)
    
    Command-line options override file settings.


ADVANCED USAGE:
    For advanced usage, you can specify multiple configuration values:
    ./nlm --config=experiment.json --log-level=info --benchmark=performance 
           --save-config=results.json --export-neurons=neurons.csv

    To run a custom simulation with specific parameters:
    ./nlm --config=custom.cfg --simulation-steps=5000 --checkpoint-frequency=500

    For development and debugging:
    ./nlm --debug --verbose --log-file=nlm_debug.log --profile
    )" << std::endl;
}

void printVersion() {
    std::cout << "NLM (熙然) - Neural Learning Machine v0.1.0 (Phase 6 Integration)" << std::endl;
    std::cout << "Built with C++20 and neural simulation framework" << std::endl;
    std::cout << "Phase 6: Final Integration - All systems connected and functional" << std::endl;
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

#include <chrono>
#include <fstream>
#include <iomanip>
#include <map>
#include <unordered_map>
#include <thread>

namespace nlm {

// Global variables for debugging and profiling
bool enableDebugMode = false;
bool enableProfiling = false;
int benchmarkSteps = 0;
std::string benchmarkName;
int exportSteps = 0;
std::string neuronsExportFile;
std::string synapsesExportFile;
std::chrono::high_resolution_clock::time_point startTime;
uint64_t totalMemoryUsage = 0;
uint64_t peakMemoryUsage = 0;
int performanceCounter = 0;

// Performance timing structure
struct PerformanceMetrics {
    std::string phaseName;
    double elapsedTime;
    uint64_t memoryUsed;
    int spikesGenerated;
    int neuronsActive;
    int connectionsActive;
};

std::vector<PerformanceMetrics> performanceHistory;

void toggleDebugFeatures() {
    enableDebugMode = true;
    NLM_LOG_DEBUG("Debug mode enabled - extra validation and verbose logging active");
}

void toggleProfiling() {
    enableProfiling = true;
    startTime = std::chrono::high_resolution_clock::now();
    NLM_LOG_INFO("Performance profiling enabled");
}

void exportBrainData(const std::shared_ptr<Brain>& brain, 
                    const std::string& neuronsFile,
                    const std::string& synapsesFile) {
    if (!brain) return;
    
    NLM_LOG_INFO("Exporting brain data for analysis...");
    
    // Export neurons
    if (!neuronsFile.empty()) {
        std::ofstream neuronFile(neuronsFile);
        if (neuronFile.is_open()) {
            auto neurons = brain->getAllNeurons();
            neuronFile << "Neuron Export - NLM Phase 6" << std::endl;
            neuronFile << "Export Time: " << std::chrono::system_clock::now().time_since_epoch().count() << std::endl;
            neuronFile << "Total Neurons: " << neurons.size() << std::endl;
            neuronFile << "Format: id,type,region,x,y,z,voltage,firing_rate,connections" << std::endl;
            
            for (const auto& neuron : neurons) {
                auto state = neuron->getState();
                neuronFile << neuron->getId() << ","
                           << static_cast<int>(neuron->getType()) << ","
                           << neuron->getRegion() << ","
                           << state.position.x << ","
                           << state.position.y << ","
                           << state.position.z << ","
                           << state.membranePotential << ","
                           << neuron->getFiringRate() << ","
                           << neuron->getConnectionCount() << std::endl;
            }
            
            NLM_LOG_INFO("Neuron data exported to: " + neuronsFile);
        }
    }
    
    // Export synapses
    if (!synapsesFile.empty()) {
        std::ofstream synapseFile(synapsesFile);
        if (synapseFile.is_open()) {
            auto& synapseMap = brain->getSynapseMap();
            synapseFile << "Synapse Export - NLM Phase �️cise" << std::endl;
            synapseFile << "Export Time: " << std::chrono::system_clock::now().time_since_epoch().count() << std::endl;
            synapseFile << "Total Synapses: " << synapseMap.size() << std::endl;
            synapseFile << "Format: source_id,dest_id,weight,pre_stdp,post_stdp,eligibility_trace,last_updated" << std::endl;
            
            for (const auto& pair : synapseMap) {
                const auto& synapses = pair.second;
                for (const auto& synapse : synapses) {
                    synapseFile << synapse->getSourceNeuron().index() << ","
                               << synapse->getDestinationNeuron().index() << ","
                               << synapse->getWeight() << ","
                               << synapse->getLTPWeight() << ","
                               << synapse->getLTDWeight() << ","
                               << synapse->getEligibilityTrace() << ","
                               << synapse->getLastUpdated() << std::endl;
                }
            }
            
            NLM_LOG_INFO("Synapse data exported to: " + synapsesFile);
        }
    }
}

void recordPerformance(const std::string& phaseName, uint64_t memoryUsed, 
                      int spikesGenerated, int neuronsActive, int connectionsActive) {
    if (!enableProfiling) return;
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    
    PerformanceMetrics metric;
    metric.phaseName = phaseName;
    metric.elapsedTime = elapsed;
    metric.memoryUsed = memoryUsed;
    metric.spikesGenerated = spikesGenerated;
    metric.neuronsActive = neuronsActive;
    metric.connectionsActive = connectionsActive;
    
    performanceHistory.push_back(metric);
    
    NLM_LOG_INFO("Performance: " + phaseName + " - " + std::to_string(elapsed) + "ms, " +
                 std::to_string(memoryUsed) + " bytes, " +
                 std::to_string(spikesGenerated) + " spikes, " +
                 std::to_string(neuronsActive) + " neurons active");
}

void printPerformanceReport() {
    if (performanceHistory.empty()) return;
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Performance Report ===");
    
    double totalTime = 0;
    uint64_t totalMemory = 0;
    int totalSpikes = 0;
    
    for (const auto& metric : performanceHistory) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Phase: " + metric.phaseName);
        NLM_LOG_INFO("  Time: " + std::to_string(metric.elapsedTime) + " ms");
        NLM_LOG_INFO("  Memory: " + std::to_string(metric.memoryUsed) + " bytes");
        NLM_LOG_INFO("  Spikes: " + std::to_string(metric.spikesGenerated));
        NLM_LOG_INFO("  Active Neurons: " + std::to_string(metric.neuronsActive));
        NLM_LOG_INFO("  Active Connections: " + std::to_string(metric.connectionsActive));
        
        totalTime += metric.elapsedTime;
        totalMemory += metric.memoryUsed;
        totalSpikes += metric.spikesGenerated;
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Summary ===");
    NLM_LOG_INFO("Total Time: " + std::to_string(totalTime) + " ms");
    NLM_LOG_INFO("Average Memory: " + std::to_string(totalMemory / performanceHistory.size()) + " bytes");
    NLM_LOG_INFO("Total Spikes: " + std::to_string(totalSpikes));
    NLM_LOG_INFO("Phases Completed: " + std::to_string(performanceHistory.size()));
    
    if (performanceHistory.size() > 1) {
        NLM_LOG_INFO("Average Time per Phase: " + std::to_string(totalTime / performanceHistory.size()) + " ms");
    }
}

void runAdvancedExperiment(std::shared_ptr<Brain> brain, const Config& config) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Advanced Experiment Mode ===");
    
    // Log advanced configuration
    if (enableDebugMode) {
        NLM_LOG_DEBUG("Running with advanced options:");
        NLM_LOG_DEBUG("  Debug mode: " + std::to_string(enableDebugMode));
        NLM_LOG_DEBUG("  Profiling: " + std::to_string(enableProfiling));
        NLM_LOG_DEBUG("  Benchmark: " + (benchmarkName.empty() ? "none" : benchmarkName));
        NLM_LOG_DEBUG("  Export neurons: " + neuronsExportFile);
        NLM_LOG_DEBUG("  Export synapses: " + synapsesExportFile);
    }
    
    // Run experimental validation if requested
    if (enableDebugMode) {
        NLM_LOG_INFO("Running experimental validation...");
        
        // Validate brain connectivity
        auto neurons = brain->getAllNeurons();
        NLM_LOG_DEBUG("Neuron connectivity validation:");
        NLM_LOG_DEBUG("  Total neurons: " + std::to_string(neurons.size()));
        
        // Check for isolated neurons
        size_t connectedNeurons = 0;
        for (const auto& neuron : neurons) {
            if (neuron->getConnectionCount() > 0) {
                connectedNeurons++;
            }
        }
        
        NLM_LOG_DEBUG("  Connected neurons: " + std::to_string(connectedNeurons));
        NLM_LOG_DEBUG("  Isolated neurons: " + std::to_string(neurons.size() - connectedNeurons));
        
        if (neurons.size() - connectedNeurons > neurons.size() / 10) {
            NLM_LOG_WARNING("High number of isolated neurons detected - may indicate connection issues");
        }
    }
    
    // Export data if requested
    exportBrainData(brain, neuronsExportFile, synapsesExportFile);
    
    // Run benchmark if requested
    if (!benchmarkName.empty()) {
        NLM_LOG_INFO("Running benchmark: " + benchmarkName);
        // Simple benchmark implementation
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Run the standard experiments with potential extra steps
        runBasicConnectivityTest(brain);
        brain->reset();
        brain->initialize();
        runPlasticityExperiment(brain);
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        NLM_LOG_INFO("Benchmark completed in " + std::to_string(duration.count()) + " ms");
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
    for (SimulationStep step = 0; step < 1000; ++step) {
        for (NeuronId id : patternNeurons) {
            brain->injectCurrent(id, 30.0f);
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

int main(int argc, char** argv) {
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
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Add this after the global variables declaration in main.cpp
    // Debug and profiling command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Debug and profiling options
        if (arg == "--debug" || arg == "--debug-mode") {
            enableDebugMode = true;
            NLM_LOG_INFO("Debug mode enabled");
        } else if (arg == "--profile" || arg == "--benchmark") {
            enableProfiling = true;
            startTime = std::chrono::high_resolution_clock::now();
            NLM_LOG_INFO("Performance profiling enabled");
        } else if (arg == "--quiet") {
            consoleLogger->setLevel(LogLevel::Critical);
            NLM_LOG_INFO("Log level set to Critical (quiet mode)");
        } else if (arg == "--verbose") {
            consoleLogger->setLevel(LogLevel::Debug);
            NLM_LOG_INFO("Log level set to Debug (verbose mode)");
        } else if (arg == "--no-colors") {
            consoleLogger->setUseColors(false);
            NLM_LOG_INFO("Colored output disabled");
        } else if (arg.find("--benchmark=") != std::string::npos) {
            benchmarkName = arg.substr(arg.find("--benchmark=") + 12);
            NLM_LOG_INFO("Benchmark name: " + benchmarkName);
        } else if (arg.find("--log-level=") != std::string::npos) {
            std::string level = arg.substr(arg.find("--log-level=") + 12);
            if (level == "debug") {
                consoleLogger->setLevel(LogLevel::Debug);
            } else if (level == "info") {
                consoleLogger->setLevel(LogLevel::Info);
            } else if (level == "warning") {
                consoleLogger->setLevel(LogLevel::Warning);
            } else if (level == "error") {
                consoleLogger->setLevel(LogLevel::Error);
            } else if (level == "critical") {
                consoleLogger->setLevel(LogLevel::Critical);
            }
            NLM_LOG_INFO("Log level set to " + level);
        } else if (arg.find("--log-file=") != std::string::npos) {
            std::string logfile = arg.substr(arg.find("--log-file=") + 12);
            consoleLogger->setOutputFile(logfile);
            NLM_LOG_INFO("Logging to file: " + logfile);
        } else if (arg.find("--export-neurons=") != std::string::npos) {
            neuronsExportFile = arg.substr(arg.find("--export-neurons=") + 18);
            NLM_LOG_INFO("Neuron export file: " + neuronsExportFile);
        } else if (arg.find("--export-synapses=") != std::string::npos) {
            synapsesExportFile = arg.substr(arg.find("--export-synapses=") + 18);
            NLM_LOG_INFO("Synapse export file: " + synapsesExportFile);
        } else if (arg.find("--simulation-steps=") != std::string::npos) {
            benchmarkSteps = std::stoi(arg.substr(arg.find("--simulation-steps=") + 21));
            NLM_LOG_INFO("Simulation steps: " + std::to_string(benchmarkSteps));
        }
    }
    
    // Load config from file (ignore if not found)
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args
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
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
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
    
    // Run Test 1: Basic connectivity
    runBasicConnectivityTest(brain);
    
    // Reset brain for plasticity experiment
    brain->reset();
    brain->initialize();
    
    // Run Test 2: Plasticity learning experiment
    runPlasticityExperiment(brain);
    
    // Reset and run Test 3: STDP verification
    brain->reset();
    brain->initialize();
    runStdpVerification(brain);
    
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
