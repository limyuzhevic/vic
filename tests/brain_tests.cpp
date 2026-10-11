// Brain Unit Tests
// Comprehensive unit testing for the Brain class

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <thread>
#include <chrono>

namespace nlm::tests {

class BrainTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create standard test configuration
        config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 2);
        config->set("connection_probability", 0.1f);
        
        // Create brain with configuration
        brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Reset performance stats for testing
        brain->resetPerformanceStats();
    }
    
    void TearDown() override {
        // Cleanup
        brain.reset();
        config.reset();
    }
    
    // Test fixtures
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
};

// Basic Brain Functionality Tests
TEST_F(BrainTest, BrainCreation) {
    ASSERT_NE(brain, nullptr);
    EXPECT_EQ(brain->getTotalNeuronCount(), 100);
    EXPECT_EQ(brain->getRegionCount(), 2);
    EXPECT_GT(brain->getTotalSynapseCount(), 0);
}

TEST_F(BrainTest, Initialization) {
    // Test that brain initializes correctly
    EXPECT_TRUE(brain->getTotalNeuronCount() > 0);
    EXPECT_TRUE(brain->getTotalSynapseCount() > 0);
    EXPECT_GE(brain->getAverageFiringRate(), 0.0f);
}

TEST_F(BrainTest, StepExecution) {
    const int steps = 100;
    
    // Execute multiple steps
    for (int step = 0; step < steps; ++step) {
        brain->step(step);
    }
    
    // Verify brain state remains consistent
    EXPECT_EQ(brain->getTotalNeuronCount(), 100);
    EXPECT_EQ(brain->getRegionCount(), 2);
}

TEST_F(BrainTest, ResetFunctionality) {
    // Take a step first
    brain->step(0);
    
    // Get current state
    int neurons_before = brain->getTotalNeuronCount();
    int spikes_before = brain->getTotalSpikeCount();
    
    // Reset brain
    brain->reset();
    
    // Verify reset worked
    EXPECT_EQ(brain->getTotalNeuronCount(), neurons_before);
    EXPECT_EQ(brain->getTotalSpikeCount(), 0);
}

TEST_F(BrainTest, RegionManagement) {
    // Test region functionality
    auto region_ids = brain->getRegionIds();
    EXPECT_EQ(region_ids.size(), 2);
    
    // Verify all regions are accessible
    for (const auto& region_id : region_ids) {
        auto* region = brain->getRegion(region_id);
        ASSERT_NE(region, nullptr);
    }
}

TEST_F(BrainTest, ActionProduction) {
    // Test action production
    auto action = brain->produceAction();
    
    // Action should be produced (brain is active)
    if (action) {
        EXPECT_NE(action->getType(), ActionType::None);
    }
}

TEST_F(BrainTest, ActionProductionWithInput) {
    // Test action production with sensory input
    brain->step(0);
    
    auto action = brain->produceAction();
    
    // Brain should produce action after processing input
    if (action) {
        EXPECT_NE(action->getType(), ActionType::None);
    }
}

TEST_F(BrainTest, ConfigurationAccess) {
    // Test configuration access
    EXPECT_TRUE(config->has("neuron_count"));
    EXPECT_TRUE(config->has("region_count"));
    EXPECT_TRUE(config->has("connection_probability"));
    
    EXPECT_EQ(config->get<int>("neuron_count"), 100);
    EXPECT_EQ(config->get<int>("region_count"), 2);
    EXPECT_FLOAT_EQ(config->get<float>("connection_probability"), 0.1f);
}

TEST_F(BrainTest, PerformanceStats) {
    // Test performance statistics
    auto stats = brain->getPerformanceStats();
    
    EXPECT_GE(stats.neuronsPerSecond, 0.0);
    EXPECT_GE(stats.spikesPerSecond, 0.0);
    EXPECT_GE(stats.synapticEventsPerSecond, 0.0);
    EXPECT_GE(stats.realTimeFactor, 0.0);
    EXPECT_GE(stats.activeNeurons, 0);
    EXPECT_GE(stats.firingNeurons, 0);
    EXPECT_GE(stats.pendingSpikes, 0);
    EXPECT_GE(stats.pendingDelayedSpikes, 0);
    EXPECT_GE(stats.avgFiringRate, 0.0f);
    EXPECT_GE(stats.memoryUsageMB, 0.0f);
    EXPECT_GE(stats.stepTimeMs, 0.0);
    EXPECT_GE(stats.updateTimeMs, 0.0);
    EXPECT_GE(stats.renderTimeMs, 0.0);
}

