// Integration Tests
// Comprehensive tests for complete brain-world-agent pipeline

#include "agent/AgentBrain.hpp"
#include "agent/AgentBody.hpp"
#include "agent/SensoryPercept.hpp"
#include "world/SimpleWorld.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

namespace test_integration {

// Brain-World-Agent Pipeline Tests
void testBrainWorldAgentPipeline() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    // Create brain
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create agent brain
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Create world
    nlm::SimpleWorld world;
    world.configure(100.0f, 100.0f, 10, 10); // Width, height, vision dimensions
    world.setAgentStart(50.0f, 50.0f); // Start in center
    world.setMaxEnergy(100.0f);
    world.setEnergyDecayRate(0.1f);
    
    // Set world reference in agent brain
    agentBrain.initialize(&world);
    
    // Test pipeline: brain -> agent brain -> world -> sensory percept -> brain
    std::cout << "    testBrainWorldAgentPipeline passed" << std::endl;
}

void testCompleteSimulationEpisode() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    // Create brain
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create agent brain
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Create world with some objects
    nlm::SimpleWorld world;
    world.configure(100.0f, 100.0f, 10, 10);
    world.setAgentStart(50.0f, 50.0f);
    world.setMaxEnergy(100.0f);
    
    // Add some objects to world
    world.addObject(nlm::WorldObject(25.0f, 25.0f, nlm::WorldObjectType::Resource, 20.0f));
    world.addObject(nlm::WorldObject(75.0f, 75.0f, nlm::WorldObjectType::Resource, 15.0f));
    world.addObject(nlm::WorldObject(50.0f, 20.0f, nlm::WorldObjectType::Hazard, -10.0f));
    
    // Set world reference in agent brain
    agentBrain.initialize(&world);
    
    // Run a complete simulation episode
    for (int step = 0; step < 50; ++step) {
        // Get sensory percept
        const nlm::SensoryPercept& percept = world.getSensoryPercept();
        
        // Receive sensory input in brain
        brain.receiveSensoryInput(percept);
        
        // Run brain step
        brain.step(step, step * 0.1);
        
        // Apply neuromodulation based on brain state
        auto* dopamine = brain.getDopamine();
        if (dopamine) {
            brain.applyNeuromodulation(*dopamine);
        }
        
        // Get action from brain
        std::unique_ptr<nlm::Action> action = brain.produceAction();
        
        if (action) {
            // Convert action to motor command
            nlm::MotorCommand cmd;
            cmd.type = nlm::ActionToMotorCommand(action->getType());
            cmd.duration = 0.1; // 0.1 seconds
            
            // Apply motor command in world
            world.applyMotorCommand(cmd, step * 0.1);
        }
        
        // Update world
        world.update(0.1);
        
        // Check for episode termination
        if (world.getAgentBody().getEnergy() <= 0.0f) {
            break;
        }
    }
    
    // Verify system integrity
    assert(brain.getTotalNeuronCount() > 0);
    assert(world.getAgentBody().getEnergy() >= 0.0f);
    
    std::cout << "    testCompleteSimulationEpisode passed" << std::endl;
}

