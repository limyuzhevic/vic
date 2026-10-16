#include "PerformanceProfiler.hpp"
#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace nlm {

// Test operation profiler
void testOperationProfiler() {
    std::cout << "Testing OperationProfiler..." << std::endl;
    
    OperationProfiler profiler;
    
    // Test recording samples
    profiler.recordSample(OperationProfiler::OperationType::NEURON_UPDATE, "Neuron Update", 100);
    profiler.recordSample(OperationProfiler::OperationType::NEURON_UPDATE, "Neuron Update", 200);
    profiler.recordSample(OperationProfiler::OperationType::SYNAPSE_UPDATE, "Synapse Update", 50);
    
    // Get profiling data
    const OperationProfiler::ProfilingData* neuronData = profiler.getProfilingData(
        OperationProfiler::OperationType::NEURON_UPDATE);
    assert(neuronData != nullptr);
    assert(neuronData->callCount == 2);
    assert(neuronData->totalTimeMicroseconds == 300);
    assert(neuronData->avgTimeMicroseconds == 150.0);
    
    const OperationProfiler::ProfilingData* synapseData = profiler.getProfilingData(
        OperationProfiler::OperationType::SYNAPSE_UPDATE);
    assert(synapseData != nullptr);
    assert(synapseData->callCount == 1);
    assert(synapseData->totalTimeMicroseconds == 50);
    
    // Get hot operations
    auto hotOps = profiler.getHotOperations(2);
    assert(hotOps.size() == 2);
    assert(hotOps[0]->operationType == OperationProfiler::OperationType::NEURON_UPDATE);
    
    // Get frequent operations
    auto freqOps = profiler.getFrequentOperations(2);
    assert(freqOps.size() == 2);
    assert(freqOps[0]->operationType == OperationProfiler::OperationType::NEURON_UPDATE);
    
    // Generate report
    std::string report = profiler.generateReport();
    assert(!report.empty());
    
    std::cout << "✓ OperationProfiler tests passed" << std::endl;
}

// Test memory tracker
void testMemoryTracker() {
    std::cout << "Testing MemoryTracker..." << std::endl;
    
    MemoryTracker tracker;
    
    // Track allocations
    int* ptr1 = new int[10];
    tracker.trackAllocation(MemoryTracker::AllocationType::NEW, ptr1, 10 * sizeof(int), "test.cpp:10", 1000);
    
    int* ptr2 = (int*)malloc(20 * sizeof(int));
    tracker.trackAllocation(MemoryTracker::AllocationType::MALLOC, ptr2, 20 * sizeof(int), "test.cpp:20", 2000);
    
    // Track deallocations
    tracker.trackDeallocation(MemoryTracker::AllocationType::NEW, ptr1);
    delete[] ptr1;
    
    // Get current allocations
    auto currentAllocations = tracker.getCurrentAllocations();
    assert(currentAllocations.size() == 1);
    
    // Get allocation info
    const MemoryTracker::AllocationInfo* allocInfo = tracker.getAllocationInfo(ptr2);
    assert(allocInfo != nullptr);
    assert(allocInfo->pointer == ptr2);
    assert(allocInfo->size == 20 * sizeof(int));
    
    // Get memory stats
    MemoryTracker::MemoryStats stats = tracker.getMemoryStats();
    assert(stats.totalAllocations == 2);
    assert(stats.currentAllocations == 1);
    assert(stats.totalMemoryBytes == (10 + 20) * sizeof(int));
    
    // Generate memory report
    std::string report = tracker.generateMemoryReport();
    assert(!report.empty());
    
    free(ptr2);
    
    std::cout << "✓ MemoryTracker tests passed" << std::endl;
}

