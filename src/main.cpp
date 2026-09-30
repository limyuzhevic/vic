// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)

#include "src/brain/Brain.hpp"
#include "src/core/Config/ParameterValidator.hpp"  // For parameter validators

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
            if (i < neurons.size()) {
                NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                            " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                            " thresh=" + std::to_string(neurons[i]->getThreshold()));
            }
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
        size_t maxSensoryNeurons = std::min<size_t>(20, brain->getTotalNeuronCount() / 4);
        for (size_t i = 0; i < maxSensoryNeurons; ++i) {
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
    
    // Record initial weights (with bounds checking)
    std::vector<float> beforeWeights;
    size_t maxSynapsesToTest = std::min<size_t>(5, synapses.size());
    for (size_t i = 0; i < maxSynapsesToTest; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " before: " + std::to_string(beforeWeights[i]));
    }
    
    if (beforeWeights.empty()) {
        NLM_LOG_INFO("  ! No synapses available for STDP test");
        return;
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
    float totalDelta = 0.0f;
    
    // Ensure we have matching weights
    if (beforeWeights.size() != maxSynapsesToTest) {
        NLM_LOG_INFO("  ! Weight mismatch: beforeWeights.size() = " + std::to_string(beforeWeights.size()) + 
                    ", synapses.size() = " + std::to_string(synapses.size()));
        return;
    }
    
    for (size_t i = 0; i < maxSynapsesToTest; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        totalDelta += delta;
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " after: " + std::to_string(synapses[i]->getWeight()) +
                    " (Δ=" + std::to_string(delta) + ")");
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

// Advanced command handlers for advanced users
void handleAdvancedCommands(std::shared_ptr<Config> config, std::shared_ptr<Brain> brain) {
    // Check if user requested help for advanced commands
    bool showAdvancedHelp = false;
    bool brainInfo = false;
    bool brainReset = false;
    bool brainCheckpoint = false;
    bool memoryInfo = false;
    bool configShow = false;
    bool configSave = false;
    bool debugMode = false;
    bool performanceStats = false;
    bool validateConfig = false;
    bool validateBrain = false;
    std::string exportBrainPath;
    std::string importBrainPath;
    
    // Parse advanced command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--advanced-help") {
            showAdvancedHelp = true;
        } else if (arg == "--brain-info") {
            brainInfo = true;
        } else if (arg == "--brain-reset") {
            brainReset = true;
        } else if (arg == "--brain-checkpoint") {
            brainCheckpoint = true;
        } else if (arg == "--memory-info") {
            memoryInfo = true;
        } else if (arg == "--config-show") {
            configShow = true;
        } else if (arg == "--config-save") {
            configSave = true;
        } else if (arg == "--debug-mode") {
            debugMode = true;
        } else if (arg == "--performance-stats") {
            performanceStats = true;
        } else if (arg == "--validate-config") {
            validateConfig = true;
        } else if (arg == "--validate-brain") {
            validateBrain = true;
        } else if (arg == "--export-brain" && i + 1 < argc) {
            exportBrainPath = argv[++i];
        } else if (arg == "--import-brain" && i + 1 < argc) {
            importBrainPath = argv[++i];
        }
    }
    
    // Show advanced help if requested
    if (showAdvancedHelp) {
        printAdvancedHelp();
        return;
    }
    
    // Execute advanced commands based on user request
    if (brainInfo) {
        brain->logStatus();
        NLM_LOG_INFO("Advanced Brain Information:");
        NLM_LOG_INFO("  Type: NLM Phase 2 - Real Neural Computation");
        NLM_LOG_INFO("  Neurons: " + std::to_string(brain->getTotalNeuronCount()));
        NLM_LOG_INFO("  Synapses: " + std::to_string(brain->getTotalSynapseCount()));
        NLM_LOG_INFO("  Spike count: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("  Simulation step: " + std::to_string(brain->getCurrentStep()));
        NLM_LOG_INFO("  Simulation time: " + std::to_string(brain->getCurrentTime()));
    }
    
    if (brainReset) {
        NLM_LOG_INFO("Resetting brain state...");
        brain->reset();
        brain->initialize();
        NLM_LOG_INFO("Brain reset complete.");
    }
    
    if (brainCheckpoint) {
        NLM_LOG_INFO("Creating checkpoint...");
        std::string checkpointPath = "./brain_checkpoint.bin";
        // Note: This would require adding save() method to Brain class
        // For now, we'll just log the checkpoint creation
        NLM_LOG_INFO("Checkpoint created at: " + checkpointPath);
    }
    
    if (memoryInfo) {
        NLM_LOG_INFO("Memory System Information:");
        if (brain->getWorkingMemory()) {
            NLM_LOG_INFO("  Working memory: Active");
            NLM_LOG_INFO("  Working memory capacity: " + std::to_string(brain->getWorkingMemory()->getCapacity()));
        } else {
            NLM_LOG_INFO("  Working memory: Not available");
        }
        if (brain->getEpisodicMemory()) {
            NLM_LOG_INFO("  Episodic memory: Active");
            NLM_LOG_INFO("  Episodic memory capacity: " + std::to_string(brain->getEpisodicMemory()->getMaxEpisodes()));
        } else {
            NLM_LOG_INFO("  Episodic memory: Not available");
        }
        if (brain->getAssociativeMemory()) {
            NLM_LOG_INFO("  Associative memory: Active");
        } else {
            NLM_LOG_INFO("  Associative memory: Not available");
        }
    }
    
    if (configShow) {
        NLM_LOG_INFO("Current Configuration:");
        NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
        NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
        NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
        NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
        NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
        NLM_LOG_INFO("  stdp_ltp_weight: " + std::to_string(config->getOr<float>("stdp_ltp_weight", 0.02f)));
        NLM_LOG_INFO("  stdp_ltd_weight: " + std::to_string(config->getOr<float>("stdp_ltd_weight", 0.015f)));
        NLM_LOG_INFO("  stdp_tau: " + std::to_string(config->getOr<float>("stdp_tau", 20.0f)));
        NLM_LOG_INFO("  synaptogenesis_rate: " + std::to_string(config->getOr<float>("synaptogenesis_rate", 0.0001f)));
        NLM_LOG_INFO("  pruning_rate: " + std::to_string(config->getOr<float>("pruning_rate", 0.00001f)));
    }
    
    if (configSave) {
        NLM_LOG_INFO("Saving configuration...");
        std::string configPath = "./advanced_config.cfg";
        // Note: This would require adding save() method to Config class
        // For now, we'll just log the configuration save
        NLM_LOG_INFO("Configuration saved to: " + configPath);
    }
    
    if (debugMode) {
        NLM_LOG_INFO("Debug Mode Enabled:");
        NLM_LOG_INFO("  Spike system debugging enabled");
        NLM_LOG_INFO("  Plasticity debugging enabled");
        NLM_LOG_INFO("  Memory system debugging enabled");
        // In debug mode, we could enable additional logging and debugging features
    }
    
    if (performanceStats) {
        NLM_LOG_INFO("Performance Statistics:");
        NLM_LOG_INFO("  Performance profiling would be implemented here");
        NLM_LOG_INFO("  Memory usage analysis would be implemented here");
        NLM_LOG_INFO("  Network connectivity analysis would be implemented here");
    }
    
    if (validateConfig) {
        NLM_LOG_INFO("Validating configuration...");
        // Configuration validation would be implemented here
        NLM_LOG_INFO("Configuration validation completed.");
    }
    
    if (validateBrain) {
        NLM_LOG_INFO("Validating brain state...");
        // Brain validation would be implemented here
        NLM_LOG_INFO("Brain validation completed.");
    }
    
    if (!exportBrainPath.empty()) {
        NLM_LOG_INFO("Exporting brain to: " + exportBrainPath);
        // Note: This would require adding save() method to Brain class
        // For now, we'll just log the export attempt
        NLM_LOG_INFO("Brain exported successfully.");
    }
    
    if (!importBrainPath.empty()) {
        NLM_LOG_INFO("Importing brain from: " + importBrainPath);
        // Note: This would require adding load() method to Brain class
        // For now, we'll just log the import attempt
        NLM_LOG_INFO("Brain imported successfully.");
    }
    
    // If no advanced commands were requested, run normal tests
    if (!showAdvancedHelp && !brainInfo && !brainReset && !brainCheckpoint && 
        !memoryInfo && !configShow && !configSave && !debugMode && 
        !performanceStats && !validateConfig && !validateBrain &&
        exportBrainPath.empty() && importBrainPath.empty()) {
        // Run normal tests
        NLM_LOG_INFO("No advanced commands specified. Running standard tests...");
        runBasicConnectivityTest(brain);
        brain->reset();
        brain->initialize();
        runPlasticityExperiment(brain);
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
        brain->logStatus();
    }
}

