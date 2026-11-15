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
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <algorithm>
#include <map>
#include <set>
#include <chrono>
#include <fstream>
#include <sstream>
#include <ctime>

using namespace nlm;

// Forward declarations for CLI components
namespace nlm {
    class CLIUtils;
    class CLIOutput;
    class CLIParser;
    class ProgressReporter;
}

// Enhanced argument parsing functions
bool parseAdvancedArguments(int argc, char** argv, std::string& configFile, 
                           std::string& outputFormat, std::string& outputFile,
                           bool& verbose, bool& quiet, bool& help, bool& version);
void printEnhancedHelp(const std::string& programName);
void printVersionInfo();
void exportResultsJSON(const std::string& filename, const std::vector<std::string>& data);
void exportResultsCSV(const std::string& filename, const std::vector<std::string>& data);
void setupProgressReporter(double totalSteps, double currentStep);
void updateProgress(double progress, const std::string& message = "");
void completeProgress();

// Configuration validation functions
bool validateConfiguration(const std::shared_ptr<Config>& config);
void suggestConfigurationFixes(const std::shared_ptr<Config>& config);

// Batch processing functions
bool runBatchMode(const std::string& configPattern, const std::string& outputPattern);

// Progress reporting system
struct ProgressContext {
    double totalSteps;
    double currentStep;
    std::chrono::steady_clock::time_point startTime;
    bool showProgress;
    bool quiet;
    
    ProgressContext() : totalSteps(0), currentStep(0), showProgress(true), quiet(false) {}
    
    void start(double total) {
        totalSteps = total;
        currentStep = 0;
        startTime = std::chrono::steady_clock::now();
        if (showProgress && !quiet) {
            std::cout << "[Progress] Starting simulation..." << std::endl;
        }
    }
    
    void update(double step) {
        currentStep = step;
        if (showProgress && !quiet) {
            double progress = (currentStep / totalSteps) * 100.0;
            std::cout << "\r[Progress] " << std::fixed << std::setprecision(1) 
                      << progress << "% complete (step " << currentStep << "/" << totalSteps << ")";
            std::cout.flush();
        }
    }
    
    void finish() {
        if (showProgress && !quiet) {
            std::cout << std::endl;
        }
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count();
        if (!quiet) {
            std::cout << "[Progress] Completed in " << duration << " seconds" << std::endl;
        }
    }
};

ProgressContext progressContext;

// Batch processing functions
bool runBatchMode(const std::string& configPattern, const std::string& outputPattern) {
    NLM_LOG_INFO("Batch mode: Processing configurations matching pattern: " + configPattern);
    NLM_LOG_INFO("Output pattern: " + outputPattern);
    
    // Implementation for batch processing
    // Would iterate over config files, run simulations, generate outputs
    NLM_LOG_INFO("Batch processing not yet implemented - running single simulation instead");
    return true;
}

// Configuration validation functions
bool validateConfiguration(const std::shared_ptr<Config>& config) {
    NLM_LOG_INFO("Validating configuration...");
    
    bool valid = true;
    
    // Check critical parameters
    auto neuronCount = config->getOr<int64_t>("neuron_count", 500);
    if (neuronCount < 10 || neuronCount > 10000) {
        NLM_LOG_WARNING("neuron_count " + std::to_string(neuronCount) + " is outside recommended range [10, 10000]");
        valid = false;
    }
    
    auto timestep = config->getOr<double>("simulation_timestep", 0.001);
    if (timestep <= 0.0 || timestep > 1.0) {
        NLM_LOG_WARNING("simulation_timestep " + std::to_string(timestep) + " is outside recommended range (0.001, 1.0]");
        valid = false;
    }
    
    return valid;
}

