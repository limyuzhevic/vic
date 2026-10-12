// Integration test utilities
// 
// Common utilities for NLM integration testing.
// Provides utilities for brain system interaction, data processing, and test utilities.

#pragma once

#include "IntegrationTest.hpp"
#include <vector>
#include <string>
#include <chrono>
#include <random>
#include <algorithm>
#include <cmath>

namespace nlm {
namespace test {

// Integration test utility functions
class IntegrationTestUtils {
public:
    IntegrationTestUtils();
    ~IntegrationTestUtils();
    
    // Brain system utilities
    static std::shared_ptr<Brain> createTestBrain(size_t neuronCount = 100, size_t regionCount = 2);
    static std::shared_ptr<Memory> getMemorySystem(Brain* brain);
    static std::shared_ptr<Neuromodulator> getNeuromodulatorSystem(Brain* brain);
    static std::shared_ptr<PredictionSystem> getPredictionSystem(Brain* brain);
    static std::shared_ptr<ConceptFormation> getConceptFormationSystem(Brain* brain);
    static std::shared_ptr<NeuralPlanner> getPlannerSystem(Brain* brain);
    static std::shared_ptr<DevelopmentSystem> getDevelopmentSystem(Brain* brain);
    
    // Data processing utilities
    static std::vector<float> generateRandomPattern(size_t size, float min = 0.0f, float max = 1.0f);
    static std::vector<float> normalizePattern(const std::vector<float>& pattern);
    static float calculatePatternSimilarity(const std::vector<float>& pattern1, const std::vector<float>& pattern2);
    static float calculatePatternCorrelation(const std::vector<float>& pattern1, const std::vector<float>& pattern2);
    
    // Test utilities
    static double measureExecutionTime(std::function<void()> func);
    static bool compareArrays(const std::vector<float>& arr1, const std::vector<float>& arr2, float tolerance = 1e-6f);
    static bool arraysEqual(const std::vector<float>& arr1, const std::vector<float>& arr2);
    static std::string generateUniqueTestId(const std::string& prefix = "test");
    
    // Integration utilities
    static void logIntegrationTest(const std::string& testName, bool passed, double executionTime);
    static void printTestResults(const std::vector<ExperimentRunnerIntegration::TestResult>& results);
    
    // Performance utilities
    static bool checkPerformanceThreshold(double executionTime, double threshold);
    static double getAverageExecutionTime(const std::vector<double>& times);
    static double getStandardDeviation(const std::vector<double>& values);
    
    // Memory system utilities
    static void verifyMemoryCapacity(NeuralWorkingMemory* workingMemory, size_t expectedCapacity);
    static void verifyEpisodicMemoryIntegrity(NeuralEpisodicMemory* episodicMemory);
    static void verifyAssociativeMemoryIntegrity(NeuralAssociativeMemory* associativeMemory);
    
    // Neuromodulation utilities
    static void verifyNeuromodulationState(Reward* reward, PredictionError* predError, Curiosity* curiosity, Novelty* novelty);
    static float calculateNeuromodulationImpact(Reward* reward, PredictionError* predError);
    
    // Prediction system utilities
    static void verifyPredictionSystemState(PredictionSystem* predictionSystem);
    static float calculatePredictionAccuracy(PredictionSystem* predictionSystem);
    
    // Cognition system utilities
    static void verifyConceptFormationState(ConceptFormation* conceptFormation);
    static void verifyPlanningState(NeuralPlanner* planner);
    static float calculateCognitivePerformance(ConceptFormation* conceptFormation, NeuralPlanner* planner);
    
    // Development system utilities
    static void verifyDevelopmentSystemState(DevelopmentSystem* developmentSystem);
    static float calculatePlasticityImpact(DevelopmentSystem* developmentSystem);
    
    // Random number generation
    static std::mt19937& getRandomGenerator();
    
private:
    static std::mt19937 randomGenerator_;
}; // class IntegrationTestUtils

} // namespace test
} // namespace nlm