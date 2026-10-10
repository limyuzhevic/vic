// NLM Performance Benchmark with Optimization Tracking
// Measures simulation performance for different network sizes
// Compares original implementation with optimized version

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <iomanip>
#include <string>
#include <algorithm>
#include <numeric>

using namespace nlm;

// Performance measurement structure
struct ComponentTiming {
    double brainStepTime;      // Brain step() time in seconds
    double agentProcessTime;    // AgentBrain process time in seconds
    double worldUpdateTime;     // World update time in seconds
    double sensoryProcessTime;  // Sensory processing time in seconds
    double plasticityTime;      // Plasticity calculation time in seconds
    double memoryTime;          // Memory system time in seconds
    double totalTime;           // Total step time in seconds
    
    ComponentTiming() : brainStepTime(0.0), agentProcessTime(0.0),
                       worldUpdateTime(0.0), sensoryProcessTime(0.0),
                       plasticityTime(0.0), memoryTime(0.0), totalTime(0.0) {}
};

struct BenchmarkResult {
    size_t neurons;
    size_t synapses;
    size_t steps;
    double totalTime;           // seconds
    double stepsPerSecond;
    double spikesPerStep;
    double synapsesPerNeuron;
    ComponentTiming componentTimings;  // Detailed component timings
    size_t peakMemoryUsage;      // Memory usage in bytes
};

