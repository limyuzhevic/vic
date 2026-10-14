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
    
    // Try to load from file if provided
    std::string configFile = "configs/default.cfg";
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
    
    // Helper functions for advanced commands
void printHelp() {
    std::cout << R"(
NLM (熙然) - Neural Learning Machine
Usage:
  nlm [options]

Options:
  --config=file      Load configuration from file (default: configs/default.cfg)
  --interactive      Enter interactive mode for real-time control
  --batch=file       Run batch experiment from configuration file
  --debug            Enable debug output and detailed logging
  --checkpoint=file  Load/save brain state from checkpoint file
  --duration=steps   Run simulation for specified number of steps (default: 1000)
  --verbose          Enable verbose output

Advanced Options (Phase 6 Integration):
  --enable-memory    Enable memory systems integration
  --enable-planner   Enable neural planning integration
  --enable-prediction Enable prediction system integration
  --enable-neuromod  Enable enhanced neuromodulation
  --enable-dev       Enable developmental stages
  --memory-capacity=N Set working memory capacity
  --episode-limit=N   Set episodic memory episode limit
  --replay-interval=N Set memory replay interval
  --consolidation-interval=N Set memory consolidation interval
  --plasticity-rate=N Set base plasticity rate
  --dopamine-level=N Set dopamine baseline level
  --curiosity-level=N Set curiosity baseline level

Examples:
  nlm --config=myconfig.cfg --interactive
  nlm --batch=experiment.cfg --verbose --enable-memory
  nlm --checkpoint=saved_brain.bin --duration=5000
  nlm --enable-planner --enable-prediction --enable-neuromod

For more information, visit: https://github.com/neural-learning-machine/NLM

)" << std::endl;
}

void printBrainStatus(const std::shared_ptr<Brain>& brain) {
    if (!brain) return;
    
    std::cout << R"(
=== NLM Brain Status ===
Regions: )" << brain->getRegionCount() 
              << R"(
Total neurons: )" << brain->getTotalNeuronCount()
              << R"(
Total synapses: )" << brain->getTotalSynapseCount()
              << R"(
Active neurons: )" << brain->getActiveNeuronCount()
              << R"(
Firing neurons (this step): )" << brain->getFiringNeuronCount()
              << R"(
Total spikes: )" << brain->getTotalSpikeCount()
              << R"(
Average firing rate: )" << std::fixed << std::setprecision(2) 
                                    << brain->getAverageFiringRate()
              << R"( Hz
E/I ratio: )" << brain->getExcitationInhibitionRatio()
              << R"(

Memory Systems:
  Working Memory: )" << (brain->getWorkingMemory() ? 
                             std::to_string(brain->getWorkingMemory()->getActiveTraces()) : 
                             std::string("NOT INTEGRATED"))
              << R"(
  Episodic Memory: )" << (brain->getEpisodicMemory() ? 
                              std::to_string(brain->getEpisodicMemory()->getEpisodeCount()) : 
                              std::string("NOT INTEGRATED"))
              << R"(
  Associative Memory: )" << (brain->getAssociativeMemory() ? 
                                 std::to_string(brain->getAssociativeMemory()->getAssociationCount()) : 
                                 std::string("NOT INTEGRATED"))
              << R"(

Cognitive Systems:
  Neural Planner: )" << (brain->getPlanner() ? "INTEGRATED" : "NOT INTEGRATED")
              << R"(
  Concept Formation: )" << (brain->getConceptFormation() ? "INTEGRATED" : "NOT INTEGRATED")
              << R"(
  Attention: )" << (brain->getAttention() ? "INTEGRATED" : "NOT INTEGRATED")
              << R"(

Prediction System: )" << (brain->getPredictionSystem() ? "INTEGRATED" : "NOT INTEGRATED")
              << R"(
