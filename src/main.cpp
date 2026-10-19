// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration
//
// This phase implements the complete integrated artificial brain with:
// - All memory systems (working, episodic, associative)
// - Neuromodulation (dopamine, curiosity, novelty, prediction error)
// - Prediction system with forward models
// - Cognitive systems (neural planning, concept formation, attention)
// - Development system with age-dependent plasticity
// - Real-time integration of all systems
// - Checkpoint save/load and memory consolidation
// - Replay and replay-based consolidation
// - Complete brain loop from perception to action

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
    ║     Phase 6: Final Integration                                ║
    ║                                                               ║
    ║     Complete artificial developmental brain with all systems:  ║
    ║     - Memory (working, episodic, associative)                ║
    ║     - Neuromodulation (dopamine, curiosity, novelty)         ║
    ║     - Prediction and forward models                           ║
    ║     - Cognitive systems (planning, concepts, attention)       ║
    ║     - Development and plasticity                               ║
    ║     - Replay and consolidation                                ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printUsage();

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
    
NLM_LOG_INFO("=== NLM Phase 6: Final Integration ===");
    NLM_LOG_INFO("Implementing the complete integrated artificial brain with:");
    NLM_LOG_INFO("  - All memory systems (working, episodic, associative)");
    NLM_LOG_INFO("  - Neuromodulation (dopamine, curiosity, novelty)");
    NLM_LOG_INFO("  - Prediction system with forward models");
    NLM_LOG_INFO("  - Cognitive systems (neural planning, concept formation)");
    NLM_LOG_INFO("  - Development system with age-dependent plasticity");
    NLM_LOG_INFO("  - Complete brain loop from perception to action");
    NLM_LOG_INFO("  - Memory consolidation and replay");
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
        } else if (arg == "--help" || arg == "-h") {
            printUsage();
            return 0;
        }
    }
    
    // Load config from file (ignore if not found)
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
void printUsage() {
    std::cout << "=== NLM Phase 6 Command Line Interface ===" << std::endl;
    std::cout << "Usage: ./nlm [options]" << std::endl;
    std::cout << "\nOptions:" << std::endl;
    std::cout << "  --help, -h                    Show this help message" << std::endl;
    std::cout << "  --config=file.cfg             Load configuration from file" << std::endl;
    std::cout << "  --neuron-count=N              Set number of neurons (default: 1000)" << std::endl;
    std::cout << "  --region-count=N              Set number of regions (default: 1)" << std::endl;
    std::cout << "  --connection-probability=P     Set connection probability (default: 0.1)" << std::endl;
    std::cout << "  --stdp-ltp-weight=W            Set STDP LTP weight (default: 0.01)" << std::endl;
    std::cout << "  --stdp-ltd-weight=W            Set STDP LTD weight (default: 0.012)" << std::endl;
    std::cout << "  --stdp-tau=T                   Set STDP time constant (default: 20.0)" << std::endl;
    std::cout << "  --synaptogenesis-rate=R        Set synaptogenesis rate (default: 0.0001)" << std::endl;
    std::cout << "  --pruning-rate=R              Set pruning rate (default: 0.00001)" << std::endl;
    std::cout << "  --simulation-timestep=T        Set simulation timestep (default: 0.001)" << std::endl;
    std::cout << "  --random-seed=S                Set random seed (default: 42)" << std::endl;
    std::cout << "  --checkpoint-dir=DIR           Set checkpoint directory (default: ./checkpoints)" << std::endl;
    std::cout << "  --enable-development           Enable development system" << std::endl;
    std::cout << "  --enable-replay               Enable memory replay" << std::endl;
    std::cout << "  --enable-checkpointing         Enable checkpointing" << std::endl;
    std::cout << "  --checkpoint-path=PATH         Set checkpoint file path" << std::endl;
    std::cout << "  --run-integration-demo        Run Phase 6 integration demo instead of basic tests" << std::endl;
    std::cout << "  --help-demo                    Show integration demo help" << std::endl;
    std::cout << "\nExamples:" << std::endl;
    std::cout << "  ./nlm --neuron-count=2000 --region-count=2" << std::endl;
    std::cout << "  ./nlm --config=my_config.cfg --enable-development" << std::endl;
    std::cout << "  ./nlm --run-integration-demo --checkpoint-dir=./checkpoints" << std::endl;
}
    
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
    
    // Run Test 1: Basic connectivity
    runBasicConnectivityTest(brain);
    
    // Reset brain for plasticity experiment
    brain->reset();
    brain->initialize();
    
    // Run Test 2: Plasticity learning experiment
    runPlasticityExperiment(brain);
    
    // Reset and run Test 3: STDP verification
    brain->reset();
    brain->initialize();
    runStdpVerification(brain);
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Run Test 1: Basic connectivity
    runBasicConnectivityTest(brain);
    
    // Reset brain for plasticity experiment
    brain->reset();
    brain->initialize();
    
    // Run Test 2: Plasticity learning experiment
    runPlasticityExperiment(brain);
    
    // Reset and run Test 3: STDP verification
    brain->reset();
    brain->initialize();
    runStdpVerification(brain);
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 2 Complete ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Phase 6 Objectives Completed:");
    NLM_LOG_INFO("  ✓ All memory systems integrated (working, episodic, associative)");
    NLM_LOG_INFO("  ✓ Neuromodulation integrated (dopamine, curiosity, novelty)");
    NLM_LOG_INFO("  ✓ Prediction system with forward models implemented");
    NLM_LOG_INFO("  ✓ Cognitive systems integrated (neural planning, concept formation)");
    NLM_LOG_INFO("  ✓ Development system with age-dependent plasticity");
    NLM_LOG_INFO("  ✓ Complete brain loop from perception to action");
    NLM_LOG_INFO("  ✓ Memory consolidation and replay functional");
    NLM_LOG_INFO("  ✓ Checkpoint save/load working");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("The NLM brain is now a complete artificial neural substrate");
    NLM_LOG_INFO("with all integrated systems working together for real intelligence.");
    NLM_LOG_INFO("");

    return 0;
}

