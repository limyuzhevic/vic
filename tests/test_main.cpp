#include <iostream>
#include <string>

// Include all test modules
#include "test_types.cpp"
#include "test_config.cpp"
#include "test_random.cpp"
#include "test_clock.cpp"
#include "test_neuron.cpp"
#include "test_synapse.cpp"
#include "test_stdp.cpp"
#include "test_brain.cpp"
#include "test_phase6_integration.cpp"

// Test runner declarations
namespace test_types { void runAll(); }
namespace test_config { void runAll(); }
namespace test_random { void runAll(); }
namespace test_clock { void runAll(); }
namespace test_neuron { void runAll(); }
namespace test_synapse { void runAll(); }
namespace test_stdp { void runAll(); }
namespace test_brain { void runAll(); }
namespace test_phase6_integration { void runPhase6IntegrationTests(); }

int main(int argc, char** argv) {
    std::cout << "=== NLM Test Suite ===" << std::endl;
    
    // Parse command line arguments for test selection
    bool runAllTests = true;
    bool runConfigTests = false;
    bool runBrainTests = false;
    bool runPhase6Tests = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--all") {
            runAllTests = true;
            runConfigTests = true;
            runBrainTests = true;
            runPhase6Tests = true;
        } else if (arg == "--config") {
            runConfigTests = true;
        } else if (arg == "--brain") {
            runBrainTests = true;
        } else if (arg == "--phase6") {
            runPhase6Tests = true;
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: nlm_test [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --all         Run all tests" << std::endl;
            std::cout << "  --config      Run configuration tests only" << std::endl;
            std::cout << "  --brain       Run brain tests only" << std::endl;
            std::cout << "  --phase6       Run Phase 6 integration tests only" << std::endl;
            std::cout << "  --help, -h    Show this help message" << std::endl;
            return 0;
        }
    }
    
    int passedTests = 0;
    int failedTests = 0;
    
    // Run test suite based on selection
    try {
        if (runAllTests || runConfigTests) {
            std::cout << "\n--- Running Configuration Tests ---" << std::endl;
            test_config::runAll();
            passedTests++;
        }
        
        if (runAllTests || runBrainTests) {
            std::cout << "\n--- Running Brain Tests ---" << std::endl;
            test_brain::runAll();
            passedTests++;
        }
        
        if (runAllTests || runPhase6Tests) {
            std::cout << "\n--- Running Phase 6 Integration Tests ---" << std::endl;
            test_phase6_integration::runPhase6IntegrationTests();
            passedTests++;
        }
        
        // Always run basic tests unless specifically excluded
        if (runAllTests) {
            std::cout << "\n--- Running Basic Unit Tests ---" << std::endl;
            test_types::runAll();
            test_random::runAll();
            test_clock::runAll();
            test_neuron::runAll();
            test_synapse::runAll();
            test_stdp::runAll();
            passedTests += 6;
        }
        
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "=== TEST SUITE FAILED ===" << std::endl;
        std::cout << "Exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cout << std::endl;
        std::cout << "=== TEST SUITE FAILED ===" << std::endl;
        std::cout << "Unknown exception occurred" << std::endl;
        return 1;
    }
    
    // Summary
    std::cout << std::endl;
    std::cout << "=== TEST SUMMARY ===" << std::endl;
    std::cout << "Tests passed: " << passedTests << std::endl;
    std::cout << "Tests failed: " << failedTests << std::endl;
    std::cout << "Total tests executed: " << passedTests << std::endl;
    
    if (failedTests > 0) {
        std::cout << "\n=== SOME TESTS FAILED ===" << std::endl;
        return 1;
    }
    
    std::cout << "\n=== ALL TESTS PASSED ===" << std::endl;
    std::cout << "NLM test suite completed successfully!" << std::endl;
    
    return 0;
}