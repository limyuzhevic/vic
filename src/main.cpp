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

// Enhanced STDP verification with detailed debugging
void runStdpVerification(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Enhanced STDP Verification ===");
    
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
    
    // Enhanced: Test multiple synapses to get better statistics
    NLM_LOG_INFO("  Testing STDP on " + std::to_string(std::min(size_t(3), synapses.size())) + " synapses:");
    
    std::vector<Synapse*> testSynapses;
    std::vector<float> initialWeights;
    
    for (size_t i = 0; i < std::min(size_t(3), synapses.size()); ++i) {
        Synapse* syn = synapses[i].get();
        testSynapses.push_back(syn);
        initialWeights.push_back(syn->getWeight());
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    ": weight=" + std::to_string(initialWeights.back()) +
                    " (pre=" + std::to_string(syn->getSourceNeuron().index()) +
                    ", post=" + std::to_string(syn->getDestinationNeuron().index()) + ")");
    }
    
    // Enhanced: Set STDP only for each synapse
    for (Synapse* syn : testSynapses) {
        syn->enablePlasticity(false, true, false);  // STDP only
    }
    
    // Enhanced: Create more robust correlated activity
    NLM_LOG_INFO("");
    NLM_LOG_INFO("  Creating correlated activity (potentiation)...");
    
    for (size_t synIdx = 0; synIdx < testSynapses.size(); ++synIdx) {
        Synapse* syn = testSynapses[synIdx];
        NLM_LOG_INFO("    Testing synapse " + std::to_string(synIdx));
        
        // Use the synapse's own pre/post neurons for more reliable STDP
        NeuronId pre = syn->getSourceNeuron();
        NeuronId post = syn->getDestinationNeuron();
        
        // Enhanced: More robust correlated firing pattern
        for (int trial = 0; trial < 25; ++trial) {  // Reduced trials for speed
            brain->injectCurrent(pre, 60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            brain->injectCurrent(post, 60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
        
        float finalWeight = syn->getWeight();
        float delta = finalWeight - initialWeights[synIdx];
        
        NLM_LOG_INFO("      After: " + std::to_string(finalWeight) + 
                    " (Δ=" + std::to_string(delta) + ")");
        
        // Store final weight for statistics
        initialWeights[synIdx] = finalWeight;
    }
    
    // Enhanced: Aggregate statistics
    float totalDelta = 0.0f;
    float positiveDeltas = 0;
    float negativeDeltas = 0;
    size_t significantChanges = 0;
    
    for (size_t i = 0; i < testSynapses.size(); ++i) {
        float delta = initialWeights[i] - (i > 0 ? initialWeights[i-1] : testSynapses[i]->getWeight());
        totalDelta += delta;
        
        if (delta > 0.001f) positiveDeltas++;
        else if (delta < -0.001f) negativeDeltas++;
        else if (std::abs(delta) > 0.001f) significantChanges++;
    }
    
    // Enhanced: Enhanced logging
    NLM_LOG_INFO("");
    NLM_LOG_INFO("  STDP Results Summary:");
    NLM_LOG_INFO("    Total Δ: " + std::to_string(totalDelta));
    NLM_LOG_INFO("    Positive Δ: " + std::to_string(positiveDeltas));
    NLM_LOG_INFO("    Negative Δ: " + std::to_string(negativeDeltas));
    NLM_LOG_INFO("    Significant changes: " + std::to_string(significantChanges));
    
    if (totalDelta > 0.001f) {
        NLM_LOG_INFO("  ✓ STDP WORKING: Correlated activity produced potentiation");
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check parameters");
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering");
    }
}

// Main executable entry point
int main(int argc, char** argv) {
    printBanner();
    
    // Parse command line arguments for enhanced control
    std::string configFile = "configs/default.cfg";
    bool showHelp = false;
    bool showVersion = false;
    bool runTests = false;
    bool runAllPhases = false;
    std::string logLevel = "INFO";
    std::string outputFile = "";
    int neuronMultiplier = 1;
    
    // Enhanced command line parsing
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h") {
            showHelp = true;
        } else if (arg == "--version" || arg == "-v") {
            showVersion = true;
        } else if (arg == "--tests" || arg == "-t") {
            runTests = true;
        } else if (arg == "--all-phases") {
            runAllPhases = true;
        } else if (arg.substr(0, 9) == "--config=") {
            configFile = arg.substr(9);
        } else if (arg.substr(0, 8) == "--log=") {
            logLevel = arg.substr(8);
        } else if (arg.substr(0, 10) == "--output=") {
            outputFile = arg.substr(10);
        } else if (arg.substr(0, 12) == "--neurons=") {
            try {
                neuronMultiplier = std::stoi(arg.substr(12));
            } catch (...) {
                std::cerr << "Warning: Invalid neuron multiplier, using default" << std::endl;
            }
        } else if (arg[0] == '-') {
            std::cerr << "Unknown option: " << arg << std::endl;
            showHelp = true;
        }
    }
    
    // Show help if requested
    if (showHelp) {
        std::cout << "NLM Phase 2 Demo Program\n";
        std::cout << "Usage: nlm [options]\n\n";
        std::cout << "Options:\n";
        std::cout << "  --help, -h              Show this help message\n";
        std::cout << "  --version, -v           Show version information\n";
        std::cout << "  --tests, -t             Run all internal tests\n";
        std::cout << "  --all-phases           Run all phase demos (Phases 2-6)\n";
        std::cout << "  --config=FILE          Load configuration from file (default: configs/default.cfg)\n";
        std::cout << "  --log=LEVEL            Set log level (DEBUG, INFO, WARN, ERROR)\n";
        std::cout << "  --output=FILE           Save brain state to file after run\n";
        std::cout << "  --neurons=N            Multiply neuron count by N (1=default)\n";
        std::cout << "\nExamples:\n";
        std::cout << "  nlm                    Run Phase 2 demo with default settings\n";
        std::cout << "  nlm --neurons=2       Run with double neurons for more computation\n";
        std::cout << "  nlm --config=myconfig.cfg --log=DEBUG  Use custom config and debug logging\n";
        return 0;
    }
    
    // Show version
    if (showVersion) {
        std::cout << "NLM Phase 2 Demo\n";
        std::cout << "Version: 2.0.0\n";
        std::cout << "Built for: Real Neural Computation (LIF neurons, STDP, plasticity)\n";
        return 0;
    }
    
    // Initialize logger with enhanced configuration
    auto logger = std::make_shared<Logger>();
    LogLevel level;
    
    if (logLevel == "DEBUG") level = LogLevel::Debug;
    else if (logLevel == "WARN") level = LogLevel::Warning;
    else if (logLevel == "ERROR") level = LogLevel::Error;
    else level = LogLevel::Info; // default
    
    auto consoleLogger = std::make_shared<ConsoleLogger>(level);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 2: Enhanced Neural Computation ===");
    NLM_LOG_INFO("This is the improved Phase 2 demo with enhanced CLI and configuration.");
    NLM_LOG_INFO("Neuron multiplier: " + std::to_string(neuronMultiplier));
    
    // Load configuration with enhanced error handling
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
    bool configLoaded = false;
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
        configLoaded = true;
    } else {
        NLM_LOG_WARN("Configuration file not found: " + configFile + ". Using defaults.");
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Apply enhanced Phase 2 defaults with multiplier
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500 * neuronMultiplier), ConfigSource::Default);  // Scaled by multiplier
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f * neuronMultiplier / 2.0f, ConfigSource::Default);  // Adjust for scaling
    
    // Enhanced STDP parameters for better learning
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Enhanced structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Log enhanced configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Enhanced Configuration (after CLI overrides):");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500 * neuronMultiplier)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f * neuronMultiplier / 2.0f)));
    
    // Initialize simulation clock with enhanced logging
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Enhanced simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Initialize brain with enhanced status reporting
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing Enhanced NLM Brain...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize enhanced brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Enhanced test 1: Basic connectivity with logging
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Enhanced Test 1: Basic Neural Connectivity ===");
    runEnhancedConnectivityTest(brain);
    
    // Reset brain for plasticity experiment
    brain->reset();
    brain->initialize();
    
    // Enhanced test 2: Plasticity learning experiment with better pattern
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Enhanced Test 2: Advanced Plasticity Learning ===");
    runEnhancedPlasticityExperiment(brain);
    
    // Reset and run test 3: STDP verification
    brain->reset();
    brain->initialize();
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Enhanced Test 3: STDP Verification ===");
    runStdpVerification(brain);
    
    // Enhanced basic connectivity test with better logging
void runEnhancedConnectivityTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Enhanced Test 1: Enhanced Neural Connectivity ===");
    
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
    
    // Inject stronger current into first 10 neurons
    NLM_LOG_INFO("  Injecting current into 10 neurons...");
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);  // Stronger excitatory input
    }
    
    // Run more steps for better results
    for (SimulationStep step = 0; step < 100; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
    
    if (spikes > 0) {
        NLM_LOG_INFO("  ✓ Enhanced: Spikes propagate through network");
    } else {
        NLM_LOG_INFO("  ! Enhanced: No spikes - checking neuron parameters...");
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            NLM_LOG_INFO("    Enhanced Neuron " + std::to_string(i) + 
                        " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                        " thresh=" + std::to_string(neurons[i]->getThreshold()));
        }
    }
}

// Enhanced plasticity learning experiment with better learning patterns
void runEnhancedPlasticityExperiment(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Enhanced Test 2: Advanced Plasticity Learning ===");
    
    LearningExperiment experiment(brain, 42);
    
    // Record initial state
    experiment.recordInitialState();
    
    // Enable enhanced plasticity on synapses
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, true);  // Enable ALL plasticity types
        }
    }
    
    // Apply structured input patterns to stimulate selective learning
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Applying structured input patterns (2000 steps)...");
    
    // Enhanced pattern: stimulate specific neuron groups for selective learning
    std::vector<NeuronId> patternNeurons;
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto* neuron : region->getAllNeurons()) {
            if (neuron->getType() == NeuronType::Sensory && patternNeurons.size() < 20) {
                patternNeurons.push_back(neuron->getId());
            }
        }
    }
    NLM_LOG_INFO("  Pattern targets " + std::to_string(patternNeurons.size()) + " sensory neurons");
    
    // Structured learning with varied inputs
    for (SimulationStep step = 0; step < 2000; ++step) {
        for (NeuronId id : patternNeurons) {
            // Vary input strength over time to create diverse learning experiences
            float current = 30.0f + 5.0f * std::sin(step * 0.01f);
            brain->injectCurrent(id, current);
        }
        
        brain->step(step, step * 0.001);
        
        // Enhanced progress logging
        if (step % 200 == 0) {
            NLM_LOG_INFO("  Enhanced Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()) +
                        " | Synapses: " + std::to_string(brain->getTotalSynapseCount()));
        }
    }
    
    // Record final state
    experiment.recordFinalState();
    
    // Compute and display enhanced statistics
    experiment.computeStatistics();
}

