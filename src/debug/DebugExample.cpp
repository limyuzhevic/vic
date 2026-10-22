// src/debug/DebugExample.cpp
// Example demonstrating the new debugging and profiling features

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "debug/DebugManager.hpp"
#include <thread>

using namespace nlm;
using namespace nlm::debug;

void demonstrateDebugManager() {
    std::cout << "=== DebugManager Demonstration ===" << std::endl;
    
    DebugManager& debug = DebugManager::getInstance();
    debug.initialize(DebugLevel::Detailed);
    
    std::cout << "Debug level: " << static_cast<int>(debug.getDebugLevel()) << std::endl;
    
    // Example performance tracking
    auto metrics1 = debug.startPerformanceTracking("Neural initialization");
    std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Simulate work
    debug.stopPerformanceTracking(metrics1);
    
    auto metrics2 = debug.startPerformanceTracking("Synapse creation");
    std::this_thread::sleep_for(std::chrono::milliseconds(5)); // Simulate work
    debug.stopPerformanceTracking(metrics2);
    
    // Demonstrate memory tracking
    auto memory_tracker = debug.getMemoryTracker();
    memory_tracker->trackAllocation(nullptr, 1024, "Example allocation");
    memory_tracker->trackAllocation(nullptr, 2048, "Another allocation");
    
    std::cout << "Total allocated memory: " << memory_tracker->getTotalAllocated() << " bytes" << std::endl;
    
    // Demonstrate spike profiling
    auto spike_profiler = debug.getSpikeProfiler();
    spike_profiler->startProfiling();
    
    // Record some sample spike events
    spike_profiler->recordSpike(NeuronId(1), 10.5, 100, SpikeType::Regular);
    spike_profiler->recordPreSpike(SynapseId(1), NeuronId(2), 10.0);
    spike_profiler->recordPostSpike(SynapseId(1), NeuronId(3), 15.2);
    spike_profiler->recordSpike(NeuronId(2), 20.1, 200, SpikeType::Burst);
    
    spike_profiler->stopProfiling();
    
    // Generate reports
    spike_profiler->generateSTDPReport("debug_reports/stdp_analysis.txt");
    spike_profiler->generateSpikeActivityReport("debug_reports/spike_activity.txt");
    spike_profiler->generatePerformanceReport("debug_reports/spike_performance.txt");
    
    std::cout << "Debug reports generated in debug_reports/ directory" << std::endl;
    
    // Demonstrate configuration profiling
    auto config = std::make_shared<Config>();
    config->set("test_param", "test_value", ConfigSource::Runtime);
    
    auto config_profiler = debug.getConfigurationProfiler(config);
    
    // Create a simple brain for demonstration
    auto brain = std::make_shared<Brain>(config);
    if (brain->initialize()) {
        config_profiler->profilePerformance(brain, "Debug demonstration");
        config_profiler->profileMemoryUsage(brain);
        config_profiler->profileNeuralDynamics(brain);
        
        config_profiler->generateConfigurationReport("debug_reports/configuration_profile.txt");
        
        std::cout << "Configuration profile generated" << std::endl;
    }
    
    // Generate comprehensive debug report
    debug.generateDebugReport();
    
    debug.shutdown();
    
    std::cout << "DebugManager demonstration completed successfully!" << std::endl;
}

void demonstrateMemoryTrackingExample() {
    std::cout << "\n=== Memory Tracking Example ===" << std::endl;
    
    DebugManager& debug = DebugManager::getInstance();
    
    auto memory_tracker = debug.getMemoryTracker();
    
    // Simulate various memory allocations
    int* data1 = new int[100];
    int* data2 = new int[50];
    std::vector<float>* vector_data = new std::vector<float>(1000);
    
    memory_tracker->trackAllocation(data1, 100 * sizeof(int), "test_vector allocation");
    memory_tracker->trackAllocation(data2, 50 * sizeof(int), "test_matrix allocation");
    memory_tracker->trackAllocation(vector_data, sizeof(std::vector<float>), "vector container");
    
    std::cout << "After allocations: " << memory_tracker->getTotalAllocated() << " bytes" << std::endl;
    
    delete[] data1;
    delete[] data2;
    delete vector_data;
    
    // Note: In real usage, we'd also track deallocations
    // memory_tracker->trackDeallocation(data1);
    // memory_tracker->trackDeallocation(data2);
    // memory_tracker->trackDeallocation(vector_data);
    
    memory_tracker->printMemoryReport();
}

void demonstrateSpikeProfilerExample() {
    std::cout << "\n=== Spike Profiler Example ===" << std::endl;
    
    DebugManager& debug = DebugManager::getInstance();
    
    auto spike_profiler = debug.getSpikeProfiler();
    
    // Create a realistic spike pattern
    spike_profiler->startProfiling();
    
    // Simulate neural activity with different spike types
    for (int i = 0; i < 50; ++i) {
        double time = i * 1.0;
        
        if (i % 5 == 0) {
            spike_profiler->recordSpike(NeuronId(i), time, i * 10, SpikeType::Burst);
        } else if (i % 7 == 0) {
            spike_profiler->recordSpike(NeuronId(i), time, i * 10, SpikeType::Modulated);
        } else {
            spike_profiler->recordSpike(NeuronId(i), time, i * 10, SpikeType::Regular);
        }
        
        // Create pre-post spike pairs for STDP
        if (i > 0) {
            spike_profiler->recordPreSpike(SynapseId(i), NeuronId(i), time - 2.0);
            if (i % 3 == 0) {
                spike_profiler->recordPostSpike(SynapseId(i), NeuronId(i), time);
            }
        }
    }
    
    spike_profiler->stopProfiling();
    
    // Generate all reports
    spike_profiler->generateSTDPReport("debug_reports/stdp_detailed.txt");
    spike_profiler->generateSpikeActivityReport("debug_reports/spike_detailed.txt");
    spike_profiler->generatePerformanceReport("debug_reports/spike_perf_detailed.txt");
    
    std::cout << "Spike profiler reports generated" << std::endl;
    std::cout << "Total spikes recorded: " << spike_profiler->spike_events_.size() << std::endl;
    std::cout << "Total pre-post pairs: " << spike_profiler->pre_post_pairs_.size() << std::endl;
}

int main() {
    std::cout << "NLM Debug and Profiling Examples" << std::endl;
    std::cout << "================================" << std::endl;
    
    demonstrateDebugManager();
    demonstrateMemoryTrackingExample();
    demonstrateSpikeProfilerExample();
    
    std::cout << "\nAll examples completed successfully!" << std::endl;
    std::cout << "Check the debug_reports/ directory for detailed output files." << std::endl;
    
    return 0;
}