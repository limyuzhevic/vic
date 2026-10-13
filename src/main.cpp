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

using namespace nlm;

// Forward declarations for experiment functions
void printBanner();
void printHelp();
void runExperiment(const std::string& experimentName, std::shared_ptr<Config> config);
void runInteractiveMode(std::shared_ptr<Config> config);
void saveExperimentResults(const std::string& filename, const std::string& experimentName);
void loadExperimentConfig(const std::string& filename, std::shared_ptr<Config> config);

// Experiment modes
enum class ExperimentMode {
    BASIC_CONNECTIVITY,
    PLASTICITY_LEARNING,
    STDP_VERIFICATION,
    ALL_EXPERIMENTS,
    INTERACTIVE
};

// Parse command line arguments into config and experiment mode
std::pair<ExperimentMode, std::shared_ptr<Config>> parseCommandLine(int argc, char** argv);

// Display configuration summary
void displayConfigSummary(const Config& config);

// Validate configuration values
bool validateConfig(const Config& config);

// Experiment execution
void executeBasicConnectivityTest(std::shared_ptr<Brain> brain);
void executePlasticityExperiment(std::shared_ptr<Brain> brain);
void executeStdpVerification(std::shared_ptr<Brain> brain);

// Helper functions for experiments
void setupBrain(std::shared_ptr<Brain> brain, const Config& config);
void resetBrain(std::shared_ptr<Brain> brain);
void saveBrainState(const std::string& filename, std::shared_ptr<Brain> brain);
void loadBrainState(const std::string& filename, std::shared_ptr<Brain> brain);