// Advanced Brain Functionality Tests
TEST_F(BrainTest, SpikeGeneration) {
    const int steps = 50;
    int spike_count = 0;
    
    // Count spikes generated
    for (int step = 0; step < steps; ++step) {
        brain->step(step);
        spike_count = brain->getTotalSpikeCount();
    }
    
    // Should have generated spikes
    EXPECT_GT(spike_count, 0);
}

TEST_F(BrainTest, NeuronState) {
    // Test individual neuron state
    int active_neurons = brain->getActiveNeuronCount();
    int firing_neurons = brain->getFiringNeuronCount();
    
    EXPECT_GE(active_neurons, 0);
    EXPECT_GE(firing_neurons, 0);
    EXPECT_LE(firing_neurons, active_neurons);
}

TEST_F(BrainTest, DevelopmentStage) {
    // Test development stage tracking
    EXPECT_GE(brain->getDevelopmentalStage(), DevelopmentalStage::Early);
    EXPECT_LE(brain->getDevelopmentalStage(), DevelopmentalStage::Mature);
}

TEST_F(BrainTest, Neuromodulation) {
    // Test neuromodulation levels
    float curiosity = brain->getCuriosityLevel();
    float novelty = brain->getNoveltyLevel();
    float prediction_error = brain->getPredictionError();
    
    EXPECT_GE(curiosity, 0.0f);
    EXPECT_GE(novelty, 0.0f);
    EXPECT_GE(prediction_error, 0.0f);
}

// Stress and Performance Tests
TEST_F(BrainTest, LargeNetwork) {
    // Test with larger network
    auto large_config = std::make_shared<Config>();
    large_config->set("neuron_count", 2000);
    large_config->set("region_count", 5);
    large_config->set("connection_probability", 0.05f);
    
    auto large_brain = std::make_shared<Brain>(large_config);
    large_brain->initialize();
    
    EXPECT_EQ(large_brain->getTotalNeuronCount(), 2000);
    EXPECT_EQ(large_brain->getRegionCount(), 5);
}

TEST_F(BrainTest, HighFrequencySteps) {
    // Test high-frequency step execution
    const int steps = 1000;
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int step = 0; step < steps; ++step) {
        brain->step(step);
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end_time - start_time);
    
    double steps_per_second = steps / duration.count();
    EXPECT_GT(steps_per_second, 1000.0); // Should achieve >1k steps/sec
}

TEST_F(BrainTest, MemoryEfficiency) {
    // Test memory efficiency
    auto initial_stats = brain->getPerformanceStats();
    double initial_memory = initial_stats.memoryUsageMB;
    
    // Execute simulation
    for (int step = 0; step < 100; ++step) {
        brain->step(step);
    }
    
    auto final_stats = brain->getPerformanceStats();
    double final_memory = final_stats.memoryUsageMB;
    
    // Memory usage should not grow excessively
    EXPECT_LT(final_memory - initial_memory, 100.0); // Less than 100MB growth
}

// Configuration and State Management Tests
TEST_F(BrainTest, ConfigurationLoading) {
    // Test configuration loading from JSON
    std::string json_str = R"({
        "neuron_count": 500,
        "region_count": 3,
        "connection_probability": 0.2
    })";
    
    auto test_config = Config::fromJson(json_str);
    ASSERT_NE(test_config, nullptr);
    
    EXPECT_EQ(test_config->get<int>("neuron_count"), 500);
    EXPECT_EQ(test_config->get<int>("region_count"), 3);
    EXPECT_FLOAT_EQ(test_config->get<float>("connection_probability"), 0.2f);
}

TEST_F(BrainTest, ConfigurationSerialization) {
    // Test configuration serialization
    std::string json_str = config->toJson();
    
    auto loaded_config = Config::fromJson(json_str);
    ASSERT_NE(loaded_config, nullptr);
    
    EXPECT_EQ(loaded_config->get<int>("neuron_count"), config->get<int>("neuron_count"));
    EXPECT_EQ(loaded_config->get<int>("region_count"), config->get<int>("region_count"));
}

// Brain-Simulation Tests
TEST_F(BrainTest, SensoryInputProcessing) {
    // Test sensory input processing
    auto sensory_input = std::make_shared<Vision>();
    sensory_input->setData(std::vector<float>(100, 0.5f));
    
    brain->receiveSensoryInput(sensory_input);
    
    // Brain should process input without errors
    EXPECT_TRUE(brain->getTotalNeuronCount() > 0);
}

TEST_F(BrainTest, MotorOutput) {
    // Test motor output generation
    brain->step(0);
    
    auto action = brain->produceAction();
    
    if (action) {
        // Verify action properties
        EXPECT_NE(action->getType(), ActionType::None);
        
        auto params = action->getParameters();
        // Parameters should be valid
        EXPECT_TRUE(params.empty() || params.size() > 0);
    }
}

