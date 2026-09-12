// Brain Tests
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_brain {

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

void testWorkingMemoryEncoding() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* wm = brain.getWorkingMemory();
    assert(wm != nullptr);
    
    // Test pattern encoding
    std::vector<float> pattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    wm->store(pattern, 1.0f);
    
    // Verify retrieval
    std::vector<float> retrieved = wm->retrieve();
    assert(retrieved.size() == pattern.size());
    
    std::cout << "    testWorkingMemoryEncoding passed" << std::endl;
}

void testWorkingMemoryStorage() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* wm = brain.getWorkingMemory();
    assert(wm != nullptr);
    
    // Store multiple patterns
    for (int i = 0; i < 5; ++i) {
        std::vector<float> pattern(10, static_cast<float>(i));
        wm->store(pattern, 1.0f);
    }
    
    // Verify storage capacity
    size_t capacity = wm->getCapacity();
    assert(capacity > 0);
    
    std::cout << "    testWorkingMemoryStorage passed" << std::endl;
}

void testWorkingMemoryRetrieval() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* wm = brain.getWorkingMemory();
    assert(wm != nullptr);
    
    // Store a specific pattern
    std::vector<float> original = {0.9f, 0.1f, 0.5f, 0.3f, 0.7f};
    wm->store(original, 1.0f);
    
    // Retrieve and compare
    std::vector<float> retrieved = wm->retrieve();
    assert(retrieved.size() == original.size());
    
    // Check individual activations
    for (size_t i = 0; i < original.size() && i < retrieved.size(); ++i) {
        assert(std::abs(retrieved[i] - original[i]) < 0.01f);
    }
    
    std::cout << "    testWorkingMemoryRetrieval passed" << std::endl;
}

void testWorkingMemoryCompetition() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* wm = brain.getWorkingMemory();
    assert(wm != nullptr);
    
    // Store multiple patterns with different strengths
    std::vector<float> pattern1 = {1.0f, 0.5f, 0.2f};
    std::vector<float> pattern2 = {0.1f, 0.9f, 0.3f};
    
    wm->store(pattern1, 2.0f);  // Stronger pattern
    wm->store(pattern2, 0.5f);  // Weaker pattern
    
    // Run competition to select winners
    wm->runCompetition();
    
    // Verify competition occurred (check for winners)
    const auto& winners = wm->getMemoryNeurons();
    assert(!winners.empty());
    
    std::cout << "    testWorkingMemoryCompetition passed" << std::endl;
}

void testPredictionSystemAccuracy() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* pred = brain.getPredictionSystem();
    assert(pred != nullptr);
    
    // Get prediction error and confidence
    float error = pred->getPredictionError();
    float confidence = pred->getConfidence();
    
    // Initial error should be low, confidence high
    assert(std::abs(error) < 1.0f);
    assert(confidence >= 0.0f && confidence <= 1.0f);
    
    std::cout << "    testPredictionSystemAccuracy passed" << std::endl;
}

void testPredictionSystemErrorCalculation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* pred = brain.getPredictionSystem();
    assert(pred != nullptr);
    
    // Get history and error
    const auto& history = pred->getErrorHistory();
    float error = pred->getPredictionError();
    
    // Update with known prediction and actual
    pred->updatePredictions(std::make_unique<nlm::InternalSignals>(), std::make_unique<nlm::InternalSignals>());
    
    // Verify error calculation
    float newError = pred->getPredictionError();
    assert(std::isfinite(newError));
    
    std::cout << "    testPredictionSystemErrorCalculation passed" << std::endl;
}

void testConceptFormationDiscovery() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* cf = brain.getConceptFormation();
    assert(cf != nullptr);
    
    // Present novel patterns
    std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> pattern2 = {0.9f, 0.8f, 0.7f, 0.6f, 0.5f};
    
    size_t conceptId1 = cf->presentExperience(pattern1, pattern1, 0.5f, 0);
    size_t conceptId2 = cf->presentExperience(pattern2, pattern2, 0.8f, 1);
    
    // Verify concept discovery
    assert(cf->getConceptCount() > 0);
    
    std::cout << "    testConceptFormationDiscovery passed" << std::endl;
}

void testConceptFormationStability() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* cf = brain.getConceptFormation();
    assert(cf != nullptr);
    
    // Create a concept
    std::vector<float> pattern = {0.3f, 0.5f, 0.7f, 0.9f};
    size_t conceptId = cf->presentExperience(pattern, pattern, 0.6f, 0);
    
    if (conceptId > 0) {
        // Update concept to check stability
        cf->updateConcept(conceptId, pattern, pattern, 0.1f);
        
        // Check stability
        float stability = cf->getConceptStability(conceptId);
        assert(stability >= 0.0f && stability <= 1.0f);
    }
    
    std::cout << "    testConceptFormationStability passed" << std::endl;
}

