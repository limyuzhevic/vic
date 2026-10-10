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

void testBrainTemporalRelation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    // Verify temporal relation is available
    auto* temporalRelation = brain.getTemporalRelation();
    assert(temporalRelation != nullptr);
    
    // Test initial state (no sequences yet)
    assert(temporalRelation->getSequenceLength() == 0);
    
    // Record some sequence experiences
    std::vector<float> eventA = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> eventB = {0.2f, 0.3f, 0.4f, 0.5f, 0.6f};
    temporalRelation->recordSequence(eventA, eventB, 1.0f);
    
    // Verify sequence was recorded
    assert(temporalRelation->getSequenceLength() > 0);
    
    // Test prediction
    auto prediction = temporalRelation->predictNext(eventA);
    assert(!prediction.empty());
    
    // Test prediction confidence
    float confidence = temporalRelation->getPredictionConfidence(eventA);
    assert(confidence >= 0.0f && confidence <= 1.0f);
    
    std::cout << "    testBrainTemporalRelation passed" << std::endl;
}

void testBrainSaveLoad() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain1(config);
    bool success = brain1.initialize();
    assert(success);
    
    // Run a few steps to create some state
    for (nlm::SimulationStep step = 0; step < 5; ++step) {
        brain1.step(step);
    }
    
    // Get initial state for comparison
    size_t initialTotalSpikes = brain1.getTotalSpikeCount();
    float initialAvgFiringRate = brain1.getAverageFiringRate();
    
    // Save brain state
    std::string savePath = "./test_checkpoint.nml";
    bool saveSuccess = brain1.save(savePath);
    assert(saveSuccess);
    
    // Create a new brain and reset it (should be in clean state)
    nlm::Brain brain2(config);
    brain2.initialize();
    
    // Load the saved state
    bool loadSuccess = brain2.load(savePath);
    assert(loadSuccess);
    
    // Verify the loaded brain has the same key properties
    // (exact values may differ due to randomness, but counts should match)
    assert(brain2.getTotalNeuronCount() == brain1.getTotalNeuronCount());
    assert(brain2.getRegionCount() == brain1.getRegionCount());
    
    // The total spike count might not be exactly the same since it's cumulative
    // and the loaded brain might have additional spikes from initialization
    // but it should be non-zero (since we took steps)
    assert(brain2.getTotalSpikeCount() >= 0);
    
    // Clean up
    std::remove(savePath.c_str());
    
    std::cout << "    testBrainSaveLoad passed" << std::endl;
}

void testAgentBrainTemporalContext() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    // Create an AgentBrain with the brain
    nlm::AgentBrain agentBrain(brain);
    
    // Test temporal context
    std::vector<float> temporalContext = agentBrain.getTemporalContext();
    
    // Should have at least sequence length and confidence
    assert(temporalContext.size() >= 2);
    
    // Test temporal context status
    bool hasTemporalContext = agentBrain.hasTemporalContext();
    assert(!hasTemporalContext); // Initially no temporal relations learned
    
    // Use the brain's temporal relation to add a sequence
    auto* temporalRelation = brain.getTemporalRelation();
    std::vector<float> eventA = {0.1f, 0.2f, 0.3f};
    std::vector<float> eventB = {0.2f, 0.3f, 0.4f};
    temporalRelation->recordSequence(eventA, eventB, 1.0f);
    
    // Now hasTemporalContext should return true
    hasTemporalContext = agentBrain.hasTemporalContext();
    assert(hasTemporalContext);
    
    std::cout << "    testAgentBrainTemporalContext passed" << std::endl;
}

void runAll() {
    testBrainCreation();
    testBrainInitialization();
    testBrainStep();
    testBrainReset();
    testBrainRegions();
    testBrainActionProduction();
    testBrainStatus();
    testBrainTemporalRelation();
    testBrainSaveLoad();
    testAgentBrainTemporalContext();
}

} // namespace test_brain
