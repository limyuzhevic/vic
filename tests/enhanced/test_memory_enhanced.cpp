// Memory Management and Leak Detection Tests Implementation
// Comprehensive testing for memory safety and resource cleanup

#include "enhanced/memory_testing.h"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "plasticity/STDP.hpp"
#include <memory>
#include <vector>
#include <iostream>

// Test for memory allocation tracking
TEST_F(MemoryAllocationTest, BasicAllocationTracking) {
    void* ptr = malloc(100);
    ASSERT_NE(ptr, nullptr) << "Memory allocation failed";
    
    // Pointer should be tracked
    EXPECT_TRUE(nlm_test::MemoryTracker::hasAllocations()) 
        << "Allocated pointer should be tracked";
    
    free(ptr);
    // Memory tracker should be cleared
    EXPECT_FALSE(nlm_test::MemoryTracker::hasLeaks())
        << "Freed pointer should be untracked";
}

// Test for smart pointer lifetime management
TEST_F(SmartPointerLifetimeTest, UniquePtrLifetime) {
    testUniquePtrLifetime();
    EXPECT_FALSE(nlm_test::MemoryTracker::hasLeaks())
        << "Unique pointer should free memory on destruction";
}

TEST_F(SmartPointerLifetimeTest, SharedPtrLifetime) {
    testSharedPtrLifetime();
    EXPECT_FALSE(nlm_test::MemoryTracker::hasLeaks())
        << "Shared pointer should free memory on destruction";
}

TEST_F(SmartPointerLifetimeTest, WeakPtrLifetime) {
    testWeakPtrLifetime();
    EXPECT_TRUE(nlm_test::MemoryTracker::hasLeaks())
        << "Weak pointer should allow original object to be freed";
    nlm_test::MemoryTracker::clear(); // Clean up
}

// Test for resource cleanup with exceptions
TEST_F(ResourceCleanupTest, ExplicitCleanup) {
    testExplicitCleanup();
}

TEST_F(ResourceCleanupTest, ExceptionSafety) {
    EXPECT_NO_THROW(testExceptionSafety())
        << "Exception should not prevent resource cleanup";
    EXPECT_FALSE(nlm_test::MemoryTracker::hasLeaks())
        << "Exception safety check failed - memory leak detected";
}

// Test for memory alignment and packing
TEST_F(MemoryAlignmentTest, MemoryAlignment) {
    testMemoryAlignment();
}

TEST_F(MemoryAlignmentTest, StructPacking) {
    testStructPacking();
}

// Test for container memory management
TEST_F(ContainerMemoryTest, VectorMemoryManagement) {
    testVectorMemoryManagement();
}

TEST_F(ContainerMemoryTest, MapMemoryManagement) {
    testMapMemoryManagement();
}

// Test for double-free detection
TEST_F(DoubleFreeTest, DoubleFreeDetection) {
    testDoubleFreeDetection();
}

TEST_F(DoubleFreeTest, NullPointerFree) {
    testNullPointerFree();
}

// Test for large object allocation
TEST_F(LargeObjectAllocationTest, LargeNeuronArray) {
    testLargeNeuronArray();
}

TEST_F(LargeObjectAllocationTest, MemoryFragmentation) {
    testMemoryFragmentation();
}

// Test for actual NLM object memory management
TEST_F(MemoryAllocationTest, NeuronMemoryManagement) {
    {
        std::unique_ptr<nlm::Neuron> neuron = std::make_unique<nlm::Neuron>(nlm::NeuronId(1));
        neuron->initializeRandom(nlm_test::TestSeedManager::getRNG());
        EXPECT_NE(neuron.get(), nullptr) << "Neuron creation failed";
        EXPECT_TRUE(neuron->isInitialized()) << "Neuron initialization failed";
        
        // Verify neuron properties
        EXPECT_GE(neuron->getMembranePotential(), -100.0f) << "Invalid membrane potential";
        EXPECT_LE(neuron->getMembranePotential(), 50.0f) << "Invalid membrane potential";
    }
    
    // After scope exit, neuron should be destroyed
    EXPECT_FALSE(nlm_test::MemoryTracker::hasLeaks()) << "Neuron memory leak detected";
}

