// NLM Memory Test Suite
// Phase 4: Memory Integration Tests

#include <iostream>
#include <cassert>
#include <vector>
#include <memory>

#include "../src/memory/Memory.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/core/Config/Config.hpp"

namespace test_memory {
    
    void test_memory_basics() {
        std::cout << "Testing Memory basics..." << std::endl;
        
        auto config = std::make_shared<Config>();
        Memory memory(config);
        
        assert(memory.initialize() == true);
        assert(memory.getMemoryCapacity() >= 0);
        
        std::cout << "  PASSED" << std::endl;
    }
    
    void test_working_memory() {
        std::cout << "Testing Working Memory system..." << std::endl;
        
        auto config = std::make_shared<Config>();
        NeuralWorkingMemory workingMemory(config);
        
        assert(workingMemory.initialize() == true);
        assert(workingMemory.getWorkingMemoryCapacity() >= 0);
        
        // Test working memory operations
        workingMemory.clearWorkingMemory();
        
        std::cout << "  PASSED" << std::endl;
    }
    
    void test_episodic_memory() {
        std::cout << "Testing Episodic Memory system..." << std::endl;
        
        auto config = std::make_shared<Config>();
        NeuralEpisodicMemory episodicMemory(config);
        
        assert(episodicMemory.initialize() == true);
        assert(episodicMemory.getEpisodicMemoryCapacity() >= 0);
        
        // Test episodic memory operations
        episodicMemory.clearEpisodicMemory();
        
        std::cout << "  PASSED" << std::endl;
    }
    
    void test_memory_integration() {
        std::cout << "Testing Memory integration with neural processing..." << std::endl;
        
        auto config = std::make_shared<Config>();
        Memory memory(config);
        
        assert(memory.initialize() == true);
        
        // Test memory storage and retrieval
        memory.storeExperience("test_experience", 1.0f);
        std::optional<float> retrieved = memory.getExperience("test_experience");
        assert(retrieved.has_value());
        assert(retrieved.value() == 1.0f);
        
        memory.clearMemory();
        
        std::cout << "  PASSED" << std::endl;
    }
    
    void test_working_memory_integration() {
        std::cout << "Testing Working Memory integration with attention..." << std::endl;
        
        auto config = std::make_shared<Config>();
        NeuralWorkingMemory workingMemory(config);
        
        assert(workingMemory.initialize() == true);
        
        // Test working memory update
        workingMemory.updateWorkingMemory(1.0f, 0.5f); // value, importance
        assert(workingMemory.getWorkingMemorySize() > 0);
        
        workingMemory.clearWorkingMemory();
        
        std::cout << "  PASSED" << std::endl;
    }
    
    void test_episodic_memory_integration() {
        std::cout << "Testing Episodic Memory integration with development..." << std::endl;
        
        auto config = std::make_shared<Config>();
        NeuralEpisodicMemory episodicMemory(config);
        
        assert(episodicMemory.initialize() == true);
        
        // Test episodic memory encoding
        episodicMemory.encodeExperience("test_episode", 1.0f, std::vector<float>{0.1f, 0.2f, 0.3f});
        assert(episodicMemory.getEpisodicMemorySize() > 0);
        
        episodicMemory.clearEpisodicMemory();
        
        std::cout << "  PASSED" << std::endl;
    }
    
    void runAll() {
        std::cout << "=== NLM Memory Tests ===" << std::endl;
        
        test_memory_basics();
        test_working_memory();
        test_episodic_memory();
        test_memory_integration();
        test_working_memory_integration();
        test_episodic_memory_integration();
        
        std::cout << std::endl;
        std::cout << "=== All Memory Tests PASSED ===" << std::endl;
    }
}