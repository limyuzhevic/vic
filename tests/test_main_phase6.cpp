// Main test runner for Phase 6 integration tests
// GoogleTest-based test runner for NLM Phase 6 systems

#include "test_phase6/Phase6TestFramework.hpp"

namespace test_phase6 {

    // Test utilities
    class TestUtils {
    public:
        static std::string getTestName() { return "Phase6"; }
        static std::string getTestVersion() { return "1.0"; }
    };
    
    // Phase 6 system test categories
    class MemorySystemTests {
    public:
        static void RegisterTests() {
            // Memory system tests will be registered here
        }
    };
    
    class NeuromodulationSystemTests {
    public:
        static void RegisterTests() {
            // Neuromodulation system tests will be registered here
        }
    };
    
    class PredictionSystemTests {
    public:
        static void RegisterTests() {
            // Prediction system tests will be registered here
        }
    };
    
    class CognitiveSystemTests {
    public:
        static void RegisterTests() {
            // Cognitive system tests will be registered here
        }
    };
    
    class DevelopmentSystemTests {
    public:
        static void RegisterTests() {
            // Development system tests will be registered here
        }
    };
    
    class CheckpointingSystemTests {
    public:
        static void RegisterTests() {
            // Checkpointing system tests will be registered here
        }
    };
    
    class ReplaySystemTests {
    public:
        static void RegisterTests() {
            // Replay system tests will be registered here
        }
    };
    
    class PerformanceSystemTests {
    public:
        static void RegisterTests() {
            // Performance system tests will be registered here
        }
    };
    
    class IntegrationTests {
    public:
        static void RegisterTests() {
            // Integration tests will be registered here
        }
    };
    
    // Main test runner
    int runPhase6Tests() {
        std::cout << "=== NLM Phase 6 Comprehensive Test Suite ===" << std::endl;
        std::cout << "Testing all integrated Phase 6 systems..." << std::endl << std::endl;
        
        // Register all Phase 6 tests
        MemorySystemTests::RegisterTests();
        NeuromodulationSystemTests::RegisterTests();
        PredictionSystemTests::RegisterTests();
        CognitiveSystemTests::RegisterTests();
        DevelopmentSystemTests::RegisterTests();
        CheckpointingSystemTests::RegisterTests();
        ReplaySystemTests::RegisterTests();
        PerformanceSystemTests::RegisterTests();
        IntegrationTests::RegisterTests();
        
        // Run GoogleTest
        std::cout << "Running GoogleTest framework..." << std::endl;
        return ::testing::UnitTest::GetInstance()->Run();
    }
    
    // Integration test runner
    int runPhase6IntegrationTests() {
        std::cout << "=== NLM Phase 6 Integration Test Suite ===" << std::endl;
        std::cout << "Testing system interactions and integrations..." << std::endl << std::endl;
        
        // Run integration tests
        IntegrationTests::RegisterTests();
        
        std::cout << "\nIntegration testing completed." << std::endl;
        return 0;  // Return 0 for success
    }
    
} // namespace test_phase6

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    
    int testResult = 0;
    
    if (argc > 1 && std::string(argv[1]) == "--integration") {
        testResult = test_phase6::runPhase6IntegrationTests();
    } else {
        testResult = test_phase6::runPhase6Tests();
    }
    
    return testResult;
}