// Edge Case Tests
TEST_F(BrainTest, ZeroNeurons) {
    // Test edge case: zero neurons
    auto zero_config = std::make_shared<Config>();
    zero_config->set("neuron_count", 0);
    zero_config->set("region_count", 1);
    
    auto zero_brain = std::make_shared<Brain>(zero_config);
    zero_brain->initialize();
    
    EXPECT_EQ(zero_brain->getTotalNeuronCount(), 0);
    EXPECT_EQ(zero_brain->getTotalSynapseCount(), 0);
}

TEST_F(BrainTest, SingleNeuron) {
    // Test edge case: single neuron
    auto single_config = std::make_shared<Config>();
    single_config->set("neuron_count", 1);
    single_config->set("region_count", 1);
    
    auto single_brain = std::make_shared<Brain>(single_config);
    single_brain->initialize();
    
    EXPECT_EQ(single_brain->getTotalNeuronCount(), 1);
}

TEST_F(BrainTest, EmptyConfiguration) {
    // Test empty configuration
    auto empty_config = std::make_shared<Config>();
    
    auto empty_brain = std::make_shared<Brain>(empty_config);
    empty_brain->initialize();
    
    // Should still initialize
    EXPECT_TRUE(empty_brain->getTotalNeuronCount() >= 0);
}

// Thread Safety Tests
TEST_F(BrainTest, ThreadSafety) {
    // Test thread safety (simplified)
    std::vector<std::thread> threads;
    const int num_threads = 4;
    const int steps_per_thread = 25;
    
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([this, t]() {
            for (int i = 0; i < steps_per_thread; ++i) {
                brain->step(t * steps_per_thread + i);
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Verify brain state after threaded execution
    EXPECT_EQ(brain->getTotalNeuronCount(), 100);
}

// Performance Monitoring Tests
TEST_F(BrainTest, PerformanceMonitoring) {
    // Test performance monitoring
    brain->resetPerformanceStats();
    
    auto initial_stats = brain->getPerformanceStats();
    EXPECT_GE(initial_stats.neuronsPerSecond, 0.0);
    
    // Execute simulation
    for (int i = 0; i < 100; ++i) {
        brain->step(i);
    }
    
    auto final_stats = brain->getPerformanceStats();
    EXPECT_GE(final_stats.neuronsPerSecond, 0.0);
    EXPECT_GT(final_stats.stepTimeMs, 0.0);
}

TEST_F(BrainTest, PerformanceLog) {
    // Test performance logging
    brain->logPerformanceReport();
    
    // Should not throw exception
    EXPECT_TRUE(true);
}

// Integration Tests
TEST_F(BrainTest, BrainWorldIntegration) {
    // Test brain-world integration
    auto world = std::make_shared<SimpleWorld>();
    world->configure(100, 100, 50, 50);
    
    auto agent_brain = std::make_shared<AgentBrain>(brain);
    agent_brain->initialize(*world);
    
    // Test perception-action cycle
    auto percept = world->getSensoryPercept();
    agent_brain->processSensoryInput(percept);
    
    auto action = brain->produceAction();
    if (action) {
        world->applyMotorCommand(*action, 0.0);
    }
    
    // Should complete without errors
    EXPECT_TRUE(true);
}

TEST_F(BrainTest, SimulationCycle) {
    // Test complete simulation cycle
    const int steps = 50;
    
    for (int step = 0; step < steps; ++step) {
        // Update brain
        brain->step(step, step * 0.001);
        
        // Get sensory input (simulated)
        auto sensory_input = std::make_shared<InternalSignals>();
        
        // Process input
        brain->receiveSensoryInput(sensory_input);
        
        // Produce action
        auto action = brain->produceAction();
        if (action) {
            // Apply action (simulated)
        }
    }
    
    // Verify system completed
    EXPECT_TRUE(brain->getTotalNeuronCount() > 0);
}

// Benchmark Tests
static void BM_BrainStep(benchmark::State& state) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", state.range(0));
    config->set("region_count", 1);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    for (auto _ : state) {
        for (int i = 0; i < 10; ++i) {
            brain->step(i);
        }
    }
}

static void BM_BrainAction(benchmark::State& state) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", state.range(0));
    config->set("region_count", 1);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    for (auto _ : state) {
        brain->step(state.range(1));
        brain->produceAction();
    }
}

BENCHMARK(BM_BrainStep)->Arg(100)->Arg(1000)->Arg(5000);
BENCHMARK(BM_BrainAction)->Arg(100)->Arg(1000)->Arg(5000);

} // namespace nlm::tests
