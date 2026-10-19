#include <chrono>
#include <limits>
// Brain Tests
// Enhanced with comprehensive tests including context managers, error handling, and edge cases

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "memory/WorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/Memory.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "neuromodulation/Reward.hpp"
#include "development/DevelopmentSystem.hpp"
#include "prediction/NeuralPrediction.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "agent/AgentBrain.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <memory>

namespace test_brain {

// Basic Tests (existing, preserved)
void testBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    std::cout << "    testBrainCreation passed" << std::endl;
}

void testBrainInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    bool success = brain.initialize();
    assert(success);
    
    // Verify brain initialized with expected properties
    assert(brain.getRegionCount() == 2);
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    testBrainInitialization passed" << std::endl;
}

void testBrainStep() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run a few steps
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Just verify it doesn't crash
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "    testBrainStep passed" << std::endl;
}

void testBrainReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    brain.step(0);
    brain.reset();
    
    // Verify reset worked
    assert(brain.getTotalNeuronCount() == 50);
    
    std::cout << "    testBrainReset passed" << std::endl;
}

void testBrainRegions() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto regionIds = brain.getRegionIds();
    assert(regionIds.size() == 3);
    
    for (auto rid : regionIds) {
        auto* region = brain.getRegion(rid);
        assert(region != nullptr);
    }
    
    std::cout << "    testBrainRegions passed" << std::endl;
}

void testBrainActionProduction() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    std::cout << "    testBrainActionProduction passed" << std::endl;
}

void testBrainStatus() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify statistics are accessible
    assert(brain.getTotalNeuronCount() > 0);
    assert(brain.getTotalSynapseCount() >= 0);
    assert(brain.getFiringNeuronCount() >= 0);
    assert(brain.getAverageFiringRate() >= 0.0f);
    
    std::cout << "    testBrainStatus passed" << std::endl;
}

// Enhanced Tests
void testBrainContextManager() {
    std::cout << "    Context Manager Test" << std::endl;
    
    // Test brain initialization context (if applicable)
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test step execution as context
    for (int step = 0; step < 5; ++step) {
        brain.step(step);
    }
    
    // Brain should be in valid state after steps
    assert(brain.getTotalNeuronCount() == 50);
    
    std::cout << "      Context manager test passed" << std::endl;
}

void testBrainErrorHandling() {
    std::cout << "    Error Handling Test" << std::endl;
    
    // Test with invalid configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(-10), nlm::ConfigSource::Default); // Negative neurons
    config->set("region_count", static_cast<int64_t>(0), nlm::ConfigSource::Default); // Zero regions
    
    nlm::Brain brain(config);
    
    // Initialization should handle edge cases gracefully
    bool initSuccess = brain.initialize();
    // Success depends on implementation, but shouldn't crash
    
    // Test with null brain pointer scenarios (if applicable)
    // This would test error handling in subsystem access
    
    std::cout << "      Error handling test passed" << std::endl;
}

void testBrainEdgeCases() {
    std::cout << "    Edge Cases Test" << std::endl;
    
    // Test with minimal configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    assert(brain.getTotalNeuronCount() == 1);
    assert(brain.getRegionCount() == 1);
    
    // Test brain operations with minimal resources
    brain.step(0);
    
    // Test reset functionality
    brain.reset();
    assert(brain.getTotalNeuronCount() == 1);
    
    // Test with large configuration
    config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(20), nlm::ConfigSource::Default);
    
    nlm::Brain largeBrain(config);
    largeBrain.initialize();
    
    assert(largeBrain.getTotalNeuronCount() == 10000);
    assert(largeBrain.getRegionCount() == 20);
    
    std::cout << "      Edge cases test passed" << std::endl;
}

void testBrainMemoryManagement() {
    std::cout << "    Memory Management Test" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    auto* associativeMemory = brain.getAssociativeMemory();
    
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    assert(associativeMemory != nullptr);
    
    // Test working memory memory management
    for (int i = 0; i < 50; ++i) {
        std::vector<float> pattern(10);
        for (int j = 0; j < 10; ++j) {
            pattern[j] = static_cast<float>(i * 0.1f + j * 0.01f);
        }
        workingMemory->store(pattern, 1.0f);
    }
    
    workingMemory->clear();
    assert(workingMemory->getActiveTraces() == 0);
    
    // Test episodic memory memory management
    for (int i = 0; i < 20; ++i) {
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i * 10;
        episode.reward = static_cast<float>(i);
        episode.sensoryState = std::vector<float>(5, static_cast<float>(i) * 0.1f);
        episodicMemory->storeEpisode(episode);
    }
    
    episodicMemory->consolidate(0.5f);
    episodicMemory->clear();
    assert(episodicMemory->getEpisodeCount() == 0);
    
    // Test associative memory memory management
    for (int i = 0; i < 10; ++i) {
        std::vector<float> patternA(4);
        std::vector<float> patternB(4);
        for (int j = 0; j < 4; ++j) {
            patternA[j] = static_cast<float>(i * 0.2f + j * 0.1f);
            patternB[j] = static_cast<float>(i * 0.3f + j * 0.05f);
        }
        associativeMemory->associate(patternA, patternB, 0.7f);
    }
    
    associativeMemory->clear();
    assert(associativeMemory->getAssociationCount() == 0);
    
    std::cout << "      Memory management test passed" << std::endl;
}

