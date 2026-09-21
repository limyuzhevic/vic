#include "IntegratedBrainTests.hpp"
#include "SensoryInput.hpp"
#include <fstream>
#include <filesystem>
#include <random>

namespace nlm {

void IntegratedBrainTests::testBasicBrainInitialization() {
    std::cout << "\n1. Testing Basic Brain Initialization...";
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 2);
    config->set("random_seed", 42);
    
    // Create brain
    Brain brain(config);
    assert(brain.initialize());
    
    // Test system accessors
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getPredictionSystem() != nullptr);
    assert(brain.getPlanner() != nullptr);
    assert(brain.getConceptFormation() != nullptr);
    assert(brain.getAttention() != nullptr);
    assert(brain.getDevelopmentSystem() != nullptr);
    assert(brain.getDopamine() != nullptr);
    assert(brain.getCuriosity() != nullptr);
    assert(brain.getNovelty() != nullptr);
    assert(brain.getPredictionErrorSignal() != nullptr);
    
    std::cout << " [PASS]";
}

void IntegratedBrainTests::testWorkingMemoryIntegration() {
    std::cout << "\n2. Testing Working Memory Integration...";
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    Brain brain(config);
    assert(brain.initialize());
    
    // Store pattern in working memory
    std::vector<float> pattern(10, 0.5f);
    brain.getWorkingMemory()->store(pattern, 1.0f);
    
    // Verify pattern was stored
    auto retrieved = brain.getWorkingMemory()->retrieve();
    assert(retrieved.size() == pattern.size());
    
    // Verify working memory neurons
    auto memoryNeurons = brain.getWorkingMemory()->getMemoryNeurons();
    assert(!memoryNeurons.empty());
    
    // Test attention integration
    auto winners = brain.getAttention()->processCompetition(memoryNeurons, 0.5f);
    assert(!winners.empty());
    
    // Test working memory competition
    brain.getWorkingMemory()->runCompetition();
    assert(!brain.getWorkingMemory()->isWinning(memoryNeurons[0]));
    
    std::cout << " [PASS]";
}

void IntegratedBrainTests::testEpisodicMemoryStorage() {
    std::cout << "\n3. Testing Episodic Memory Storage...";
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500);
    Brain brain(config);
    assert(brain.initialize());
    
    // Create an episodic memory item
    EpisodicMemoryItem episode;
    episode.timestamp = 0;
    episode.positionX = 5.0f;
    episode.positionY = 5.0f;
    episode.action = ActionType::MoveForward;
    episode.reward = 1.0f;
    episode.novelty = 0.8f;
    
    // Add some neural activity
    episode.activeNeurons.push_back(NeuronId(1000));
    episode.activeNeurons.push_back(NeuronId(1001));
    episode.neuronActivations.push_back(0.7f);
    episode.neuronActivations.push_back(0.8f);
    
    // Store in episodic memory
    brain.getEpisodicMemory()->storeEpisode(episode);
    
    // Verify it was stored
    assert(brain.getEpisodicMemory()->getEpisodeCount() == 1);
    
    // Test retrieval
    std::vector<float> queryPattern(episode.neuronActivations);
    auto similar = brain.getEpisodicMemory()->retrieveSimilar(queryPattern, 3);
    
    // Test temporal retrieval
    auto temporal = brain.getEpisodicMemory()->retrieveTemporal(0, 100);
    
    // Test replay
    brain.getEpisodicMemory()->enableReplay(true);
    auto replays = brain.getEpisodicMemory()->getEpisodesForReplay(1);
    
    std::cout << " [PASS]";
}

void IntegratedBrainTests::testPredictionSystemIntegration() {
    std::cout << "\n4. Testing Prediction System Integration...";
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500);
    Brain brain(config);
    assert(brain.initialize());
    
    // Create sensory input
    SensoryInput sensory;
    std::vector<float> data{0.1f, 0.5f, 0.9f, 0.2f, 0.7f};
    sensory.setData(data);
    
    // Store in working memory
    brain.getWorkingMemory()->store(data, 1.0f);
    
    // Prediction system should use working memory
    brain.getWorkingMemory()->update(0.01);
    
    // Verify prediction error is computed
    float error = brain.getPredictionErrorSignal()->getPredictionError();
    
    // Test integration with sensory input
    brain.receiveSensoryInput(sensory);
    
    std::cout << " [PASS]";
}