// Test performance metrics collector
void testPerformanceMetricsCollector() {
    std::cout << "Testing PerformanceMetricsCollector..." << std::endl;
    
    PerformanceMetricsCollector collector;
    
    // Create sample metrics
    PerformanceMetricsCollector::BrainMetrics metrics1;
    metrics1.simulationStep = 100;
    metrics1.simulationTime = 1.0;
    metrics1.stepTime = 0.01;
    metrics1.neuronCount = 1000;
    metrics1.activeNeurons = 500;
    metrics1.firingNeurons = 50;
    metrics1.totalSpikes = 1000;
    metrics1.neuronsPerSecond = 10000.0;
    metrics1.spikesPerSecond = 50000.0;
    metrics1.memoryUsageBytes = 1024 * 1024;  // 1MB
    metrics1.threadCount = 4;
    metrics1.activeThreads = 3;
    
    PerformanceMetricsCollector::BrainMetrics metrics2;
    metrics2.simulationStep = 101;
    metrics2.simulationTime = 1.01;
    metrics2.stepTime = 0.01;
    metrics2.neuronCount = 1000;
    metrics2.activeNeurons = 520;
    metrics2.firingNeurons = 60;
    metrics2.totalSpikes = 1060;
    metrics2.neuronsPerSecond = 10100.0;
    metrics2.spikesPerSecond = 51000.0;
    metrics2.memoryUsageBytes = 1024 * 1024;  // 1MB
    metrics2.threadCount = 4;
    metrics2.activeThreads = 3;
    
    // Record metrics
    collector.recordMetrics(metrics1);
    collector.recordMetrics(metrics2);
    
    // Get recent metrics
    auto recentMetrics = collector.getRecentMetrics(10);
    assert(recentMetrics.size() == 2);
    assert(recentMetrics[0].simulationStep == 100);
    assert(recentMetrics[1].simulationStep == 101);
    
    // Get metrics in range
    auto rangeMetrics = collector.getMetricsInRange(100, 101);
    assert(rangeMetrics.size() == 2);
    
    // Get performance summary
    PerformanceMetricsCollector::PerformanceSummary summary = collector.getPerformanceSummary();
    assert(summary.averageStepTime > 0.0);
    assert(summary.minStepTime > 0.0);
    assert(summary.maxStepTime > 0.0);
    assert(summary.averageNeuronsPerSecond > 0.0);
    assert(summary.averageSpikesPerSecond > 0.0);
    
    // Generate metrics report
    std::string report = collector.generateMetricsReport();
    assert(!report.empty());
    
    std::cout << "✓ PerformanceMetricsCollector tests passed" << std::endl;
}

// Test optimization analyzer
void testOptimizationAnalyzer() {
    std::cout << "Testing OptimizationAnalyzer..." << std::endl;
    
    OptimizationAnalyzer analyzer;
    
    // Create sample profiling data
    OperationProfiler profiler;
    profiler.recordSample(OperationProfiler::OperationType::NEURON_UPDATE, "Neuron Update", 1000);
    profiler.recordSample(OperationProfiler::OperationType::NEURON_UPDATE, "Neuron Update", 2000);
    profiler.recordSample(OperationProfiler::OperationType::SYNAPSE_UPDATE, "Synapse Update", 500);
    
    // Create sample memory tracking data
    MemoryTracker memoryTracker;
    
    // Create sample metrics
    PerformanceMetricsCollector metricsCollector;
    PerformanceMetricsCollector::BrainMetrics metrics;
    metrics.threadCount = 4;
    metrics.activeThreads = 2;
    metricsCollector.recordMetrics(metrics);
    
    // Analyze operation profiling
    auto opOpportunities = analyzer.analyzeOperationProfiling(profiler);
    assert(!opOpportunities.empty());
    
    // Analyze memory tracking
    auto memOpportunities = analyzer.analyzeMemoryTracking(memoryTracker);
    assert(!memOpportunities.empty());
    
    // Analyze metrics
    auto metricsOpportunities = analyzer.analyzeMetrics(metricsCollector);
    assert(!metricsOpportunities.empty());
    
    // Generate optimization report
    std::string report = analyzer.generateOptimizationReport(opOpportunities);
    assert(!report.empty());
    
    // Prioritize opportunities
    std::vector<size_t> priorities = analyzer.prioritizeOpportunities(opOpportunities);
    assert(!priorities.empty());
    
    // Get recommendations
    PerformanceMetricsCollector::BottleneckAnalysis bottleneck;
    bottleneck.bottleneckType = "cpu";
    bottleneck.recommendations = {"Increase multithreading", "Optimize neural computations"};
    
    std::vector<std::string> recommendations = analyzer.getRecommendations(bottleneck);
    assert(!recommendations.empty());
    
    std::cout << "✓ OptimizationAnalyzer tests passed" << std::endl;
}

