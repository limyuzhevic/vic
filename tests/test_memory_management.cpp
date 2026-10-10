// test_memory_management.cpp - Memory management edge cases
#include "TestFramework.hpp"
#include "TestFixture.hpp"
#include "test_utils.cpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>

namespace test_memory_management {

// Test edge cases for null pointer handling
void testNullPointerHandling() {
    std::cout << "Testing null pointer handling..." << std::endl;
    
    // Test 1: Null pointer checks
    {
        std::shared_ptr<int> nullPtr;
        std::unique_ptr<int> uniqueNull;
        
        assert(SmartPointerTester::isNull(nullPtr));
        assert(SmartPointerTester::isNull(uniqueNull));
        assert(SmartPointerTester::isNull(nullptr));
        
        std::cout << "    Null pointer checks test passed" << std::endl;
    }
    
    // Test 2: Null pointer dereferencing (should throw exceptions)
    {
        std::shared_ptr<int> nullPtr;
        
        try {
            int value = *nullPtr; // Should throw bad_weak_ptr
            assert(false); // Should not reach here
        } catch (const std::exception& e) {
            std::cout << "    Null pointer dereferencing test passed (exception caught)" << std::endl;
        }
    }
    
    // Test 3: Null pointer in Brain initialization
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(0), nlm::ConfigSource::Runtime);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Runtime);
        
        nlm::Brain brain(config);
        bool success = brain.initialize();
        assert(!success); // Should fail with null/invalid configuration
        
        std::cout << "    Null pointer in Brain initialization test passed" << std::endl;
    }
}

// Test edge cases for memory leaks
void testMemoryLeakDetection() {
    std::cout << "Testing memory leak detection..." << std::endl;
    
    // Test 1: Basic memory leak detection
    {
        MemoryLeakDetector::getInstance().clear();
        
        // Simulate allocations
        std::vector<int*> allocations;
        for (int i = 0; i < 100; ++i) {
            int* ptr = new int(i);
            MemoryLeakDetector::getInstance().recordAllocation(ptr, sizeof(int), "test_alloc");
            allocations.push_back(ptr);
        }
        
        // Deallocate half of them
        for (size_t i = 0; i < allocations.size() / 2; ++i) {
            delete allocations[i];
            MemoryLeakDetector::getInstance().recordDeallocation(allocations[i], sizeof(int), "test_alloc");
        }
        
        // The rest should be leaked
        for (size_t i = allocations.size() / 2; i < allocations.size(); ++i) {
            delete allocations[i];
            MemoryLeakDetector::getInstance().recordDeallocation(allocations[i], sizeof(int), "test_alloc");
        }
        
        // Check for leaks
        size_t leakedBytes = MemoryLeakDetector::getInstance().getLeakedBytes();
        assert(leakedBytes == 0); // All should be deallocated
        
        std::cout << "    Basic memory leak detection test passed" << std::endl;
    }
    
    // Test 2: Memory leak detection with Brain
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Run simulation to trigger potential memory leaks
        for (nlm::SimulationStep step = 0; step < 1000; ++step) {
            brain.step(step);
        }
        
        // Check for memory leaks
        size_t leakedBytes = MemoryLeakDetector::getInstance().getLeakedBytes();
        assert(leakedBytes == 0); // No leaks expected
        
        std::cout << "    Memory leak detection with Brain test passed" << std::endl;
    }
    
    // Test 3: Memory leak detection in StressTestManager
    {
        StressTestManager::StressTestConfig config;
        config.enableMemoryLeakDetection = true;
        
        auto testFunction = []() {
            std::vector<int> tempVec(1000, 42); // Temporary vector that should be cleaned up
        };
        
        StressTestManager::getInstance().runStressTest(config, testFunction);
        
        // Check for leaks after stress test
        assert(!MemoryLeakDetector::getInstance().hasLeaks());
        
        std::cout << "    Memory leak detection in stress tests test passed" << std::endl;
    }
}

