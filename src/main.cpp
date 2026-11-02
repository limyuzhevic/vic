// NLM (熙然) - Neural Learning Machine
// Improved Phase 2: Real Neural Computation with Enhanced CLI and Modularity
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)
// - Enhanced command-line interface and test selection
// - Comprehensive error handling and validation

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
#include <unordered_map>
#include <algorithm>
#include <stdexcept>
#include <sstream>

using namespace nlm;

// Command-line argument parser
class CommandLineParser {
public:
    struct Options {
        std::string configFile = "configs/default.cfg";
        bool help = false;
        bool version = false;
        std::string mode = "full"; // full, connectivity, plasticity, stdp, custom
        bool verbose = false;
        bool quiet = false;
        uint32_t steps = 0;
        float currentStrength = 0.0f;
        size_t neuronsToStimulate = 0;
        bool runBenchmark = false;
        bool saveCheckpoint = false;
        std::string checkpointFile;
        bool loadCheckpoint = false;
        std::string loadFile;
        bool validateConfig = false;
    };
    
    static Options parse(int argc, char** argv) {
        Options opts;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                opts.help = true;
            } else if (arg == "--version") {
                opts.version = true;
            } else if (arg == "--config") {
                if (i + 1 < argc) {
                    opts.configFile = argv[++i];
                } else {
                    throw std::runtime_error("Error: --config requires a filename");
                }
            } else if (arg == "--mode") {
                if (i + 1 < argc) {
                    opts.mode = argv[++i];
                } else {
                    throw std::runtime_error("Error: --mode requires a mode name");
                }
            } else if (arg == "--verbose" || arg == "-v") {
                opts.verbose = true;
            } else if (arg == "--quiet" || arg == "-q") {
                opts.quiet = true;
            } else if (arg == "--steps") {
                if (i + 1 < argc) {
                    opts.steps = std::stoul(argv[++i]);
                } else {
                    throw std::runtime_error("Error: --steps requires a number");
                }
            } else if (arg == "--current") {
                if (i + 1 < argc) {
                    opts.currentStrength = std::stof(argv[++i]);
                } else {
                    throw std::runtime_error("Error: --current requires a float value");
                }
            } else if (arg == "--neurons") {
                if (i + 1 < argc) {
                    opts.neuronsToStimulate = std::stoul(argv[++i]);
                } else {
                    throw std::runtime_error("Error: --neurons requires a number");
                }
            } else if (arg == "--benchmark") {
                opts.runBenchmark = true;
            } else if (arg == "--save") {
                opts.saveCheckpoint = true;
                if (i + 1 < argc) {
                    opts.checkpointFile = argv[++i];
                } else {
                    opts.checkpointFile = "brain_checkpoint.bin";
                }
            } else if (arg == "--load") {
                opts.loadCheckpoint = true;
                if (i + 1 < argc) {
                    opts.loadFile = argv[++i];
                } else {
                    throw std::runtime_error("Error: --load requires a filename");
                }
            } else if (arg == "--validate") {
                opts.validateConfig = true;
            } else {
                // Unknown argument
                throw std::runtime_error("Error: unknown argument: " + arg);
            }
        }
        
        return opts;
    }
    
    static void printHelp() {
        std::cout << R"(
NLM - Neural Learning Machine (熙然)

USAGE:
    ./nlm [OPTIONS]

MODES:
    --mode full           Run all tests (default)
    --mode connectivity   Run only basic connectivity test
    --mode plasticity     Run only plasticity learning experiment  
    --mode stdp           Run only STDP verification
    --mode custom         Run custom test with specified parameters

CONFIGURATION:
    --config filename     Load configuration from file (default: configs/default.cfg)
    --validate            Validate configuration before running

EXECUTION CONTROL:
    --verbose, -v         Verbose output
    --quiet, -q           Quiet mode (only errors and results)
    --steps N             Number of simulation steps for custom mode
    --current FLOAT       Current strength for stimulation (mV)
    --neurons N           Number of neurons to stimulate

CHECKPOINT CONTROL:
    --save [filename]     Save brain state to checkpoint (default: brain_checkpoint.bin)
    --load filename       Load brain state from checkpoint

MISCELLANEOUS:
    --help, -h            Show this help message
    --version             Show version information

EXAMPLES:
    ./nlm --mode full                    # Run all tests with default config
    ./nlm --mode connectivity --steps 100  # Run connectivity test with 100 steps
    ./nlm --mode custom --current 50 --neurons 20  # Custom stimulation
    ./nlm --load saved_brain.bin --mode full  # Resume from checkpoint
    ./nlm --validate --config my_config.cfg  # Validate config file

NOTES:
    Mode selection is exclusive - only the last --mode specified is used.
    Default configuration is in configs/default.cfg.

)" << std::endl;
    }
};

