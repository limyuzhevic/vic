// NLM GoogleTest Test Runner
// This file serves as the main test entry point for GoogleTest
// All actual test cases are defined in the individual test files (test_types.cpp, test_config.cpp, etc.)

#include <gtest/gtest.h>

// Test framework initialization
GTEST_API_ int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

// Backward compatibility - if any code expects the old test_main.cpp functionality,
// these namespace declarations will prevent compilation errors
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