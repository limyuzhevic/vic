// Test Framework Interface
// Defines the interface for all test suites

#pragma once

#include "TestFramework.hpp"

// All test suites include this header
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

// Enhanced test suites
namespace test_enhanced {
    void runValidationTests();
    void runPerformanceTests();
    void runIntegrationTests();
    void runMemoryTests();
    void runStressTests();
}