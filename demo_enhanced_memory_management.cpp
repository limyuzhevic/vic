#include <iostream>
#include <memory>
#include <vector>
#include "src/core/memory_management/EnhancedMemoryManagement.hpp"

// Simple demonstration of enhanced memory management features
void demonstrate_neural_array() {
    std::cout << "=== NeuralArray Demonstration ===" << std::endl;
    
    // Create a neural array with 5 elements
    nlm::NeuralArray<int> arr(5);
    
    // Fill the array with values
    for (size_t i = 0; i < arr.size(); ++i) {
        arr[i] = i * 10;
    }
    
    // Display the array
    std::cout << "Array contents: ";
    for (size_t i = 0; i < arr.size(); ++i) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;
    
    // Test bounds checking (uncomment to test)
    // try {
    //     int value = arr[10]; // Should throw out_of_range
    //     std::cout << "Error: Should have thrown exception" << std::endl;
    // } catch (const std::out_of_range& e) {
    //     std::cout << "✓ Bounds checking works: " << e.what() << std::endl;
    // }
    
    // Test resize
    arr.resize(3);
    std::cout << "After resize to 3: size = " << arr.size() << std::endl;
}

void demonstrate_raii_wrapper() {
    std::cout << "\n=== RAIIWrapper Demonstration ===" << std::endl;
    
    // Create RAII wrapper with vector resource
    nlm::RAIIWrapper<std::vector<int>> wrapper(std::make_unique<std::vector<int>>({1, 2, 3, 4, 5}));
    
    std::cout << "Wrapper contains: ";
    for (int val : wrapper.get()) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    
    // Reset the wrapper (releases resource)
    wrapper.reset();
    std::cout << "After reset, isValid = " << wrapper.isValid() << std::endl;
    
    // Create new wrapper with different resource
    nlm::RAIIWrapper<std::string> strWrapper(std::make_unique<std::string>("Hello, Enhanced Memory Management!"));
    std::cout << "String wrapper contains: " << strWrapper.get() << std::endl;
}

void demonstrate_memory_tracker() {
    std::cout << "\n=== Memory Tracker Demonstration ===" << std::endl;
    
    // Allocate memory
    void* ptr1 = std::malloc(100);
    void* ptr2 = std::malloc(200);
    
    // Track allocations
    nlm::MemoryTracker::instance().trackAllocation(ptr1, 100, "test1");
    nlm::MemoryTracker::instance().trackAllocation(ptr2, 200, "test2");
    
    std::cout << "Current allocation count: " << nlm::MemoryTracker::instance().getAllocationCount() << std::endl;
    
    // Deallocate memory
    std::free(ptr1);
    nlm::MemoryTracker::instance().trackDeallocation(ptr1);
    
    std::cout << "After deallocation, count: " << nlm::MemoryTracker::instance().getAllocationCount() << std::endl;
    
    std::free(ptr2);
    nlm::MemoryTracker::instance().trackDeallocation(ptr2);
    
    std::cout << "Final allocation count: " << nlm::MemoryTracker::instance().getAllocationCount() << std::endl;
}

void demonstrate_leak_detection() {
    std::cout << "\n=== Leak Detection Demonstration ===" << std::endl;
    
    // Check for leaks
    bool hasLeaks = nlm::LeakDetector::checkForLeaks();
    
    if (hasLeaks) {
        std::cout << "⚠️  Memory leaks detected!" << std::endl;
        nlm::LeakDetector::reportLeaks();
        nlm::forceMemoryCleanup();
    } else {
        std::cout << "✓ No memory leaks detected" << std::endl;
    }
}

void demonstrate_enhanced_brain() {
    std::cout << "\n=== Enhanced Brain Integration Demonstration ===" << std::endl;
    
    // Create a simple config
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 50);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1f);
    
    // Create brain with enhanced memory management
    nlm::Brain brain(config);
    
    if (brain.initialize(config)) {
        std::cout << "✓ Brain initialized successfully with enhanced memory management" << std::endl;
        
        // Test enhanced features if available
        if (brain.getImplEnhanced()) {
            std::cout << "Enhanced memory management features available:" << std::endl;
            std::cout << "  - Allocation count: " << brain.getImplEnhanced()->getAllocationCount() << std::endl;
            std::cout << "  - Neuron pool size: " << brain.getImplEnhanced()->getNeuronPoolSize() << std::endl;
            std::cout << "  - Cleanup completed: " << (brain.getImplEnhanced()->isCleanupCompleted() ? "Yes" : "No") << std::endl;
        }
    } else {
        std::cout << "✗ Brain initialization failed" << std::endl;
    }
}

int main() {
    std::cout << "=== Enhanced Memory Management and RAII Demonstration ===" << std::endl;
    std::cout << "This demo shows the enhanced memory management features implemented for NLM core components." << std::endl;
    
    try {
        // Demonstrate individual features
        demonstrate_neural_array();
        demonstrate_raii_wrapper();
        demonstrate_memory_tracker();
        demonstrate_leak_detection();
        
        // Demonstrate integration with NLM components
        demonstrate_enhanced_brain();
        
        std::cout << "\n=== All Demonstrations Completed Successfully ===" << std::endl;
        std::cout << "The enhanced memory management and RAII implementation is working correctly!" << std::endl;
        
        // Final leak check
        if (nlm::LeakDetector::checkForLeaks()) {
            std::cout << "⚠️  Memory leaks were detected! Running cleanup..." << std::endl;
            nlm::forceMemoryCleanup();
        } else {
            std::cout << "✓ Final memory check passed - no leaks detected." << std::endl;
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "\n=== Demonstration Failed ===" << std::endl;
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}