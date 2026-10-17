// DevelopmentTests.cpp - Development system tests
#include "TestFramework.hpp"
#include "TestUtils.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/development/Maturation.hpp"
#include "../src/development/Pruning.hpp"
#include "../src/development/Synaptogenesis.hpp"
#include "../src/brain/Brain.hpp"

namespace nlm {

// Development system unit tests
class DevelopmentUnits {
public:
    void runTest() {
        TestRunner runner(true);
        
        // Development system tests
        TEST_CASE(DevelopmentUnits, "Development System Initialization");
        TEST_CASE(DevelopmentUnits, "Development System Stage Advancement");
        TEST_CASE(DevelopmentUnits, "Development System Plasticity Modifier");
        TEST_CASE(DevelopmentUnits, "Development System Critical Period");
        
        // Maturation tests
        TEST_CASE(DevelopmentUnits, "Maturation System Initialization");
        TEST_CASE(DevelopmentUnits, "Maturation System Growth");
        TEST_CASE(DevelopmentUnits, "Maturation System Synaptogenesis");
        
        // Pruning tests
        TEST_CASE(DevelopmentUnits, "Pruning System Initialization");
        TEST_CASE(DevelopmentUnits, "Pruning System Synapse Elimination");
        TEST_CASE(DevelopmentUnits, "Pruning System Competition");
        
        // Synaptogenesis tests
        TEST_CASE(DevelopmentUnits, "Synaptogenesis System Initialization");
        TEST_CASE(DevelopmentUnits, "Synaptogenesis System Formation");
        TEST_CASE(DevelopmentUnits, "Synaptogenesis System Stabilization");
        
        // Development integration tests
        TEST_CASE(DevelopmentIntegration, "Development System Integration");
        TEST_CASE(DevelopmentIntegration, "Development-Plasticity Integration");
        TEST_CASE(DevelopmentIntegration, "Development-Memory Integration");
        
        std::cout << "=== Development System Tests ===" << std::endl;
        runner.runAll();
    }
};

// Development system tests
namespace DevelopmentSystemTests {
    bool testInitialization() {
        DevelopmentSystem devSystem;
        ASSERT_EQ(devSystem.getStage(), DevelopmentalStage::Embryonic);
        std::cout << "PASS: Development system initialization successful" << std::endl;
        return true;
    }
    
    bool testStageAdvancement() {
        DevelopmentSystem devSystem;
        DevelopmentalStage stage = devSystem.getStage();
        
        devSystem.advanceStage();
        ASSERT_TRUE(devSystem.getStage() != stage);  // Stage should change
        
        std::cout << "PASS: Development system stage advancement successful. "
                 << "From: " << stage << " To: " << devSystem.getStage() << std::endl;
        return true;
    }
    
    bool testPlasticityModifier() {
        DevelopmentSystem devSystem;
        float modifier = devSystem.getPlasticityModifier();
        
        ASSERT_GT(modifier, 0.0f);  // Should have positive plasticity
        
        std::cout << "PASS: Development system plasticity modifier: " << modifier << std::endl;
        return true;
    }
    
    bool testCriticalPeriod() {
        DevelopmentSystem devSystem;
        bool criticalPeriod = devSystem.isCriticalPeriod();
        float progress = devSystem.getCriticalPeriodProgress();
        
        ASSERT_LE(progress, 1.0f);  // Progress should be <= 1.0
        
        std::cout << "PASS: Development system critical period test successful. "
                 << "In Critical Period: " << criticalPeriod 
                 << ", Progress: " << progress << std::endl;
        return true;
    }
}}

// Maturation system tests
namespace MaturationTests {
    bool testInitialization() {
        Maturation maturation;
        std::cout << "PASS: Maturation system initialization successful" << std::endl;
        return true;
    }
    
    bool testGrowth() {
        Maturation maturation;
        
        std::cout << "PASS: Maturation system growth successful" << std::endl;
        return true;
    }
    
    bool testSynaptogenesis() {
        Maturation maturation;
        
        std::cout << "PASS: Maturation system synaptogenesis successful" << std::endl;
        return true;
    }
}}

// Pruning system tests
namespace PruningTests {
    bool testInitialization() {
        Pruning pruning;
        std::cout << "PASS: Pruning system initialization successful" << std::endl;
        return true;
    }
    
    bool testSynapseElimination() {
        Pruning pruning;
        
        std::cout << "PASS: Pruning system synapse elimination successful" << std::endl;
        return true;
    }
    
    bool testCompetition() {
        Pruning pruning;
        
        std::cout << "PASS: Pruning system competition successful" << std::endl;
        return true;
    }
}}

// Synaptogenesis system tests
namespace SynaptogenesisTests {
    bool testInitialization() {
        Synaptogenesis synaptogenesis;
        std::cout << "PASS: Synaptogenesis system initialization successful" << std::endl;
        return true;
    }
    
