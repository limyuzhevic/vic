// NLM (熙然) - Neural Learning Machine
// Enhanced Main Entry Point
//
// This is the main entry point for NLM with improved structure, modularity,
// and user experience. Supports multiple demo modes and configurations.

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
#include <map>
#include <chrono>

using namespace nlm;

// Forward declarations for demo functions
void runPhase2Demo(const std::shared_ptr<Config>& config);
void runPhase3Demo(const std::shared_ptr<Config>& config);
void runPhase4Demo(const std::shared_ptr<Config>& config);
void runPhase5Demo(const std::shared_ptr<Config>& config);
void runPhase6Demo(const std::shared_ptr<Config>& config);
void runBenchmark(const std::shared_ptr<Config>& config);
void runLearningExperiment(const std::shared_ptr<Brain>& brain);
void runConnectivityTest(const std::shared_ptr<Brain>& brain);
void runPlasticityTest(const std::shared_ptr<Brain>& brain);
void runSTDPTest(const std::shared_ptr<Brain>& brain);
void printBanner();
void printHelp();
void printVersion();
std::string getCurrentTimestamp();
std::string getSystemInfo();
void setupLogging(const std::shared_ptr<Config>& config);
void logSystemInfo();
void runComprehensiveDemo(const std::shared_ptr<Config>& config);
void runQuickTests(const std::shared_ptr<Config>& config);
void saveBrainState(const std::shared_ptr<Brain>& brain, const std::string& filename);
std::shared_ptr<Brain> loadBrainState(const std::shared_ptr<Config>& config, const std::string& filename);
bool validateConfig(const std::shared_ptr<Config>& config);

// Enhanced main.cpp for improved NLM usability
// This file now provides:
// 1. Modular, clean structure with separated concerns
// 2. Comprehensive command-line interface
// 3. Better error handling and user feedback
// 4. Multiple demo modes and advanced options
// 5. Improved logging and system information
// 6. Configuration presets and file management

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
#include <map>
#include <chrono>
#include <fstream>
#include <sstream>

using namespace nlm;

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
    // Parse command line arguments
    ConfigOptions options = parseAdvancedArgs(argc, argv);
    
    // Setup logging based on options
    auto config = std::make_shared<Config>();
    
    // Apply preset configurations
    if (options.preset == "default") {
        config->set("neuron_count", static_cast<int64_t>(options.neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(options.regionCount), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    } else if (options.preset == "research") {
        config->set("neuron_count", static_cast<int64_t>(options.neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(options.regionCount), ConfigSource::Default);
        config->set("connection_probability", 0.05f, ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(12345), ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.05f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.025f, ConfigSource::Default);
    } else if (options.preset == "demo") {
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.2f, ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(999), ConfigSource::Default);
    } else if (options.preset == "production") {
        config->set("neuron_count", static_cast<int64_t>(options.neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(options.regionCount), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    }
    
    // Set verbose/quiet logging options
    config->set("verbose_logging", options.verbose, ConfigSource::Runtime);
    config->set("quiet_logging", options.quiet, ConfigSource::Runtime);
    
    // Load configuration from file if specified
    if (options.configFile != "configs/default.cfg") {
        if (config->loadFromFile(options.configFile)) {
            if (!options.quiet) {
                std::cout << "Loaded configuration from: " << options.configFile << std::endl;
            }
        }
    }
    
    // Apply command line configuration
    config->loadFromArgs(argc, argv);
    
    // Validate configuration
    if (!validateConfig(config)) {
        std::cerr << "ERROR: Invalid configuration" << std::endl;
        return 1;
    }
    
    // Setup logging system
    setupLogging(config);
    
    // Log system information
    if (!options.quiet) {
        logSystemInfo();
        printBanner();
        
        std::cout << "Starting NLM " << options.demoMode << " demo..." << std::endl;
        std::cout << "Timestamp: " << getCurrentTimestamp() << std::endl;
        std::cout << "Parameters: Neurons=" << options.neuronCount 
                  << ", Steps=" << options.maxSteps 
                  << ", Learning=" << (options.enableLearning ? "Yes" : "No")
                  << ", Development=" << (options.enableDevelopment ? "Yes" : "No") << std::endl;
        std::cout << std::endl;
    }
    
    // Handle save/load operations
    if (!options.saveFile.empty()) {
        auto brain = std::make_shared<Brain>(config);
        if (brain->initialize()) {
            saveBrainState(brain, options.saveFile);
            if (!options.quiet) {
                std::cout << "Brain state saved to: " << options.saveFile << std::endl;
            }
            return 0;
        } else {
            std::cerr << "ERROR: Failed to initialize brain for save operation" << std::endl;
            return 1;
        }
    }
    
    if (!options.loadFile.empty()) {
        auto brain = loadBrainState(config, options.loadFile);
        if (brain) {
            if (!options.quiet) {
                std::cout << "Brain state loaded from: " << options.loadFile << std::endl;
            }
            // Continue with loaded brain
            brain->logStatus();
        } else {
            std::cerr << "ERROR: Failed to load brain state" << std::endl;
            return 1;
        }
    }
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    if (!options.quiet) {
        brain->logStatus();
    }
    
    // Run the selected demo
    bool success = false;
    
    if (options.demoMode == "phase2") {
        success = runPhase2Demo(config, brain);
    } else if (options.demoMode == "phase3") {
        success = runPhase3Demo(config, brain);
    } else if (options.demoMode == "phase4") {
        success = runPhase4Demo(config, brain);
    } else if (options.demoMode == "phase5") {
        success = runPhase5Demo(config, brain);
    } else if (options.demoMode == "phase6") {
        success = runPhase6Demo(config, brain);
    } else if (options.demoMode == "benchmark") {
        success = runBenchmark(config, brain);
    } else if (options.demoMode == "learning") {
        success = runLearningExperiment(brain);
    } else if (options.demoMode == "comprehensive") {
        success = runComprehensiveDemo(config, brain);
    } else if (options.demoMode == "quick") {
        success = runQuickTests(config, brain);
    } else {
        std::cerr << "ERROR: Unknown demo mode: " << options.demoMode << std::endl;
        printHelp();
        return 1;
    }
    
    if (!success) {
        std::cerr << "ERROR: Demo failed" << std::endl;
        return 1;
    }
    
    if (!options.quiet) {
        std::cout << std::endl;
        std::cout << "=== NLM Demo Completed Successfully ===" << std::endl;
        std::cout << "Timestamp: " << getCurrentTimestamp() << std::endl;
        std::cout << "Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "Final Spike Count: " << brain->getTotalSpikeCount() << std::endl;
    }
    
    return 0;
}
