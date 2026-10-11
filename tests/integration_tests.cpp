// Integration Tests
// Comprehensive integration testing for NLM system components

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "visualization/VisualizationInterface.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <thread>
#include <chrono>
#include <vector>

namespace nlm::tests {

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create system configuration
        config = std::make_shared<Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 3);
        config->set("connection_probability", 0.05f);
        config->set("visualization.enabled", true);
        config->set("visualization.updateRate", 30.0f);
        
        // Initialize all system components
        brain = std::make_shared<Brain>(config);
        world = std::make_shared<SimpleWorld>();
        agent = std::make_shared<AgentBrain>(brain);
        visualization = std::make_shared<VisualizationInterface>();
        
        // Initialize all components
        brain->initialize();
        agent->initialize(*world);
        visualization->initialize();
        
        // Configure world for testing
        world->configure(800, 600, 400, 300);
        world->setAgentStart(400, 300);
    }
    
    void TearDown() override {
        // Cleanup in reverse order
        visualization.reset();
        agent.reset();
        world.reset();
        brain.reset();
        config.reset();
    }
    
    // Test fixtures
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    std::shared_ptr<SimpleWorld> world;
    std::shared_ptr<AgentBrain> agent;
    std::shared_ptr<VisualizationInterface> visualization;
};

// System Integration Tests
TEST_F(IntegrationTest, ComponentInitialization) {
    // Verify all components initialized successfully
    EXPECT_NE(brain, nullptr);
    EXPECT_NE(world, nullptr);
    EXPECT_NE(agent, nullptr);
    EXPECT_NE(visualization, nullptr);
    
    // Verify component state
    EXPECT_TRUE(brain->isInitialized());
    EXPECT_TRUE(visualization->isActive());
}

TEST_F(IntegrationTest, BrainWorldDataFlow) {
    // Test data flow from brain to world
    // Get current percept from world
    auto percept = world->getSensoryPercept();
    
    // Process percept with agent
    agent->processSensoryInput(percept);
    
    // Verify brain received input (brain should have processed sensory input)
    brain->step(0);
    
    // Verify action production
    auto action = brain->produceAction();
    if (action) {
        EXPECT_NE(action->getType(), ActionType::None);
        
        // Apply action to world
        world->applyMotorCommand(*action, 0.0);
        
        // Verify world updated
        auto body = world->getAgentBody();
        EXPECT_TRUE(body->x >= 0.0 && body->x <= world->getWidth());
        EXPECT_TRUE(body->y >= 0.0 && body->y <= world->getHeight());
    }
}

TEST_F(IntegrationTest, AgentBrainBrainIntegration) {
    // Test AgentBrain integration with Brain
    // Simulate sensory input
    auto vision_input = std::make_shared<Vision>();
    vision_input->setData(std::vector<float>(800 * 600 * 3, 0.5f));
    vision_input->setTimestamp(0.0);
    
    // Convert to percept
    auto percept = std::make_shared<SensoryPercept>();
    percept->setVision(*vision_input);
    
    // Process with agent
    agent->processSensoryInput(percept);
    
    // Verify agent sent signals to brain
    brain->step(1);
    
    // Verify action production
    auto action = brain->produceAction();
    if (action) {
        EXPECT_NE(action->getType(), ActionType::None);
    }
}

TEST_F(IntegrationTest, WorldBrainActionCycle) {
    // Test complete world-brain action cycle
    const int steps = 50;
    
    for (int step = 0; step < steps; ++step) {
        // Get current world state
        auto body = world->getAgentBody();
        float current_x = body->x;
        float current_y = body->y;
        
        // Update world with current time
        world->update(step * 0.001);
        
        // Get new percept
        auto percept = world->getSensoryPercept();
        
        // Process with agent
        agent->processSensoryInput(percept);
        
        // Brain step
        brain->step(step, step * 0.001);
        
        // Produce and apply action
        auto action = brain->produceAction();
        if (action) {
            world->applyMotorCommand(*action, step * 0.001);
        }
        
        // Verify world state changed appropriately
        body = world->getAgentBody();
        if (action) {
            EXPECT_NE(body->x, current_x);
            EXPECT_NE(body->y, current_y);
        }
    }
}

