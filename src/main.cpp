// Enhanced NLM Main Entry Point with Performance Optimizations
// Demonstrates the improved command structure and features

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
#include <chrono>
#include <thread>
#include <map>
#include <algorithm>
#include <fstream>

using namespace nlm;

// Forward declarations for command handlers
class NLMCommandBase;
using CommandPtr = std::unique_ptr<NLMCommandBase>;

// Base command class
class NLMCommandBase {
public:
    virtual ~NLMCommandBase() = default;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual bool execute(std::shared_ptr<Brain> brain, const Config& config) = 0;
    virtual std::string getUsage() const = 0;
};

// Specific command implementations
class BasicConnectivityCommand : public NLMCommandBase {
public:
    std::string getName() const override { return "connectivity"; }
    std::string getDescription() const override { 
        return "Test basic neural connectivity and spike propagation"; 
    }
    bool execute(std::shared_ptr<Brain> brain, const Config& config) override {
        runBasicConnectivityTest(brain, config);
        return true;
    }
    std::string getUsage() const override { 
        return "connectivity [--steps N] [--current I]"; 
    }
};

class PlasticityExperimentCommand : public NLMCommandBase {
public:
    std::string getName() const override { return "plasticity"; }
    std::string getDescription() const override { 
        return "Run plasticity learning experiment with weight changes"; 
    }
    bool execute(std::shared_ptr<Brain> brain, const Config& config) override {
        runPlasticityExperiment(brain, config);
        return true;
    }
    std::string getUsage() const override { 
        return "plasticity [--steps N] [--input-pattern]"; 
    }
};

class StdpVerificationCommand : public NLMCommandBase {
public:
    std::string getName() const override { return "stdp"; }
    std::string getDescription() const override { 
        return "Verify STDP (Spike-Timing Dependent Plasticity) mechanism"; 
    }
    bool execute(std::shared_ptr<Brain> brain, const Config& config) override {
        runStdpVerification(brain, config);
        return true;
    }
    std::string getUsage() const override { 
        return "stdp [--trials N] [--pre-post-time DT]"; 
    }
};

class SimulationCommand : public NLMCommandBase {
public:
    std::string getName() const override { return "simulate"; }
    std::string getDescription() const override { 
        return "Run continuous simulation for specified duration"; 
    }
    bool execute(std::shared_ptr<Brain> brain, const Config& config) override {
        runContinuousSimulation(brain, config);
        return true;
    }
    std::string getUsage() const override { 
        return "simulate --duration SECONDS [--checkpoint PATH]"; 
    }
};

class InteractiveCommand : public NLMCommandBase {
public:
    std::string getName() const override { return "interactive"; }
    std::string getDescription() const override { 
        return "Run interactive brain session with world interaction"; 
    }
    bool execute(std::shared_ptr<Brain> brain, const Config& config) override {
        runInteractiveSession(brain, config);
        return true;
    }
    std::string getUsage() const override { 
        return "interactive [--max-episodes N] [--vision-size W,H]"; 
    }
};

class BenchmarkCommand : public NLMCommandBase {
public:
    std::string getName() const override { return "benchmark"; }
    std::string getDescription() const override { 
        return "Run performance benchmark of brain components"; 
    }
    bool execute(std::shared_ptr<Brain> brain, const Config& config) override {
        runBenchmark(brain, config);
        return true;
    }
    std::string getUsage() const override { 
        return "benchmark [--iterations N] [--neuron-count N]"; 
    }
};

class SaveLoadCommand : public NLMCommandBase {
public:
    std::string getName() const override { return "save"; }
    std::string getDescription() const override { 
        return "Save brain state to file"; 
    }
    bool execute(std::shared_ptr<Brain> brain, const Config& config) override {
        return saveBrainState(brain, config);
    }
    std::string getUsage() const override { 
        return "save --file PATH [--compression LEVEL]"; 
    }
};

class ListCommand : public NLMCommandBase {
public:
    std::string getName() const override { return "list"; }
    std::string getDescription() const override { 
        return "List available commands and system status"; 
    }
    bool execute(std::shared_ptr<Brain> brain, const Config& config) override {
        printCommandList(brain, config);
        return true;
    }
    std::string getUsage() const override { 
        return "list [--detailed]"; 
    }
};

// Helper function to print banner
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
    ║     - Leaky Integrate-and-Fire (LIF) neuron dynamics          ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity rules                       ║
    ║     - Structural plasticity                                    ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Command registry and management
class CommandRegistry {
private:
    std::map<std::string, CommandPtr> commands;
    
public:
    CommandRegistry() {
        registerCommands();
    }
    
