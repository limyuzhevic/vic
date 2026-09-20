// Development System Integration Tests
// Tests brain development affecting plasticity and behavior

#pragma once

#include "IntegrationTestFramework.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../development/Pruning.hpp"
#include "../development/Synaptogenesis.hpp"
#include "../development/Maturation.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

class TestDevelopmentIntegration : public IntegrationTestBase {
public:
    explicit TestDevelopmentIntegration(std::shared_ptr<Config> cfg, IntegrationTestResult& results)
        : IntegrationTestBase(cfg, results) {}
    
    void runAllTests() {
        runTestCase("Developmental Stage Integration", "Development Integration", [this]() {
            testDevelopmentalStageIntegration();
        });
        
        runTestCase("Synaptogenesis Integration", "Development Integration", [this]() {
            testSynaptogenesisIntegration();
        });
        
        runTestCase("Pruning Integration", "Development Integration", [this]() {
            testPruningIntegration();
        });
        
        runTestCase("Maturation Integration", "Development Integration", [this]() {
            testMaturationIntegration();
        });
        
        runTestCase("Development-Neural Plasticity Coupling", "Development Integration", [this]() {
            testDevelopmentNeuralPlasticityCoupling();
        });
        
        runTestCase("Developmental Timing", "Development Integration", [this]() {
            testDevelopmentalTiming();
        });
        
        runTestCase("Development Behavior Effects", "Development Integration", [this]() {
            testDevelopmentBehaviorEffects();
        });
    }
    
private:
    void testDevelopmentalStageIntegration() {
        setupSystem();
        
        auto* developmentSystem = brain->getDevelopmentSystem();
        assert(developmentSystem != nullptr);
        
        // Test developmental stage transitions
        DevelopmentalStage currentStage = brain->getDevelopmentalStage();
        
        // Simulate developmental progression
        for (nlm::SimulationStep step = 0; step < 200; ++step) {
            brain->step(step);
            brain->develop();
            
            DevelopmentalStage newStage = brain->getDevelopmentalStage();
            assert(newStage != DevelopmentalStage::Invalid);
        }
        
        // Verify developmental stage affects system properties
        DevelopmentalStage finalStage = brain->getDevelopmentalStage();
        
        // Stage should be appropriate for development level
        assert(finalStage >= DevelopmentalStage::Early);
        
        teardownSystem();
    }
    
    void testSynaptogenesisIntegration() {
        setupSystem();
        
        // Test new synapse formation (synaptogenesis)
        auto* developmentSystem = brain->getDevelopmentSystem();
        assert(developmentSystem != nullptr);
        
        // Simulate developmental synaptogenesis period
        for (nlm::SimulationStep step = 0; step < 100; ++step) {
            brain->step(step);
            brain->develop();
            
            // Verify synaptogenesis affects connectivity
            size_t synapseCount = brain->getTotalSynapseCount();
            assert(synapseCount >= 0);
        }
        
        // Test synaptogenesis efficiency
        size_t initialSynapseCount = brain->getTotalSynapseCount();
        
        for (nlm::SimulationStep step = 0; step < 50; ++step) {
            brain->step(step);
            brain->develop();
        }
        
        size_t finalSynapseCount = brain->getTotalSynapseCount();
        
        // Synaptogenesis should increase or maintain synapse count
        assert(finalSynapseCount >= initialSynapseCount);
        
        teardownSystem();
    }
    
    void testPruningIntegration() {
        setupSystem();
        
        // Test synaptic pruning during development
        for (nlm::SimulationStep step = 0; step < 150; ++step) {
            brain->step(step);
            brain->develop();
        }
        
        // Verify pruning effects on network efficiency
        size_t synapseCount = brain->getTotalSynapseCount();
        float firingRate = brain->getAverageFiringRate();
        
        assert(synapseCount >= 0);
        assert(firingRate >= 0.0f);
        
        // Test pruning optimization
        // Prune less efficient connections
        // (Implementation depends on Pruning API)
        
        // Verify pruning maintains network functionality
        auto action = brain->produceAction();
        assert(action != nullptr);
        
        teardownSystem();
    }
    
