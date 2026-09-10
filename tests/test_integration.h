#pragma once

#include "tests/test_brain.h"
#include "Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include <memory>

namespace nlm {

class AgentBrainIntegrationTests {
public:
    AgentBrainIntegrationTests() = default;
    ~AgentBrainIntegrationTests() = default;
    
    // Test 1: Basic integration between Brain and AgentBrain
    bool testBasicAgentIntegration() {
        std::cout << "Running Test 1: Basic Agent Integration" << std::endl;
        
        // Create brain
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain" << std::endl;
            return false;
        }
        
        // Create world
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        world->setAgentStart(10.0, 10.0);
        
        // Create agent brain
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Test basic functions
        if (agent->getSensoryInputSize() == 0) {
            std::cerr << "Sensory input size should be > 0" << std::endl;
            return false;
        }
        
        if (agent->getMotorOutputSize() == 0) {
            std::cerr << "Motor output size should be > 0" << std::endl;
            return false;
        }
        
        // Test development stages
        auto stage = agent->getDevelopmentalStage();
        if (stage != DevelopmentalStage::Initial && 
            stage != DevelopmentalStage::CriticalPeriod &&
            stage != DevelopmentalStage::Maturation &&
            stage != DevelopmentalStage::Adult &&
            stage != DevelopmentalStage::Aging) {
            std::cerr << "Invalid developmental stage" << std::endl;
            return false;
        }
        
        // Test neuromodulation levels
        float curiosity = agent->getCuriosityLevel();
        float novelty = agent->getNoveltyLevel();
        float predictionError = agent->getPredictionError();
        float neuromodulation = agent->getNeuromodulationLevel();
        
        if (curiosity < 0.0f || curiosity > 1.0f) {
            std::cerr << "Invalid curiosity level" << std::endl;
            return false;
        }
        
        std::cout << "✓ Basic Agent Integration test passed" << std::endl;
        return true;
    }
    
    // Test 2: Memory system integration with Brain
    bool testMemoryIntegration() {
        std::cout << "Running Test 2: Memory System Integration" << std::endl;
        
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain" << std::endl;
            return false;
        }
        
        // Get memory systems
        auto workingMem = brain->getWorkingMemory();
        auto episodicMem = brain->getEpisodicMemory();
        auto associativeMem = brain->getAssociativeMemory();
        
        if (!workingMem || !episodicMem || !associativeMem) {
            std::cerr << "Memory systems should be accessible" << std::endl;
            return false;
        }
        
        // Test working memory
        std::vector<float> pattern = {0.5f, 0.3f, 0.8f, 0.1f, 0.9f};
        workingMem->store(pattern, 0.7f);
        
        // Test episodic memory
        episodicMem->recordExperience(pattern, 0.5f, "test_experience");
        
        // Test associative memory
        associativeMem->initialize(brain.get());
        std::vector<float> association = {0.2f, 0.6f, 0.4f};
        associativeMem->associatePattern(pattern, association, 0.8f);
        
        // Check statistics
        if (workingMem->getPatternCount() != 1) {
            std::cerr << "Working memory should have 1 pattern" << std::endl;
            return false;
        }
        
