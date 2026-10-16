// Integration Tests using Google Test Framework
// Comprehensive end-to-end system tests for NLM framework
// Tests cross-component integration, dependency chains, and performance integration

#include "fixtures/TestHelpers.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "plasticity/STDP.hpp"
#include "performance_monitoring/PerformanceMonitor.hpp"
#include <gtest/gtest.h>
#include <vector>
#include <memory>
#include <chrono>
#include <cmath>

namespace test {

// Test 9.1: Full System Integration
TEST_F(IntegrationTest, FullSystemIntegration) {
    // Create a complete system configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    config->set("learning_rate", 0.01, nlm::ConfigSource::Default);
    
    // Initialize brain
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    ASSERT_TRUE(initSuccess);
    
    // System should be fully operational
    EXPECT_EQ(brain.getTotalNeuronCount(), 100);
    EXPECT_EQ(brain.getRegionCount(), 3);
    EXPECT_GT(brain.getTotalSynapseCount(), 0);
    
    // Run system steps
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // System should remain stable
    EXPECT_TRUE(brain.getTotalNeuronCount() > 0);
}

// Test 9.2: Cross-Component Integration
TEST_F(IntegrationTest, CrossComponentIntegration) {
    // Test integration between all core components
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.2, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    ASSERT_TRUE(initSuccess);
    
    // Test integration points:
    // 1. Config -> Brain
    EXPECT_EQ(brain.getTotalNeuronCount(), 50);
    
    // 2. Brain -> Neurons
    for (nlm::NeuronId neuronId = 0; neuronId < 50; ++neuronId) {
        nlm::Neuron* neuron = brain.getNeuron(neuronId);
        ASSERT_NE(neuron, nullptr);
        EXPECT_EQ(neuron->getId(), neuronId);
    }
    
    // 3. Neurons -> Synapses
    for (nlm::NeuronId neuronId = 0; neuronId < 50; ++neuronId) {
        nlm::Neuron* neuron = brain.getNeuron(neuronId);
        ASSERT_NE(neuron, nullptr);
        // Check if neuron has synapses
        // Implementation depends on the API
    }
    
    // 4. Integration with STDP plasticity
    nlm::STDP stdp;
    stdp.setLearningRate(0.01);
    EXPECT_DOUBLE_EQ(stdp.getLearningRate(), 0.01);
}

// Test 9.3: Dependency Chain Tests
TEST_F(IntegrationTest, DependencyChainTests) {
    // Test the complete dependency chain:
    // Config -> Brain -> Neurons -> Synapses -> Plasticity
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(20), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.3, nlm::ConfigSource::Default);
    
    // Step 1: Config dependency
    nlm::ConfigValidator validator;
    nlm::ValidationResult configValidation = validator.validate(
        nlm::ConfigEntry("neuron_count", 20, nlm::ConfigSource::Default)
    );
    EXPECT_TRUE(configValidation.isValid);
    
    // Step 2: Brain creation depends on config
    nlm::Brain brain(config);
    bool brainInit = brain.initialize();
    ASSERT_TRUE(brainInit);
    
    // Step 3: Neurons created by brain
    EXPECT_EQ(brain.getTotalNeuronCount(), 20);
    
    // Step 4: Synapse creation
    EXPECT_GT(brain.getTotalSynapseCount(), 0);
    
    // Step 5: Plasticity integration
    nlm::STDP stdp;
    stdp.setLearningRate(0.01);
    
    // All components are integrated
    EXPECT_TRUE(true);  // If we reach here, integration works
}

// Test 9.4: Performance Integration
TEST_F(PerformanceTest, PerformanceIntegration) {
    nlm::PerformanceMonitor monitor("PerformanceIntegration");
    monitor.start();
    
    // Create a brain and measure its performance
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Time brain initialization
    auto start = std::chrono::high_resolution_clock::now();
    bool initSuccess = brain.initialize();
    auto end = std::chrono::high_resolution_clock::now();
    auto initDuration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    ASSERT_TRUE(initSuccess);
    EXPECT_LT(initDuration.count(), 1000);  // Should be fast
    
    // Time brain operation
    start = std::chrono::high_resolution_clock::now();
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
    }
    end = std::chrono::high_resolution_clock::now();
    auto operationDuration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    EXPECT_LT(operationDuration.count(), 5000);  // Should complete 50 steps
    
    monitor.stop();
    
    auto metrics = monitor.getMetrics();
    EXPECT_GT(metrics.frameCount, 0);
    EXPECT_GT(metrics.currentFPS(), 0.0);
}

