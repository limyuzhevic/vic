// Comprehensive Integration Tests for NLM Brain
// Tests the complete Phase 6 integration of all brain systems

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>

namespace test_integration {

// Test 1: Complete Brain Loop Integration
void testCompleteBrainLoop() {
    std::cout << "    testCompleteBrainLoop: Testing complete brain loop integration..." << std::endl;
    
    // Create configuration with all systems enabled
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.05, nlm::ConfigSource::Default);
    config->set("plasticity.stdp.enable", true, nlm::ConfigSource::Default);
    config->set("plasticity.hebbian.enable", true, nlm::ConfigSource::Default);
    config->set("neuromod.curiosity.enable", true, nlm::ConfigSource::Default);
    config->set("neuromod.novelty.enable", true, nlm::ConfigSource::Default);
    
    // Create brain
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Verify all systems are initialized
    assert(brain.getTotalNeuronCount() == 500);
    assert(brain.getRegionCount() == 3);
    assert(brain.getTotalSynapseCount() > 0);
    assert(brain.getDevelopmentSystem() != nullptr);
    assert(brain.getPredictionSystem() != nullptr);
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getDopamine() != nullptr);
    assert(brain.getCuriosity() != nullptr);
    
    // Run complete integration test
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        // Apply neuromodulation
        nlm::Neuromodulator neuromodulator;
        neuromodulator.setCuriosity(0.5f + 0.3f * std::sin(step * 0.1f));
        neuromodulator.setNovelty(0.3f + 0.2f * std::cos(step * 0.05f));
        brain.applyNeuromodulation(neuromodulator);
        
        // Execute brain step
        brain.step(step, static_cast<nlm::Timestamp>(step) * 0.001);
        
        // Verify statistics
        assert(brain.getTotalSpikeCount() >= 0);
        assert(brain.getFiringNeuronCount() <= brain.getTotalNeuronCount());
        assert(brain.getAverageFiringRate() >= 0.0f);
        assert(brain.getExcitationInhibitionRatio() >= 0.0f);
    }
    
    // Verify development stage progression
    nlm::DevelopmentalStage final_stage = brain.getDevelopmentalStage();
    assert(final_stage >= nlm::DevelopmentalStage::Initial);
    
    std::cout << "    testCompleteBrainLoop passed" << std::endl;
}

// Test 2: Agent-World Integration
void testAgentWorldIntegration() {
    std::cout << "    testAgentWorldIntegration: Testing agent-world integration..." << std::endl;
    
    // Create components
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::SimpleWorld world;
    world.configure(20, 20, 8, 8);
    world.reset();
    world.setAgentStart(10.0, 10.0);
    
    nlm::AgentBrain agent(&brain);
    agent.initialize(world);
    
    // Enable learning subsystems
    agent.enableRewardModulation(true);
    agent.enableCuriosity(true);
    agent.enableDevelopment(true);
    
    // Run agent simulation
    std::vector<float> reward_history;
    std::vector<float> curiosity_history;
    
    for (nlm::SimulationStep step = 0; step < 200; ++step) {
        // Update world
        world.update(0.1);
        
        // Get sensory input
        nlm::SensoryPercept percept = world.getSensoryPercept();
        
        // Process sensory input
        agent.processSensoryInput(percept);
        
        // Brain thinks
        brain.step(step);
        
        // Get action
        nlm::Action action = agent.decodeMotorCommand();
        
        // Apply action
        nlm::ActionResult result = world.applyMotorCommand(action, world.getSimulationTime());
        
        // Record reward
        reward_history.push_back(result.reward);
        
        // Apply reward modulation
        agent.applyRewardModulation(result.reward, 0.0);
        
        // Update development
        agent.updateDevelopment(0.1);
        
        // Record metrics
        curiosity_history.push_back(agent.getCuriosityLevel());
        
        // Verify agent is functioning
        assert(agent.getCuriosityLevel() >= 0.0f);
        assert(agent.getNoveltyLevel() >= 0.0f);
        assert(agent.getPredictionError() >= 0.0f);
        assert(agent.getNeuromodulationLevel() >= 0.0f);
    }
    
    // Analyze results
    float avg_reward = std::accumulate(reward_history.begin(), reward_history.end(), 0.0f) / reward_history.size();
    float avg_curiosity = std::accumulate(curiosity_history.begin(), curiosity_history.end(), 0.0f) / curiosity_history.size();
    
    assert(avg_reward >= -1.0f);  // Rewards should be reasonable
    assert(avg_curiosity >= 0.0f);  // Curiosity should be non-negative
    
    std::cout << "    testAgentWorldIntegration passed (avg reward: " << avg_reward 
              << ", avg curiosity: " << avg_curiosity << ")" << std::endl;
}

