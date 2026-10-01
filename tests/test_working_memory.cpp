// Memory Tests
// Tests for the improved memory systems with capacity management and error handling

#include "memory/Memory.hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <limits>

namespace test_working_memory {
    
    void testWorkingMemoryStoreRetrieve() {
        nlm::WorkingMemory memory;
        
        // Test basic store/retrieve
        memory.store(nlm::NeuronId(1), 0.5f);
        assert(memory.retrieve(nlm::NeuronId(1)) == 0.5f);
        
        // Test overwrite
        memory.store(nlm::NeuronId(1), 0.8f);
        assert(memory.retrieve(nlm::NeuronId(1)) == 0.8f);
        
        // Test multiple items
        memory.store(nlm::NeuronId(2), 0.3f);
        memory.store(nlm::NeuronId(3), 0.7f);
        assert(memory.retrieve(nlm::NeuronId(2)) == 0.3f);
        assert(memory.retrieve(nlm::NeuronId(3)) == 0.7f);
        
        // Test non-existent item returns 0.0f
        assert(memory.retrieve(nlm::NeuronId(999)) == 0.0f);
        
        std::cout << "    testWorkingMemoryStoreRetrieve passed" << std::endl;
    }
    
    void testWorkingMemoryCapacity() {
        nlm::WorkingMemory memory;
        
        // Set very small capacity by creating new memory with smaller capacity
        struct SmallWorkingMemory : public nlm::WorkingMemory {
            SmallWorkingMemory() : nlm::WorkingMemory() {
                // Access internal capacity through a friend or public method
                // For now, we'll use the public interface
            }
        };
        
        // Test LRU eviction - store more items than capacity
        // This is harder to test without exposing internal structure
        // We'll test that basic capacity limits work
        
        for (int i = 0; i < 50; ++i) {
            memory.store(nlm::NeuronId(i), 0.1f);
        }
        
        assert(memory.getCurrentSize() <= memory.getCapacity());
        std::cout << "    testWorkingMemoryCapacity passed" << std::endl;
    }
    
    void testWorkingMemoryDecay() {
        nlm::WorkingMemory memory;
        
        memory.store(nlm::NeuronId(1), 0.8f);
        memory.store(nlm::NeuronId(2), 0.6f);
        
        float before = memory.retrieve(nlm::NeuronId(1));
        memory.decay(0.1f);  // 10% decay
        float after = memory.retrieve(nlm::NeuronId(1));
        
        assert(before > after);
        assert(after == before * 0.9f);
        
        std::cout << "    testWorkingMemoryDecay passed" << std::endl;
    }
    
    void testWorkingMemoryClear() {
        nlm::WorkingMemory memory;
        
        memory.store(nlm::NeuronId(1), 0.5f);
        memory.store(nlm::NeuronId(2), 0.7f);
        assert(memory.getCurrentSize() > 0);
        
        memory.clear();
        assert(memory.getCurrentSize() == 0);
        assert(memory.retrieve(nlm::NeuronId(1)) == 0.0f);
        assert(memory.retrieve(nlm::NeuronId(2)) == 0.0f);
        
        std::cout << "    testWorkingMemoryClear passed" << std::endl;
    }
    
    void testWorkingMemoryInputValidation() {
        nlm::WorkingMemory memory;
        
        // Test valid input
        memory.store(nlm::NeuronId(1), 0.5f);
        assert(memory.retrieve(nlm::NeuronId(1)) == 0.5f);
        
        // Test invalid input should throw
        bool exceptionThrown = false;
        try {
            memory.store(nlm::NeuronId(2), std::numeric_limits<float>::infinity());
        } catch (const std::invalid_argument& e) {
            exceptionThrown = true;
            assert(std::string(e.what()).find("invalid value") != std::string::npos);
        }
        assert(exceptionThrown);
        
        std::cout << "    testWorkingMemoryInputValidation passed" << std::endl;
    }
    
    void runAll() {
        std::cout << "Running Working Memory tests..." << std::endl;
        testWorkingMemoryStoreRetrieve();
        testWorkingMemoryCapacity();
        testWorkingMemoryDecay();
        testWorkingMemoryClear();
        testWorkingMemoryInputValidation();
    }

} // namespace test_working_memory