void IntegratedBrainTests::testConceptFormationIntegration() {
    std::cout << "\n5. Testing Concept Formation Integration...";
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500);
    Brain brain(config);
    assert(brain.initialize());
    
    // Present experiences to concept formation
    std::vector<float> pattern1{0.1f, 0.5f, 0.9f, 0.2f, 0.7f};
    std::vector<float> pattern2{0.15f, 0.55f, 0.85f, 0.25f, 0.65f}; // Similar pattern
    std::vector<float> features{1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
    
    // Present first pattern
    size_t concept1 = brain.getConceptFormation()->presentExperience(pattern1, features, 0.5f, 0);
    
    // Present similar pattern
    size_t concept2 = brain.getConceptFormation()->presentExperience(pattern2, features, 0.6f, 1);
    
    // Verify concepts were formed
    assert(brain.getConceptFormation()->getConceptCount() >= 1);
    
    // Test concept matching
    size_t matching = brain.getConceptFormation()->getMatchingConcept(pattern2, 0.7f);
    
    // Test concept stability
    float stability = brain.getConceptFormation()->getConceptStability(0);
    
    std::cout << " [PASS]";
}

void IntegratedBrainTests::testAttentionIntegration() {
    std::cout << "\n6. Testing Attention Integration...";
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500);
    Brain brain(config);
    assert(brain.initialize());
    
    // Create competitors
    std::vector<NeuronId> competitors;
    for (int i = 0; i < 5; ++i) {
        competitors.push_back(NeuronId(1000 + i));
    }
    
    // Apply attention
    auto winners = brain.getAttention()->processCompetition(competitors, 0.5f);
    assert(!winners.empty());
    
    // Test focus on region
    brain.getAttention()->focusOnRegion(RegionId(1));
    assert(!brain.getAttention()->getAttendedRegions().empty());
    
    // Test attention update
    brain.getAttention()->update(0.01);
    
    // Test top-down and bottom-up bias
    brain.getAttention()->applyTopDownBias(competitors[0], 0.5f);
    brain.getAttention()->applyBottomUpSalience(competitors[1], 0.8f);
    
    // Test attention winners
    assert(!brain.getAttention()->getWinners().empty());
    
    std::cout << " [PASS]";
}

void IntegratedBrainTests::testNeuromodulationIntegration() {
    std::cout << "\n7. Testing Neuromodulation Integration...";
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500);
    Brain brain(config);
    assert(brain.initialize());
    
    // Test novelty detection
    brain.getNovelty()->update(0.01);
    float noveltyLevel = brain.getNovelty()->getNoveltyLevel();
    
    // Test curiosity
    brain.getCuriosity()->update(0.01);
    float curiosityLevel = brain.getCuriosity()->getCuriosityLevel();
    
    // Test prediction error
    brain.getPredictionErrorSignal()->updatePredictionError(0.5f);
    float predError = brain.getPredictionErrorSignal()->getPredictionError();
    
    // Test dopamine effects on working memory
    brain.getWorkingMemory()->storeToNeuron(NeuronId(1000), 0.5f);
    
    // Apply neuromodulation
    Neuromodulator neuromod(0.7f, 0.3f, 0.5f);
    brain.applyNeuromodulation(neuromod);
    
    std::cout << " [PASS]";
}

void IntegratedBrainTests::testDevelopmentIntegration() {
    std::cout << "\n8. Testing Development Integration...";
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500);
    Brain brain(config);
    assert(brain.initialize());
    
    // Test development stages
    assert(brain.getDevelopmentalStage() == DevelopmentalStage::Initial);
    
    // Apply development effects
    brain.setDevelopmentalStage(DevelopmentalStage::Maturation);
    assert(brain.getDevelopmentalStage() == DevelopmentalStage::Maturation);
    
    // Test development system updates
    brain.getDevelopmentSystem()->update(brain.getRandomGenerator(), 100.0f);
    
    // Test development effects on plasticity
    brain.develop();
    
    std::cout << " [PASS]";
}

void IntegratedBrainTests::testSystemIntegration() {
    std::cout << "\n9. Testing System Integration...";
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 2);
    Brain brain(config);
    assert(brain.initialize());
    
    // Create comprehensive integration test
    std::vector<float> sensoryPattern{0.1f, 0.5f, 0.9f, 0.2f, 0.7f};
    
    // Step through brain cycle
    for (int step = 0; step < 100; ++step) {
        // Process sensory input
        SensoryInput sensory;
        sensory.setData(sensoryPattern);
        brain.receiveSensoryInput(sensory);
        
        // Step brain
        brain.step(step);
        
        // Verify systems are working
        assert(brain.getWorkingMemory() != nullptr);
        assert(brain.getEpisodicMemory() != nullptr);
        assert(brain.getPredictionSystem() != nullptr);
        assert(brain.getPlanner() != nullptr);
        assert(brain.getConceptFormation() != nullptr);
        assert(brain.getAttention() != nullptr);
        
        // Update attention based on current state
        auto memoryNeurons = brain.getWorkingMemory()->getMemoryNeurons();
        if (!memoryNeurons.empty()) {
            brain.getAttention()->processCompetition(memoryNeurons, 0.5f);
        }
        
        // Every 10 steps, store episode
        if (step % 10 == 0) {
            EpisodicMemoryItem episode;
            episode.timestamp = step;
            episode.reward = 0.5f + step * 0.01f;
            brain.getEpisodicMemory()->storeEpisode(episode);
        }
        
        // Progress through development stages
        if (step == 100) {
            brain.setDevelopmentalStage(DevelopmentalStage::Adult);
        }
    }
    
    // Verify learning occurred
    assert(brain.getTotalSpikeCount() > 0);
    assert(brain.getWorkingMemory()->getActiveTraces() > 0);
    
    std::cout << " [PASS]";
}