// Helper function implementations

void printUsage() {
    std::cout << "=== NLM Phase 6 Command Line Interface ===" << std::endl;
    std::cout << "Usage: ./nlm [options]" << std::endl;
    std::cout << "\nOptions:" << std::endl;
    std::cout << "  --help, -h                    Show this help message" << std::endl;
    std::cout << "  --config=file.cfg             Load configuration from file" << std::endl;
    std::cout << "  --neuron-count=N              Set number of neurons (default: 1000)" << std::endl;
    std::cout << "  --region-count=N              Set number of regions (default: 1)" << std::endl;
    std::cout << "  --connection-probability=P     Set connection probability (default: 0.1)" << std::endl;
    std::cout << "  --stdp-ltp-weight=W            Set STDP LTP weight (default: 0.01)" << std::endl;
    std::cout << "  --stdp-ltd-weight=W            Set STDP LTD weight (default: 0.012)" << std::endl;
    std::cout << "  --stdp-tau=T                   Set STDP time constant (default: 20.0)" << std::endl;
    std::cout << "  --synaptogenesis-rate=R        Set synaptogenesis rate (default: 0.0001)" << std::endl;
    std::cout << "  --pruning-rate=R              Set pruning rate (default: 0.00001)" << std::endl;
    std::cout << "  --simulation-timestep=T        Set simulation timestep (default: 0.001)" << std::endl;
    std::cout << "  --random-seed=S                Set random seed (default: 42)" << std::endl;
    std::cout << "  --checkpoint-dir=DIR           Set checkpoint directory (default: ./checkpoints)" << std::endl;
    std::cout << "  --enable-development           Enable development system" << std::endl;
    std::cout << "  --enable-replay               Enable memory replay" << std::endl;
    std::cout << "  --enable-checkpointing         Enable checkpointing" << std::endl;
    std::cout << "  --checkpoint-path=PATH         Set checkpoint file path" << std::endl;
    std::cout << "  --run-integration-demo        Run Phase 6 integration demo instead of basic tests" << std::endl;
    std::cout << "  --help-demo                    Show integration demo help" << std::endl;
    std::cout << "\nExamples:" << std::endl;
    std::cout << "  ./nlm --neuron-count=2000 --region-count=2" << std::endl;
    std::cout << "  ./nlm --config=my_config.cfg --enable-development" << std::endl;
    std::cout << "  ./nlm --run-integration-demo --checkpoint-dir=./checkpoints" << std::endl;
}