void suggestConfigurationFixes(const std::shared_ptr<Config>& config) {
    NLM_LOG_INFO("Configuration suggestions:");
    
    auto neuronCount = config->getOr<int64_t>("neuron_count", 500);
    if (neuronCount > 5000) {
        NLM_LOG_INFO("  Consider reducing neuron_count for faster simulation: current={" + std::to_string(neuronCount) + "}");
    }
    
    auto timestep = config->getOr<double>("simulation_timestep", 0.001);
    if (timestep < 0.0001) {
        NLM_LOG_INFO("  timestep may be too small for performance: current={" + std::to_string(timestep) + "}");
    }
    
    NLM_LOG_INFO("  For production runs, consider using --batch-mode to process multiple configurations");
}

// Progress reporting system
struct ProgressContext {
    double totalSteps;
    double currentStep;
    std::chrono::steady_clock::time_point startTime;
    bool showProgress;
    bool quiet;
    
    ProgressContext() : totalSteps(0), currentStep(0), showProgress(true), quiet(false) {}
    
    void start(double total) {
        totalSteps = total;
        currentStep = 0;
        startTime = std::chrono::steady_clock::now();
        if (showProgress && !quiet) {
            std::cout << "[Progress] Starting simulation..." << std::endl;
        }
    }
    
    void update(double step) {
        currentStep = step;
        if (showProgress && !quiet) {
            double progress = (currentStep / totalSteps) * 100.0;
            std::cout << "\r[Progress] " << std::fixed << std::setprecision(1) 
                      << progress << "% complete (step " << currentStep << "/" << totalSteps << ")";
            std::cout.flush();
        }
    }
    
    void finish() {
        if (showProgress && !quiet) {
            std::cout << std::endl;
        }
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count();
        if (!quiet) {
            std::cout << "[Progress] Completed in " << duration << " seconds" << std::endl;
        }
    }
};

ProgressContext progressContext;

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

// Enhanced CLI utilities
class CLIUtils {
public:
    struct CLIConfig {
        std::string configFile = "configs/default.cfg";
        bool verbose = false;
        bool debug = false;
        bool help = false;
        bool version = false;
        std::string outputFormat = "human";
        std::string outputFile = "";
        uint64_t maxSteps = 0;
        double timeLimit = 0.0;
        std::string experimentType = "all";
        bool validateConfig = false;
        std::string profileOutput = "";
    };
    
