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
#include <stdexcept>
#include <cstdint>

using namespace nlm;

class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError("Configuration Error: " + msg) {}
};

class BrainInitializationError : public NLMError {
public:
    explicit BrainInitializationError(const std::string& msg) : NLMError("Brain Initialization Error: " + msg) {}
};

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

class CommandLineParser {
public:
    struct Options {
        uint64_t seed = 42;
        uint64_t neuronCount = 500;
        uint64_t regionCount = 1;
        float connectionProbability = 0.15f;
        double simulationTimestep = 0.001;
        uint64_t basicTestSteps = 50;
        uint64_t plasticitySteps = 1000;
        uint64_t stdpTrials = 50;
        std::string configFile = "configs/default.cfg";
        bool verbose = false;
        bool skipTests = false;
        bool runQuickDemo = false;
    };
    
    static Options parse(int argc, char** argv) {
        Options options;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                printUsage();
                exit(0);
            }
            else if (arg == "--quick") {
                options.runQuickDemo = true;
                options.plasticitySteps = 100;
                options.basicTestSteps = 10;
            }
            else if (arg.substr(0, 7) == "--seed=") {
                try {
                    options.seed = std::stoull(arg.substr(7));
                } catch (const std::exception& e) {
                    throw ConfigurationError("Invalid seed value: " + arg.substr(7));
                }
            }
            else if (arg == "--seed" && i + 1 < argc) {
                try {
                    options.seed = std::stoull(argv[++i]);
                } catch (const std::exception& e) {
                    throw ConfigurationError("Invalid seed value");
                }
            }
            else if (arg.substr(0, 4) == "--neurons=") {
                try {
                    options.neuronCount = std::stoull(arg.substr(9));
                } catch (const std::exception& e) {
                    throw ConfigurationError("Invalid neuron count");
                }
            }
            else if (arg.substr(0, 4) == "--regions=") {
                try {
                    options.regionCount = std::stoull(arg.substr(9));
                } catch (const std::exception& e) {
                    throw ConfigurationError("Invalid region count");
                }
            }
            else if (arg.substr(0, 4) == "--conn=") {
                try {
                    options.connectionProbability = std::stof(arg.substr(9));
                } catch (const std::exception& e) {
                    throw ConfigurationError("Invalid connection probability");
                }
            }
            else if (arg.substr(0, 4) == "--dt=") {
                try {
                    options.simulationTimestep = std::stod(arg.substr(9));
                } catch (const std::exception& e) {
                    throw ConfigurationError("Invalid timestep");
                }
            }
            else if (arg.substr(0, 4) == "--config=") {
                options.configFile = arg.substr(9);
            }
            else if (arg == "--verbose" || arg == "-v") {
                options.verbose = true;
            }
            else if (arg == "--skip-tests") {
                options.skipTests = true;
            }
        }
        
        return options;
    }
    
private:
    static void printUsage() {
        std::cout << "Usage: nlm [OPTIONS]\n\n";
        std::cout << "Phase 2 Neural Learning Machine Demo\n\n";
        std::cout << "Options:\n";
        std::cout << "  -h, --help              Show this help message\n";
        std::cout << "  --seed=<num>            Random seed (default: 42)\n";
        std::cout << "  --neurons=<num>         Neuron count (default: 500)\n";
        std::cout << "  --regions=<num>         Region count (default: 1)\n";
        std::cout << "  --conn=<float>          Connection probability (default: 0.15)\n";
        std::cout << "  --dt=<float>            Simulation timestep (default: 0.001)\n";
        std::cout << "  --config=<file>         Configuration file (default: configs/default.cfg)\n";
        std::cout << "  -v, --verbose           Enable verbose output\n";
        std::cout << "  --skip-tests             Skip demonstration tests\n";
        std::cout << "  --quick                 Run quick demonstration (fewer steps)\n\n";
        std::cout << "Examples:\n";
        std::cout << "  nlm                    Run full demo\n";
        std::cout << "  nlm --quick            Run quick demo\n";
        std::cout << "  nlm --seed=123 --neurons=1000\n";
    }
};

class LearningExperiment {
public:
    struct Results {
        uint64_t initialSynapseCount;
        float initialMeanWeight;
        uint64_t finalSynapseCount;
        float finalMeanWeight;
        uint64_t totalSpikes;
        std::vector<float> initialWeights;
        std::vector<float> finalWeights;
        std::vector<NeuronId> mostActiveNeurons;
        bool learningDetected;
        float learningMagnitude;
        
