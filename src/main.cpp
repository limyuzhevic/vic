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
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "experiments/ExperimentRunner.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <map>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                 ║
    ║                                                               ║
    ║     Complete artificial brain with integrated systems.         ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printHelp() {
    std::cout << "=== NLM Phase 6 Command Line Interface ===" << std::endl;
    std::cout << "Available commands:" << std::endl;
    std::cout << "  demo         Run Phase 6 integration demo" << std::endl;
    std::cout << "  verify       Verify system integration" << std::endl;
    std::cout << "  config show  Show current configuration" << std::endl;
    std::cout << "  config set <key> <value>  Set configuration value" << std::endl;
    std::cout << "  log level <level>  Set log level (debug, info, warning, error, critical)" << std::endl;
    std::cout << "  help         Show this help" << std::endl;
    std::cout << "  quit, exit   Exit program" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  ./nlm demo --neurons 1000 --steps 5000" << std::endl;
    std::cout << "  ./nlm verify --quick" << std::endl;
    std::cout << "  ./nlm log level info" << std::endl;
}

// Brain Control Interface
class BrainController {
public:
    BrainController(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    // System inspection
    void inspectSystem() const {
        std::cout << "=== NLM Brain System Inspection ===" << std::endl;
        
        if (!brain_) {
            std::cout << "ERROR: Brain not initialized" << std::endl;
            return;
        }
        
        std::cout << "Brain Status:" << std::endl;
        std::cout << "  Total neurons: " << brain_->getTotalNeuronCount() << std::endl;
        std::cout << "  Total synapses: " << brain_->getTotalSynapseCount() << std::endl;
        std::cout << "  Active neurons: " << brain_->getActiveNeuronCount() << std::endl;
        std::cout << "  Firing neurons: " << brain_->getFiringNeuronCount() << std::endl;
        std::cout << "  Total spikes: " << brain_->getTotalSpikeCount() << std::endl;
        std::cout << "  Pending spike events: " << brain_->getPendingSpikeEventCount() << std::endl;
        
        // Memory systems
        std::cout << "" << std::endl;
        std::cout << "Memory Systems:" << std::endl;
        if (auto* wm = brain_->getWorkingMemory()) {
            std::cout << "  Working Memory: " << wm->getActiveTraces() << " active traces, " 
                      << wm->getCapacity() << " capacity" << std::endl;
        } else {
            std::cout << "  Working Memory: NOT AVAILABLE" << std::endl;
        }
        
        if (auto* em = brain_->getEpisodicMemory()) {
            std::cout << "  Episodic Memory: " << em->getEpisodeCount() << " episodes stored" << std::endl;
        } else {
            std::cout << "  Episodic Memory: NOT AVAILABLE" << std::endl;
        }
        
        // Neuromodulation
        std::cout << "" << std::endl;
        std::cout << "Neuromodulation:" << std::endl;
        if (auto* dopamine = brain_->getDopamine()) {
            std::cout << "  Dopamine: " << dopamine->getLevel() << std::endl;
        } else {
            std::cout << "  Dopamine: NOT AVAILABLE" << std::endl;
        }
        
        if (auto* curiosity = brain_->getCuriosity()) {
            std::cout << "  Curiosity: " << curiosity->getLevel() << std::endl;
        } else {
            std::cout << "  Curiosity: NOT AVAILABLE" << std::endl;
        }
        
        // Cognition
        std::cout << "" << std::endl;
        std::cout << "Cognitive Systems:" << std::endl;
        if (auto* planner = brain_->getPlanner()) {
            std::cout << "  Neural Planner: AVAILABLE" << std::endl;
        } else {
            std::cout << "  Neural Planner: NOT AVAILABLE" << std::endl;
        }
        
        if (auto* conceptForm = brain_->getConceptFormation()) {
            std::cout << "  Concept Formation: " << conceptForm->getConceptCount() << " concepts" << std::endl;
        } else {
            std::cout << "  Concept Formation: NOT AVAILABLE" << std::endl;
        }
        
        if (auto* attention = brain_->getAttention()) {
            std::cout << "  Attention System: AVAILABLE" << std::endl;
        } else {
            std::cout << "  Attention System: NOT AVAILABLE" << std::endl;
        }
        
        // Prediction
        std::cout << "" << std::endl;
        std::cout << "Prediction System:" << std::endl;
        if (auto* pred = brain_->getPredictionSystem()) {
            std::cout << "  Prediction System: AVAILABLE" << std::endl;
        } else {
            std::cout << "  Prediction System: NOT AVAILABLE" << std::endl;
        }
        
        // Development
        std::cout << "" << std::endl;
        std::cout << "Development:" << std::endl;
        std::cout << "  Stage: " << static_cast<int>(brain_->getDevelopmentalStage()) << std::endl;
        
        // Statistics
        std::cout << "" << std::endl;
        std::cout << "Brain Statistics:" << std::endl;
        std::cout << "  E/I Ratio: " << std::fixed << std::setprecision(3) << brain_->getExcitationInhibitionRatio() << std::endl;
        std::cout << "  Average firing rate: " << std::fixed << std::setprecision(6) << brain_->getAverageFiringRate() << std::endl;
        
        // Regional info
        std::cout << "" << std::endl;
        std::cout << "Regions:" << std::endl;
        for (const auto& id : brain_->getRegionIds()) {
            if (auto* region = brain_->getRegion(id)) {
                std::cout << "  Region " << id.index() << " ('" << region->getName() << "'): "
                          << region->getTotalNeuronCount() << " neurons, "
                          << region->getSynapseCount() << " synapses" << std::endl;
            }
        }
    }
    
    // Run simulation with custom parameters
    void runSimulation(int steps, int neurons, const std::string& configFile) {
        std::cout << "=== Running NLM Phase 6 Integration ===" << std::endl;
        std::cout << "Configuration: " << neurons << " neurons, " << steps << " steps" << std::endl;
        
        // Load config
        auto config = std::make_shared<Config>();
        
        if (!configFile.empty() && config->loadFromFile(configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + configFile);
        } else {
            NLM_LOG_INFO("Using default configuration.");
        }
        
        // Set parameters
        config->set("neuron_count", static_cast<int64_t>(neurons), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        
        // Initialize brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
            return;
        }
        
        std::cout << "Brain initialized successfully." << std::endl;
        
        // Create Phase 6 experiment
        Phase6IntegratedExperiment experiment;
        Phase6Config phaseConfig;
        phaseConfig.neuronCount = neurons;
        phaseConfig.maxSteps = steps;
        phaseConfig.enableCheckpointing = true;
        phaseConfig.enableReplay = true;
        phaseConfig.enableDevelopment = true;
        
        // Run experiment
        auto result = experiment.run(phaseConfig);
        
        // Display results
        std::cout << "" << std::endl;
        std::cout << "=== Experiment Results ===" << std::endl;
        std::cout << "Total reward: " << std::fixed << std::setprecision(6) << result.totalReward << std::endl;
        std::cout << "Average firing rate: " << std::fixed << std::setprecision(6) << result.avgFiringRate << std::endl;
        std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
        std::cout << "Novelty level: " << result.noveltyLevel << std::endl;
        std::cout << "Curiosity level: " << result.curiosityLevel << std::endl;
        
        std::cout << "" << std::endl;
        std::cout << "=== Integration Status ===" << std::endl;
        std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
        std::cout << "Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
        
        std::cout << "" << std::endl;
        std::cout << "=== Phase 6 Integration Complete ===" << std::endl;
    }
    
    // Run quick integration verification
    void verifyIntegration(bool quick) {
        std::cout << "=== Phase 6 Integration Verification ===" << std::endl;
        if (quick) {
            std::cout << "Running quick integration check..." << std::endl;
        } else {
            std::cout << "Running full integration verification..." << std::endl;
        }
        
        // Create minimal brain for verification
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "ERROR: Brain initialization failed!" << std::endl;
            return;
        }
        
        Phase6IntegratedExperiment experiment;
        
        if (quick) {
            bool success = experiment.verifyIntegration();
            if (success) {
                std::cout << "✓ Quick integration verification PASSED" << std::endl;
            } else {
                std::cout << "✗ Quick integration verification FAILED" << std::endl;
            }
        } else {
            std::cout << "Running full integration tests..." << std::endl;
            experiment.testMemoryIntegration();
            experiment.testNeuromodulationIntegration();
            experiment.testCheckpointing();
            experiment.testReplay();
            std::cout << "✓ Full integration verification COMPLETED" << std::endl;
        }
    }
    
private:
    std::shared_ptr<Brain> brain_;
};

