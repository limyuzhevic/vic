// NLM Stress Test Suite
// Tests system scalability and stability under extreme conditions

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"

#include <thread>

using namespace nlm;

struct StressTestResult {
    size_t neurons;
    size_t synapses;
    size_t steps;
    double totalTime;      // seconds
    double stepsPerSecond;
    double spikesPerStep;
    double synapsesPerNeuron;
    double memoryUsageMB;  // Approximate
    bool successful;       // Whether test completed without crashing
    std::string errorMessage;
};

// Simple memory estimation (very rough approximation)
double estimateMemoryUsage(const Brain& brain) {
    // Rough estimate: neurons (state) + synapses (connections) + structures
    size_t neuronStateBytes = 200 * brain.getTotalNeuronCount();  // ~200 bytes per neuron
    size_t synapseStateBytes = 100 * brain.getTotalSynapseCount(); // ~100 bytes per synapse
    size_t structOverhead = 50000;  // Fixed structures
    return (neuronStateBytes + synapseStateBytes + structOverhead) / (1024.0 * 1024.0);
}

StressTestResult runStressTest(size_t neuronCount, size_t steps) {
    StressTestResult result;
    result.neurons = neuronCount;
    result.synapses = 0;
    result.steps = steps;
    result.successful = false;
    result.errorMessage = "";
    
    std::cout << "  Running stress test: " << neuronCount << " neurons, " 
              << steps << " steps..." << std::flush;
    
    try {
        // Create configuration with minimal overhead
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.05f, ConfigSource::Default);  // Lower for large networks
        config->set("stdp_ltp_weight", 0.005f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.006f, ConfigSource::Default);
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        
        // Initialize with error checking
        if (!brain->initialize()) {
            throw std::runtime_error("Brain initialization failed");
        }
        
        result.synapses = brain->getTotalSynapseCount();
        
        // Run simulation with periodic status
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < steps; ++step) {
            // Inject small input to keep network active
            for (size_t i = 0; i < std::min(size_t(20), neuronCount / 500); ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
            }
            
            // Step with error checking
            if (step % 1000 == 0) {
                // Log progress periodically for large tests
                std::cout << "." << std::flush;
            }
            
            brain->step(step, step * 0.001);
            
            // Check for catastrophic failure
            if (step % 5000 == 0 && brain->getTotalSpikeCount() > 1000000000) {
                throw std::runtime_error("Too many spikes - possible runaway excitation");
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Collect results
        result.totalTime = totalTime;
        result.stepsPerSecond = steps / totalTime;
        result.spikesPerStep = static_cast<double>(brain->getTotalSpikeCount()) / steps;
        result.synapsesPerNeuron = static_cast<double>(result.synapses) / result.neurons;
        result.memoryUsageMB = estimateMemoryUsage(*brain);
        result.successful = true;
        
        std::cout << " Done!" << std::endl;
        std::cout << "    Time: " << std::fixed << std::setprecision(2) << totalTime << "s" << std::endl;
        std::cout << "    Steps/sec: " << std::fixed << std::setprecision(0) << result.stepsPerSecond << std::endl;
        std::cout << "    Spikes/step: " << std::fixed << std::setprecision(2) << result.spikesPerStep << std::endl;
        
    } catch (const std::exception& e) {
        result.successful = false;
        result.errorMessage = std::string("Exception: ") + e.what();
        std::cout << " Failed! " << e.what() << std::endl;
    } catch (...) {
        result.successful = false;
        result.errorMessage = "Unknown exception";
        std::cout << " Failed! (unknown exception)" << std::endl;
    }
    
    return result;
}

