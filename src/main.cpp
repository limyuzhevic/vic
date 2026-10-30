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

void printAdvancedUsage() {
    std::cout << R"(
=== NLM PHASE 6 ADVANCED USAGE ===

COMMANDS:
  nlm [options]                    Run NLM Phase 6 Integrated Artificial Brain
  nlm --help                      Show basic usage
  nlm --advanced-help              Show advanced options and examples

BASIC OPTIONS:
  --config <file>                 Configuration file path (default: configs/default.cfg)
  --help                          Show this help message
  --advanced-help                  Show advanced usage and examples

ADVANCED COMMAND-LINE OPTIONS:

MEMORY SYSTEM CONTROL:
  --memory-capacity <size>         Working memory capacity (default: 500 from config)
  --episodic-episodes <size>       Episodic memory episode limit (default: 5000 from config)
  --consolidation-threshold <float> Memory consolidation threshold (default: 0.3)

PREDICTION SYSTEM CONTROL:
  --prediction-enabled <bool>       Enable prediction system (default: true)
  --prediction-history <size>       Prediction history size (default: 1000)
  --prediction-error-threshold <f>  Prediction error threshold (default: 0.1)

COGNITION SYSTEM CONTROL:
  --neural-planner <depth>          Neural planner depth (default: 5)
  --concept-formation <bool>        Enable concept formation (default: true)
  --attention-system <bool>         Enable attention system (default: true)
  --attention-inhibition <f>        Attention inhibition strength (default: 0.5)
  --attention-excitation <f>        Attention excitation strength (default: 1.5)

DEVELOPMENTAL CONTROL:
  --development-stage <stage>       Set developmental stage (Initial|CriticalPeriod|Maturation|Adult)
  --plasticity-modulation <f>       Development plasticity modulation (default: 0.5)
  --development-interval <steps>    Development update interval (default: 1000)

NEUROMODULATION CONTROL:
  --dopamine-level <f>              Dopamine baseline level (default: 0.1)
  --curiosity-threshold <f>         Curiosity threshold (default: 0.3)
  --novelty-sensitivity <f>         Novelty sensitivity (default: 0.8)
  --prediction-error-weight <f>     Prediction error signal weight (default: 1.0)

ENVIRONMENTAL PARAMETERS:
  --environment <name>              Environment name (default: IntegratedEnvironment)
  --exploration-rate <f>            Exploration rate (default: 0.1)
  --exploitation-rate <f>           Exploitation rate (default: 0.9)
  --reward-discount <f>             Reward discount factor (default: 0.995)

CHECKPOINTING OPTIONS:
  --checkpoint-dir <path>            Checkpoint directory (default: ./checkpoints_phase6)
  --checkpoint-interval <steps>      Checkpoint saving interval (default: 5000)
  --checkpoint-compression <mode>    Checkpoint compression (Balanced|Fast|Best)
  --save-brain <file>               Save brain state to file and exit
  --load-brain <file>               Load brain state from file and exit

EXPERIMENTAL MODES:
  --experiment <mode>               Run specific experiment:
                                     memory-test | prediction-test | cognition-test |
                                     development-test | full-integration | neural-planner-test
  --experiment-mode <mode>           Experiment execution mode: simulate | record | benchmark
  --experiment-duration <steps>      Experiment duration in steps (default: 1000)

VISUALIZATION OPTIONS:
  --visualization <bool>            Enable visualization (default: true)
  --visualization-output <file>      Visualization output file (default: brain_viz.mp4)
  --visualization-update-rate <f>    Visualization update rate (default: 60.0 Hz)
  --visualize-network <bool>         Enable network visualization (default: true)
  --visualize-spikes <bool>          Enable spike raster visualization (default: true)
  --visualize-weights <bool>          Enable weight matrix visualization (default: false)
  --visualization-quality <quality>   Visualization quality (low|medium|high)

PERFORMANCE OPTIONS:
  --performance-monitoring <bool>    Enable performance monitoring (default: true)
  --performance-sampling <rate>      Performance sampling rate (default: 100 Hz)
  --memory-pool-size <size>          Memory pool size (default: 100000)
  --event-queue-size <size>          Event queue size (default: 10000)

LOGGING OPTIONS:
  --log-level <level>               Logging level (DEBUG|INFO|WARNING|ERROR)
  --log-to-file <bool>               Enable file logging (default: true)
  --log-file <path>                  Log file path (default: nlm_phase6.log)
  --log-rotation-size <bytes>        Log rotation size in bytes (default: 10485760 = 10MB)
  --log-memory-debug <bool>          Enable memory system debug logging (default: false)
  --log-prediction-debug <bool>      Enable prediction system debug logging (default: false)
  --log-cognition-debug <bool>       Enable cognition system debug logging (default: false)
  --log-neuromodulation-debug <bool>  Enable neuromodulation system debug logging (default: false)

DEBUGGING OPTIONS:
  --debug-mode                      Enable debug mode with enhanced output (default: false)
  --debug-spike-timing              Debug spike timing and propagation (default: false)
  --debug-plasticity <type>         Debug specific plasticity: STDP|Hebbian|Structural (default: none)
  --debug-memory-neurons <ids>       Debug working memory for neurons (comma-separated IDs)
  --debug-step-range <range>         Debug specific step range: start-end (default: none)
  --debug-verbose                   Enable verbose debug output

CONFIGURATION OVERRIDE:
  --config-key <key>=<value>          Override configuration key with value
  --config-file <file>               Alternative configuration file

PRESET MODES:
  --quick-test                      Fast test mode (reduced neurons, shorter duration)
  --full-test                       Complete integration test (full scale)
  --learning-test                   Focus on learning and plasticity demonstration
  --memory-test                     Focus on memory system capabilities
  --prediction-test                 Focus on prediction system capabilities
  --development-test                Focus on developmental progression
  --cognition-test                  Focus on cognitive system integration

EXAMPLES:
  # Quick integration test
  nlm --quick-test --visualization
  
  # Save brain state for later loading
  nlm --save-brain mybrain.checkpoint --development-stage Adult
  
  # Load and continue training
  nlm --load-brain mybrain.checkpoint --experiment learning-test
  
  # Run prediction system benchmark
  nlm --experiment prediction-test --experiment-mode benchmark --experiment-duration 5000
  
  # Debug memory system with visualization
  nlm --debug-memory-neurons 1,2,3,4,5 --visualization --log-memory-debug
  
  # Development stage demonstration
  nlm --development-stage Initial --plasticity-modulation 1.0
  nlm --development-stage CriticalPeriod --plasticity-modulation 0.8
  nlm --development-stage Maturation --plasticity-modulation 0.5
  nlm --development-stage Adult --plasticity-modulation 0.2
  
  # Exploration vs exploitation balance
  nlm --exploration-rate 0.3 --exploitation-rate 0.7
  
  # High-performance visualization
  nlm --visualization --visualize-network --visualize-spikes --visualization-quality high
  
  # Comprehensive logging for research
  nlm --log-level DEBUG --log-to-file --log-prediction-debug --log-cognition-debug
  
  # Performance benchmarking
  nlm --experiment full-integration --experiment-mode benchmark --performance-monitoring
  
)" << std::endl;
}
