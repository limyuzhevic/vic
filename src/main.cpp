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

// Utility functions
void printUsage() {
    std::cout << "\n";
    std::cout << "USAGE: ./nlm [OPTIONS]\n" << std::endl;
    std::cout << "\n";
    std::cout << "REQUIRED ARGUMENTS:\n" << std::endl;
    std::cout << "  --help                  Show this help message\n" << std::endl;
    std::cout << "\n";
    std::cout << "CONFIGURATION OPTIONS:\n" << std::endl;
    std::cout << "  --config=FILE           Load configuration from file (default: configs/default.cfg)\n" << std::endl;
    std::cout << "  --random-seed=N         Set random seed (default: 42)\n" << std::endl;
    std::cout << "  --neuron-count=N        Number of neurons (default: 500)\n" << std::endl;
    std::cout << "  --region-count=N        Number of regions (default: 1)\n" << std::endl;
    std::cout << "  --connection-prob=N     Probability of synaptic connections (default: 0.15)\n" << std::endl;
    std::cout << "  --timestep=N            Simulation timestep in seconds (default: 0.001)\n" << std::endl;
    std::cout << "\n";
    std::cout << "NEURAL PARAMETERS:\n" << std::endl;
    std::cout << "  --stdp- LTP-weight=N    STDP Long-Term Potentiation weight (default: 0.02)\n" << std::endl;
    std::cout << "  --stdp- LTD-weight=N    STDP Long-Term Depression weight (default: 0.015)\n" << std::endl;
    std::cout << "  --stdp- tau=N           STDP time constant (default: 20.0)\n" << std::endl;
    std::cout << "  --synaptogenesis-rate=N Structural synaptogenesis rate (default: 0.0001)\n" << std::endl;
    std::cout << "  --pruning-rate=N        Structural pruning rate (default: 0.00001)\n" << std::endl;
    std::cout << "\n";
    std::cout << "BEHAVIORAL PARAMETERS:\n" << std::endl;
    std::cout << "  --replay-interval=N     Memory replay interval (default: 100)\n" << std::endl;
    std::cout << "  --consolidation-interval=N Memory consolidation interval (default: 1000)\n" << std::endl;
    std::cout << "\n";
    std::cout << "EXPERIMENT OPTIONS:\n" << std::endl;
    std::cout << "  --run-test=N            Run specific test (1=basic connectivity, 2=plasticity, 3=STDP, 4=all)\n" << std::endl;
    std::cout << "  --experiment-mode=STRING Experiment mode: \"phase2\", \"phase5\", \"phase6\" (default: phase2)\n" << std::endl;
    std::cout << "  --enable-checkpoint      Enable checkpoint saving\n" << std::endl;
    std::cout << "  --checkpoint-file=FILE   Checkpoint file path\n" << std::endl;
    std::cout << "\n";
    std::cout << "DEBUGGING OPTIONS:\n" << std::endl;
    std::cout << "  --log-level=N           Log level: 0=Debug, 1=Info, 2=Warning, 3=Error, 4=Critical (default: 1)\n" << std::endl;
    std::cout << "  --enable-colors         Enable colored console output\n" << std::endl;
    std::cout << "  --log-to-file=FILE      Write logs to file in addition to console\n" << std::endl;
    std::cout << "  --profile               Enable performance profiling\n" << std::endl;
    std::cout << "\n";
    std::cout << "ADVANCED OPTIONS:\n" << std::endl;
    std::cout << "  --threads=N             Number of threads for parallel processing (default: 1)\n" << std::endl;
    std::cout << "  --memory-limit=BYTES    Memory limit for neural structures\n" << std::endl;
    std::cout << "  --enable-experimental    Enable experimental features\n" << std::endl;
    std::cout << "\n";
    std::cout << "EXAMPLES:\n" << std::endl;
    std::cout << "  ./nlm --help\n" << std::endl;
    std::cout << "  ./nlm --neuron-count=1000 --timestep=0.002\n" << std::endl;
    std::cout << "  ./nlm --config=myconfig.cfg --run-test=4 --enable-checkpoint\n" << std::endl;
    std::cout << "\n";
}

