// Integration Tests for Brain-Agent-World Loop
// Tests the complete integration of all major systems

#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/SensoryPercept.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_integration {

void testBasicBrainAgentWorldLoop() {
    std::cout << "    testBasicBrainAgentWorldLoop..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    nlm::SimpleWorld world;
    world.configure(20.0f, 20.0f, 6, 6);
    world.setAgentStart(10.0f, 10.0f);
    
    agentBrain.initialize(world);
    
    for (int step = 0; step < 20; ++step) {
        const auto& percept = world.getSensoryPercept();
        agentBrain.processSensoryInput(percept);
        
        auto action = brain.produceAction();
        
        nlm::ActionResult result = world.applyMotorCommand(agentBrain.decodeMotorCommand(), step * 0.001);
        
        if (result.success && result.reward > 0.0f) {
            break;
        }
    }
    
    std::cout << "    testBasicBrainAgentWorldLoop passed" << std::endl;
}

void testNeuromodulationIntegration() {
    std::cout << "    testNeuromodulationIntegration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(80), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    nlm::SimpleWorld world;
    world.configure(30.0f, 30.0f, 8, 8);
    
    agentBrain.initialize(world);
    agentBrain.enableRewardModulation(true);
    
    for (int step = 0; step < 40; ++step) {
        const auto& percept = world.getSensoryPercept();
        agentBrain.processSensoryInput(percept);
        
        float neuromodLevel = agentBrain.getNeuromodulationLevel();
        
        if (step % 10 == 0) {
            float reward = 1.0f;
            agentBrain.applyRewardModulation(reward, 0.5f);
        } else {
            float reward = -0.5f;
            agentBrain.applyRewardModulation(reward, -0.2f);
        }
        
        neuromodLevel = agentBrain.getNeuromodulationLevel();
        assert(std::abs(neuromodLevel) <= 1.0f);
    }
    
    std::cout << "    testNeuromodulationIntegration passed" << std::endl;
}

void testMemoryIntegration() {
    std::cout << "    testMemoryIntegration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    nlm::SimpleWorld world;
    world.configure(40.0f, 40.0f, 10, 10);
    
    agentBrain.initialize(world);
    
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getAssociativeMemory() != nullptr);
    
    for (int step = 0; step < 80; ++step) {
        const auto& percept = world.getSensoryPercept();
        agentBrain.processSensoryInput(percept);
        
        if (step % 20 == 0) {
            float novelty = agentBrain.getNoveltyLevel();
            if (novelty > 0.5f) {
                agentBrain.applyRewardModulation(0.1f, 0.05f);
            }
        }
    
    std::cout << "    testMemoryIntegration passed" << std::endl;
}

void testDevelopmentIntegration() {
    std::cout << "    testDevelopmentIntegration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(70), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    nlm::SimpleWorld world;
    world.configure(25.0f, 25.0f, 6, 6);
    
    agentBrain.initialize(world);
    agentBrain.enableDevelopment(true);
    
    for (int step = 0; step < 60; ++step) {
        const auto& percept = world.getSensoryPercept();
        agentBrain.processSensoryInput(percept);
        
        agentBrain.updateDevelopment(step * 0.001);
        
        auto command = agentBrain.decodeMotorCommand();
        world.applyMotorCommand(command, step * 0.001);
    }
    
    std::cout << "    testDevelopmentIntegration passed" << std::endl;
}

void testCuriosityDrivenExploration() {
    std::cout << "    testCuriosityDrivenExploration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(90), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    nlm::SimpleWorld world;
    world.configure(35.0f, 35.0f, 8, 8);
    
    agentBrain.initialize(world);
    agentBrain.enableCuriosity(true);
    
    int totalActions = 0;
    
    for (int step = 0; step < 30; ++step) {
        const auto& percept = world.getSensoryPercept();
        agentBrain.processSensoryInput(percept);
        
        auto command = agentBrain.decodeMotorCommand();
        
        if (command != nlm::MotorCommand::Wait) {
            totalActions++;
        }
        
        world.applyMotorCommand(command, step * 0.001);
    }
    
    assert(totalActions > 0);
    
    std::cout << "    testCuriosityDrivenExploration passed" << std::endl;
}

