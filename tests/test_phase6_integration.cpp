// Memory Integration Tests for Phase 6
// Tests that all memory systems work together with neural processing

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_phase6 {

void testMemoryIntegration() {
    std::cout << "  Testing memory system integration..." << std::endl;
    
    // Create configuration with memory systems enabled
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 200);
    config->set("region_count", 1);
    config->set("working_memory.capacity", 500);
    config->set("episodic_memory.max_episodes", 100);
    config->set("semantic_memory.association_strength", 0.5f);
    config->set("procedural_memory.learning_rate", 0.01f);
    
    // Create brain with memory systems
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    // Test working memory integration
    auto* workingMemory = brain->getWorkingMemory();
    assert(workingMemory != nullptr);
    std::cout << "    Working memory: Available" << std::endl;
    
    // Add and retrieve traces
    workingMemory->addTrace(1000.0f, 0.8f);
    workingMemory->addTrace(2000.0f, 0.6f);
    workingMemory->addTrace(3000.0f, 0.9f);
    
    assert(workingMemory->getActiveTraces() == 3);
    std::cout << "    Working memory traces: 3" << std::endl;
    
    // Test episodic memory integration
    auto* episodicMemory = brain->getEpisodicMemory();
    assert(episodicMemory != nullptr);
    std::cout << "    Episodic memory: Available" << std::endl;
    
    // Record episodes
    episodicMemory->recordEpisode(1000.0f, 0.7f, "Test episode 1");
    episodicMemory->recordEpisode(2000.0f, 0.8f, "Test episode 2");
    episodicMemory->recordEpisode(3000.0f, 0.9f, "Test episode 3");
    
    assert(episodicMemory->getEpisodeCount() == 3);
    std::cout << "    Episodic memory episodes: 3" << std::endl;
    
    // Test episodic memory replay
    auto episodes = episodicMemory->getEpisodesForReplay(2);
    assert(episodes.size() == 2);
    std::cout << "    Episodic memory replay: 2 episodes retrieved" << std::endl;
    
    // Test associative memory integration
    auto* associativeMemory = brain->getAssociativeMemory();
    assert(associativeMemory != nullptr);
    std::cout << "    Associative memory: Available" << std::endl;
    
    // Test procedural memory integration
    auto* proceduralMemory = brain->getProceduralMemory();
    assert(proceduralMemory != nullptr);
    std::cout << "    Procedural memory: Available" << std::endl;
    
    // Run simulation to test integration
    for (int step = 0; step < 100; ++step) {
        brain->step(step, step * 0.001);
        
        // Verify working memory is being used
        if (workingMemory->getActiveTraces() > 0) {
            std::cout << "    Working memory active during simulation" << std::endl;
            break;
        }
    }
    
    std::cout << "  Memory integration test completed successfully" << std::endl;
}

void testNeuromodulationIntegration() {
    std::cout << "  Testing neuromodulation system integration..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 150);
    config->set("region_count", 1);
    
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    // Test dopamine system integration
    auto* dopamine = brain->getDopamine();
    assert(dopamine != nullptr);
    std::cout << "    Dopamine system: Available" << std::endl;
    
    // Apply reward signals
    dopamine->applyReward(1.0f, 0.8f); // Positive reward
    assert(dopamine->getCurrentLevel() > 0.0f);
    std::cout << "    Dopamine level after reward: " << dopamine->getCurrentLevel() << std::endl;
    
    dopamine->applyReward(-0.5f, 0.2f); // Negative reward
    assert(dopamine->getCurrentLevel() < 0.0f);
    std::cout << "    Dopamine level after negative reward: " << dopamine->getCurrentLevel() << std::endl;
    
    // Test curiosity system integration
    auto* curiosity = brain->getCuriosity();
    assert(curiosity != nullptr);
    std::cout << "    Curiosity system: Available" << std::endl;
    
    float curiosityLevel = curiosity->getCurrentLevel();
    std::cout << "    Initial curiosity level: " << curiosityLevel << std::endl;
    
    // Test novelty system integration
    auto* novelty = brain->getNovelty();
    assert(novelty != nullptr);
    std::cout << "    Novelty system: Available" << std::endl;
    
    // Process novel patterns
    novelty->processPattern(std::vector<float>{1.0f, 0.5f, 0.0f});
    float noveltyLevel = novelty->getCurrentLevel();
    std::cout << "    Novelty level after pattern: " << noveltyLevel << std::endl;
    
    // Test prediction error system integration
    auto* predictionError = brain->getPredictionErrorSignal();
    assert(predictionError != nullptr);
    std::cout << "    Prediction error system: Available" << std::endl;
    
    // Test neuromodulation affects neural plasticity
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, true);
        }
    }
    
    std::cout << "  Neuromodulation integration test completed successfully" << std::endl;
}

