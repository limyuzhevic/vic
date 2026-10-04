// NLM Test Main
// Phase 2: Real Neural Computation Tests
// Phase 6: Integrated Brain Systems Tests

#include <iostream>
#include <cassert>
#include <string>
#include <vector>

// Forward declarations of test functions
// Phase 2 tests
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

// Phase 6 integration tests
namespace test_phase6 {
    void runAll();
}

namespace test_connectivity {
    void runAll();
}

namespace test_memory_integration {
    void runAll();
}

namespace test_neuromodulation_integration {
    void runAll();
}

namespace test_checkpointing {
    void runAll();
}

namespace test_replay {
    void runAll();
}

// Command-line option parsing
struct TestOptions {
    bool runPhase2 = true;        // Default: run Phase 2 tests
    bool runPhase6 = true;        // Default: run Phase 6 tests
    bool runConnectivity = true;   // Default: run connectivity tests
    bool runMemoryIntegration = true; // Default: run memory integration tests
    bool runNeuromodulationIntegration = true; // Default: run neuromodulation integration tests
    bool runCheckpointing = true;   // Default: run checkpointing tests
    bool runReplay = true;         // Default: run replay tests
    bool help = false;
    
    static TestOptions parseArgs(int argc, char** argv);
};

int main(int argc, char** argv) {
    TestOptions options = TestOptions::parseArgs(argc, argv);
    
    if (options.help) {
        std::cout << "=== NLM Test Suite ===" << std::endl;
        std::cout << "Usage: nlm_test [options]" << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --phase2          Run Phase 2 tests (default: enabled)" << std::endl;
        std::cout << "  --phase6          Run Phase 6 integration tests (default: enabled)" << std::endl;
        std::cout << "  --no-phase6       Disable Phase 6 tests" << std::endl;
        std::cout << "  --connectivity    Run system connectivity tests (default: enabled)" << std::endl;
        std::cout << "  --no-connectivity Disable connectivity tests" << std::endl;
        std::cout << "  --memory          Run memory integration tests (default: enabled)" << std::endl;
        std::cout << "  --no-memory       Disable memory integration tests" << std::endl;
        std::cout << "  --neuromodulation  Run neuromodulation integration tests (default: enabled)" << std::endl;
        std::cout << "  --no-neuromodulation Disable neuromodulation integration tests" << std::endl;
        std::cout << "  --checkpointing    Run checkpointing tests (default: enabled)" << std::endl;
        std::cout << "  --no-checkpointing Disable checkpointing tests" << std::endl;
        std::cout << "  --replay          Run replay and consolidation tests (default: enabled)" << std::endl;
        std::cout << "  --no-replay       Disable replay tests" << std::endl;
        std::cout << "  --help            Show this help message" << std::endl;
        return 0;
    }
    
    std::cout << "=== NLM Test Suite ===" << std::endl;
    
    // Summary of test configuration
    std::cout << "Test Configuration:" << std::endl;
    std::cout << "  Phase 2 Tests: " << (options.runPhase2 ? "ENABLED" : "DISABLED") << std::endl;
    std::cout << "  Phase 6 Integration Tests: " << (options.runPhase6 ? "ENABLED" : "DISABLED") << std::endl;
    std::cout << "  System Connectivity: " << (options.runConnectivity ? "ENABLED" : "DISABLED") << std::endl;
    std::cout << "  Memory Integration: " << (options.runMemoryIntegration ? "ENABLED" : "DISABLED") << std::endl;
    std::cout << "  Neuromodulation Integration: " << (options.runNeuromodulationIntegration ? "ENABLED" : "DISABLED") << std::endl;
    std::cout << "  Checkpointing: " << (options.runCheckpointing ? "ENABLED" : "DISABLED") << std::endl;
    std::cout << "  Replay & Consolidation: " << (options.runReplay ? "ENABLED" : "DISABLED") << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    int testCount = 0;
    int passedCount = 0;
    int failedCount = 0;
    
    // Run Phase 2 tests if enabled
    if (options.runPhase2) {
        std::cout << "=== Phase 2 Tests (Real Neural Computation) ===" << std::endl;
        try {
            test_types::runAll();
            passedCount++;
            testCount++;
            std::cout << "  PASSED" << std::endl;
        } catch (const std::exception& e) {
            failedCount++;
            allPassed = false;
            std::cout << "  FAILED: " << e.what() << std::endl;
        }
        
        try {
            test_config::runAll();
            passedCount++;
            testCount++;
            std::cout << "  PASSED" << std::endl;
        } catch (const std::exception& e) {
            failedCount++;
            allPassed = false;
            std::cout << "  FAILED: " << e.what() << std::endl;
        }
        
        try {
            test_random::runAll();
            passedCount++;
            testCount++;
            std::cout << "  PASSED" << std::endl;
        } catch (const std::exception& e) {
            failedCount++;
            allPassed = false;
            std::cout << "  FAILED: " << e.what() << std::endl;
        }
        
        try {
            test_clock::runAll();
            passedCount++;
            testCount++;
            std::cout << "  PASSED" << std::endl;
        } catch (const std::exception& e) {
            failedCount++;
            allPassed = false;
            std::cout << "  FAILED: " << e.what() << std::endl;
        }
        
        try {
            test_neuron::runAll();
            passedCount++;
            testCount++;
            std::cout << "  PASSED" << std::endl;
        } catch (const std::exception& e) {
            failedCount++;
            allPassed = false;
            std::cout << "  FAILED: " << e.what() << std::endl;
        }
        
        try {
            test_synapse::runAll();
            passedCount++;
            testCount++;
            std::cout << "  PASSED" << std::endl;
        } catch (const std::exception& e) {
            failedCount++;
            allPassed = false;
            std::cout << "  FAILED: " << e.what() << std::endl;
        }
        
        try {
            test_stdp::runAll();
            passedCount++;
            testCount++;
            std::cout << "  PASSED" << std::endl;
        } catch (const std::exception& e) {
            failedCount++;
            allPassed = false;
            std::cout << "  FAILED: " << e.what() << std::endl;
        }
        
        try {
            test_brain::runAll();
            passedCount++;
            testCount++;
            std::cout << "  PASSED" << std::endl;
        } catch (const std::exception& e) {
            failedCount++;
            allPassed = false;
            std::cout << "  FAILED: " << e.what() << std::endl;
        }
        
        std::cout << "=== Phase 2 Tests Complete ===" << std::endl;
        std::cout << std::endl;
    }
    
    // Run Phase 6 integration tests if enabled
    if (options.runPhase6) {
        std::cout << "=== Phase 6 Integration Tests (Complete Brain System) ===" << std::endl;
        std::cout << "Testing all integrated brain systems..." << std::endl;
        
        try {
            test_phase6::runAll();
            passedCount++;
            testCount++;
            std::cout << "  PASSED" << std::endl;
        } catch (const std::exception& e) {
            failedCount++;
            allPassed = false;
            std::cout << "  FAILED: " << e.what() << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "Testing system connectivity and integration..." << std::endl;
        
        if (options.runConnectivity) {
            try {
                test_connectivity::runAll();
                passedCount++;
                testCount++;
                std::cout << "  PASSED" << std::endl;
            } catch (const std::exception& e) {
                failedCount++;
                allPassed = false;
                std::cout << "  FAILED: " << e.what() << std::endl;
            }
        }
        
        std::cout << std::endl;
        std::cout << "Testing memory system integration..." << std::endl;
        
        if (options.runMemoryIntegration) {
            try {
                test_memory_integration::runAll();
                passedCount++;
                testCount++;
                std::cout << "  PASSED" << std::endl;
            } catch (const std::exception& e) {
                failedCount++;
                allPassed = false;
                std::cout << "  FAILED: " << e.what() << std::endl;
            }
        }
        
        std::cout << std::endl;
        std::cout << "Testing neuromodulation integration..." << std::endl;
        
        if (options.runNeuromodulationIntegration) {
            try {
                test_neuromodulation_integration::runAll();
                passedCount++;
                testCount++;
                std::cout << "  PASSED" << std::endl;
            } catch (const std::exception& e) {
                failedCount++;
                allPassed = false;
                std::cout << "  FAILED: " << e.what() << std::endl;
            }
        }
        
        std::cout << std::endl;
        std::cout << "Testing checkpoint save/load functionality..." << std::endl;
        
        if (options.runCheckpointing) {
            try {
                test_checkpointing::runAll();
                passedCount++;
                testCount++;
                std::cout << "  PASSED" << std::endl;
            } catch (const std::exception& e) {
                failedCount++;
                allPassed = false;
                std::cout << "  FAILED: " << e.what() << std::endl;
            }
        }
        
        std::cout << std::endl;
        std::cout << "Testing replay and consolidation capabilities..." << std::endl;
        
        if (options.runReplay) {
            try {
                test_replay::runAll();
                passedCount++;
                testCount++;
                std::cout << "  PASSED" << std::endl;
            } catch (const std::exception& e) {
                failedCount++;
                allPassed = false;
                std::cout << "  FAILED: " << e.what() << std::endl;
            }
        }
        
        std::cout << "=== Phase 6 Integration Tests Complete ===" << std::endl;
        std::cout << std::endl;
    }
    
    // Test summary
    std::cout << "=== Test Summary ===" << std::endl;
    std::cout << "Total Tests: " << testCount << std::endl;
    std::cout << "Passed: " << passedCount << std::endl;
    std::cout << "Failed: " << failedCount << std::endl;
    std::cout << std::endl;
    
    if (allPassed) {
        std::cout << "=== ALL TESTS PASSED ===" << std::endl;
        return 0;
    } else {
        std::cout << "=== SOME TESTS FAILED ===" << std::endl;
        return 1;
    }
}

TestOptions TestOptions::parseArgs(int argc, char** argv) {
    TestOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--phase2") {
            options.runPhase2 = true;
        } else if (arg == "--no-phase2") {
            options.runPhase2 = false;
        } else if (arg == "--phase6") {
            options.runPhase6 = true;
        } else if (arg == "--no-phase6") {
            options.runPhase6 = false;
        } else if (arg == "--connectivity") {
            options.runConnectivity = true;
        } else if (arg == "--no-connectivity") {
            options.runConnectivity = false;
        } else if (arg == "--memory") {
            options.runMemoryIntegration = true;
        } else if (arg == "--no-memory") {
            options.runMemoryIntegration = false;
        } else if (arg == "--neuromodulation") {
            options.runNeuromodulationIntegration = true;
        } else if (arg == "--no-neuromodulation") {
            options.runNeuromodulationIntegration = false;
        } else if (arg == "--checkpointing") {
            options.runCheckpointing = true;
        } else if (arg == "--no-checkpointing") {
            options.runCheckpointing = false;
        } else if (arg == "--replay") {
            options.runReplay = true;
        } else if (arg == "--no-replay") {
            options.runReplay = false;
        } else if (arg == "--help" || arg == "-h") {
            options.help = true;
        }
    }
    
    return options;
}