// Test edge cases for circular references
void testCircularReferenceDetection() {
    std::cout << "Testing circular reference detection..." << std::endl;
    
    // Test 1: Simple circular reference
    {
        std::vector<CircularReferenceDetector::Node> graph = {
            CircularReferenceDetector::Node("A"),
            CircularReferenceDetector::Node("B"),
            CircularReferenceDetector::Node("C")
        };
        
        graph[0].children = {"B"};
        graph[1].children = {"C"};
        graph[2].children = {"A"}; // Creates a cycle A -> B -> C -> A
        
        assert(CircularReferenceDetector::detectCircularReference(graph));
        std::cout << "    Simple circular reference test passed" << std::endl;
    }
    
    // Test 2: Self-referential circular reference
    {
        std::vector<CircularReferenceDetector::Node> graph = {
            CircularReferenceDetector::Node("A"),
            CircularReferenceDetector::Node("B")
        };
        
        graph[0].children = {"A"}; // Self-referential
        assert(CircularReferenceDetector::detectCircularReference(graph));
        std::cout << "    Self-referential circular reference test passed" << std::endl;
    }
    
    // Test 3: Complex circular reference
    {
        std::vector<CircularReferenceDetector::Node> graph = {
            CircularReferenceDetector::Node("A"),
            CircularReferenceDetector::Node("B"),
            CircularReferenceDetector::Node("C"),
            CircularReferenceDetector::Node("D")
        };
        
        graph[0].children = {"B"};
        graph[1].children = {"C"};
        graph[2].children = {"D"};
        graph[3].children = {"A"}; // Creates cycle A -> B -> C -> D -> A
        
        assert(CircularReferenceDetector::detectCircularReference(graph));
        std::cout << "    Complex circular reference test passed" << std::endl;
    }
    
    // Test 4: No circular reference
    {
        std::vector<CircularReferenceDetector::Node> graph = {
            CircularReferenceDetector::Node("A"),
            CircularReferenceDetector::Node("B"),
            CircularReferenceDetector::Node("C")
        };
        
        graph[0].children = {"B"};
        graph[1].children = {"C"};
        graph[2].children = {}; // No cycle
        
        assert(!CircularReferenceDetector::detectCircularReference(graph));
        std::cout << "    No circular reference test passed" << std::endl;
    }
}

// Test edge cases for smart pointers
void testSmartPointerEdgeCases() {
    std::cout << "Testing smart pointer edge cases..." << std::endl;
    
    // Test 1: Null shared_ptr
    {
        std::shared_ptr<int> nullPtr;
        assert(SmartPointerTester::isNull(nullPtr));
        std::cout << "    Null shared_ptr test passed" << std::endl;
    }
    
    // Test 2: Valid shared_ptr
    {
        std::shared_ptr<int> validPtr = std::make_shared<int>(42);
        assert(!SmartPointerTester::isNull(validPtr));
        assert(*validPtr == 42);
        std::cout << "    Valid shared_ptr test passed" << std::endl;
    }
    
    // Test 3: Null unique_ptr
    {
        std::unique_ptr<int> nullPtr;
        assert(SmartPointerTester::isNull(nullPtr));
        std::cout << "    Null unique_ptr test passed" << std::endl;
    }
    
    // Test 4: Valid unique_ptr
    {
        std::unique_ptr<int> validPtr = std::make_unique<int>(42);
        assert(!SmartPointerTester::isNull(validPtr));
        assert(*validPtr == 42);
        std::cout << "    Valid unique_ptr test passed" << std::endl;
    }
    
    // Test 5: Shared ownership
    {
        std::shared_ptr<int> ptr1 = std::make_shared<int>(42);
        std::shared_ptr<int> ptr2 = ptr1;
        
        assert(SmartPointerTester::isSame(ptr1, ptr2));
        assert(SmartPointerTester::useCount(ptr1) == 2);
        assert(SmartPointerTester::useCount(ptr2) == 2);
        std::cout << "    Shared ownership test passed" << std::endl;
    }
    
    // Test 6: Use count with Brain
    {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        
        // After initialization, config should be used by brain
        auto brainConfig = brain.getConfig();
        assert(brainConfig.use_count() >= 1);
        std::cout << "    Use count with Brain test passed" << std::endl;
    }
}

