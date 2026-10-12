// Integration test assertions
// 
// Specialized assertions for NLM integration testing.
// Provides enhanced test assertions for neural system integration testing.

#pragma once

#include "IntegrationTest.hpp"
#include <string>
#include <vector>
#include <sstream>
#include <cmath>

namespace nlm {
namespace test {

// Integration test assertions
class IntegrationTestAssertions {
public:
    IntegrationTestAssertions();
    ~IntegrationTestAssertions();
    
    // Memory system assertions
    static void assertMemoryIntegration(Brain* brain, const std::string& testContext);
    static void assertWorkingMemoryState(NeuralWorkingMemory* workingMemory, float expectedActivation, float tolerance = 0.1f);
    static void assertEpisodicMemoryState(NeuralEpisodicMemory* episodicMemory, size_t expectedEpisodeCount);
    static void assertAssociativeMemoryState(NeuralAssociativeMemory* associativeMemory, NeuronId neuronA, NeuronId neuronB, float expectedStrength, float tolerance = 0.1f);
    
    // Neuromodulation system assertions
    static void assertNeuromodulationIntegration(Brain* brain, const std::string& testContext);
    static void assertDopamineState(Dopamine* dopamine, float expectedLevel, float tolerance = 0.1f);
    static void assertCuriosityState(Curiosity* curiosity, float expectedLevel, float tolerance = 0.1f);
    static void assertNoveltyState(Novelty* novelty, float expectedLevel, float tolerance = 0.1f);
    static void assertPredictionErrorState(PredictionError* predictionError, float expectedError, float tolerance = 0.1f);
    static void assertRewardState(Reward* reward, float expectedValue, float tolerance = 0.1f);
    
    // Prediction system assertions
    static void assertPredictionSystemIntegration(Brain* brain, const std::string& testContext);
    static void assertPredictionAccuracy(PredictionSystem* predictionSystem, float expectedAccuracy, float tolerance = 0.1f);
    static void assertPredictionError(PredictionSystem* predictionSystem, float expectedError, float tolerance = 0.1f);
    static void assertPredictionConfidence(PredictionSystem* predictionSystem, float expectedConfidence, float tolerance = 0.1f);
    
    // Cognition system assertions
    static void assertCognitionSystemIntegration(Brain* brain, const std::string& testContext);
    static void assertConceptFormationState(ConceptFormation* conceptFormation, size_t expectedConceptCount);
    static void assertConceptFormationIntegrity(ConceptFormation* conceptFormation, float minStability = 0.5f);
    static void assertPlanningState(NeuralPlanner* planner, float expectedConfidence, float tolerance = 0.1f);
    static void assertPlanningIntegrity(NeuralPlanner* planner, size_t maxDepth = 5);
    
    // Development system assertions
    static void assertDevelopmentSystemIntegration(Brain* brain, const std::string& testContext);
    static void assertDevelopmentalStage(DevelopmentSystem* developmentSystem, DevelopmentalStage expectedStage);
    static void assertCriticalPeriod(DevelopmentSystem* developmentSystem, bool expectedCriticalPeriod, float tolerance = 0.1f);
    static void assertPlasticityModifier(DevelopmentSystem* developmentSystem, float expectedPlasticity, float tolerance = 0.1f);
    
    // Cross-system integration assertions
    static void assertCompleteBrainLoop(Brain* brain);
    static void assertMemoryDevelopmentIntegration(Brain* brain);
    static void assertNeuromodulationMotivationIntegration(Brain* brain);
    static void assertPredictionBehaviorIntegration(Brain* brain);
    
    // Performance assertions
    static void assertPerformanceThreshold(double executionTime, double threshold, const std::string& operationName);
    static void assertMemoryUsage(size_t memoryUsage, size_t maxMemoryUsage, const std::string& memoryType);
    static void assertCPUUsage(float cpuUsage, float maxCPUUsage, const std::string& operationName);
    
    // Statistical assertions
    static void assertStatisticalSignificance(const std::vector<double>& values, double confidence = 0.95, double threshold = 0.05);
    static void assertMeanValue(const std::vector<double>& values, double expectedMean, double tolerance = 0.1f);
    static void assertVariance(const std::vector<double>& values, double expectedVariance, double tolerance = 0.1f);
    static void assertCorrelation(const std::vector<double>& values1, const std::vector<double>& values2, double expectedCorrelation, double tolerance = 0.1f);
    
    // Pattern matching assertions
    static void assertPatternMatching(const std::vector<float>& pattern1, const std::vector<float>& pattern2, float expectedSimilarity, float tolerance = 0.1f);
    static void assertPatternCompletion(const std::vector<float>& originalPattern, const std::vector<float>& completedPattern, float expectedCompletion, float tolerance = 0.1f);
    static void assertPatternAssociation(NeuralAssociativeMemory* associativeMemory, NeuronId neuronA, NeuronId neuronB, float expectedStrength, float tolerance = 0.1f);
    
    // Time-based assertions
    static void assertExecutionTime(double executionTime, double maxExecutionTime, const std::string& operationName);
    static void assertMemoryDecay(float initialActivation, float decayedActivation, TimestepDuration dt, float decayRate, float tolerance = 0.1f);
    static void assertDevelopmentTimeline(Brain* brain, TimestepDuration expectedStep, double expectedAge);
    
    // Integration error reporting
    static std::string generateIntegrationErrorMessage(const std::string& system1, const std::string& system2, const std::string& issue);
    static void logAssertionFailure(const std::string& assertion, const std::string& message);
    
    // Helper methods
    static std::string formatAssertionMessage(const std::string& assertion, const std::string& expected, const std::string& actual);
    static std::string formatSystemName(const std::string& systemName);
    
private:
    // Helper assertions
    static bool compareWithTolerance(float value1, float value2, float tolerance);
    static bool compareArraysWithTolerance(const std::vector<float>& arr1, const std::vector<float>& arr2, float tolerance);
}; // class IntegrationTestAssertions

} // namespace test
} // namespace nlm