int main(int argc, char** argv) {
    if (argc < 2) {
        printBanner();
        printHelp();
        return 0;
    }
    
    std::string command = argv[1];
    
    // Initialize global logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Parse common options
    std::string configFile;
    std::string logLevel;
    
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--config" && i + 1 < argc) {
            configFile = argv[++i];
        } else if (arg == "--log-level" && i + 1 < argc) {
            logLevel = argv[++i];
        }
    }
    
    // Set log level if specified
    if (!logLevel.empty()) {
        if (logLevel == "debug") logger->setLevel(LogLevel::Debug);
        else if (logLevel == "info") logger->setLevel(LogLevel::Info);
        else if (logLevel == "warning") logger->setLevel(LogLevel::Warning);
        else if (logLevel == "error") logger->setLevel(LogLevel::Error);
        else if (logLevel == "critical") logger->setLevel(LogLevel::Critical);
    }
    
    // Handle commands
    if (command == "demo") {
        printBanner();
        
        int neurons = 500;
        int steps = 2000;
        
        // Parse demo arguments
        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--neurons" && i + 1 < argc) neurons = std::stoi(argv[++i]);
            else if (arg == "--steps" && i + 1 < argc) steps = std::stoi(argv[++i]);
        }
        
        // Initialize basic config for demo
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neurons), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        
        // Initialize brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
            return 1;
        }
        
        // Create and run Phase 6 demo
        Phase6IntegratedExperiment experiment;
        Phase6Config phaseConfig;
        phaseConfig.neuronCount = neurons;
        phaseConfig.maxSteps = steps;
        phaseConfig.enableCheckpointing = true;
        phaseConfig.enableReplay = true;
        phaseConfig.enableDevelopment = true;
        
        std::cout << "=== NLM Phase 6 Demo ===" << std::endl;
        std::cout << "Running " << neurons << " neurons for " << steps << " steps..." << std::endl;
        
        auto result = experiment.run(phaseConfig);
        
        std::cout << "" << std::endl;
        std::cout << "=== Demo Complete ===" << std::endl;
        return 0;
        
    } else if (command == "verify") {
        printBanner();
        
        bool quick = false;
        for (int i = 2; i < argc; ++i) {
            if (std::string(argv[i]) == "--quick") {
                quick = true;
            }
        }
        
        // We need a brain for verification, create a simple one
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "ERROR: Failed to initialize brain for verification!" << std::endl;
            return 1;
        }
        
        BrainController controller(brain);
        controller.verifyIntegration(quick);
        
    } else if (command == "inspect") {
        printBanner();
        
        // We need a brain for inspection, create a simple one
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "ERROR: Failed to initialize brain for inspection!" << std::endl;
            return 1;
        }
        
        BrainController controller(brain);
        controller.inspectSystem();
        
    } else if (command == "help") {
        printBanner();
        printHelp();
    } else {
        std::cerr << "Unknown command: " << command << std::endl;
        std::cerr << "Type 'nlm help' for available commands." << std::endl;
        return 1;
    }
    
    return 0;
}