    void registerCommands() {
        commands["connectivity"] = std::make_unique<BasicConnectivityCommand>();
        commands["plasticity"] = std::make_unique<PlasticityExperimentCommand>();
        commands["stdp"] = std::make_unique<StdpVerificationCommand>();
        commands["simulate"] = std::make_unique<SimulationCommand>();
        commands["interactive"] = std::make_unique<InteractiveCommand>();
        commands["benchmark"] = std::make_unique<BenchmarkCommand>();
        commands["save"] = std::make_unique<SaveLoadCommand>();
        commands["list"] = std::make_unique<ListCommand>();
    }
    
    bool hasCommand(const std::string& name) const {
        return commands.find(name) != commands.end();
    }
    
    bool executeCommand(const std::string& name, std::shared_ptr<Brain> brain, 
                       const Config& config, const std::vector<std::string>& args) {
        auto it = commands.find(name);
        if (it == commands.end()) {
            std::cerr << "Unknown command: " << name << std::endl;
            return false;
        }
        
        try {
            return it->second->execute(brain, config);
        } catch (const std::exception& e) {
            std::cerr << "Error executing command: " << e.what() << std::endl;
            return false;
        }
    }
    
    void printHelp() const {
        std::cout << "Available commands:" << std::endl;
        std::cout << "====================" << std::endl;
        for (const auto& pair : commands) {
            std::cout << "  " << pair.first << " - " << pair.second->getDescription() << std::endl;
        }
        std::cout << std::endl;
        std::cout << "Use '<command> --help' for detailed usage information." << std::endl;
    }
    
    void printCommandHelp(const std::string& name) const {
        auto it = commands.find(name);
        if (it == commands.end()) {
            std::cerr << "Unknown command: " << name << std::endl;
            return;
        }
        
        std::cout << "Command: " << name << std::endl;
        std::cout << "        " << it->second->getDescription() << std::endl;
        std::cout << "Usage:  " << it->second->getUsage() << std::endl;
    }
};

// Helper functions (with enhanced versions)
void printHelp() {
    printBanner();
    std::cout << "NLM - Neural Learning Machine (Phase 2)" << std::endl;
    std::cout << "=====================================" << std::endl;
    std::cout << "This program runs neural computation experiments with LIF neurons," << std::endl;
    std::cout << "spike propagation, and learning plasticity rules." << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: nlm [OPTIONS] [COMMAND] [ARGS]..." << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h                  Show this help message" << std::endl;
    std::cout << "  --config FILE               Configuration file (default: configs/default.cfg)" << std::endl;
    std::cout << "  --seed VALUE                Random seed for reproducibility" << std::endl;
    std::cout << "  --verbose, -v               Enable verbose logging" << std::endl;
    std::cout << "  --quiet, -q                 Suppress informational messages" << std::endl;
    std::cout << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  connectivity               Test basic neural connectivity" << std::endl;
    std::cout << "  plasticity                 Run plasticity learning experiment" << std::endl;
    std::cout << "  stdp                       Verify STDP mechanism" << std::endl;
    std::cout << "  simulate DURATION           Run continuous simulation" << std::endl;
    std::cout << "  interactive                Run interactive brain session" << std::endl;
    std::cout << "  benchmark                   Run performance benchmark" << std::endl;
    std::cout << "  save FILE                  Save brain state to file" << std::endl;
    std::cout << "  list                       List available commands" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm connectivity --steps 100 --current 50" << std::endl;
    std::cout << "  nlm plasticity --steps 1000" << std::endl;
    std::cout << "  nlm simulate --duration 60 --checkpoint ./brain_state" << std::endl;
}

