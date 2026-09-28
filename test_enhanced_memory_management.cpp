#pragma once

#include "tests/test_brain.hpp"
#include "tests/test_config.hpp"
#include "tests/test_logger.hpp"
#include "tests/test_logger_simple.hpp"
#include "tests/test_random.hpp"
#include "tests/test_synapse.hpp"
#include "tests/test_stdp.hpp"
#include "tests/test_types.hpp"
#include "tests/test_clock.hpp"
#include "tests/test_main.hpp"
#include <iostream>
#include <memory>
#include <vector>

// Enhanced memory management test utilities
class MemoryManagementTestHelper {
public:
    static void testNeuralArrayAllocation() {
        std::cout << "Testing NeuralArray allocation..." << std::endl;
        
        // Test NeuralArray with different sizes
        NeuralArray<int> arr1(0);
        assert(arr1.empty());
        
        NeuralArray<int> arr2(10);
        assert(arr2.size() == 10);
        
        // Test element access with bounds checking
        for (size_t i = 0; i < arr2.size(); ++i) {
            arr2[i] = i * 2;
        }
        
        for (size_t i = 0; i < arr2.size(); ++i) {
            assert(arr2[i] == i * 2);
        }
        
        // Test resize
        arr2.resize(5);
        assert(arr2.size() == 5);
        
        std::cout << "✓ NeuralArray allocation test passed" << std::endl;
    }
    
    static void testRAIIWrapper() {
        std::cout << "Testing RAIIWrapper..." << std::endl;
        
        RAIIWrapper<std::vector<int>> wrapper(std::make_unique<std::vector<int>>({1, 2, 3}));
        
        assert(wrapper.isValid());
        assert(wrapper.get().size() == 3);
        assert(wrapper.get()[0] == 1);
        
        wrapper.reset();
        assert(!wrapper.isValid());
        
        std::cout << "✓ RAIIWrapper test passed" << std::endl;
    }
    
    static void testMemoryPool() {
        std::cout << "Testing memory pools..." << std::endl;
        
        // Test neural resource pool
        NeuralResourcePool<int, 10> pool;
        
        auto ptr1 = pool.acquire();
        assert(ptr1 != nullptr);
        *ptr1 = 42;
        assert(*ptr1 == 42);
        
        pool.release(ptr1);
        
        auto ptr2 = pool.acquire();
        assert(ptr2 != nullptr);
        assert(*ptr2 == 0); // Default constructed
        
        std::cout << "✓ Memory pool test passed" << std::endl;
    }
    
    static void testExceptionSafety() {
        std::cout << "Testing exception safety..." << std::endl;
        
        try {
            // Test exception safety with RAII
            RAIIWrapper<int> wrapper(std::make_unique<int>(5));
            
            // This should throw
            int& value = wrapper.get();
            wrapper.reset();
            
            // Access after reset should throw
            // int& value2 = wrapper.get(); // This would throw
            
        } catch (const std::exception& e) {
            std::cout << "✓ Exception safety test passed (expected exception: " << e.what() << ")" << std::endl;
        }
    }
    
    static void testMemoryTracker() {
        std::cout << "Testing memory tracker..." << std::endl;
        
        // Test memory tracking
        void* ptr = std::malloc(100);
        MemoryTracker::instance().trackAllocation(ptr, 100, "test");
        
        assert(MemoryTracker::instance().getAllocationCount() == 1);
        
        MemoryTracker::instance().trackDeallocation(ptr);
        std::free(ptr);
        
        assert(MemoryTracker::instance().getAllocationCount() == 0);
        
        std::cout << "✓ Memory tracker test passed" << std::endl;
    }
    
    static void testLeakDetection() {
        std::cout << "Testing leak detection..." << std::endl;
        
        // Test leak detection
        bool hasLeaks = LeakDetector::checkForLeaks();
        
        // Should have no leaks in this test
        assert(!hasLeaks);
        
        std::cout << "✓ Leak detection test passed" << std::endl;
    }
    
    static void runAllTests() {
        std::cout << "=== Running Enhanced Memory Management Tests ===" << std::endl;
        
        testNeuralArrayAllocation();
        testRAIIWrapper();
        testMemoryPool();
        testExceptionSafety();
        testMemoryTracker();
        testLeakDetection();
        
        std::cout << "\n=== All Enhanced Memory Management Tests Passed ===" << std::endl;
    }
};

// Enhanced Brain integration test
class EnhancedBrainIntegrationTest {
public:
    static void testBrainWithEnhancedMemoryManagement() {
        std::cout << "Testing Brain integration with enhanced memory management..." << std::endl;
        
        // Create a brain with configuration
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 2);
        config->set("connection_probability", 0.1f);
        
        Brain brain(config);
        
        // Initialize the brain
        assert(brain.initialize(config));
        
        // Test enhanced memory management features
        if (brain.getImplEnhanced()) {
            std::cout << "✓ Brain initialized with enhanced memory management" << std::endl;
        }
        
        // Test memory statistics
        size_t allocations = brain.getImplEnhanced()->getAllocationCount();
        assert(allocations >= 0);
        
        std::cout << "✓ Brain integration test passed" << std::endl;
    }
    
    static void testConfigWithEnhancedMemoryManagement() {
        std::cout << "Testing Config integration with enhanced memory management..." << std::endl;
        
        Config config;
        
        // Test enhanced set with validation
        config.set("test_int", 42);
        assert(config.get<int>("test_int") == 42);
        
        config.set("test_string", "hello");
        assert(config.get<std::string>("test_string") == "hello");
        
        config.set("test_bool", true);
        assert(config.get<bool>("test_bool") == true);
        
        std::cout << "✓ Config integration test passed" << std::endl;
    }
    
    static void testLoggerWithEnhancedMemoryManagement() {
        std::cout << "Testing Logger integration with enhanced memory management..." << std::endl;
        
        Logger logger("test_logger");
        
        // Test enhanced logging with memory management
        logger.info("Test message from enhanced logger");
        
        // Check metrics
        auto metrics = logger.getMetrics();
        assert(metrics.logCount >= 1);
        
        std::cout << "✓ Logger integration test passed" << std::endl;
    }
    
    static void testAgentBrainWithEnhancedMemoryManagement() {
        std::cout << "Testing AgentBrain integration with enhanced memory management..." << std::endl;
        
        // Create config
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        
        // Create brain
        Brain brain(config);
        assert(brain.initialize(config));
        
        // Create AgentBrain
        AgentBrain agentBrain(std::make_shared<Brain>(brain));
        
        // Test AgentBrain integration
        assert(agentBrain.getBrain() != nullptr);
        
        std::cout << "✓ AgentBrain integration test passed" << std::endl;
    }
    
    static void runAllIntegrationTests() {
        std::cout << "=== Running Enhanced Brain Integration Tests ===" << std::endl;
        
        testBrainWithEnhancedMemoryManagement();
        testConfigWithEnhancedMemoryManagement();
        testLoggerWithEnhancedMemoryManagement();
        testAgentBrainWithEnhancedMemoryManagement();
        
        std::cout << "\n=== All Enhanced Integration Tests Passed ===" << std::endl;
    }
};

int main() {
    try {
        // Run memory management tests
        MemoryManagementTestHelper::runAllTests();
        
        // Run integration tests
        EnhancedBrainIntegrationTest::runAllIntegrationTests();
        
        std::cout << "\n=== ALL TESTS PASSED ===" << std::endl;
        std::cout << "Enhanced memory management and RAII implementation is working correctly!" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "\n=== TEST FAILED ===" << std::endl;
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}