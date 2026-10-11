// NLM Unit Testing Framework
// Memory System Tests

#include "framework/TestFramework.hpp"
#include "brain/MemorySystem.hpp"
#include "brain/MemoryModule.hpp"
#include <cassert>
#include <algorithm>

class TestMemorySystem : public TestCase {
public:
    TestMemorySystem() : TestCase("MemorySystem") {}
    
    void testMemoryModuleInitialization() {
        MemoryModule module;
        assert(module.getCapacity() > 0);
        assert(module.getAvailableMemory() <= module.getCapacity());
        assert(module.getUsagePercentage() >= 0.0 && module.getUsagePercentage() <= 100.0);
    }
    
    void testShortTermMemory() {
        MemorySystem memory;
        
        // Store and retrieve short-term data
        std::vector<double> data = {1.0, 2.0, 3.0, 4.0, 5.0};
        memory.storeShortTerm(data, MemoryType::SPATIAL);
        
        auto retrieved = memory.retrieveShortTerm(MemoryType::SPATIAL);
        assert(retrieved.size() == data.size());
        assert(retrieved == data);
    }
    
    void testLongTermMemory() {
        MemorySystem memory;
        
        // Store pattern in long-term memory
        Pattern pattern;
        pattern.weights = std::vector<double>(100, 0.5);
        pattern.learning_rate = 0.01;
        
        memory.storeLongTerm(pattern, MemoryType::TEMPORAL);
        
        auto retrieved_patterns = memory.getLongTermPatterns(MemoryType::TEMPORAL);
        assert(!retrieved_patterns.empty());
    }
    
    void testMemoryDecay() {
        MemorySystem memory;
        
        // Test that memory traces decay over time
        memory.storeShortTerm({1.0, 2.0}, MemoryType::SPATIAL);
        double strength_before = memory.getMemoryStrength(MemoryType::SPATIAL);
        
        // Advance time
        memory.advanceTime(10.0);
        
        double strength_after = memory.getMemoryStrength(MemoryType::SPATIAL);
        assert(strength_after < strength_before); // Should decay
    }
    
    void testMemoryCapacityLimits() {
        MemorySystem memory;
        
        // Fill memory to capacity
        std::vector<double> large_data(10000, 1.0);
        memory.storeShortTerm(large_data, MemoryType::PATTERN);
        
        // Should handle capacity limits gracefully
        assert(memory.getUsagePercentage() >= 0.0 && memory.getUsagePercentage() <= 100.0);
    }
};