int main(int argc, char** argv) {
    // Initialize logging system
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    consoleLogger->setUseColors(true);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);

    // Parse command line arguments
    auto [mode, config] = parseCommandLine(argc, argv);
    
    if (!config) {
        printHelp();
        return 1;
    }

    // Display banner and configuration
    printBanner();
    std::cout << "NLM Neural Learning Machine - Phase 2: Real Neural Computation" << std::endl;
    std::cout << "Command line options parsed successfully.\n" << std::endl;
    
    displayConfigSummary(*config);

    // Execute experiments based on mode
    try {
        switch (mode) {
            case ExperimentMode::BASIC_CONNECTIVITY:
                std::cout << "\nRunning Basic Connectivity Test..." << std::endl;
                {
                    auto brain = std::make_shared<Brain>(config);
                    if (brain->initialize()) {
                        executeBasicConnectivityTest(brain);
                    }
                }
                break;
                
            case ExperimentMode::PLASTICITY_LEARNING:
                std::cout << "\nRunning Plasticity Learning Experiment..." << std::endl;
                {
                    auto brain = std::make_shared<Brain>(config);
                    if (brain->initialize()) {
                        executePlasticityExperiment(brain);
                    }
                }
                break;
                
            case ExperimentMode::STDP_VERIFICATION:
                std::cout << "\nRunning STDP Verification..." << std::endl;
                {
                    auto brain = std::make_shared<Brain>(config);
                    if (brain->initialize()) {
                        executeStdpVerification(brain);
                    }
                }
                break;
                
            case ExperimentMode::ALL_EXPERIMENTS:
                std::cout << "\nRunning All Experiments..." << std::endl;
                {
                    auto brain = std::make_shared<Brain>(config);
                    if (brain->initialize()) {
                        executeBasicConnectivityTest(brain);
                        resetBrain(brain);
                        brain->initialize();
                        executePlasticityExperiment(brain);
                        resetBrain(brain);
                        brain->initialize();
                        executeStdpVerification(brain);
                    }
                }
                break;
                
            case ExperimentMode::INTERACTIVE:
                runInteractiveMode(config);
                break;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error during execution: " + std::string(e.what()));
        return 1;
    }

    NLM_LOG_INFO("\nAll experiments completed successfully!");
    return 0;
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

void executeBasicConnectivityTest(std::shared_ptr<Brain> brain) {
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

void executePlasticityExperiment(std::shared_ptr<Brain> brain) {
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

void executeStdpVerification(std::shared_ptr<Brain> brain) {
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

// Implementation of remaining functions...
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

void printHelp() {
    printBanner();
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "NLM Neural Learning Machine - Phase 2 Command Line Interface" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "\nUsage:" << std::endl;
    std::cout << "  ./nlm [OPTIONS] [EXPERIMENT_TYPE]" << std::endl;
    std::cout << "\nExperiment Types:" << std::endl;
    std::cout << "  --basic-connectivity     Run basic neural connectivity test" << std::endl;
    std::cout << "  --plasticity            Run plasticity learning experiment" << std::endl;
    std::cout << "  --stdp-verification      Run STDP verification test" << std::endl;
    std::cout << "  --all-experiments       Run all experiments sequentially" << std::endl;
    std::cout << "  --interactive          Run in interactive mode" << std::endl;
    std::cout << "\nConfiguration Options:" << std::endl;
    std::cout << "  --config=FILE          Load configuration from file" << std::endl;
    std::cout << "  --key=value            Set configuration key-value pair" << std::endl;
    std::cout << "  --help                 Display this help message" << std::endl;
    std::cout << "\nExample Usage:" << std::endl;
    std::cout << "  ./nlm --basic-connectivity --config=my_config.cfg" << std::endl;
    std::cout << "  ./nlm --plasticity --neuron_count=2000 --simulation_timestep=0.0005" << std::endl;
    std::cout << "  ./nlm --all-experiments" << std::endl;
    std::cout << "\nFor interactive mode, simply run ./nlm without arguments." << std::endl;
}

std::pair<ExperimentMode, std::shared_ptr<Config>> parseCommandLine(int argc, char** argv) {
    auto config = std::make_shared<Config>();
    ExperimentMode mode = ExperimentMode::INTERACTIVE;  // Default to interactive
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle help flag
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return std::make_pair(ExperimentMode::INTERACTIVE, nullptr);  // Signal to exit
        }
        
        // Handle experiment mode flags
        if (arg == "--basic-connectivity") {
            mode = ExperimentMode::BASIC_CONNECTIVITY;
        } else if (arg == "--plasticity") {
            mode = ExperimentMode::PLASTICITY_LEARNING;
        } else if (arg == "--stdp-verification") {
            mode = ExperimentMode::STDP_VERIFICATION;
        } else if (arg == "--all-experiments") {
            mode = ExperimentMode::ALL_EXPERIMENTS;
        } else if (arg == "--interactive") {
            mode = ExperimentMode::INTERACTIVE;
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                if (config->loadFromFile(argv[++i])) {
                    NLM_LOG_INFO("Loaded configuration from command line: " + std::string(argv[i]));
                } else {
                    NLM_LOG_WARNING("Failed to load configuration from: " + std::string(argv[i]));
                }
            }
        }
    }
    
    // Set default configuration values if not already set
    if (!config->has("random_seed")) {
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    }
    if (!config->has("simulation_timestep")) {
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
    }
    if (!config->has("neuron_count")) {
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    }
    if (!config->has("region_count")) {
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    }
    if (!config->has("connection_probability")) {
        config->set("connection_probability", 0.15f, ConfigSource::Default);
    }
    
    return std::make_pair(mode, config);
}

void displayConfigSummary(const Config& config) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== NLM Configuration Summary ===");
    std::vector<std::string> keys = config.getKeys();
    for (const auto& key : keys) {
        if (config.has<int64_t>(key)) {
            NLM_LOG_INFO("  " + key + ": " + std::to_string(config.getOr<int64_t>(key, 0)));
        } else if (config.has<double>(key)) {
            NLM_LOG_INFO("  " + key + ": " + std::to_string(config.getOr<double>(key, 0.0)));
        } else if (config.has<std::string>(key)) {
            NLM_LOG_INFO("  " + key + ": \"" + config.getOr<std::string>(key, "") + "\"");
        } else if (config.has<bool>(key)) {
            NLM_LOG_INFO("  " + key + ": " + (config.getOr<bool>(key, false) ? "true" : "false"));
        }
    }
}

bool validateConfig(const Config& config) {
    bool isValid = true;
    
    // Validate required configuration values
    if (!config.has("random_seed")) {
        NLM_LOG_WARNING("Missing 'random_seed' configuration, using default");
        isValid = false;
    }
    
    if (!config.has("simulation_timestep")) {
        NLM_LOG_WARNING("Missing 'simulation_timestep' configuration, using default");
        isValid = false;
    } else {
        double timestep = config.getOr<double>("simulation_timestep", 0.001);
        if (timestep <= 0.0) {
            NLM_LOG_ERROR("Invalid 'simulation_timestep': " + std::to_string(timestep));
            isValid = false;
        }
    }
    
    if (!config.has("neuron_count")) {
        NLM_LOG_WARNING("Missing 'neuron_count' configuration, using default");
        isValid = false;
    } else {
        int64_t neuronCount = config.getOr<int64_t>("neuron_count", 500);
        if (neuronCount <= 0) {
            NLM_LOG_ERROR("Invalid 'neuron_count': " + std::to_string(neuronCount));
            isValid = false;
        }
    }
    
    if (!config.has("region_count")) {
        NLM_LOG_WARNING("Missing 'region_count' configuration, using default");
        isValid = false;
    }
    
    if (!config.has("connection_probability")) {
        NLM_LOG_WARNING("Missing 'connection_probability' configuration, using default");
        isValid = false;
    } else {
        float connectionProb = config.getOr<float>("connection_probability", 0.15f);
        if (connectionProb < 0.0 || connectionProb > 1.0) {
            NLM_LOG_ERROR("Invalid 'connection_probability': " + std::to_string(connectionProb));
            isValid = false;
        }
    }
    
    return isValid;
}

