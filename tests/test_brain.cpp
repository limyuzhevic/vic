// Enhanced Brain Tests
// Now includes comprehensive tests for brain initialization, plasticity, and advanced features
// #include "brain/Brain.hpp"
// #include "core/Config/Config.hpp"
// #include <cassert>
// #include <iostream>

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
    
    // Test action production
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    // Brain should produce either MoveForward or Wait
    assert(action->getType() == nlm::ActionType::MoveForward || 
           action->getType() == nlm::ActionType::Wait);
    
    std::cout << "    testBrainActionProduction passed" << std::endl;
}

void testBrainSensoryInput() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create a simple sensory input
    nlm::SensoryInput input;
    std::vector<double> data = {0.5, 0.8, 0.3};
    input.setData(data);
    
    brain.receiveSensoryInput(input);
    
    // Just verify it doesn't crash
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "    testBrainSensoryInput passed" << std::endl;
}

void testBrainPlasticity() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.2, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Enable plasticity on all synapses
    auto* region = brain.getRegion(nlm::RegionId(1));
    if (region) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
    }
    
    // Record initial weights
    std::vector<float> initialWeights;
    if (region) {
        for (const auto& syn : region->getSynapses()) {
            initialWeights.push_back(syn->getWeight());
        }
    }
    
    // Apply some input to trigger plasticity
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 20.0f);
        brain.step(step);
    }
    
    // Record final weights
    std::vector<float> finalWeights;
    if (region) {
        for (const auto& syn : region->getSynapses()) {
            finalWeights.push_back(syn->getWeight());
        }
    }
    
    // Check that some weights changed (plasticity occurred)
    bool weightsChanged = false;
    for (size_t i = 0; i < std::min(initialWeights.size(), finalWeights.size()); ++i) {
        if (std::abs(initialWeights[i] - finalWeights[i]) > 0.001f) {
            weightsChanged = true;
            break;
        }
    }
    
    // Plasticity should have occurred
    assert(weightsChanged);
    
    std::cout << "    testBrainPlasticity passed" << std::endl;
}

void testBrainCheckpointing() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(20), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create a checkpoint file
    std::string checkpointFile = "test_checkpoint.bin";
    bool saveSuccess = brain.save(checkpointFile);
    assert(saveSuccess);
    
    // Create a new brain and load the checkpoint
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", static_cast<int64_t>(20), nlm::ConfigSource::Default);
    config2->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain2(config2);
    brain2.initialize();
    
    bool loadSuccess = brain2.load(checkpointFile);
    assert(loadSuccess);
    
    // Clean up
    std::remove(checkpointFile.c_str());
    
    std::cout << "    testBrainCheckpointing passed" << std::endl;
}

void runAll() {
    testBrainCreation();
    testBrainInitialization();
    testBrainStep();
    testBrainReset();
    testBrainRegions();
    testBrainActionProduction();
    testBrainSensoryInput();
    testBrainPlasticity();
    testBrainCheckpointing();
    
    std::cout << std::endl;
    std::cout << "=== Brain Tests ===" << std::endl;
    std::cout << "All brain tests passed!" << std::endl;
}

} // namespace test_brain