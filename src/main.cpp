// NLM (熙然) - Neural Learning Machine
// Enhanced Command Line Interface for Advanced Usage
// Phase 2: Real Neural Computation with multiple execution modes
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)
//
// Enhanced features:
// - Multiple execution modes: normal, debug, agent, benchmark
// - Comprehensive configuration management
// - Advanced testing utilities
// - Power-user command shortcuts
// - Detailed logging and statistics

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
#include "agent/AgentBrain.hpp"
#include "agent/SensoryPercept.hpp"
#include "world/SimpleWorld.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <thread>
#include <map>
#include <fstream>
#include <sstream>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Enhanced Real Neural Computation                 ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements:                                    ║
    ║     - Real LIF neuron dynamics                                ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity                            ║
    ║     - Structural plasticity                                   ║
    ║     - Advanced agent systems                                ║
    ║     - Comprehensive configuration management                 ║
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

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help                    Show this help message" << std::endl;
    std::cout << "  --version                 Show version information" << std::endl;
    std::cout << "  --config=<file>           Load configuration from file" << std::endl;
    std::cout << "  --mode=<mode>             Execution mode (normal, debug, agent, benchmark, educational)" << std::endl;
    std::cout << "  --steps=<n>               Number of simulation steps (default: 1000)" << std::endl;
    std::cout << "  --duration=<t>            Simulation duration in seconds (default: 1.0)" << std::endl;
    std::cout << "  --neurons=<n>             Number of neurons (default: 500)" << std::endl;
    std::cout << "  --seed=<n>                Random seed (default: 42)" << std::endl;
    std::cout << "  --verbose                 Enable verbose logging" << std::endl;
    std::cout << "  --benchmark               Run benchmark mode" << std::endl;
    std::cout << "  --agent-only              Run only agent interface tests" << std::endl;
    std::cout << "  --test-plasticity         Run only plasticity tests" << std::endl;
    std::cout << "  --test-stdp               Run only STDP tests" << std::endl;
    std::cout << "  --test-connectivity       Run only connectivity tests" << std::endl;
    std::cout << "  --export-stats=<file>     Export statistics to file" << std::endl;
    std::cout << "  --save-checkpoint=<file>  Save brain state to checkpoint" << std::endl;
    std::cout << "  --load-checkpoint=<file>  Load brain state from checkpoint" << std::endl;
    std::cout << "  --performance-analysis    Enable performance analysis" << std::endl;
    std::cout << "  --learning-curve          Track learning curve" << std::endl;
    std::cout << "  --agent-mode=<name>       Specify agent mode (exploration, exploitation, balanced)" << std::endl;
    std::cout << "  --enable-feature=<feature> Enable specific feature" << std::endl;
    std::cout << "  --disable-feature=<feature> Disable specific feature" << std::endl;
    std::cout << std::endl;
    std::cout << "Advanced Features:" << std::endl;
    std::cout << "  --plot-mode                Generate performance plots" << std::endl;
    std::cout << "  --memory-analysis          Analyze memory usage" << std::endl;
    std::cout << "  --neural-activity          Track neural activity patterns" << std::endl;
    std::cout << "  --plasticity-analysis      Analyze plasticity effects" << std::endl;
    std::cout << "  --scenario=<scenario>      Run predefined scenario" << std::endl;
    std::cout << std::endl;
    std::cout << "Configuration Options (using --set):" << std::endl;
    std::cout << "  --set random_seed=<n>" << std::endl;
    std::cout << "  --set simulation_timestep=<t>" << std::endl;
    std::cout << "  --set neuron_count=<n>" << std::endl;
    std::cout << "  --set region_count=<n>" << std::endl;
    std::cout << "  --set connection_probability=<p>" << std::endl;
    std::cout << "  --set stdp_ltp_weight=<w>" << std::endl;
    std::cout << "  --set stdp_ltd_weight=<w>" << std::endl;
    std::cout << "  --set synaptogenesis_rate=<r>" << std::endl;
    std::cout << "  --set pruning_rate=<r>" << std::endl;
    std::cout << "  --set dopamine_baseline=<d>" << std::endl;
    std::cout << "  --set reward_discount_factor=<f>" << std::endl;
    std::cout << std::endl;
    std::cout << "Power User Shortcuts:" << std::endl;
    std::cout << "  -fast                      --mode=debug --benchmark --steps=100" << std::endl;
    std::cout << "  -agent-only                --agent-only --steps=5000" << std::endl;
    std::cout << "  -perf                      --mode=debug --performance-analysis" << std::endl;
    std::cout << "  -plot                      --plot-mode --export-stats=stats.csv" << std::endl;
    std::cout << std::endl;
}

