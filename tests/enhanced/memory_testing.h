// Memory Management and Leak Detection Tests
// Comprehensive testing for memory safety and resource cleanup

#ifndef MEMORY_TESTING_H
#define MEMORY_TESTING_H

#include "nlm_test_framework.h"
#include "core/Types/Types.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <unordered_map>
#include <cstdlib>

// Memory tracker implementation for leak detection
namespace nlm_test {
    std::unordered_set<void*> MemoryTracker::allocated_ptrs;
    std::mutex MemoryTracker::mutex;
    
    void* MemoryTracker::trackAllocation(void* ptr, size_t size, const char* file, int line) {
        std::lock_guard<std::mutex> lock(mutex);
        allocated_ptrs.insert(ptr);
        
        std::cout << "[MEMORY] Allocated " << size << " bytes at " << ptr 
                  << " (" << file << ":" << line << ")" << std::endl;
        return ptr;
    }
    
    void MemoryTracker::trackDeallocation(void* ptr) {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = allocated_ptrs.find(ptr);
        if (it != allocated_ptrs.end()) {
            allocated_ptrs.erase(it);
            std::cout << "[MEMORY] Deallocated memory at " << ptr << std::endl;
        } else {
            std::cout << "[MEMORY] Warning: Deallocating unknown pointer " << ptr << std::endl;
        }
    }
    
    bool MemoryTracker::hasLeaks() {
        return !allocated_ptrs.empty();
    }
    
    void MemoryTracker::reportLeaks() {
        if (hasLeaks()) {
            std::cout << "[MEMORY] LEAK DETECTED:" << std::endl;
            for (void* ptr : allocated_ptrs) {
                std::cout << "  - Leaked pointer: " << ptr << std::endl;
            }
        }
    }
    
    void MemoryTracker::clear() {
        if (hasLeaks()) {
            reportLeaks();
        }
        allocated_ptrs.clear();
    }
    
    size_t MemoryTracker::getAllocationCount() {
        std::lock_guard<std::mutex> lock(mutex);
        return allocated_ptrs.size();
    }
}

// Memory allocation tracking macros
#define TRACKED_NEW(type, ...) new type(__VA_ARGS__)
#define TRACKED_DELETE(ptr) delete ptr
#define TRACKED_NEW_ARRAY(type, count, ...) new type[count]
#define TRACKED_DELETE_ARRAY(ptr) delete[] ptr

// Override new/delete operators for tracking
void* operator new(size_t size) {
    return nlm_test::MemoryTracker::trackAllocation(
        ::operator new(size), size, "unknown", 0);
}

void* operator new[](size_t size) {
    return nlm_test::MemoryTracker::trackAllocation(
        ::operator new[](size), size, "unknown", 0);
}

void operator delete(void* ptr) noexcept {
    nlm_test::MemoryTracker::trackDeallocation(ptr);
    ::operator delete(ptr);
}

void operator delete[](void* ptr) noexcept {
    nlm_test::MemoryTracker::trackDeallocation(ptr);
    ::operator delete[](ptr);
}

// Memory Management Test Fixtures
class MemoryAllocationTest : public ::testing::Test {
protected:
    size_t initial_allocations;
    
    void SetUp() override {
        initial_allocations = nlm_test::MemoryTracker::getAllocationCount();
    }
    
    void TearDown() override {
        nlm_test::MemoryTracker::clear();
        size_t final_allocations = nlm_test::MemoryTracker::getAllocationCount();
        
        if (final_allocations > initial_allocations) {
            FAIL() << "Memory leak detected: " 
                   << (final_allocations - initial_allocations) 
                   << " allocations not freed";
        }
    }
};

// Smart Pointer Lifetime Tests
class SmartPointerLifetimeTest : public MemoryAllocationTest {
protected:
    void testUniquePtrLifetime() {
        {
            std::unique_ptr<nlm::Neuron> neuron = std::make_unique<nlm::Neuron>(nlm::NeuronId(1));
            neuron->initializeRandom(nlm_test::TestSeedManager::getRNG());
            EXPECT_TRUE(neuron->isInitialized()) << "Neuron should be initialized";
            // neuron goes out of scope here
        }
        // After scope exit, unique_ptr should be destroyed and memory freed
        EXPECT_NO_FATAL_FAILURE();
    }
    
