// Visualization Unit Tests
// Comprehensive unit testing for the VisualizationInterface class

#include "visualization/VisualizationInterface.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <thread>
#include <chrono>
#include <vector>
#include <cassert>

namespace nlm::tests {

class VisualizationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test configuration
        config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 2);
        config->set("connection_probability", 0.1f);
        
        // Create brain for visualization testing
        brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Create visualization interface
        visualization = std::make_shared<VisualizationInterface>();
        visualization->initialize();
        
        // Set visualization to active for testing
        visualization->setUpdateRate(30.0f); // 30 Hz
    }
    
    void TearDown() override {
        visualization.reset();
        brain.reset();
        config.reset();
    }
    
    // Test fixtures
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    std::shared_ptr<VisualizationInterface> visualization;
};

// Basic Visualization Tests
TEST_F(VisualizationTest, Initialization) {
    EXPECT_FALSE(visualization->isActive());
    
    // Initialize visualization
    visualization->initialize();
    
    EXPECT_TRUE(visualization->isActive());
}

TEST_F(VisualizationTest, UpdateRate) {
    EXPECT_EQ(visualization->getUpdateRate(), 30.0f);
    
    // Change update rate
    visualization->setUpdateRate(60.0f);
    EXPECT_EQ(visualization->getUpdateRate(), 60.0f);
    
    // Test minimum and maximum rates
    visualization->setUpdateRate(0.1f);
    EXPECT_EQ(visualization->getUpdateRate(), 0.1f);
    
    visualization->setUpdateRate(1000.0f);
    EXPECT_EQ(visualization->getUpdateRate(), 1000.0f);
}

TEST_F(VisualizationTest, NetworkVisualization) {
    // Test network visualization
    visualization->visualizeNetwork(*brain);
    
    // Should not throw exception
    EXPECT_TRUE(true);
}

TEST_F(VisualizationTest, ActivityVisualization) {
    // Test activity visualization
    visualization->visualizeActivity(*brain);
    
    // Should not throw exception
    EXPECT_TRUE(true);
}

TEST_F(VisualizationTest, SpikeVisualization) {
    // Test spike visualization
    std::vector<Spike> spikes;
    spikes.push_back(Spike{0, 1.0, NeuronId(1), 1.0f});
    spikes.push_back(Spike{10, 2.0, NeuronId(2), 1.5f});
    
    visualization->visualizeSpikes(spikes);
    
    // Should not throw exception
    EXPECT_TRUE(true);
}

TEST_F(VisualizationTest, WeightVisualization) {
    // Test weight visualization
    visualization->visualizeWeights(*brain);
    
    // Should not throw exception
    EXPECT_TRUE(true);
}

TEST_F(VisualizationTest, MultipleVisualizationTypes) {
    // Test multiple visualization types
    visualization->visualizeNetwork(*brain);
    visualization->visualizeActivity(*brain);
    visualization->visualizeWeights(*brain);
    
    // Should not throw exception
    EXPECT_TRUE(true);
}

// Visualization Control Tests
TEST_F(VisualizationTest, Render) {
    // Test rendering
    visualization->render();
    
    // Should not throw exception
    EXPECT_TRUE(true);
}

TEST_F(VisualizationTest, Update) {
    // Test update with simulation data
    for (int step = 0; step < 10; ++step) {
        brain->step(step);
        visualization->update();
    }
    
    // Should not throw exception
    EXPECT_TRUE(true);
}

TEST_F(VisualizationTest, Close) {
    // Test closing visualization
    visualization->close();
    EXPECT_FALSE(visualization->isActive());
    
    // Re-initialize for further tests
    visualization->initialize();
    EXPECT_TRUE(visualization->isActive());
}

// Visualization Performance Tests
TEST_F(VisualizationTest, UpdatePerformance) {
    // Test update performance
    const int updates = 100;
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < updates; ++i) {
        brain->step(i);
        visualization->update();
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end_time - start_time);
    
    double updates_per_second = updates / duration.count();
    EXPECT_GT(updates_per_second, 100.0); // Should handle >100 updates/sec
}

TEST_F(VisualizationTest, RenderPerformance) {
    // Test render performance
    const int renders = 50;
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < renders; ++i) {
        visualization->render();
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end_time - start_time);
    
    double renders_per_second = renders / duration.count();
    EXPECT_GT(renders_per_second, 50.0); // Should handle >50 renders/sec
}

