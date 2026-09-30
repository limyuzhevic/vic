// test_performance_monitor.cpp
// Tests for PerformanceMonitor functionality

#include "../performance/PerformanceMonitor.hpp"
#include <cassert>
#include <iostream>
#include <chrono>

void testPerformanceMonitorBasics() {
    std::cout << "Testing PerformanceMonitor basics..." << std::endl;
    
    nlm::PerformanceMonitor monitor;
    
    // Initially should have zero metrics
    auto metrics = monitor.getMetrics();
    assert(metrics.totalSteps == 0);
    assert(metrics.totalSpikes == 0);
    assert(metrics.neuronsProcessed == 0);
    assert(metrics.synapsesProcessed == 0);
    
    // Record some steps
    monitor.recordStep(100, 500, 2000);
    monitor.recordStep(200, 600, 3000);
    
    metrics = monitor.getMetrics();
    assert(metrics.totalSteps == 200);
    assert(metrics.neuronsProcessed == 1100);
    assert(metrics.synapsesProcessed == 5000);
    
    // Record spikes
    monitor.recordSpike(1);
    monitor.recordSpike(2);
    monitor.recordSpike(5);
    
    metrics = monitor.getMetrics();
    assert(metrics.totalSpikes == 3);
    
    // Record memory events
    monitor.recordMemoryEvent(nlm::MemoryEventType::ALLOCATION);
    monitor.recordMemoryEvent(nlm::MemoryEventType::COMPACTION);
    
    metrics = monitor.getMetrics();
    assert(metrics.memoryEvents == 2);
    
    // Record checkpoints
    monitor.recordCheckpoint(100, 1000, 1024 * 1024);  // 1MB
    monitor.recordCheckpoint(200, 2000, 2 * 1024 * 1024);  // 2MB
    
    metrics = monitor.getMetrics();
    assert(metrics.checkpointsCreated == 2);
    
    // Record replay events
    monitor.recordReplay(5, 100);
    monitor.recordReplay(3, 50);
    
    metrics = monitor.getMetrics();
    assert(metrics.replayEvents == 2);
    
    std::cout << "✓ PerformanceMonitor basics test passed" << std::endl;
}

void testPerformanceMonitorReset() {
    std::cout << "Testing PerformanceMonitor reset..." << std::endl;
    
    nlm::PerformanceMonitor monitor;
    
    // Add some data
    monitor.recordStep(100, 500, 2000);
    monitor.recordSpike(1);
    monitor.recordMemoryEvent(nlm::MemoryEventType::ALLOCATION);
    
    // Reset
    monitor.reset();
    
    auto metrics = monitor.getMetrics();
    assert(metrics.totalSteps == 0);
    assert(metrics.totalSpikes == 0);
    assert(metrics.neuronsProcessed == 0);
    assert(metrics.synapsesProcessed == 0);
    assert(metrics.memoryEvents == 0);
    
    std::cout << "✓ PerformanceMonitor reset test passed" << std::endl;
}

void testPerformanceMonitorSimulationTime() {
    std::cout << "Testing PerformanceMonitor simulation time tracking..." << std::endl;
    
    nlm::PerformanceMonitor monitor;
    
    // Record simulation time
    monitor.recordSimulationTime(10.5);
    monitor.recordSimulationTime(5.25);
    
    auto metrics = monitor.getMetrics();
    assert(metrics.simulationTime == 15.75);
    
    std::cout << "✓ PerformanceMonitor simulation time test passed" << std::endl;
}

void testPerformanceBenchmark() {
    std::cout << "Testing PerformanceBenchmark..." << std::endl;
    
    nlm::PerformanceBenchmark benchmark;
    
    // Add some benchmark results
    nlm::PerformanceMetrics metrics1;
    metrics1.totalSteps = 100;
    metrics1.totalSpikes = 50;
    metrics1.neuronsProcessed = 500;
    metrics1.synapsesProcessed = 2000;
    metrics1.simulationTime = 5.0;
    metrics1.memoryUsage = 1024 * 1024;
    
    nlm::PerformanceMetrics metrics2;
    metrics2.totalSteps = 200;
    metrics2.totalSpikes = 100;
    metrics2.neuronsProcessed = 800;
    metrics2.synapsesProcessed = 3000;
    metrics2.simulationTime = 8.0;
    metrics2.memoryUsage = 2 * 1024 * 1024;
    
    benchmark.addBenchmark("test1", metrics1);
    benchmark.addBenchmark("test2", metrics2);
    
    // Test average
    auto avg = benchmark.getAverage();
    assert(avg.totalSteps == 150);
    assert(avg.neuronsProcessed == 650);
    assert(avg.synapsesProcessed == 2500);
    
    // Test best performance
    auto best = benchmark.getBestPerformance();
    assert(best.totalSteps == 100);  // test1 has lower steps
    
    std::cout << "✓ PerformanceBenchmark test passed" << std::endl;
}

int main() {
    std::cout << "=== Performance Monitoring Tests ===" << std::endl << std::endl;
    
    try {
        testPerformanceMonitorBasics();
        testPerformanceMonitorReset();
        testPerformanceMonitorSimulationTime();
        testPerformanceBenchmark();
        
        std::cout << std::endl;
        std::cout << "=== All Performance Tests Passed ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
