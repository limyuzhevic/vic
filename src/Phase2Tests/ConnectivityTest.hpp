// Phase 2 Connectivity Test Implementation
#pragma once

#include "core/Phase2TestFramework.hpp"
#include "core/Phase2TestUtils.hpp"
#include <memory>

namespace nlm {
namespace phase2 {

/**
 * Phase 2 connectivity test
 */
class ConnectivityTest : public Phase2Test {
public:
    ConnectivityTest() = default;
    ~ConnectivityTest() override = default;
    
    TestResult run(std::shared_ptr<Config> config) override;
    std::string getName() const override { return "Connectivity Test"; }
    std::string getDescription() const override { 
        return "Tests basic neural connectivity and spike propagation"; 
    }
    bool shouldRun(const Phase2TestConfig& config) const override { 
        return config.runConnectivityTest; 
    }
};

TestResult ConnectivityTest::run(std::shared_ptr<Config> config) {
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
        
        // Inject current into neurons and test spike propagation
        TestUtils::applyInputCurrent(brain, 10, NeuronType::Sensory, PHASE2_INJECT_CURRENT);
        
        // Run simulation
        TestUtils::runSimulation(brain, PHASE2_CONNECTIVITY_TEST_STEPS);
        
        // Check if spikes were generated
        uint64_t spikes = brain->getTotalSpikeCount();
        uint64_t activeNeurons = brain->getFiringNeuronCount();
        
        result.passed = (spikes > 0);
        result.message = "Connectivity test " + std::string(result.passed ? "passed" : "failed");
        result.details.push_back("Spikes generated: " + std::to_string(spikes));
        result.details.push_back("Active neurons: " + std::to_string(activeNeurons));
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.message = "Error running connectivity test: " + std::string(e.what());
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.executionTime = std::chrono::duration<double>(endTime - startTime).count();
    
    return result;
}

} // namespace phase2
} // namespace nlm