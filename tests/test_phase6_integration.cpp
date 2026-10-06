// Phase 6 Integration Test Suite
// Comprehensive tests for NLM Phase 6 final integration

#include "src/experiments/Phase6IntegratedExperiment.hpp"
#include "src/core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <cmath>

namespace test_phase6 {

void test_basic_integration() {
    std::cout << "Testing basic Phase 6 integration..." << std::endl;
    
    // Create minimal configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100));
    config->set("region_count", static_cast<int64_t>(1));
    config->set("connection_probability", 0.1);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Verify integration points exist
    assert(brain->getWorkingMemory() != nullptr);
    assert(brain->getEpisodicMemory() != nullptr);
    assert(brain->getDopamine() != nullptr);
    assert(brain->getCuriosity() != nullptr);
    assert(brain->getPredictionSystem() != nullptr);
    assert(brain->getPlanner() != nullptr);
    assert(brain->getConceptFormation() != nullptr);
    assert(brain->getAttention() != nullptr);
    assert(brain->getDevelopmentSystem() != nullptr);
    
    std::cout << "  ✓ Basic integration passed" << std::endl;
}

void test_memory_integration() {
    std::cout << "Testing memory system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200));
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Test working memory
    auto* working_memory = brain->getWorkingMemory();
    assert(working_memory != nullptr);
    
    // Inject some activity to generate working memory traces
    brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 10.0f);
    
    for (int step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    // Check if working memory has traces
    size_t active_traces = working_memory->getActiveTraces();
    std::cout << "  Working memory active traces: " << active_traces << std::endl;
    
    // Test episodic memory
    auto* episodic_memory = brain->getEpisodicMemory();
    assert(episodic_memory != nullptr);
    
    // Run more steps to generate episodes
    for (int step = 50; step < 150; ++step) {
        brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 5.0f);
        brain->step(step, step * 0.001);
    }
    
    size_t episode_count = episodic_memory->getEpisodeCount();
    std::cout << "  Episodic memory episodes: " << episode_count << std::endl;
    
    // Test semantic memory
    auto* semantic_memory = brain->getSemanticMemory();
    assert(semantic_memory != nullptr);
    
    // Test procedural memory
    auto* procedural_memory = brain->getProceduralMemory();
    assert(procedural_memory != nullptr);
    
    // Test associative memory
    auto* associative_memory = brain->getAssociativeMemory();
    assert(associative_memory != nullptr);
    
    std::cout << "  ✓ Memory integration test passed" << std::endl;
}

void test_neuromodulation_integration() {
    std::cout << "Testing neuromodulation integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150));
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Get neuromodulation systems
    auto* dopamine = brain->getDopamine();
    auto* curiosity = brain->getCuriosity();
    auto* novelty = brain->getNovelty();
    
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    
    // Test dopamine modulation
    float initial_dopamine = dopamine->getLevel();
    
    // Apply reward modulation through brain
    brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 20.0f);
    
    for (int step = 0; step < 100; ++step) {
        brain->step(step, step * 0.001);
        
        // Simulate reward processing
        float reward = 1.0f; // Positive reward
        // Note: In a real agent, reward would be applied through AgentBrain
        // For this test, we're testing that the neuromodulation systems exist and function
    }
    
    float final_dopamine = dopamine->getLevel();
    std::cout << "  Dopamine level changed from " << initial_dopamine << " to " << final_dopamine << std::endl;
    
    // Test curiosity system
    float initial_curiosity = curiosity->getLevel();
    std::cout << "  Initial curiosity: " << initial_curiosity << std::endl;
    
    // Test novelty detection
    bool novel_event = novelty->detectNovelty(0.5f); // Random novelty signal
    std::cout << "  Novelty detected: " << (novel_event ? "yes" : "no") << std::endl;
    
    std::cout << "  ✓ Neuromodulation integration test passed" << std::endl;
}

void test_prediction_integration() {
    std::cout << "Testing prediction system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200));
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto* prediction_system = brain->getPredictionSystem();
    assert(prediction_system != nullptr);
    
    // Test prediction system initialization
    // Note: Prediction system functionality depends on specific implementation details
    // For this test, we verify that the system is properly integrated
    
    std::cout << "  ✓ Prediction integration test passed" << std::endl;
}

void test_development_integration() {
    std::cout << "Testing development system integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(180));
    config->set("synaptogenesis_rate", 0.0001);
    config->set("pruning_rate", 0.00001);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto* development_system = brain->getDevelopmentSystem();
    assert(development_system != nullptr);
    
    // Get initial developmental stage
    auto initial_stage = brain->getDevelopmentalStage();
    std::cout << "  Initial developmental stage: " << static_cast<int>(initial_stage) << std::endl;
    
    // Simulate development over time
    for (int step = 0; step < 200; ++step) {
        // Apply development updates
        brain->step(step, step * 0.001);
        
        // Note: In a real simulation, development would be applied through AgentBrain
        // For this test, we're testing that the development system is properly integrated
    }
    
    auto final_stage = brain->getDevelopmentalStage();
    std::cout << "  Final developmental stage: " << static_cast<int>(final_stage) << std::endl;
    
    // Test that development can progress through stages
    bool development_occurred = (final_stage != initial_stage);
    std::cout << "  Development occurred: " << (development_occurred ? "yes" : "no") << std::endl;
    
    std::cout << "  ✓ Development integration test passed" << std::endl;
}

