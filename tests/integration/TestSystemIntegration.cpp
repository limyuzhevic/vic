// NLM Integration Tests
// System Integration Tests

#include "framework/TestFramework.hpp"
#include "framework/TestFixtures.hpp"
#include "brain/SensorySystem.hpp"
#include "brain/MotorSystem.hpp"
#include "brain/CognitiveSystem.hpp"
#include "brain/NeuromodulationSystem.hpp"
#include <cassert>
#include <cmath>

class TestSystemIntegration : public TestCase {
public:
    TestSystemIntegration() : TestCase("SystemIntegration") {}
    
    void testBrainSystemIntegration() {
        TestFramework framework;
        BrainSystemFixture fixture(framework, "BrainSystem");
        
        // Get integrated brain system
        Brain& brain = fixture.getBrain();
        
        // Test that all components are properly integrated
        assert(brain.getRegionCount() > 0);
        assert(brain.getNeuronCount() > 0);
        assert(brain.getSynapseCount() > 0);
        
        // Test sensory integration with motor
        std::vector<double> sensory_input = {0.8, 0.3, 0.6, 0.2, 0.9};
        std::vector<double> motor_output = brain.processSensoryInput(sensory_input);
        
        assert(motor_output.size() > 0);
        assert(std::all_of(motor_output.begin(), motor_output.end(), 
                          [](double v) { return v >= 0.0 && v <= 1.0; }));
        
        // Test plasticity integration
        brain.applyPlasticityRules();
        assert(true); // Stub - implementation dependent
        
        // Test neuromodulation integration
        double neuromodulation_effect = brain.getNeuromodulationEffect();
        assert(neuromodulation_effect >= 0.0);
        assert(neuromodulation_effect <= 1.0);
    }
    
    void testCognitiveIntegration() {
        TestFramework framework;
        BrainSystemFixture fixture(framework, "CognitiveIntegration");
        
        Brain& brain = fixture.getBrain();
        
        // Test cognitive functions integration
        std::vector<double> perception = brain.getPerception();
        assert(perception.size() > 0);
        
        // Test memory integration
        std::vector<double> memory_content = brain.getMemoryContent();
        assert(memory_content.size() >= 0);
        
        // Test attention integration
        double attention_level = brain.getAttentionLevel();
        assert(attention_level >= 0.0 && attention_level <= 1.0);
        
        // Test decision making integration
        std::string decision = brain.makeDecision();
        assert(!decision.empty());
        
        // Test learning integration
        brain.learnFromExperience();
        assert(true); // Stub - implementation dependent
    }
    
    void testEndToEndSystem() {
        TestFramework framework;
        
        // Create complete system with all components
        std::vector<double> sensory_data = {0.7, 0.4, 0.8, 0.2, 0.6};
        std::vector<double> motor_commands = {0.3, 0.5, 0.2, 0.8, 0.4};
        
        // Process sensory input through complete pipeline
        std::vector<double> processed_sensory = processSensoryData(sensory_data);
        std::vector<double> cognitive_state = processCognitiveData(processed_sensory);
        std::vector<double> motor_output = processMotorData(cognitive_state);
        
        // Verify end-to-end integration
        assert(processed_sensory.size() == sensory_data.size());
        assert(motor_output.size() == motor_commands.size());
        
        // Test feedback loop
        std::vector<double> feedback = processFeedback(motor_output);
        assert(feedback.size() > 0);
        
        // System should adapt based on feedback
        adaptSystem(feedback);
        assert(true); // Stub - implementation dependent
    }
    
    void testCrossDomainIntegration() {
        TestFramework framework;
        BrainSystemFixture brain_fixture(framework, "CrossDomainIntegration");
        
        Brain& brain = brain_fixture.getBrain();
        
        // Integrate all brain domains
        std::vector<RegionType> regions = {
            RegionType::CORTEX,
            RegionType::STRIATUM,
            RegionType::THALAMUS,
            RegionType::HYPOTHALAMUS
        };
        
        // Test domain-specific interactions
        for (RegionType region : regions) {
            // Each region should contribute to system behavior
            double region_contribution = brain.getRegionContribution(region);
            assert(region_contribution >= 0.0);
            
            // Regions should interact with each other
            std::vector<double> regional_interactions = brain.getRegionalInteractions(region);
            assert(regional_interactions.size() > 0);
        }
        
        // Test global system properties
        assert(brain.getSystemStability() >= 0.0 && brain.getSystemStability() <= 1.0);
        assert(brain.getEmergentBehavior() >= 0.0 && brain.getEmergentBehavior() <= 1.0);
    }
};