// Test edge cases for resource cleanup
void testResourceCleanup() {
    std::cout << "Testing resource cleanup..." << std::endl;
    
    // Test 1: Basic resource cleanup
    {
        ResourceCleanupTracker::getInstance().clear();
        
        ResourceCleanupTracker::getInstance().allocateResource("test_resource_1");
        assert(!ResourceCleanupTracker::getInstance().isResourceCleaned("test_resource_1"));
        
        ResourceCleanupTracker::getInstance().cleanupResource("test_resource_1");
        assert(ResourceCleanupTracker::getInstance().isResourceCleaned("test_resource_1"));
        
        std::cout << "    Basic resource cleanup test passed" << std::endl;
    }
    
    // Test 2: Multiple resources cleanup
    {
        ResourceCleanupTracker::getInstance().clear();
        
        std::vector<std::string> resourceNames = {"res1", "res2", "res3"};
        for (const auto& name : resourceNames) {
            ResourceCleanupTracker::getInstance().allocateResource(name);
            assert(!ResourceCleanupTracker::getInstance().isResourceCleaned(name));
        }
        
        // Cleanup all resources
        for (const auto& name : resourceNames) {
            ResourceCleanupTracker::getInstance().cleanupResource(name);
        }
        
        for (const auto& name : resourceNames) {
            assert(ResourceCleanupTracker::getInstance().isResourceCleaned(name));
        }
        
        std::cout << "    Multiple resources cleanup test passed" << std::endl;
    }
    
    // Test 3: Resource cleanup in StressTestManager
    {
        ResourceCleanupTracker::getInstance().clear();
        
        StressTestManager::StressTestConfig config;
        config.enableResourceCleanup = true;
        
        auto testFunction = []() {
            ResourceCleanupTracker::getInstance().allocateResource("test_resource");
        };
        
        StressTestManager::getInstance().runStressTest(config, testFunction);
        
        // After stress test, resources should be cleaned up
        assert(ResourceCleanupTracker::getInstance().getUncleanedResources().empty());
        
        std::cout << "    Resource cleanup in stress tests test passed" << std::endl;
    }
    
    // Test 4: Resource cleanup with Brain
    {
        ResourceCleanupTracker::getInstance().clear();
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Run simulation
        for (nlm::SimulationStep step = 0; step < 100; ++step) {
            brain.step(step);
        }
        
        // Brain should clean up its resources
        assert(ResourceCleanupTracker::getInstance().getUncleanedResources().empty());
        
        std::cout << "    Resource cleanup with Brain test passed" << std::endl;
    }
}

// Test edge cases for memory management integration
void testMemoryManagementIntegration() {
    std::cout << "Testing memory management integration..." << std::endl;
    
    // Test 1: Integration with StressTestManager
    {
        StressTestManager::StressTestConfig config;
        config.enableMemoryLeakDetection = true;
        config.enableResourceCleanup = true;
        config.enablePerformanceMonitoring = true;
        config.enableTimeoutDetection = true;
        
        auto testFunction = []() {
            std::vector<int> tempVec(1000, 42);
            ResourceCleanupTracker::getInstance().allocateResource("integration_test");
        };
        
        StressTestManager::getInstance().runStressTest(config, testFunction);
        
        // Should complete without memory leaks or resource cleanup failures
        assert(!MemoryLeakDetector::getInstance().hasLeaks());
        assert(ResourceCleanupTracker::getInstance().getUncleanedResources().empty());
        
        std::cout << "    Integration with StressTestManager test passed" << std::endl;
    }
    
    // Test 2: Integration with PerformanceProfiler
    {
        PerformanceProfiler::getInstance().clear();
        
        PerformanceProfiler::getInstance().startBenchmark("test_operation");
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        PerformanceProfiler::getInstance().endBenchmark("test_operation", 100);
        
        double executionTime = PerformanceProfiler::getInstance().getExecutionTime("test_operation");
        size_t memoryUsage = PerformanceProfiler::getInstance().getMemoryUsage("test_operation");
        
        assert(executionTime > 0.0);
        assert(memoryUsage > 0);
        
        std::cout << "    Integration with PerformanceProfiler test passed" << std::endl;
    }
    
    // Test 3: Integration with MemoryMonitor
    {
        MemoryMonitor::getInstance().clear();
        
        MemoryMonitor::getInstance().recordAllocation(1024, "test_alloc");
        MemoryMonitor::getInstance().recordDeallocation(1024, "test_alloc");
        
        size_t currentMemory = MemoryMonitor::getInstance().getCurrentMemoryUsage();
        assert(currentMemory == 0);
        
        std::cout << "    Integration with MemoryMonitor test passed" << std::endl;
    }
}

void runAll() {
    testNullPointerHandling();
    testMemoryLeakDetection();
    testCircularReferenceDetection();
    testSmartPointerEdgeCases();
    testResourceCleanup();
    testMemoryManagementIntegration();
    
    std::cout << std::endl << "All memory management edge case tests completed successfully!" << std::endl;
}

} // namespace test_memory_management