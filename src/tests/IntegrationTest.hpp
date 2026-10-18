// Test Integration Test
// 
// Comprehensive integration test for the complete NLM brain loop
// This test verifies that all systems (memory, neuromodulation, cognition, etc.)
// work together correctly in a realistic brain simulation scenario.
//
// Integration Test Structure:
// 1. System Initialization
// 2. Sensory Processing Verification
// 3. Motor Action Selection Verification  
// 4. Memory System Integration Verification
// 5. Neuromodulation Integration Verification
// 6. Plasticity Integration Verification
// 7. Checkpoint/Restore Verification
// 8. Stress Test (large-scale simulation)
//
// Test Framework:
// - Uses unit test framework (Google Test or similar)
// - Independent of specific simulation parameters
// - Comprehensive coverage of integration points
// - Performance benchmarks included
// - Validation of brain loop closure

#ifndef NLM_INTEGRATION_TEST_HPP
#define NLM_INTEGRATION_TEST_HPP

#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "memory/WorkingMemory.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/NeuralAssociativeMemory.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "neuromodulation/Novelty.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/AttentionalSelection.hpp"
#include "prediction/PredictionSystem.hpp"
#include "development/DevelopmentSystem.hpp"
#include "performance/CheckpointSystem.hpp"
#include <memory>
#include <vector>
#include <string>
#include <chrono>
#include <random>
#include <fstream>

namespace nlm {

// Integration test metrics structure
struct IntegrationMetrics {
    int totalTests = 0;
    int passedTests = 0;
    int failedTests = 0;
    
    struct TestResult {
        std::string testName;
        bool passed = false;
        std::string message;
        std::chrono::system_clock::time_point timestamp;
    };
    
    std::vector<TestResult> testResults;
};

// Integration test class
class IntegrationTest {
public:
    IntegrationTest();
    ~IntegrationTest();
    
    // Initialize test infrastructure
    bool initialize();
    
    // Run all integration tests
    bool runAllTests();
    
    // Cleanup test resources
    void cleanup();
    
    // Get test results
    const IntegrationMetrics& getTestResults() const { return testResults; }
    
    // Print test summary
    void printTestSummary();
    
private:
    // Test suite methods
    bool runSystemInitializationTests();
    bool runSensoryProcessingTests();
    bool runMotorActionTests();
    bool runMemoryIntegrationTests();
    bool runNeuromodulationTests();
    bool runPlasticityTests();
    bool runDevelopmentTests();
    bool runCheckpointTests();
    bool runStressTests();
    
    // Helper methods
    void logTestResult(const std::string& testName, bool passed, const std::string& message);
    void setupConfiguration(std::shared_ptr<Config> config);
    
    // Test data
    IntegrationMetrics testResults;
    
    // Test components
    std::shared_ptr<Config> config;
    Brain* brain;
    AgentBrain* agentBrain;
    SimpleWorld* world;
    IntegrationMetrics* metricsCollector;
    std::unique_ptr<std::mt19937> randomGenerator;
};

} // namespace nlm

#endif // NLM_INTEGRATION_TEST_HPP