// Test memory pool tracker
void testMemoryPoolTracker() {
    std::cout << "Testing MemoryPoolTracker..." << std::endl;
    
    MemoryPoolTracker tracker;
    
    // Track memory pool allocations
    tracker.trackMemoryPoolAllocation("NeuronPool", sizeof(float), 100);
    tracker.trackMemoryPoolAllocation("SynapsePool", sizeof(float), 200);
    
    // Track memory pool deallocations
    tracker.trackMemoryPoolDeallocation("NeuronPool", 50);
    tracker.trackMemoryPoolDeallocation("SynapsePool", 100);
    
    // Get pool stats
    std::vector<MemoryPoolTracker::PoolStats> poolStats = tracker.getPoolStats();
    assert(poolStats.size() == 2);
    
    // Check specific pool
    bool neuronPoolFound = false;
    for (const auto& stats : poolStats) {
        if (stats.poolName == "NeuronPool") {
            neuronPoolFound = true;
            assert(stats.totalObjectsAllocated == 100);
            assert(stats.totalObjectsDeallocated == 50);
            assert(stats.currentObjects == 50);
            assert(stats.allocationEfficiency == 0.5);
        }
    }
    assert(neuronPoolFound);
    
    // Get pool recommendations
    std::vector<std::string> recommendations = tracker.getPoolRecommendations();
    assert(!recommendations.empty());
    
    // Generate pool report
    std::string report = tracker.generatePoolReport();
    assert(!report.empty());
    
    std::cout << "✓ MemoryPoolTracker tests passed" << std::endl;
}

// Test brain performance profiler integration
void testBrainPerformanceProfiler() {
    std::cout << "Testing BrainPerformanceProfiler integration..." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    
    // Create brain
    Brain brain(config);
    
    // Initialize performance profiler
    if (auto* profiler = &brain.getPerformanceProfiler()) {
        profiler->setOperationProfilingEnabled(true);
        profiler->setMemoryTrackingEnabled(true);
        profiler->setMetricsCollectionEnabled(true);
    }
    
    // Check profiler is active
    assert(brain.isPerformanceMonitoring() == false);  // Not started yet
    
    // Start performance monitoring
    brain.startPerformanceMonitoring();
    assert(brain.isPerformanceMonitoring() == true);
    
    // Record performance snapshot
    brain.recordPerformanceSnapshot();
    
    // Stop performance monitoring
    brain.stopPerformanceMonitoring();
    assert(brain.isPerformanceMonitoring() == false);
    
    // Generate performance report
    std::string report = brain.generatePerformanceReport();
    assert(!report.empty());
    
    // Generate optimization report
    std::string optimizationReport = brain.generateOptimizationReport();
    assert(!optimizationReport.empty());
    
    std::cout << "✓ BrainPerformanceProfiler integration tests passed" << std::endl;
}

int main() {
    try {
        std::cout << "Running Performance Profiling Tests..." << std::endl;
        std::cout << std::endl;
        
        testOperationProfiler();
        std::cout << std::endl;
        
        testMemoryTracker();
        std::cout << std::endl;
        
        testPerformanceMetricsCollector();
        std::cout << std::endl;
        
        testOptimizationAnalyzer();
        std::cout << std::endl;
        
        testMemoryPoolTracker();
        std::cout << std::endl;
        
        testBrainPerformanceProfiler();
        std::cout << std::endl;
        
        std::cout << "All performance profiling tests passed!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
}

} // namespace nlm