BenchmarkResult runBenchmark(size_t neuronCount, size_t steps) {
    std::cout << "  Running benchmark: " << neuronCount << " neurons, " 
              << steps << " steps..." << std::flush;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Create world
    auto world = std::make_unique<SimpleWorld>();
    world->configure(20.0f, 20.0f, 16, 16);
    world->setRandomSeed(42);
    world->reset();
    
    // Create agent brain
    auto agentBrain = std::make_unique<AgentBrain>(brain);
    agentBrain->initialize(*world);
    
    // Initialize performance tracking
    ComponentTiming componentTimes;
    
    // Run simulation
    auto totalStartTime = std::chrono::high_resolution_clock::now();
    
    for (size_t step = 0; step < steps; ++step) {
        auto stepStartTime = std::chrono::high_resolution_clock::now();
        
        // Inject some random input
        for (size_t i = 0; i < std::min(size_t(10), neuronCount / 10); ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        // STEP 1: World update
        auto worldStart = std::chrono::high_resolution_clock::now();
        world->update(0.001);
        auto worldEnd = std::chrono::high_resolution_clock::now();
        componentTimes.worldUpdateTime += std::chrono::duration<double>(worldEnd - worldStart).count();
        
        // STEP 2: Agent brain processing
        auto agentStart = std::chrono::high_resolution_clock::now();
        agentBrain->processSensoryInput(world->getSensoryPercept());
        auto agentEnd = std::chrono::high_resolution_clock::now();
        componentTimes.agentProcessTime += std::chrono::duration<double>(agentEnd - agentStart).count();
        
        // STEP 3: Brain step
        auto brainStart = std::chrono::high_resolution_clock::now();
        brain->step(step, step * 0.001);
        auto brainEnd = std::chrono::high_resolution_clock::now();
        componentTimes.brainStepTime += std::chrono::duration<double>(brainEnd - brainStart).count();
        
        // STEP 4: Motor command decoding and world interaction
        auto motorStart = std::chrono::high_resolution_clock::now();
        auto action = agentBrain->decodeMotorCommand();
        if (action) {
            auto result = world->applyMotorCommand(action->type, step * 0.001);
            agentBrain->applyRewardModulation(result.reward, 0.0f);
        }
        agentBrain->updateDevelopment(0.001);
        auto motorEnd = std::chrono::high_resolution_clock::now();
        componentTimes.sensoryProcessTime += std::chrono::duration<double>(motorEnd - motorStart).count();
        
        // STEP 5: Neuromodulation and plasticity (combined)
        auto plasticityStart = std::chrono::high_resolution_clock::now();
        brain->updatePlasticity();
        agentBrain->applyRewardModulation(0.0f, 0.0f);  // Dummy call for timing
        auto plasticityEnd = std::chrono::high_resolution_clock::now();
        componentTimes.plasticityTime += std::chrono::duration<double>(plasticityEnd - plasticityStart).count();
        
        // STEP 6: Memory system updates
        auto memoryStart = std::chrono::high_resolution_clock::now();
        if (brain->getWorkingMemory()) {
            brain->getWorkingMemory()->decay(0.01f);
        }
        if (brain->getEpisodicMemory()) {
            // Memory updates happen automatically during brain step
        }
        auto memoryEnd = std::chrono::high_resolution_clock::now();
        componentTimes.memoryTime += std::chrono::duration<double>(memoryEnd - memoryStart).count();
        
        auto stepEndTime = std::chrono::high_resolution_clock::now();
        componentTimes.totalTime += std::chrono::duration<double>(stepEndTime - stepStartTime).count();
    }
    
    auto totalEndTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(totalEndTime - totalStartTime).count();
    
    // Collect results
    BenchmarkResult result;
    result.neurons = brain->getTotalNeuronCount();
    result.synapses = brain->getTotalSynapseCount();
    result.steps = steps;
    result.totalTime = totalTime;
    result.stepsPerSecond = steps / totalTime;
    result.spikesPerStep = static_cast<double>(brain->getTotalSpikeCount()) / steps;
    result.synapsesPerNeuron = static_cast<double>(result.synapses) / result.neurons;
    result.componentTimings = componentTimes;
    result.peakMemoryUsage = 0;  // Placeholder for memory tracking
    
    std::cout << " Done!" << std::endl;
    std::cout << "    Time: " << std::fixed << std::setprecision(2) << totalTime << "s" << std::endl;
    std::cout << "    Steps/sec: " << std::fixed << std::setprecision(0) << result.stepsPerSecond << std::endl;
    std::cout << "    Spikes/step: " << std::fixed << std::setprecision(2) << result.spikesPerStep << std::endl;
    std::cout << "    Component breakdown:" << std::endl;
    std::cout << "      World update: " << std::fixed << std::setprecision(3) << componentTimes.worldUpdateTime << "s (" 
              << (componentTimes.worldUpdateTime / totalTime * 100) << "%)" << std::endl;
    std::cout << "      Agent processing: " << std::fixed << std::setprecision(3) << componentTimes.agentProcessTime << "s (" 
              << (componentTimes.agentProcessTime / totalTime * 100) << "%)" << std::endl;
    std::cout << "      Brain step: " << std::fixed << std::setprecision(3) << componentTimes.brainStepTime << "s (" 
              << (componentTimes.brainStepTime / totalTime * 100) << "%)" << std::endl;
    std::cout << "      Plasticity: " << std::fixed << std::setprecision(3) << componentTimes.plasticityTime << "s (" 
              << (componentTimes.plasticityTime / totalTime * 100) << "%)" << std::endl;
    std::cout << "      Memory: " << std::fixed << std::setprecision(3) << componentTimes.memoryTime << "s (" 
              << (componentTimes.memoryTime / totalTime * 100) << "%)" << std::endl;
    
    return result;
}

void printBenchmarkResults(const std::vector<BenchmarkResult>& results) {
    std::cout << std::endl;
    std::cout << "╔═══════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                     NLM Performance Benchmark Results                           ║" << std::endl;
    std::cout << "╠═══════════════╦═══════════════╦═══════════╦═════════════╦══════════════════════╣" << std::endl;
    std::cout << "║   Neurons     ║   Synapses    ║ Steps/sec ║ Spikes/step ║ Synapses/Neuron      ║" << std::endl;
    std::cout << "╠═══════════════╬═══════════════╬═══════════╬═════════════╬══════════════════════╣" << std::endl;
    
    for (const auto& r : results) {
        std::cout << "║ " << std::setw(13) << r.neurons 
                  << " ║ " << std::setw(13) << r.synapses
                  << " ║ " << std::setw(9) << std::fixed << std::setprecision(0) << r.stepsPerSecond
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(2) << r.spikesPerStep
                  << " ║ " << std::setw(20) << std::fixed << std::setprecision(2) << r.synapsesPerNeuron
                  << " ║" << std::endl;
    }
    
    std::cout << "╚═══════════════╩═══════════════╩═══════════╩═════════════╩══════════════════════╝" << std::endl;
    
    // Print detailed component breakdown
    std::cout << std::endl;
    std::cout << "╔═══════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                     Component Performance Breakdown                             ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
    
    for (const auto& r : results) {
        std::cout << "║ Neurons: " << std::setw(10) << r.neurons << " " << std::string(50, ' ') << "║" << std::endl;
        std::cout << "║ Brain step: " << std::fixed << std::setprecision(3) << r.componentTimings.brainStepTime 
                  << "s (" << (r.componentTimings.brainStepTime / r.totalTime * 100) << "%)" << std::endl;
        std::cout << "║ Agent process: " << std::fixed << std::setprecision(3) << r.componentTimings.agentProcessTime 
                  << "s (" << (r.componentTimings.agentProcessTime / r.totalTime * 100) << "%)" << std::endl;
        std::cout << "║ World update: " << std::fixed << std::setprecision(3) << r.componentTimings.worldUpdateTime 
                  << "s (" << (r.componentTimings.worldUpdateTime / r.totalTime * 100) << "%)" << std::endl;
        std::cout << "║ Plasticity: " << std::fixed << std::setprecision(3) << r.componentTimings.plasticityTime 
                  << "s (" << (r.componentTimings.plasticityTime / r.totalTime * 100) << "%)" << std::endl;
        std::cout << "╠════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
    }
    
    std::cout << "╚═══════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
}

void runComprehensiveBenchmark() {
    std::cout << "Running NLM Performance Benchmarks..." << std::endl;
    std::cout << std::endl;
    
    std::vector<BenchmarkResult> results;
    
    // Benchmark configurations: (neuron_count, steps)
    std::vector<std::pair<size_t, size_t>> benchmarks = {
        {100, 1000},      // 100 neurons, 1000 steps
        {500, 1000},      // 500 neurons, 1000 steps
        {1000, 500},      // 1000 neurons, 500 steps
        {2000, 200},      // 2000 neurons, 200 steps
    };
    
    for (const auto& [neurons, steps] : benchmarks) {
        auto result = runBenchmark(neurons, steps);
        results.push_back(result);
        std::cout << std::endl;
    }
    
    // Print summary
    printBenchmarkResults(results);
    
    std::cout << "Benchmark complete." << std::endl;
}

// Main function for standalone benchmarking
int main() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Performance Benchmark                                     ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    runComprehensiveBenchmark();
    
    return 0;
}