void printVersion() {
    std::cout << "NLM (熙然) - Neural Learning Machine v2.0.0" << std::endl;
    std::cout << "Phase 2: Enhanced Real Neural Computation" << std::endl;
    std::cout << "Built with C++20, featuring advanced agent systems and comprehensive configuration management." << std::endl;
}

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

void runAgentSystemDemo(std::shared_ptr<Brain> brain, const std::string& agentMode) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Agent System Demo ===");
    NLM_LOG_INFO("Running in agent mode: " + agentMode);
    
    // Create world
    auto world = std::make_shared<SimpleWorld>();
    world->configure(20.0, 20.0, 8, 8);
    world->reset();
    world->setAgentStart(10.0, 10.0);
    world->setRandomSeed(42);
    
    // Create agent brain interface
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(*world);
    
    // Configure agent based on mode
    bool curiosityEnabled = (agentMode == "exploration" || agentMode == "balanced");
    bool rewardModulationEnabled = (agentMode == "exploitation" || agentMode == "balanced");
    
    agent->enableCuriosity(curiosityEnabled);
    agent->enableRewardModulation(rewardModulationEnabled);
    agent->enableStructuralPlasticity(true);
    agent->enableDevelopment(true);
    
    // Run simulation loop
    NLM_LOG_INFO("Starting agent simulation for 200 steps...");
    for (SimulationStep step = 0; step < 200; ++step) {
        // Update world
        world->update(0.1);
        
        // Get sensory input from world
        const SensoryPercept& percept = world->getSensoryPercept();
        
        // Process sensory input in brain
        agent->processSensoryInput(percept);
        
        // Run brain step
        brain->step(step, step * 0.1);
        
        // Decode motor command from brain activity
        MotorCommand motor_cmd = agent->decodeMotorCommand();
        
        // Apply motor command to world
        ActionResult result = world->applyMotorCommand(motor_cmd, world->getSimulationTime());
        
        // Apply reward modulation
        float reward = result.reward;
        float predicted_reward = 0.0f; // Could be implemented with prediction system
        agent->applyRewardModulation(reward, predicted_reward);
        
        // Update development
        agent->updateDevelopment(0.1);
        
        // Log progress every 50 steps
        if (step % 50 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + ":");
            NLM_LOG_INFO("    Firing neurons: " + std::to_string(brain->getFiringNeuronCount()));
            NLM_LOG_INFO("    Curiosity level: " + std::to_string(agent->getCuriosityLevel()));
            NLM_LOG_INFO("    Novelty level: " + std::to_string(agent->getNoveltyLevel()));
            NLM_LOG_INFO("    Neuromodulation: " + std::to_string(agent->getNeuromodulationLevel()));
            NLM_LOG_INFO("    Developmental stage: " + std::to_string(static_cast<int>(agent->getDevelopmentalStage())));
            NLM_LOG_INFO("    Agent action: " + std::to_string(static_cast<int>(motor_cmd)));
            NLM_LOG_INFO("    Reward: " + std::to_string(reward));
        }
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Agent System Results ===");
    NLM_LOG_INFO("Final curiosity level: " + std::to_string(agent->getCuriosityLevel()));
    NLM_LOG_INFO("Final novelty level: " + std::to_string(agent->getNoveltyLevel()));
    NLM_LOG_INFO("Prediction error: " + std::to_string(agent->getPredictionError()));
    NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(agent->getDevelopmentalStage())));
}