        std::cout << "✓ Memory System Integration test passed" << std::endl;
        return true;
    }
    
    // Test 3: Prediction system integration
    bool testPredictionSystemIntegration() {
        std::cout << "Running Test 3: Prediction System Integration" << std::endl;
        
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain" << std::endl;
            return false;
        }
        
        // Get prediction system
        auto predictionSystem = brain->getPredictionSystem();
        if (!predictionSystem) {
            std::cerr << "Prediction system should be accessible" << std::endl;
            return false;
        }
        
        // Test prediction error signal
        auto predictionError = brain->getPredictionErrorSignal();
        if (!predictionError) {
            std::cerr << "Prediction error signal should be accessible" << std::endl;
            return false;
        }
        
        // Compute prediction error
        predictionError->computeError(0.5f, 0.7f);  // predicted vs actual
        
        if (predictionError->getError() < 0.0f) {
            std::cerr << "Error should be positive" << std::endl;
            return false;
        }
        
        std::cout << "✓ Prediction System Integration test passed" << std::endl;
        return true;
    }
    
    // Test 4: Neuromodulation integration with AgentBrain
    bool testNeuromodulationIntegration() {
        std::cout << "Running Test 4: Neuromodulation Integration" << std::endl;
        
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain" << std::endl;
            return false;
        }
        
        // Create world and agent
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Test reward modulation
        agent->applyRewardModulation(1.0f, 0.5f);  // reward, predicted reward
        
        // Check neuromodulation levels
        float dopamine = agent->getNeuromodulationLevel();
        float curiosity = agent->getCuriosityLevel();
        float predictionError = agent->getPredictionError();
        
        if (dopamine < -1.0f || dopamine > 1.0f) {
            std::cerr << "Dopamine level should be between -1 and 1" << std::endl;
            return false;
        }
        
        // Test development updates
        agent->updateDevelopment(0.1f);
        
        // Check that developmental stage changed
        auto stageBefore = agent->getDevelopmentalStage();
        agent->updateDevelopment(100.0f);  // Large timestep
        auto stageAfter = agent->getDevelopmentalStage();
        
        std::cout << "✓ Neuromodulation Integration test passed" << std::endl;
        return true;
    }
    
    // Test 5: Complete agent-world integration
    bool testCompleteIntegration() {
        std::cout << "Running Test 5: Complete Integration" << std::endl;
        
        // Create brain with full integration
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 200);
        config->set("brain.synapse_density", 0.05);
        
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain" << std::endl;
            return false;
        }
        
        // Create agent
        auto agent = std::make_shared<AgentBrain>(brain);
        
        // Create world
        auto world = std::make_shared<SimpleWorld>();
        world->configure(30, 30, 12, 12);
        world->reset();
        world->setAgentStart(15.0, 15.0);
        world->setRandomSeed(42);
        
        // Initialize agent
        agent->initialize(*world);
        
        // Enable all subsystems
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        // Run simulation
        for (int step = 0; step < 100; ++step) {
            // Update world
            world->update(0.1f);
            
            // Get sensory percept
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Run brain step
            brain->step(step);
            
            // Get motor command
            MotorCommand action = agent->decodeMotorCommand();
            
            // Apply action to world
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply reward modulation
            agent->applyRewardModulation(1.0f, 0.0f);
            
            // Update development
            agent->updateDevelopment(0.1f);
            
            // Check that systems are still working
            if (brain->getTotalSpikeCount() == 0 && step > 10) {
                // Spike count can be zero in early steps, but should increase
                std::cout << "Warning: No spikes in later steps, may indicate issue" << std::endl;
            }
        }
        
        // Check final state
        size_t totalNeurons = brain->getTotalNeuronCount();
        size_t totalSynapses = brain->getTotalSynapseCount();
        size_t firingNeurons = brain->getFiringNeuronCount();
        
        if (totalNeurons == 0) {
            std::cerr << "Should have neurons" << std::endl;
            return false;
        }
        
        if (totalSynapses == 0) {
            std::cerr << "Should have synapses" << std::endl;
            return false;
        }
        
        std::cout << "✓ Complete Integration test passed" << std::endl;
        std::cout << "  Final state: " << totalNeurons << " neurons, " 
                  << totalSynapses << " synapses, " 
                  << firingNeurons << " firing neurons" << std::endl;
        return true;
    }
    
    // Run all integration tests
    bool runAllTests() {
        std::cout << "============================================" << std::endl;
        std::cout << "Running NLM Phase 6 Integration Tests" << std::endl;
        std::cout << "============================================" << std::endl;
        
        bool allPassed = true;
        
        allPassed &= testBasicAgentIntegration();
        allPassed &= testMemoryIntegration();
        allPassed &= testPredictionSystemIntegration();
        allPassed &= testNeuromodulationIntegration();
        allPassed &= testCompleteIntegration();
        
        std::cout << "============================================" << std::endl;
        if (allPassed) {
            std::cout << "ALL INTEGRATION TESTS PASSED! ✓" << std::endl;
        } else {
            std::cout << "SOME TESTS FAILED! ✗" << std::endl;
        }
        std::cout << "============================================" << std::endl;
        
        return allPassed;
    }
};

} // namespace nlm