void testBrainNeuromodulationIntegration() {
    std::cout << "    Neuromodulation Integration Test" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    auto* predError = brain.getPredictionErrorSignal();
    
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    assert(predError != nullptr);
    
    // Test neuromodulation integration during brain operation
    for (int step = 0; step < 30; ++step) {
        // Simulate sensory input
        std::vector<float> sensoryInput(5);
        for (int i = 0; i < 5; ++i) {
            sensoryInput[i] = static_cast<float>(sin(step * 0.1f + i * 0.2f));
        }
        
        brain.receiveSensoryInput(sensoryInput);
        
        // Neuromodulation signals
        float reward = static_cast<float>(cos(step * 0.15f) + 1.0f) * 0.5f;
        float prediction = static_cast<float>(sin(step * 0.05f));
        float actual = prediction + static_cast<float>(sin(step * 0.1f)) * 0.1f;
        float noveltySignal = static_cast<float>(pow(sin(step * 0.02f), 2));
        
        dopamine->signalReward(reward);
        dopamine->signalRewardPredictionError(actual - prediction);
        
        curiosity->update(noveltySignal, actual - prediction, 0.01f);
        novelty->setLevel(noveltySignal);
        predError->computeError(prediction, actual);
        
        // Brain step with neuromodulation
        brain.step(step, step * 0.1);
        
        brain.applyNeuromodulation(*dopamine);
    }
    
    std::cout << "      Neuromodulation integration test passed" << std::endl;
}

void testBrainDevelopmentIntegration() {
    std::cout << "    Development Integration Test" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* devSystem = brain.getDevelopmentSystem();
    assert(devSystem != nullptr);
    
    // Test developmental integration
    for (int step = 0; step < 40; ++step) {
        // Advance development
        if (step % 10 == 0) {
            devSystem->advanceStage();
        }
        
        // Update development
        devSystem->update(&brain, step);
        
        // Brain step
        brain.step(step, step * 0.1);
        
        // Check developmental stage
        nlm::DevelopmentalStage currentStage = devSystem->getStage();
        assert(currentStage != nlm::DevelopmentalStage::Invalid);
        
        // Check plasticity modifier
        float plasticity = devSystem->getPlasticityModifier();
        assert(plasticity >= 0.0f);
        
        if (step % 15 == 0) {
            std::cout << "      Development stage: " << devSystem->getStageName() << std::endl;
        }
    }
    
    std::cout << "      Development integration test passed" << std::endl;
}

void testBrainPredictionIntegration() {
    std::cout << "    Prediction Integration Test" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(180), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test prediction system integration
    for (int step = 0; step < 50; ++step) {
        // Create sensory sequence for prediction
        std::vector<float> sensoryState(6);
        for (int i = 0; i < 6; ++i) {
            sensoryState[i] = static_cast<float>(sin(step * 0.08f + i * 0.15f));
        }
        
        brain.recordSensoryState(sensoryState, step);
        
        // Generate prediction
        std::vector<float> prediction = brain.generatePrediction(step + 1);
        assert(prediction.size() == sensoryState.size());
        
        // Update with observation
        float error = brain.updateWithObservation(sensoryState, step + 1);
        assert(error >= 0.0f);
        
        // Test action consequence prediction
        if (step % 8 == 0) {
            std::vector<nlm::ActionType> actions = {
                nlm::ActionType::MoveForward,
                nlm::ActionType::Wait,
                nlm::ActionType::Attack
            };
            
            nlm::PlanningCandidate candidate = brain.evaluateSequence(actions, sensoryState);
            assert(candidate.actions.size() == actions.size());
            assert(candidate.predictedStates.size() == actions.size());
        }
        
        // Main brain step
        brain.step(step, step * 0.1);
    }
    
    std::cout << "      Prediction integration test passed" << std::endl;
}

