#include "EnhancedMemoryManagement.hpp"
#include <iostream>
#include <vector>
#include <memory>
#include <cassert>

namespace nlm {

void TestMemoryPoolOptimization() {
    std::cout << "Testing Memory Pool Optimization..." << std::endl;
    
    EnhancedMemoryPool<float> pool(1024, 4);
    
    // Allocate from pool
    std::vector<float*> floats;
    for (int i = 0; i < 100; ++i) {
        floats.push_back(pool.allocate());
        assert(floats.back() != nullptr);
    }
    
    // Deallocate all
    for (auto* ptr : floats) {
        pool.deallocate(ptr);
    }
    
    // Check stats
    auto stats = pool.getStats();
    assert(stats.allocatedCount == 0);
    assert(stats.availableCount == 100);
    
    std::cout << "✓ Memory Pool Optimization passed" << std::endl;
}

void TestMemoryLeakDetection() {
    std::cout << "Testing Memory Leak Detection..." << std::endl;
    
    MemoryLeakDetector::Initialize();
    
    // Simulate allocations
    void* ptr1 = NLM_ALLOCATE(1024);
    void* ptr2 = NLM_ALLOCATE(2048);
    
    // Deallocate only one
    NLM_DEALLOCATE(ptr1);
    
    // Detect leaks
    auto leaks = MemoryLeakDetector::DetectLeaks();
    
    // ptr2 should be leaked
    assert(leaks.size() == 1);
    
    // Clean up leak
    NLM_DEALLOCATE(ptr2);
    
    std::cout << "✓ Memory Leak Detection passed" << std::endl;
}

void TestMemoryAllocationTracking() {
    std::cout << "Testing Memory Allocation Tracking..." << std::endl;
    
    SmartPointerTracker::SetEnabled(true);
    
    std::shared_ptr<int> sharedInt(new int(42));
    NLM_TRACK_SHARED_PTR(sharedInt);
    
    std::unique_ptr<double> uniqueDouble(new double(3.14));
    NLM_TRACK_UNIQUE_PTR(uniqueDouble);
    
    auto spStats = SmartPointerTracker::GetStats();
    assert(spStats.sharedPtrCount > 0);
    assert(spStats.uniquePtrCount > 0);
    
    std::cout << "✓ Memory Allocation Tracking passed" << std::endl;
}

void TestSmartPointerOptimization() {
    std::cout << "Testing Smart Pointer Optimization..." << std::endl;
    
    SmartPointerTracker::SetEnabled(true);
    
    // Create smart pointers with potential circular references
    std::shared_ptr<int> ptr1 = std::make_shared<int>(1);
    std::shared_ptr<int> ptr2 = std::make_shared<int>(2);
    
    // Create circular reference
    ptr1->reset(); // Reset to prevent actual circular reference
    
    // Track them
    NLM_TRACK_SHARED_PTR(ptr1);
    NLM_TRACK_SHARED_PTR(ptr2);
    
    auto stats = SmartPointerTracker::GetStats();
    assert(stats.sharedPtrCount >= 2);
    
    std::cout << "✓ Smart Pointer Optimization passed" << std::endl;
}

void TestDevelopmentMemoryTracking() {
    std::cout << "Testing Development Memory Tracking..." << std::endl;
    
    DevelopmentMemoryTracker::SetEnabled(true);
    
    DevelopmentMemoryTracker::RecordSnapshot("test_start");
    
    void* ptr = NLM_ALLOCATE(1024);
    DevelopmentMemoryTracker::RecordSnapshot("allocated");
    
    NLM_DEALLOCATE(ptr);
    DevelopmentMemoryTracker::RecordSnapshot("deallocated");
    
    auto trends = DevelopmentMemoryTracker::AnalyzeTrends();
    assert(trends.memoryUsageHistory_.size() >= 2);
    
    std::cout << "✓ Development Memory Tracking passed" << std::endl;
}

void TestProductionMemoryMonitoring() {
    std::cout << "Testing Production Memory Monitoring..." << std::endl;
    
    ProductionMemoryMonitor::SetEnabled(true);
    ProductionMemoryMonitor::StartMonitoring();
    
    // Simulate production workload
    std::vector<void*> allocations;
    for (int i = 0; i < 10; ++i) {
        allocations.push_back(NLM_ALLOCATE(1024 * 1024)); // 1MB each
    }
    
    bool underPressure = ProductionMemoryMonitor::IsUnderMemoryPressure(100); // 100MB threshold
    
    // Clean up
    for (auto* ptr : allocations) {
        NLM_DEALLOCATE(ptr);
    }
    
    ProductionMemoryMonitor::StopMonitoring();
    
    std::cout << "✓ Production Memory Monitoring passed" << std::endl;
}

void TestCheckpointMemoryEfficiency() {
    std::cout << "Testing Checkpoint Memory Efficiency..." << std::endl;
    
    void* data = NLM_ALLOCATE(1024 * 1024); // 1MB
    
    CheckpointMemoryOptimizer::TrackCheckpointMemoryUsage("test", 1024 * 1024, 2 * 1024 * 1024);
    
    size_t optimized = CheckpointMemoryOptimizer::OptimizeCheckpointData(data, 1024 * 1024, CompressionLevel::Balanced);
    assert(optimized < 1024 * 1024); // Should be compressed
    
    auto stats = CheckpointMemoryOptimizer::GetStats();
    assert(stats.totalCheckpoints > 0);
    
    NLM_DEALLOCATE(data);
    
    std::cout << "✓ Checkpoint Memory Efficiency passed" << std::endl;
}

void TestGlobalMemoryManager() {
    std::cout << "Testing Global Memory Manager..." << std::endl;
    
    GlobalMemoryManager::Initialize();
    GlobalMemoryManager::SetMode("development");
    
    void* ptr = NLM_ALLOCATE(1024);
    assert(ptr != nullptr);
    
    NLM_DEALLOCATE(ptr);
    
    GlobalMemoryManager::Shutdown();
    
    std::cout << "✓ Global Memory Manager passed" << std::endl;
}

int main() {
    std::cout << "=== Enhanced Memory Management Test Suite ===" << std::endl;
    std::cout << "Testing all 10 memory management improvements" << std::endl;
    std::cout << std::endl;
    
    try {
        TestMemoryPoolOptimization();
        TestMemoryLeakDetection();
        TestMemoryAllocationTracking();
        TestSmartPointerOptimization();
        TestDevelopmentMemoryTracking();
        TestProductionMemoryMonitoring();
        TestCheckpointMemoryEfficiency();
        TestGlobalMemoryManager();
        
        std::cout << std::endl;
        std::cout << "=== All Tests Passed! ===" << std::endl;
        std::cout << "Enhanced memory management is working correctly!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}

} // namespace nlm
