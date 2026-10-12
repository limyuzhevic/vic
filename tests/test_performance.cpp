// Performance Optimization Tests
#include "brain/BrainAdapter.hpp"
#include "performance/EventSystem.hpp"
#include "performance/ConnectivityManager.hpp"
#include "performance/ParallelProcessor.hpp"
#include "performance/SIMDProcessor.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <chrono>

namespace test_performance {

void testBrainAdapterCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Test with BrainAdapter
    nlm::BrainAdapter adapter(config);
    
    // Verify adapter initialized correctly
    assert(adapter.isEventDrivenEnabled());  // Should be enabled by default
    assert(adapter.isMultithreadingEnabled());  // Should be enabled by default
    assert(adapter.isSIMDEnabled());  // Should be enabled by default
    assert(adapter.isSparseEnabled());  // Should be enabled by default
    
    // Verify thread count
    assert(adapter.getThreadCount() > 0);
    
    std::cout << "    testBrainAdapterCreation passed" << std::endl;
}

void testPerformanceStatistics() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("enable_performance_monitoring", true);
    
    nlm::BrainAdapter adapter(config);
    adapter.enablePerformanceMonitoring(true);
    
    // Run some steps
    for (nlm::SimulationStep step = 0; step < 5; ++step) {
        adapter.step(step);
    }
    
    // Check performance statistics
    auto stats = adapter.getPerformanceStats();
    assert(stats.activeNeurons > 0);
    assert(stats.totalSpikes >= 0);
    assert(stats.stepTimeMs >= 0.0);
    
    std::cout << "    testPerformanceStatistics passed" << std::endl;
}

void testMemoryPoolManager() {
    // Test memory pool manager directly
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_pool_size", static_cast<int64_t>(1024), nlm::ConfigSource::Default);
    config->set("synapse_pool_size", static_cast<int64_t>(4096), nlm::ConfigSource::Default);
    
    nlm::MemoryPoolManager& poolManager = nlm::MemoryPoolManager::getInstance();
    poolManager.setConfig(config);
    
    // Test allocation and deallocation
    nlm::NeuronId neuronId = poolManager.allocateNeuron(nlm::NeuronType::Internal);
    assert(neuronId != nlm::INVALID_NEURON_ID);
    
    nlm::SynapseId synapseId = poolManager.allocateSynapse(nlm::SynapseType::Excitatory);
    assert(synapseId != nlm::INVALID_SYNAPSE_ID);
    
    // Test deallocation
    poolManager.deallocateNeuron(neuronId);
    poolManager.deallocateSynapse(synapseId);
    
    // Check statistics
    assert(poolManager.getAllocatedCount() >= 0);
    assert(poolManager.getPeakUsage() >= 0);
    
    std::cout << "    testMemoryPoolManager passed" << std::endl;
}

void testEventSystem() {
    auto config = std::make_shared<nlm::Config>();
    
    nlm::EventSystem eventSystem(1000);
    
    // Test event scheduling
    nlm::EventSystem::Event event;
    event.type = nlm::EventSystem::EventType::Spike;
    event.step = 1;
    event.time = 0.001;
    event.neuronId = 42;
    event.synapseId = 24;
    event.weightChange = 0.5f;
    
    eventSystem.scheduleEvent(event);
    assert(eventSystem.hasEvents());
    
    // Test event processing
    nlm::EventSystem::Event processedEvent = eventSystem.processNextEvent();
    assert(processedEvent.neuronId == 42);
    assert(processedEvent.synapseId == 24);
    assert(processedEvent.weightChange == 0.5f);
    
    std::cout << "    testEventSystem passed" << std::endl;
}

void testConnectivityManager() {
    auto config = std::make_shared<nlm::Config>();
    
    nlm::ConnectivityManager connectivity(50, 0.1f);
    
    // Test adding connections
    connectivity.addConnection(1, 2, nlm::SynapseType::Excitatory);
    connectivity.addConnection(2, 3, nlm::SynapseType::Inhibitory);
    connectivity.addConnection(3, 4, nlm::SynapseType::Electrical);
    
    // Test connection existence
    assert(connectivity.hasConnection(1, 2));
    assert(connectivity.hasConnection(2, 3));
    assert(connectivity.hasConnection(3, 4));
    assert(!connectivity.hasConnection(1, 3));
    assert(!connectivity.hasConnection(2, 4));
    
    // Test connection statistics
    nlm::ConnectivityManager::Stats stats = connectivity.getStats();
    assert(stats.numNeurons == 50);
    assert(stats.numSynapses == 3);
    assert(stats.avgSynapsesPerNeuron > 0);
    
    // Test connection retrieval
    auto outgoing = connectivity.getConnectionsFrom(1);
    assert(outgoing.size() == 1);
    assert(outgoing[0].targetNeuron == 2);
    
    std::cout << "    testConnectivityManager passed" << std::endl;
}