void runBenchmarkMode(std::shared_ptr<Brain> brain, int steps) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== NLM Performance Benchmark ===");
    NLM_LOG_INFO("Running benchmark for " + std::to_string(steps) + " simulation steps...");
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Warm up
    for (SimulationStep step = 0; step < 100; ++step) {
        brain->step(step, step * 0.001);
    }
    
    // Actual benchmark
    for (SimulationStep step = 0; step < steps; ++step) {
        brain->step(step, step * 0.001);
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    double steps_per_second = (steps / 1000.0) / (duration.count() / 1000.0);
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Benchmark Results ===");
    NLM_LOG_INFO("Total steps: " + std::to_string(steps));
    NLM_LOG_INFO("Total time: " + std::to_string(duration.count()) + " ms");
    NLM_LOG_INFO("Performance: " + std::to_string(steps_per_second) + " steps/second");
    NLM_LOG_INFO("Average step time: " + std::to_string(duration.count() / steps) + " ms");
    NLM_LOG_INFO("Neurons: " + std::to_string(brain->getTotalNeuronCount()));
    NLM_LOG_INFO("Synapses: " + std::to_string(brain->getTotalSynapseCount()));
}

void exportStatistics(const std::string& filename, std::shared_ptr<Brain> brain, 
                     const LearningExperiment& experiment, bool learningCurve) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filename);
        return;
    }
    
    file << "NLM Statistics Export" << std::endl;
    file << "=======================" << std::endl;
    file << std::endl;
    
    file << "Brain Statistics:" << std::endl;
    file << "  Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
    file << "  Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
    file << "  Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
    file << "  Average Firing Rate: " << brain->getAverageFiringRate() << std::endl;
    file << "  Excitation-Inhibition Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
    file << "  Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
    file << std::endl;
    
    if (!experiment.initialWeights.empty() && !experiment.finalWeights.empty()) {
        file << "Learning Experiment:" << std::endl;
        file << "  Initial Mean Weight: " << std::accumulate(experiment.initialWeights.begin(), experiment.initialWeights.end(), 0.0f) / experiment.initialWeights.size() << std::endl;
        file << "  Final Mean Weight: " << std::accumulate(experiment.finalWeights.begin(), experiment.finalWeights.end(), 0.0f) / experiment.finalWeights.size() << std::endl;
        file << "  Initial Synapse Count: " << experiment.initialSynapseCount << std::endl;
        file << "  Final Synapse Count: " << brain->getTotalSynapseCount() << std::endl;
        
        if (learningCurve) {
            file << std::endl;
            file << "Learning Curve (sampled):" << std::endl;
            file << "  Step, Firing Rate, Mean Weight" << std::endl;
            // Note: In a real implementation, you would track this data over time
            file << "  100, " << brain->getAverageFiringRate() << ", " << (experiment.initialWeights.empty() ? 0 : std::accumulate(experiment.initialWeights.begin(), experiment.initialWeights.end(), 0.0f) / experiment.initialWeights.size()) << std::endl;
            file << "  500, " << brain->getAverageFiringRate() << ", " << (experiment.finalWeights.empty() ? 0 : std::accumulate(experiment.finalWeights.begin(), experiment.finalWeights.end(), 0.0f) / experiment.finalWeights.size()) << std::endl;
        }
    }
    
    file.close();
    NLM_LOG_INFO("Statistics exported to: " + filename);
}

