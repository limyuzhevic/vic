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

namespace {
    // Configuration constants
    constexpr uint64_t DEFAULT_RANDOM_SEED = 42;
    constexpr double DEFAULT_TIMESTEP = 0.001;
    constexpr int64_t DEFAULT_NEURON_COUNT = 500;
    constexpr int64_t DEFAULT_REGION_COUNT = 1;
    constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.15f;
    constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.02f;
    constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.015f;
    constexpr float DEFAULT_STDP_TAU = 20.0f;
    constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;
    constexpr float DEFAULT_PRUNING_RATE = 0.00001f;
    
    // Demo modes
    enum class DemoMode {
        BASIC_CONNECTIVITY,
        PLASTICITY_LEARNING,
        STDP_VERIFICATION,
        ALL_TESTS,
        BRIEF
    };
}

} // namespace nlm

namespace {
DemoMode parseDemoMode(const std::string& modeStr) {
    if (modeStr == "brief" || modeStr == "b") return DemoMode::BRIEF;
    if (modeStr == "connectivity" || modeStr == "c") return DemoMode::BASIC_CONNECTIVITY;
    if (modeStr == "plasticity" || modeStr == "p") return DemoMode::PLASTICITY_LEARNING;
    if (modeStr == "stdp" || modeStr == "s") return DemoMode::STDP_VERIFICATION;
    return DemoMode::ALL_TESTS;  // Default
}

std::string demoModeToString(DemoMode mode) {
    switch (mode) {
        case DemoMode::BRIEF: return "BRIEF";
        case DemoMode::BASIC_CONNECTIVITY: return "BASIC_CONNECTIVITY";
        case DemoMode::PLASTICITY_LEARNING: return "PLASTICITY_LEARNING";
        case DemoMode::STDP_VERIFICATION: return "STDP_VERIFICATION";
        case DemoMode::ALL_TESTS: return "ALL_TESTS";
        default: return "UNKNOWN";
    }
}

void printDemoUsage() {
    std::cout << "=== NLM Phase 2 Demo Options ===\n" << std::endl;
    std::cout << "Usage: nlm [options]\n" << std::endl;
    std::cout << "Options:\n" << std::endl;
    std::cout << "  -h, --help           Show this help message\n" << std::endl;
    std::cout << "  -b, --brief          Run brief demo (connectivity only)\n" << std::endl;
    std::cout << "  -c, --connectivity   Run basic connectivity demo only\n" << std::endl;
    std::cout << "  -p, --plasticity     Run plasticity learning demo only\n" << std::endl;
    std::cout << "  -s, --stdp           Run STDP verification demo only\n" << std::endl;
    std::cout << "  -v, --verbose        Enable verbose logging\n" << std::endl;
    std::cout << "  --config <file>      Load configuration from file\n" << std::endl;
    std::cout << "\nDefault: All three demos (connectivity, plasticity, STDP)\n" << std::endl;
    std::cout << "Examples:\n" << std::endl;
    std::cout << "  nlm --brief          Quick demo of neural connectivity\n" << std::endl;
    std::cout << "  nlm --connectivity   Test basic spike propagation\n" << std::endl;
    std::cout << "  nlm --plasticity     Demonstrate learning through experience\n" << std::endl;
    std::cout << "  nlm --stdp           Verify spike-timing dependent plasticity\n" << std::endl;
    std::cout << "  nlm --verbose        Detailed output for debugging\n" << std::endl;
}

void loadConfiguration(std::shared_ptr<Config> config, int argc, char** argv) {
    // Load from file if specified
    std::string configFile = "configs/default.cfg";
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h") continue;  // Skip help flag
        if (arg == "--brief" || arg == "-b") continue;
        if (arg == "--connectivity" || arg == "-c") continue;
        if (arg == "--plasticity" || arg == "-p") continue;
        if (arg == "--stdp" || arg == "-s") continue;
        if (arg == "--verbose" || arg == "-v") continue;
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
            break;  // Only use first config file specified
        }
    }
    
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration (file not found: " + configFile + ")");
    }
    
    // Load from command line arguments
    config->loadFromArgs(argc, argv);
}

void applyPhase2Defaults(std::shared_ptr<Config> config) {
    // Set phase 2 specific defaults if not already configured
    if (!config->has("random_seed")) {
        config->set("random_seed", DEFAULT_RANDOM_SEED, ConfigSource::Default);
    }
    if (!config->has("simulation_timestep")) {
        config->set("simulation_timestep", DEFAULT_TIMESTEP, ConfigSource::Default);
    }
    if (!config->has("neuron_count")) {
        config->set("neuron_count", DEFAULT_NEURON_COUNT, ConfigSource::Default);
    }
    if (!config->has("region_count")) {
        config->set("region_count", DEFAULT_REGION_COUNT, ConfigSource::Default);
    }
    if (!config->has("connection_probability")) {
        config->set("connection_probability", DEFAULT_CONNECTION_PROBABILITY, ConfigSource::Default);
    }
    
    // STDP parameters
    if (!config->has("stdp_ltp_weight")) {
        config->set("stdp_ltp_weight", DEFAULT_STDP_LTP_WEIGHT, ConfigSource::Default);
    }
    if (!config->has("stdp_ltd_weight")) {
        config->set("stdp_ltd_weight", DEFAULT_STDP_LTD_WEIGHT, ConfigSource::Default);
    }
    if (!config->has("stdp_tau")) {
        config->set("stdp_tau", DEFAULT_STDP_TAU, ConfigSource::Default);
    }
    
    // Structural plasticity parameters
    if (!config->has("synaptogenesis_rate")) {
        config->set("synaptogenesis_rate", DEFAULT_SYNAPTOGENESIS_RATE, ConfigSource::Default);
    }
    if (!config->has("pruning_rate")) {
        config->set("pruning_rate", DEFAULT_PRUNING_RATE, ConfigSource::Default);
    }
}

void printConfigurationSummary(const Config& config) {
    NLM_LOG_INFO("=== Configuration Summary ===");
    
    auto printValue = [&config](const std::string& key, const std::string& description) {
        if (config.has(key)) {
            if (key.find("random_seed") != std::string::npos) {
                NLM_LOG_INFO(description + ": " + std::to_string(config.getOr<int64_t>(key, 0)));
            } else if (key.find("timestep") != std::string::npos) {
                NLM_LOG_INFO(description + ": " + std::to_string(config.getOr<double>(key, 0.0)) + "s");
            } else if (key.find("weight") != std::string::npos) {
                NLM_LOG_INFO(description + ": " + std::to_string(config.getOr<float>(key, 0.0f)));
            } else if (key.find("rate") != std::string::npos) {
                NLM_LOG_INFO(description + ": " + std::to_string(config.getOr<float>(key, 0.0f)));
            } else {
                NLM_LOG_INFO(description + ": " + std::to_string(config.getOr<int64_t>(key, 0)));
            }
        }
    };
    
    printValue("random_seed", "Random seed");
    printValue("simulation_timestep", "Simulation timestep");
    printValue("neuron_count", "Neuron count");
    printValue("region_count", "Region count");
    printValue("connection_probability", "Connection probability");
    printValue("stdp_ltp_weight", "STDP LTP weight");
    printValue("stdp_ltd_weight", "STDP LTD weight");
    printValue("stdp_tau", "STDP time constant");
    printValue("synaptogenesis_rate", "Synaptogenesis rate");
    printValue("pruning_rate", "Pruning rate");
    
    NLM_LOG_INFO("=====================");
}