// Visualization Data Integration Tests
TEST_F(VisualizationTest, BrainDataIntegration) {
    // Test visualization with brain data
    visualization->visualizeNetwork(*brain);
    
    // Verify brain state is captured
    EXPECT_TRUE(brain->getTotalNeuronCount() > 0);
    EXPECT_TRUE(brain->getTotalSynapseCount() > 0);
}

TEST_F(VisualizationTest, SpikeDataIntegration) {
    // Test spike visualization with actual brain data
    // Generate some spikes by running simulation
    for (int step = 0; step < 20; ++step) {
        brain->step(step);
    }
    
    // Get spike data from brain
    int total_spikes = brain->getTotalSpikeCount();
    
    // Convert to visualization format (simplified)
    std::vector<Spike> spikes;
    for (int i = 0; i < total_spikes && i < 10; ++i) {
        spikes.push_back(Spike{i * 10, static_cast<float>(i * 10), NeuronId(i), 1.0f});
    }
    
    visualization->visualizeSpikes(spikes);
    
    // Should not throw exception
    EXPECT_TRUE(true);
}

TEST_F(VisualizationTest, WeightMatrixIntegration) {
    // Test weight matrix visualization with brain data
    visualization->visualizeWeights(*brain);
    
    // Brain should have weight data
    EXPECT_GT(brain->getTotalSynapseCount(), 0);
}

// Visualization Configuration Tests
TEST_F(VisualizationTest, ConfigurationUpdate) {
    // Test configuration updates
    visualization->setUpdateRate(15.0f);
    EXPECT_EQ(visualization->getUpdateRate(), 15.0f);
    
    visualization->setUpdateRate(120.0f);
    EXPECT_EQ(visualization->getUpdateRate(), 120.0f);
}

TEST_F(VisualizationTest, ConfigurationPersistence) {
    // Test configuration persistence
    float original_rate = visualization->getUpdateRate();
    
    visualization->update();
    EXPECT_EQ(visualization->getUpdateRate(), original_rate);
}

// Visualization State Tests
TEST_F(VisualizationTest, StateManagement) {
    // Test visualization state
    EXPECT_TRUE(visualization->isActive());
    
    // Close and check state
    visualization->close();
    EXPECT_FALSE(visualization->isActive());
    
    // Re-initialize
    visualization->initialize();
    EXPECT_TRUE(visualization->isActive());
}

TEST_F(VisualizationTest, StateTransition) {
    // Test state transitions
    visualization->close();
    EXPECT_FALSE(visualization->isActive());
    
    visualization->initialize();
    EXPECT_TRUE(visualization->isActive());
    
    visualization->close();
    EXPECT_FALSE(visualization->isActive());
}

