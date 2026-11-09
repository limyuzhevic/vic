#ifndef INTEGRATION_TEST_HPP
#define INTEGRATION_TEST_HPP

#include "Brain.hpp"
#include <chrono>
#include <thread>

namespace nlm {

class IntegrationTest {
public:
    IntegrationTest() = default;
    ~IntegrationTest() = default;
    
    // Comprehensive integration test suite
    struct TestResults {
        bool predictionSystemIntegrated = false;
        bool conceptFormationIntegrated = false;
        bool memorySystemsIntegrated = false;
        bool neuromodulationIntegrated = false;
        bool developmentIntegrated = false;
        bool predictionErrorIntegration = false;
        bool conceptPredictionInteraction = false;
        bool workingMemoryPredictionFeedback = false;
        bool episodicMemoryReplay = false;
        float totalScore = 0.0f;
    };
    
    // Test prediction system integration with neural dynamics
    bool testPredictionSystemIntegration();
    
    // Test concept formation integration with prediction systems
    bool testConceptFormationIntegration();
    
    // Test memory system integration with neural substrate
    bool testMemorySystemIntegration();
    
    // Test neuromodulation integration with prediction errors
    bool testNeuromodulationIntegration();
    
    // Test development system integration
    bool testDevelopmentIntegration();
    
    // Test cross-system interactions
    bool testCrossSystemInteractions();
    
    // Run full integration test suite
    TestResults runFullIntegrationTest();
    
    // Performance benchmark
    struct PerformanceMetrics {
        double totalTime = 0.0;
        size_t neuronsTested = 0;
        size_t predictions = 0;
        size_t conceptsDiscovered = 0;
        float avgPredictionAccuracy = 0.0f;
    };
    
    PerformanceMetrics benchmarkIntegration(int durationSeconds = 60);
    
    // Stress test the integrated brain
    bool stressTest(const std::vector<float>& testPattern, 
                   size_t steps, 
                   float targetAccuracy);
    
    // Test replay and consolidation
    bool testReplayConsolidation();
    
    // Test learning dynamics over time
    bool testLongTermLearning();
    
private:
    // Helper functions for testing
    bool createTestBrain(std::shared_ptr<Config>& cfg, 
                         std::shared_ptr<Brain>& brain,
                         const std::string& testName);
    
    bool verifySystemConnections(const std::shared_ptr<Brain>& brain,
                                 const std::string& testName);
    
    bool measurePredictionAccuracy(std::shared_ptr<Brain>& brain,
                                   size_t steps);
    
    bool measureConceptStability(std::shared_ptr<Brain>& brain,
                                  size_t steps);
    
    bool measureMemoryRetention(std::shared_ptr<Brain>& brain,
                                size_t steps);
    
    float calculateIntegrationScore(const TestResults& results);
};

} // namespace nlm

#endif // INTEGRATION_TEST_HPP