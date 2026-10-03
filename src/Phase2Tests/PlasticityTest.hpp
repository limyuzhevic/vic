// Phase 2 Plasticity Test Implementation
#pragma once

#include "core/Phase2TestFramework.hpp"
#include "core/Phase2TestUtils.hpp"
#include <memory>
#include <algorithm>

namespace nlm {
namespace phase2 {

/**
 * Phase 2 plasticity test
 */
class PlasticityTest : public Phase2Test {
public:
    PlasticityTest() = default;
    ~PlasticityTest() override = default;
    
    TestResult run(std::shared_ptr<Config> config) override;
    std::string getName() const override { return "Plasticity Test"; }
    std::string getDescription() const override { 
        return "Tests synaptic plasticity and learning through experience"; 
    }
    bool shouldRun(const Phase2TestConfig& config) const override { 
        return config.runPlasticityTest; 
    }
};

TestResult PlasticityTest::run(std::shared_ptr<Config> config) {
    TestResult result(getName());
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        // Create brain
        auto brain = TestUtils::createBrain(config);
        if (!brain->initialize()) {
            result.passed = false;
            result.message = "Failed to initialize brain";
            return result;
        }
        
        // Record initial state
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            result.passed = false;
            result.message = "No brain regions found";
            return result;
        }
        
        std::vector<float> initialWeights;
        for (const auto& syn : region->getSynapses()) {
            initialWeights.push_back(syn->getWeight());
        }
        
        // Enable plasticity
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
        
        // Apply repeated input pattern
        TestUtils::createInputPattern(brain, 20, PHASE2_CURRENT_STRENGTH);
        TestUtils::runSimulation(brain, PHASE2_PLASTICITY_STEPS);
        
        // Record final state
        std::vector<float> finalWeights;
        for (const auto& syn : region->getSynapses()) {
            finalWeights.push_back(syn->getWeight());
        }
        
        // Calculate weight changes
        auto weightChanges = TestUtils::calculateWeightChange(initialWeights, finalWeights);
        
        // Check if learning occurred
        bool learningOccurred = TestUtils::learningOccurred(initialWeights, finalWeights);
        
        result.passed = learningOccurred;
        result.message = "Plasticity test " + std::string(result.passed ? "passed" : "failed");
        result.details.push_back("Initial mean weight: " + std::to_string(weightChanges.initialMean));
        result.details.push_back("Final mean weight: " + std::to_string(weightChanges.finalMean));
        result.details.push_back("Weight change: " + std::to_string(weightChanges.change));
        result.details.push_back("Strengthened: " + std::to_string(weightChanges.strengthened));
        result.details.push_back("Weakened: " + std::to_string(weightChanges.weakened));
        result.details.push_back("Unchanged: " + std::to_string(weightChanges.unchanged));
        result.details.push_back("Learning occurred: " + std::string(learningOccurred ? "Yes" : "No"));
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.message = "Error running plasticity test: " + std::string(e.what());
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.executionTime = std::chrono::duration<double>(endTime - startTime).count();
    
    return result;
}

} // namespace phase2
} // namespace nlm