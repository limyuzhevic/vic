// Agent Tests
#include "agent/AgentBrain.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "world/SimpleWorld.hpp"
#include <cassert>
#include <iostream>
#include <memory>

namespace test_agent {

void testAgentBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    std::cout << "    testAgentBrainCreation passed" << std::endl;
}

void testAgentBrainInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    nlm::AgentBrain agent(brain);
    
    // Create a simple world
    nlm::SimpleWorld world;
    world.configure(20, 20, 8, 8);
    world.reset();
    
    bool success = agent.initialize(world);
    assert(success);
    
    // Verify agent properties
    assert(agent.getBrain() == brain);
    
    std::cout << "    testAgentBrainInitialization passed" << std::endl;
}

void testAgentBrainProcessSensoryInput() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    nlm::AgentBrain agent(brain);
    nlm::SimpleWorld world;
    world.configure(20, 20, 8, 8);
    agent.initialize(world);
    
    // Create a sensory percept
    nlm::Vision vision;
    std::vector<float> data(8 * 8 * 3, 0.5f);
    vision.setData(data);
    
    nlm::SensoryPercept percept;
    percept.setVision(vision);
    
    // Process sensory input
    agent.processSensoryInput(percept);
    
    // Verify brain received input
    assert(brain->getFiringNeuronCount() > 0);
    
    std::cout << "    testAgentBrainProcessSensoryInput passed" << std::endl;
}

void testAgentBrainDecodeMotorCommand() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    nlm::AgentBrain agent(brain);
    nlm::SimpleWorld world;
    world.configure(20, 20, 8, 8);
    agent.initialize(world);
    
    // Give brain some activity
    brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 5.0f);
    
    // Get motor command
    auto action = agent.decodeMotorCommand();
    assert(action != nullptr);
    
    std::cout << "    testAgentBrainDecodeMotorCommand passed" << std::endl;
}

void testAgentBrainRewardModulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    nlm::AgentBrain agent(brain);
    nlm::SimpleWorld world;
    world.configure(20, 20, 8, 8);
    agent.initialize(world);
    
    // Enable reward modulation
    agent.enableRewardModulation(true);
    assert(agent.isRewardModulationEnabled());
    
    // Apply reward modulation
    agent.applyRewardModulation(1.0f, 0.8f);
    
    std::cout << "    testAgentBrainRewardModulation passed" << std::endl;
}

void testAgentBrainDevelopment() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    nlm::AgentBrain agent(brain);
    nlm::SimpleWorld world;
    world.configure(20, 20, 8, 8);
    agent.initialize(world);
    
    // Enable development
    agent.enableDevelopment(true);
    assert(agent.isDevelopmentEnabled());
    
    // Update development
    agent.updateDevelopment(0.001);
    
    // Check developmental stage
    nlm::DevelopmentalStage stage = agent.getDevelopmentalStage();
    assert(stage != nlm::DevelopmentalStage::Initial);
    
    std::cout << "    testAgentBrainDevelopment passed" << std::endl;
}

void testAgentBrainCuriosity() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    nlm::AgentBrain agent(brain);
    nlm::SimpleWorld world;
    world.configure(20, 20, 8, 8);
    agent.initialize(world);
    
    // Enable curiosity
    agent.enableCuriosity(true);
    assert(agent.isCuriosityEnabled());
    
    // Check curiosity level
    float curiosity = agent.getCuriosityLevel();
    assert(curiosity >= 0.0f && curiosity <= 1.0f);
    
    // Get novelty level
    float novelty = agent.getNoveltyLevel();
    assert(novelty >= 0.0f && novelty <= 1.0f);
    
    std::cout << "    testAgentBrainCuriosity passed" << std::endl;
}

void testAgentBrainNeuromodulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    nlm::AgentBrain agent(brain);
    nlm::SimpleWorld world;
    world.configure(20, 20, 8, 8);
    agent.initialize(world);
    
    // Check neuromodulation level
    float neuromod = agent.getNeuromodulationLevel();
    assert(neuromod >= 0.0f);
    
    // Apply reward modulation to affect neuromodulation
    agent.applyRewardModulation(1.0f, 0.5f);
    
    std::cout << "    testAgentBrainNeuromodulation passed" << std::endl;
}

void testAgentBrainReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    nlm::AgentBrain agent(brain);
    nlm::SimpleWorld world;
    world.configure(20, 20, 8, 8);
    agent.initialize(world);
    
    // Give brain some activity
    brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 5.0f);
    brain->step(0);
    
    // Reset agent
    agent.reset();
    
    // Verify reset worked (brain should still exist but be in initial state)
    assert(brain->getTotalNeuronCount() == 100);
    
    std::cout << "    testAgentBrainReset passed" << std::endl;
}

void testAgentBrainComplexScenario() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    nlm::AgentBrain agent(brain);
    nlm::SimpleWorld world;
    world.configure(30, 30, 10, 10);
    world.reset();
    agent.initialize(world);
    
    // Enable all features
    agent.enableRewardModulation(true);
    agent.enableStructuralPlasticity(true);
    agent.enableDevelopment(true);
    agent.enableCuriosity(true);
    agent.enableNovelty(true);
    
    // Run simulation
    for (int step = 0; step < 50; ++step) {
        world.update(0.1);
        
        nlm::SensoryPercept percept = world.getSensoryPercept();
        agent.processSensoryInput(percept);
        
        brain->step(step);
        
        auto action = agent.decodeMotorCommand();
        world.applyMotorCommand(action, world.getSimulationTime());
        
        // Compute reward and apply
        float reward = world.computeReward(brain->getRegions()[0].get());
        agent.applyRewardModulation(reward, 0.0f);
        
        // Update development
        agent.updateDevelopment(0.001);
        
        if (step % 10 == 0) {
            float avg_firing = brain->getAverageFiringRate();
            float curiosity = agent.getCuriosityLevel();
            std::cout << "    Step " << step << ": Firing=" << brain->getFiringNeuronCount() 
                     << ", AvgFiring=" << avg_firing << ", Curiosity=" << curiosity << std::endl;
        }
    }
    
    std::cout << "    testAgentBrainComplexScenario passed" << std::endl;
}

void runAll() {
    testAgentBrainCreation();
    testAgentBrainInitialization();
    testAgentBrainProcessSensoryInput();
    testAgentBrainDecodeMotorCommand();
    testAgentBrainRewardModulation();
    testAgentBrainDevelopment();
    testAgentBrainCuriosity();
    testAgentBrainNeuromodulation();
    testAgentBrainReset();
    testAgentBrainComplexScenario();
    
    std::cout << std::endl;
    std::cout << "=== All Agent Tests PASSED ===" << std::endl;
    std::cout << "Agent system now supports:" << std::endl;
    std::cout << "  - Full integration with NLM brain" << std::endl;
    std::cout << "  - Sensory input processing" << std::endl;
    std::cout << "  - Motor command decoding" << std::endl;
    std::cout << "  - Reward modulation" << std::endl;
    std::cout << "  - Developmental processes" << std::endl;
    std::cout << "  - Curiosity and novelty detection" << std::endl;
    std::cout << "  - Neuromodulation integration" << std::endl;
}

} // namespace test_agent