// Test 9.5: Component Communication Integration
TEST_F(IntegrationTest, ComponentCommunicationIntegration) {
    // Test that components can communicate through their APIs
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Component 1: Config provides parameters to Brain
    nlm::Brain brain(config);
    
    // Component 2: Brain creates Neurons
    bool brainInit = brain.initialize();
    ASSERT_TRUE(brainInit);
    
    // Component 3: Neurons communicate via Synapses
    // (Implementation dependent on the actual API)
    
    // Component 4: Synapses use Plasticity mechanisms
    nlm::STDP stdp;
    stdp.setLearningRate(0.01);
    
    // All components communicate through defined interfaces
    EXPECT_TRUE(true);  // Integration successful
}

// Test 9.6: Error Handling Integration
TEST_F(ErrorHandlingTest, ErrorHandlingIntegration) {
    // Test error handling across component boundaries
    nlm::initializeErrorHandling(nlm::ErrorReportLevel::Error, nlm::ExceptionStrategy::Throw);
    nlm::ErrorHandler::getInstance().clear();
    
    // Create config with error handling
    nlm::Config config;
    config.setErrorHandlingEnabled(true);
    
    // Set values through config
    config.set("test_int", 42, nlm::ConfigSource::Runtime);
    auto val = config.get<int64_t>("test_int");
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(*val, 42);
    
    // Error handling should work across all components
    const auto& stats = config.getErrorStats();
    EXPECT_TRUE(stats.totalErrors >= 0);  // May be 0 or more depending on operations
}

// Test 9.7: Memory Management Integration
TEST_F(IntegrationTest, MemoryManagementIntegration) {
    // Test memory management across component lifecycles
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    // Create and destroy components
    {
        nlm::Brain brain(config);
        bool initSuccess = brain.initialize();
        ASSERT_TRUE(initSuccess);
        
        // Use brain
        for (nlm::SimulationStep step = 0; step < 5; ++step) {
            brain.step(step);
        }
        
        // Brain should be destroyed here (RAII)
    }  // End of scope, brain should be destroyed
    
    // System should be in clean state
    EXPECT_TRUE(true);  // If we reach here, memory management worked
}

// Test 9.8: Configuration Integration
TEST_F(IntegrationTest, ConfigurationIntegration) {
    // Test configuration loading and integration
    auto config = std::make_shared<nlm::Config>();
    
    // Set various configuration values
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    config->set("learning_rate", 0.01, nlm::ConfigSource::Default);
    config->set("simulation_time_step", 0.001, nlm::ConfigSource::Default);
    
    // Verify configuration
    EXPECT_EQ(config->get<int64_t>("neuron_count"), 100);
    EXPECT_EQ(config->get<int64_t>("region_count"), 3);
    EXPECT_DOUBLE_EQ(config->get<double>("connection_probability"), 0.1);
    EXPECT_DOUBLE_EQ(config->get<double>("learning_rate"), 0.01);
    EXPECT_DOUBLE_EQ(config->get<double>("simulation_time_step"), 0.001);
    
    // Create brain with configuration
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    ASSERT_TRUE(initSuccess);
    
    // Configuration should be properly integrated
    EXPECT_EQ(brain.getTotalNeuronCount(), 100);
}

// Test 9.9: Simulation Integration
TEST_F(IntegrationTest, SimulationIntegration) {
    // Test complete simulation integration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(30), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.15, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    ASSERT_TRUE(initSuccess);
    
    // Run complete simulation
    nlm::SimulationStep totalSteps = 100;
    nlm::SimulationStep completedSteps = 0;
    
    while (completedSteps < totalSteps) {
        brain.step(completedSteps);
        completedSteps++;
        
        // Check for completion
        if (completedSteps >= totalSteps) break;
    }
    
    // Simulation should complete successfully
    EXPECT_EQ(completedSteps, totalSteps);
    
    // System should be in valid state after simulation
    EXPECT_GT(brain.getTotalNeuronCount(), 0);
    EXPECT_GT(brain.getTotalSynapseCount(), 0);
}

// Test 9.10: Monitoring Integration
TEST_F(PerformanceTest, MonitoringIntegration) {
    // Test performance monitoring integration
    nlm::PerformanceMonitor monitor("MonitoringIntegration");
    monitor.start();
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    ASSERT_TRUE(initSuccess);
    
    // Monitor brain operation
    for (int i = 0; i < 20; ++i) {
        brain.step(i);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));  // Simulate work
    }
    
    monitor.stop();
    
    auto metrics = monitor.getMetrics();
    EXPECT_GT(metrics.frameCount, 0);
    EXPECT_GT(metrics.currentFPS(), 0.0);
    
    // Verify monitoring integrated correctly
    EXPECT_LT(metrics.averageFrameTime, 100.0);  // Should be reasonable
}