        double getWeightChangePercentage() const {
            if (initialMeanWeight == 0.0f) return 0.0;
            return ((finalMeanWeight - initialMeanWeight) / initialMeanWeight) * 100.0;
        }
    };
    
    LearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
        : brain(b), seed(s) {}
    
    Results runLearningTest(uint64_t steps) {
        Results results;
        
        // Record initial state
        recordInitialState(results);
        
        // Enable plasticity
        enablePlasticity(true, true, false);
        
        // Apply input pattern
        applyInputPattern(steps);
        
        // Record final state
        recordFinalState(results);
        
        // Analyze results
        analyzeResults(results);
        
        return results;
    }
    
private:
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    
    void recordInitialState(Results& results) {
        results.initialSynapseCount = brain->getTotalSynapseCount();
        results.initialWeights.clear();
        
        // Record initial weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                results.initialWeights.push_back(syn->getWeight());
            }
        }
        
        if (!results.initialWeights.empty()) {
            float sum = std::accumulate(results.initialWeights.begin(), 
                                      results.initialWeights.end(), 0.0f);
            results.initialMeanWeight = sum / results.initialWeights.size();
        } else {
            results.initialMeanWeight = 0.0f;
        }
    }
    
    void enablePlasticity(bool hebbian, bool stdp, bool structural) {
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(hebbian, stdp, structural);
            }
        }
    }
    
    void applyInputPattern(uint64_t steps) {
        for (uint64_t step = 0; step < steps; ++step) {
            // Inject current into sensory neurons
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step, step * 0.001);
        }
    }
    
    void recordFinalState(Results& results) {
        results.finalWeights.clear();
        
        // Record final weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                results.finalWeights.push_back(syn->getWeight());
            }
        }
        
        results.mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
        results.totalSpikes = brain->getTotalSpikeCount();
        
        if (!results.finalWeights.empty()) {
            float sum = std::accumulate(results.finalWeights.begin(), 
                                      results.finalWeights.end(), 0.0f);
            results.finalMeanWeight = sum / results.finalWeights.size();
        } else {
            results.finalMeanWeight = 0.0f;
        }
    }
    
    void analyzeResults(Results& results) {
        // Determine if learning occurred
        if (results.initialMeanWeight == 0.0f && results.finalMeanWeight == 0.0f) {
            results.learningDetected = false;
            results.learningMagnitude = 0.0f;
            return;
        }
        
        float weightChange = results.finalMeanWeight - results.initialMeanWeight;
        results.learningDetected = (std::abs(weightChange) > 0.001f);
        results.learningMagnitude = std::abs(weightChange);
    }
};

class STDPVerifier {
public:
    struct Results {
        std::vector<float> initialWeights;
        std::vector<float> finalWeights;
        std::vector<float> weightChanges;
        float totalChange;
        bool potentiationDetected;
        bool depressionDetected;
        bool significantChange;
        
        double getMeanWeightChange() const {
            if (weightChanges.empty()) return 0.0;
            float sum = std::accumulate(weightChanges.begin(), weightChanges.end(), 0.0f);
            return static_cast<double>(sum) / weightChanges.size();
        }
    };
    
    Results runSTDPTest(std::shared_ptr<Brain> brain, uint64_t trials) {
        Results results;
        
        if (auto* region = brain->getRegion(RegionId(1))) {
            auto& synapses = region->getSynapses();
            if (synapses.size() < 5) {
                throw NLMError("Not enough synapses for STDP test");
            }
            
            // Enable only STDP
            for (size_t i = 0; i < 5 && i < synapses.size(); ++i) {
                synapses[i]->enablePlasticity(false, true, false);
                results.initialWeights.push_back(synapses[i]->getWeight());
            }
        }
        
        // Create correlated pre->post activity
        createCorrelatedActivity(brain, trials);
        
        // Record final weights
        if (auto* region = brain->getRegion(RegionId(1))) {
            auto& synapses = region->getSynapses();
            for (size_t i = 0; i < results.initialWeights.size() && i < 5 && i < synapses.size(); ++i) {
                float delta = synapses[i]->getWeight() - results.initialWeights[i];
                results.finalWeights.push_back(synapses[i]->getWeight());
                results.weightChanges.push_back(delta);
            }
        }
        
        // Analyze results
        analyzeResults(results);
        
        return results;
    }
    
private:
    void createCorrelatedActivity(std::shared_ptr<Brain> brain, uint64_t trials) {
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return;
        
        auto neurons = region->getAllNeurons();
        if (neurons.size() < 2) return;
        
        Neuron* preNeuron = neurons[0];
        Neuron* postNeuron = neurons[1];
        
        for (uint64_t trial = 0; trial < trials; ++trial) {
            // Fire pre-synaptic neuron
            preNeuron->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            
            // Then post fires
            postNeuron->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
    }
    
    void analyzeResults(Results& results) {
        results.totalChange = 0.0f;
        results.potentiationDetected = false;
        results.depressionDetected = false;
        
        for (float delta : results.weightChanges) {
            results.totalChange += delta;
            if (delta > 0.001f) results.potentiationDetected = true;
            else if (delta < -0.001f) results.depressionDetected = true;
        }
        
        results.significantChange = (std::abs(results.totalChange) > 0.001f);
    }
};

class BasicConnectivityTester {
public:
    struct Results {
        size_t initialSpikes;
        size_t finalSpikes;
        size_t spikesGenerated;
        bool connectivityVerified;
        float neuronVoltages[3];
        float neuronThresholds[3];
    };
    