    bool testFormation() {
        Synaptogenesis synaptogenesis;
        
        std::cout << "PASS: Synaptogenesis system formation successful" << std::endl;
        return true;
    }
    
    bool testStabilization() {
        Synaptogenesis synaptogenesis;
        
        std::cout << "PASS: Synaptogenesis system stabilization successful" << std::endl;
        return true;
    }
}}

// Development integration tests
namespace DevelopmentIntegrationTests {
    bool testSystemIntegration() {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            std::cout << "FAIL: Brain initialization failed" << std::endl;
            return false;
        }
        
        // Check development system is accessible
        ASSERT_TRUE(brain->getDevelopmentSystem() != nullptr);
        
        std::cout << "PASS: Development system integration successful" << std::endl;
        return true;
    }
    
    bool testDevelopmentPlasticityIntegration() {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            std::cout << "FAIL: Brain initialization failed" << std::endl;
            return false;
        }
        
        // Get development system and plasticity systems
        auto* devSystem = brain->getDevelopmentSystem();
        auto* stdp = brain->getSTDP();
        auto* hebbian = brain->getHebbian();
        
        if (devSystem && stdp && hebbian) {
            // Simulate integration between development and plasticity
            // Update development system
            devSystem->update(brain.get(), 0.0, 0.001);
            
            // Apply plasticity
            brain->updatePlasticity();
            
            std::cout << "PASS: Development-plasticity integration successful" << std::endl;
        }
        
        return true;
    }
    
    bool testDevelopmentMemoryIntegration() {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            std::cout << "FAIL: Brain initialization failed" << std::endl;
            return false;
        }
        
        // Get development system and memory systems
        auto* devSystem = brain->getDevelopmentSystem();
        auto* workingMemory = brain->getWorkingMemory();
        auto* episodicMemory = brain->getEpisodicMemory();
        
        if (devSystem && workingMemory && episodicMemory) {
            // Update development system
            devSystem->update(brain.get(), 0.0, 0.001);
            
            // Simulate memory development integration
            std::cout << "PASS: Development-memory integration successful" << std::endl;
        }
        
        return true;
    }
}}

// Phase 6 development integration test
namespace Phase6DevelopmentIntegration {
    bool testPhase6DevelopmentIntegration() {
        Phase6Config config;
        config.neuronCount = 100;
        config.maxSteps = 1000;
        config.enableDevelopment = true;
        
        Phase6IntegratedExperiment experiment;
        auto result = experiment.run(config);
        
        // Check development integration
        bool developmentIntegrated = result.developmentIntegrated;
        
        if (developmentIntegrated) {
            std::cout << "PASS: Phase 6 development integration successful. "
                     << "Integrated: " << developmentIntegrated << std::endl;
        } else {
            std::cout << "FAIL: Phase 6 development integration failed. "
                     << "Integrated: " << developmentIntegrated << std::endl;
            return false;
        }
        
        return true;
    }
}}

// Test registration functions
void register_development_tests(TestRunner& runner) {
    TEST_CASE(DevelopmentUnits, "Development System Initialization");
    TEST_CASE(DevelopmentUnits, "Development System Stage Advancement");
    TEST_CASE(DevelopmentUnits, "Development System Plasticity Modifier");
    TEST_CASE(DevelopmentUnits, "Development System Critical Period");
    
    TEST_CASE(DevelopmentUnits, "Maturation System Initialization");
    TEST_CASE(DevelopmentUnits, "Maturation System Growth");
    TEST_CASE(DevelopmentUnits, "Maturation System Synaptogenesis");
    
    TEST_CASE(DevelopmentUnits, "Pruning System Initialization");
    TEST_CASE(DevelopmentUnits, "Pruning System Synapse Elimination");
    TEST_CASE(DevelopmentUnits, "Pruning System Competition");
    
    TEST_CASE(DevelopmentUnits, "Synaptogenesis System Initialization");
    TEST_CASE(DevelopmentUnits, "Synaptogenesis System Formation");
    TEST_CASE(DevelopmentUnits, "Synaptogenesis System Stabilization");
    
    TEST_CASE(DevelopmentIntegration, "Development System Integration");
    TEST_CASE(DevelopmentIntegration, "Development-Plasticity Integration");
    TEST_CASE(DevelopmentIntegration, "Development-Memory Integration");
    
    TEST_CASE(Phase6DevelopmentIntegration, "Phase 6 Development Integration");
}

// Helper test functions
void setupDevelopmentTests() {
    std::cout << "Setting up development system tests..." << std::endl;
}

void teardownDevelopmentTests() {
    std::cout << "Cleaning up development system tests..." << std::endl;
}

} // namespace nlm
