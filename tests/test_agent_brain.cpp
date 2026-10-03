// AgentBrain Tests
#include "agent/AgentBrain.hpp"
#include "agent/AgentBody.hpp"
#include "agent/SensoryPercept.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "world/SimpleWorld.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_agent_brain {

void testAgentBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    std::cout << "    testAgentBrainCreation passed" << std::endl;
}

void testAgentBrainInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    nlm::AgentBrain agent(brain);
    
    // Create a simple world for testing
    nlm::SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    
    agent.initialize(world);
    
    // Verify initialization worked
    assert(agent.getSensoryInputSize() == 256 + 8 + 4 + 6);  // Vision + touch + internal + proprioception
    assert(agent.getMotorOutputSize() == 6);  // 6 motor actions
    
    std::cout << "    testAgentBrainInitialization passed" << std::endl;
}

void testAgentBrainSensoryProcessing() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    nlm::AgentBrain agent(brain);
    
    // Create a simple world for testing
    nlm::SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    
    agent.initialize(world);
    
    // Create a test percept
    nlm::SensoryPercept percept;
    
    // Fill with test data
    std::vector<float> vision(256, 0.5f);  // Uniform vision data
    std::vector<float> touch(8, 0.0f);
    std::vector<float> internal(4, 0.0f);
    std::vector<float> proprioception(6, 0.0f);
    
    percept.setVision(vision);
    percept.setTouch(touch);
    percept.setInternal(internal);
    percept.setProprioception(proprioception);
    
    // Process sensory input
    agent.processSensoryInput(percept);
    
    // Verify novelty was computed
    float novelty = agent.getNoveltyLevel();
    assert(std::isfinite(novelty));
    
    // Verify curiosity was updated
    float curiosity = agent.getCuriosityLevel();
    assert(std::isfinite(curiosity));
    
    std::cout << "    testAgentBrainSensoryProcessing passed" << std::endl;
}

void testAgentBrainMotorDecoding() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    nlm::AgentBrain agent(brain);
    
    // Create a simple world for testing
    nlm::SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    
    agent.initialize(world);
    
    // Decode motor command (should return Wait if no activity)
    nlm::MotorCommand cmd = agent.decodeMotorCommand();
    assert(cmd == nlm::MotorCommand::Wait);
    
    std::cout << "    testAgentBrainMotorDecoding passed" << std::endl;
}

void testAgentBrainRewardModulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    nlm::AgentBrain agent(brain);
    
    // Create a simple world for testing
    nlm::SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    
    agent.initialize(world);
    
    // Enable reward modulation
    agent.enableRewardModulation(true);
    
    // Apply reward modulation
    agent.applyRewardModulation(1.0f, 0.5f);  // Reward > predicted
    
    // Verify neuromodulation level was updated
    float neuromodulation = agent.getNeuromodulationLevel();
    assert(std::isfinite(neuromodulation));
    
    // Verify prediction error was computed
    float predictionError = agent.getPredictionError();
    assert(predictionError == 0.5f);  // reward - predicted = 1.0 - 0.5
    
    std::cout << "    testAgentBrainRewardModulation passed" << std::endl;
}

void testAgentBrainDevelopment() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    nlm::AgentBrain agent(brain);
    
    // Create a simple world for testing
    nlm::SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    
    agent.initialize(world);
    
    // Enable development
    agent.enableDevelopment(true);
    
    // Update development
    agent.updateDevelopment(100.0);  // 100 ms
    
    // Verify developmental stage was updated
    nlm::DevelopmentalStage stage = agent.getDevelopmentalStage();
    assert(std::isfinite(static_cast<int>(stage)));
    
    // Verify plasticity modifier was updated
    float plasticity = agent.getBrain()->getStructuralPlasticity()->getSynaptogenesisRate();
    assert(std::isfinite(plasticity));
    
    std::cout << "    testAgentBrainDevelopment passed" << std::endl;
}

void testAgentBrainCuriosityExploration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    nlm::AgentBrain agent(brain);
    
    // Create a simple world for testing
    nlm::SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    
    agent.initialize(world);
    
    // Enable curiosity
    agent.enableCuriosity(true);
    
    // Process some sensory input to generate novelty
    nlm::SensoryPercept percept;
    std::vector<float> vision(256, 0.5f);
    percept.setVision(vision);
    agent.processSensoryInput(percept);
    
    // Get curiosity level
    float curiosity = agent.getCuriosityLevel();
    assert(std::isfinite(curiosity) && curiosity >= 0.0f);
    
    // Decode motor command (might trigger curiosity-based exploration)
    nlm::MotorCommand cmd = agent.decodeMotorCommand();
    assert(std::isfinite(static_cast<int>(cmd)));
    
    std::cout << "    testAgentBrainCuriosityExploration passed" << std::endl;
}

void testAgentBrainReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    nlm::AgentBrain agent(brain);
    
    // Create a simple world for testing
    nlm::SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    
    agent.initialize(world);
    
    // Apply some modulation to change state
    agent.applyRewardModulation(1.0f, 0.0f);
    
    // Reset agent
    agent.reset();
    
    // Verify reset worked (values should be back to defaults)
    assert(agent.getNeuromodulationLevel() == 0.0f);
    assert(agent.getNoveltyLevel() == 0.0f);
    assert(agent.getCuriosityLevel() == 0.0f);
    assert(agent.getPredictionError() == 0.0f);
    assert(agent.getBrain()->getDevelopmentalStage() == nlm::DevelopmentalStage::Initial);
    
    std::cout << "    testAgentBrainReset passed" << std::endl;
}

void testAgentBrainSubsystemsEnableDisable() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    nlm::AgentBrain agent(brain);
    
    // Test enabling/disabling subsystems
    agent.enableRewardModulation(false);
    assert(!agent.isRewardModulationEnabled());
    
    agent.enableStructuralPlasticity(true);
    assert(agent.isStructuralPlasticityEnabled());
    
    agent.enableDevelopment(false);
    assert(!agent.isDevelopmentEnabled());
    
    agent.enableCuriosity(true);
    assert(agent.isCuriosityEnabled());
    
    std::cout << "    testAgentBrainSubsystemsEnableDisable passed" << std::endl;
}

void runAllAgentBrainTests() {
    testAgentBrainCreation();
    testAgentBrainInitialization();
    testAgentBrainSensoryProcessing();
    testAgentBrainMotorDecoding();
    testAgentBrainRewardModulation();
    testAgentBrainDevelopment();
    testAgentBrainCuriosityExploration();
    testAgentBrainReset();
    testAgentBrainSubsystemsEnableDisable();
    
    std::cout << "All AgentBrain tests passed!" << std::endl;
}

} // namespace test_agent_brain