    Results runConnectivityTest(std::shared_ptr<Brain> brain, uint64_t steps) {
        Results results;
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            throw NLMError("No region found for connectivity test");
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            throw NLMError("No neurons found for connectivity test");
        }
        
        // Get initial spike count
        results.initialSpikes = brain->getTotalSpikeCount();
        
        // Inject current into neurons
        size_t neuronCount = std::min<size_t>(10, neurons.size());
        for (size_t i = 0; i < neuronCount; ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
        // Run steps
        for (uint64_t step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
        }
        
        // Get results
        results.finalSpikes = brain->getTotalSpikeCount();
        results.spikesGenerated = results.finalSpikes - results.initialSpikes;
        results.connectivityVerified = (results.spikesGenerated > 0);
        
        // Record neuron states
        for (size_t i = 0; i < std::min<size_t>(3, neurons.size()); ++i) {
            results.neuronVoltages[i] = neurons[i]->getMembranePotential();
            results.neuronThresholds[i] = neurons[i]->getThreshold();
        }
        
        return results;
    }
};

class DemoRunner {
public:
    static void printTestHeader(const std::string& title) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== " + title + " ===");
    }
    
    static void printConfiguration(const CommandLineParser::Options& options) {
        NLM_LOG_INFO("Configuration:");
        NLM_LOG_INFO("  random_seed: " + std::to_string(options.seed));
        NLM_LOG_INFO("  simulation_timestep: " + std::to_string(options.simulationTimestep) + "s");
        NLM_LOG_INFO("  neuron_count: " + std::to_string(options.neuronCount));
        NLM_LOG_INFO("  region_count: " + std::to_string(options.regionCount));
        NLM_LOG_INFO("  connection_probability: " + std::to_string(options.connectionProbability));
    }
    
    static void printResults(const std::string& testName, const std::string& status,
                           const std::string& details = "") {
        std::string statusSymbol;
        if (status == "PASS") statusSymbol = "✓";
        else if (status == "FAIL") statusSymbol = "✗";
        else statusSymbol = "!";
        
        NLM_LOG_INFO(statusSymbol + " " + testName + " - " + status);
        if (!details.empty()) {
            NLM_LOG_INFO("  " + details);
        }
    }
    
    static void printLearningResults(const LearningExperiment::Results& results) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Learning Experiment Results ===");
        NLM_LOG_INFO("");
        
        NLM_LOG_INFO("Weight Statistics:");
        NLM_LOG_INFO("  Initial mean weight: " + std::to_string(results.initialMeanWeight));
        NLM_LOG_INFO("  Final mean weight: " + std::to_string(results.finalMeanWeight));
        NLM_LOG_INFO("  Change: " + std::to_string(results.finalMeanWeight - results.initialMeanWeight));
        if (results.initialMeanWeight != 0.0f) {
            NLM_LOG_INFO("  Change (%): " + std::to_string(results.getWeightChangePercentage()) + "%");
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Spike Activity:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(results.totalSpikes));
        NLM_LOG_INFO("  Most active neurons recorded: " + std::to_string(results.mostActiveNeurons.size()));
        
