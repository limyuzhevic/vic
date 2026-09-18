#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>
#include <algorithm>
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/Memory.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "development/DevelopmentSystem.hpp"
#include "development/Synaptogenesis.hpp"
#include "development/Maturation.hpp"
#include "development/Pruning.hpp"
#include "core/Config/Config.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"

namespace test_integration {

void testCompleteBrainWorldAgentLoop() {
    std::cout << "Testing complete brain-world-agent loop..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("learning_rate", 0.01, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    assert(brain.getTotalNeuronCount() == 50);
    assert(brain.getRegionCount() == 2);
    
    nlm::SensoryInput sensoryInput;
    sensoryInput.addInput(1, 0.5f);
    sensoryInput.addInput(2, 0.3f);
    
    brain.receiveSensoryInput(sensoryInput);
    
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
    }
    
    std::unique_ptr<nlm::Action> action = brain.produceAction();
    assert(action != nullptr);
    
    action->addAction(nlm::MotorCommand::MOVE_FORWARD, 0.5f);
    assert(action->getActionCount() == 1);
    
    assert(brain.getTotalSpikeCount() >= 0);
    assert(brain.getFiringNeuronCount() >= 0);
    
    std::cout << "  PASSED" << std::endl;
}

void testCheckpointing() {
    std::cout << "Testing checkpointing..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(30), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("checkpoint_interval", 10, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    for (nlm::SimulationStep step = 0; step < 25; ++step) {
        brain.step(step);
    }
    
    std::string checkpoint_file = "/tmp/test_checkpoint.bin";
    bool checkpoint_saved = brain.saveToFile(checkpoint_file);
    assert(checkpoint_saved);
    
    assert(std::filesystem::exists(checkpoint_file));
    
    nlm::Brain brain2(config);
    assert(brain2.initialize());
    
    bool checkpoint_loaded = brain2.loadFromFile(checkpoint_file);
    assert(checkpoint_loaded);
    
    assert(brain2.getTotalNeuronCount() == brain.getTotalNeuronCount());
    
    std::filesystem::remove(checkpoint_file);
    
    std::cout << "  PASSED" << std::endl;
}

void testReplayFunctionality() {
    std::cout << "Testing replay functionality..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(20), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("replay_interval", 5, nlm::ConfigSource::Default);
    config->set("replay_strength", 0.5, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    std::vector<nlm::SimulationStep> spikes;
    
    for (nlm::SimulationStep step = 0; step < 15; ++step) {
        brain.step(step);
        
        auto* spike_system = brain.getSpikeSystem();
        if (spike_system) {
            auto pending_spikes = spike_system->getPendingSpikes();
            for (const auto& spike : pending_spikes) {
                spikes.push_back(spike.step);
            }
        }
    }
    
    std::cout << "  PASSED" << std::endl;
}

void testMemoryReplay() {
    std::cout << "Testing memory replay..." << std::endl;
    
    nlm::NeuralWorkingMemory workingMem(10, 100.0f, 0.5f);
    nlm::NeuralEpisodicMemory episodicMem(100, 0.3f, 0.1f);
    
    nlm::MemoryItem item1;
    item1.type = nlm::MemoryItemType::Working;
    item1.content = std::vector<float>{1.0f, 2.0f, 3.0f};
    item1.timestamp = 1000.0f;
    
    workingMem.encode(item1);
    
    nlm::EpisodicEvent event1;
    event1.timestamp = 1001.0f;
    event1.context = std::vector<float>{0.1f, 0.2f};
    event1.outcome = std::vector<float>{1.0f, 0.0f};
    event1.significance = 0.7f;
    
    episodicMem.encode(event1);
    
    nlm::MemoryItem retrieved = workingMem.retrieve(0.0f);
    nlm::EpisodicEvent retrieved_event = episodicMem.retrieve(1001.0f);
    
    assert(retrieved.type == nlm::MemoryItemType::Working);
    assert(retrieved_event.timestamp == 1001.0f);
    
    workingMem.update(50.0f);
    episodicMem.update(100.0f);
    
    std::cout << "  PASSED" << std::endl;
}

void testNeuromodulationIntegration() {
    std::cout << "Testing neuromodulation integration..." << std::endl;
    
    nlm::Brain brain(nullptr);
    
    nlm::Curiosity curiosity(0.7f, 0.3f, 0.1f, 0.5f, 1.0f);
    assert(curiosity.getLevel() == 0.0f);
    
    curiosity.update(0.8f, 1.0f);
    assert(curiosity.getLevel() > 0.0f);
    
    nlm::Novelty novelty_detector(0.5f, 0.8f, 0.01f, 0.05f);
    assert(novelty_detector.getCurrentNovelty() == 0.0f);
    
    nlm::Novelty::NoveltyInput input;
    input.patternId = 123;
    input.intensity = 0.9f;
    input.context = "test_pattern";
    
    novelty_detector.update(input);
    assert(novelty_detector.getCurrentNovelty() > novelty_detector.getThreshold());
    
    nlm::PredictionError prediction_error(0.5f, 0.2f, 0.1f);
    float error = prediction_error.update(0.6f, 0.3f);
    assert(error > prediction_error.getPredictionThreshold());
    
    std::cout << "  PASSED" << std::endl;
}

void testDevelopmentIntegration() {
    std::cout << "Testing development integration..." << std::endl;
    
    nlm::DevelopmentSystem devSystem;
    assert(devSystem.getDevelopmentalStage() == nlm::DevelopmentalStage::Initial);
    
    devSystem.update(10.0f);
    assert(devSystem.getDevelopmentalStage() != nlm::DevelopmentalStage::Initial);
    
    nlm::Synaptogenesis synaptogenesis(0.1f, 0.5f, 0.2f, 0.8f);
    assert(synaptogenesis.getConnectionProbability(0.0f) > 0.0f);
    
    nlm::Maturation maturation(0.3f, 0.9f, 1.0f, 0.1f);
    assert(maturation.getMaturationLevel(0.0f) < maturation.getTargetMaturity());
    
    nlm::Pruning pruning(0.4f, 0.6f, 0.15f, 0.8f);
    assert(pruning.getPruningProbability(0.5f) > 0.0f);
    
    std::cout << "  PASSED" << std::endl;
}

void testSensoryMotorIntegration() {
    std::cout << "Testing sensory-motor integration..." << std::endl;
    
    nlm::SensoryInput sensoryInput;
    sensoryInput.addInput(1, 0.5f);
    sensoryInput.addInput(2, 0.3f);
    
    assert(sensoryInput.getInputCount() == 2);
    
    auto value1 = sensoryInput.getInputValue(1);
    assert(value1.has_value() && *value1 == 0.5f);
    
    auto value2 = sensoryInput.getInputValue(2);
    assert(value2.has_value() && *value2 == 0.3f);
    
    nlm::Action action;
    action.addAction(nlm::MotorCommand::MOVE_FORWARD, 0.5f);
    action.addAction(nlm::MotorCommand::TURN_LEFT, 0.3f);
    
    assert(action.getActionCount() == 2);
    
    std::vector<nlm::MotorCommand> commands = action.getCommands();
    assert(commands.size() == 2);
    
    std::cout << "  PASSED" << std::endl;
}

void runAll() {
    std::cout << "=== Integration Tests ===" << std::endl;
    
    testCompleteBrainWorldAgentLoop();
    testCheckpointing();
    testReplayFunctionality();
    testMemoryReplay();
    testNeuromodulationIntegration();
    testDevelopmentIntegration();
    testSensoryMotorIntegration();
    
    std::cout << "=== All Integration Tests PASSED ===" << std::endl;
}

} // namespace test_integration