// Learning experiment analysis
class LearningAnalyzer {
public:
    struct AnalysisResults {
        float initialMeanWeight = 0.0f;
        float finalMeanWeight = 0.0f;
        float weightChange = 0.0f;
        size_t strengthened = 0;
        size_t weakened = 0;
        size_t unchanged = 0;
        size_t totalSpikes = 0;
        double learningScore = 0.0;
        bool learningDetected = false;
    };
    
    static AnalysisResults analyzeExperiment(
        const std::vector<float>& initialWeights,
        const std::vector<float>& finalWeights,
        const std::shared_ptr<Brain>& brain) {
        
        AnalysisResults results;
        
        if (initialWeights.empty() || finalWeights.empty()) {
            std::cerr << "ERROR: No weights recorded for analysis" << std::endl;
            return results;
        }
        
        // Compute weight statistics
        float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        results.initialMeanWeight = initialSum / initialWeights.size();
        results.finalMeanWeight = finalSum / finalWeights.size();
        results.weightChange = results.finalMeanWeight - results.initialMeanWeight;
        
        // Count synaptic changes
        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        for (size_t i = 0; i < minSize; ++i) {
            float delta = finalWeights[i] - initialWeights[i];
            if (delta > 0.01f) ++results.strengthened;
            else if (delta < -0.01f) ++results.weakened;
            else ++results.unchanged;
        }
        
        // Get spike statistics
        results.totalSpikes = brain->getTotalSpikeCount();
        
        // Calculate learning score (weighted combination of metrics)
        float weightChangeImportance = std::abs(results.weightChange);
        float activityImportance = static_cast<float>(results.totalSpikes) / 1000.0f;
        float plasticityScore = static_cast<float>(results.strengthened) / std::max<size_t>(1, initialWeights.size());
        
        results.learningScore = (weightChangeImportance + activityImportance + plasticityScore) / 3.0f;
        results.learningDetected = (std::abs(results.weightChange) > 0.001f) || 
                                  (results.strengthened > 0 || results.weakened > 0);
        
        return results;
    }
    
    static void printAnalysis(const AnalysisResults& results, const std::string& testName) {
        std::cout << "\n=== " << testName << " Results ===" << std::endl;
        
        std::cout << "Weight Statistics:" << std::endl;
        std::cout << "  Initial mean weight: " << std::fixed << std::setprecision(4) << results.initialMeanWeight << std::endl;
        std::cout << "  Final mean weight:   " << std::fixed << std::setprecision(4) << results.finalMeanWeight << std::endl;
        std::cout << "  Change:              " << std::fixed << std::setprecision(4) << results.weightChange << std::endl;
        
        std::cout << "\nSynaptic Changes:" << std::endl;
        std::cout << "  Strengthened: " << results.strengthened << std::endl;
        std::cout << "  Weakened:    " << results.weakened << std::endl;
        std::cout << "  Unchanged:   " << results.unchanged << std::endl;
        
        std::cout << "\nActivity Summary:" << std::endl;
        std::cout << "  Total spikes: " << results.totalSpikes << std::endl;
        
        std::cout << "\nLearning Assessment:" << std::endl;
        if (results.learningDetected) {
            std::cout << "  ✓ LEARNING DETECTED: Score = " << std::fixed << std::setprecision(3) << results.learningScore << std::endl;
        } else {
            std::cout << "  ✗ NO LEARNING: Weights did not change significantly" << std::endl;
        }
    }
};

// Enhanced logging utilities
class LogUtils {
public:
    static void logTestHeader(const std::string& testName, const std::unordered_map<std::string, std::string>& metadata = {}) {
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "=== " << testName << " ===" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        
        for (const auto& pair : metadata) {
            std::cout << "  " << pair.first << ": " << pair.second << std::endl;
        }
    }
    
    static void logSuccess(const std::string& message) {
        std::cout << "  ✓ " << message << std::endl;
    }
    
    static void logError(const std::string& message) {
        std::cerr << "  ✗ ERROR: " << message << std::endl;
    }
    
    static void logInfo(const std::string& message) {
        std::cout << "  ℹ " << message << std::endl;
    }
    
    static void logWarning(const std::string& message) {
        std::cout << "  ⚠ " << message << std::endl;
    }
};

// Test classes
class BasicConnectivityTest {
public:
    static bool run(std::shared_ptr<Brain> brain, uint32_t steps = 50, 
                   float currentStrength = 50.0f, size_t neuronsToStimulate = 10,
                   bool verbose = false) {
        
        LogUtils::logTestHeader("Test 1: Basic Neural Connectivity");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            LogUtils::logError("No regions found in brain");
            return false;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            LogUtils::logError("No neurons found!");
            return false;
        }
        
        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        if (verbose) {
            LogUtils::logInfo("Injecting current into " + std::to_string(std::min(neuronsToStimulate, neurons.size())) + 
                            " neurons...");
        }
        