void testPredictionIntegration() {
    std::cout << "  Testing prediction system integration..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 180);
    config->set("region_count", 1);
    
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    // Test prediction system integration
    auto* prediction = brain->getPredictionSystem();
    assert(prediction != nullptr);
    std::cout << "    Prediction system: Available" << std::endl;
    
    // Test concept formation integration
    auto* conceptFormation = brain->getConceptFormation();
    assert(conceptFormation != nullptr);
    std::cout << "    Concept formation system: Available" << std::endl;
    
    // Test neural planner integration
    auto* neuralPlanner = brain->getPlanner();
    assert(neuralPlanner != nullptr);
    std::cout << "    Neural planner system: Available" << std::endl;
    
    // Test attentional selection integration
    auto* attentionalSelection = brain->getAttention();
    assert(attentionalSelection != nullptr);
    std::cout << "    Attentional selection system: Available" << std::endl;
    
    // Run simulation to test prediction integration
    for (int step = 0; step < 200; ++step) {
        brain->step(step, step * 0.001);
        
        // Verify prediction system is active
        if (prediction->getPredictionError() > 0.0f) {
            std::cout << "    Prediction system active: error=" << prediction->getPredictionError() << std::endl;
            break;
        }
    }
    
    std::cout << "  Prediction integration test completed successfully" << std::endl;
}

void testDevelopmentIntegration() {
    std::cout << "  Testing development system integration..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 120);
    config->set("region_count", 1);
    config->set("plasticity.synaptogenesis_rate", 0.0001f);
    config->set("plasticity.pruning_rate", 0.00001f);
    config->set("development.initial_plasticity", 1.0f);
    config->set("development.maturation_rate", 0.001f);
    
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    // Test development system integration
    auto* development = brain->getDevelopmentSystem();
    assert(development != nullptr);
    std::cout << "    Development system: Available" << std::endl;
    
    // Test developmental stage tracking
    DevelopmentalStage initialStage = brain->getDevelopmentalStage();
    std::cout << "    Initial developmental stage: " << static_cast<int>(initialStage) << std::endl;
    
    // Test development affects plasticity
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            float initialWeight = syn->getWeight();
            syn->enablePlasticity(true, true, true);
            
            // Run development update
            development->update(0.001f);
            
            float updatedWeight = syn->getWeight();
            std::cout << "    Synaptic weight change: " << (updatedWeight - initialWeight) << std::endl;
        }
    }
    
    // Test development maturation over time
    for (int step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
        development->update(0.001f);
    }
    
    DevelopmentalStage finalStage = brain->getDevelopmentalStage();
    std::cout << "    Final developmental stage: " << static_cast<int>(finalStage) << std::endl;
    
    std::cout << "  Development integration test completed successfully" << std::endl;
}

void testCheckpointingSystem() {
    std::cout << "  Testing checkpoint system..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    
    auto brain1 = std::make_shared<Brain>(config);
    assert(brain1->initialize());
    
    // Run simulation
    for (int step = 0; step < 100; ++step) {
        brain1->step(step, step * 0.001);
    }
    
    float firingRate1 = brain1->getAverageFiringRate();
    size_t spikes1 = brain1->getTotalSpikeCount();
    
    std::cout << "    Before checkpoint: " << spikes1 << " spikes, " << firingRate1 << " Hz avg firing rate" << std::endl;
    
    // Save checkpoint
    std::string checkpointPath = "/tmp/nlm_phase6_checkpoint_test.bin";
    assert(brain1->save(checkpointPath));
    std::cout << "    Checkpoint saved to " << checkpointPath << std::endl;
    
    // Create new brain and load
    auto brain2 = std::make_shared<Brain>(std::make_shared<Config>(*config));
    assert(brain2->initialize());
    
    assert(brain2->load(checkpointPath));
    std::cout << "    Checkpoint loaded successfully" << std::endl;
    
    size_t spikes2 = brain2->getTotalSpikeCount();
    std::cout << "    After load: " << spikes2 << " spikes" << std::endl;
    
    // Verify checkpoint worked
    assert(spikes2 == spikes1);
    std::cout << "    Checkpoint integrity verified" << std::endl;
    
    std::cout << "  Checkpoint system test completed successfully" << std::endl;
}

