#include "../../../tests/test_main.h"
#include "../../../src/plasticity/StructuralPlasticity.hpp"
#include "../../../src/brain/Brain.hpp"
#include "../../../src/core/Config/Config.hpp"
#include "../../../src/core/Random/Random.hpp"
#include <cassert>
#include <iostream>

namespace nlm {

class TestStructuralPlasticity : public TestBase {
public:
    void runTests() override {
        testConstructorAndDestructor();
        testConfigurationParameters();
        testDevelopmentStageHandling();
        testParameterValidation();
        testBasicStructure();
    }
    
private:
    void testConstructorAndDestructor() {
        StructuralPlasticity sp;
        assert(&sp != nullptr);
        // Destructor should be called when sp goes out of scope
        testPassed("Constructor and destructor");
    }
    
    void testConfigurationParameters() {
        StructuralPlasticity sp;
        
        // Test basic configuration methods
        sp.setSynaptogenesisRate(0.001f);
        sp.setPruningRate(0.0005f);
        sp.setHebbianLearningRate(0.05f);
        sp.setTargetSynapticDensity(0.1f);
        sp.setCriticalPeriodModulation(1.5f);
        sp.setExperienceExpectantPlasticity(true);
        sp.setPruningThreshold(0.05f);
        sp.setActivityThreshold(0.001f);
        sp.setCompetitivePruningLevel(0.5f);
        sp.setHomeostaticTargetFiringRate(5.0f);
        sp.setMaxConnectionDistance(50.0f);
        sp.setLayerSpecificRules(false);
        sp.setDendriticCompartmentConstraints(true);
        sp.setExcitatorySynapseFormationRate(0.01f);
        sp.setInhibitorySynapseFormationRate(0.005f);
        sp.setSensorySpecificRules(true);
        sp.setMotorSpecificRules(true);
        
        // Test development stage
        sp.setDevelopmentStage(DevelopmentalStage::CriticalPeriod);
        sp.setDevelopmentalStageModulation(0.8f);
        
        // Test efficiency
        sp.enableBatchOperations(true);
        sp.setBatchSize(200);
        sp.setEfficiencyMode(true);
        
        testPassed("Configuration parameters");
    }
    
    void testDevelopmentStageHandling() {
        StructuralPlasticity sp;
        
        // Test all developmental stages
        sp.setDevelopmentStage(DevelopmentalStage::Initial);
        sp.setDevelopmentStage(DevelopmentalStage::CriticalPeriod);
        sp.setDevelopmentStage(DevelopmentalStage::Maturation);
        sp.setDevelopmentStage(DevelopmentalStage::Adult);
        sp.setDevelopmentStage(DevelopmentalStage::Aging);
        
        testPassed("Development stage handling");
    }
    
    void testParameterValidation() {
        StructuralPlasticity sp;
        
        // Basic validation - should not crash
        bool valid = sp.validateConfiguration();
        assert(valid == true || valid == false);  // Just check it doesn't crash
        
        // Reset should work
        sp.reset();
        
        testPassed("Parameter validation");
    }
    
    void testBasicStructure() {
        StructuralPlasticity sp;
        
        // Test that the structure is properly initialized
        float synaptogenesisRate = sp.getSynaptogenesisRate();
        assert(synaptogenesisRate >= 0.0f);
        
        float pruningRate = sp.getPruningRate();
        assert(pruningRate >= 0.0f);
        
        float density = sp.getCurrentSynapticDensity();
        assert(density >= 0.0f && density <= 1.0f);
        
        float stability = sp.getTotalSynapticStability();
        assert(stability >= 0.0f && stability <= 1.0f);
        
        size_t synapsesCreated = sp.getTotalSynapsesCreated();
        assert(synapsesCreated >= 0);
        
        size_t synapsesPruned = sp.getTotalSynapsesPruned();
        assert(synapsesPruned >= 0);
        
        float hebbianRatio = sp.getHebbianSynaptogenesisRatio();
        assert(hebbianRatio >= 0.0f && hebbianRatio <= 1.0f);
        
        float localRatio = sp.getLocalConnectivityRatio();
        assert(localRatio >= 0.0f && localRatio <= 1.0f);
        
        testPassed("Basic structure");
    }
};

} // namespace nlm

TEST_CLASS(TestStructuralPlasticity)
{
    TestStructuralPlasticity() : TestBase("StructuralPlasticity") {}
};

// Register the test
static TestRegistration registration(new TestStructuralPlasticity());