void testParallelProcessor() {
    auto config = std::make_shared<nlm::Config>();
    config->set("thread_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::ParallelProcessor processor(2);
    
    // Test basic functionality
    size_t completedTasks = processor.getCompletedTasks();
    assert(completedTasks >= 0);
    
    // Test parallel processing
    std::vector<int> data(100, 0);
    
    // Simple parallel task
    processor.submitTask([&data]() {
        for (auto& val : data) {
            val++;
        }
    }, 0);
    
    processor.waitForCompletion();
    
    // Check that data was processed
    assert(data[0] == 1);
    
    std::cout << "    testParallelProcessor passed" << std::endl;
}

void testSIMDProcessor() {
    nlm::SIMDProcessor simd;
    
    // Test SIMD feature detection
    assert(simd.isSIMDAvailable() || !simd.isSIMDAvailable());  // Just check it doesn't crash
    
    // Test aligned memory allocation
    void* alignedPtr = simd.alignedAllocate(1024, 64);
    assert(alignedPtr != nullptr);
    assert(simd.isAligned(alignedPtr, 64));
    
    simd.alignedDeallocate(alignedPtr);
    
    std::cout << "    testSIMDProcessor passed" << std::endl;
}

void testPerformanceMonitoring() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("enable_performance_monitoring", true);
    
    nlm::BrainAdapter adapter(config);
    adapter.enablePerformanceMonitoring(true);
    
    // Run simulation steps
    auto startTime = std::chrono::high_resolution_clock::now();
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        adapter.step(step);
    }
    auto endTime = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    assert(duration >= 0.0);
    
    // Performance should be tracked
    auto stats = adapter.getPerformanceStats();
    assert(stats.stepTimeMs >= 0.0);
    
    std::cout << "    testPerformanceMonitoring passed" << std::endl;
}

void testOptimizationConfiguration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("enable_event_driven", true);
    config->set("enable_multithreading", true);
    config->set("enable_simd", true);
    config->set("enable_sparse_connectivity", true);
    config->set("thread_count", static_cast<int64_t>(4), nlm::ConfigSource::Default);
    
    nlm::BrainAdapter adapter(config);
    
    // Check optimization configuration
    auto optConfig = adapter.getOptimizationConfig();
    assert(optConfig.eventDrivenEnabled == true);
    assert(optConfig.multithreadingEnabled == true);
    assert(optConfig.simdEnabled == true);
    assert(optConfig.sparseConnectivityEnabled == true);
    assert(optConfig.threadCount == 4);
    
    std::cout << "    testOptimizationConfiguration passed" << std::endl;
}

void testBackwardCompatibility() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Test that BrainAdapter can be used as a Brain
    nlm::BrainAdapter brain(config);
    
    // All Brain methods should be available
    assert(brain.getTotalNeuronCount() > 0);
    assert(brain.getTotalSynapseCount() > 0);
    assert(brain.getRegionCount() == 1);
    
    // Test basic brain operations
    bool initialized = brain.initialize();
    assert(initialized);
    
    brain.step(0);
    brain.step(1);
    
    // Test statistics
    nlm::BrainAdapter::PerformanceStats stats = brain.getPerformanceStats();
    assert(stats.stepTimeMs >= 0.0);
    
    std::cout << "    testBackwardCompatibility passed" << std::endl;
}

void runAllPerformanceTests() {
    std::cout << "=== Performance Optimization Tests ===" << std::endl;
    
    testBrainAdapterCreation();
    testMemoryPoolManager();
    testEventSystem();
    testConnectivityManager();
    testParallelProcessor();
    testSIMDProcessor();
    testPerformanceStatistics();
    testPerformanceMonitoring();
    testOptimizationConfiguration();
    testBackwardCompatibility();
    
    std::cout << "\n=== All Performance Tests Passed ===" << std::endl;
}

} // namespace test_performance

// Main function for standalone execution
int main() {
    test_performance::runAllPerformanceTests();
    return 0;
}