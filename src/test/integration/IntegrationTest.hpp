// Integration test framework base class for NLM brain systems integration testing
// 
// This base class provides common functionality for testing the integration of all brain systems
// in the Neural Learning Machine (NLM) architecture. It includes:
// - Common test setup and teardown
// - Test utilities for brain system interaction
// - Integration test assertions for neural systems
// - Performance monitoring and validation
// - Cross-system test data management

#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../memory/Memory.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../neuromodulation/Reward.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../prediction/NeuralPrediction.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../development/DevelopmentSystem.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <chrono>
#include <functional>

namespace nlm {
namespace test {

// Integration test configuration
struct IntegrationTestConfig {
    // Simulation parameters
    SimulationStep maxSteps = 1000;
    float timestep = 0.01f;
    size_t neuronCount = 100;
    size_t regionCount = 2;
    
    // Test parameters
    float rewardThreshold = 0.5f;
    float curiosityThreshold = 0.3f;
    float predictionErrorThreshold = 0.2f;
    float memoryRetentionThreshold = 0.7f;
    
    // Performance thresholds
    float maxMemoryUpdateTime = 0.01f; // seconds
    float maxIntegrationTime = 0.05f; // seconds
    size_t maxActiveNeurons = 50;
    
    // Development parameters
    double initialAge = 0.0;
    double maturityAge = 100.0;
};

// Base class for all integration tests
class IntegrationTest : public ::testing::Test {
protected:
    IntegrationTest() = default;
    virtual ~IntegrationTest() = default;
    
    // Test setup - called before each test
    void SetUp() override;
    
    // Test teardown - called after each test
    void TearDown() override;
    
    // Helper methods for integration testing
    std::shared_ptr<Brain> createBrain(const IntegrationTestConfig& config);
    std::shared_ptr<WorkingMemory> getWorkingMemory(Brain* brain);
    std::shared_ptr<EpisodicMemory> getEpisodicMemory(Brain* brain);
    std::shared_ptr<AssociativeMemory> getAssociativeMemory(Brain* brain);
    std::shared_ptr<Reward> getRewardSystem(Brain* brain);
    std::shared_ptr<PredictionError> getPredictionErrorSystem(Brain* brain);
    std::shared_ptr<Curiosity> getCuriositySystem(Brain* brain);
    std::shared_ptr<Novelty> getNoveltySystem(Brain* brain);
    std::shared_ptr<PredictionSystem> getPredictionSystem(Brain* brain);
    std::shared_ptr<NeuralPrediction> getNeuralPredictionSystem(Brain* brain);
    std::shared_ptr<ConceptFormation> getConceptFormationSystem(Brain* brain);
    std::shared_ptr<NeuralPlanner> getPlannerSystem(Brain* brain);
    std::shared_ptr<DevelopmentSystem> getDevelopmentSystem(Brain* brain);
    std::shared_ptr<Neuromodulator> getNeuromodulatorSystem(Brain* brain);
    
    // Integration test utilities
    template<typename T>
    bool compareArrays(const std::vector<T>& arr1, const std::vector<T>& arr2, float tolerance = 1e-6f);
    
    template<typename T>
    bool arraysEqual(const std::vector<T>& arr1, const std::vector<T>& arr2);
    
    float calculateCrossCorrelation(const std::vector<float>& vec1, const std::vector<float>& vec2);
    
    // Performance monitoring
    double measureExecutionTime(std::function<void()> func);
    
    // Memory system integration helpers
    void testMemorySystemIntegration(Brain* brain, const std::string& testName);
    void testWorkingMemoryIntegration(Brain* brain);
    void testEpisodicMemoryIntegration(Brain* brain);
    void testAssociativeMemoryIntegration(Brain* brain);
    void testMemoryConsolidation(Brain* brain);
    
    // Neuromodulation system integration helpers
    void testNeuromodulationSystemIntegration(Brain* brain, const std::string& testName);
    void testDopamineIntegration(Brain* brain);
    void testCuriosityIntegration(Brain* brain);
    void testNoveltyIntegration(Brain* brain);
    void testPredictionErrorIntegration(Brain* brain);
    
    // Prediction system integration helpers
    void testPredictionSystemIntegration(Brain* brain, const std::string& testName);
    void testForwardPredictionIntegration(Brain* brain);
    void testValuePredictionIntegration(Brain* brain);
    void testBehavioralPredictionIntegration(Brain* brain);
    void testMetaPredictionIntegration(Brain* brain);
    
    // Cognition system integration helpers
    void testCognitionSystemIntegration(Brain* brain, const std::string& testName);
    void testConceptFormationIntegration(Brain* brain);
    void testAttentionSelectionIntegration(Brain* brain);
    void testPlanningIntegration(Brain* brain);
    void testSelfModelIntegration(Brain* brain);
    
    // Development system integration helpers
    void testDevelopmentSystemIntegration(Brain* brain, const std::string& testName);
    void testPlasticityMaturationIntegration(Brain* brain);
    void testCriticalPeriodIntegration(Brain* brain);
    void testStructuralPlasticityIntegration(Brain* brain);
    void testAdaptiveDevelopmentIntegration(Brain* brain);
    
    // Cross-system workflow helpers
    void testCompleteBrainLoop(Brain* brain);
    void testMemoryDevelopmentInteraction(Brain* brain);
    void testNeuromodulationMotivationIntegration(Brain* brain);
    void testPredictionBehaviorIntegration(Brain* brain);
    
    // Member variables
    IntegrationTestConfig config_;
    std::shared_ptr<Brain> brain_;
    bool initialized_ = false;
}; // class IntegrationTest

} // namespace test
} // namespace nlm