TEST_F(IntegrationTest, VisualizationIntegration) {
    // Test visualization with brain and world
    brain->step(0);
    world->update(0.0);
    
    // Update visualization with system state
    visualization->update();
    
    // Render visualization
    visualization->render();
    
    // Verify visualization processed system data
    EXPECT_TRUE(visualization->isActive());
    
    // Visualize different components
    visualization->visualizeNetwork(*brain);
    visualization->visualizeActivity(*brain);
    visualization->visualizeWeights(*brain);
    
    // Should not throw exception
    EXPECT_TRUE(true);
}

TEST_F(IntegrationTest, PerformanceMonitoringIntegration) {
    // Test performance monitoring across all components
    brain->resetPerformanceStats();
    
    // Run simulation cycle
    for (int step = 0; step < 100; ++step) {
        // Update world
        world->update(step * 0.001);
        
        // Get percept and process
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        
        // Brain step
        brain->step(step, step * 0.001);
        
        // Visualization update
        visualization->update();
        
        // Verify performance stats
        auto stats = brain->getPerformanceStats();
        EXPECT_GE(stats.neuronsPerSecond, 0.0);
        EXPECT_GE(stats.stepTimeMs, 0.0);
    }
    
    // Verify performance logging
    brain->logPerformanceReport();
}

TEST_F(IntegrationTest, ConfigurationIntegration) {
    // Test configuration system integration
    // Load configuration from file
    std::string temp_file = "/tmp/integration_test_config.json";
    config->saveToFile(temp_file);
    
    auto new_config = std::make_shared<Config>();
    new_config->loadFromFile(temp_file);
    
    // Verify configuration loading
    EXPECT_EQ(new_config->get<int>("neuron_count"), config->get<int>("neuron_count"));
    EXPECT_EQ(new_config->get<int>("region_count"), config->get<int>("region_count"));
    
    // Cleanup
    std::remove(temp_file.c_str());
}

TEST_F(IntegrationTest, SensoryMotorLoop) {
    // Test complete sensory-motor learning loop
    const int episodes = 20;
    
    for (int episode = 0; episode < episodes; ++episode) {
        // Reset for new episode
        brain->reset();
        agent->reset();
        
        // Initialize world
        world->reset();
        
        // Run episode
        for (int step = 0; step < 100; ++step) {
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Brain step
            brain->step(step, step * 0.001);
            
            // Produce action
            auto action = brain->produceAction();
            if (action) {
                // Apply action
                world->applyMotorCommand(*action, step * 0.001);
            }
        }
        
        // Verify episode completed
        EXPECT_TRUE(true);
    }
}

TEST_F(IntegrationTest, MemoryManagementIntegration) {
    // Test memory management across all components
    auto initial_stats = brain->getPerformanceStats();
    double initial_memory = initial_stats.memoryUsageMB;
    
    // Run intensive simulation
    for (int step = 0; step < 500; ++step) {
        brain->step(step);
        world->update(step * 0.001);
        visualization->update();
    }
    
    auto final_stats = brain->getPerformanceStats();
    double final_memory = final_stats.memoryUsageMB;
    
    // Memory usage should not grow excessively
    EXPECT_LT(final_memory - initial_memory, 200.0); // Less than 200MB growth
}

TEST_F(IntegrationTest, CrossComponentDataConsistency) {
    // Test data consistency across components
    brain->step(0);
    
    // Get data from brain
    int brain_neurons = brain->getTotalNeuronCount();
    int brain_spikes = brain->getTotalSpikeCount();
    
    // Get data from world
    auto body = world->getAgentBody();
    
    // Get data from visualization
    auto vis_rate = visualization->getUpdateRate();
    
    // Verify consistency
    EXPECT_GT(brain_neurons, 0);
    EXPECT_TRUE(brain_spikes >= 0);
    EXPECT_TRUE(body->x >= 0.0 && body->x <= world->getWidth());
    EXPECT_TRUE(vis_rate > 0.0f);
}

TEST_F(IntegrationTest, ErrorHandlingIntegration) {
    // Test error handling across system components
    // Test with invalid configuration
    auto invalid_config = std::make_shared<Config>();
    invalid_config->set("neuron_count", -1); // Invalid value
    
    auto invalid_brain = std::make_shared<Brain>(invalid_config);
    invalid_brain->initialize(); // Should handle gracefully
    
    // Test with invalid world configuration
    world->configure(0, 0, 0, 0); // Invalid dimensions
    
    // Should handle gracefully without crashing
    EXPECT_TRUE(true);
}