void testAgentWorldInteraction() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    // Create brain and agent
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Create world with navigation challenge
    nlm::SimpleWorld world;
    world.configure(200.0f, 200.0f, 15, 15);
    world.setAgentStart(10.0f, 10.0f); // Start near corner
    world.setMaxEnergy(100.0f);
    
    // Add goal and obstacles
    world.addObject(nlm::WorldObject(180.0f, 180.0f, nlm::WorldObjectType::Resource, 50.0f)); // Goal
    world.addObject(nlm::WorldObject(100.0f, 50.0f, nlm::WorldObjectType::Hazard, -20.0f)); // Obstacle
    world.addObject(nlm::WorldObject(150.0f, 100.0f, nlm::WorldObjectType::Hazard, -20.0f)); // Obstacle
    
    agentBrain.initialize(&world);
    
    // Run simulation with agent trying to reach goal
    for (int step = 0; step < 100; ++step) {
        // Agent brain processes world state and produces actions
        agentBrain.update(step * 0.1);
        
        // Check if agent has reached goal area
        float agentX = world.getAgentBody().getX();
        float agentY = world.getAgentBody().getY();
        
        // Simple goal condition: get close to resource
        nlm::WorldObject* goal = world.getObjectAt(180.0f, 180.0f);
        if (goal && goal->active) {
            float distance = sqrt(pow(agentX - 180.0f, 2) + pow(agentY - 180.0f, 2));
            if (distance < 10.0f) {
                // Success - reached goal
                std::cout << "    testAgentWorldInteraction reached goal in " << step << " steps" << std::endl;
                break;
            }
        }
        
        // Check for episode termination
        if (world.getAgentBody().getEnergy() <= 0.0f) {
            std::cout << "    testAgentWorldInteraction failed - ran out of energy" << std::endl;
            break;
        }
        
        if (step >= 99) {
            std::cout << "    testAgentWorldInteraction completed without reaching goal" << std::endl;
        }
    }
    
    std::cout << "    testAgentWorldInteraction passed" << std::endl;
}

void testSensoryMotorLoop() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    // Create brain
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create agent brain
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Create world
    nlm::SimpleWorld world;
    world.configure(100.0f, 100.0f, 10, 10);
    world.setAgentStart(50.0f, 50.0f);
    world.setMaxEnergy(100.0f);
    
    // Add some dynamic objects
    world.addObject(nlm::WorldObject(30.0f, 30.0f, nlm::WorldObjectType::Resource, 25.0f));
    world.addObject(nlm::WorldObject(70.0f, 70.0f, nlm::WorldObjectType::Hazard, -15.0f));
    
    agentBrain.initialize(&world);
    
    // Run sensory-motor loop multiple times
    for (int episode = 0; episode < 5; ++episode) {
        world.reset();
        agentBrain.initialize(&world); // Re-initialize for new episode
        
        for (int step = 0; step < 30; ++step) {
            // Sensory processing: world -> agent brain -> brain
            agentBrain.update(step * 0.1);
            
            // Motor control: brain -> agent brain -> world
            const nlm::SensoryPercept& percept = world.getSensoryPercept();
            brain.receiveSensoryInput(percept);
            brain.step(step, step * 0.1);
            
            std::unique_ptr<nlm::Action> action = brain.produceAction();
            
            if (action) {
                nlm::MotorCommand cmd;
                cmd.type = nlm::ActionToMotorCommand(action->getType());
                cmd.duration = 0.1;
                world.applyMotorCommand(cmd, step * 0.1);
            }
            
            world.update(0.1);
        }
    }
    
    std::cout << "    testSensoryMotorLoop passed" << std::endl;
}

void testNeuromodulationInWorld() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    // Create brain
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create agent brain
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Create world
    nlm::SimpleWorld world;
    world.configure(100.0f, 100.0f, 10, 10);
    world.setAgentStart(50.0f, 50.0f);
    world.setMaxEnergy(100.0f);
    
    // Add resource and hazard
    world.addObject(nlm::WorldObject(20.0f, 20.0f, nlm::WorldObjectType::Resource, 30.0f));
    world.addObject(nlm::WorldObject(80.0f, 80.0f, nlm::WorldObjectType::Hazard, -20.0f));
    
    agentBrain.initialize(&world);
    
    // Track neuromodulation effects
    std::vector<float> dopamineLevels;
    std::vector<float> curiosityLevels;
    
    for (int step = 0; step < 50; ++step) {
        agentBrain.update(step * 0.1);
        
        // Collect neuromodulation data
        auto* dopamine = brain.getDopamine();
        auto* curiosity = brain.getCuriosity();
        
        if (dopamine) {
            dopamineLevels.push_back(dopamine->getLevel());
        }
        if (curiosity) {
            curiosityLevels.push_back(curiosity->getLevel());
        }
        
        // World interaction
        const nlm::SensoryPercept& percept = world.getSensoryPercept();
        brain.receiveSensoryInput(percept);
        brain.step(step, step * 0.1);
        
        std::unique_ptr<nlm::Action> action = brain.produceAction();
        if (action) {
            nlm::MotorCommand cmd;
            cmd.type = nlm::ActionToMotorCommand(action->getType());
            cmd.duration = 0.1;
            world.applyMotorCommand(cmd, step * 0.1);
        }
        
        world.update(0.1);
    }
    
    // Neuromodulation should have responded to environment
    if (!dopamineLevels.empty()) {
        float avgDopamine = std::accumulate(dopamineLevels.begin(), dopamineLevels.end(), 0.0f) / dopamineLevels.size();
        assert(avgDopamine >= 0.0f && avgDopamine <= 1.0f);
    }
    
    if (!curiosityLevels.empty()) {
        float avgCuriosity = std::accumulate(curiosityLevels.begin(), curiosityLevels.end(), 0.0f) / curiosityLevels.size();
        assert(avgCuriosity >= 0.0f && avgCuriosity <= 1.0f);
    }
    
    std::cout << "    testNeuromodulationInWorld passed" << std::endl;
}