        // Inject current into specified neurons
        size_t neuronsToStim = std::min(neuronsToStimulate, neurons.size());
        for (size_t i = 0; i < neuronsToStim; ++i) {
            neurons[i]->injectCurrent(currentStrength);
        }
        
        // Run simulation steps
        for (SimulationStep step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikesGenerated = brain->getTotalSpikeCount() - initialSpikes;
        
        LogUtils::logInfo("Spikes generated: " + std::to_string(spikesGenerated));
        
        if (spikesGenerated > 0) {
            LogUtils::logSuccess("Spikes propagate through network");
            return true;
        } else {
            LogUtils::logWarning("No spikes - checking neuron parameters...");
            for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
                float v = neurons[i]->getMembranePotential();
                float thresh = neurons[i]->getThreshold();
                LogUtils::logInfo("  Neuron " + std::to_string(i) + 
                                " V=" + std::to_string(v) + 
                                " thresh=" + std::to_string(thresh));
            }
            return false;
        }
    }
};

class PlasticityExperiment {
public:
    struct LearningData {
        std::vector<float> initialWeights;
        std::vector<float> finalWeights;
        size_t initialSynapseCount = 0;
        std::vector<NeuronId> mostActiveNeurons;
        size_t totalSpikes = 0;
    };
    
    static LearningData run(std::shared_ptr<Brain> brain, uint32_t steps = 1000,
                           float currentStrength = 30.0f, size_t neuronsToStimulate = 20,
                           bool verbose = false) {
        
        LogUtils::logTestHeader("Test 2: Plasticity Learning Experiment");
        
        LearningData data;
        
        // Record initial state
        data.initialSynapseCount = brain->getTotalSynapseCount();
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                data.initialWeights.push_back(syn->getWeight());
            }
        }
        
        LogUtils::logInfo("Initial state recorded:");
        LogUtils::logInfo("  Synapses: " + std::to_string(data.initialSynapseCount));
        if (!data.initialWeights.empty()) {
            float sum = std::accumulate(data.initialWeights.begin(), data.initialWeights.end(), 0.0f);
            float mean = sum / data.initialWeights.size();
            LogUtils::logInfo("  Mean weight: " + std::to_string(mean));
        }
        
        // Enable plasticity
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false); // Enable Hebbian and STDP
            }
        }
        
        // Prepare stimulation pattern
        std::vector<NeuronId> patternNeurons;
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto* neuron : region->getAllNeurons()) {
                if (neuron->getType() == NeuronType::Sensory && patternNeurons.size() < neuronsToStimulate) {
                    patternNeurons.push_back(neuron->getId());
                }
            }
        }
        
        LogUtils::logInfo("Applying repeated input patterns (" + std::to_string(steps) + " steps)...");
        LogUtils::logInfo("Stimulating " + std::to_string(patternNeurons.size()) + " sensory neurons each step.");
        
        // Apply stimulation pattern
        for (SimulationStep step = 0; step < steps; ++step) {
            for (NeuronId id : patternNeurons) {
                brain->injectCurrent(id, currentStrength);
            }
            
            brain->step(step, step * 0.001);
            
            if (verbose && step % 100 == 0) {
                LogUtils::logInfo("  Step " + std::to_string(step) + 
                                " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                                " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        // Record final state
        data.totalSpikes = brain->getTotalSpikeCount();
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                data.finalWeights.push_back(syn->getWeight());
            }
            data.mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
        }
        
        LogUtils::logInfo("Final state recorded:");
        LogUtils::logInfo("  Total spikes: " + std::to_string(data.totalSpikes));
        if (!data.finalWeights.empty()) {
            float sum = std::accumulate(data.finalWeights.begin(), data.finalWeights.end(), 0.0f);
            float mean = sum / data.finalWeights.size();
            LogUtils::logInfo("  Mean weight: " + std::to_string(mean));
        }
        
        return data;
    }
    
    static LearningData runFull(std::shared_ptr<Brain> brain) {
        return run(brain, 1000, 30.0f, 20, false);
    }
    
    static AnalysisResults analyze(const LearningData& data) {
        return LearningAnalyzer::analyzeExperiment(data.initialWeights, data.finalWeights, 
                                                  std::make_shared<Brain>(std::shared_ptr<Brain>()));
    }
    
    static void printResults(const LearningData& data, const AnalysisResults& analysis) {
        std::cout << "\n=== Plasticity Experiment Results ===" << std::endl;
        
        std::cout << "\nSynaptic Changes:" << std::endl;
        size_t strengthened = 0, weakened = 0, unchanged = 0;
        size_t minSize = std::min(data.initialWeights.size(), data.finalWeights.size());
        for (size_t i = 0; i < minSize; ++i) {
            float delta = data.finalWeights[i] - data.initialWeights[i];
            if (delta > 0.01f) ++strengthened;
            else if (delta < -0.01f) ++weakened;
            else ++unchanged;
        }
        
        std::cout << "  Strengthened: " << strengthened << std::endl;
        std::cout << "  Weakened:    " << weakened << std::endl;
        std::cout << "  Unchanged:   " << unchanged << std::endl;
        
        std::cout << "\nSpike Activity:" << std::endl;
        std::cout << "  Total spikes: " << data.totalSpikes << std::endl;
        std::cout << "  Most active neurons recorded: " << data.mostActiveNeurons.size() << std::endl;
        
        // Determine if learning occurred
        bool learningOccurred = (std::abs(analysis.weightChange) > 0.001f) ||
                                (strengthened > 0 || weakened > 0);
        
        std::cout << "\nLearning Assessment:" << std::endl;
        if (learningOccurred) {
            std::cout << "  ✓ LEARNING DETECTED: Synaptic weights changed through experience" << std::endl;
        } else {
            std::cout << "  ✗ NO LEARNING: Weights did not change significantly" << std::endl;
        }
    }
};