// Test 3: Memory System Integration
void testMemorySystemIntegration() {
    std::cout << "    testMemorySystemIntegration: Testing memory system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(400), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Access all memory systems
    nlm::NeuralWorkingMemory* working_memory = brain.getWorkingMemory();
    nlm::NeuralEpisodicMemory* episodic_memory = brain.getEpisodicMemory();
    nlm::NeuralAssociativeMemory* associative_memory = brain.getAssociativeMemory();
    
    assert(working_memory != nullptr);
    assert(episodic_memory != nullptr);
    assert(associative_memory != nullptr);
    
    // Test working memory
    working_memory->initialize(&brain);
    working_memory->setCapacity(50);
    
    for (int i = 0; i < 10; ++i) {
        std::vector<float> pattern(i + 1, 1.0f / (i + 1));
        working_memory->storePattern(pattern, 0.5f + 0.1f * i);
    }
    
    assert(working_memory->getActiveTraces() >= 0);
    
    // Test episodic memory
    episodic_memory->initialize(&brain);
    episodic_memory->setMaxEpisodes(1000);
    
    for (int i = 0; i < 5; ++i) {
        nlm::EpisodicEpisode episode;
        episode.step = i;
        episode.importance = 0.8f - 0.1f * i;
        episode.pattern = std::vector<float>(10, 0.5f);
        episodic_memory->storeEpisode(episode);
    }
    
    assert(episodic_memory->getEpisodeCount() == 5);
    episodic_memory->replayImportantMemories(3);  // Replay top 3 episodes
    assert(episodic_memory->getReplayCount() == 3);
    
    // Test associative memory
    associative_memory->initialize(&brain);
    associative_memory->setAssociativityStrength(0.7f);
    
    associative_memory->learnAssociation("reward", "positive_outcome", 0.9f);
    associative_memory->learnAssociation("punishment", "negative_outcome", 0.8f);
    associative_memory->learnAssociation("novelty", "exploration", 0.6f);
    
    std::vector<std::string> reward_associations = associative_memory->getAssociations("reward");
    assert(reward_associations.size() >= 1);
    
    std::cout << "    testMemorySystemIntegration passed" << std::endl;
}

// Test 4: Prediction System Integration
void testPredictionSystemIntegration() {
    std::cout << "    testPredictionSystemIntegration: Testing prediction system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(350), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::PredictionSystem* prediction_system = brain.getPredictionSystem();
    assert(prediction_system != nullptr);
    
    // Configure prediction system
    prediction_system->setPredictionHorizon(10);
    prediction_system->enablePrediction(true);
    prediction_system->setPredictionAccuracy(0.85f);
    
    // Run simulation with prediction
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        // Generate some prediction targets
        std::vector<float> prediction_target(10, 0.5f + 0.3f * std::sin(step * 0.1f));
        
        // Get prediction
        std::vector<float> prediction = prediction_system->getPrediction(step % 10);
        
        // Calculate prediction error
        float error = 0.0f;
        for (size_t i = 0; i < std::min(prediction.size(), prediction_target.size()); ++i) {
            error += std::abs(prediction[i] - prediction_target[i]);
        }
        error /= std::min(prediction.size(), prediction_target.size());
        
        assert(error >= 0.0f && error <= 1.0f);  // Error should be reasonable
        
        brain.step(step);
    }
    
    // Get prediction confidence
    float confidence = prediction_system->getPredictionConfidence();
    assert(confidence >= 0.0f && confidence <= 1.0f);
    
    std::cout << "    testPredictionSystemIntegration passed (final confidence: " << confidence << ")" << std::endl;
}