// Enhanced argument parsing with more options
void parseAdvancedArgs(int argc, char** argv, std::shared_ptr<Config> config) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help") {
            printUsage();
            exit(0);
        }
        else if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                config->set("config_file", arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                config->set("config_file", argv[++i]);
            }
        }
        else if (arg == "--enable-checkpoint") {
            config->set("enable_checkpoint", true);
        }
        else if (arg.substr(0, 17) == "--checkpoint-file") {
            if (arg.find('=') != std::string::npos) {
                config->set("checkpoint_file", arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                config->set("checkpoint_file", argv[++i]);
            }
        }
        else if (arg.substr(0, 14) == "--log-to-file") {
            if (arg.find('=') != std::string::npos) {
                config->set("log_file", arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                config->set("log_file", argv[++i]);
            }
        }
        else if (arg == "--enable-colors") {
            config->set("enable_colors", true);
        }
        else if (arg == "--profile") {
            config->set("enable_profiling", true);
        }
        else if (arg == "--enable-experimental") {
            config->set("enable_experimental", true);
        }
        else if (arg.substr(0, 14) == "--log-level") {
            int level = 1; // default
            if (arg.find('=') != std::string::npos) {
                level = std::stoi(arg.substr(arg.find('=') + 1));
            }
            config->set("log_level", level);
        }
        else if (arg.substr(0, 15) == "--random-seed") {
            uint64_t seed = 42;
            if (arg.find('=') != std::string::npos) {
                seed = static_cast<uint64_t>(std::stoull(arg.substr(arg.find('=') + 1)));
            }
            config->set("random_seed", static_cast<int64_t>(seed));
        }
        else if (arg.substr(0, 15) == "--neuron-count") {
            size_t count = 500;
            if (arg.find('=') != std::string::npos) {
                count = static_cast<size_t>(std::stoull(arg.substr(arg.find('=') + 1)));
            }
            config->set("neuron_count", static_cast<int64_t>(count));
        }
        else if (arg.substr(0, 14) == "--region-count") {
            size_t count = 1;
            if (arg.find('=') != std::string::npos) {
                count = static_cast<size_t>(std::stoull(arg.substr(arg.find('=') + 1)));
            }
            config->set("region_count", static_cast<int64_t>(count));
        }
        else if (arg.substr(0, 19) == "--connection-prob") {
            float prob = 0.15f;
            if (arg.find('=') != std::string::npos) {
                prob = std::stof(arg.substr(arg.find('=') + 1));
            }
            config->set("connection_probability", prob);
        }
        else if (arg.substr(0, 11) == "--timestep") {
            double timestep = 0.001;
            if (arg.find('=') != std::string::npos) {
                timestep = std::stod(arg.substr(arg.find('=') + 1));
            }
            config->set("simulation_timestep", timestep);
        }
        else if (arg.substr(0, 15) == "--stdp-LTP-weight") {
            float weight = 0.02f;
            if (arg.find('=') != std::string::npos) {
                weight = std::stof(arg.substr(arg.find('=') + 1));
            }
            config->set("stdp_ltp_weight", weight);
        }
        else if (arg.substr(0, 16) == "--stdp-LTD-weight") {
            float weight = 0.015f;
            if (arg.find('=') != std::string::npos) {
                weight = std::stof(arg.substr(arg.find('=') + 1));
            }
            config->set("stdp_ltd_weight", weight);
        }
        else if (arg.substr(0, 10) == "--stdp-tau") {
            float tau = 20.0f;
            if (arg.find('=') != std::string::npos) {
                tau = std::stof(arg.substr(arg.find('=') + 1));
            }
            config->set("stdp_tau", tau);
        }
        else if (arg.substr(0, 22) == "--synaptogenesis-rate") {
            float rate = 0.0001f;
            if (arg.find('=') != std::string::npos) {
                rate = std::stof(arg.substr(arg.find('=') + 1));
            }
            config->set("synaptogenesis_rate", rate);
        }
        else if (arg.substr(0, 16) == "--pruning-rate") {
            float rate = 0.00001f;
            if (arg.find('=') != std::string::npos) {
                rate = std::stof(arg.substr(arg.find('=') + 1));
            }
            config->set("pruning_rate", rate);
        }
        else if (arg.substr(0, 18) == "--replay-interval") {
            size_t interval = 100;
            if (arg.find('=') != std::string::npos) {
                interval = static_cast<size_t>(std::stoull(arg.substr(arg.find('=') + 1)));
            }
            config->set("replay_interval", interval);
        }
        else if (arg.substr(0, 26) == "--consolidation-interval") {
            size_t interval = 1000;
            if (arg.find('=') != std::string::npos) {
                interval = static_cast<size_t>(std::stoull(arg.substr(arg.find('=') + 1)));
            }
            config->set("consolidation_interval", interval);
        }
        else if (arg.substr(0, 12) == "--run-test") {
            int test = 4;  // default: run all tests
            if (arg.find('=') != std::string::npos) {
                test = std::stoi(arg.substr(arg.find('=') + 1));
            }
            config->set("run_test", test);
        }
        else if (arg.substr(0, 20) == "--experiment-mode") {
            std::string mode = "phase2";
            if (arg.find('=') != std::string::npos) {
                mode = arg.substr(arg.find('=') + 1);
            }
            config->set("experiment_mode", mode);
        }
        else if (arg.substr(0, 12) == "--threads") {
            int threads = 1;
            if (arg.find('=') != std::string::npos) {
                threads = std::stoi(arg.substr(arg.find('=') + 1));
            }
            config->set("thread_count", threads);
        }
    }
}