void printStressTestResults(const std::vector<StressTestResult>& results) {
    std::cout << std::endl;
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                        NLM Stress Test Results                                                  ║" << std::endl;
    std::cout << "╠══════════════╦══════════════╦═══════════╦════════════╦══════════════════╦══════════════╦══════════════╣" << std::endl;
    std::cout << "║    Neurons   ║    Synapses  ║ Steps/sec  ║ Spikes/step║ Memory Usage  ║ Successful  ║   Duration   ║" << std::endl;
    std::cout << "╠══════════════╬══════════════╬═══════════╬════════════╬══════════════════╬══════════════╬══════════════╣" << std::endl;
    
    for (const auto& r : results) {
        std::string success = r.successful ? "✓" : "✗";
        std::string duration = r.successful ? 
            (r.totalTime < 60.0 ? std::to_string((int)r.totalTime) + "s" : 
             std::to_string(r.totalTime / 60.0) + "m") : "FAILED";
        
        std::cout << "║ " << std::setw(11) << r.neurons
                  << " ║ " << std::setw(12) << r.synapses
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(0) << r.stepsPerSecond
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(2) << r.spikesPerStep
                  << " ║ " << std::setw(18) << std::fixed << std::setprecision(2) << r.memoryUsageMB << " MB"
                  << " ║ " << std::setw(11) << success
                  << " ║ " << std::setw(11) << duration
                  << " ║" << std::endl;
    }
    
    std::cout << "╚══════════════╩══════════════╩═══════════╩════════════╩══════════════════╩══════════════╩══════════════╝" << std::endl;
    std::cout << std::endl;
    
    // Summary
    std::cout << "Stress Test Summary:" << std::endl;
    size_t successfulTests = 0;
    double maxMemory = 0.0;
    double minStepsPerSec = 1e18;
    
    for (const auto& r : results) {
        if (r.successful) {
            successfulTests++;
            if (r.memoryUsageMB > maxMemory) maxMemory = r.memoryUsageMB;
            if (r.stepsPerSecond < minStepsPerSec) minStepsPerSec = r.stepsPerSecond;
        }
    }
    
    std::cout << "  Successful tests: " << successfulTests << "/" << results.size() << std::endl;
    std::cout << "  Max memory usage: " << std::fixed << std::setprecision(2) << maxMemory << " MB" << std::endl;
    std::cout << "  Min performance: " << std::fixed << std::setprecision(0) << minStepsPerSec << " steps/sec" << std::endl;
    
    if (results.size() > 0) {
        double successRate = static_cast<double>(successfulTests) / results.size() * 100.0;
        std::cout << "  Success rate: " << std::fixed << std::setprecision(1) << successRate << "%" << std::endl;
    }
}

int main() {
    std::cout << R"(
    ╔════════════════════════════════════════════════════════════════════════════════════════╗
    ║                                                                                ║
    ║     NLM — 熙然                                                                    ║
    ║     Neural Learning Machine                                                       ║
    ║                                                                                ║
    ║     Stress Testing Suite                                                          ║
    ║                                                                                ║
    ╚════════════════════════════════════════════════════════════════════════════════════════╝
    )" << std::endl;
    
    // Initialize logger for stress testing
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "Running NLM Stress Tests (testing scalability to large neuron populations)..." << std::endl;
    std::cout << std::endl;
    
    std::vector<StressTestResult> results;
    
    // Stress test configurations: progressively larger networks
    std::vector<std::pair<size_t, size_t>> stressTests = {
        {1000, 500},      // 1000 neurons, 500 steps (baseline)
        {5000, 200},      // 5000 neurons, 200 steps (small network)
        {10000, 100},     // 10k neurons, 100 steps (medium network)
        {20000, 50},      // 20k neurons, 50 steps (large network)
        {50000, 20},      // 50k neurons, 20 steps (very large network)
    };
    
    std::cout << "Warning: These tests may consume significant memory and CPU!" << std::endl;
    std::cout << "Proceeding with stress tests..." << std::endl;
    std::cout << std::endl;
    
    for (const auto& [neurons, steps] : stressTests) {
        auto result = runStressTest(neurons, steps);
        results.push_back(result);
        std::cout << std::endl;
        
        // Brief pause between tests
        std::cout << "Pausing briefly between tests..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    
    // Print comprehensive results
    printStressTestResults(results);
    
    std::cout << std::endl;
    std::cout << "Stress testing complete." << std::endl;
    std::cout << "The system has been tested at scales from " << results.front().neurons 
              << " to " << results.back().neurons << " neurons." << std::endl;
    
    return 0;
}