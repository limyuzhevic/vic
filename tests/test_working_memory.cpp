// Working Memory Tests
// Phase 6: Working Memory System Integration Tests

#include "memory/WorkingMemoryTest.hpp"
#include <iostream>

namespace test_working_memory {
    void runAll() {
        std::cout << "  Running WorkingMemory tests..." << std::endl;
        nlm::WorkingMemoryTest::runAllTests();
        std::cout << "  PASSED" << std::endl;
    }
}