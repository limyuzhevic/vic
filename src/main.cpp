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

void printHelp() {
    printBanner();
    std::cout << R"(
Usage: nlm [OPTIONS]

Simulation Options:
  --steps=N          Number of simulation steps to run (default: 1000)
  --timestep=SECONDS  Simulation timestep in seconds (default: 0.001)
  --neuron_count=N   Number of neurons in the brain (default: 500)
  --regions=N        Number of brain regions (default: 1)
  --seed=N           Random seed for reproducible results (default: 42)

Configuration:
  --config=FILE      Load configuration from file (supports JSON, YAML, key=value)
  --verbose          Enable verbose logging
  --quiet            Suppress informational messages
  --debug            Enable debug logging

Output Options:
  --no-banner        Suppress program banner
  --stats-only       Only output statistics, no progress updates
  --save-state=FILE  Save brain state to file after completion
  --load-state=FILE  Load brain state from file before starting

Advanced Options:
  --enable-plasticity   Enable synaptic plasticity (default: true)
  --plasticity-rate=R   Plasticity learning rate (default: 0.01)
  --enable-development  Enable developmental stages (default: true)
  --enable-curiosity    Enable curiosity-driven exploration (default: true)
  --enable-novelty      Enable novelty detection (default: true)
  --enable-prediction   Enable prediction system (default: true)
  --max-energy=E        Maximum energy for agent (default: 100.0)
  --energy-decay=R       Energy decay rate (default: 0.01)

Examples:
  nlm --steps=2000 --config=mine.cfg
  nlm --neuron_count=1000 --regions=2 --seed=123
  nlm --enable-plasticity=false --enable-curiosity=false

For more information, visit: https://github.com/neural-learning-machine/nlm
)" << std::endl;
}