class STDPVerification {
public:
    static bool run(std::shared_ptr<Brain> brain, int trials = 50,
                   float currentStrength = 60.0f, bool verbose = false) {
        
        LogUtils::logTestHeader("Test 3: STDP Verification");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            LogUtils::logError("No regions found for STDP test");
            return false;
        }
        
        auto& synapses = region->getSynapses();
        if (synapses.empty()) {
            LogUtils::logError("No synapses for STDP test");
            return false;
        }
        
        // Use first synapse for testing
        Synapse* syn = synapses[0].get();
        syn->enablePlasticity(false, true, false);  // STDP only
        float beforeWeight = syn->getWeight();
        
        LogUtils::logInfo("Testing STDP on 1 synapse:");
        LogUtils::logInfo("  Synapse before: " + std::to_string(beforeWeight) +
                        " (pre=" + std::to_string(syn->getSourceNeuron().index()) +
                        ", post=" + std::to_string(syn->getDestinationNeuron().index()) + ")");
        
        // Create correlated pre->post activity (potentiation)
        LogUtils::logInfo("\nCreating correlated pre->post activity (potentiation)...");
        
        for (int trial = 0; trial < trials; ++trial) {
            brain->injectCurrent(syn->getSourceNeuron(), currentStrength);
            brain->step(trial * 2, trial * 2 * 0.001);
            brain->injectCurrent(syn->getDestinationNeuron(), currentStrength);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
        
        float afterWeight = syn->getWeight();
        float delta = afterWeight - beforeWeight;
        
        LogUtils::logInfo("\nAfter correlated activity:");
        LogUtils::logInfo("  Synapse after: " + std::to_string(afterWeight) +
                        " (Δ=" + std::to_string(delta) + ")");
        
        if (std::abs(delta) > 0.001f) {
            if (delta > 0.0f) {
                LogUtils::logSuccess("STDP WORKING: Pre-before-post produced potentiation");
            } else {
                LogUtils::logWarning("STDP reversed: Check parameters");
            }
            return true;
        } else {
            LogUtils::logWarning("No change: STDP may not be triggering");
            return false;
        }
    }
};

class CustomTest {
public:
    static bool run(std::shared_ptr<Brain> brain, uint32_t steps = 100,
                   float currentStrength = 30.0f, size_t neuronsToStimulate = 5,
                   bool verbose = false) {
        
        LogUtils::logTestHeader("Custom Test");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            LogUtils::logError("No regions found in brain");
            return false;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            LogUtils::logError("No neurons found!");
            return false;
        }
        
        // Prepare stimulation pattern
        std::vector<NeuronId> stimulationNeurons;
        size_t neuronsToStim = std::min(neuronsToStimulate, neurons.size());
        for (size_t i = 0; i < neuronsToStim; ++i) {
            stimulationNeurons.push_back(neurons[i]->getId());
        }
        