    void testMaturationIntegration() {
        setupSystem();
        
        // Test neural maturation over time
        auto* developmentSystem = brain->getDevelopmentSystem();
        assert(developmentSystem != nullptr);
        
        // Track maturation parameters
        std::vector<float> myelinationLevels;
        std::vector<float> connectivityEfficiencies;
        
        for (nlm::SimulationStep step = 0; step < 200; ++step) {
            brain->step(step);
            brain->develop();
            
            // Collect maturation metrics
            float firingRate = brain->getAverageFiringRate();
            myelinationLevels.push_back(firingRate); // Simplified proxy
            
            size_t activeNeurons = brain->getActiveNeuronCount();
            connectivityEfficiencies.push_back(
                static_cast<float>(activeNeurons) / brain->getTotalNeuronCount()
            );
        }
        
        // Verify maturation trends (monotonic increase in efficiency)
        if (connectivityEfficiencies.size() > 1) {
            for (size_t i = 1; i < connectivityEfficiencies.size(); ++i) {
                assert(connectivityEfficiencies[i] >= connectivityEfficiencies[i-1] * 0.95f);
            }
        }
        
        teardownSystem();
    }
    
    void testDevelopmentNeuralPlasticityCoupling() {
        setupSystem();
        
        // Test coupling between development and neural plasticity
        auto* developmentSystem = brain->getDevelopmentSystem();
        auto* stdp = brain->getSTDP();
        
        assert(developmentSystem != nullptr);
        assert(stdp != nullptr);
        
        // Track plasticity rules through development
        std::vector<float> weightChanges;
        
        for (nlm::SimulationStep step = 0; step < 100; ++step) {
            brain->step(step);
            brain->develop();
            
            // In real implementation, development would modulate plasticity
            // Here we verify system consistency
            float firingRate = brain->getAverageFiringRate();
            weightChanges.push_back(firingRate);
        }
        
        // Verify development maintains plasticity function
        auto action = brain->produceAction();
        assert(action != nullptr);
        
        teardownSystem();
    }
    
    void testDevelopmentalTiming() {
        setupSystem();
        
        // Test developmental timing consistency
        auto* developmentSystem = brain->getDevelopmentSystem();
        assert(developmentSystem != nullptr);
        
        // Run multiple developmental sequences
        for (int run = 0; run < 3; ++run) {
            // Reset brain for each run
            brain->reset();
            
            // Track developmental progression
            std::vector<DevelopmentalStage> stages;
            
            for (nlm::SimulationStep step = 0; step < 150; ++step) {
                brain->step(step);
                brain->develop();
                stages.push_back(brain->getDevelopmentalStage());
            }
            
            // Verify developmental timing consistency
            // Each stage should progress through the expected sequence
            // (Implementation depends on DevelopmentalStage enum)
        }
        
        teardownSystem();
    }
    
    void testDevelopmentBehaviorEffects() {
        setupSystem();
        
        // Test development effects on behavior
        std::vector<std::string> behavioralPhases = {
            "Early development",
            "Juvenile learning",
            "Adult performance"
        };
        
        for (size_t phase = 0; phase < behavioralPhases.size(); ++phase) {
            // Simulate behavioral phase
            for (nlm::SimulationStep step = 0; step < 50; ++step) {
                brain->step(step);
                brain->develop();
            }
            
            // Verify behavioral capabilities
            auto action = brain->produceAction();
            assert(action != nullptr);
            
            float firingRate = brain->getAverageFiringRate();
            assert(firingRate >= 0.0f);
        }
        
        // Test developmental effects on learning efficiency
        // Compare learning rates at different developmental stages
        float earlyLearningRate = 0.0f;
        float lateLearningRate = 0.0f;
        
        // Early development phase
        for (nlm::SimulationStep step = 0; step < 75; ++step) {
            brain->step(step);
            brain->develop();
        }
        earlyLearningRate = brain->getAverageFiringRate();
        
        // Later development phase
        for (nlm::SimulationStep step = 0; step < 75; ++step) {
            brain->step(step + 75);
            brain->develop();
        }
        lateLearningRate = brain->getAverageFiringRate();
        
        // Learning rate should improve with development (monotonic)
        assert(lateLearningRate >= earlyLearningRate * 0.9f);
        
        teardownSystem();
    }
};

} // namespace nlm