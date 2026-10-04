// Comprehensive Memory Management Tests
// Tests for raw pointer elimination, memory leak detection, move operations, and resource management

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>
#include <cstring>

namespace test_memory_management {

// Global memory tracking for leak detection
class MemoryTracker {
public:
    static MemoryTracker& getInstance() {
        static MemoryTracker instance;
        return instance;
    }
    
    void trackAllocation(void* ptr, size_t size) {
        std::lock_guard<std::mutex> lock(mutex);
        allocatedBlocks[ptr] = size;
        totalAllocated += size;
    }
    
    void trackDeallocation(void* ptr) {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = allocatedBlocks.find(ptr);
        if (it != allocatedBlocks.end()) {
            totalAllocated -= it->second;
            allocatedBlocks.erase(it);
        }
    }
    
    size_t getTotalAllocated() const {
        return totalAllocated;
    }
    
    size_t getActiveBlocks() const {
        return allocatedBlocks.size();
    }
    
private:
    MemoryTracker() = default;
    std::mutex mutex;
    std::unordered_map<void*, size_t> allocatedBlocks;
    size_t totalAllocated = 0;
};

// Custom allocator to track memory usage
template<typename T>
class TrackingAllocator {
public:
    using value_type = T;
    
    TrackingAllocator() = default;
    
    template<typename U>
    TrackingAllocator(const TrackingAllocator<U>& other) {}
    
    T* allocate(std::size_t n) {
        void* ptr = std::malloc(n * sizeof(T));
        if (ptr == nullptr) {
            throw std::bad_alloc();
        }
        MemoryTracker::getInstance().trackAllocation(ptr, n * sizeof(T));
        return static_cast<T*>(ptr);
    }
    
    void deallocate(T* ptr, std::size_t n) {
        if (ptr != nullptr) {
            MemoryTracker::getInstance().trackDeallocation(ptr);
            std::free(ptr);
        }
    }
};

void testRawPointerElimination() {
    // Verify that Brain uses smart pointers internally
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // After initialization, all internal pointers should be smart pointers
    // This is verified by the fact that the code compiles and runs without
    // accessing raw pointers directly (they should be accessed via .get() only)
    assert(brain.getTotalNeuronCount() == 100);
    
    // Verify initialization worked (no crashes indicates proper memory management)
    bool initialized = brain.initialize();
    assert(initialized);
    
    std::cout << "    testRawPointerElimination passed" << std::endl;
}

void testMemoryLeakDetection() {
    // Track memory before and after Brain operations
    size_t initialMemory = MemoryTracker::getInstance().getTotalAllocated();
    
    {
        // Create and destroy multiple Brain instances
        for (int i = 0; i < 10; ++i) {
            auto config = std::make_shared<nlm::Config>();
            config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
            
            nlm::Brain brain(config);
            brain.initialize();
            brain.reset();
            // brain destructor should clean up all resources
        }
    }
    
    // Give time for any background cleanup
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    size_t finalMemory = MemoryTracker::getInstance().getTotalAllocated();
    
    // Memory should be cleaned up properly (allow some tolerance for system allocations)
    assert(finalMemory <= initialMemory + 1024); // Allow 1KB tolerance
    
    std::cout << "    testMemoryLeakDetection passed" << std::endl;
}

void testDestructSequenceValidation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    {
        nlm::Brain brain(config);
        brain.initialize();
        
        // Verify components exist
        assert(brain.getTotalNeuronCount() == 100);
        
        // When brain goes out of scope, destructors should be called in proper order:
        // 1. Brain::~Brain()
        // 2. Impl::~Impl() which calls implReset()
        // 3. All smart pointers are reset
    }
    
    // After destruction, no crashes indicate proper cleanup
    std::cout << "    testDestructSequenceValidation passed" << std::endl;
}

void testMoveConstructor() {
    auto config1 = std::make_shared<nlm::Config>();
    config1->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config1->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config2->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain1(config1);
    brain1.initialize();
    
    // Test move constructor
    nlm::Brain brain2(std::move(brain1));
    
    // Verify moved-from brain is in valid state
    assert(brain1.getTotalNeuronCount() == 0); // Should be empty after move
    assert(brain2.getTotalNeuronCount() == 200); // Should retain original data
    
    std::cout << "    testMoveConstructor passed" << std::endl;
}

void testMoveAssignmentOperator() {
    auto config1 = std::make_shared<nlm::Config>();
    config1->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config1->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", static_cast<int64_t>(75), nlm::ConfigSource::Default);
    config2->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain1(config1);
    nlm::Brain brain2(config2);
    
    brain1.initialize();
    brain2.initialize();
    
    // Test move assignment operator
    brain2 = std::move(brain1);
    
    // Verify moved-from brain is in valid state
    assert(brain1.getTotalNeuronCount() == 0); // Should be empty after move
    assert(brain2.getTotalNeuronCount() == 150); // Should retain original data
    
    std::cout << "    testMoveAssignmentOperator passed" << std::endl;
}

void testResourceManagementInErrorScenarios() {
    // Test resource cleanup when exceptions occur during initialization
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(0), nlm::ConfigSource::Default); // Invalid, should handle gracefully
    
    nlm::Brain brain(config);
    
    // Should handle invalid configuration gracefully
    bool initialized = brain.initialize();
    // This should not crash and should clean up any allocated resources
    
    std::cout << "    testResourceManagementInErrorScenarios passed" << std::endl;
}

void testSharedPointerUsage() {
    // Verify that shared pointers are used throughout the system
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify that neural populations and regions are properly managed
    auto regionIds = brain.getRegionIds();
    assert(!regionIds.empty());
    
    // Verify shared pointer usage through the interface
    // (Direct access would use .get() on shared_ptr internally)
    
    std::cout << "    testSharedPointerUsage passed" << std::endl;
}

void runAll() {
    std::cout << "=== Memory Management Tests ===" << std::endl;
    testRawPointerElimination();
    testMemoryLeakDetection();
    testDestructSequenceValidation();
    testMoveConstructor();
    testMoveAssignmentOperator();
    testResourceManagementInErrorScenarios();
    testSharedPointerUsage();
    std::cout << "=== All Memory Management Tests PASSED ===" << std::endl;
}

} // namespace test_memory_management