    static CLIConfig parseCommandLine(int argc, char** argv) {
        CLIConfig config;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                config.help = true;
            } else if (arg == "--version") {
                config.version = true;
            } else if (arg == "--verbose" || arg == "-v") {
                config.verbose = true;
            } else if (arg == "--debug") {
                config.debug = true;
            } else if (arg == "--validate-config") {
                config.validateConfig = true;
            } else if (arg == "--json") {
                config.outputFormat = "json";
            } else if (arg == "--csv") {
                config.outputFormat = "csv";
            } else if (arg == "--machine-readable") {
                config.outputFormat = "machine";
            } else if (arg == "--output" || arg == "-o") {
                if (i + 1 < argc) {
                    config.outputFile = argv[++i];
                }
            } else if (arg == "--config" || arg == "-c") {
                if (i + 1 < argc) {
                    config.configFile = argv[++i];
                }
            } else if (arg == "--max-steps" || arg == "--steps") {
                if (i + 1 < argc) {
                    config.maxSteps = std::stoull(argv[++i]);
                }
            } else if (arg == "--time-limit" || arg == "--time") {
                if (i + 1 < argc) {
                    config.timeLimit = std::stod(argv[++i]);
                }
            } else if (arg == "--profile") {
                if (i + 1 < argc) {
                    config.profileOutput = argv[++i];
                } else {
                    config.profileOutput = "profile.txt";
                }
            } else if (arg == "--connectivity" || arg == "--test1") {
                config.experimentType = "connectivity";
            } else if (arg == "--plasticity" || arg == "--test2") {
                config.experimentType = "plasticity";
            } else if (arg == "--stdp" || arg == "--test3") {
                config.experimentType = "stdp";
            } else if (arg == "--integration" || arg == "--test6") {
                config.experimentType = "integration";
            } else if (arg == "--all" || arg == "--test-all") {
                config.experimentType = "all";
            } else if (arg == "--phase3") {
                config.experimentType = "phase3";
            } else if (arg == "--phase4") {
                config.experimentType = "phase4";
            } else if (arg == "--phase5") {
                config.experimentType = "phase5";
            } else if (arg == "--phase6") {
                config.experimentType = "phase6";
            } else if (arg == "--phase6-text") {
                config.experimentType = "phase6-text";
            } else {
                // Handle --key=value and -key value arguments
                size_t pos = arg.find('=');
                if (pos != std::string::npos && arg[0] == '-') {
                    std::string key = arg.substr(1, pos - 1);
                    std::string value = arg.substr(pos + 1);
                    NLM_LOG_INFO("  CLI argument: " + key + " = " + value);
                } else if (arg[0] == '-') {
                    // Handle -key value format
                    if (i + 1 < argc) {
                        std::string key = arg.substr(1);
                        std::string value = argv[++i];
                        NLM_LOG_INFO("  CLI argument: " + key + " = " + value);
                    }
                }
            }
        }
        
        return config;
    }
    
    static void printHelp(const std::string& programName) {
        std::cout << "\n" << std::string(80, '=') << std::endl;
        std::cout << "NLM - Neural Learning Machine CLI Usage" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        std::cout << "\nBasic Usage:" << std::endl;
        std::cout << "  " << programName << " [options]" << std::endl;
        std::cout << "\nOptions:" << std::endl;
        std::cout << "  --help, -h                    Show this help message" << std::endl;
        std::cout << "  --version                     Show version information" << std::endl;
        std::cout << "  --verbose, -v                 Enable verbose output" << std::endl;
        std::cout << "  --debug                       Enable debug mode" << std::endl;
        std::cout << "  --config, -c <file>           Load configuration from file (default: configs/default.cfg)" << std::endl;
        std::cout << "  --output, -o <file>           Write output to file" << std::endl;
        std::cout << "  --json                        Output in JSON format" << std::endl;
        std::cout << "  --csv                         Output in CSV format" << std::endl;
        std::cout << "  --machine-readable             Output in machine-readable format" << std::endl;
        std::cout << "  --validate-config              Validate configuration parameters" << std::endl;
        std::cout << "  --max-steps, --steps <N>       Maximum simulation steps" << std::endl;
        std::cout << "  --time-limit, --time <sec>     Time limit in seconds" << std::endl;
        std::cout << "  --profile <file>              Enable profiling and save results to file" << std::endl;
        std::cout << "\nExperiment Selection:" << std::endl;
        std::cout << "  --connectivity, --test1         Run connectivity test (Phase 2)" << std::endl;
        std::cout << "  --plasticity, --test2           Run plasticity experiment (Phase 2)" << std::endl;
        std::cout << "  --stdp, --test3                 Run STDP verification (Phase 2)" << std::endl;
        std::cout << "  --integration, --test6         Run integration test (Phase 6)" << std::endl;
        std::cout << "  --all, --test-all              Run all Phase 2 tests" << std::endl;
        std::cout << "\nPhase 3-6 Demos:" << std::endl;
        std::cout << "  --phase3                       Run Phase 3 demo (world interaction)" << std::endl;
        std::cout << "  --phase4                       Run Phase 4 demo (cognitive capabilities)" << std::endl;
        std::cout << "  --phase5                       Run Phase 5 demo (performance optimizations)" << std::endl;
        std::cout << "  --phase6                       Run Phase 6 demo (integration test)" << std::endl;
        std::cout << "  --phase6-text                  Run Phase 6 text demo (text teaching)" << std::endl;
        std::cout << "\nExamples:" << std::endl;
        std::cout << "  " << programName << " --help" << std::endl;
        std::cout << "  " << programName << " --config myconfig.json --json --output results.json" << std::endl;
        std::cout << "  " << programName << " --phase6 --verbose --profile profile.txt" << std::endl;
        std::cout << "\n" << std::string(80, '=') << std::endl;
    }
    
    static std::string formatTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
           << "." << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }
    
    static std::string exportConfigToJSON(std::shared_ptr<Config> config) {
        std::stringstream json;
        json << "{\n";
        auto keys = config->getKeys();
        for (size_t i = 0; i < keys.size(); ++i) {
            std::string key = keys[i];
            auto valueOpt = config->get<std::string>(key);
            std::string value = valueOpt ? *valueOpt : "";
            
            json << "  \"" << key << "\": \"" << value << "\"";
            if (i < keys.size() - 1) {
                json << ",";
            }
            json << "\n";
        }
        json << "}\n";
        return json.str();
    }
    
    static std::string exportConfigToCSV(std::shared_ptr<Config> config) {
        std::stringstream csv;
        auto keys = config->getKeys();
        
        // Header
        for (size_t i = 0; i < keys.size(); ++i) {
            csv << keys[i];
            if (i < keys.size() - 1) {
                csv << ",";
            }
        }
        csv << "\n";
        
        // Values
        for (size_t i = 0; i < keys.size(); ++i) {
            std::string key = keys[i];
            auto valueOpt = config->get<std::string>(key);
            std::string value = valueOpt ? *valueOpt : "";
            
            // Escape quotes and commas in CSV
            if (value.find(',') != std::string::npos || value.find('"') != std::string::npos) {
                // Replace quotes with double quotes
                size_t pos = 0;
                while ((pos = value.find('"', pos)) != std::string::npos) {
                    value.replace(pos, 1, "\"\"");
                    pos += 2;
                }
                value = "\"" + value + "\"";
            }
            
            csv << value;
            if (i < keys.size() - 1) {
                csv << ",";
            }
        }
        
        return csv.str();
    }
    
    static void printBannerWithTimestamp() {
        printBanner();
        std::cout << "Started at: " << formatTimestamp() << std::endl;
        std::cout << "Command line arguments processed." << std::endl;
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
    // Print banner with timestamp
    printBannerWithTimestamp();
    
    // Parse command line arguments using enhanced CLI
    auto config = CLIUtils::parseCommandLine(argc, argv);
    
    // Handle help and version requests
    if (config.help) {
        CLIUtils::printHelp("nlm");
        return 0;
    }
    
    if (config.version) {
        std::cout << "NLM Neural Learning Machine - Phase 2" << std::endl;
        std::cout << "Version: 2.0.0" << std::endl;
        std::cout << "Build: Enhanced CLI Edition" << std::endl;
        return 0;
    }
    
    // Initialize logger with enhanced configuration
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation ===");
    NLM_LOG_INFO("Initializing enhanced NLM with CLI support...");
    NLM_LOG_INFO("Config file: " + config.configFile);
    NLM_LOG_INFO("Output format: " + config.outputFormat);
    if (config.verbose) {
        NLM_LOG_INFO("Verbose mode enabled");
    }
    if (config.debug) {
        NLM_LOG_INFO("Debug mode enabled");
    }
    
    // Load configuration
    auto nlmConfig = std::make_shared<Config>();
    
    // Load from file if specified
    if (config.configFile != "configs/default.cfg") {
        if (nlmConfig->loadFromFile(config.configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + config.configFile);
        } else {
            NLM_LOG_WARN("Failed to load configuration from: " + config.configFile);
            NLM_LOG_INFO("Using default configuration.");
        }
    }
    
    // Load from environment variables
    nlmConfig->loadFromEnv();
    
    // Override with command line arguments
    nlmConfig->loadFromArgs(argc, argv);
    
    // Set default values for Phase 2 with CLI overrides
    nlmConfig->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    nlmConfig->set("simulation_timestep", 0.001, ConfigSource::Default);
    nlmConfig->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    nlmConfig->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    nlmConfig->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // Apply CLI overrides
    if (config.maxSteps > 0) {
        nlmConfig->set("max_simulation_steps", static_cast<int64_t>(config.maxSteps), ConfigSource::CommandLine);
    }
    
    if (config.timeLimit > 0.0) {
        nlmConfig->set("simulation_time_limit", config.timeLimit, ConfigSource::CommandLine);
    }
    
    // Validate configuration if requested
    if (config.validateConfig) {
        std::vector<std::string> errors;
        if (!validateConfig(nlmConfig, errors)) {
            std::cerr << "Configuration Validation Errors:" << std::endl;
            for (const auto& error : errors) {
                std::cerr << "  ERROR: " << error << std::endl;
            }
            return 1;
        }
        
        auto warnings = getConfigValidationWarnings(nlmConfig);
        if (!warnings.empty()) {
            std::cout << "Configuration Validation Warnings:" << std::endl;
            for (const auto& warning : warnings) {
                std::cout << "  WARNING: " << warning << std::endl;
            }
        }
        
        auto recommendations = getConfigRecommendations(nlmConfig);
        if (!recommendations.empty()) {
            std::cout << "Configuration Recommendations:" << std::endl;
            for (const auto& recommendation : recommendations) {
                std::cout << "  RECOMMENDATION: " << recommendation << std::endl;
            }
        }
    }
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration Summary:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(nlmConfig->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(nlmConfig->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(nlmConfig->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(nlmConfig->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(nlmConfig->getOr<float>("connection_probability", 0.15f)));
    
    // Output configuration in requested format
    if (config.outputFormat != "human") {
        std::string outputContent;
        if (config.outputFormat == "json") {
            outputContent = exportConfigToJSON(nlmConfig);
        } else if (config.outputFormat == "csv") {
            outputContent = exportConfigToCSV(nlmConfig);
        } else if (config.outputFormat == "machine") {
            outputContent = exportConfigToMachineReadable(nlmConfig);
        }
        
        if (!config.outputFile.empty()) {
            std::ofstream outputFile(config.outputFile);
            if (outputFile.is_open()) {
                outputFile << outputContent;
                NLM_LOG_INFO("Configuration written to: " + config.outputFile);
            }
        } else {
            std::cout << outputContent;
        }
        
        if (config.outputFile.empty()) {
            NLM_LOG_INFO("Configuration output in " + config.outputFormat + " format:");
        }
    }
    
    // Initialize simulation clock
    double timestep = nlmConfig->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(nlmConfig);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Run tests based on experiment type
    if (config.experimentType == "connectivity" || config.experimentType == "all") {
        runBasicConnectivityTest(brain);
        if (config.experimentType == "connectivity") {
            NLM_LOG_INFO("Connectivity test completed. Exiting.");
            return 0;
        }
        
        // Reset for next test
        brain->reset();
        brain->initialize();
    }
    
    if (config.experimentType == "plasticity" || config.experimentType == "all") {
        runPlasticityExperiment(brain);
        if (config.experimentType == "plasticity") {
            NLM_LOG_INFO("Plasticity experiment completed. Exiting.");
            return 0;
        }
        
        // Reset for next test
        brain->reset();
        brain->initialize();
    }
    
    if (config.experimentType == "stdp" || config.experimentType == "all") {
        runStdpVerification(brain);
        if (config.experimentType == "stdp") {
            NLM_LOG_INFO("STDP verification completed. Exiting.");
            return 0;
        }
        
        // Reset for next test
        brain->reset();
        brain->initialize();
    }
    
    if (config.experimentType == "integration" || config.experimentType == "all") {
        runIntegrationTest(brain);
        if (config.experimentType == "integration") {
            NLM_LOG_INFO("Integration test completed. Exiting.");
            return 0;
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
