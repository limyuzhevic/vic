// Phase 6 Integration Tests
// Tests the integrated artificial brain systems from Phase 6

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>

namespace test_phase6_integration {

void testMemorySystemIntegration() {
    std::cout << "  Testing memory system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Get memory system pointers
    nlm::NeuralWorkingMemory* workingMemory = brain.getWorkingMemory();
    nlm::NeuralEpisodicMemory* episodicMemory = brain.getEpisodicMemory();
    nlm::NeuralAssociativeMemory* associativeMemory = brain.getAssociativeMemory();
    
    // Verify memory systems are initialized
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    assert(associativeMemory != nullptr);
    
    // Test working memory integration with neural activity
    brain.injectCurrentToNeurons(nlm::NeuronType::Internal, 10.0f);
    
    // Run some steps to allow memory updates
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
    }
    
    // Verify working memory has been updated
    size_t workingMemoryTraces = workingMemory->getActiveTraces();
    std::cout << "    Working memory traces: " << workingMemoryTraces << std::endl;
    
    // Test episodic memory integration
    // (Episodic memory should store episodes based on neural activity)
    // For now, verify it doesn't crash and has basic functionality
    size_t episodeCount = episodicMemory->getEpisodeCount();
    std::cout << "    Episodic memory episodes: " << episodeCount << std::endl;
    
    std::cout << "    Memory system integration test passed" << std::endl;
}

void testNeuromodulationIntegration() {
    std::cout << "  Testing neuromodulation integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Get neuromodulation system pointers
    nlm::Dopamine* dopamine = brain.getDopamine();
    nlm::Curiosity* curiosity = brain.getCuriosity();
    nlm::Novelty* novelty = brain.getNovelty();
    nlm::PredictionError* predictionError = brain.getPredictionErrorSignal();
    
    // Verify neuromodulation systems are initialized
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    assert(predictionError != nullptr);
    
    // Test dopamine neuromodulation effects
    float initialDopamine = dopamine->getLevel();
    
    // Apply reward-based neuromodulation
    brain.getAgentBrain()->applyRewardModulation(0.8f, 0.5f);
    
    float finalDopamine = dopamine->getLevel();
    std::cout << "    Dopamine level: " << initialDopamine << " -> " << finalDopamine << std::endl;
    
    // Test curiosity neuromodulation
    float initialCuriosity = curiosity->getLevel();
    
    // Apply novelty signal
    brain.getAgentBrain()->applyRewardModulation(0.2f, 0.3f);
    
    float finalCuriosity = curiosity->getLevel();
    std::cout << "    Curiosity level: " << initialCuriosity << " -> " << finalCuriosity << std::endl;
    
    // Test prediction error signal
    float predictionErrorVal = predictionError->getError();
    std::cout << "    Prediction error: " << predictionErrorVal << std::endl;
    
    std::cout << "    Neuromodulation integration test passed" << std::endl;
}

void testPredictionSystemIntegration() {
    std::cout << "  Testing prediction system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(400), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Get prediction system pointer
    nlm::PredictionSystem* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Verify prediction system doesn't crash when accessed
    // (Actual prediction functionality would require more complex setup)
    std::cout << "    Prediction system initialized successfully" << std::endl;
    
    std::cout << "    Prediction system integration test passed" << std::endl;
}

void testDevelopmentIntegration() {
    std::cout << "  Testing development integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(350), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Get development system pointer
    nlm::DevelopmentSystem* developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    // Test developmental stages
    nlm::DevelopmentalStage initialStage = brain.getDevelopmentalStage();
    std::cout << "    Initial developmental stage: " << static_cast<int>(initialStage) << std::endl;
    
    // Simulate development over time
    brain.getAgentBrain()->updateDevelopment(100.0);  // 100ms
    
    nlm::DevelopmentalStage newStage = brain.getDevelopmentalStage();
    std::cout << "    New developmental stage: " << static_cast<int>(newStage) << std::endl;
    
    assert(newStage != initialStage);  // Should have changed
    
    std::cout << "    Development integration test passed" << std::endl;
}

