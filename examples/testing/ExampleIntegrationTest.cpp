// NLM Testing Framework
// Integration Testing Example

#include "framework/TestFramework.hpp"
#include "framework/TestFixtures.hpp"
#include <cassert>

class ExampleIntegrationTest : public TestCase {
public:
    ExampleIntegrationTest() : TestCase("ExampleIntegrationTest") {}
    
    void runTest() override {
        // Example: Test system-wide integration
        TestFramework framework;
        
        // Create complete system with all components
        BrainSystemFixture brain_fixture(framework, "CompleteSystem");
        NeuromodulationFixture neuromod_fixture(framework, "Neuromodulation");
        
        Brain& brain = brain_fixture.getBrain();
        NeuromodulationSystem& neuromod = neuromod_fixture.getNeuromodulation();
        
        // Test 1: End-to-end sensory-motor integration
        std::vector<double> sensory_data = {0.7, 0.4, 0.8, 0.2, 0.6, 0.3, 0.9, 0.1, 0.5, 0.7};
        std::vector<double> processed_sensory = brain.processSensoryInput(sensory_data);
        
        std::vector<double> cognitive_state = brain.processCognitiveData(processed_sensory);
        std::vector<double> motor_output = brain.processMotorOutput(cognitive_state);
        
        assert(processed_sensory.size() == sensory_data.size());
        assert(motor_output.size() > 0);
        
        // Test 2: Neuromodulation integration
        double reward = neuromod.processReward(0.9);
        double attention = neuromod.processAttention(0.8);
        double stress = neuromod.processStress(0.7);
        
        assert(reward > 0.0);
        assert(attention > 0.0);
        assert(stress >= 0.0);
        
        // Test 3: Memory integration
        MemorySystem& memory = brain_fixture.getMemorySystem();
        
        std::vector<double> learning_data = {0.1, 0.2, 0.3, 0.4, 0.5};
        memory.storePattern(learning_data);
        
        std::vector<double> recalled_data = memory.retrievePattern();
        assert(recalled_data == learning_data);
        
        // Test 4: Plasticity integration
        brain.applyPlasticityRules();
        neuromod.updateNeuromodulators();
        
        // Test 5: Cognitive integration
        std::string decision = brain.makeDecision();
        assert(!decision.empty());
        
        double attention_level = brain.getAttentionLevel();
        double memory_strength = memory.getMemoryStrength();
        
        assert(attention_level >= 0.0 && attention_level <= 1.0);
        assert(memory_strength >= 0.0 && memory_strength <= 1.0);
        
        // Test 6: Performance integration
        PerformanceProfiler profiler;
        profiler.startProfile("integration_test");
        
        // Simulate complex integration
        std::vector<double> complex_input = brain_fixture.getSensorData();
        std::vector<double> complex_output = neuromod.processSignals(complex_input);
        
        profiler.endProfile();
        
        auto metrics = profiler.getMetrics("integration_test");
        assert(metrics.duration.count() > 0);
        assert(std::isfinite(metrics.gflops));
    }
};

// Example usage
void example_integration_test_usage() {
    TestFramework framework;
    ExampleIntegrationTest test;
    
    framework.registerTest(test);
    framework.executeTest(test);
    framework.reportResults();
}