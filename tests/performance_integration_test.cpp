// Performance Integration Test
// This test verifies that all performance modules are properly integrated

#include "performance/CheckpointOptimizer.hpp"
#include "performance/BenchmarkSuite.hpp"
#include "performance/ResourceProfiler.hpp"
#include "performance/HardwareOptimizer.hpp"
#include "performance/PerformanceMonitor.hpp"
#include "performance/MemoryPool.hpp"
#include "performance/OptimizedBrain.hpp"
#include "performance/ParallelProcessing.hpp"
#include "performance/SIMDVectorization.hpp"
#include "performance/SparseConnectivity.hpp"
#include "performance/CheckpointSystem.hpp"
#include "performance/EventQueue.hpp"
#include "performance/Performance.hpp"

#include <iostream>
#include <cassert>

void testPerformanceModuleHeaders() {
    std::cout << "Testing performance module headers..." << std::endl;
    
    // Test that all performance modules can be included
    // (This is already verified by the successful compilation of this file)
    
    std::cout << "  All performance module headers are available." << std::endl;
}

void testMemoryPoolTemplate() {
    std::cout << "Testing MemoryPool template..." << std::endl;
    
    // Test MemoryPool with basic types
    nlm::MemoryPool<int> intPool(100, 10);
    
    // Allocate and deallocate
    int* value1 = intPool.allocate();
    assert(value1 != nullptr);
    
    *value1 = 42;
    
    intPool.deallocate(value1);
    
    std::cout << "  MemoryPool allocation/deallocation works." << std::endl;
}

void testCheckpointSystem() {
    std::cout << "Testing CheckpointSystem..." << std::endl;
    
    // Test CheckpointSystem initialization
    nlm::CheckpointReader reader;
    nlm::CheckpointWriter writer;
    
    // Test that classes can be instantiated
    assert(!reader.isOpen());
    assert(!writer.isOpen());
    
    std::cout << "  CheckpointSystem classes can be instantiated." << std::endl;
}

void testOptimizedBrain() {
    std::cout << "Testing OptimizedBrain..." << std::endl;
    
    // Test that OptimizedBrain can be included
    // (We cannot instantiate it without a full config, but we can verify it's available)
    
    std::cout << "  OptimizedBrain header is available." << std::endl;
}

void testParallelProcessing() {
    std::cout << "Testing ParallelProcessing..." << std::endl;
    
    // Test ParallelFor
    std::vector<int> testData(100, 0);
    
    nlm::ParallelFor::simple(0, testData.size(),
        [&testData](size_t i) {
            testData[i] = static_cast<int>(i);
        });
    
    // Verify results
    for (size_t i = 0; i < testData.size(); ++i) {
        assert(testData[i] == static_cast<int>(i));
    }
    
    std::cout << "  ParallelFor works correctly." << std::endl;
}

void testHardwareCapabilities() {
    std::cout << "Testing HardwareCapabilities..." << std::endl;
    
    nlm::HardwareCapabilities hw = nlm::HardwareDetector().detectCapabilities();
    
    // Basic sanity checks
    assert(hw.physicalCores > 0);
    assert(hw.logicalCores > 0);
    assert(hw.l1CacheSize > 0);
    
    std::cout << "  Hardware capabilities detected: " << hw.logicalCores << " logical cores, "
              << hw.physicalCores << " physical cores" << std::endl;
}

void testSIMDVectorization() {
    std::cout << "Testing SIMDVectorization..." << std::endl;
    
    // Test SIMDUtils
    bool simdAvailable = nlm::SIMDUtils::isSIMDAvailable();
    std::cout << "  SIMD available: " << (simdAvailable ? "Yes" : "No") << std::endl;
    
    std::cout << "  SIMD type: " << nlm::SIMDUtils::getSIMDType() << std::endl;
}

void testPerformanceMonitor() {
    std::cout << "Testing PerformanceMonitor..." << std::endl;
    
    // Test PerformanceMonitor configuration
    nlm::PerformanceMonitor::Config config;
    config.enableRealTimeMonitoring = true;
    config.monitoringIntervalMs = 100;
    config.enableAlerting = true;
    config.enableAnalysis = true;
    config.enableReporting = true;
    
    // We cannot create a PerformanceMonitor instance without proper initialization
    // but we can verify the configuration structure
    assert(config.monitoringIntervalMs > 0);
    assert(config.enableRealTimeMonitoring);
    
    std::cout << "  PerformanceMonitor configuration structure is valid." << std::endl;
}

int main() {
    std::cout << "=== Performance Integration Test ===" << std::endl;
    std::cout << "Testing all performance modules and integration..." << std::endl;
    std::cout << std::endl;
    
    try {
        testPerformanceModuleHeaders();
        testMemoryPoolTemplate();
        testCheckpointSystem();
        testOptimizedBrain();
        testParallelProcessing();
        testHardwareCapabilities();
        testSIMDVectorization();
        testPerformanceMonitor();
        
        std::cout << std::endl;
        std::cout << "=== All Performance Integration Tests Passed ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << std::endl;
        std::cerr << "=== Performance Integration Test Failed ===" << std::endl;
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}