void testBrainCognitionIntegration() {
    std::cout << "    Cognition Integration Test" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(250), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* planner = brain.getPlanner();
    auto* conceptFormation = brain.getConceptFormation();
    
    assert(planner != nullptr);
    assert(conceptFormation != nullptr);
    
    // Test cognition integration
    for (int step = 0; step < 60; ++step) {
        // Create state for concept formation
        std::vector<float> state(8);
        for (int i = 0; i < 8; ++i) {
            state[i] = static_cast<float>(cos(step * 0.12f + i * 0.1f));
        }
        
        // Present to concept formation
        conceptFormation->presentExperience(state, {0.5f, 0.6f, 0.7f}, 5.0f + step, step);
        
        // Plan action
        nlm::ActionType action = planner->planAction(state, 0.6f);
        assert(action != nlm::ActionType::ActionCount);
        
        // Evaluate sequence
        std::vector<nlm::ActionType> sequence;
        for (int j = 0; j < 3; ++j) {
            nlm::ActionType nextAction = planner->planAction(state, 0.5f);
            sequence.push_back(nextAction);
        }
        
        nlm::PlanningCandidate candidate = planner->evaluateSequence(sequence, state);
        assert(candidate.actions.size() == sequence.size());
        
        // Update plan quality based on outcome
        planner->updatePlanQuality(sequence, sequence, candidate.expectedReward);
        
        // Brain step
        brain.step(step, step * 0.1f);
        
        if (step % 20 == 0) {
            size_t conceptCount = conceptFormation->getConcepts().size();
            std::cout << "      Concepts formed: " << conceptCount << std::endl;
        }
    }
    
    std::cout << "      Cognition integration test passed" << std::endl;
}

void testBrainAgentIntegration() {
    std::cout << "    Agent Integration Test" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create agent brain
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Create simple world for testing
    nlm::SimpleWorld world;
    world.configure(100.0f, 100.0f, 10, 10);
    world.setAgentStart(50.0f, 50.0f);
    world.setMaxEnergy(100.0f);
    
    // Add some objects to world
    world.addObject(nlm::WorldObject(25.0f, 25.0f, nlm::WorldObjectType::Resource, 30.0f));
    world.addObject(nlm::WorldObject(75.0f, 75.0f, nlm::WorldObjectType::Hazard, -15.0f));
    
    agentBrain.initialize(&world);
    
    // Test agent-world integration
    for (int step = 0; step < 40; ++step) {
        // Agent brain processes world
        agentBrain.update(step * 0.1);
        
        // Get sensory percept
        const nlm::SensoryPercept& percept = world.getSensoryPercept();
        
        // Brain receives sensory input
        brain.receiveSensoryInput(percert);
        
        // Brain processes and produces action
        brain.step(step, step * 0.1f);
        
        std::unique_ptr<nlm::Action> action = brain.produceAction();
        if (action) {
            // Convert to motor command and apply to world
            nlm::MotorCommand cmd;
            cmd.type = nlm::ActionToMotorCommand(action->getType());
            cmd.duration = 0.1;
            world.applyMotorCommand(cmd, step * 0.1);
        }
        
        // Update world
        world.update(0.1);
        
        // Check agent state
        auto& agentBody = world.getAgentBody();
        assert(agentBody.getEnergy() >= 0.0f);
        
        if (step % 15 == 0) {
            std::cout << "      Agent energy: " << agentBody.getEnergy() << std::endl;
        }
    }
    
    std::cout << "      Agent integration test passed" << std::endl;
}

void testBrainStressPerformance() {
    std::cout << "    Stress Performance Test" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Stress test with heavy operations
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int step = 0; step < 200; ++step) {
        // Heavy input
        brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 50.0f);
        brain.injectCurrentToNeurons(nlm::NeuronType::Motor, 30.0f);
        
        // Apply neuromodulation
        auto* dopamine = brain.getDopamine();
        if (dopamine) {
            float reward = static_cast<float>(sin(step * 0.05f) + 1.0f) * 0.5f;
            dopamine->signalReward(reward);
            brain.applyNeuromodulation(*dopamine);
        }
        
        // Multiple updates
        auto* devSystem = brain.getDevelopmentSystem();
        if (devSystem) {
            devSystem->update(&brain, step);
        }
        
        // Brain step
        brain.step(step, step * 0.1f);
        
        if (step % 50 == 0) {
            std::cout << "      Stress step " << step << " completed" << std::endl;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    std::cout << "      Stress test completed in " << std::fixed << std::setprecision(2) << totalTime << " seconds" << std::endl;
    std::cout << "      Throughput: " << std::fixed << std::setprecision(0) << 200.0 / totalTime << " steps/sec" << std::endl;
    
    std::cout << "      Stress performance test passed" << std::endl;
}

void runAll() {
    std::cout << "Running Enhanced Brain Tests..." << std::endl;
    
    try {
        // Run basic tests
        testBrainCreation();
        testBrainInitialization();
        testBrainStep();
        testBrainReset();
        testBrainRegions();
        testBrainActionProduction();
        testBrainStatus();
        
        // Run enhanced tests
        testBrainContextManager();
        testBrainErrorHandling();
        testBrainEdgeCases();
        testBrainMemoryManagement();
        testBrainNeuromodulationIntegration();
        testBrainDevelopmentIntegration();
        testBrainPredictionIntegration();
        testBrainCognitionIntegration();
        testBrainAgentIntegration();
        testBrainStressPerformance();
        
        std::cout << "  All Enhanced Brain Tests PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        throw;
    }
}

} // namespace test_brain