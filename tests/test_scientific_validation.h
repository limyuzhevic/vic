// NLM Scientific Validation Test Suite
// Phase 6 Comprehensive Testing Framework
// Based on Phase 6 audit requirements

#ifndef TEST_SCIENTIFIC_VALIDATION_H
#define TEST_SCIENTIFIC_VALIDATION_H

#include "src/brain/Brain.hpp"
#include "src/memory/Memory.hpp"
#include "src/prediction/PredictionSystem.hpp"
#include "src/cognition/NeuralPlanner.hpp"
#include "src/neuromodulation/Neuromodulator.hpp"
#include "src/experiments/ExperimentRunner.hpp"
#include "src/experiments/Metrics.hpp"
#include "src/core/Config/Config.hpp"
#include "src/core/Random/Random.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/agent/SensoryPercept.hpp"
#include "src/agent/MotorCommand.hpp"
#include "src/world/SimpleWorld.hpp"
#include "src/environment/Environment.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <map>
#include <random>
#include <chrono>
#include <iomanip>
#include <memory>

namespace test_scientific_validation {

using namespace nlm;

class ScientificValidationTestSuite {
public:
    ScientificValidationTestSuite();
    ~ScientificValidationTestSuite();
    
    void runAllTests();
    
    void setRandomSeed(uint64_t seed);
    uint64_t getRandomSeed() const;
    std::mt19937& getRandomGenerator();
    
protected:
    uint64_t randomSeed;
    std::mt19937 randomGenerator;
    
    // Helper methods
    float getRandomFloat(float min, float max);
    int getRandomInt(int min, int max);
    float calculateMean(const std::vector<float>& values);
    float calculateStdDev(const std::vector<float>& values, float mean);
    float calculateAreaUnderCurve(const std::vector<float>& yValues, float timeStep = 1.0f);
    bool checkStatisticalSignificance(const std::vector<float>& before, const std::vector<float>& after, float threshold = 0.05f);
    
    // Test infrastructure
    std::shared_ptr<Brain> createTestBrain(size_t neuronCount = 500, size_t regionCount = 2);
    std::shared_ptr<Environment> createTestEnvironment(int width = 16, int height = 16);
    std::shared_ptr<AgentBrain> createTestAgent(std::shared_ptr<Brain> brain, 
                                                std::shared_ptr<Environment> environment);
    bool runSimulation(std::shared_ptr<AgentBrain> agent, 
                      std::shared_ptr<Environment> environment,
                      uint64_t steps);
    
private:
    // Test category implementations
    void runMemoryRetentionTests();
    void runLearningProgressTests();
    void runContinualLearningTests();
    void runGeneralizationTests();
    void runPredictionAccuracyTests();
    void runIndividualDifferencesTests();
};

} // namespace test_scientific_validation

#endif // TEST_SCIENTIFIC_VALIDATION_H