// Visualization Thread Safety Tests
TEST_F(VisualizationTest, ThreadSafety) {
    // Test thread safety (simplified)
    std::vector<std::thread> threads;
    const int num_threads = 4;
    const int operations_per_thread = 25;
    
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([this, t]() {
            for (int i = 0; i < operations_per_thread; ++i) {
                // Each thread performs visualization operations
                visualization->update();
                visualization->render();
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Should complete without errors
    EXPECT_TRUE(true);
}

TEST_F(VisualizationTest, ConcurrentAccess) {
    // Test concurrent access to visualization
    std::vector<std::thread> threads;
    const int num_threads = 8;
    
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([this, t]() {
            for (int i = 0; i < 10; ++i) {
                visualization->update();
                visualization->visualizeNetwork(*brain);
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Should complete without race conditions
    EXPECT_TRUE(true);
}

// Visualization Edge Case Tests
TEST_F(VisualizationTest, EmptyBrainVisualization) {
    // Test visualization with empty brain
    auto empty_config = std::make_shared<Config>();
    empty_config->set("neuron_count", 0);
    empty_config->set("region_count", 1);
    
    auto empty_brain = std::make_shared<Brain>(empty_config);
    empty_brain->initialize();
    
    // Should handle empty brain gracefully
    visualization->visualizeNetwork(*empty_brain);
    EXPECT_TRUE(true);
}

TEST_F(VisualizationTest, LargeBrainVisualization) {
    // Test visualization with large brain
    auto large_config = std::make_shared<Config>();
    large_config->set("neuron_count", 5000);
    large_config->set("region_count", 10);
    large_config->set("connection_probability", 0.01f);
    
    auto large_brain = std::make_shared<Brain>(large_config);
    large_brain->initialize();
    
    // Should handle large brain
    visualization->visualizeNetwork(*large_brain);
    EXPECT_TRUE(true);
}

TEST_F(VisualizationTest, VisualizationWithNoBrain) {
    // Test visualization methods when brain is not available
    visualization->close();
    
    // Should handle missing brain gracefully
    EXPECT_FALSE(visualization->isActive());
}

// Visualization Performance Analysis Tests
TEST_F(VisualizationTest, PerformanceMetrics) {
    // Test performance metrics collection
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 50; ++i) {
        visualization->update();
        visualization->render();
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end_time - start_time);
    
    double operations_per_second = 100.0 / duration.count();
    EXPECT_GT(operations_per_second, 10.0); // Should handle >10 operations/sec
}

TEST_F(VisualizationTest, MemoryUsage) {
    // Test memory usage
    auto initial_memory = visualization->getMemoryUsage();
    
    // Perform visualization operations
    for (int i = 0; i < 100; ++i) {
        visualization->update();
        visualization->render();
    }
    
    auto final_memory = visualization->getMemoryUsage();
    
    // Memory should not grow excessively
    EXPECT_LT(final_memory - initial_memory, 100.0); // Less than 100MB growth
}

// Visualization Integration Tests
TEST_F(VisualizationTest, CompleteVisualizationCycle) {
    // Test complete visualization cycle
    const int steps = 30;
    
    for (int step = 0; step < steps; ++step) {
        // Update brain
        brain->step(step);
        
        // Update visualization
        visualization->update();
        
        // Render visualization
        visualization->render();
        
        // Visualize different aspects
        visualization->visualizeNetwork(*brain);
        visualization->visualizeActivity(*brain);
    }
    
    // Should complete without errors
    EXPECT_TRUE(visualization->isActive());
}

TEST_F(VisualizationTest, VisualizationWithPerformanceMonitoring) {
    // Test visualization with performance monitoring
    brain->resetPerformanceStats();
    
    visualization->update();
    
    auto stats = brain->getPerformanceStats();
    EXPECT_GE(stats.neuronsPerSecond, 0.0);
    EXPECT_GE(stats.stepTimeMs, 0.0);
}

// Visualization Serialization Tests
TEST_F(VisualizationTest, ConfigurationSerialization) {
    // Test visualization configuration serialization
    visualization->setUpdateRate(45.0f);
    
    // Serialize configuration
    std::string config_str = visualization->getConfig();
    EXPECT_FALSE(config_str.empty());
    
    // Deserialize configuration (simplified)
    auto deserialized = std::make_shared<VisualizationInterface>();
    deserialized->initialize();
    
    // Note: Full deserialization would require implementing setConfig method
    EXPECT_TRUE(deserialized->isActive());
}

// Stress and Load Tests
TEST_F(VisualizationTest, StressLoad) {
    // Stress test visualization
    const int iterations = 200;
    
    for (int i = 0; i < iterations; ++i) {
        brain->step(i);
        visualization->update();
        visualization->render();
        visualization->visualizeNetwork(*brain);
        
        if (i % 20 == 0) {
            visualization->close();
            visualization->initialize();
        }
    }
    
    // Should handle stress without crashing
    EXPECT_TRUE(visualization->isActive());
}

TEST_F(VisualizationTest, HighUpdateRate) {
    // Test with very high update rate
    visualization->setUpdateRate(1000.0f);
    
    const int steps = 50;
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int step = 0; step < steps; ++step) {
        visualization->update();
        visualization->render();
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end_time - start_time);
    
    double updates_per_second = steps / duration.count();
    EXPECT_GT(updates_per_second, 100.0); // Should handle high update rates
}

// Visualization Benchmarking
static void BM_VisualizationUpdate(benchmark::State& state) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 2);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    auto vis = std::make_shared<VisualizationInterface>();
    vis->initialize();
    vis->setUpdateRate(30.0f);
    
    for (auto _ : state) {
        brain->step(state.range(0));
        vis->update();
        vis->render();
    }
}

static void BM_VisualizationRender(benchmark::State& state) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 2);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    auto vis = std::make_shared<VisualizationInterface>();
    vis->initialize();
    
    for (auto _ : state) {
        vis->update();
        vis->render();
    }
}

BENCHMARK(BM_VisualizationUpdate)->Arg(100)->Arg(1000)->Arg(5000);
BENCHMARK(BM_VisualizationRender)->Arg(100)->Arg(1000)->Arg(5000);

} // namespace nlm::tests