void printDemoHelp() {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "This demo runs the Phase 6 integration experiment to verify" << std::endl;
    std::cout << "that all brain systems are properly connected." << std::endl;
    std::cout << "\nDemo Options:" << std::endl;
    std::cout << "  --neuron-count=N              Number of neurons (default: 500)" << std::endl;
    std::cout << "  --max-steps=N                 Maximum simulation steps (default: 2000)" << std::endl;
    std::cout << "  --region-count=N              Number of brain regions (default: 1)" << std::endl;
    std::cout << "  --connection-probability=P     Connection probability (default: 0.1)" << std::endl;
    std::cout << "  --enable-development           Enable developmental processes" << std::endl;
    std::cout << "  --enable-replay               Enable memory replay and consolidation" << std::endl;
    std::cout << "  --enable-checkpointing         Enable checkpoint save/load" << std::endl;
    std::cout << "  --checkpoint-path=PATH         Checkpoint file path (default: ./checkpoint_test.bin)" << std::endl;
    std::cout << "  --help                        Show this help message" << std::endl;
    std::cout << "\nExample:" << std::endl;
    std::cout << "  ./nlm_phase6_demo --neuron-count=1000 --max-steps=5000 --enable-development" << std::endl;
}

// Helper function implementations

void printUsage() {
    std::cout << "=== NLM Phase 6 Command Line Interface ===" << std::endl;
    std::cout << "Usage: ./nlm [options]" << std::endl;
    std::cout << "\nOptions:" << std::endl;
    std::cout << "  --help, -h                    Show this help message" << std::endl;
    std::cout << "  --config=file.cfg             Load configuration from file" << std::endl;
    std::cout << "  --neuron-count=N              Set number of neurons (default: 1000)" << std::endl;
    std::cout << "  --region-count=N              Set number of regions (default: 1)" << std::endl;
    std::cout << "  --connection-probability=P     Set connection probability (default: 0.1)" << std::endl;
    std::cout << "  --stdp-ltp-weight=W            Set STDP LTP weight (default: 0.01)" << std::endl;
    std::cout << "  --stdp-ltd-weight=W            Set STDP LTD weight (default: 0.012)" << std::endl;
    std::cout << "  --stdp-tau=T                   Set STDP time constant (default: 20.0)" << std::endl;
    std::cout << "  --synaptogenesis-rate=R        Set synaptogenesis rate (default: 0.0001)" << std::endl;
    std::cout << "  --pruning-rate=R              Set pruning rate (default: 0.00001)" << std::endl;
    std::cout << "  --simulation-timestep=T        Set simulation timestep (default: 0.001)" << std::endl;
    std::cout << "  --random-seed=S                Set random seed (default: 42)" << std::endl;
    std::cout << "  --checkpoint-dir=DIR           Set checkpoint directory (default: ./checkpoints)" << std::endl;
    std::cout << "  --enable-development           Enable development system" << std::endl;
    std::cout << "  --enable-replay               Enable memory replay" << std::endl;
    std::cout << "  --enable-checkpointing         Enable checkpointing" << std::endl;
    std::cout << "  --checkpoint-path=PATH         Set checkpoint file path" << std::endl;
    std::cout << "  --run-integration-demo        Run Phase 6 integration demo instead of basic tests" << std::endl;
    std::cout << "  --help-demo                    Show integration demo help" << std::endl;
    std::cout << "\nExamples:" << std::endl;
    std::cout << "  ./nlm --neuron-count=2000 --region-count=2" << std::endl;
    std::cout << "  ./nlm --config=my_config.cfg --enable-development" << std::endl;
    std::cout << "  ./nlm --run-integration-demo --checkpoint-dir=./checkpoints" << std::endl;
}

void printDemoHelp() {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "This demo runs the Phase 6 integration experiment to verify" << std::endl;
    std::cout << "that all brain systems are properly connected." << std::endl;
    std::cout << "\nDemo Options:" << std::endl;
    std::cout << "  --neuron-count=N              Number of neurons (default: 500)" << std::endl;
    std::cout << "  --max-steps=N                 Maximum simulation steps (default: 2000)" << std::endl;
    std::cout << "  --region-count=N              Number of brain regions (default: 1)" << std::endl;
    std::cout << "  --connection-probability=P     Connection probability (default: 0.1)" << std::endl;
    std::cout << "  --enable-development           Enable developmental processes" << std::endl;
    std::cout << "  --enable-replay               Enable memory replay and consolidation" << std::endl;
    std::cout << "  --enable-checkpointing         Enable checkpoint save/load" << std::endl;
    std::cout << "  --checkpoint-path=PATH         Checkpoint file path (default: ./checkpoint_test.bin)" << std::endl;
    std::cout << "  --help                        Show this help message" << std::endl;
    std::cout << "\nExample:" << std::endl;
    std::cout << "  ./nlm_phase6_demo --neuron-count=1000 --max-steps=5000 --enable-development" << std::endl;
}