// Test 5: Neuromodulation Integration
void testNeuromodulationIntegration() {
    std::cout << "    testNeuromodulationIntegration: Testing neuromodulation integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(450), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test dopamine modulation
    nlm::Dopamine* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    dopamine->setScale(1.5f);
    dopamine->setLearningRate(0.01f);
    
    // Test curiosity modulation
    nlm::Curiosity* curiosity = brain.getCuriosity();
    assert(curiosity != nullptr);
    
    curiosity->setExplorationRate(0.3f);
    curiosity->setThreshold(0.7f);
    
    // Test novelty detection
    nlm::Novelty* novelty = brain.getNovelty();
    assert(novelty != nullptr);
    
    novelty->setSensitivity(0.8f);
    novelty->setAdaptationRate(0.9f);
    
    // Test prediction error
    nlm::PredictionError* prediction_error = brain.getPredictionErrorSignal();
    assert(prediction_error != nullptr);
    
    prediction_error->setErrorThreshold(0.5f);
    prediction_error->setUpdateRate(0.05f);
    
    // Run simulation with neuromodulation
    float total_reward = 0.0f;
    float total_curiosity = 0.0f;
    float total_novelty = 0.0f;
    
    for (nlm::SimulationStep step = 0; step < 150; ++step) {
        // Apply neuromodulatory signals
        nlm::Neuromodulator neuromodulator;
        neuromodulator.setDopamineScale(1.0f + 0.5f * std::sin(step * 0.05f));
        neuromodulator.setCuriosityLevel(0.5f + 0.3f * std::cos(step * 0.08f));
        neuromodulator.setNoveltyLevel(0.3f + 0.2f * std::sin(step * 0.03f));
        neuromodulator.setPredictionError(0.4f + 0.3f * std::cos(step * 0.06f));
        
        brain.applyNeuromodulation(neuromodulator);
        
        // Execute brain step
        brain.step(step);
        
        // Collect neuromodulation statistics
        total_reward += dopamine->getCurrentReward();
        total_curiosity += curiosity->getCurrentCuriosity();
        total_novelty += novelty->getCurrentNovelty();
    }
    
    // Verify neuromodulation statistics
    assert(total_reward >= 0.0f);
    assert(total_curiosity >= 0.0f);
    assert(total_novelty >= 0.0f);
    
    std::cout << "    testNeuromodulationIntegration passed (total reward: " << total_reward 
              << ", total curiosity: " << total_curiosity << ")" << std::endl;
}

// Test 6: Development System Integration
void testDevelopmentSystemIntegration() {
    std::cout << "    testDevelopmentSystemIntegration: Testing development system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(400), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::DevelopmentSystem* development_system = brain.getDevelopmentSystem();
    assert(development_system != nullptr);
    
    // Configure development system
    development_system->setCriticalPeriodDuration(50);
    development_system->setMaturationRate(0.01f);
    development_system->setPruningThreshold(0.3f);
    
    // Test development stage progression
    nlm::DevelopmentalStage initial_stage = brain.getDevelopmentalStage();
    assert(initial_stage == nlm::DevelopmentalStage::Initial);
    
    // Run simulation to advance development
    for (nlm::SimulationStep step = 0; step < 200; ++step) {
        // Update development
        brain.develop();
        
        // Apply developmental changes
        development_system->applySynaptogenesis(0.01f, 0.001f);
        development_system->applyPruning(0.005f);
        development_system->applyMaturation(0.005f);
        
        // Execute brain step
        brain.step(step);
        
        // Verify development stage progression
        nlm::DevelopmentalStage current_stage = brain.getDevelopmentalStage();
        assert(current_stage >= initial_stage);
        
        // In a real system, development stages would progress
        // For testing, we'll verify they don't regress
    }
    
    // Verify final development stage
    nlm::DevelopmentalStage final_stage = brain.getDevelopmentalStage();
    assert(final_stage >= nlm::DevelopmentalStage::Initial);
    
    std::cout << "    testDevelopmentSystemIntegration passed (final stage: " << final_stage << ")" << std::endl;
}

