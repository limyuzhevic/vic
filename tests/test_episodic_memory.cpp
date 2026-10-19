// Episodic Memory Tests
// Phase 6: Episodic Memory System Integration Tests

#include "memory/EpisodicMemoryTest.hpp"
#include <iostream>

namespace test_episodic_memory {
    void runAll() {
        std::cout << "  Running EpisodicMemory tests..." << std::endl;
        nlm::EpisodicMemoryTest::runAllTests();
        std::cout << "  PASSED" << std::endl;
    }
}
