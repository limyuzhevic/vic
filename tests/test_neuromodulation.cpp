#include "core/TestFramework/TestFramework.hpp"
#include "core/Neuromodulation/Neuromodulator.hpp"
#include <cassert>
#include <iostream>

namespace test_neuromodulation {

class NeuromodulationTestSuite : public TestSuite {
public:
    std::string getName() const override {
        return "Neuromodulation";
    }
    
    std::vector<TestResult> runTests() override {
        std::vector<TestResult> results;
        
        auto start = std::chrono::high_resolution_clock::now();
        
        try {
            // Test 1: Dopamine functionality
            testDopamineFunctionality();
            results.emplace_back("DopamineFunctionality", true, "Dopamine functionality test passed", getDuration(start));
        } catch (const std::exception& e) {
            results.emplace_back("DopamineFunctionality", false, std::string("Exception: ") + e.what(), getDuration(start), "std::exception", 0);
        }
        
        start = std::chrono::high_resolution_clock::now();
        try {
            // Test 2: Acetylcholine functionality
            testAcetylcholineFunctionality();
            results.emplace_back("AcetylcholineFunctionality", true, "Acetylcholine functionality test passed", getDuration(start));
        } catch (const std::exception& e) {
            results.emplace_back("AcetylcholineFunctionality", false, std::string("Exception: ") + e.what(), getDuration(start), "std::exception", 0);
        }
        
        return results;
    }
    
private:
    double getDuration(std::chrono::high_resolution_clock::time_point start) {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(end - start).count();
    }
    
    void testDopamineFunctionality() {
        // Create Dopamine instance
        nlm::Dopamine dopamine;
        
        // Test initial level
        ASSERT_TRUE(dopamine.getLevel() == 0.0f);
        
        // Test setting level
        dopamine.setLevel(0.5f);
        ASSERT_TRUE(dopamine.getLevel() == 0.5f);
        
        // Test plasticity factor calculation
        float plasticityFactor = dopamine.getPlasticityFactor();
        ASSERT_TRUE(plasticityFactor >= 0.0f && plasticityFactor <= 2.0f);
        
        // Test reward signaling
        dopamine.signalReward(1.0f);
        ASSERT_TRUE(dopamine.getLevel() > 0.0f);
        
        // Test reward prediction error signaling
        dopamine.signalRewardPredictionError(0.1f);
        ASSERT_TRUE(dopamine.getLevel() > 0.0f);
        
        // Test update
        dopamine.update(0.001f);
        // Level should decay towards baseline
        ASSERT_TRUE(dopamine.getLevel() >= 0.0f);
    }
    
    void testAcetylcholineFunctionality() {
        // Create Acetylcholine instance
        nlm::Acetylcholine acetylcholine;
        
        // Test initial level
        ASSERT_TRUE(acetylcholine.getLevel() == 0.0f);
        
        // Test setting level
        acetylcholine.setLevel(0.3f);
        ASSERT_TRUE(acetylcholine.getLevel() == 0.3f);
        
        // Test plasticity factor calculation
        float plasticityFactor = acetylcholine.getPlasticityFactor();
        ASSERT_TRUE(plasticityFactor > 0.0f);
        
        // Test update
        acetylcholine.update(0.001f);
        // Level should decay towards baseline
        ASSERT_TRUE(acetylcholine.getLevel() >= 0.0f);
    }
};

} // namespace test_neuromodulation

// Test runner
int main() {
    nlm::TestRunner runner;
    
    // Add test suite
    runner.addSuite(std::make_unique<test_neuromodulation::NeuromodulationTestSuite>());
    
    // Run all tests
    auto results = runner.runAllTests();
    
    // Print results
    runner.printResults(results);
    
    // Exit with error code if any tests failed
    if (runner.getFailedTests() > 0) {
        return 1;
    }
    
    return 0;
}