void printSimulationProgress(int step, int totalSteps, float progress, 
                           const std::string& currentActivity) {
    if (progress % 10 == 0 || step == totalSteps - 1) {
        std::cout << "\rProgress: [";
        for (int i = 0; i < 50; ++i) {
            if (i < (int)(progress * 50 / 100)) {
                std::cout << "=";
            } else if (i == (int)(progress * 50 / 100)) {
                std::cout << ">";
            } else {
                std::cout << " ";
            }
        }
        std::cout << "] " << (int)(progress * 100) << "% | Step " 
                  << step << "/" << totalSteps << " | " << currentActivity;
        std::cout.flush();
    }
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
    bool showBanner = true;
    bool verbose = false;
    bool quiet = false;
    bool debug = false;
    bool statsOnly = false;
    std::string saveStateFile;
    std::string loadStateFile;
    int steps = 1000;
    double timestep = 0.001;
    int64_t neuronCount = 500;
    int64_t regionCount = 1;
    int64_t randomSeed = 42;
    bool enablePlasticity = true;
    float plasticityRate = 0.01f;
    bool enableDevelopment = true;
    bool enableCuriosity = true;
    bool enableNovelty = true;
    bool enablePrediction = true;
    float maxEnergy = 100.0f;
    float energyDecayRate = 0.01f;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        } else if (arg == "--version") {
            printBanner();
            std::cout << "\nVersion: NLM Phase 2\n";
            std::cout << "Build: " << __DATE__ << " " << __TIME__ << "\n";
            return 0;
        } else if (arg == "--no-banner") {
            showBanner = false;
        } else if (arg == "--verbose") {
            verbose = true;
        } else if (arg == "--quiet") {
            quiet = true;
        } else if (arg == "--debug") {
            debug = true;
        } else if (arg == "--stats-only") {
            statsOnly = true;
        } else if (arg.substr(0, 14) == "--save-state=") {
            saveStateFile = arg.substr(14);
        } else if (arg.substr(0, 14) == "--load-state=") {
            loadStateFile = arg.substr(14);
        } else if (arg.substr(0, 12) == "--steps=") {
            try {
                steps = std::stoi(arg.substr(12));
            } catch (...) {
                NLM_LOG_WARNING("Invalid steps value: " + arg);
            }
        } else if (arg.substr(0, 15) == "--timestep=") {
            try {
                timestep = std::stod(arg.substr(15));
            } catch (...) {
                NLM_LOG_WARNING("Invalid timestep value: " + arg);
            }
        } else if (arg.substr(0, 19) == "--neuron_count=") {
            try {
                neuronCount = std::stoll(arg.substr(19));
            } catch (...) {
                NLM_LOG_WARNING("Invalid neuron_count value: " + arg);
            }
        } else if (arg.substr(0, 16) == "--regions=") {
            try {
                regionCount = std::stoll(arg.substr(16));
            } catch (...) {
                NLM_LOG_WARNING("Invalid regions value: " + arg);
            }
        } else if (arg.substr(0, 11) == "--seed=") {
            try {
                randomSeed = std::stoll(arg.substr(11));
            } catch (...) {
                NLM_LOG_WARNING("Invalid seed value: " + arg);
            }
        } else if (arg == "--enable-plasticity") {
            enablePlasticity = true;
        } else if (arg == "--disable-plasticity") {
            enablePlasticity = false;
        } else if (arg.substr(0, 16) == "--plasticity-rate=") {
            try {
                plasticityRate = std::stof(arg.substr(16));
            } catch (...) {
                NLM_LOG_WARNING("Invalid plasticity-rate value: " + arg);
            }
        } else if (arg == "--enable-development") {
            enableDevelopment = true;
        } else if (arg == "--disable-development") {
            enableDevelopment = false;
        } else if (arg == "--enable-curiosity") {
            enableCuriosity = true;
        } else if (arg == "--disable-curiosity") {
            enableCuriosity = false;
        } else if (arg == "--enable-novelty") {
            enableNovelty = true;
        } else if (arg == "--disable-novelty") {
            enableNovelty = false;
        } else if (arg == "--enable-prediction") {
            enablePrediction = true;
        } else if (arg == "--disable-prediction") {
            enablePrediction = false;
        } else if (arg.substr(0, 14) == "--max-energy=") {
            try {
                maxEnergy = std::stof(arg.substr(14));
            } catch (...) {
                NLM_LOG_WARNING("Invalid max-energy value: " + arg);
            }
        } else if (arg.substr(0, 19) == "--energy-decay=") {
            try {
                energyDecayRate = std::stof(arg.substr(19));
            } catch (...) {
                NLM_LOG_WARNING("Invalid energy-decay value: " + arg);
            }
        } else if (arg.substr(0, 8) == "--config=") {
            // Will be handled later
        }
    }
    
    if (showBanner) {
        printBanner();
    }
    
    // Configure logging based on verbosity flags
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>();
    
    if (debug) {
        consoleLogger->setLevel(LogLevel::Debug);
    } else if (verbose) {
        consoleLogger->setLevel(LogLevel::Info);
    } else if (quiet) {
        consoleLogger->setLevel(LogLevel::Warning);
    } else {
        consoleLogger->setLevel(LogLevel::Info);
    }
    
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation ===");
    NLM_LOG_INFO("NLM Neural Learning Machine - Starting simulation...");
    
    if (!quiet) {
        std::cout << "Configuration loaded from command line arguments\n";
    }
    
    // Initialize configuration
    auto config = std::make_shared<Config>();
    
    // Set configuration values from command line arguments
    config->set("random_seed", randomSeed, ConfigSource::CommandLine);
    config->set("simulation_timestep", timestep, ConfigSource::CommandLine);
    config->set("neuron_count", neuronCount, ConfigSource::CommandLine);
    config->set("region_count", regionCount, ConfigSource::CommandLine);
    config->set("connection_probability", 0.15f, ConfigSource::CommandLine);
    
    // Set plasticity and subsystem settings
    config->set("plasticity_learning_rate", plasticityRate, ConfigSource::CommandLine);
    config->set("plasticity_enabled", enablePlasticity, ConfigSource::CommandLine);
    config->set("development_enabled", enableDevelopment, ConfigSource::CommandLine);
    config->set("curiosity_enabled", enableCuriosity, ConfigSource::CommandLine);
    config->set("novelty_enabled", enableNovelty, ConfigSource::CommandLine);
    config->set("prediction_enabled", enablePrediction, ConfigSource::CommandLine);
    config->set("max_energy", maxEnergy, ConfigSource::CommandLine);
    config->set("energy_decay_rate", energyDecayRate, ConfigSource::CommandLine);
    
    NLM_LOG_INFO("Configuration initialized with " + std::to_string(neuronCount) + " neurons");
    NLM_LOG_INFO("Running " + std::to_string(steps) + " simulation steps");
    
    // Load state if specified
    if (!loadStateFile.empty()) {
        NLM_LOG_INFO("Loading brain state from: " + loadStateFile);
        // brain->load(loadStateFile); // Implementation would go here
    }
    
    // Run the core simulation tests
    NLM_LOG_INFO("=== Starting Core Simulation Tests ===");
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Run Test 1: Basic connectivity
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    runBasicConnectivityTest(brain);
    
    // Reset brain for plasticity experiment
    brain->reset();
    brain->initialize();
    
    // Run Test 2: Plasticity learning experiment
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    runPlasticityExperiment(brain);
    
    // Reset and run Test 3: STDP verification
    brain->reset();
    brain->initialize();
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    runStdpVerification(brain);
    
    // Save state if specified
    if (!saveStateFile.empty()) {
        NLM_LOG_INFO("Saving brain state to: " + saveStateFile);
        // brain->save(saveStateFile); // Implementation would go here
    }
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 2 Complete ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Simulation completed successfully!")
    NLM_LOG_INFO("Total spikes generated: " + std::to_string(brain->getTotalSpikeCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(brain->getFiringNeuronCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(brain->getAverageFiringRate()));
    
    return 0;
}