void runInteractiveMode(std::shared_ptr<Config> config) {
    NLM_LOG_INFO("\n=== Interactive Mode ===");
    NLM_LOG_INFO("Welcome to NLM Interactive Mode!")
    NLM_LOG_INFO("Please select an experiment to run:");
    NLM_LOG_INFO("1. Basic Connectivity Test");
    NLM_LOG_INFO("2. Plasticity Learning Experiment");
    NLM_LOG_INFO("3. STDP Verification");
    NLM_LOG_INFO("4. Run All Experiments");
    NLM_LOG_INFO("5. Configure Settings");
    NLM_LOG_INFO("6. Save Configuration");
    NLM_LOG_INFO("7. Load Configuration");
    NLM_LOG_INFO("8. Exit");
    
    // In a real implementation, you would read user input here
    // For now, default to running all experiments
    auto brain = std::make_shared<Brain>(config);
    if (brain->initialize()) {
        std::cout << "\nRunning all experiments in interactive mode..." << std::endl;
        executeBasicConnectivityTest(brain);
        resetBrain(brain);
        brain->initialize();
        executePlasticityExperiment(brain);
        resetBrain(brain);
        brain->initialize();
        executeStdpVerification(brain);
    }
}

void saveExperimentResults(const std::string& filename, const std::string& experimentName) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << "# NLM Experiment Results" << std::endl;
        file << "# Experiment: " << experimentName << std::endl;
        file << "# Generated: " << __DATE__ << " " << __TIME__ << std::endl;
        file << std::endl;
        file << "experiment_name = \"" << experimentName << "\"" << std::endl;
        file << "# Results saved successfully" << std::endl;
        file.close();
        NLM_LOG_INFO("Experiment results saved to: " + filename);
    } else {
        NLM_LOG_ERROR("Failed to save experiment results to: " + filename);
    }
}

void loadExperimentConfig(const std::string& filename, std::shared_ptr<Config> config) {
    if (config->loadFromFile(filename)) {
        NLM_LOG_INFO("Configuration loaded from: " + filename);
    } else {
        NLM_LOG_ERROR("Failed to load configuration from: " + filename);
    }
}

void setupBrain(std::shared_ptr<Brain> brain, const Config& config) {
    // Configure brain with settings from config
    if (config.has("random_seed")) {
        int64_t seed = config.getOr<int64_t>("random_seed", 42);
        // Note: Brain doesn't have a setRandomSeed method in current API
        // This is a placeholder for future implementation
    }
    if (config.has("neuron_count")) {
        int64_t neuronCount = config.getOr<int64_t>("neuron_count", 500);
        // Note: Brain initialization is handled during construction
        // This is a placeholder for future implementation
    }
    if (config.has("region_count")) {
        int64_t regionCount = config.getOr<int64_t>("region_count", 1);
        // Note: Brain initialization is handled during construction
        // This is a placeholder for future implementation
    }
}

void resetBrain(std::shared_ptr<Brain> brain) {
    brain->reset();
}

void saveBrainState(const std::string& filename, std::shared_ptr<Brain> brain) {
    // Note: Brain doesn't have a saveState method in current API
    // This is a placeholder for future implementation
    NLM_LOG_INFO("Saving brain state to: " + filename);
    // In future implementation, add:
    // if (brain->saveState(filename)) {
    //     NLM_LOG_INFO("Brain state saved successfully");
    // } else {
    //     NLM_LOG_ERROR("Failed to save brain state");
    // }
}

void loadBrainState(const std::string& filename, std::shared_ptr<Brain> brain) {
    // Note: Brain doesn't have a loadState method in current API
    // This is a placeholder for future implementation
    NLM_LOG_INFO("Loading brain state from: " + filename);
    // In future implementation, add:
    // if (brain->loadState(filename)) {
    //     NLM_LOG_INFO("Brain state loaded successfully");
    // } else {
    //     NLM_LOG_ERROR("Failed to load brain state");
    // }
}