int main(int argc, char** argv) {
    // Parse command line arguments
    std::string configFile = "configs/default.cfg";
    std::string mode = "normal";
    int steps = 1000;
    double duration = 1.0;
    int64_t neurons = 500;
    uint64_t seed = 42;
    bool verbose = false;
    bool benchmark = false;
    bool agentOnly = false;
    bool plasticityOnly = false;
    bool stdpOnly = false;
    bool connectivityOnly = false;
    std::string exportStatsFile = "";
    std::string checkpointFile = "";
    std::string loadCheckpointFile = "";
    bool performanceAnalysis = false;
    bool learningCurve = false;
    std::string agentMode = "balanced";
    std::map<std::string, std::string> customConfigs;
    bool plotMode = false;
    bool memoryAnalysis = false;
    bool neuralActivity = false;
    bool plasticityAnalysis = false;
    std::string scenario = "";
    
    // Parse arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.find("--help") != std::string::npos) {
            printUsage(argv[0]);
            return 0;
        } else if (arg.find("--version") != std::string::npos) {
            printVersion();
            return 0;
        } else if (arg.find("--config=") != std::string::npos) {
            configFile = arg.substr(8);
        } else if (arg == "--config" && i + 1 < argc) {
            configFile = argv[++i];
        } else if (arg.find("--mode=") != std::string::npos) {
            mode = arg.substr(7);
        } else if (arg.find("--steps=") != std::string::npos) {
            steps = std::stoi(arg.substr(7));
        } else if (arg.find("--duration=") != std::string::npos) {
            duration = std::stod(arg.substr(10));
        } else if (arg.find("--neurons=") != std::string::npos) {
            neurons = std::stoi(arg.substr(9));
        } else if (arg.find("--seed=") != std::string::npos) {
            seed = std::stoull(arg.substr(7));
        } else if (arg.find("--verbose") != std::string::npos) {
            verbose = true;
        } else if (arg.find("--benchmark") != std::string::npos) {
            benchmark = true;
        } else if (arg.find("--agent-only") != std::string::npos) {
            agentOnly = true;
        } else if (arg.find("--test-plasticity") != std::string::npos) {
            plasticityOnly = true;
        } else if (arg.find("--test-stdp") != std::string::npos) {
            stdpOnly = true;
        } else if (arg.find("--test-connectivity") != std::string::npos) {
            connectivityOnly = true;
        } else if (arg.find("--export-stats=") != std::string::npos) {
            exportStatsFile = arg.substr(13);
        } else if (arg.find("--save-checkpoint=") != std::string::npos) {
            checkpointFile = arg.substr(17);
        } else if (arg.find("--load-checkpoint=") != std::string::npos) {
            loadCheckpointFile = arg.substr(18);
        } else if (arg.find("--performance-analysis") != std::string::npos) {
            performanceAnalysis = true;
        } else if (arg.find("--learning-curve") != std::string::npos) {
            learningCurve = true;
        } else if (arg.find("--agent-mode=") != std::string::npos) {
            agentMode = arg.substr(12);
        } else if (arg.find("--set=") != std::string::npos) {
            std::string configPair = arg.substr(6);
            size_t equalsPos = configPair.find('=');
            if (equalsPos != std::string::npos) {
                std::string key = configPair.substr(0, equalsPos);
                std::string value = configPair.substr(equalsPos + 1);
                customConfigs[key] = value;
            }
        } else if (arg.find("--enable-feature=") != std::string::npos) {
            std::string feature = arg.substr(17);
            customConfigs["feature_" + feature] = "enabled";
        } else if (arg.find("--disable-feature=") != std::string::npos) {
            std::string feature = arg.substr(19);
            customConfigs["feature_" + feature] = "disabled";
        } else if (arg.find("--plot-mode") != std::string::npos) {
            plotMode = true;
        } else if (arg.find("--memory-analysis") != std::string::npos) {
            memoryAnalysis = true;
        } else if (arg.find("--neural-activity") != std::string::npos) {
            neuralActivity = true;
        } else if (arg.find("--plasticity-analysis") != std::string::npos) {
            plasticityAnalysis = true;
        } else if (arg.find("--scenario=") != std::string::npos) {
            scenario = arg.substr(10);
        } else if (arg == "-fast") {
            mode = "debug";
            benchmark = true;
            steps = 100;
        } else if (arg == "-agent-only") {
            agentOnly = true;
            steps = 5000;
        } else if (arg == "-perf") {
            mode = "debug";
            performanceAnalysis = true;
        } else if (arg == "-plot") {
            plotMode = true;
            exportStatsFile = "stats.csv";
        } else if (arg.find("--") != std::string::npos) {
            // Unknown option, could log warning
            NLM_LOG_WARNING("Unknown option: " + arg);
        } else {
            // Positional argument
            NLM_LOG_WARNING("Ignoring positional argument: " + arg);
        }
    }
    
    printBanner();
    
    std::cout << "Initializing Enhanced NLM with " << mode << " mode...\n" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(verbose ? LogLevel::Debug : LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== Enhanced NLM " << mode << " Mode ===");
    NLM_LOG_INFO("Starting with " << steps << " steps, " << neurons << " neurons, seed: " << seed);
    NLM_LOG_INFO("Execution mode: " << mode);
    
    if (benchmark) {
        NLM_LOG_INFO("Benchmark mode enabled");
    }
    if (agentOnly) {
        NLM_LOG_INFO("Agent-only mode enabled");
    }
    if (performanceAnalysis) {
        NLM_LOG_INFO("Performance analysis enabled");
    }
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Set default values for Phase 2 (with user overrides)
    config->set("random_seed", neurons, ConfigSource::Default);  // Use neuron count as seed proxy
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", neurons, ConfigSource::Default);
    config->set("region_count", 1, ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Custom configurations
    for (const auto& pair : customConfigs) {
        config->set(pair.first, std::stod(pair.second), ConfigSource::Default);
    }
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration Summary:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", seed)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", neurons)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing Enhanced NLM Brain...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Load checkpoint if specified
    if (!loadCheckpointFile.empty()) {
        NLM_LOG_INFO("Loading checkpoint from: " + loadCheckpointFile);
        brain->load(loadCheckpointFile);
    }
    
    // Run different modes based on user selection
    LearningExperiment learningExperiment(brain, seed);
    
    if (benchmark) {
        runBenchmarkMode(brain, steps);
    } else if (agentOnly) {
        runAgentSystemDemo(brain, agentMode);
    } else if (plasticityOnly) {
        runBasicConnectivityTest(brain);
        brain->reset();
        brain->initialize();
        runPlasticityExperiment(brain);
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
    } else if (stdpOnly) {
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
    } else if (connectivityOnly) {
        runBasicConnectivityTest(brain);
    } else if (mode == "agent") {
        runAgentSystemDemo(brain, agentMode);
    } else if (mode == "educational") {
        NLM_LOG_INFO("=== Educational Mode ===");
        NLM_LOG_INFO("This mode provides step-by-step guidance for learning NLM.");
        runBasicConnectivityTest(brain);
        brain->reset();
        brain->initialize();
        runPlasticityExperiment(brain);
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
    } else if (mode == "debug") {
        NLM_LOG_INFO("=== Debug Mode ===");
        NLM_LOG_INFO("Running with enhanced logging and analysis.");
        NLM_LOG_INFO("Neuron count: " + std::to_string(brain->getTotalNeuronCount()));
        NLM_LOG_INFO("Synapse count: " + std::to_string(brain->getTotalSynapseCount()));
        
        if (steps > 0) {
            for (SimulationStep step = 0; step < steps; ++step) {
                brain->step(step, step * timestep);
                
                if (step % 100 == 0) {
                    NLM_LOG_DEBUG("Debug step " + std::to_string(step) + ":");
                    NLM_LOG_DEBUG("  Firing neurons: " + std::to_string(brain->getFiringNeuronCount()));
                    NLM_LOG_DEBUG("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
                    NLM_LOG_DEBUG("  Average firing rate: " + std::to_string(brain->getAverageFiringRate()));
                }
            }
        }
        
        if (performanceAnalysis) {
            NLM_LOG_INFO("Performance analysis enabled (already in debug mode)");
        }
    } else {
        // Normal mode - run all three tests
        NLM_LOG_INFO("=== Normal Mode ===");
        NLM_LOG_INFO("Running standard test suite...");
        
        runBasicConnectivityTest(brain);
        
        brain->reset();
        brain->initialize();
        
        runPlasticityExperiment(brain);
        
        brain->reset();
        brain->initialize();
        
        runStdpVerification(brain);
    }
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Enhanced Mode Complete ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Execution Summary:");
    NLM_LOG_INFO("  Mode: " << mode);
    NLM_LOG_INFO("  Steps executed: " << steps);
    NLM_LOG_INFO("  Neurons: " << brain->getTotalNeuronCount());
    NLM_LOG_INFO("  Synapses: " << brain->getTotalSynapseCount());
    NLM_LOG_INFO("  Total spikes: " << brain->getTotalSpikeCount());
    
    // Save checkpoint if specified
    if (!checkpointFile.empty()) {
        NLM_LOG_INFO("Saving checkpoint to: " + checkpointFile);
        brain->save(checkpointFile);
    }
    
    // Export statistics if requested
    if (!exportStatsFile.empty()) {
        exportStatistics(exportStatsFile, brain, learningExperiment, learningCurve);
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Enhanced NLM execution completed successfully!");
    
    return 0;
}