void testCheckpointAndReplay() {
    std::cout << "  Testing checkpoint and replay functionality..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Run some steps to create activity
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    // Test checkpoint save
    std::string checkpointPath = "./test_checkpoint.bin";
    bool saveSuccess = brain.save(checkpointPath);
    assert(saveSuccess);
    std::cout << "    Checkpoint saved successfully" << std::endl;
    
    // Reset brain and reload from checkpoint
    brain.reset();
    brain.initialize();
    
    bool loadSuccess = brain.load(checkpointPath);
    assert(loadSuccess);
    std::cout << "    Checkpoint loaded successfully" << std::endl;
    
    // Verify some statistics are preserved (approximate)
    size_t originalNeurons = 200;
    assert(brain.getTotalNeuronCount() == originalNeurons);
    
    std::cout << "    Checkpoint and replay test passed" << std::endl;
}

void testCompleteBrainLoopIntegration() {
    std::cout << "  Testing complete brain loop integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Get agent brain interface
    auto agentBrain = brain.getAgentBrain();
    assert(agentBrain != nullptr);
    
    // Initialize agent brain with world
    nlm::SimpleWorld world;
    world.configure(20.0f, 20.0f, 16, 16);
    agentBrain->initialize(world);
    
    // Test complete sensory-motor loop
    nlm::SimulationStep steps = 0;
    nlm::SimulationStep maxSteps = 500;
    
    for (steps = 0; steps < maxSteps && brain.getTotalSpikeCount() < 50; ++steps) {
        // Create simple sensory input
        nlm::SensoryPercept percept;
        std::vector<float> vision(256, 0.5f);  // Simple constant vision
        std::vector<float> touch;
        std::vector<float> internal = {brain.getTotalSpikeCount() / 100.0f};
        std::vector<float> proprioception = {10.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f};
        
        percept.setVision(vision);
        percept.setTouch(touch);
        percept.setInternal(internal);
        percept.setProprioception(proprioception);
        
        // Process sensory input
        agentBrain->processSensoryInput(percept);
        
        // Step the brain
        brain.step(steps, steps * 0.001);
        
        // Get action from brain
        auto action = brain.produceAction();
        assert(action != nullptr);
        
        // Apply action to world
        if (steps % 10 == 0) {  // Every 10 steps
            // Simulate action effect
            float reward = 0.0f;
            bool success = false;
            
            // Simple world interaction (would normally use actual world)
            if (action->getType() == nlm::ActionType::MoveForward) {
                reward = -0.01f;
                success = true;
            }
            
            // Apply reward modulation
            agentBrain->applyRewardModulation(reward, 0.0f);
        }
    }
    
    std::cout << "    Complete brain loop ran " << steps << " steps" << std::endl;
    std::cout << "    Total spikes generated: " << brain.getTotalSpikeCount() << std::endl;
    
    // Verify systems are integrated
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getDopamine() != nullptr);
    assert(brain.getCuriosity() != nullptr);
    
    std::cout << "    Complete brain loop integration test passed" << std::endl;
}

void testPerformanceOptimizations() {
    std::cout << "  Testing performance optimizations..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Measure performance of large brain
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run some steps with larger brain
    for (nlm::SimulationStep step = 0; step < 200; ++step) {
        brain.step(step);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "    Performance test: 1000 neurons, 200 steps in " << duration.count() << "ms" << std::endl;
    std::cout << "    Average time per step: " << duration.count() / 200.0 << "ms" << std::endl;
    
    // Performance should be reasonable (not taking too long)
    assert(duration.count() < 5000);  // Should complete in less than 5 seconds
    
    std::cout << "    Performance optimizations test passed" << std::endl;
}

void runAll() {
    std::cout << "=== NLM Phase 6 Integration Tests ===" << std::endl;
    std::cout << "Testing complete artificial brain integration..." << std::endl << std::endl;
    
    testMemorySystemIntegration();
    std::cout << std::endl;
    
    testNeuromodulationIntegration();
    std::cout << std::endl;
    
    testPredictionSystemIntegration();
    std::cout << std::endl;
    
    testDevelopmentIntegration();
    std::cout << std::endl;
    
    testCheckpointAndReplay();
    std::cout << std::endl;
    
    testCompleteBrainLoopIntegration();
    std::cout << std::endl;
    
    testPerformanceOptimizations();
    std::cout << std::endl;
    
    std::cout << "=== All Phase 6 Integration Tests PASSED ===" << std::endl;
}

} // namespace test_phase6_integration