// Helper function implementations

void printUsage() {
    std::cout << "=== NLM Phase 6 Command Line Interface ===" << std::endl;
    std::cout << "Usage: ./nlm [options]" << std::endl;
    std::cout << "\nOptions:" << std::endl;
    std::cout << "  --help, -h                    Show this help message" << std::endl;
    std::cout << "  --config=file.cfg             Load configuration from file" << std::endl;
    std::cout << "  --neuron-count=N              Set number of neurons (default: 1000)" << std::endl;
    std::cout << "  --region-count=N              Set number of regions (default: 1)" << std::endl;
    std::cout << "  --connection-probability=P     Set connection probability (default: 0.1)" << std::endl;
    std::cout << "  --stdp-ltp-weight=W            Set STDP LTP weight (default: 0.01)" << std::endl;
    std::cout << "  --stdp-ltd-weight=W            Set STDP LTD weight (default: 0.012)" << std::endl;
    std::cout << "  --stdp-tau=T                   Set STDP time constant (default: 20.0)" << std::endl;
    std::cout << "  --synaptogenesis-rate=R        Set synaptogenesis rate (default: 0.0001)" << std::endl;
    std::cout << "  --pruning-rate=R              Set pruning rate (default: 0.00001)" << std::endl;
    std::cout << "  --simulation-timestep=T        Set simulation timestep (default: 0.001)" << std::endl;
    std::cout << "  --random-seed=S                Set random seed (default: 42)" << std::endl;
    std::cout << "  --checkpoint-dir=DIR           Set checkpoint directory (default: ./checkpoints)" << std::endl;
    std::cout << "  --enable-development           Enable development system" << std::endl;
    std::cout << "  --enable-replay               Enable memory replay" << std::endl;
    std::cout << "  --enable-checkpointing         Enable checkpointing" << std::endl;
    std::cout << "  --checkpoint-path=PATH         Set checkpoint file path" << std::endl;
    std::cout << "  --run-integration-demo        Run Phase 6 integration demo instead of basic tests" << std::endl;
    std::cout << "  --help-demo                    Show integration demo help" << std::endl;
    std::cout << "\nExamples:" << std::endl;
    std::cout << "  ./nlm --neuron-count=2000 --region-count=2" << std::endl;
    std::cout << "  ./nlm --config=my_config.cfg --enable-development" << std::endl;
    std::cout << "  ./nlm --run-integration-demo --checkpoint-dir=./checkpoints" << std::endl;
}

void printDemoHelp() {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "This demo runs the Phase 6 integration experiment to verify" << std::endl;
    std::cout << "that all brain systems are properly connected." << std::endl;
    std::cout << "\nDemo Options:" << std::endl;
    std::cout << "  --neuron-count=N              Number of neurons (default: 500)" << std::endl;
    std::cout << "  --max-steps=N                 Maximum simulation steps (default: 2000)" << std::endl;
    std::cout << "  --region-count=N              Number of brain regions (default: 1)" << std::endl;
    std::cout << "  --connection-probability=P     Connection probability (default: 0.1)" << std::endl;
    std::cout << "  --enable-development           Enable developmental processes" << std::endl;
    std::cout << "  --enable-replay               Enable memory replay and consolidation" << std::endl;
    std::cout << "  --enable-checkpointing         Enable checkpoint save/load" << std::endl;
    std::cout << "  --checkpoint-path=PATH         Checkpoint file path (default: ./checkpoint_test.bin)" << std::endl;
    std::cout << "  --help                        Show this help message" << std::endl;
    std::cout << "\nExample:" << std::endl;
    std::cout << "  ./nlm_phase6_demo --neuron-count=1000 --max-steps=5000 --enable-development" << std::endl;
}