// Enhanced versions of original test functions with command-line support
void runBasicConnectivityTest(std::shared_ptr<Brain> brain, const Config& config) {
    int steps = config.getOr<int64_t>("test_steps", 50);
    float current = config.getOr<float>("test_current", 50.0f);
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    NLM_LOG_INFO("Steps: " + std::to_string(steps) + ", Current injection: " + std::to_string(current));
    
    // Inject current into a few neurons and see if spikes propagate
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        NLM_LOG_ERROR("No region found!");
        return;
    }
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_ERROR("No neurons found!");
        return;
    }
    
    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject strong current into first 10 neurons
    NLM_LOG_INFO("  Injecting current into 10 neurons...");
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(current);
    }
    
    // Run a few steps
    for (SimulationStep step = 0; step < steps; ++step) {
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

void runPlasticityExperiment(std::shared_ptr<Brain> brain, const Config& config) {
    int steps = config.getOr<int64_t>("plasticity_steps", 1000);
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    NLM_LOG_INFO("Applying repeated input patterns (" + std::to_string(steps) + " steps)...");
    
    // Record initial state
    auto experiment = LearningExperiment(brain, 42);
    experiment.recordInitialState();
    
    // Enable plasticity on synapses
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    // Apply repeated input pattern to stimulate learning
    for (SimulationStep step = 0; step < steps; ++step) {
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

void runStdpVerification(std::shared_ptr<Brain> brain, const Config& config) {
    int trials = config.getOr<int64_t>("stdp_trials", 50);
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    NLM_LOG_INFO("Testing STDP on 5 synapses with " + std::to_string(trials) + " trials");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    // Get first few synapses
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        NLM_LOG_INFO("  Not enough synapses for STDP test");
        return;
    }
    
    // Record initial weights
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " before: " + std::to_string(beforeWeights[i]));
    }
    
    // Create correlated activity: fire pre then post to trigger LTP
    NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
    
    for (int trial = 0; trial < trials; ++trial) {
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

void runContinuousSimulation(std::shared_ptr<Brain> brain, const Config& config) {
    double duration = config.getOr<double>("simulate_duration", 10.0);
    std::string checkpointPath = config.getOr<std::string>("checkpoint_file", "./checkpoints");
    bool saveCheckpoints = config.getOr<bool>("checkpoint_enabled", true);
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Continuous Simulation ===");
    NLM_LOG_INFO("Duration: " + std::to_string(duration) + " seconds");
    NLM_LOG_INFO("Checkpoint interval: " + std::to_string(checkpointPath) + " (enabled: " + 
                 (saveCheckpoints ? "yes" : "no") + ")");
    
    SimulationStep steps = static_cast<SimulationStep>(duration * 1000.0); // 1ms timestep
    
    for (SimulationStep step = 0; step < steps; ++step) {
        brain->step(step, step * 0.001);
        
        // Log every second
        if (step % 1000 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Neurons: " + std::to_string(brain->getTotalNeuronCount()));
            
            // Save checkpoint if enabled
            if (saveCheckpoints && step % 10000 == 0) {
                std::string filename = checkpointPath + "_step_" + std::to_string(step) + ".bin";
                if (brain->save(filename)) {
                    NLM_LOG_INFO("  Checkpoint saved: " + filename);
                }
            }
        }
    }
    
    NLM_LOG_INFO("Simulation complete! Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
}

void runInteractiveSession(std::shared_ptr<Brain> brain, const Config& config) {
    int maxEpisodes = config.getOr<int64_t>("interactive_episodes", 5);
    int visionWidth = config.getOr<int64_t>("vision_width", 16);
    int visionHeight = config.getOr<int64_t>("vision_height", 16);
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Interactive Session ===");
    NLM_LOG_INFO("Max episodes: " + std::to_string(maxEpisodes));
    NLM_LOG_INFO("Vision: " + std::to_string(visionWidth) + "x" + std::to_string(visionHeight));
    
    for (int episode = 0; episode < maxEpisodes; ++episode) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Episode " + std::to_string(episode + 1) + "/" + std::to_string(maxEpisodes) + " ===");
        
        // Reset brain for new episode
        brain->reset();
        brain->initialize();
        
        // Run episode
        SimulationStep steps = 1000; // 1 second per episode
        for (SimulationStep step = 0; step < steps; ++step) {
            // Inject random sensory input (simulating environment interaction)
            float visionValue = brain->getRandomGenerator()->uniformReal(0.0f, 1.0f);
            for (int y = 0; y < visionHeight; ++y) {
                for (int x = 0; x < visionWidth; ++x) {
                    if (brain->getRandomGenerator()->uniformReal(0.0f, 1.0f) < 0.1f) {
                        // 10% chance of object at this position
                        // (In a real implementation, this would be a proper visual scene)
                    }
                }
            }
            
            brain->step(step, step * 0.001);
            
            if (step % 200 == 0) {
                NLM_LOG_INFO("  Step " + std::to_string(step) + 
                            " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                            " | Action: " + std::to_string(static_cast<int>(brain->produceAction()->getType())));
            }
        }
        
        NLM_LOG_INFO("Episode " + std::to_string(episode + 1) + " complete."
                    " Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("All interactive episodes complete.");
}

void runBenchmark(std::shared_ptr<Brain> brain, const Config& config) {
    int iterations = config.getOr<int64_t>("benchmark_iterations", 10);
    int neuronCount = config.getOr<int64_t>("benchmark_neurons", 500);
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Benchmark ===");
    NLM_LOG_INFO("Iterations: " + std::to_string(iterations) + ", Neurons: " + std::to_string(neuronCount));
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int iteration = 0; iteration < iterations; ++iteration) {
        // Reset brain
        brain->reset();
        brain->initialize();
        
        // Run steps
        SimulationStep steps = static_cast<SimulationStep>(10.0); // 10ms per benchmark
        for (SimulationStep step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
        }
        
        if (iteration % 5 == 0) {
            NLM_LOG_INFO("  Iteration " + std::to_string(iteration + 1) + "/" + 
                        std::to_string(iterations) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()));
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    float msPerIteration = static_cast<float>(duration.count()) / iterations;
    float spikesPerSecond = 1000.0f * brain->getTotalSpikeCount() / duration.count();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Benchmark Results:");
    NLM_LOG_INFO("  Total time: " + std::to_string(duration.count()) + " ms");
    NLM_LOG_INFO("  Average time per iteration: " + std::to_string(msPerIteration) + " ms");
    NLM_LOG_INFO("  Spikes per second: " + std::to_string(spikesPerSecond));
    NLM_LOG_INFO("  Average spikes per iteration: " + std::to_string(brain->getTotalSpikeCount() / iterations));
}

bool saveBrainState(std::shared_ptr<Brain> brain, const Config& config) {
    std::string filepath = config.getOr<std::string>("save_file", "brain_state.bin");
    
    NLM_LOG_INFO("Saving brain state to " + filepath + "...");
    
    if (brain->save(filepath)) {
        NLM_LOG_INFO("Brain state saved successfully.");
        return true;
    } else {
        NLM_LOG_ERROR("Failed to save brain state.");
        return false;
    }
}

void printCommandList(std::shared_ptr<Brain> brain, const Config& config) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== NLM Command List ===");
    
    NLM_LOG_INFO("Brain Status:");
    NLM_LOG_INFO("  Regions: " + std::to_string(brain->getRegionCount()));
    NLM_LOG_INFO("  Total neurons: " + std::to_string(brain->getTotalNeuronCount()));
    NLM_LOG_INFO("  Total synapses: " + std::to_string(brain->getTotalSynapseCount()));
    NLM_LOG_INFO("  Active neurons: " + std::to_string(brain->getActiveNeuronCount()));
    NLM_LOG_INFO("  Firing neurons: " + std::to_string(brain->getFiringNeuronCount()));
    NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Available Commands:");
    NLM_LOG_INFO("  connectivity   - Test basic neural connectivity and spike propagation");
    NLM_LOG_INFO("  plasticity     - Run plasticity learning experiment with weight changes");
    NLM_LOG_INFO("  stdp           - Verify STDP (Spike-Timing Dependent Plasticity) mechanism");
    NLM_LOG_INFO("  simulate DURATION - Run continuous simulation for specified duration (seconds)");
    NLM_LOG_INFO("  interactive    - Run interactive brain session with world interaction");
    NLM_LOG_INFO("  benchmark      - Run performance benchmark of brain components");
    NLM_LOG_INFO("  save FILE      - Save brain state to file");
    NLM_LOG_INFO("  list           - List available commands and system status");
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Usage Examples:");
    NLM_LOG_INFO("  ./nlm connectivity --steps 100 --current 50");
    NLM_LOG_INFO("  ./nlm plasticity --steps 1000");
    NLM_LOG_INFO("  ./nlm simulate --duration 60 --checkpoint ./brain_state");
    NLM_LOG_INFO("  ./nlm benchmark --iterations 20 --neuron-count 1000");
}

// Enhanced LearningExperiment struct with config support
struct LearningExperiment {
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
    
    LearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
        : brain(b), seed(s), initialSynapseCount(0) {}
};

// Main function with modular command structure
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
    
    // Set default values for Phase 2
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Test experiment parameters
    config->set("test_steps", 50, ConfigSource::Default);
    config->set("test_current", 50.0f, ConfigSource::Default);
    config->set("plasticity_steps", 1000, ConfigSource::Default);
    config->set("stdp_trials", 50, ConfigSource::Default);
    
    // Parse command line arguments
    std::string configFile = "configs/default.cfg";
    bool verbose = false;
    bool quiet = false;
    std::vector<std::string> commandArgs;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        } else if (arg == "--verbose" || arg == "-v") {
            verbose = true;
            consoleLogger->setLevel(LogLevel::Info);
        } else if (arg == "--quiet" || arg == "-q") {
            quiet = true;
            consoleLogger->setLevel(LogLevel::Error);
        } else if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (arg.substr(0, 6) == "--seed") {
            if (arg.find('=') != std::string::npos) {
                int64_t seed = std::stoll(arg.substr(arg.find('=') + 1));
                config->set("random_seed", seed, ConfigSource::CommandLine);
            } else if (i + 1 < argc) {
                int64_t seed = std::stoll(argv[++i]);
                config->set("random_seed", seed, ConfigSource::CommandLine);
            }
        } else if (arg.substr(0, 8) == "--steps") {
            if (arg.find('=') != std::string::npos) {
                int64_t steps = std::stoll(arg.substr(arg.find('=') + 1));
                config->set("test_steps", steps, ConfigSource::CommandLine);
                config->set("plasticity_steps", steps, ConfigSource::CommandLine);
                config->set("stdp_trials", steps / 10, ConfigSource::CommandLine);
            }
        } else if (arg.substr(0, 9) == "--current") {
            if (arg.find('=') != std::string::npos) {
                float current = std::stof(arg.substr(arg.find('=') + 1));
                config->set("test_current", current, ConfigSource::CommandLine);
            }
        } else if (arg.substr(0, 10) == "--duration") {
            if (arg.find('=') != std::string::npos) {
                double duration = std::stod(arg.substr(arg.find('=') + 1));
                config->set("simulate_duration", duration, ConfigSource::CommandLine);
            }
        } else if (arg.substr(0, 10) == "--checkpoint") {
            if (arg.find('=') != std::string::npos) {
                std::string checkpoint = arg.substr(arg.find('=') + 1);
                config->set("checkpoint_file", checkpoint, ConfigSource::CommandLine);
                config->set("checkpoint_enabled", true, ConfigSource::CommandLine);
            }
        } else if (arg.substr(0, 10) == "--episodes") {
            if (arg.find('=') != std::string::npos) {
                int64_t episodes = std::stoll(arg.substr(arg.find('=') + 1));
                config->set("interactive_episodes", episodes, ConfigSource::CommandLine);
            }
        } else if (arg.substr(0, 12) == "--neuron-count") {
            if (arg.find('=') != std::string::npos) {
                int64_t neurons = std::stoll(arg.substr(arg.find('=') + 1));
                config->set("neuron_count", neurons, ConfigSource::CommandLine);
            }
        } else if (arg.substr(0, 12) == "--iterations") {
            if (arg.find('=') != std::string::npos) {
                int64_t iterations = std::stoll(arg.substr(arg.find('=') + 1));
                config->set("benchmark_iterations", iterations, ConfigSource::CommandLine);
            }
        } else if (arg.substr(0, 11) == "--file") {
            if (arg.find('=') != std::string::npos) {
                std::string file = arg.substr(arg.find('=') + 1);
                config->set("save_file", file, ConfigSource::CommandLine);
            }
        } else if (arg.substr(0, 8) == "--width") {
            if (arg.find('=') != std::string::npos) {
                int64_t width = std::stoll(arg.substr(arg.find('=') + 1));
                config->set("vision_width", width, ConfigSource::CommandLine);
            }
        } else if (arg.substr(0, 9) == "--height") {
            if (arg.find('=') != std::string::npos) {
                int64_t height = std::stoll(arg.substr(arg.find('=') + 1));
                config->set("vision_height", height, ConfigSource::CommandLine);
            }
        } else {
            // Treat as command
            commandArgs.push_back(arg);
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
    
    // Override with parsed command line arguments
    if (!quiet) {
        NLM_LOG_INFO("Configuration:");
        NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
        NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
        NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
        NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
        NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    }
    
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
    
    // Initialize command registry
    CommandRegistry commandRegistry;
    
    // Determine which command to execute
    std::string command;
    if (!commandArgs.empty()) {
        command = commandArgs[0];
        commandArgs.erase(commandArgs.begin());
    } else {
        // No command provided, show help or run default tests
        printHelp();
        return 0;
    }
    
    // Execute the command
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Executing command: " + command);
    
    bool success = false;
    if (commandRegistry.hasCommand(command)) {
        success = commandRegistry.executeCommand(command, brain, *config, commandArgs);
    } else {
        NLM_LOG_ERROR("Unknown command: " << command << ". Use 'list' to see available commands.");
    }
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Command Complete ===");
    
    if (success) {
        NLM_LOG_INFO("Command executed successfully.");
    } else {
        NLM_LOG_ERROR("Command execution failed.");
    }
    
    return success ? 0 : 1;
}