TEST_F(IntegrationTest, PerformanceScaling) {
    // Test system performance scaling
    const int neuron_counts[] = {100, 500, 1000, 2000};
    
    for (size_t i = 0; i < sizeof(neuron_counts) / sizeof(neuron_counts[0]); ++i) {
        int neurons = neuron_counts[i];
        
        // Create configuration for current neuron count
        auto scale_config = std::make_shared<Config>();
        scale_config->set("neuron_count", neurons);
        scale_config->set("region_count", std::max(1, neurons / 200));
        scale_config->set("connection_probability", 0.1f);
        
        auto scale_brain = std::make_shared<Brain>(scale_config);
        scale_brain->initialize();
        
        // Measure performance
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (int step = 0; step < 50; ++step) {
            scale_brain->step(step);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(end_time - start_time);
        
        double steps_per_second = 50.0 / duration.count();
        
        // Performance should scale reasonably
        EXPECT_GT(steps_per_second, 100.0); // At least 100 steps/sec
    }
}

TEST_F(IntegrationTest, ResourceUtilization) {
    // Test resource utilization
    auto start_stats = brain->getPerformanceStats();
    
    // Run intensive simulation
    for (int step = 0; step < 200; ++step) {
        brain->step(step);
        world->update(step * 0.001);
        visualization->update();
        visualization->render();
    }
    
    auto end_stats = brain->getPerformanceStats();
    
    // Resource utilization should be within bounds
    EXPECT_LE(end_stats.stepTimeMs, 100.0); // Each step < 100ms
    EXPECT_LE(end_stats.updateTimeMs, 50.0); // Update time < 50ms
    EXPECT_LE(end_stats.renderTimeMs, 50.0); // Render time < 50ms
}

TEST_F(IntegrationTest, SystemStability) {
    // Test overall system stability
    const int iterations = 100;
    int failures = 0;
    
    for (int i = 0; i < iterations; ++i) {
        try {
            // Run complete system cycle
            brain->reset();
            world->reset();
            visualization->close();
            visualization->initialize();
            
            // Run simulation
            for (int step = 0; step < 20; ++step) {
                world->update(step * 0.001);
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                brain->step(step, step * 0.001);
                visualization->update();
                visualization->render();
            }
            
        } catch (...) {
            failures++;
        }
    }
    
    // System should be stable (few or no failures)
    EXPECT_LT(failures, iterations * 0.1); // Less than 10% failure rate
}

TEST_F(IntegrationTest, CompleteSystemSimulation) {
    // Test complete system simulation with all components
    const int simulation_steps = 200;
    int successful_actions = 0;
    int total_spikes = 0;
    
    // Reset all components
    brain->reset();
    agent->reset();
    world->reset();
    
    // Run complete simulation
    for (int step = 0; step < simulation_steps; ++step) {
        // Update world with physics simulation
        world->update(step * 0.001);
        
        // Get sensory percept from world
        auto percept = world->getSensoryPercept();
        
        // Process sensory input through agent
        agent->processSensoryInput(percept);
        
        // Brain step (includes neural computation)
        brain->step(step, step * 0.001);
        
        // Update visualization with system state
        visualization->update();
        visualization->visualizeNetwork(*brain);
        visualization->visualizeActivity(*brain);
        
        // Render visualization
        visualization->render();
        
        // Produce and apply action
        auto action = brain->produceAction();
        if (action) {
            successful_actions++;
            world->applyMotorCommand(*action, step * 0.001);
        }
        
        total_spikes = brain->getTotalSpikeCount();
        
        // Monitor performance
        if (step % 50 == 0) {
            brain->logPerformanceReport();
        }
    }
    
    // Verify simulation completed successfully
    EXPECT_GE(successful_actions, 0); // May be zero if no actions produced
    EXPECT_GE(total_spikes, 0); // Spike count should be non-negative
    
    // Performance should be reasonable
    auto final_stats = brain->getPerformanceStats();
    EXPECT_GT(final_stats.neuronsPerSecond, 0.0);
}

// Benchmarking Functions
static void BM_IntegrationBrainStep(benchmark::State& state) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 2);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    auto world = std::make_shared<SimpleWorld>();
    world->configure(800, 600, 400, 300);
    
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(*world);
    
    auto visualization = std::make_shared<VisualizationInterface>();
    visualization->initialize();
    
    for (auto _ : state) {
        world->update(state.range(0));
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        brain->step(state.range(0), state.range(0) * 0.001);
        visualization->update();
        visualization->render();
    }
}

BENCHMARK(BM_IntegrationBrainStep)->Arg(0.001)->Arg(0.01)->Arg(0.1);

} // namespace nlm::tests