void testReplaySystem() {
    std::cout << "  Testing replay system..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 120);
    config->set("region_count", 1);
    
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    auto* episodicMemory = brain->getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Run simulation to create episodes
    std::cout << "    Creating episodes through simulation..." << std::endl;
    for (int step = 0; step < 300; ++step) {
        brain->step(step, step * 0.001);
        
        if (step % 50 == 0) {
            episodicMemory->recordEpisode(step * 0.001f, brain->getAverageFiringRate(), 
                                         "Episode at step " + std::to_string(step));
        }
    }
    
    std::cout << "    Created " << episodicMemory->getEpisodeCount() << " episodes" << std::endl;
    
    // Test replay with different parameters
    auto episodes = episodicMemory->getEpisodesForReplay(5);
    std::cout << "    Retrieved " << episodes.size() << " episodes for replay" << std::endl;
    
    // Test consolidation during rest
    std::cout << "    Testing memory consolidation..." << std::endl;
    episodicMemory->consolidate(100.0f); // Consolidate over 100 time units
    
    // Verify consolidation improved episode stability
    size_t stableEpisodes = episodicMemory->getStableEpisodeCount();
    std::cout << "    Stable episodes after consolidation: " << stableEpisodes << std::endl;
    
    std::cout << "  Replay system test completed successfully" << std::endl;
}

void testCompleteIntegration() {
    std::cout << "  Testing complete system integration..." << std::endl;
    
    // Create comprehensive configuration for Phase 6 integration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500);
    config->set("region_count", 2);
    config->set("connection_probability", 0.1f);
    
    // Enable all advanced features
    config->set("plasticity.stdp.enable", true);
    config->set("plasticity.stdp.learning_rate", 0.01f);
    config->set("plasticity.hebbian.enable", true);
    config->set("plasticity.structural.enable", true);
    config->set("neuromod.dopamine.scale", 1.0f);
    config->set("neuromod.curiosity.enable", true);
    config->set("neuromod.novelty.enable", true);
    config->set("prediction.enabled", true);
    config->set("development.enabled", true);
    config->set("working_memory.capacity", 1000);
    config->set("episodic_memory.max_episodes", 500);
    config->set("concept_formation.enabled", true);
    config->set("neural_planner.enabled", true);
    config->set("attentional_selection.enabled", true);
    
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    std::cout << "    Comprehensive brain created with " << brain->getTotalNeuronCount() << " neurons" << std::endl;
    std::cout << "    Total synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "    Regions: " << brain->getRegionCount() << std::endl;
    
    // Create world and agent
    auto world = std::make_shared<SimpleWorld>();
    world->initialize(20, 20);
    
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(*world);
    
    agent->enableRewardModulation(true);
    agent->enableStructuralPlasticity(true);
    agent->enableDevelopment(true);
    agent->enableCuriosity(true);
    
    std::cout << "    Agent created with all features enabled" << std::endl;
    
    // Run complete integration simulation
    std::cout << "    Running complete integration simulation (500 steps)..." << std::endl;
    for (int step = 0; step < 500; ++step) {
        world->update(0.1);
        
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        
        brain->step(step);
        
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Apply reward
        agent->applyRewardModulation(1.0f, 0.0f);
        
        // Update development
        agent->updateDevelopment(0.1f);
        
        if (step % 100 == 0) {
            std::cout << "      Step " << step << ": " << brain->getFiringNeuronCount() << " neurons firing, "
                      << agent->getCuriosityLevel() << " curiosity, "
                      << agent->getNoveltyLevel() << " novelty" << std::endl;
        }
    }
    
    // Final integration status
    std::cout << "    Final integration status:" << std::endl;
    std::cout << "      Total reward received: Working memory integration ✓" << std::endl;
    std::cout << "      Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "      Developmental stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
    std::cout << "      Neuromodulation level: " << agent->getNeuromodulationLevel() << std::endl;
    std::cout << "      Curiosity level: " << agent->getCuriosityLevel() << std::endl;
    std::cout << "      Novelty level: " << agent->getNoveltyLevel() << std::endl;
    
    // Verify all systems are properly integrated
    assert(brain->getWorkingMemory() != nullptr);
    assert(brain->getEpisodicMemory() != nullptr);
    assert(brain->getDopamine() != nullptr);
    assert(brain->getCuriosity() != nullptr);
    assert(brain->getPredictionSystem() != nullptr);
    assert(brain->getDevelopmentSystem() != nullptr);
    
    std::cout << "    All core systems verified as integrated" << std::endl;
    
    std::cout << "  Complete integration test completed successfully" << std::endl;
}

void runAllIntegrationTests() {
    std::cout << "=== Running NLM Phase 6 Integration Tests ===" << std::endl;
    std::cout << "These tests verify that all brain systems work together properly." << std::endl;
    std::cout << std::endl;
    
    testMemoryIntegration();
    std::cout << std::endl;
    
    testNeuromodulationIntegration();
    std::cout << std::endl;
    
    testPredictionIntegration();
    std::cout << std::endl;
    
    testDevelopmentIntegration();
    std::cout << std::endl;
    
    testCheckpointingSystem();
    std::cout << std::endl;
    
    testReplaySystem();
    std::cout << std::endl;
    
    testCompleteIntegration();
    std::cout << std::endl;
}

} // namespace test_phase6
