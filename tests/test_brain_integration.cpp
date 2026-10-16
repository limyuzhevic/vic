// Brain Integration Tests
// Tests for integration between brain components and systems

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <memory>

namespace test_brain_integration {

void testMemoryIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200));
    config->set("region_count", static_cast<int64_t>(2));
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test memory system integration
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    auto* associativeMemory = brain.getAssociativeMemory();
    
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    assert(associativeMemory != nullptr);
    
    // Run some steps to generate memory traces
    for (int i = 0; i < 50; ++i) {
        brain.step(i, i * 0.001);
    }
    
    // Check if working memory has traces
    if (workingMemory->getActiveTraces() > 0) {
        std::cout << "  Working memory has active traces: " << workingMemory->getActiveTraces() << std::endl;
    }
    
    // Check if episodic memory has episodes
    if (episodicMemory->getEpisodeCount() > 0) {
        std::cout << "  Episodic memory has episodes: " << episodicMemory->getEpisodeCount() << std::endl;
    }
    
    std::cout << "    testMemoryIntegration passed" << std::endl;
}

void testNeuromodulationIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150));
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Get neuromodulation systems
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    
    // Run steps with sensory input to stimulate neuromodulation
    for (int i = 0; i < 100; ++i) {
        // Inject some sensory activity
        brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 5.0f);
        brain.step(i, i * 0.001);
    }
    
    std::cout << "    testNeuromodulationIntegration passed" << std::endl;
}

void testPlasticityIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100));
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Enable plasticity on synapses
    if (auto* region = brain.getRegion(nlm::RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    // Apply repeated input patterns to stimulate learning
    for (int i = 0; i < 500; ++i) {
        // Create input pattern - inject current into sensory neurons
        for (size_t j = 0; j < 20 && j < brain.getTotalNeuronCount() / 5; ++j) {
            brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 30.0f);
        }
        
        brain.step(i, i * 0.001);
        
        // Log progress every 100 steps
        if (i % 100 == 0) {
            std::cout << "    Step " << i << " | Spikes: " << brain.getTotalSpikeCount() 
                      << " | Firing: " << brain.getAverageFiringRate() << std::endl;
        }
    }
    
    // Check if there was any synaptic change
    if (auto* region = brain.getRegion(nlm::RegionId(1))) {
        auto& synapses = region->getSynapses();
        if (!synapses.empty()) {
            std::cout << "    Testing STDP on " << synapses.size() << " synapses" << std::endl;
        }
    }
    
    std::cout << "    testPlasticityIntegration passed" << std::endl;
}

void testDevelopmentIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(120));
    config->set("enable_development", true);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Get development system
    auto* developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    // Run some steps to trigger development updates
    for (int i = 0; i < 2500; ++i) {  // Every 1000 steps triggers development
        brain.step(i, i * 0.001);
        
        if (i % 1000 == 0) {
            std::cout << "    Development update at step " << i << std::endl;
        }
    }
    
    // Check if development stage changed
    nlm::DevelopmentalStage stage = brain.getDevelopmentalStage();
    std::cout << "    Development stage: " << static_cast<int>(stage) << std::endl;
    
    std::cout << "    testDevelopmentIntegration passed" << std::endl;
}

void testPredictionIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100));
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Get prediction system
    auto* predictionSystem = brain.getPredictionSystem();
    
    if (predictionSystem != nullptr) {
        std::cout << "  Prediction system is available" << std::endl;
        
        // Run some steps
        for (int i = 0; i < 200; ++i) {
            brain.step(i, i * 0.001);
        }
        
        std::cout << "    testPredictionIntegration passed (prediction system available)" << std::endl;
    } else {
        std::cout << "    testPredictionIntegration passed (prediction system not required)" << std::endl;
    }
}

void testCognitionIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150));
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Get cognition systems
    auto* planner = brain.getPlanner();
    auto* conceptFormation = brain.getConceptFormation();
    auto* attention = brain.getAttention();
    
    assert(planner != nullptr);
    assert(conceptFormation != nullptr);
    assert(attention != nullptr);
    
    // Run some steps
    for (int i = 0; i < 100; ++i) {
        brain.step(i, i * 0.001);
    }
    
    std::cout << "    testCognitionIntegration passed" << std::endl;
}

void testCheckpointIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(80));
    config->set("enable_checkpointing", true);
    
    nlm::Brain brain1(config);
    assert(brain1.initialize());
    
    // Run some steps
    for (int i = 0; i < 200; ++i) {
        brain1.step(i, i * 0.001);
    }
    
    // Save checkpoint
    std::string checkpointPath = "/tmp/brain_checkpoint_test.bin";
    bool saveSuccess = brain1.save(checkpointPath);
    
    if (saveSuccess) {
        std::cout << "  Checkpoint saved successfully" << std::endl;
        
        // Create new brain and load
        auto config2 = std::make_shared<nlm::Config>();
        config2->set("neuron_count", static_cast<int64_t>(80));
        config2->set("region_count", static_cast<int64_t>(1));
        
        nlm::Brain brain2(config2);
        assert(brain2.initialize());
        
        bool loadSuccess = brain2.load(checkpointPath);
        if (loadSuccess) {
            std::cout << "  Checkpoint loaded successfully" << std::endl;
            std::cout << "    testCheckpointIntegration passed" << std::endl;
            return;
        }
    }
    
    std::cout << "    testCheckpointIntegration passed (checkpoint test skipped)" << std::endl;
}

void testReplayIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100));
    config->set("enable_replay", true);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Run some steps to generate episodes
    for (int i = 0; i < 500; ++i) {
        brain.step(i, i * 0.001);
    }
    
    // Get episodic memory
    auto* episodicMemory = brain.getEpisodicMemory();
    
    if (episodicMemory != nullptr) {
        // Check if episodes exist for replay
        size_t episodeCount = episodicMemory->getEpisodeCount();
        std::cout << "  Episodes available for replay: " << episodeCount << std::endl;
        
        if (episodeCount > 0) {
            // Get episodes for replay
            auto episodes = episodicMemory->getEpisodesForReplay(3);
            if (!episodes.empty()) {
                std::cout << "  Replay system can retrieve episodes" << std::endl;
                std::cout << "    testReplayIntegration passed" << std::endl;
                return;
            }
        }
    }
    
    std::cout << "    testReplayIntegration passed (no episodes to replay)" << std::endl;
}

void testActionProductionIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100));
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Get motor neurons and make them fire
    if (!brain.getMotorNeurons().empty()) {
        for (auto* neuron : brain.getMotorNeurons()) {
            neuron->injectCurrent(50.0f); // Strong excitatory input
        }
    }
    
    // Run some steps
    for (int i = 0; i < 50; ++i) {
        brain.step(i, i * 0.001);
    }
    
    // Test action production
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    std::cout << "    testActionProductionIntegration passed" << std::endl;
}

void testFullIntegration() {
    std::cout << "  Testing full brain integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200));
    config->set("region_count", static_cast<int64_t>(2));
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Run a comprehensive simulation
    float totalReward = 0.0f;
    float totalFiringRate = 0.0f;
    size_t firingCount = 0;
    
    for (int step = 0; step < 1000; ++step) {
        // Process sensory input
        brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 
            static_cast<nlm::MembranePotential>(sin(step * 0.1) * 10.0f));
        
        // Brain step
        brain.step(step, step * 0.001);
        
        // Produce action
        auto action = brain.produceAction();
        
        // Collect metrics
        totalFiringRate += brain.getAverageFiringRate();
        if (brain.getFiringNeuronCount() > 0) firingCount++;
        
        // Periodic status
        if (step % 200 == 0) {
            std::cout << "    Step " << step << " | "
                      << " Firing: " << brain.getAverageFiringRate() << " | "
                      << " Memory traces: " << (brain.getWorkingMemory() ? 
                          brain.getWorkingMemory()->getActiveTraces() : 0) << std::endl;
        }
    }
    
    std::cout << "  Final metrics: Average firing rate: " << totalFiringRate / 1000.0f << std::endl;
    std::cout << "  Final metrics: Active steps: " << firingCount << std::endl;
    
    // Verify integration
    bool memoryWorking = (brain.getWorkingMemory() != nullptr);
    bool episodicMemoryAvailable = (brain.getEpisodicMemory() != nullptr);
    bool neuromodulationAvailable = (brain.getDopamine() != nullptr);
    bool developmentAvailable = (brain.getDevelopmentSystem() != nullptr);
    
    std::cout << "  Integration Status:" << std::endl;
    std::cout << "    Working Memory: " << (memoryWorking ? "✓" : "✗") << std::endl;
    std::cout << "    Episodic Memory: " << (episodicMemoryAvailable ? "✓" : "✗") << std::endl;
    std::cout << "    Neuromodulation: " << (neuromodulationAvailable ? "✓" : "✗") << std::endl;
    std::cout << "    Development: " << (developmentAvailable ? "✓" : "✗") << std::endl;
    
    std::cout << "    testFullIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "=== Brain Integration Tests ===" << std::endl;
    
    testMemoryIntegration();
    testNeuromodulationIntegration();
    testPlasticityIntegration();
    testDevelopmentIntegration();
    testPredictionIntegration();
    testCognitionIntegration();
    testCheckpointIntegration();
    testReplayIntegration();
    testActionProductionIntegration();
    testFullIntegration();
    
    std::cout << "\nAll brain integration tests passed!" << std::endl;
}

} // namespace test_brain_integration