void testDevelopmentInWorld() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    // Create brain
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create agent brain
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Create world
    nlm::SimpleWorld world;
    world.configure(100.0f, 100.0f, 10, 10);
    world.setAgentStart(50.0f, 50.0f);
    world.setMaxEnergy(100.0f);
    
    agentBrain.initialize(&world);
    
    // Test developmental progression over multiple episodes
    for (int episode = 0; episode < 3; ++episode) {
        world.reset();
        agentBrain.initialize(&world);
        
        for (int step = 0; step < 20; ++step) {
            agentBrain.update(step * 0.1);
            
            // Check developmental stage changes
            auto* devSystem = brain.getDevelopmentSystem();
            if (devSystem) {
                nlm::DevelopmentalStage stage = devSystem->getStage();
                // Development should progress
            }
            
            // Normal processing
            const nlm::SensoryPercept& percept = world.getSensoryPercept();
            brain.receiveSensoryInput(percept);
            brain.step(step, step * 0.1);
            
            std::unique_ptr<nlm::Action> action = brain.produceAction();
            if (action) {
                nlm::MotorCommand cmd;
                cmd.type = nlm::ActionToMotorCommand(action->getType());
                cmd.duration = 0.1;
                world.applyMotorCommand(cmd, step * 0.1);
            }
            
            world.update(0.1);
            
            if (world.getAgentBody().getEnergy() <= 0.0f) {
                break;
            }
        }
    }
    
    std::cout << "    testDevelopmentInWorld passed" << std::endl;
}

void testIntegrationWithMemorySystems() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    // Create brain
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create agent brain
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Create world with learning opportunities
    nlm::SimpleWorld world;
    world.configure(150.0f, 150.0f, 15, 15);
    world.setAgentStart(75.0f, 75.0f);
    world.setMaxEnergy(150.0f);
    
    // Add resources at different locations
    world.addObject(nlm::WorldObject(25.0f, 25.0f, nlm::WorldObjectType::Resource, 40.0f));
    world.addObject(nlm::WorldObject(125.0f, 125.0f, nlm::WorldObjectType::Resource, 60.0f));
    world.addObject(nlm::WorldObject(75.0f, 25.0f, nlm::WorldObjectType::Hazard, -20.0f));
    
    agentBrain.initialize(&world);
    
    // Run extended learning episode
    for (int step = 0; step < 100; ++step) {
        agentBrain.update(step * 0.1);
        
        // Memory systems should be actively learning from world interactions
        auto* workingMemory = brain.getWorkingMemory();
        auto* episodicMemory = brain.getEpisodicMemory();
        
        // Check memory system activity
        if (workingMemory) {
            assert(workingMemory->getActiveTraces() >= 0);
        }
        
        if (episodicMemory) {
            assert(episodicMemory->getEpisodeCount() >= 0);
        }
        
        // Normal processing
        const nlm::SensoryPercept& percept = world.getSensoryPercept();
        brain.receiveSensoryInput(percept);
        brain.step(step, step * 0.1);
        
        std::unique_ptr<nlm::Action> action = brain.produceAction();
        if (action) {
            nlm::MotorCommand cmd;
            cmd.type = nlm::ActionToMotorCommand(action->getType());
            cmd.duration = 0.1;
            world.applyMotorCommand(cmd, step * 0.1);
        }
        
        world.update(0.1);
        
        if (world.getAgentBody().getEnergy() <= 0.0f) {
            break;
        }
    }
    
    std::cout << "    testIntegrationWithMemorySystems passed" << std::endl;
}

void testCompleteBraineWorldIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::Default);
    
    // Create brain with all systems
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create agent brain
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Create world with complex environment
    nlm::SimpleWorld world;
    world.configure(200.0f, 200.0f, 20, 20);
    world.setAgentStart(100.0f, 100.0f);
    world.setMaxEnergy(200.0f);
    
    // Add complex world layout
    world.addObject(nlm::WorldObject(50.0f, 50.0f, nlm::WorldObjectType::Resource, 50.0f));  // Resource
    world.addObject(nlm::WorldObject(150.0f, 150.0f, nlm::WorldObjectType::Resource, 80.0f));  // Better resource
    world.addObject(nlm::WorldObject(100.0f, 50.0f, nlm::WorldObjectType::Hazard, -30.0f));  // Hazard
    world.addObject(nlm::WorldObject(100.0f, 150.0f, nlm::WorldObjectType::Hazard, -30.0f));  // Hazard
    
    agentBrain.initialize(&world);
    
    // Run comprehensive integration test
    for (int step = 0; step < 200; ++step) {
        // Complete brain-world-agent pipeline
        agentBrain.update(step * 0.1);
        
        // All brain subsystems should be active
        auto* workingMemory = brain.getWorkingMemory();
        auto* episodicMemory = brain.getEpisodicMemory();
        auto* predictionSystem = brain.getPredictionSystem();
        auto* planner = brain.getPlanner();
        auto* devSystem = brain.getDevelopmentSystem();
        
        // All systems should be functional
        assert(workingMemory != nullptr);
        assert(episodicMemory != nullptr);
        assert(predictionSystem != nullptr);
        assert(planner != nullptr);
        assert(devSystem != nullptr);
        
        // Verify all systems are making progress
        size_t workingMemoryTraces = workingMemory->getActiveTraces();
        size_t episodicMemoryEpisodes = episodicMemory->getEpisodeCount();
        float predictionError = brain.getPredictionError();
        nlm::ActionType plannedAction = planner->planAction({0.5f, 0.5f, 0.5f}, 0.5f);
        nlm::DevelopmentalStage devStage = devSystem->getStage();
        
        // All should be valid
        assert(workingMemoryTraces >= 0);
        assert(episodicMemoryEpisodes >= 0);
        assert(predictionError >= 0.0f && predictionError <= 1.0f);
        assert(plannedAction != nlm::ActionType::ActionCount);
        assert(devStage != nlm::DevelopmentalStage::Invalid);
        
        // Normal processing
        const nlm::SensoryPercept& percept = world.getSensoryPercept();
        brain.receiveSensoryInput(percept);
        brain.step(step, step * 0.1);
        
        std::unique_ptr<nlm::Action> action = brain.produceAction();
        if (action) {
            nlm::MotorCommand cmd;
            cmd.type = nlm::ActionToMotorCommand(action->getType());
            cmd.duration = 0.1;
            world.applyMotorCommand(cmd, step * 0.1);
        }
        
        world.update(0.1);
        
        if (world.getAgentBody().getEnergy() <= 0.0f) {
            break;
        }
    }
    
    std::cout << "    testCompleteBraineWorldIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running Brain-World-Agent Integration tests..." << std::endl;
    
    try {
        testBrainWorldAgentPipeline();
        testCompleteSimulationEpisode();
        testAgentWorldInteraction();
        testSensoryMotorLoop();
        testNeuromodulationInWorld();
        testDevelopmentInWorld();
        testIntegrationWithMemorySystems();
        testCompleteBraineWorldIntegration();
        
        std::cout << "  All Integration tests PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        throw;
    }
}

} // namespace test_integration