TEST_F(MemoryAllocationTest, SynapseMemoryManagement) {
    {
        std::unique_ptr<nlm::Synapse> synapse = std::make_unique<nlm::Synapse>(
            nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        
        synapse->setType(nlm::SynapseType::Excitatory);
        synapse->setWeight(0.75f);
        
        EXPECT_NE(synapse.get(), nullptr) << "Synapse creation failed";
        EXPECT_EQ(synapse->getType(), nlm::SynapseType::Excitatory) << "Synapse type incorrect";
        EXPECT_FLOAT_EQ(synapse->getWeight(), 0.75f) << "Synapse weight incorrect";
    }
    
    // After scope exit, synapse should be destroyed
    EXPECT_FALSE(nlm_test::MemoryTracker::hasLeaks()) << "Synapse memory leak detected";
}

TEST_F(MemoryAllocationTest, PlasticityRuleMemoryManagement) {
    {
        std::unique_ptr<nlm::STDP> stdp = std::make_unique<nlm::STDP>();
        std::unique_ptr<nlm::Hebbian> hebbian = std::make_unique<nlm::Hebbian>();
        
        EXPECT_NE(stdp.get(), nullptr) << "STDP creation failed";
        EXPECT_NE(hebbian.get(), nullptr) << "Hebbian creation failed";
        
        EXPECT_TRUE(stdp->getName() != nullptr) << "STDP name should not be null";
        EXPECT_TRUE(hebbian->getName() != nullptr) << "Hebbian name should not be null";
    }
    
    // After scope exit, plasticity rules should be destroyed
    EXPECT_FALSE(nlm_test::MemoryTracker::hasLeaks()) << "Plasticity rule memory leak detected";
}

// Test for memory stress scenarios
TEST_F(MemoryAllocationTest, MemoryStressTest) {
    std::vector<std::unique_ptr<nlm::Neuron>> neurons;
    neurons.reserve(10000);
    
    // Create many neurons
    for (int i = 0; i < 10000; ++i) {
        neurons.emplace_back(std::make_unique<nlm::Neuron>(nlm::NeuronId(i % 1000)));
        neurons.back()->initializeRandom(nlm_test::TestSeedManager::getRNG());
    }
    
    EXPECT_EQ(neurons.size(), 10000) << "Neuron stress test failed - wrong count";
    
    // Verify some neurons
    for (int i = 0; i < 100 && i < neurons.size(); ++i) {
        EXPECT_NE(neurons[i].get(), nullptr) << "Neuron access failed in stress test";
        EXPECT_TRUE(neurons[i]->isInitialized()) << "Neuron not properly initialized";
    }
    
    // Cleanup
    neurons.clear();
    EXPECT_FALSE(nlm_test::MemoryTracker::hasLeaks()) << "Memory stress test failed - leaks detected";
}

// Test for circular reference detection
TEST_F(MemoryAllocationTest, CircularReferenceTest) {
    // This test ensures we can detect and handle circular references
    std::vector<std::unique_ptr<nlm::Neuron>> neurons;
    
    // Create neurons without circular references
    for (int i = 0; i < 10; ++i) {
        neurons.emplace_back(std::make_unique<nlm::Neuron>(nlm::NeuronId(i)));
    }
    
    // No circular references should be created
    EXPECT_FALSE(nlm_test::MemoryTracker::hasLeaks()) 
        << "Circular reference detection failed - unexpected leak";
    
    // Cleanup
    neurons.clear();
    nlm_test::MemoryTracker::clear();
}

// Test for memory allocation patterns
TEST_F(MemoryAllocationTest, AllocationPatternTest) {
    // Test various allocation patterns
    std::vector<void*> allocations;
    
    // Small allocations
    for (int i = 0; i < 100; ++i) {
        allocations.push_back(malloc(16));
        ASSERT_NE(allocations.back(), nullptr);
    }
    
    // Medium allocations
    for (int i = 0; i < 50; ++i) {
        allocations.push_back(malloc(256));
        ASSERT_NE(allocations.back(), nullptr);
    }
    
    // Large allocations
    for (int i = 0; i < 10; ++i) {
        allocations.push_back(malloc(4096));
        ASSERT_NE(allocations.back(), nullptr);
    }
    
    EXPECT_EQ(allocations.size(), 160) << "Allocation pattern test failed - wrong count";
    
    // Free all allocations
    for (void* ptr : allocations) {
        free(ptr);
    }
    
    EXPECT_FALSE(nlm_test::MemoryTracker::hasLeaks())
        << "Allocation pattern test failed - leaks detected";
}