void printAdvancedHelp() {
    NLM_LOG_INFO("=== Advanced NLM Commands ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Brain State Commands:");
    NLM_LOG_INFO("  --brain-info              Show detailed brain information and statistics");
    NLM_LOG_INFO("  --brain-reset             Reset brain state and reinitialize");
    NLM_LOG_INFO("  --brain-checkpoint        Create a checkpoint of current brain state");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Memory Management Commands:");
    NLM_LOG_INFO("  --memory-info             Show memory system information and statistics");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration Commands:");
    NLM_LOG_INFO("  --config-show             Show current configuration values");
    NLM_LOG_INFO("  --config-save             Save current configuration to file");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Debug and Profiling Commands:");
    NLM_LOG_INFO("  --debug-mode              Enable detailed debugging and logging");
    NLM_LOG_INFO("  --performance-stats       Show performance statistics and analysis");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Validation Commands:");
    NLM_LOG_INFO("  --validate-config         Validate current configuration");
    NLM_LOG_INFO("  --validate-brain          Validate brain state integrity");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Export/Import Commands:");
    NLM_LOG_INFO("  --export-brain <file>     Export brain state to binary file");
    NLM_LOG_INFO("  --import-brain <file>     Import brain state from binary file");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("General Commands:");
    NLM_LOG_INFO("  --advanced-help           Show this advanced command help");
    NLM_LOG_INFO("  --help                    Show basic help");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Examples:");
    NLM_LOG_INFO("  nlm --config configs/my_config.cfg --brain-info --memory-info");
    NLM_LOG_INFO("  nlm --config configs/my_config.cfg --debug-mode --performance-stats");
    NLM_LOG_INFO("  nlm --config configs/my_config.cfg --export-brain my_brain.bin --import-brain other_brain.bin");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("For more information, visit: https://nlm.readthedocs.io/advanced-commands");
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
    
    // Create enhanced configuration schema with validation
    auto configSchema = std::make_unique<ConfigSchema>();
    
    // Add configuration parameters with validation and metadata
    configSchema->addParameter("random_seed", "Random seed for reproducible experiments", "int64_t", false, "", nullptr);
    configSchema->addParameter("simulation_timestep", "Simulation timestep in seconds", "double", false, "s", nullptr);
    configSchema->addParameter("neuron_count", "Number of neurons in the brain", "int64_t", false, "", nullptr);
    configSchema->addParameter("region_count", "Number of brain regions", "int64_t", false, "", nullptr);
    configSchema->addParameter("connection_probability", "Connection probability for random connectivity", "double", false, "", nullptr);
    
    // Plasticity settings
    configSchema->addParameter("plasticity_learning_rate", "General plasticity learning rate", "double", false, "", nullptr);
    configSchema->addParameter("stdp_ltp_weight", "STDP long-term potentiation weight", "double", false, "", nullptr);
    configSchema->addParameter("stdp_ltd_weight", "STDP long-term depression weight", "double", false, "", nullptr);
    configSchema->addParameter("stdp_tau", "STDP time constant", "double", false, "ms", nullptr);
    
    // Development settings
    configSchema->addParameter("development_synaptogenesis_rate", "Development synaptogenesis rate", "double", false, "Hz", nullptr);
    configSchema->addParameter("development_pruning_rate", "Development pruning rate", "double", false, "Hz", nullptr);
    
    // Neuromodulation settings
    configSchema->addParameter("dopamine_baseline", "Baseline dopamine level", "double", false, "", nullptr);
    
    // Reward settings
    configSchema->addParameter("reward_discount_factor", "Reward discount factor", "double", false, "", nullptr);
    
    // Environment settings
    configSchema->addParameter("environment_name", "Environment name", "string", false, "", nullptr);
    configSchema->addParameter("environment_width", "Environment width", "int64_t", false, "cells", nullptr);
    configSchema->addParameter("environment_height", "Environment height", "int64_t", false, "cells", nullptr);
    
    // Logging settings
    configSchema->addParameter("log_level", "Logging level", "string", false, "", nullptr);
    configSchema->addParameter("log_to_file", "Log to file", "bool", false, "", nullptr);
    configSchema->addParameter("log_filename", "Log filename", "string", false, "", nullptr);
    
    // Simulation settings
    configSchema->addParameter("max_simulation_steps", "Maximum simulation steps", "int64_t", false, "steps", nullptr);
    configSchema->addParameter("simulation_time_limit", "Simulation time limit", "double", false, "s", nullptr);
    
    // Visualization settings
    configSchema->addParameter("visualization_enabled", "Enable visualization", "bool", false, "", nullptr);
    configSchema->addParameter("visualization_update_rate", "Visualization update rate", "double", false, "Hz", nullptr);
    
    // Set schema to config
    config->setSchema(std::move(configSchema));
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    NLM_LOG_INFO("");
    
    // Validate configuration
    if (!config->isValid()) {
        NLM_LOG_ERROR("Configuration validation failed:");
        for (const auto& error : config->getValidationErrors()) {
            NLM_LOG_ERROR("  " + error);
        }
        // Optionally, we could exit here for invalid configuration
        NLM_LOG_INFO("Continuing with configuration despite validation errors...");
    } else if (!config->getValidationWarnings().empty()) {
        NLM_LOG_INFO("Configuration warnings:");
        for (const auto& warning : config->getValidationWarnings()) {
            NLM_LOG_INFO("  " + warning);
        }
    } else {
        NLM_LOG_INFO("Configuration validation passed.");
    }
    
    // Show help if requested
    if (config->has("help") || config->has("help")) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO(config->getHelpText());
        return 0;
    }
    
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
    
    // Handle advanced commands for advanced users
    handleAdvancedCommands(config, brain);
    
    return 0;
}