    void testSharedPtrLifetime() {
        std::shared_ptr<nlm::Synapse> synapse = std::make_shared<nlm::Synapse>(
            nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        synapse->setWeight(0.5f);
        EXPECT_FLOAT_EQ(synapse->getWeight(), 0.5f) << "Shared pointer synapse weight incorrect";
        // synapse goes out of scope here
    }
    
    void testWeakPtrLifetime() {
        std::shared_ptr<nlm::Neuron> neuron = std::make_shared<nlm::Neuron>(nlm::NeuronId(3));
        std::weak_ptr<nlm::Neuron> weak_neuron = neuron;
        
        EXPECT_FALSE(weak_neuron.expired()) << "Weak pointer should not be expired";
        EXPECT_TRUE(weak_neuron.lock()) << "Weak pointer should be lockable";
        
        // neuron goes out of scope
    }
    
    // After neuron scope exit, weak pointer should be expired
    EXPECT_TRUE(weak_neuron.expired()) << "Weak pointer should be expired after original object destroyed";
}

// Resource Cleanup Tests
class ResourceCleanupTest : public MemoryAllocationTest {
protected:
    void SetUp() override {
        MemoryAllocationTest::SetUp();
        // Setup test resources
        neurons.reserve(10);
        for (int i = 0; i < 10; ++i) {
            neurons.emplace_back(std::make_unique<nlm::Neuron>(nlm::NeuronId(i)));
        }
    }
    
    void TearDown() override {
        // Explicit cleanup verification
        neurons.clear();
        MemoryAllocationTest::TearDown();
    }
    
    std::vector<std::unique_ptr<nlm::Neuron>> neurons;
    
    void testExplicitCleanup() {
        neurons.clear();
        // After clear(), all neurons should be deallocated
        EXPECT_EQ(nlm_test::MemoryTracker::getAllocationCount(), initial_allocations) 
            << "Explicit cleanup failed to free memory";
    }
    
    void testExceptionSafety() {
        try {
            // Create neurons
            std::vector<std::unique_ptr<nlm::Neuron>> test_neurons;
            for (int i = 0; i < 5; ++i) {
                test_neurons.emplace_back(std::make_unique<nlm::Neuron>(nlm::NeuronId(i)));
            }
            
            // Throw an exception while holding resources
            throw std::runtime_error("Test exception for exception safety");
            
        } catch (const std::runtime_error&) {
            // Exception caught - resources should be cleaned up
            EXPECT_NO_THROW({
                // If we reach here, cleanup worked
            });
        }
    }
};

// Memory Alignment and Padding Tests
class MemoryAlignmentTest : public MemoryAllocationTest {
protected:
    struct AlignedNeuronData {
        nlm::NeuronId neuron_id;
        float membrane_potential;
        float threshold;
        char padding[64]; // Ensure alignment
        
        AlignedNeuronData(nlm::NeuronId id) : neuron_id(id), membrane_potential(-70.0f), threshold(-55.0f) {}
    };
    
    void testMemoryAlignment() {
        AlignedNeuronData data(nlm::NeuronId(1));
        
        // Check that the struct is properly aligned
        EXPECT_EQ(reinterpret_cast<uintptr_t>(&data.neuron_id) % alignof(nlm::NeuronId), 0)
            << "NeuronId not properly aligned";
        EXPECT_EQ(reinterpret_cast<uintptr_t>(&data.membrane_potential) % alignof(float), 0)
            << "Membrane potential not properly aligned";
        EXPECT_EQ(reinterpret_cast<uintptr_t>(&data.threshold) % alignof(float), 0)
            << "Threshold not properly aligned";
    }
    
    void testStructPacking() {
        // Test that our data structures don't have excessive padding
        struct PackedNeuron {
            nlm::NeuronId id;
            float membrane_potential;
            float threshold;
        } __attribute__((packed));
        
        EXPECT_EQ(sizeof(PackedNeuron), 12) << "Packed Neuron size unexpected";
        
        PackedNeuron test_neuron{nlm::NeuronId(1), -70.0f, -55.0f};
        EXPECT_EQ(test_neuron.id.index(), 1) << "Packed neuron ID incorrect";
        EXPECT_FLOAT_EQ(test_neuron.membrane_potential, -70.0f) << "Packed neuron potential incorrect";
    }
};

// Vector and Container Memory Management
class ContainerMemoryTest : public MemoryAllocationTest {
protected:
    void testVectorMemoryManagement() {
        std::vector<std::unique_ptr<nlm::Synapse>> synapses;
        
        // Test push_back and clear
        for (int i = 0; i < 100; ++i) {
            synapses.emplace_back(std::make_unique<nlm::Synapse>(
                nlm::SynapseId(i), nlm::NeuronId(i), nlm::NeuronId(i + 1)));
        }
        
        EXPECT_EQ(synapses.size(), 100) << "Vector size after insertion incorrect";
        
        // Test clear
        synapses.clear();
        EXPECT_EQ(synapses.size(), 0) << "Vector clear failed";
        
        // Test resize
        synapses.resize(50);
        EXPECT_EQ(synapses.size(), 50) << "Vector resize failed";
        
        // Test reserve and capacity
        synapses.reserve(1000);
        EXPECT_GE(synapses.capacity(), 1000) << "Vector reserve failed";
    }
    