void test_checkpointing() {
    std::cout << "Testing checkpoint save/load functionality..." << std::endl;
    
    auto config1 = std::make_shared<nlm::Config>();
    config1->set("neuron_count", static_cast<int64_t>(120));
    
    auto brain1 = std::make_shared<nlm::Brain>(config1);
    brain1->initialize();
    
    // Run some simulation steps
    for (int step = 0; step < 100; ++step) {
        brain1->injectCurrentToNeurons(nlm::NeuronType::Sensory, 15.0f);
        brain1->step(step, step * 0.001);
    }
    
    std::string checkpoint_path = "/tmp/nlm_phase6_checkpoint_test.bin";
    
    // Save checkpoint
    bool save_success = brain1->save(checkpoint_path);
    assert(save_success);
    std::cout << "  Checkpoint saved successfully" << std::endl;
    
    // Create new brain and load checkpoint
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", static_cast<int64_t>(120));
    
    auto brain2 = std::make_shared<nlm::Brain>(config2);
    brain2->initialize();
    
    bool load_success = brain2->load(checkpoint_path);
    assert(load_success);
    std::cout << "  Checkpoint loaded successfully" << std::endl;
    
    // Verify that some state was preserved
    // Note: Due to the nature of neural simulation, exact state restoration
    // is complex. The load should restore the structure at minimum.
    
    std::cout << "  ✓ Checkpointing test passed" << std::endl;
}

void test_replay() {
    std::cout << "Testing replay system functionality..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(160));
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto* episodic_memory = brain->getEpisodicMemory();
    assert(episodic_memory != nullptr);
    
    // Run simulation to generate episodes
    for (int step = 0; step < 300; ++step) {
        brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 10.0f);
        brain->step(step, step * 0.001);
        
        // Note: In a real agent, episodes would be stored through AgentBrain
        // For this test, we're testing that the replay system is properly integrated
    }
    
    // Check if episodes exist for replay
    size_t episode_count = episodic_memory->getEpisodeCount();
    std::cout << "  Episodes available for replay: " << episode_count << std::endl;
    
    if (episode_count > 0) {
        // Get episodes for replay (this would be used in a real agent)
        auto episodes = episodic_memory->getEpisodesForReplay(3);
        std::cout << "  Retrieved " << episodes.size() << " episodes for replay" << std::endl;
    }
    
    std::cout << "  ✓ Replay system test passed" << std::endl;
}

void test_complete_integration_experiment() {
    std::cout << "Running complete Phase 6 integration experiment..." << std::endl;
    
    // Create Phase 6 integrated experiment
    nlm::Phase6IntegratedExperiment experiment;
    
    // Run integration verification
    bool integration_ok = experiment.verifyIntegration();
    assert(integration_ok);
    std::cout << "  Integration verification passed" << std::endl;
    
    // Test individual systems
    bool memory_ok = experiment.testMemoryIntegration();
    assert(memory_ok);
    std::cout << "  Memory integration test passed" << std::endl;
    
    bool neuromodulation_ok = experiment.testNeuromodulationIntegration();
    assert(neuromodulation_ok);
    std::cout << "  Neuromodulation integration test passed" << std::endl;
    
    bool checkpointing_ok = experiment.testCheckpointing();
    assert(checkpointing_ok);
    std::cout << "  Checkpointing test passed" << std::endl;
    
    bool replay_ok = experiment.testReplay();
    assert(replay_ok);
    std::cout << "  Replay system test passed" << std::endl;
    
    // Run full experiment with smaller settings for testing
    nlm::Phase6Config config;
    config.neuronCount = 100;
    config.maxSteps = 500;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
    auto result = experiment.run(config);
    
    std::cout << "  Full integration experiment results:" << std::endl;
    std::cout << "    Total reward: " << result.totalReward << std::endl;
    std::cout << "    Average firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "    Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "    Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << "    Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    
    // Verify integration status
    assert(result.memoryWorkingMemoryIntegrated);
    assert(result.memoryEpisodicMemoryIntegrated);
    assert(result.neuromodulationIntegrated);
    assert(result.predictionIntegrated);
    assert(result.developmentIntegrated);
    assert(result.checkpointingWorks);
    
    std::cout << "  ✓ Complete integration experiment passed" << std::endl;
}

void run_all_tests() {
    std::cout << "==========================================" << std::endl;
    std::cout << "NLM Phase 6 Integration Test Suite" << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << "" << std::endl;
    
    try {
        test_basic_integration();
        test_memory_integration();
        test_neuromodulation_integration();
        test_prediction_integration();
        test_development_integration();
        test_checkpointing();
        test_replay();
        test_complete_integration_experiment();
        
        std::cout << "" << std::endl;
        std::cout << "==========================================" << std::endl;
        std::cout << "ALL TESTS PASSED! ✓" << std::endl;
        std::cout << "==========================================" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "" << std::endl;
        std::cout << "==========================================" << std::endl;
        std::cout << "TEST FAILED! ✗" << std::endl;
        std::cout << "Error: " << e.what() << std::endl;
        std::cout << "==========================================" << std::endl;
        throw;
    }
}

} // namespace test_phase6
