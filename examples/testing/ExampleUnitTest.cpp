// NLM Testing Framework
// Unit Testing Example

#include "framework/TestFramework.hpp"
#include "framework/TestFixtures.hpp"
#include <cassert>

class ExampleUnitTest : public TestCase {
public:
    ExampleUnitTest() : TestCase("ExampleUnitTest") {}
    
    void runTest() override {
        // Example: Test basic brain functionality
        TestFramework framework;
        BrainSystemFixture fixture(framework, "BrainSystem");
        
        Brain& brain = fixture.getBrain();
        
        // Test 1: Basic brain initialization
        assert(brain.getRegionCount() > 0);
        assert(brain.getNeuronCount() > 0);
        assert(brain.getSynapseCount() > 0);
        
        // Test 2: Neuron dynamics
        std::vector<double> sensory_input = {0.8, 0.3, 0.6, 0.2, 0.9};
        std::vector<double> motor_output = brain.processSensoryInput(sensory_input);
        
        assert(motor_output.size() == sensory_input.size());
        assert(std::all_of(motor_output.begin(), motor_output.end(),
                          [](double v) { return v >= 0.0 && v <= 1.0; }));
        
        // Test 3: Memory operations
        MemorySystem& memory = fixture.getMemorySystem();
        
        std::vector<double> data = {1.0, 2.0, 3.0, 4.0, 5.0};
        memory.storePattern(data);
        
        std::vector<double> retrieved = memory.retrievePattern();
        assert(retrieved == data);
        
        // Test 4: Neuromodulation
        NeuromodulationSystem& neuromod = fixture.getNeuromodulation();
        
        double reward_signal = neuromod.processReward(0.8);
        assert(reward_signal > 0.0);
        
        // Test 5: Plasticity
        brain.applyPlasticityRules();
        assert(true); // Stub for actual plasticity test
        
        // Test 6: Cognitive functions
        std::string decision = brain.makeDecision();
        assert(!decision.empty());
        
        // Test 7: Integration with other systems
        double attention_level = brain.getAttentionLevel();
        assert(attention_level >= 0.0 && attention_level <= 1.0);
    }
};

// Example usage
void example_unit_test_usage() {
    TestFramework framework;
    ExampleUnitTest test;
    
    framework.registerTest(test);
    framework.executeTest(test);
    framework.reportResults();
}