        LogUtils::logInfo("Running custom test with " + std::to_string(steps) + 
                         " steps, stimulating " + std::to_string(neuronsToStim) + 
                         " neurons with current " + std::to_string(currentStrength));
        
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Run custom simulation
        for (SimulationStep step = 0; step < steps; ++step) {
            // Apply custom stimulation pattern
            for (NeuronId id : stimulationNeurons) {
                brain->injectCurrent(id, currentStrength);
            }
            
            brain->step(step, step * 0.001);
            
            if (verbose && step % 50 == 0) {
                LogUtils::logInfo("  Step " + std::to_string(step) + 
                                " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                                " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        size_t spikesGenerated = brain->getTotalSpikeCount() - initialSpikes;
        LogUtils::logInfo("Total spikes generated: " + std::to_string(spikesGenerated));
        
        return true;
    }
};

class CheckpointManager {
public:
    static bool saveBrain(const std::string& filepath, const std::shared_ptr<Brain>& brain) {
        LogUtils::logInfo("Saving brain state to " + filepath);
        
        try {
            if (brain->save(filepath)) {
                LogUtils::logSuccess("Brain state saved successfully");
                return true;
            } else {
                LogUtils::logError("Failed to save brain state");
                return false;
            }
        } catch (const std::exception& e) {
            LogUtils::logError(std::string("Exception saving brain: ") + e.what());
            return false;
        }
    }
    
    static bool loadBrain(const std::string& filepath, std::shared_ptr<Brain>& brain) {
        LogUtils::logInfo("Loading brain state from " + filepath);
        
        try {
            if (brain->load(filepath)) {
                LogUtils::logSuccess("Brain state loaded successfully");
                return true;
            } else {
                LogUtils::logError("Failed to load brain state");
                return false;
            }
        } catch (const std::exception& e) {
            LogUtils::logError(std::string("Exception loading brain: ") + e.what());
            return false;
        }
    }
};

class ConfigurationManager {
public:
    struct ValidationResult {
        bool isValid = true;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        void addError(const std::string& error) {
            errors.push_back(error);
            isValid = false;
        }
        
        void addWarning(const std::string& warning) {
            warnings.push_back(warning);
        }
        
        bool hasErrors() const { return !errors.empty(); }
        bool hasWarnings() const { return !warnings.empty(); }
        
        std::string getSummary() const {
            std::stringstream ss;
            if (isValid) {
                ss << "Configuration is valid";
                if (hasWarnings()) {
                    ss << ", with " << warnings.size() << " warning(s)";
                }
                return ss.str();
            } else {
                ss << "Configuration is invalid with " << errors.size() << " error(s):\n";
                for (const auto& error : errors) {
                    ss << "  - " << error << "\n";
                }
                return ss.str();
            }
        }
    };
    
    // Phase 6 configuration defaults
    struct Phase6Config {
        // Memory system configuration
        size_t workingMemoryCapacity = 50;  // neurons
        size_t maxEpisodicEpisodes = 1000;
        float episodicReplayInterval = 100.0f;  // steps
        float consolidationInterval = 1000.0f;   // steps
        
        // Prediction system
        bool predictionEnabled = true;
        float predictionAccuracy = 0.8f;
        
        // Cognitive systems
        size_t planningDepth = 5;
        float attentionInhibitionStrength = 0.5f;
        float attentionExcitationStrength = 1.5f;
        
        // Neuromodulation
        float dopamineBaseline = 0.1f;
        float curiosityLevel = 0.3f;
        float noveltyThreshold = 0.05f;
        
        // Development
        DevelopmentalStage developmentalStage = DevelopmentalStage::Initial;
        float developmentalPlasticityMod = 1.0f;
        
        // Replay and consolidation
        size_t replayEveryNSteps = 100;
        size_t consolidateEveryNSteps = 1000;
        float consolidationThreshold = 0.3f;
    };
    
    // Validate configuration against Phase 6 requirements
    static ValidationResult validateConfiguration(const std::shared_ptr<Config>& config) {
        ValidationResult result;
        
        // Basic validation
        validateBasicParameters(config, result);
        
        // Phase 6 specific validation
        validatePhase6Parameters(config, result);
        
        // Inter-parameter validation
        validateInterDependencies(config, result);
        
        return result;
    }
    
    // Get Phase 6 configuration
    static Phase6Config getPhase6Configuration(const std::shared_ptr<Config>& config) {
        Phase6Config phase6Config;
        
        // Memory system
        phase6Config.workingMemoryCapacity = static_cast<size_t>(
            config->getOr<int64_t>("working_memory_capacity", 50));
        phase6Config.maxEpisodicEpisodes = static_cast<size_t>(
            config->getOr<int64_t>("max_episodic_episodes", 1000));
        phase6Config.episodicReplayInterval = config->getOr<float>(
            "episodic_replay_interval", 100.0f);
        phase6Config.consolidationInterval = config->getOr<float>(
            "consolidation_interval", 1000.0f);
        
        // Prediction system
        phase6Config.predictionEnabled = config->getOr<bool>("prediction_enabled", true);
        phase6Config.predictionAccuracy = config->getOr<float>("prediction_accuracy", 0.8f);
        
        // Cognitive systems
        phase6Config.planningDepth = static_cast<size_t>(
            config->getOr<int64_t>("planning_depth", 5));
        phase6Config.attentionInhibitionStrength = config->getOr<float>(
            "attention_inhibition_strength", 0.5f);
        phase6Config.attentionExcitationStrength = config->getOr<float>(
            "attention_excitation_strength", 1.5f);
        
        // Neuromodulation
        phase6Config.dopamineBaseline = config->getOr<float>("dopamine_baseline", 0.1f);
        phase6Config.curiosityLevel = config->getOr<float>("curiosity_level", 0.3f);
        phase6Config.noveltyThreshold = config->getOr<float>("novelty_threshold", 0.05f);
        
        // Development
        std::string stageStr = config->getOr<std::string>("developmental_stage", "Initial");
        phase6Config.developmentalStage = stringToDevelopmentalStage(stageStr);
        phase6Config.developmentalPlasticityMod = config->getOr<float>(
            "developmental_plasticity_mod", 1.0f);
        
        // Replay and consolidation
        phase6Config.replayEveryNSteps = static_cast<size_t>(
            config->getOr<int64_t>("replay_every_n_steps", 100));
        phase6Config.consolidateEveryNSteps = static_cast<size_t>(
            config->getOr<int64_t>("consolidate_every_n_steps", 1000));
        phase6Config.consolidationThreshold = config->getOr<float>(
            "consolidation_threshold", 0.3f);
        
        return phase6Config;
    }
    
    // Apply Phase 6 configuration to brain
    static void applyPhase6Configuration(std::shared_ptr<Brain> brain, const Phase6Config& phase6Config) {
        // This would apply the configuration to the brain's subsystems
        // For now, we just log the configuration
        LogUtils::logInfo("Applying Phase 6 configuration:");
        LogUtils::logInfo("  Working memory capacity: " + std::to_string(phase6Config.workingMemoryCapacity));
        LogUtils::logInfo("  Max episodic episodes: " + std::to_string(phase6Config.maxEpisodicEpisodes));
        LogUtils::logInfo("  Development stage: " + developmentalStageToString(phase6Config.developmentalStage));
        LogUtils::logInfo("  Planning depth: " + std::to_string(phase6Config.planningDepth));
        LogUtils::logInfo("  Attention (inh/exc): " + std::to_string(phase6Config.attentionInhibitionStrength) + "/" + 
                         std::to_string(phase6Config.attentionExcitationStrength));
        LogUtils::logInfo("  Neuromodulation levels: Dopamine=" + std::to_string(phase6Config.dopamineBaseline) +
                         ", Curiosity=" + std::to_string(phase6Config.curiosityLevel) +
                         ", Novelty threshold=" + std::to_string(phase6Config.noveltyThreshold));
    }
    
private:
    static void validateBasicParameters(const std::shared_ptr<Config>& config, ValidationResult& result) {
        // Validate basic ranges
        auto validateInt = [&config, &result](const std::string& key, int64_t min, int64_t max, 
                                              const std::string& description) {
            if (auto valueOpt = config->get<int64_t>(key)) {
                int64_t value = *valueOpt;
                if (value < min || value > max) {
                    result.addError(description + " for " + key + " must be between " + 
                                   std::to_string(min) + " and " + std::to_string(max) + ", got " + 
                                   std::to_string(value));
                }
            }
        };
        
        auto validateFloat = [&config, &result](const std::string& key, float min, float max, 
                                               const std::string& description) {
            if (auto valueOpt = config->get<float>(key)) {
                float value = *valueOpt;
                if (value < min || value > max) {
                    result.addError(description + " for " + key + " must be between " + 
                                   std::to_string(min) + " and " + std::to_string(max) + ", got " + 
                                   std::to_string(value));
                }
            }
        };
        
        // Validate neuron count
        validateInt("neuron_count", 10, 10000, "Neuron count");
        
        // Validate region count
        validateInt("region_count", 1, 10, "Region count");
        
        // Validate timestep
        validateFloat("simulation_timestep", 0.0001, 0.1, "Simulation timestep");
        
        // Validate connection probability
        validateFloat("connection_probability", 0.0f, 1.0f, "Connection probability");
        
        // Validate plasticity parameters
        validateFloat("stdp_ltp_weight", 0.0f, 1.0f, "STDP LTP weight");
        validateFloat("stdp_ltd_weight", 0.0f, 1.0f, "STDP LTD weight");
        validateFloat("stdp_tau", 1.0f, 100.0f, "STDP time constant");
        
        // Validate plasticity rates
        validateFloat("synaptogenesis_rate", 0.0f, 0.1f, "Synaptogenesis rate");
        validateFloat("pruning_rate", 0.0f, 0.1f, "Pruning rate");
        
        // Validate neuromodulation levels
        validateFloat("dopamine_baseline", 0.0f, 1.0f, "Dopamine baseline");
    }
    
    static void validatePhase6Parameters(const std::shared_ptr<Config>& config, ValidationResult& result) {
        // Validate Phase 6 specific parameters
        
        // Memory system validation
        auto validateMemoryParam = [&config, &result](const std::string& key, size_t min, size_t max, 
                                                    const std::string& description) {
            if (auto valueOpt = config->get<int64_t>(key)) {
                size_t value = static_cast<size_t>(*valueOpt);
                if (value < min || value > max) {
                    result.addError(description + " must be between " + std::to_string(min) + 
                                   " and " + std::to_string(max) + ", got " + std::to_string(value));
                }
            }
        };
        
        validateMemoryParam("working_memory_capacity", 10, 1000, "Working memory capacity");
        validateMemoryParam("max_episodic_episodes", 10, 10000, "Max episodic episodes");
        validateMemoryParam("replay_every_n_steps", 10, 10000, "Replay interval");
        validateMemoryParam("consolidate_every_n_steps", 100, 100000, "Consolidation interval");
        
        // Validation for cognitive parameters
        validateFloat("planning_depth", 1.0f, 20.0f, "Planning depth");
        validateFloat("attention_inhibition_strength", 0.0f, 5.0f, "Attention inhibition strength");
        validateFloat("attention_excitation_strength", 0.0f, 5.0f, "Attention excitation strength");
        
        // Validation for neuromodulation
        validateFloat("curiosity_level", 0.0f, 1.0f, "Curiosity level");
        validateFloat("novelty_threshold", 0.0f, 1.0f, "Novelty threshold");
        
        // Validation for development
        std::string stageStr = config->getOr<std::string>("developmental_stage", "Initial");
        std::string validStages = "Initial,CriticalPeriod,Maturation,Adult";
        if (std::find(validStages.begin(), validStages.end(), stageStr) == std::string::npos) {
            result.addError("Invalid developmental_stage: " + stageStr + ". Valid options: " + validStages);
        }
        
        validateFloat("developmental_plasticity_mod", 0.0f, 2.0f, "Developmental plasticity modifier");
        
        // Prediction system validation
        validateFloat("prediction_accuracy", 0.0f, 1.0f, "Prediction accuracy");
    }
    
    static void validateInterDependencies(const std::shared_ptr<Config>& config, ValidationResult& result) {
        // Validate relationships between parameters
        
        // Ensure attention excitation is greater than inhibition for proper balance
        auto getFloat = [&config](const std::string& key, float defaultValue) {
            if (auto valueOpt = config->get<float>(key)) {
                return *valueOpt;
            }
            return defaultValue;
        };
        
        float inhStrength = getFloat("attention_inhibition_strength", 0.5f);
        float excStrength = getFloat("attention_excitation_strength", 1.5f);
        
        if (excStrength <= inhStrength) {
            result.addWarning("Attention excitation strength (" + std::to_string(excStrength) +
                            ") should be greater than inhibition strength (" + std::to_string(inhStrength) +
                            ") for proper competitive selection");
        }
        
        // Ensure plasticity rates are reasonable
        float synapRate = getFloat("synaptogenesis_rate", 0.0001f);
        float pruneRate = getFloat("pruning_rate", 0.00001f);
        
        if (synapRate < 0.000001f) {
            result.addWarning("Synaptogenesis rate is very low (" + std::to_string(synapRate) + ") - network may not grow enough");
        }
        
        if (pruneRate > synapRate * 10) {
            result.addWarning("Pruning rate is much higher than synaptogenesis rate - network may shrink too fast");
        }
        
        // Ensure neuromodulation levels are reasonable
        float dopamine = getFloat("dopamine_baseline", 0.1f);
        if (dopamine > 1.0f) {
            result.addWarning("Dopamine baseline is high (" + std::to_string(dopamine) + ") - may cause excessive excitation");
        }
    }
    
    static DevelopmentalStage stringToDevelopmentalStage(const std::string& stageStr) {
        static const std::unordered_map<std::string, DevelopmentalStage> stageMap = {
            {"Initial", DevelopmentalStage::Initial},
            {"CriticalPeriod", DevelopmentalStage::CriticalPeriod},
            {"Maturation", DevelopmentalStage::Maturation},
            {"Adult", DevelopmentalStage::Adult}
        };
        
        auto it = stageMap.find(stageStr);
        if (it != stageMap.end()) {
            return it->second;
        }
        
        // Default to Initial
        return DevelopmentalStage::Initial;
    }
    
    static std::string developmentalStageToString(DevelopmentalStage stage) {
        static const std::unordered_map<DevelopmentalStage, std::string> stageMap = {
            {DevelopmentalStage::Initial, "Initial"},
            {DevelopmentalStage::CriticalPeriod, "CriticalPeriod"},
            {DevelopmentalStage::Maturation, "Maturation"},
            {DevelopmentalStage::Adult, "Adult"}
        };
        
        auto it = stageMap.find(stage);
        if (it != stageMap.end()) {
            return it->second;
        }
        
        return "Initial";
    }
};

int main(int argc, char** argv) {
    try {
        // Parse command line arguments
        auto options = CommandLineParser::parse(argc, argv);
        
        // Handle help and version
        if (options.help) {
            CommandLineParser::printHelp();
            return 0;
        }
        
        if (options.version) {
            std::cout << "NLM Phase 2 Enhanced Version" << std::endl;
            return 0;
        }
        
        // Initialize logger with proper level
        auto logger = std::make_shared<Logger>();
        auto logLevel = options.quiet ? LogLevel::Error : 
                       options.verbose ? LogLevel::Debug : LogLevel::Info;
        auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        // Print banner
        std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation (Enhanced)               ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
        
        // Load configuration
        auto config = std::make_shared<Config>();
        
        std::string configFile = options.configFile;
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
        
        // Load config from file (ignore if not found)
        if (!options.quiet) {
            if (config->loadFromFile(configFile)) {
                LogUtils::logInfo("Loaded configuration from: " + configFile);
            } else {
                LogUtils::logInfo("Using default configuration.");
            }
        }
        
        // Override with command line args
        config->loadFromArgs(argc, argv);
        
        // Set default values for Phase 2
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        // STDP parameters
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
        
        // Structural plasticity parameters
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
        
        // Validate configuration if requested
        if (options.validateConfig) {
            LogUtils::logInfo("Configuration validation requested...");
            auto validation = ConfigurationManager::validateConfiguration(config);
            LogUtils::logInfo(validation.getSummary());
            if (validation.hasWarnings()) {
                LogUtils::logWarning("Configuration loaded with " + std::to_string(validation.warnings.size()) + " warning(s)");
            }
            if (validation.hasErrors()) {
                LogUtils::logError("Configuration validation failed with errors");
            } else {
                LogUtils::logSuccess("Configuration validation complete");
            }
        }
        
        // Apply Phase 6 configuration
        auto phase6Config = ConfigurationManager::getPhase6Configuration(config);
        ConfigurationManager::applyPhase6Configuration(brain, phase6Config);
        
        // Initialize simulation clock
        double timestep = config->getOr<double>("simulation_timestep", 0.001);
        SimulationClock clock(timestep);
        
        if (!options.quiet) {
            LogUtils::logInfo("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
        }
        
        // Initialize brain
        if (!options.quiet) {
            LogUtils::logInfo("Initializing NLM Brain...");
        }
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            LogUtils::logError("Failed to initialize brain!");
            return 1;
        }
        
        brain->logStatus();
        
        // Handle checkpoint loading
        if (options.loadCheckpoint) {
            if (!CheckpointManager::loadBrain(options.loadFile, brain)) {
                LogUtils::logError("Failed to load checkpoint, continuing with fresh brain");
            }
        }
        
        // Run tests based on mode
        bool success = true;
        
        if (options.mode == "full") {
            // Run all tests (original behavior)
            success = BasicConnectivityTest::run(brain);
            if (success) {
                brain->reset();
                brain->initialize();
                success = PlasticityExperiment::runFull(brain) && success;
                if (success) {
                    brain->reset();
                    brain->initialize();
                    success = STDPVerification::run(brain) && success;
                }
            }
        } else if (options.mode == "connectivity") {
            success = BasicConnectivityTest::run(brain, options.steps, 
                                                options.currentStrength, options.neuronsToStimulate,
                                                options.verbose);
        } else if (options.mode == "plasticity") {
            PlasticityExperiment::LearningData data = PlasticityExperiment::run(
                brain, options.steps, options.currentStrength, options.neuronsToStimulate, options.verbose);
            PlasticityExperiment::AnalysisResults analysis = PlasticityExperiment::analyze(data);
            PlasticityExperiment::printResults(data, analysis);
            success = analysis.learningDetected;
        } else if (options.mode == "stdp") {
            success = STDPVerification::run(brain, 50, options.currentStrength, options.verbose);
        } else if (options.mode == "custom") {
            success = CustomTest::run(brain, options.steps, options.currentStrength, 
                                    options.neuronsToStimulate, options.verbose);
        } else {
            LogUtils::logError("Unknown mode: " + options.mode);
            return 1;
        }
        
        // Save checkpoint if requested
        if (options.saveCheckpoint) {
            std::string checkpointFile = options.checkpointFile.empty() ? "brain_checkpoint.bin" : options.checkpointFile;
            CheckpointManager::saveBrain(checkpointFile, brain);
        }
        
        // Final brain status
        if (!options.quiet) {
            LogUtils::logTestHeader("Final Brain Status");
            brain->logStatus();
        }
        
        // Overall results
        std::cout << "\n" << std::string(60, '=') << std::endl;
        if (success) {
            std::cout << "✓ All tests completed successfully" << std::endl;
        } else {
            std::cout << "✗ Some tests failed" << std::endl;
        }
        std::cout << std::string(60, '=') << std::endl;
        
        return success ? 0 : 1;
        
    } catch (const std::exception& e) {
        std::cerr << "FATAL ERROR: " << e.what() << std::endl;
        return 1;
    }
}