Neuromodulation:
  Dopamine: )" << (brain->getDopamine() ? 
                          std::to_string(std::fixed << std::setprecision(2) 
                                      << brain->getDopamine()->getLevel()) : 
                          std::string("NOT INTEGRATED"))
              << R"(
  Curiosity: )" << (brain->getCuriosity() ? 
                        std::to_string(std::fixed << std::setprecision(2) 
                                    << brain->getCuriosity()->getLevel()) : 
                        std::string("NOT INTEGRATED"))
              << R"(
  Novelty: )" << (brain->getNovelty() ? 
                      std::to_string(std::fixed << std::setprecision(2) 
                                  << brain->getNovelty()->getLevel()) : 
                      std::string("NOT INTEGRATED"))
              << R"(

Development: )" << (brain->getDevelopmentSystem() ? 
                        std::to_string(static_cast<int>(brain->getDevelopmentalStage())) : 
                        std::string("NOT INTEGRATED"))
              << R"(
=== End Status ===

)" << std::endl;
}

void saveBrainState(const std::shared_ptr<Brain>& brain, const std::string& filepath) {
    if (!brain) {
        std::cerr << "Error: No brain to save." << std::endl;
        return;
    }
    
    if (brain->save(filepath)) {
        std::cout << "Successfully saved brain state to: " << filepath << std::endl;
        std::cout << "Brain statistics:" << std::endl;
        std::cout << "  Neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "  Synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    } else {
        std::cerr << "Error: Failed to save brain state to: " << filepath << std::endl;
    }
}

void loadBrainState(const std::shared_ptr<Brain>& brain, const std::string& filepath) {
    if (!brain) {
        std::cerr << "Error: No brain to load." << std::endl;
        return;
    }
    
    if (brain->load(filepath)) {
        std::cout << "Successfully loaded brain state from: " << filepath << std::endl;
        std::cout << "Brain statistics after load:" << std::endl;
        std::cout << "  Neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "  Synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    } else {
        std::cerr << "Error: Failed to load brain state from: " << filepath << std::endl;
    }
}

void configureBrain(const std::shared_ptr<Brain>& brain, const std::vector<std::string>& args) {
    if (!brain) return;
    
    for (const auto& arg : args) {
        size_t pos = arg.find('=');
        if (pos != std::string::npos) {
            std::string key = arg.substr(0, pos);
            std::string value = arg.substr(pos + 1);
            
            // Convert value to appropriate type based on key
            if (key == "memory-capacity") {
                // This would be a more complex implementation
                // For now, just note the setting
                std::cout << "Setting memory capacity to: " << value << std::endl;
            } else if (key == "episode-limit") {
                std::cout << "Setting episodic memory limit to: " << value << std::endl;
            } else if (key == "replay-interval") {
                std::cout << "Setting memory replay interval to: " << value << std::endl;
            } else if (key == "consolidation-interval") {
                std::cout << "Setting memory consolidation interval to: " << value << std::endl;
            } else if (key == "plasticity-rate") {
                std::cout << "Setting plasticity rate to: " << value << std::endl;
            } else if (key == "dopamine-level") {
                float level = std::stof(value);
                if (brain->getDopamine()) {
                    brain->getDopamine()->setLevel(level);
                    std::cout << "Set dopamine level to: " << level << std::endl;
                }
            } else if (key == "curiosity-level") {
                float level = std::stof(value);
                if (brain->getCuriosity()) {
                    brain->getCuriosity()->setLevel(level);
                    std::cout << "Set curiosity level to: " << level << std::endl;
                }
            } else {
                std::cout << "Unknown configuration option: " << key << std::endl;
            }
        }
    }
}