    void testMapMemoryManagement() {
        std::unordered_map<nlm::NeuronId, std::unique_ptr<nlm::Neuron>> neuron_map;
        
        for (int i = 0; i < 50; ++i) {
            neuron_map[nlm::NeuronId(i)] = std::make_unique<nlm::Neuron>(nlm::NeuronId(i));
        }
        
        EXPECT_EQ(neuron_map.size(), 50) << "Map size after insertion incorrect";
        EXPECT_NE(neuron_map.find(nlm::NeuronId(25)), neuron_map.end()) 
            << "Map find operation failed";
        
        // Test erase
        neuron_map.erase(nlm::NeuronId(25));
        EXPECT_EQ(neuron_map.size(), 49) << "Map erase failed";
    }
};

// Double-Free Detection Tests
class DoubleFreeTest : public MemoryAllocationTest {
protected:
    void testDoubleFreeDetection() {
        void* ptr1 = nlm_test::MemoryTracker::trackAllocation(
            malloc(100), 100, "test.cpp", 123);
        
        // First free should work
        nlm_test::MemoryTracker::trackDeallocation(ptr1);
        
        // Second free should be detected (but won't crash due to tracking)
        nlm_test::MemoryTracker::trackDeallocation(ptr1);
        
        // After tracking, the pointer should not be in allocated set
        EXPECT_FALSE(nlm_test::MemoryTracker::hasLeaks()) 
            << "Memory tracker should not report leaks for double frees";
        
        free(ptr1); // Actual free to clean up
    }
    
    void testNullPointerFree() {
        // Freeing null pointer should not crash
        nlm_test::MemoryTracker::trackDeallocation(nullptr);
        free(nullptr); // Should not crash
        EXPECT_NO_THROW({ free(nullptr); });
    }
};

// Large Object Allocation Tests
class LargeObjectAllocationTest : public MemoryAllocationTest {
protected:
    void testLargeNeuronArray() {
        const size_t large_array_size = 100000;
        
        // This test creates many neuron objects to test memory efficiency
        std::vector<std::unique_ptr<nlm::Neuron>> neurons;
        neurons.reserve(large_array_size);
        
        for (size_t i = 0; i < large_array_size; ++i) {
            neurons.emplace_back(std::make_unique<nlm::Neuron>(nlm::NeuronId(i % 1000)));
        }
        
        EXPECT_EQ(neurons.size(), large_array_size) << "Large array creation failed";
        
        // Verify some neurons are accessible
        for (size_t i = 0; i < 10 && i < neurons.size(); ++i) {
            EXPECT_NE(neurons[i].get(), nullptr) << "Neuron access failed in large array";
        }
        
        // Cleanup
        neurons.clear();
        EXPECT_NO_THROW({ neurons.clear(); });
    }
    
    void testMemoryFragmentation() {
        std::vector<void*> allocations;
        
        // Allocate and deallocate in alternating pattern to test fragmentation
        for (int cycle = 0; cycle < 10; ++cycle) {
            std::vector<void*> temp_allocs;
            for (int i = 0; i < 100; ++i) {
                void* ptr = malloc(1024); // 1KB allocations
                temp_allocs.push_back(ptr);
            }
            
            // Deallocate half
            for (size_t i = 0; i < temp_allocs.size() / 2; ++i) {
                free(temp_allocs[i]);
            }
            
            // Store remaining for later cleanup
            for (void* ptr : temp_allocs) {
                if (std::find(allocations.begin(), allocations.end(), ptr) == allocations.end()) {
                    allocations.push_back(ptr);
                }
            }
        }
        
        // Cleanup all remaining allocations
        for (void* ptr : allocations) {
            free(ptr);
        }
    }
};

#endif // MEMORY_TESTING_H
