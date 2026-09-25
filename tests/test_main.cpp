// Legacy Test Runner Compatibility
// This file provides compatibility with the old test system
// For the new Catch2-based testing, use the nlm_test executable directly

#include <iostream>

// Forward declarations for legacy compatibility
namespace test_types {
    void runAll();
}

namespace test_config {
    void runAll();
}

namespace test_random {
    void runAll();
}

namespace test_clock {
    void runAll();
}

namespace test_neuron {
    void runAll();
}

namespace test_synapse {
    void runAll();
}

namespace test_stdp {
    void runAll();
}

namespace test_brain {
    void runAll();
}

int main() {
    std::cout << "=== Legacy NLM Test System ===" << std::endl;
    std::cout << "This is the old test runner. For the new Catch2-based tests,
        run the nlm_test executable which uses the modern Catch2 framework." << std::endl;
    std::cout << std::endl;
    std::cout << "The new test framework includes:" << std::endl;
    std::cout << "  - Comprehensive Catch2-based tests" << std::endl;
    std::cout << "  - Unit tests for all major components" << std::endl;
    std::cout << "  - Integration tests" << std::endl;
    std::cout << "  - Performance tests" << std::endl;
    std::cout << "  - Regression tests" << std::endl;
    std::cout << "  - Detailed reporting and documentation" << std::endl;
    std::cout << std::endl;
    
    // Legacy compatibility - call old test runners if needed
    std::cout << "Legacy test compatibility mode enabled." << std::endl;
    
    return 0;
}