void testNeuromodulationDynamics() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test dopamine system
    auto* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    float initialLevel = dopamine->getLevel();
    assert(std::isfinite(initialLevel));
    
    // Test novelty detection
    auto* novelty = brain.getNovelty();
    assert(novelty != nullptr);
    
    std::cout << "    testNeuromodulationDynamics passed" << std::endl;
}

void testNeuromodulatorInteractions() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get all neuromodulators
    auto* dopamine = brain.getDopamine();
    auto* acetylcholine = brain.getAcetylcholine();
    auto* curiosity = brain.getCuriosity();
    auto* predictionError = brain.getPredictionErrorSignal();
    
    assert(dopamine != nullptr);
    assert(acetylcholine != nullptr);
    assert(curiosity != nullptr);
    assert(predictionError != nullptr);
    
    // Check levels are in valid range
    float daLevel = dopamine->getLevel();
    float achLevel = acetylcholine->getLevel();
    
    assert(daLevel >= 0.0f && daLevel <= 10.0f);
    assert(achLevel >= 0.0f && achLevel <= 10.0f);
    
    std::cout << "    testNeuromodulatorInteractions passed" << std::endl;
}

void testCheckpointSaveLoad() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("checkpoint_dir", "./test_checkpoints", nlm::ConfigSource::Default);
    
    nlm::Brain brain1(config);
    brain1.initialize();
    
    // Run a few steps to create activity
    for (nlm::SimulationStep step = 0; step < 5; ++step) {
        brain1.step(step);
    }
    
    // Save brain state
    std::string checkpointFile = "./test_checkpoint.brain";
    bool saved = brain1.save(checkpointFile);
    assert(saved);
    
    // Create a new brain and load the checkpoint
    nlm::Brain brain2(config);
    brain2.initialize();
    
    bool loaded = brain2.load(checkpointFile);
    assert(loaded);
    
    // Verify loaded state
    assert(brain2.getTotalNeuronCount() == brain1.getTotalNeuronCount());
    assert(brain2.getTotalSynapseCount() == brain1.getTotalSynapseCount());
    
    std::cout << "    testCheckpointSaveLoad passed" << std::endl;
}

void testIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run full integration test
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
        
        // Verify all systems are working
        assert(brain.getWorkingMemory() != nullptr);
        assert(brain.getEpisodicMemory() != nullptr);
        assert(brain.getPredictionSystem() != nullptr);
        assert(brain.getConceptFormation() != nullptr);
        assert(brain.getDopamine() != nullptr);
        assert(brain.getAcetylcholine() != nullptr);
        assert(brain.getCuriosity() != nullptr);
        
        // Check some basic statistics
        assert(brain.getTotalNeuronCount() > 0);
        assert(brain.getTotalSynapseCount() >= 0);
    }
    
    // Test action production
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    // Test reset
    brain.reset();
    assert(brain.getTotalNeuronCount() > 0);
    
    std::cout << "    testIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running Brain tests..." << std::endl;
    testBrainCreation();
    testBrainInitialization();
    testBrainStep();
    testBrainReset();
    testBrainRegions();
    testBrainActionProduction();
    testBrainStatus();
    
    std::cout << "Running Working Memory tests..." << std::endl;
    testWorkingMemoryEncoding();
    testWorkingMemoryStorage();
    testWorkingMemoryRetrieval();
    testWorkingMemoryCompetition();
    
    std::cout << "Running Prediction System tests..." << std::endl;
    testPredictionSystemAccuracy();
    testPredictionSystemErrorCalculation();
    
    std::cout << "Running Concept Formation tests..." << std::endl;
    testConceptFormationDiscovery();
    testConceptFormationStability();
    
    std::cout << "Running Neuromodulation tests..." << std::endl;
    testNeuromodulationDynamics();
    testNeuromodulatorInteractions();
    
    std::cout << "Running Checkpoint tests..." << std::endl;
    testCheckpointSaveLoad();
    
    std::cout << "Running Integration tests..." << std::endl;
    testIntegration();
}

} // namespace test_brain

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

void runAll() {
    testBrainCreation();
    testBrainInitialization();
    testBrainStep();
    testBrainReset();
    testBrainRegions();
    testBrainActionProduction();
    testBrainStatus();
}

} // namespace test_brain
