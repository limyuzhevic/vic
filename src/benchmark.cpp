// NLM Performance Benchmark
// Measures simulation performance for different network sizes

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
    // Helper function to calculate memory usage
    double calculateMemoryUsage(const std::shared_ptr<Brain>& brain);
    
    // Helper function to calculate network utilization
    double calculateNetworkUtilization(const BenchmarkResult& result);

// Simple performance measurement
struct BenchmarkResult {
    size_t neurons;
    size_t synapses;
    size_t steps;
    double totalTime;      // seconds
    double stepsPerSecond;
    double spikesPerStep;
    double synapsesPerNeuron;
    double avgFiringRate;
    double memoryUsage;
    double networkUtilization;
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
    
    // Run simulation
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (size_t step = 0; step < steps; ++step) {
        // Inject some random input
        for (size_t i = 0; i < std::min(size_t(10), neuronCount / 10); ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Collect results
    BenchmarkResult result;
    result.neurons = brain->getTotalNeuronCount();
    result.synapses = brain->getTotalSynapseCount();
    result.steps = steps;
    result.totalTime = totalTime;
    result.stepsPerSecond = steps / totalTime;
    result.spikesPerStep = static_cast<double>(brain->getTotalSpikeCount()) / steps;
    result.synapsesPerNeuron = static_cast<double>(result.synapses) / result.neurons;
    result.avgFiringRate = brain->getAverageFiringRate();
    result.memoryUsage = calculateMemoryUsage(brain); // Add this helper function
    result.networkUtilization = calculateNetworkUtilization(result); // Add this helper function
    
    std::cout << " Done!" << std::endl;
    std::cout << "    Time: " << std::fixed << std::setprecision(2) << totalTime << "s" << std::endl;
    std::cout << "    Steps/sec: " << std::fixed << std::setprecision(0) << result.stepsPerSecond << std::endl;
    std::cout << "    Spikes/step: " << std::fixed << std::setprecision(2) << result.spikesPerStep << std::endl;
    std::cout << "    Firing rate: " << std::fixed << std::setprecision(2) << result.avgFiringRate << std::endl;
    std::cout << "    Network utilization: " << std::fixed << std::setprecision(2) << result.networkUtilization * 100 << "%" << std::endl;
    
    return result;
}

void printBenchmarkResults(const std::vector<BenchmarkResult>& results) {
    std::cout << std::endl;
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                          NLM Performance Benchmark Results                               ║" << std::endl;
    std::cout << "╠═══════════════╦═══════════════╦═══════════╦════════════╦════════════╦═════════════════╦══════════════════╗" << std::endl;
    std::cout << "║   Neurons     ║   Synapses    ║ Steps/sec  ║ Spikes/step ║ Firing rate ║ Synapses/Neuron ║ Network Util   ║" << std::endl;
    std::cout << "╠═══════════════╬═══════════════╬═══════════╬════════════╦════════════╦═════════════════╬══════════════════╣" << std::endl;
    
    for (const auto& r : results) {
        std::cout << "║ " << std::setw(13) << r.neurons 
                  << " ║ " << std::setw(13) << r.synapses
                  << " ║ " << std::setw(9) << std::fixed << std::setprecision(0) << r.stepsPerSecond
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(2) << r.spikesPerStep
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(2) << r.avgFiringRate
                  << " ║ " << std::setw(20) << std::fixed << std::setprecision(2) << r.synapsesPerNeuron
                  << " ║ " << std::setw(15) << std::fixed << std::setprecision(2) << r.networkUtilization
                  << " ║" << std::endl;
    }
    
    std::cout << "╚═══════════════╩═══════════════╩═══════════╩════════════╩════════════╩═════════════════╩══════════════════╝" << std::endl;
    std::cout << std::endl;
}

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
    
return 0;
    }
    
    // Helper function to calculate memory usage
    double calculateMemoryUsage(const std::shared_ptr<Brain>& brain) {
        if (!brain) return 0.0;
        
        // Estimate memory usage based on brain components
        // This is a simplified approximation - real memory usage would depend on actual allocation
        double baseMemory = 1.0; // Base overhead
        double neuronMemory = brain->getTotalNeuronCount() * 0.1; // ~100 bytes per neuron
        double synapseMemory = brain->getTotalSynapseCount() * 0.05; // ~50 bytes per synapse
        double memoryPoolMemory = brain->getTotalNeuronCount() * 0.05; // ~50 bytes per neuron for pools
        
        return baseMemory + neuronMemory + synapseMemory + memoryPoolMemory;
    }
    
    // Helper function to calculate network utilization
    double calculateNetworkUtilization(const BenchmarkResult& result) {
        if (result.neurons == 0) return 0.0;
        
        // Calculate synaptic density as a proxy for network utilization
        double synapticDensity = result.synapsesPerNeuron;
        
        // Normalize based on expected density for healthy neural networks
        // Typical networks have between 1 and 20 synapses per neuron
        double optimalDensity = 10.0;
        double utilization = std::min(1.0, synapticDensity / optimalDensity);
        
        return utilization;
    }
}