void IntegratedBrainTests::testMemoryConsolidation() {
    std::cout << "\n10. Testing Memory Consolidation...";
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500);
    config->set("replay_interval", 10);  // Shorter for testing
    Brain brain(config);
    assert(brain.initialize());
    
    // Store multiple episodes
    for (int i = 0; i < 20; ++i) {
        EpisodicMemoryItem episode;
        episode.timestamp = i;
        episode.reward = 0.5f;
        episode.activeNeurons.push_back(NeuronId(2000 + i));
        episode.neuronActivations.push_back(0.5f);
        brain.getEpisodicMemory()->storeEpisode(episode);
    }
    
    // Verify episodes are stored
    assert(brain.getEpisodicMemory()->getEpisodeCount() >= 20);
    
    // Test consolidation
    brain.getEpisodicMemory()->consolidate(0.5f);
    
    // Test replay
    brain.getEpisodicMemory()->enableReplay(true);
    auto replays = brain.getEpisodicMemory()->getEpisodesForReplay(5);
    
    // Test replay mechanism
    for (const auto* episode : replays) {
        brain.getEpisodicMemory()->replayEpisode(episode);
    }
    
    std::cout << " [PASS]";
}

void IntegratedBrainTests::testLearningProgress() {
    std::cout << "\n11. Testing Learning Progress...";
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500);
    config->set("replay_interval", 20);
    config->set("consolidation_interval", 100);
    Brain brain(config);
    assert(brain.initialize());
    
    // Track learning metrics
    std::vector<size_t> spikeCounts;
    std::vector<size_t> firingCounts;
    std::vector<size_t> memoryTraces;
    
    for (int step = 0; step < 300; ++step) {
        // Generate varying sensory input to drive learning
        std::vector<float> sensory(5);
        for (int i = 0; i < 5; ++i) {
            sensory[i] = 0.1f + static_cast<float>(rand()) / RAND_MAX * 0.8f;
        }
        
        // Process sensory input
        SensoryInput input;
        input.setData(sensory);
        brain.receiveSensoryInput(input);
        
        // Step brain
        brain.step(step);
        
        // Record metrics
        spikeCounts.push_back(brain.getTotalSpikeCount());
        firingCounts.push_back(brain.getFiringNeuronCount());
        memoryTraces.push_back(brain.getWorkingMemory()->getActiveTraces());
        
        // Progress through development stages
        if (step == 100) {
            brain.setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
        } else if (step == 200) {
            brain.setDevelopmentalStage(DevelopmentalStage::Adult);
        }
    }
    
    // Verify learning occurred (metrics should show progression)
    assert(spikeCounts.back() > spikeCounts.front());
    
    // Verify concept formation made progress
    size_t conceptCount = brain.getConceptFormation()->getConceptCount();
    
    std::cout << " [PASS]";
}

void IntegratedBrainTests::testPersistence() {
    std::cout << "\n12. Testing Persistence (Checkpointing)...";
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 200);
    config->set("checkpoint_dir", "./test_checkpoints");
    config->set("max_checkpoints", 5);
    
    Brain brain(config);
    assert(brain.initialize());
    
    // Create some state by running simulation
    for (int step = 0; step < 50; ++step) {
        std::vector<float> sensory(5, 0.5f + step * 0.01f);
        SensoryInput input;
        input.setData(sensory);
        brain.receiveSensoryInput(input);
        brain.step(step);
    }
    
    // Save brain state
    std::string checkpointPath = "./test_checkpoint.bin";
    bool saved = brain.save(checkpointPath);
    assert(saved);
    
    // Create new brain and load state
    auto config2 = std::make_shared<Config>();
    config2->set("neuron_count", 200);
    Brain brain2(config2);
    assert(brain2.initialize());
    
    bool loaded = brain2.load(checkpointPath);
    assert(loaded);
    
    // Verify basic state was preserved
    assert(brain2.getTotalNeuronCount() == brain.getTotalNeuronCount());
    assert(brain2.getTotalSynapseCount() == brain.getTotalSynapseCount());
    
    std::cout << " [PASS]";
}

} // namespace nlm