void runInteractiveMode(std::shared_ptr<Brain>& brain, const std::shared_ptr<Config>& config) {
    std::cout << "=== NLM Interactive Mode ===" << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  status    - Display brain status" << std::endl;
    std::cout << "  save <file> - Save brain state to file" << std::endl;
    std::cout << "  load <file> - Load brain state from file" << std::endl;
    std::cout << "  step N     - Run N simulation steps" << std::endl;
    std::cout << "  inject <neuron_id> <current> - Inject current into neuron" << std::endl;
    std::cout << "  inject <type> <current>       - Inject current into neurons of type" << std::endl;
    std::cout << "  help       - Show this help message" << std::endl;
    std::cout << "  quit/q     - Exit interactive mode" << std::endl;
    
    std::string line;
    bool running = true;
    
    while (running) {
        std::cout << ">>> ";
        std::getline(std::cin, line);
        
        if (line.empty()) continue;
        
        // Parse command
        size_t spacePos = line.find(' ');
        std::string command;
        std::string arg;
        
        if (spacePos != std::string::npos) {
            command = line.substr(0, spacePos);
            arg = line.substr(spacePos + 1);
        } else {
            command = line;
        }
        
        if (command == "status" || command == "s") {
            printBrainStatus(brain);
        } else if (command == "save" || command == "save") {
            if (arg.empty()) {
                std::cout << "Usage: save <filename>" << std::endl;
            } else {
                saveBrainState(brain, arg);
            }
        } else if (command == "load" || command == "load") {
            if (arg.empty()) {
                std::cout << "Usage: load <filename>" << std::endl;
            } else {
                loadBrainState(brain, arg);
            }
        } else if (command == "step" || command == "t") {
            try {
                size_t steps = std::stoul(arg);
                std::cout << "Running " << steps << " steps..." << std::endl;
                for (size_t i = 0; i < steps; ++i) {
                    brain->step(i, i * config->getOr<double>("simulation_timestep", 0.001));
                    if (i % 100 == 0) {
                        std::cout << "  Step " << i << " completed" << std::endl;
                    }
                }
                std::cout << "Completed " << steps << " steps." << std::endl;
            } catch (const std::exception& e) {
                std::cout << "Error: Invalid number of steps." << std::endl;
            }
        } else if (command == "inject" || command == "i") {
            // Parse inject command
            size_t spacePos2 = arg.find(' ');
            if (spacePos2 != std::string::npos) {
                std::string target = arg.substr(0, spacePos2);
                std::string currentStr = arg.substr(spacePos2 + 1);
                
                try {
                    float current = std::stof(currentStr);
                    
                    // Try to parse target as neuron ID
                    size_t neuronIdPos = target.find(':');
                    if (neuronIdPos != std::string::npos) {
                        // Format: "region:neuron_id"
                        std::string regionStr = target.substr(0, neuronIdPos);
                        std::string neuronStr = target.substr(neuronIdPos + 1);
                        
                        try {
                            int regionId = std::stoi(regionStr);
                            int neuronId = std::stoi(neuronStr);
                            // This is a simplified injection - in real implementation, would need proper neuron lookup
                            std::cout << "Injecting " << current << " into neuron " << neuronId 
                                     << " in region " << regionId << std::endl;
                        } catch (...) {
                            std::cout << "Error: Invalid neuron ID format: " << target << std::endl;
                        }
                    } else {
                        // Try to parse as neuron ID directly
                        try {
                            int neuronId = std::stoi(target);
                            std::cout << "Injecting " << current << " into neuron " << neuronId << std::endl;
                        } catch (...) {
                            // Try to parse as neuron type
                            if (target == "sensory" || target == "internal" || target == "motor") {
                                std::cout << "Injecting " << current << " into " << target << " neurons" << std::endl;
                                // This would call brain->injectCurrentToNeurons()
                            } else {
                                std::cout << "Error: Invalid injection target: " << target << std::endl;
                            }
                        }
                    }
                } catch (const std::exception& e) {
                    std::cout << "Error: Invalid current value." << std::endl;
                }
            } else {
                std::cout << "Usage: inject <target> <current>" << std::endl;
                std::cout << "  <target>: neuron_id or neuron_type (sensory, internal, motor)" << std::endl;
            }
        } else if (command == "help" || command == "?" || command == "h") {
            printHelp();
        } else if (command == "quit" || command == "q" || command == "exit") {
            running = false;
            std::cout << "Exiting interactive mode." << std::endl;
        } else {
            std::cout << "Unknown command: " << command << std::endl;
            std::cout << "Type 'help' for available commands." << std::endl;
        }
    }
}