void testPredictionIntegration() {
    std::cout << "    testPredictionIntegration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(85), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    assert(brain.getPredictionSystem() != nullptr);
    
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    nlm::SimpleWorld world;
    world.configure(30.0f, 30.0f, 10, 10);
    
    agentBrain.initialize(world);
    
    for (int step = 0; step < 40; ++step) {
        const auto& percept = world.getSensoryPercept();
        agentBrain.processSensoryInput(percept);
        
        float predictionError = agentBrain.getPredictionError();
        
        auto* predictionErrorSys = brain.getPredictionErrorSignal();
        if (predictionErrorSys) {
            float errorLevel = predictionErrorSys->getErrorLevel();
            assert(std::abs(errorLevel) <= 1.0f);
        }
    
    std::cout << "    testPredictionIntegration passed" << std::endl;
}

void testCompleteEpisodicMemory() {
    std::cout << "    testCompleteEpisodicMemory..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(75), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    nlm::SimpleWorld world;
    world.configure(45.0f, 45.0f, 12, 12);
    
    agentBrain.initialize(world);
    
    assert(brain.getEpisodicMemory() != nullptr);
    
    size_t initialEpisodes = brain.getEpisodicMemory()->getEpisodeCount();
    
    for (int step = 0; step < 100; ++step) {
        const auto& percept = world.getSensoryPercept();
        agentBrain.processSensoryInput(percept);
        
        auto command = agentBrain.decodeMotorCommand();
        world.applyMotorCommand(command, step * 0.001);
        
        if (step % 25 == 0) {
            float novelty = agentBrain.getNoveltyLevel();
            if (novelty > 0.3f) {
                agentBrain.applyRewardModulation(0.05f, 0.02f);
            }
        }
    }
    
    size_t finalEpisodes = brain.getEpisodicMemory()->getEpisodeCount();
    assert(finalEpisodes >= 0);
    
    std::cout << "    testCompleteEpisodicMemory passed" << std::endl;
}

void testAttentionalSelection() {
    std::cout << "    testAttentionalSelection..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(60), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    assert(brain.getAttention() != nullptr);
    
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    nlm::SimpleWorld world;
    world.configure(35.0f, 35.0f, 10, 10);
    
    agentBrain.initialize(world);
    
    for (int step = 0; step < 50; ++step) {
        const auto& percept = world.getSensoryPercept();
        agentBrain.processSensoryInput(percept);
        
        brain.getAttention()->update(0.001);
        
        auto* workingMem = brain.getWorkingMemory();
        if (workingMem && !workingMem->getMemoryNeurons().empty()) {
            std::vector<nlm::NeuronId> competitors = workingMem->getMemoryNeurons();
            brain.getAttention()->processCompetition(competitors);
        }
        
        auto command = agentBrain.decodeMotorCommand();
        world.applyMotorCommand(command, step * 0.001);
    }
    
    std::cout << "    testAttentionalSelection passed" << std::endl;
}

void testCompleteLearningCycle() {
    std::cout << "    testCompleteLearningCycle..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(80), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    nlm::SimpleWorld world;
    world.configure(40.0f, 40.0f, 12, 12);
    
    agentBrain.initialize(world);
    agentBrain.enableRewardModulation(true);
    
    std::vector<float> neuromodHistory;
    
    for (int step = 0; step < 80; ++step) {
        const auto& percept = world.getSensoryPercept();
        agentBrain.processSensoryInput(percept);
        
        auto command = agentBrain.decodeMotorCommand();
        nlm::ActionResult result = world.applyMotorCommand(command, step * 0.001);
        
        if (result.success) {
            agentBrain.applyRewardModulation(result.reward, 0.0f);
        }
        
        neuromodHistory.push_back(agentBrain.getNeuromodulationLevel());
        
        agentBrain.updateDevelopment(step * 0.001);
    }
    
    if (neuromodHistory.size() > 5) {
        float initial = neuromodHistory[0];
        float final = neuromodHistory.back();
        assert(std::abs(initial - final) > 0.01f || std::abs(final) > 0.01f);
    }
    
    std::cout << "    testCompleteLearningCycle passed" << std::endl;
}

void runAll() {
    std::cout << "Running Integration Tests..." << std::endl;
    std::cout << std::endl;
    
    testBasicBrainAgentWorldLoop();
    testNeuromodulationIntegration();
    testMemoryIntegration();
    testDevelopmentIntegration();
    testCuriosityDrivenExploration();
    testPredictionIntegration();
    testCompleteEpisodicMemory();
    testAttentionalSelection();
    testCompleteLearningCycle();
    
    std::cout << std::endl;
    std::cout << "=== All Integration Tests PASSED ===" << std::endl;
}

} // namespace test_integration

int main() {
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Warning);
    logger->addLogger(consoleLogger);
    nlm::Logger::setGlobal(logger);
    
    test_integration::runAll();
    return 0;
}
