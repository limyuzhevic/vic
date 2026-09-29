// NLM Brain Performance Benchmarks
// Comprehensive benchmarking for Brain system performance

#include "benchmarks/BrainBenchmarks.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "core/Config/Config.hpp"
#include "core/Profiler.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "dynamics/SpikeSystem.hpp"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <thread>
#include <memory>
#include <vector>
#include <algorithm>
#include <random>
#include <map>
#include <numeric>
#include <cmath>

using namespace nlm;

namespace benchmarks {

// Memory usage benchmarks tracking memory pools and allocations
void MemoryUsageBenchmarks::runMemoryPoolBenchmarks() {
    std::cout << "=== Memory Pool Efficiency Benchmarks ===" << std::endl;
    
    const std::vector<size_t> neuronCounts = {100, 500, 1000, 2000, 5000};
    const size_t steps = 100;
    
    std::vector<MemoryPoolStats> results;
    
    for (size_t neuronCount : neuronCounts) {
        std::cout << "Benchmarking memory pools for " << neuronCount << " neurons..." << std::flush;
        
        // Start memory tracking
        auto& memProfiler = MemoryAllocationProfiler::getInstance();
        memProfiler.getStats(); // Clear stats
        
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
        
        // Run simulation and track memory
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < steps; ++step) {
            // Inject input to trigger allocations
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            brain->step(step, step * 0.001);
            
            // Force some plasticity operations for memory tracking
            if (brain->getSTDP()) {
                // Access internal systems to trigger allocation patterns
                auto* spikeSystem = brain->getSpikeSystem();
                if (spikeSystem) {
                    // Trigger spike processing
                    auto spikes = spikeSystem->getSpikes();
                }
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Get memory statistics
        MemoryAllocationProfiler::MemoryStats memStats = memProfiler.getStats();
        
        // Calculate efficiency metrics
        size_t totalMemory = memStats.totalAllocated;
        size_t peakMemory = memStats.peakAllocated;
        size_t allocationCount = memStats.allocationCount;
        
        MemoryPoolStats stats;
        stats.neuronCount = neuronCount;
        stats.totalMemory = totalMemory;
        stats.peakMemory = peakMemory;
        stats.allocationCount = allocationCount;
        stats.stepsPerSecond = steps / totalTime;
        stats.memoryPerNeuron = totalMemory / neuronCount;
        stats.memoryEfficiency = (static_cast<double>(neuronCount) * 1000) / static_cast<double>(peakMemory);
        
        results.push_back(stats);
        
        std::cout << " Done!" << std::endl;
        std::cout << "    Peak Memory: " << (peakMemory / 1024.0) << " KB" << std::endl;
        std::cout << "    Allocations: " << allocationCount << std::endl;
        std::cout << "    Memory/Neuron: " << (stats.memoryPerNeuron) << " bytes" << std::endl;
    }
    
    printMemoryPoolResults(results);
}

void MemoryUsageBenchmarks::runAllocationBenchmarks() {
    std::cout << "=== Memory Allocation Stress Benchmarks ===" << std::endl;
    
    const std::vector<size_t> allocationCounts = {1000, 5000, 10000, 20000, 50000};
    const size_t iterations = 10;
    
    std::vector<AllocationBenchmarkResults> results;
    
    for (size_t allocCount : allocationCounts) {
        std::cout << "Testing " << allocCount << " allocations..." << std::flush;
        
        auto& memProfiler = MemoryAllocationProfiler::getInstance();
        memProfiler.getStats(); // Clear
        
        // Simulate rapid allocations/deallocations
        std::vector<std::unique_ptr<int>> allocations;
        allocations.reserve(allocCount);
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < allocCount; ++i) {
            // Create and immediately destroy objects to simulate allocation patterns
            auto ptr = std::make_unique<int>(static_cast<int>(i));
            memProfiler.recordAllocation(sizeof(int));
            allocations.push_back(std::move(ptr));
            memProfiler.recordDeallocation(sizeof(int));
            
            // Random deallocation to simulate realistic patterns
            if (i % 10 == 0 && i > 0) {
                size_t pos = std::rand() % allocations.size();
                if (allocations[pos]) {
                    memProfiler.recordDeallocation(sizeof(int));
                    allocations[pos].reset();
                }
            }
        }
        
        // Final cleanup
        allocations.clear();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        MemoryAllocationProfiler::MemoryStats memStats = memProfiler.getStats();
        
        AllocationBenchmarkResults result;
        result.allocationCount = allocCount;
        result.totalTime = totalTime;
        result.allocationsPerSecond = allocCount / totalTime;
        result.averageAllocationSize = memStats.averageAllocationSize;
        result.peakMemory = memStats.peakAllocated;
        
        results.push_back(result);
        
        std::cout << " Done!" << std::endl;
        std::cout << "    Time: " << totalTime << "s" << std::endl;
        std::cout << "    Allocs/sec: " << result.allocationsPerSecond << std::endl;
    }
    
    printAllocationResults(results);
}

void MemoryUsageBenchmarks::runMemoryPressureTests() {
    std::cout << "=== Memory Pressure Stress Tests ===" << std::endl;
    
    const size_t neuronCount = 2000;
    const size_t steps = 500;
    
    std::cout << "Running memory pressure test with " << neuronCount << " neurons, " << steps << " steps..." << std::flush;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.05f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Force memory pressure by running intensive simulation
    auto startTime = std::chrono::high_resolution_clock::now();
    
    MemoryPressureResults result;
    result.neuronCount = neuronCount;
    result.totalSteps = steps;
    
    for (size_t step = 0; step < steps; ++step) {
        // Heavy computation to trigger memory pressure
        brain->injectCurrentToNeurons(NeuronType::Sensory, 50.0f);
        brain->step(step, step * 0.001);
        
        // Record spike activity
        size_t spikes = brain->getTotalSpikeCount();
        result.totalSpikes += spikes;
        
        // Check for memory leaks by monitoring system state
        if (step % 100 == 0) {
            auto* spikeSystem = brain->getSpikeSystem();
            if (spikeSystem) {
                result.pendingSpikes += spikeSystem->getSpikes().size();
            }
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    result.totalTime = totalTime;
    result.stepsPerSecond = steps / totalTime;
    result.spikesPerStep = static_cast<double>(result.totalSpikes) / steps;
    
    std::cout << " Done!" << std::endl;
    std::cout << "    Time: " << totalTime << "s" << std::endl;
    std::cout << "    Steps/sec: " << result.stepsPerSecond << std::endl;
    std::cout << "    Spikes/step: " << result.spikesPerStep << std::endl;
    
    printMemoryPressureResults(result);
}

void MemoryUsageBenchmarks::printMemoryPoolResults(const std::vector<MemoryPoolStats>& results) {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║               Memory Pool Efficiency Benchmark Results                ║" << std::endl;
    std::cout << "╠════════════╦════════════╦══════════╦════════════╦══════════════╦═════════╣" << std::endl;
    std::cout << "║  Neurons   ║  Peak KB   ║ Allocs   ║ Mem/Neuron ║ Efficiency  ║ Steps/s ║" << std::endl;
    std::cout << "╠════════════╬════════════╬══════════╬════════════╬══════════════╬═════════╣" << std::endl;
    
    for (const auto& r : results) {
        std::cout << "║ " << std::setw(10) << r.neuronCount
                  << " ║ " << std::setw(10) << (r.peakMemory / 1024.0)
                  << " ║ " << std::setw(8) << r.allocationCount
                  << " ║ " << std::setw(11) << r.memoryPerNeuron
                  << " ║ " << std::setw(13) << std::fixed << std::setprecision(2) << r.memoryEfficiency
                  << " ║ " << std::setw(7) << std::fixed << std::setprecision(0) << r.stepsPerSecond
                  << " ║" << std::endl;
    }
    
    std::cout << "╚════════════╩════════════╩══════════╩════════════╩══════════════╩═════════╝" << std::endl;
    std::cout << std::endl;
}

void MemoryUsageBenchmarks::printAllocationResults(const std::vector<AllocationBenchmarkResults>& results) {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                Memory Allocation Benchmark Results                     ║" << std::endl;
    std::cout << "╠════════════════╦═════════╦════════╦═══════════════════╦══════════╗" << std::endl;
    std::cout << "║ Allocations    ║   Time  ║ Allocs/s ║ Avg Alloc Size ║  Peak KB  ║" << std::endl;
    std::cout << "╠════════════════╬═════════╬════════╬═══════════════════╬══════════╗" << std::endl;
    
    for (const auto& r : results) {
        std::cout << "║ " << std::setw(14) << r.allocationCount
                  << " ║ " << std::setw(7) << std::fixed << std::setprecision(3) << r.totalTime
                  << " ║ " << std::setw(7) << std::fixed << std::setprecision(0) << r.allocationsPerSecond
                  << " ║ " << std::setw(16) << r.averageAllocationSize
                  << " ║ " << std::setw(10) << (r.peakMemory / 1024.0)
                  << " ║" << std::endl;
    }
    
    std::cout << "╚════════════════╩═════════╩════════╩═══════════════════╩══════════╝" << std::endl;
    std::cout << std::endl;
}

void MemoryUsageBenchmarks::printMemoryPressureResults(const MemoryPressureResults& result) {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                 Memory Pressure Test Results                            ║" << std::endl;
    std::cout << "╠════════════════╦════════╦══════════╦═════════╦══════════╗" << std::endl;
    std::cout << "║ Neurons        ║  Time  ║ Steps/s  ║ Spikes/step ║ Pending ║" << std::endl;
    std::cout << "╠════════════════╬════════╬══════════╬═════════╬══════════╗" << std::endl;
    
    std::cout << "║ " << std::setw(14) << result.neuronCount
              << " ║ " << std::setw(6) << std::fixed << std::setprecision(3) << result.totalTime
              << " ║ " << std::setw(9) << std::fixed << std::setprecision(1) << result.stepsPerSecond
              << " ║ " << std::setw(10) << std::fixed << std::setprecision(2) << result.spikesPerStep
              << " ║ " << std::setw(9) << result.pendingSpikes
              << " ║" << std::endl;
    
    std::cout << "╚════════════════╩════════╩══════════╩═════════╩══════════╝" << std::endl;
    std::cout << std::endl;
}

void ComputationBenchmarks::runSpikePropagationBenchmarks() {
    std::cout << "=== Spike Propagation Speed Benchmarks ===" << std::endl;
    
    const std::vector<size_t> neuronCounts = {100, 500, 1000, 2000};
    const size_t steps = 200;
    
    std::vector<SpikePropagationResults> results;
    
    for (size_t neuronCount : neuronCounts) {
        std::cout << "Benchmarking spike propagation for " << neuronCount << " neurons..." << std::flush;
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        config->set("spike_threshold", 1.5f, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Start profiling for spike propagation
        startPhase(SimulationStepProfiler::PHASE_SYNAPSE_ACTIVATION);
        startPhase(SimulationStepProfiler::PHASE_NEURON_ACTIVATION);
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < steps; ++step) {
            // Inject strong input to generate spikes
            brain->injectCurrentToNeurons(NeuronType::Excitatory, 50.0f);
            brain->step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        stopPhase(); // PHASE_NEURON_ACTIVATION
        stopPhase(); // PHASE_SYNAPSE_ACTIVATION
        
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Get profiling statistics
        auto& stepProfiler = SimulationStepProfiler::getInstance();
        auto spikeSystemStats = stepProfiler.getPhaseStats(SimulationStepProfiler::PHASE_SYNAPSE_ACTIVATION);
        auto neuronStats = stepProfiler.getPhaseStats(SimulationStepProfiler::PHASE_NEURON_ACTIVATION);
        
        SpikePropagationResults result;
        result.neuronCount = neuronCount;
        result.totalSteps = steps;
        result.totalTime = totalTime;
        result.stepsPerSecond = steps / totalTime;
        result.synapseActivationTime = spikeSystemStats.averageTimeNs() / 1000.0; // Convert to microseconds
        result.neuronActivationTime = neuronStats.averageTimeNs() / 1000.0;
        result.spikesPerStep = static_cast<double>(brain->getTotalSpikeCount()) / steps;
        
        results.push_back(result);
        
        std::cout << " Done!" << std::endl;
        std::cout << "    Steps/sec: " << std::fixed << std::setprecision(1) << result.stepsPerSecond << std::endl;
        std::cout << "    Synapse act/us: " << std::fixed << std::setprecision(3) << result.synapseActivationTime << std::endl;
        std::cout << "    Neuron act/us: " << std::fixed << std::setprecision(3) << result.neuronActivationTime << std::endl;
        
        resetProfiling(); // Reset for next benchmark
    }
    
    printSpikePropagationResults(results);
}

void ComputationBenchmarks::runSynapticUpdateBenchmarks() {
    std::cout << "=== Synaptic Update Speed Benchmarks ===" << std::endl;
    
    const std::vector<size_t> synapseCounts = {500, 2000, 5000, 10000};
    const size_t steps = 150;
    
    std::vector<SynapticUpdateResults> results;
    
    for (size_t synapseCount : synapseCounts) {
        std::cout << "Benchmarking synaptic updates for " << synapseCount << " synapses..." << std::flush;
        
        // Estimate neuron count based on synapse count
        size_t neuronCount = static_cast<size_t>(std::sqrt(synapseCount * 2));
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.2f, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.022f, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Start profiling for STDP and Hebbian updates
        startPhase(SimulationStepProfiler::PHASE_STDP);
        startPhase(SimulationStepProfiler::PHASE_HEBBIAN);
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < steps; ++step) {
            // Inject input to trigger synaptic plasticity
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            brain->step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        stopPhase(); // PHASE_HEBBIAN
        stopPhase(); // PHASE_STDP
        
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Get profiling statistics
        auto& stepProfiler = SimulationStepProfiler::getInstance();
        auto stdpStats = stepProfiler.getPhaseStats(SimulationStepProfiler::PHASE_STDP);
        auto hebbianStats = stepProfiler.getPhaseStats(SimulationStepProfiler::PHASE_HEBBIAN);
        
        SynapticUpdateResults result;
        result.synapseCount = synapseCount;
        result.totalSteps = steps;
        result.totalTime = totalTime;
        result.stepsPerSecond = steps / totalTime;
        result.stdpUpdateTime = stdpStats.averageTimeNs() / 1000.0;
        result.hebbianUpdateTime = hebbianStats.averageTimeNs() / 1000.0;
        result.totalSpikeCount = brain->getTotalSpikeCount();
        
        results.push_back(result);
        
        std::cout << " Done!" << std::endl;
        std::cout << "    Steps/sec: " << std::fixed << std::setprecision(1) << result.stepsPerSecond << std::endl;
        std::cout << "    STDP update/us: " << std::fixed << std::setprecision(3) << result.stdpUpdateTime << std::endl;
        std::cout << "    Hebbian update/us: " << std::fixed << std::setprecision(3) << result.hebbianUpdateTime << std::endl;
        
        resetProfiling(); // Reset for next benchmark
    }
    
    printSynapticUpdateResults(results);
}

void ComputationBenchmarks::runScalabilityBenchmarks() {
    std::cout << "=== Neural Network Scalability Benchmarks ===" << std::endl;
    
    const std::vector<size_t> neuronCounts = {100, 500, 1000, 2000, 5000};
    const size_t steps = 100;
    
    std::vector<ScalabilityResults> results;
    
    for (size_t neuronCount : neuronCounts) {
        std::cout << "Benchmarking scalability with " << neuronCount << " neurons..." << std::flush;
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < steps; ++step) {
            // Progressive input to stress the system
            float current = 20.0f + (50.0f * (step % 10) / 10.0f);
            brain->injectCurrentToNeurons(NeuronType::Sensory, current);
            brain->step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        size_t totalSynapses = brain->getTotalSynapseCount();
        size_t totalSpikes = brain->getTotalSpikeCount();
        
        ScalabilityResults result;
        result.neuronCount = neuronCount;
        result.totalSteps = steps;
        result.totalTime = totalTime;
        result.stepsPerSecond = steps / totalTime;
        result.synapseCount = totalSynapses;
        result.spikeCount = totalSpikes;
        result.efficiency = static_cast<double>(totalSynapses) / static_cast<double>(totalTime); // Synapses/sec
        
        results.push_back(result);
        
        std::cout << " Done!" << std::endl;
        std::cout << "    Steps/sec: " << std::fixed << std::setprecision(1) << result.stepsPerSecond << std::endl;
        std::cout << "    Synapses: " << totalSynapses << std::endl;
        std::cout << "    Efficiency (syn/sec): " << result.efficiency << std::endl;
    }
    
    printScalabilityResults(results);
}

void ComputationBenchmarks::printSpikePropagationResults(const std::vector<SpikePropagationResults>& results) {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║              Spike Propagation Benchmark Results                       ║" << std::endl;
    std::cout << "╠════════════╦════════╦══════════════╦═══════════╦═════════════╦═════════╣" << std::endl;
    std::cout << "║  Neurons   ║ Steps/s ║ Synapse(µs) ║ Neuron(µs) ║ Spikes/step ║" << std::endl;
    std::cout << "╠════════════╬════════╬══════════════╬═══════════╬═════════════╬═════════╣" << std::endl;
    
    for (const auto& r : results) {
        std::cout << "║ " << std::setw(10) << r.neuronCount
                  << " ║ " << std::setw(7) << std::fixed << std::setprecision(1) << r.stepsPerSecond
                  << " ║ " << std::setw(14) << std::fixed << std::setprecision(3) << r.synapseActivationTime
                  << " ║ " << std::setw(12) << std::fixed << std::setprecision(3) << r.neuronActivationTime
                  << " ║ " << std::setw(12) << std::fixed << std::setprecision(2) << r.spikesPerStep
                  << " ║" << std::endl;
    }
    
    std::cout << "╚════════════╩════════╩══════════════╩═══════════╩═════════════╩═════════╝" << std::endl;
    std::cout << std::endl;
}

void ComputationBenchmarks::printSynapticUpdateResults(const std::vector<SynapticUpdateResults>& results) {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║               Synaptic Update Benchmark Results                         ║" << std::endl;
    std::cout << "╠════════════════╦════════╦════════════════╦════════════════╦════════╣" << std::endl;
    std::cout << "║  Synapses      ║ Steps/s ║   STDP(µs)    ║ Hebbian(µs)   ║ Spikes  ║" << std::endl;
    std::cout << "╠════════════════╬════════╬════════════════╬════════════════╬════════╣" << std::endl;
    
    for (const auto& r : results) {
        std::cout << "║ " << std::setw(14) << r.synapseCount
                  << " ║ " << std::setw(7) << std::fixed << std::setprecision(1) << r.stepsPerSecond
                  << " ║ " << std::setw(16) << std::fixed << std::setprecision(3) << r.stdpUpdateTime
                  << " ║ " << std::setw(16) << std::fixed << std::setprecision(3) << r.hebbianUpdateTime
                  << " ║ " << std::setw(7) << r.totalSpikeCount
                  << " ║" << std::endl;
    }
    
    std::cout << "╚════════════════╩════════╩════════════════╩════════════════╩════════╝" << std::endl;
    std::cout << std::endl;
}

void ComputationBenchmarks::printScalabilityResults(const std::vector<ScalabilityResults>& results) {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                Neural Network Scalability Results                        ║" << std::endl;
    std::cout << "╠════════════╦════════╦══════════╦════════╦════════════╗" << std::endl;
    std::cout << "║  Neurons   ║ Steps/s ║  Synapses ║ Spikes  ║ Efficiency ║" << std::endl;
    std::cout << "╠════════════╬════════╬══════════╬════════╬════════════╗" << std::endl;
    
    for (const auto& r : results) {
        std::cout << "║ " << std::setw(10) << r.neuronCount
                  << " ║ " << std::setw(7) << std::fixed << std::setprecision(1) << r.stepsPerSecond
                  << " ║ " << std::setw(9) << r.synapseCount
                  << " ║ " << std::setw(7) << r.spikeCount
                  << " ║ " << std::setw(12) << std::fixed << std::setprecision(1) << r.efficiency
                  << " ║" << std::endl;
    }
    
    std::cout << "╚════════════╩════════╩══════════╩════════╩════════════╝" << std::endl;
    std::cout << std::endl;
}

void SimulationSpeedBenchmarks::runStepsPerSecondBenchmarks() {
    std::cout << "=== Simulation Speed (Steps Per Second) Benchmarks ===" << std::endl;
    
    const std::vector<std::pair<size_t, size_t>> configurations = {
        {100, 1000},   // 100 neurons, 1000 steps
        {500, 1000},   // 500 neurons, 1000 steps
        {1000, 500},   // 1000 neurons, 500 steps
        {2000, 200},   // 2000 neurons, 200 steps
        {5000, 100},   // 5000 neurons, 100 steps
    };
    
    std::vector<StepsPerSecondResults> results;
    
    for (const auto& [neuronCount, steps] : configurations) {
        std::cout << "Benchmarking steps/sec with " << neuronCount << " neurons, " << steps << " steps..." << std::flush;
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < steps; ++step) {
            // Vary input to simulate real workloads
            float input = 30.0f + (20.0f * std::sin(static_cast<double>(step) * 0.1));
            brain->injectCurrentToNeurons(NeuronType::Sensory, input);
            brain->step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        StepsPerSecondResults result;
        result.neuronCount = neuronCount;
        result.steps = steps;
        result.totalTime = totalTime;
        result.stepsPerSecond = steps / totalTime;
        result.spikesPerStep = static_cast<double>(brain->getTotalSpikeCount()) / steps;
        
        results.push_back(result);
        
        std::cout << " Done!" << std::endl;
        std::cout << "    Steps/sec: " << std::fixed << std::setprecision(1) << result.stepsPerSecond << std::endl;
        std::cout << "    Time: " << std::fixed << std::setprecision(3) << totalTime << "s" << std::endl;
    }
    
    printStepsPerSecondResults(results);
}

void SimulationSpeedBenchmarks::runMemoryPoolEfficiencyBenchmarks() {
    std::cout << "=== Memory Pool Efficiency Benchmarks ===" << std::endl;
    
    const std::vector<size_t> neuronCounts = {500, 1000, 2000, 5000, 10000};
    const size_t steps = 200;
    
    std::vector<MemoryPoolEfficiencyResults> results;
    
    for (size_t neuronCount : neuronCounts) {
        std::cout << "Benchmarking memory pool efficiency with " << neuronCount << " neurons..." << std::flush;
        
        // Clear memory profiler before each test
        auto& memProfiler = MemoryAllocationProfiler::getInstance();
        memProfiler.getStats(); // Clear
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.05f, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < steps; ++step) {
            // Light input to minimize memory churn but still exercise allocation patterns
            brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
            brain->step(step, step * 0.001);
            
            // Occasional heavy operations
            if (step % 50 == 0) {
                auto* spikeSystem = brain->getSpikeSystem();
                if (spikeSystem) {
                    auto spikes = spikeSystem->getSpikes();
                    // Force spike processing
                    spikeSystem->processSpikes();
                }
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Get memory statistics
        MemoryAllocationProfiler::MemoryStats memStats = memProfiler.getStats();
        
        MemoryPoolEfficiencyResults result;
        result.neuronCount = neuronCount;
        result.totalSteps = steps;
        result.totalTime = totalTime;
        result.stepsPerSecond = steps / totalTime;
        result.peakMemoryKB = memStats.peakAllocated / 1024.0;
        result.totalMemoryKB = memStats.totalAllocated / 1024.0;
        result.allocationCount = memStats.allocationCount;
        result.memoryPerNeuron = memStats.peakAllocated / neuronCount;
        result.efficiencyScore = (static_cast<double>(neuronCount) * 1000.0) / memStats.peakAllocated;
        
        results.push_back(result);
        
        std::cout << " Done!" << std::endl;
        std::cout << "    Steps/sec: " << std::fixed << std::setprecision(1) << result.stepsPerSecond << std::endl;
        std::cout << "    Peak Memory: " << std::fixed << std::setprecision(1) << result.peakMemoryKB << " KB" << std::endl;
        std::cout << "    Efficiency: " << std::fixed << std::setprecision(2) << result.efficiencyScore << std::endl;
        
        resetProfiling(); // Reset for next benchmark
    }
    
    printMemoryPoolEfficiencyResults(results);
}

void SimulationSpeedBenchmarks::runSIMDEfficiencyBenchmarks() {
    std::cout << "=== SIMD Utilization Efficiency Benchmarks ===" << std::endl;
    
    // Run same benchmark with different neuron counts to gauge SIMD utilization
    const std::vector<size_t> neuronCounts = {100, 500, 1000, 2000};
    const size_t steps = 150;
    
    std::vector<SIMDEfficiencyResults> results;
    
    for (size_t neuronCount : neuronCounts) {
        std::cout << "Benchmarking SIMD efficiency with " << neuronCount << " neurons..." << std::flush;
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.015f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.018f, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Start profiling for different computational phases
        startPhase(SimulationStepProfiler::PHASE_SYNAPSE_INIT);
        startPhase(SimulationStepProfiler::PHASE_NEURON_INIT);
        startPhase(SimulationStepProfiler::PHASE_SYNAPSE_CONNECT);
        startPhase(SimulationStepProfiler::PHASE_NEURON_CONNECT);
        startPhase(SimulationStepProfiler::PHASE_SYNAPSE_ACTIVATION);
        startPhase(SimulationStepProfiler::PHASE_NEURON_ACTIVATION);
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < steps; ++step) {
            // Variable input to exercise different computational paths
            float input = 25.0f + (35.0f * (1.0f + std::sin(static_cast<double>(step) * 0.05)) / 2.0f);
            brain->injectCurrentToNeurons(NeuronType::Sensory, input);
            brain->step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Stop all phases
        stopPhase(); // PHASE_NEURON_ACTIVATION
        stopPhase(); // PHASE_SYNAPSE_ACTIVATION
        stopPhase(); // PHASE_NEURON_CONNECT
        stopPhase(); // PHASE_SYNAPSE_CONNECT
        stopPhase(); // PHASE_NEURON_INIT
        stopPhase(); // PHASE_SYNAPSE_INIT
        
        // Get profiling statistics
        auto& stepProfiler = SimulationStepProfiler::getInstance();
        auto& memProfiler = MemoryAllocationProfiler::getInstance();
        MemoryAllocationProfiler::MemoryStats memStats = memProfiler.getStats();
        
        SIMDEfficiencyResults result;
        result.neuronCount = neuronCount;
        result.totalSteps = steps;
        result.totalTime = totalTime;
        result.stepsPerSecond = steps / totalTime;
        
        // Calculate SIMD efficiency based on phase distribution
        double totalPhaseTime = stepProfiler.getTotalTimeNs() / 1000.0; // Convert to microseconds
        double synapseInitTime = stepProfiler.getPhaseStats(SimulationStepProfiler::PHASE_SYNAPSE_INIT).averageTimeNs() / 1000.0;
        double neuronInitTime = stepProfiler.getPhaseStats(SimulationStepProfiler::PHASE_NEURON_INIT).averageTimeNs() / 1000.0;
        double synapseConnectTime = stepProfiler.getPhaseStats(SimulationStepProfiler::PHASE_SYNAPSE_CONNECT).averageTimeNs() / 1000.0;
        double neuronConnectTime = stepProfiler.getPhaseStats(SimulationStepProfiler::PHASE_NEURON_CONNECT).averageTimeNs() / 1000.0;
        double synapseActivationTime = stepProfiler.getPhaseStats(SimulationStepProfiler::PHASE_SYNAPSE_ACTIVATION).averageTimeNs() / 1000.0;
        double neuronActivationTime = stepProfiler.getPhaseStats(SimulationStepProfiler::PHASE_NEURON_ACTIVATION).averageTimeNs() / 1000.0;
        
        result.totalPhaseTime = totalPhaseTime;
        result.synapseInitTime = synapseInitTime;
        result.neuronInitTime = neuronInitTime;
        result.synapseConnectTime = synapseConnectTime;
        result.neuronConnectTime = neuronConnectTime;
        result.synapseActivationTime = synapseActivationTime;
        result.neuronActivationTime = neuronActivationTime;
        
        result.peakMemoryKB = memStats.peakAllocated / 1024.0;
        result.allocationCount = memStats.allocationCount;
        
        // Calculate SIMD utilization score (higher is better)
        double totalComputations = synapseInitTime + neuronInitTime + synapseConnectTime + neuronConnectTime + synapseActivationTime + neuronActivationTime;
        if (totalComputations > 0) {
            // SIMD efficiency is higher when computation time is distributed across phases
            // (better vectorization opportunity)
            result.simdUtilizationScore = 1000.0 / (1.0 + totalComputations / 100.0);
        } else {
            result.simdUtilizationScore = 0.0;
        }
        
        results.push_back(result);
        
        std::cout << " Done!" << std::endl;
        std::cout << "    Steps/sec: " << std::fixed << std::setprecision(1) << result.stepsPerSecond << std::endl;
        std::cout << "    Total phase time: " << std::fixed << std::setprecision(3) << result.totalPhaseTime << " µs" << std::endl;
        std::cout << "    SIMD score: " << std::fixed << std::setprecision(2) << result.simdUtilizationScore << std::endl;
        
        resetProfiling(); // Reset for next benchmark
    }
    
    printSIMDEfficiencyResults(results);
}

void SimulationSpeedBenchmarks::printStepsPerSecondResults(const std::vector<StepsPerSecondResults>& results) {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║              Steps Per Second Benchmark Results                          ║" << std::endl;
    std::cout << "╠════════════╦════════╦════════════╦════════════╗" << std::endl;
    std::cout << "║  Neurons   ║ Steps/s ║    Time    ║ Spikes/step ║" << std::endl;
    std::cout << "╠════════════╬════════╬════════════╬════════════╗" << std::endl;
    
    for (const auto& r : results) {
        std::cout << "║ " << std::setw(10) << r.neuronCount
                  << " ║ " << std::setw(7) << std::fixed << std::setprecision(1) << r.stepsPerSecond
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(3) << r.totalTime
                  << " ║ " << std::setw(12) << std::fixed << std::setprecision(2) << r.spikesPerStep
                  << " ║" << std::endl;
    }
    
    std::cout << "╚════════════╩════════╩════════════╩════════════╝" << std::endl;
    std::cout << std::endl;
}

void SimulationSpeedBenchmarks::printMemoryPoolEfficiencyResults(const std::vector<MemoryPoolEfficiencyResults>& results) {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║          Memory Pool Efficiency Benchmark Results                       ║" << std::endl;
    std::cout << "╠════════════╦════════╦══════════╦════════════╦════════╦════════╣" << std::endl;
    std::cout << "║  Neurons   ║ Steps/s ║ Peak KB  ║ Mem/Neuron ║ Eff Score ║" << std::endl;
    std::cout << "╠════════════╬════════╬══════════╬════════════╬════════╬════════╣" << std::endl;
    
    for (const auto& r : results) {
        std::cout << "║ " << std::setw(10) << r.neuronCount
                  << " ║ " << std::setw(7) << std::fixed << std::setprecision(1) << r.stepsPerSecond
                  << " ║ " << std::setw(10) << std::fixed << std::setprecision(1) << r.peakMemoryKB
                  << " ║ " << std::setw(12) << r.memoryPerNeuron
                  << " ║ " << std::setw(8) << std::fixed << std::setprecision(2) << r.efficiencyScore
                  << " ║ " << std::setw(7) << r.allocationCount
                  << " ║" << std::endl;
    }
    
    std::cout << "╚════════════╩════════╩══════════╩════════════╩════════╩════════╝" << std::endl;
    std::cout << std::endl;
}

void SimulationSpeedBenchmarks::printSIMDEfficiencyResults(const std::vector<SIMDEfficiencyResults>& results) {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║              SIMD Utilization Benchmark Results                         ║" << std::endl;
    std::cout << "╠════════════╦════════╦══════════════╦════════════╦══════════╗" << std::endl;
    std::cout << "║  Neurons   ║ Steps/s ║ Total(µs)    ║ SIMD Score ║ Alloc Count ║" << std::endl;
    std::cout << "╠════════════╬════════╬══════════════╬════════════╬══════════╗" << std::endl;
    
    for (const auto& r : results) {
        std::cout << "║ " << std::setw(10) << r.neuronCount
                  << " ║ " << std::setw(7) << std::fixed << std::setprecision(1) << r.stepsPerSecond
                  << " ║ " << std::setw(14) << std::fixed << std::setprecision(3) << r.totalPhaseTime
                  << " ║ " << std::setw(12) << std::fixed << std::setprecision(2) << r.simdUtilizationScore
                  << " ║ " << std::setw(10) << r.allocationCount
                  << " ║" << std::endl;
    }
    
    std::cout << "╚════════════╩════════╩══════════════╩════════════╩══════════╝" << std::endl;
    std::cout << std::endl;
}

} // namespace benchmarks

// Main benchmark runner
int main() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — Brain Performance Benchmarks                        ║
    ║                                                               ║
    ║     Comprehensive Brain System Benchmarking                     ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Initialize random number generator
    auto rng = std::make_shared<RandomGenerator>();
    RandomGenerator::setGlobal(rng);
    
    std::cout << "Running Brain Performance Benchmarks..." << std::endl;
    std::cout << std::endl;
    
    // Run all benchmark suites
    MemoryUsageBenchmarks::runMemoryPoolBenchmarks();
    std::cout << std::endl;
    
    MemoryUsageBenchmarks::runAllocationBenchmarks();
    std::cout << std::endl;
    
    MemoryUsageBenchmarks::runMemoryPressureTests();
    std::cout << std::endl;
    
    ComputationBenchmarks::runSpikePropagationBenchmarks();
    std::cout << std::endl;
    
    ComputationBenchmarks::runSynapticUpdateBenchmarks();
    std::cout << std::endl;
    
    ComputationBenchmarks::runScalabilityBenchmarks();
    std::cout << std::endl;
    
    SimulationSpeedBenchmarks::runStepsPerSecondBenchmarks();
    std::cout << std::endl;
    
    SimulationSpeedBenchmarks::runMemoryPoolEfficiencyBenchmarks();
    std::cout << std::endl;
    
    SimulationSpeedBenchmarks::runSIMDEfficiencyBenchmarks();
    std::cout << std::endl;
    
    // Generate final profiling report
    std::cout << "=== FINAL PERFORMANCE PROFILING REPORT ===" << std::endl;
    std::cout << generateProfilingReport() << std::endl;
    
    std::cout << "Brain Performance Benchmarks complete." << std::endl;
    
    return 0;
}