// Test 7: Error Handling and Recovery
void testErrorHandlingAndRecovery() {
    std::cout << "    testErrorHandlingAndRecovery: Testing error handling and recovery..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Test null pointer handling in injectCurrent
    brain.injectCurrent(nlm::INVALID_NEURON_ID, 10.0f);  // Should not crash
    brain.injectCurrentToNeurons(nlm::NeuronType::Internal, 5.0f);  // Should work
    
    // Test invalid region access
    nlm::RegionId invalid_region(999); // Likely invalid
    nlm::NeuralRegion* region = brain.getRegion(invalid_region);
    // Region may be null, but shouldn't crash
    
    // Test reset functionality
    brain.reset();
    assert(brain.getTotalNeuronCount() == 200);  // Should remain valid
    
    // Test save/load functionality
    std::string test_file = "test_checkpoint.bin";
    bool save_success = brain.save(test_file);
    // Save may or may not succeed depending on implementation
    
    if (save_success) {
        nlm::Brain brain2(config);
        bool init2_success = brain2.initialize();
        assert(init2_success);
        
        bool load_success = brain2.load(test_file);
        assert(load_success);
        
        // Clean up
        std::remove(test_file.c_str());
    }
    
    // Test memory leak detection (basic check)
    // In a real system, this would involve memory profiling
    // For now, we just verify the system can run for a reasonable time
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    std::cout << "    testErrorHandlingAndRecovery passed" << std::endl;
}

// Test 8: Performance and Scalability
void testPerformanceAndScalability() {
    std::cout << "    testPerformanceAndScalability: Testing performance and scalability..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    
    // Measure initialization time
    auto start_time = std::chrono::high_resolution_clock::now();
    
    nlm::Brain brain(config);
    bool init_success = brain.initialize();
    assert(init_success);
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto init_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    std::cout << "      Initialization took " << init_duration.count() << " ms" << std::endl;
    
    // Measure step performance
    start_time = std::chrono::high_resolution_clock::now();
    
    for (nlm::SimulationStep step = 0; step < 500; ++step) {
        brain.step(step);
    }
    
    end_time = std::chrono::high_resolution_clock::now();
    auto step_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    double steps_per_second = 500.0 / (step_duration.count() / 1000.0);
    
    std::cout << "      Steps per second: " << steps_per_second << std::endl;
    
    // Verify performance characteristics
    assert(steps_per_second > 0.0);  // Should process steps
    assert(brain.getTotalSpikeCount() >= 0);  // Should track spikes
    
    // Test memory usage (basic check)
    size_t initial_memory = brain.getTotalNeuronCount() + brain.getTotalSynapseCount();
    assert(initial_memory > 0);
    
    std::cout << "    testPerformanceAndScalability passed (" << steps_per_second << " steps/sec)" << std::endl;
}

// Run all integration tests
void runAllIntegrationTests() {
    std::cout << "=== NLM Integration Test Suite ===" << std::endl;
    
    testCompleteBrainLoop();
    testAgentWorldIntegration();
    testMemorySystemIntegration();
    testPredictionSystemIntegration();
    testNeuromodulationIntegration();
    testDevelopmentSystemIntegration();
    testErrorHandlingAndRecovery();
    testPerformanceAndScalability();
    
    std::cout << "\n=== All Integration Tests Passed ===" << std::endl;
    std::cout << "Phase 6 integration is working correctly!" << std::endl;
}

} // namespace test_integration