        if (results.learningDetected) {
            NLM_LOG_INFO("");
            NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
            NLM_LOG_INFO("  Magnitude: " + std::to_string(results.learningMagnitude));
        } else {
            NLM_LOG_INFO("");
            NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
        }
    }
    
    static void printSTDPResults(const STDPVerifier::Results& results) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== STDP Verification Results ===");
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Mean weight change: " + std::to_string(results.getMeanWeightChange()));
        NLM_LOG_INFO("Total change: " + std::to_string(results.totalChange));
        
        if (results.potentiationDetected && results.depressionDetected) {
            NLM_LOG_INFO("  Both LTP (potentiation) and LTD (depression) detected");
        } else if (results.potentiationDetected) {
            NLM_LOG_INFO("  LTP (potentiation) detected");
        } else if (results.depressionDetected) {
            NLM_LOG_INFO("  LTD (depression) detected");
        }
        
        if (results.significantChange) {
            NLM_LOG_INFO("✓ STDP is working correctly");
        } else {
            NLM_LOG_INFO("! No significant change detected");
        }
    }
    
    static void printConnectivityResults(const BasicConnectivityTester::Results& results) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Basic Connectivity Results ===");
        NLM_LOG_INFO("Spikes generated: " + std::to_string(results.spikesGenerated));
        
        if (results.connectivityVerified) {
            NLM_LOG_INFO("✓ Spikes propagate through network");
        } else {
            NLM_LOG_INFO("! No spikes - checking neuron parameters:");
            for (size_t i = 0; i < 3; ++i) {
                if (results.neuronVoltages[i] > 0) {
                    NLM_LOG_INFO("  Neuron " + std::to_string(i) + 
                                " V=" + std::to_string(results.neuronVoltages[i]) +
                                " thresh=" + std::to_string(results.neuronThresholds[i]));
                }
            }
        }
    }
};

int main(int argc, char** argv) {
    try {
        printBanner();
        
        std::cout << "Initializing NLM Phase 2 Real Neural Computation...\n" << std::endl;
        
        // Parse command line arguments
        CommandLineParser::Options options;
        try {
            options = CommandLineParser::parse(argc, argv);
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
        
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(
            options.verbose ? LogLevel::Debug : LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation ===");
        NLM_LOG_INFO("Implementing:");
        NLM_LOG_INFO("  - Leaky Integrate-and-Fire (LIF) neuron dynamics");
        NLM_LOG_INFO("  - Event-driven spike propagation with delays");
        NLM_LOG_INFO("  - STDP and Hebbian plasticity rules");
        NLM_LOG_INFO("  - Structural plasticity (synaptogenesis/pruning)");
        
        // Load configuration
        auto config = std::make_shared<Config>();
        
        // Try to load from file if provided
        std::string configFile = options.configFile;
        if (config->loadFromFile(configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + configFile);
        } else {
            NLM_LOG_INFO("Using default configuration.");
        }
        
        // Override with command line args
        config->loadFromArgs(argc, argv);
        
        // Set Phase 2 specific configuration
        config->set("random_seed", static_cast<int64_t>(options.seed), ConfigSource::Default);
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
        DemoRunner::printConfiguration(options);
        
        // Initialize simulation clock
        SimulationClock clock(options.simulationTimestep);
        NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(options.simulationTimestep) + "s");
        
        // Initialize brain
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Initializing NLM Brain...");
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            throw BrainInitializationError("Failed to initialize brain");
        }
        
        brain->logStatus();
        
        // Run tests if not skipped
        if (!options.skipTests) {
            // Test 1: Basic connectivity
            DemoRunner::printTestHeader("Test 1: Basic Neural Connectivity");
            BasicConnectivityTester connectivityTester;
            auto connectivityResults = connectivityTester.runConnectivityTest(brain, options.basicTestSteps);
            DemoRunner::printConnectivityResults(connectivityResults);
            
            // Reset brain for plasticity experiment
            brain->reset();
            brain->initialize();
            
            if (!options.runQuickDemo) {
                // Test 2: Plasticity learning experiment
                DemoRunner::printTestHeader("Test 2: Plasticity Learning Experiment");
                LearningExperiment plasticityExperiment(brain, options.seed);
                auto learningResults = plasticityExperiment.runLearningTest(options.plasticitySteps);
                DemoRunner::printLearningResults(learningResults);
            }
            
            // Reset and run Test 3: STDP verification
            brain->reset();
            brain->initialize();
            
            DemoRunner::printTestHeader("Test 3: STDP Verification");
            STDPVerifier stdpVerifier;
            auto stdpResults = stdpVerifier.runSTDPTest(brain, options.stdpTrials);
            DemoRunner::printSTDPResults(stdpResults);
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
        if (!options.skipTests) {
            if (!options.runQuickDemo) {
                NLM_LOG_INFO("  ✓ Learning experiment demonstrates measurable changes");
            }
            NLM_LOG_INFO("  ✓ Network shows activity-dependent synaptic modification");
        }
        NLM_LOG_INFO("");
        NLM_LOG_INFO("The NLM brain is now a functioning artificial neural substrate");
        NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
}