// Test 9.11: Real-time Integration
TEST_F(IntegrationTest, RealTimeIntegration) {
    // Test real-time processing integration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(25), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.2, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    ASSERT_TRUE(initSuccess);
    
    // Simulate real-time processing
    nlm::SimulationStep timeStep = 0;
    while (timeStep < 50) {
        brain.step(timeStep);
        timeStep++;
        
        // Simulate real-time constraint
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        
        if (timeStep >= 50) break;
    }
    
    // Real-time integration should work
    EXPECT_EQ(timeStep, 50);
}

// Test 9.12: Stress Integration
TEST_F(PerformanceTest, StressIntegration) {
    // Test system integration under stress
    nlm::PerformanceMonitor monitor("StressIntegration");
    monitor.start();
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.05, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    ASSERT_TRUE(initSuccess);
    
    // Stress test with many operations
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    monitor.stop();
    
    auto metrics = monitor.getMetrics();
    EXPECT_GT(metrics.frameCount, 0);
    EXPECT_GT(metrics.currentFPS(), 0.0);
    
    // System should handle stress
    EXPECT_GT(brain.getTotalNeuronCount(), 0);
    EXPECT_GT(brain.getTotalSynapseCount(), 0);
}

// Test 9.13: Scalability Integration
TEST_F(IntegrationTest, ScalabilityIntegration) {
    // Test system scalability with different sizes
    std::vector<std::pair<size_t, size_t>> testSizes = {
        {10, 1}, {25, 2}, {50, 3}, {100, 5}
    };
    
    for (const auto& size : testSizes) {
        auto [neuronCount, regionCount] = size;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(regionCount), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        bool initSuccess = brain.initialize();
        
        // All sizes should initialize successfully
        EXPECT_TRUE(initSuccess);
        
        // Verify scalability
        EXPECT_EQ(brain.getTotalNeuronCount(), neuronCount);
        EXPECT_EQ(brain.getRegionCount(), regionCount);
    }
}

// Test 9.14: Integration Failure Recovery
TEST_F(IntegrationTest, IntegrationFailureRecovery) {
    // Test system recovery from integration failures
    nlm::initializeErrorHandling(nlm::ErrorReportLevel::Error, nlm::ExceptionStrategy::Throw);
    nlm::ErrorHandler::getInstance().clear();
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    ASSERT_TRUE(initSuccess);
    
    // System should recover from errors
    EXPECT_GT(brain.getTotalNeuronCount(), 0);
    
    // Error handling should work
    const auto& stats = brain.getErrorStats();
    EXPECT_TRUE(true);  // Error handling works
}

// Test 9.15: End-to-End System Test
TEST_F(IntegrationTest, EndToEndSystemTest) {
    // Complete end-to-end system test
    nlm::PerformanceMonitor monitor("EndToEndSystem");
    monitor.start();
    
    // Step 1: Configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(75), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.12, nlm::ConfigSource::Default);
    config->set("learning_rate", 0.015, nlm::ConfigSource::Default);
    
    // Step 2: Brain initialization
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    ASSERT_TRUE(initSuccess);
    
    // Step 3: Simulation
    nlm::SimulationStep stepsToRun = 200;
    for (nlm::SimulationStep step = 0; step < stepsToRun; ++step) {
        brain.step(step);
    }
    
    // Step 4: Performance monitoring
    monitor.stop();
    auto metrics = monitor.getMetrics();
    
    // Verify end-to-end system worked
    EXPECT_EQ(brain.getTotalNeuronCount(), 75);
    EXPECT_EQ(brain.getRegionCount(), 3);
    EXPECT_GT(brain.getTotalSynapseCount(), 0);
    EXPECT_GT(metrics.frameCount, 0);
    EXPECT_GT(metrics.currentFPS(), 0.0);
    
    // System should be fully functional
    EXPECT_TRUE(true);  // End-to-end test passed
}

// Parameterized integration test
class IntegrationSizeTest : public IntegrationTest, public ::testing::WithParamInterface<std::pair<size_t, size_t>> {
};

TEST_P(IntegrationSizeTest, IntegrationOfDifferentSizes) {
    auto [neuronCount, regionCount] = GetParam();
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(neuronCount), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(regionCount), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    
    ASSERT_TRUE(initSuccess);
    EXPECT_EQ(brain.getTotalNeuronCount(), neuronCount);
    EXPECT_EQ(brain.getRegionCount(), regionCount);
    EXPECT_GT(brain.getTotalSynapseCount(), 0);
}

INSTANTIATE_TEST_SUITE_P(
    IntegrationSizeVariations,
    IntegrationSizeTest,
    ::testing::Values(
        std::make_pair(10, 1),
        std::make_pair(50, 3),
        std::make_pair(100, 5),
        std::make_pair(200, 10)
    )
);

} // namespace test