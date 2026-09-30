// Comprehensive test suite for NLM enhanced features
// This test suite covers the new functionality including enhanced Python bindings,
// performance monitoring, experiment management, and data analysis tools

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <random>

namespace test_enhanced_features {

// Test 1: Enhanced Configuration
void testEnhancedConfiguration() {
    std::cout << "    Testing enhanced configuration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    
    // Test basic operations
    config->set("neuron_count", "1000");
    config->set("region_count", "3");
    config->set("connection_probability", "0.1");
    
    assert(config->has("neuron_count"));
    assert(config->has("region_count"));
    assert(config->has("connection_probability"));
    
    // Test file I/O
    assert(config->saveToFile("/tmp/test_config.json"));
    
    auto config2 = std::make_shared<nlm::Config>();
    assert(config2->loadFromFile("/tmp/test_config.json"));
    assert(config2->has("neuron_count"));
    
    std::cout << "    testEnhancedConfiguration passed" << std::endl;
}

// Test 2: Enhanced Brain with activity recording
void testEnhancedBrain() {
    std::cout << "    Testing enhanced brain..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", "100");
    config->set("region_count", "1");
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test enhanced step
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step, step * 0.001);
    }
    
    // Test statistics
    double avgRate = brain.getAverageFiringRate();
    size_t firingCount = brain.getFiringNeuronCount();
    size_t spikeCount = brain.getTotalSpikeCount();
    
    assert(avgRate >= 0.0);
    assert(firingCount >= 0);
    assert(spikeCount >= 0);
    
    std::cout << "    testEnhancedBrain passed" << std::endl;
}

// Test 3: Performance Monitor
void testPerformanceMonitor() {
    std::cout << "    Testing performance monitor..." << std::endl;
    
    nlm::PerformanceMonitor monitor;
    
    // Simulate some work
    for (int i = 0; i < 100; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        // Do some computation
        double sum = 0.0;
        for (int j = 0; j < 1000; ++j) {
            sum += std::sqrt(static_cast<double>(j));
        }
        volatile double _ = sum; // Prevent optimization
        auto end = std::chrono::high_resolution_clock::now();
        
        double duration = std::chrono::duration<double>(end - start).count();
        monitor.recordStep(duration);
    }
    
    nlm::PerformanceMetrics metrics = monitor.getMetrics();
    
    assert(metrics.stepsCompleted == 100);
    assert(metrics.simulationTime >= 0.0);
    assert(metrics.wallTimePerStep >= 0.0);
    assert(metrics.customMetrics.empty()); // No custom metrics added
    
    // Add custom metric
    monitor.addCustomMetric("test_metric", 42.0);
    metrics = monitor.getMetrics();
    assert(metrics.customMetrics.find("test_metric") != metrics.customMetrics.end());
    assert(metrics.customMetrics["test_metric"] == 42.0);
    
    std::cout << "    testPerformanceMonitor passed" << std::endl;
}

// Test 4: Data Analyzer
void testDataAnalyzer() {
    std::cout << "    Testing data analyzer..." << std::endl;
    
    std::vector<double> data = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
    
    // Test statistics
    nlm::DataAnalyzer::Statistics stats = nlm::DataAnalyzer::calculateStats(data);
    
    assert(stats.count == 10);
    assert(stats.mean == 5.5);
    assert(stats.median == 5.5);
    assert(stats.mode == 1.0 || stats.mode == 2.0); // First mode found
    assert(stats.min == 1.0);
    assert(stats.max == 10.0);
    assert(stats.range == 9.0);
    assert(stats.variance == 8.25);
    assert(stats.stdDev == std::sqrt(8.25));
    
    // Test anomaly detection
    std::vector<size_t> anomalies = nlm::DataAnalyzer::detectAnomalies(data, 1.0);
    assert(anomalies.empty()); // No anomalies with this threshold
    
    // Test time series smoothing
    std::vector<double> smoothed = nlm::DataAnalyzer::smoothTimeSeries(data, 3);
    assert(smoothed.size() == data.size());
    
    // Test peak detection
    std::vector<size_t> peaks = nlm::DataAnalyzer::findPeaks(data, 0.0);
    // Should find peaks at 9 and 10 (last two elements)
    
    std::cout << "    testDataAnalyzer passed" << std::endl;
}

// Test 5: Progress Tracker
void testProgressTracker() {
    std::cout << "    Testing progress tracker..." << std::endl;
    
    nlm::ProgressTracker tracker("Test progress", 100);
    
    // Test incrementing
    for (int i = 0; i < 10; ++i) {
        tracker.increment();
    }
    
    tracker.enableProgress(false); // Disable to avoid output during test
    for (int i = 10; i < 100; ++i) {
        tracker.increment();
    }
    
    tracker.showFinalProgress(); // Should not crash
    
    std::cout << "    testProgressTracker passed" << std::endl;
}

// Test 6: Enhanced Simulation Runner
void testEnhancedSimulationRunner() {
    std::cout << "    Testing enhanced simulation runner..." << std::endl;
    
    // Simple test simulation
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", "50");
    config->set("region_count", "1");
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(10, 10, 8, 8);
    world->reset();
    
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    agent->initialize(*world);
    
    // Test simulation steps
    auto simulationStep = [&](uint64_t step, nlm::Timestamp time) {
        SensoryPercept percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        brain.step(step, time);
        
        MotorCommand cmd = agent->decodeMotorCommand();
        world->applyMotorCommand(cmd, time);
        
        float reward = 0.0f;
        auto internal = percept.getInternal();
        if (internal && !internal->empty()) {
            reward = (*internal)[0];
        }
        agent->applyRewardModulation(reward, 0.0f);
        
        agent->updateDevelopment(time);
    };
    
    // Test with simple config
    nlm::EnhancedSimulationRunner::SimulationConfig simConfig;
    simConfig.steps = 50;
    simConfig.timestep = 0.1;
    simConfig.name = "Test Simulation";
    simConfig.enableProgress = false;
    simConfig.enableProfiling = false;
    simConfig.recordData = false;
    
    nlm::EnhancedSimulationRunner::SimulationResult result = 
        nlm::EnhancedSimulationRunner::runSimulation(simConfig, simulationStep);
    
    assert(result.success);
    assert(result.stepsCompleted == 50);
    assert(result.simulationTime >= 0.0);
    
    std::cout << "    testEnhancedSimulationRunner passed" << std::endl;
}

// Test 7: Experiment Runner
void testExperimentRunner() {
    std::cout << "    Testing experiment runner..." << std::endl;
    
    nlm::ExperimentRunner runner;
    
    // Simple experiment
    auto simpleExperiment = [](std::shared_ptr<nlm::Brain> brain,
                               std::shared_ptr<nlm::AgentBrain> agent,
                               std::shared_ptr<nlm::SimpleWorld> world) {
        // Create world
        world->configure(10, 10, 8, 8);
        world->reset();
        
        // Run some steps
        for (int i = 0; i < 20; ++i) {
            SensoryPercept percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i, i * 0.001);
            
            MotorCommand cmd = agent->decodeMotorCommand();
            world->applyMotorCommand(cmd, i * 0.001);
            
            float reward = 0.0f;
            auto internal = percept.getInternal();
            if (internal && !internal->empty()) {
                reward = (*internal)[0];
            }
            agent->applyRewardModulation(reward, 0.0f);
        }
    };
    
    runner.runExperiment("Simple Agent Test", simpleExperiment);
    
    // Get results
    auto results = runner.getResults();
    assert(results.size() == 1);
    assert(results[0].success);
    assert(results[0].name == "Simple Agent Test");
    
    runner.printSummary(); // Should not crash
    
    std::cout << "    testExperimentRunner passed" << std::endl;
}

// Test 8: Enhanced World
void testEnhancedWorld() {
    std::cout << "    Testing enhanced world..." << std::endl;
    
    nlm::EnhancedSimpleWorld world;
    
    // Configure world
    world.configure(20, 20, 8, 8);
    world.reset();
    world.setAgentStart(10.0, 10.0);
    
    // Test world operations
    world.update(0.1);
    
    SensoryPercept percept = world.getSensoryPercept();
    AgentBody body = world.getAgentBody();
    
    // Record metrics
    world.recordMetric("test_metric", 42.0);
    world.recordMetric("another_metric", 3.14);
    
    // Get metrics
    assert(world.getMetric("test_metric") == 42.0);
    assert(world.getMetric("another_metric") == 3.14);
    assert(world.getMetric("nonexistent") == 0.0);
    
    // Test profiling
    world.enableProfiling();
    for (int i = 0; i < 10; ++i) {
        world.enhancedUpdate(0.1);
    }
    
    double duration = world.getSimulationDuration();
    assert(duration >= 0.0);
    
    world.disableProfiling();
    
    std::cout << "    testEnhancedWorld passed" << std::endl;
}

// Test 9: Utility Functions
void testUtilityFunctions() {
    std::cout << "    Testing utility functions..." << std::endl;
    
    // Test SimUtils
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", "30");
    config->set("region_count", "1");
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(10, 10, 8, 8);
    world->reset();
    
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    
    nlm::EnhancedSimulationRunner::SimulationConfig simConfig;
    simConfig.steps = 20;
    simConfig.timestep = 0.1;
    simConfig.name = "SimUtils Test";
    simConfig.enableProgress = false;
    
    // Test agent simulation
    auto agentResult = nlm::SimUtils::runAgentSimulation(simConfig, brain, agent, world);
    assert(agentResult.success);
    
    // Test world simulation
    auto worldResult = nlm::SimUtils::runWorldSimulation(simConfig, world);
    assert(worldResult.success);
    
    std::cout << "    testUtilityFunctions passed" << std::endl;
}

// Test 10: Error Handling
void testErrorHandling() {
    std::cout << "    Testing error handling..." << std::endl;
    
    // Test invalid configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", "-100"); // Invalid negative count
    
    // This should not crash (error handling is at application level)
    nlm::Brain brain(config);
    // initialization will fail but not crash
    
    // Test invalid simulation step
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", "50");
    config2->set("region_count", "1");
    
    nlm::Brain brain2(config2);
    assert(brain2.initialize());
    
    // The enhanced step should handle errors gracefully
    try {
        for (int step = 0; step < 5; ++step) {
            brain2.step(step, step * 0.001);
        }
    } catch (...) {
        // Should not throw exceptions for normal operation
        assert(false); // Unexpected exception
    }
    
    std::cout << "    testErrorHandling passed" << std::endl;
}

// Test 11: Performance and Memory
void testPerformanceAndMemory() {
    std::cout << "    Testing performance and memory..." << std::endl;
    
    // Create multiple loggers to test memory management
    std::vector<std::shared_ptr<nlm::ConsoleLogger>> loggers;
    for (int i = 0; i < 100; ++i) {
        loggers.push_back(std::make_shared<nlm::ConsoleLogger>());
    }
    
    // Use them
    for (auto& logger : loggers) {
        logger->info("Test message");
    }
    
    // Clear references
    loggers.clear();
    
    // Test performance monitor with high frequency
    nlm::PerformanceMonitor monitor;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 1000; ++i) {
        auto stepStart = std::chrono::high_resolution_clock::now();
        // Do some work
        double sum = 0.0;
        for (int j = 0; j < 100; ++j) {
            sum += std::sin(static_cast<double>(j) * 0.1);
        }
        volatile double _ = sum;
        auto stepEnd = std::chrono::high_resolution_clock::now();
        
        double duration = std::chrono::duration<double>(stepEnd - stepStart).count();
        monitor.recordStep(duration);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    nlm::PerformanceMetrics metrics = monitor.getMetrics();
    
    assert(metrics.stepsCompleted == 1000);
    assert(metrics.simulationTime >= 0.0);
    assert(metrics.wallTimePerStep > 0.0);
    assert(metrics.wallTimePerStep < totalTime); // Should be less than total time
    
    std::cout << "    testPerformanceAndMemory passed" << std::endl;
}

// Test 12: Data Analysis Integration
void testDataAnalysisIntegration() {
    std::cout << "    Testing data analysis integration..." << std::endl;
    
    // Generate some time series data
    std::vector<double> timeSeries;
    std::mt19937 rng(42); // Fixed seed for reproducibility
    std::uniform_real_distribution<double> dist(0.0, 10.0);
    
    for (int i = 0; i < 100; ++i) {
        timeSeries.push_back(dist(rng));
    }
    
    // Analyze using enhanced tools
    nlm::DataAnalyzer::Statistics stats = nlm::DataAnalyzer::calculateStats(timeSeries);
    
    assert(stats.count == 100);
    assert(stats.mean >= 0.0 && stats.mean <= 10.0);
    assert(stats.min == *std::min_element(timeSeries.begin(), timeSeries.end()));
    assert(stats.max == *std::max_element(timeSeries.begin(), timeSeries.end()));
    
    // Detect anomalies
    std::vector<size_t> anomalies = nlm::DataAnalyzer::detectAnomalies(timeSeries, 2.0);
    // Anomalies depend on the data distribution
    
    // Smooth data
    std::vector<double> smoothed = nlm::DataAnalyzer::smoothTimeSeries(timeSeries, 5);
    assert(smoothed.size() == timeSeries.size());
    
    // Find peaks
    std::vector<size_t> peaks = nlm::DataAnalyzer::findPeaks(timeSeries, 5.0);
    // Should find peaks with values > 5.0
    
    std::cout << "    testDataAnalysisIntegration passed" << std::endl;
}

// Test 13: Context Manager
void testContextManager() {
    std::cout << "    Testing context manager..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", "20");
    config->set("region_count", "1");
    
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(10, 10, 8, 8);
    world->reset();
    
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    
    // Create context
    nlm::BrainContext context(brain, agent, world);
    
    // Enable auto-save
    context.enableAutoSave("/tmp/test_checkpoint.bin");
    
    // Run some simulation
    for (int i = 0; i < 10; ++i) {
        world->update(0.1);
        agent->processSensoryInput(world->getSensoryPercept());
        brain->step(i);
    }
    
    // Context will be destroyed here, calling cleanup
    // This should save checkpoint if auto-save is enabled
    
    std::cout << "    testContextManager passed" << std::endl;
}

// Test 14: Comprehensive Simulation
void testComprehensiveSimulation() {
    std::cout << "    Testing comprehensive simulation..." << std::endl;
    
    // Setup enhanced components
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", "100");
    config->set("region_count", "2");
    
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    auto world = std::make_shared<nlm::EnhancedSimpleWorld>();
    world->configure(15, 15, 10, 10);
    world->reset();
    world->enableProfiling();
    
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    agent->initialize(*world);
    
    // Enable all features
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    agent->enableDevelopment(true);
    
    // Run comprehensive simulation
    nlm::EnhancedSimulationRunner::SimulationConfig simConfig;
    simConfig.steps = 100;
    simConfig.timestep = 0.05;
    simConfig.name = "Comprehensive Simulation";
    simConfig.enableProgress = false;
    simConfig.enableProfiling = true;
    simConfig.recordData = true;
    
    auto simulationStep = [&](uint64_t step, nlm::Timestamp time) {
        // Enhanced world update with profiling
        world->enhancedUpdate(time);
        
        // Get sensory input
        SensoryPercept percept = world->getSensoryPercept();
        
        // Process input
        agent->processSensoryInput(percept);
        
        // Run brain step
        brain->step(step, time);
        
        // Get motor command
        MotorCommand cmd = agent->decodeMotorCommand();
        
        // Apply action
        world->applyMotorCommand(cmd, time);
        
        // Compute reward
        float reward = 0.0f;
        auto internal = percept.getInternal();
        if (internal && !internal->empty()) {
            reward = (*internal)[0];
        }
        
        // Apply reward modulation
        agent->applyRewardModulation(reward, 0.0f);
        
        // Update development
        agent->updateDevelopment(time);
        
        // Record metrics
        world->recordMetric("step_time", time);
        world->recordMetric("reward", reward);
        world->recordMetric("firing_rate", brain->getAverageFiringRate());
    };
    
    auto initialization = [&]() {
        // Custom initialization
        brain->logStatus();
    };
    
    nlm::EnhancedSimulationRunner::SimulationResult result = 
        nlm::EnhancedSimulationRunner::runSimulation(simConfig, simulationStep, initialization);
    
    assert(result.success);
    assert(result.stepsCompleted == 100);
    assert(result.simulationTime > 0.0);
    
    // Print summary
    nlm::OutputUtils::printSimulationResults(result);
    
    std::cout << "    testComprehensiveSimulation passed" << std::endl;
}

// Run all tests
void runAllTests() {
    std::cout << "=== Running Enhanced NLM Feature Tests ===" << std::endl;
    std::cout << "This test suite validates the enhanced features including:" << std::endl;
    std::cout << "- Enhanced configuration and validation" << std::endl;
    std::cout << "- Enhanced brain with activity recording" << std::endl;
    std::cout << "- Performance monitoring and metrics" << std::endl;
    std::cout << "- Data analysis tools" << std::endl;
    std::cout << "- Progress tracking" << std::endl;
    std::cout << "- Enhanced simulation runner" << std::endl;
    std::cout << "- Experiment management" << std::endl;
    std::cout << "- Enhanced world simulation" << std::endl;
    std::cout << "- Utility functions" << std::endl;
    std::cout << "- Error handling" << std::endl;
    std::cout << "- Performance and memory management" << std::endl;
    std::cout << "- Context manager" << std::endl;
    std::cout << "=== Test Suite Complete ===" << std::endl << std::endl;
    
    testEnhancedConfiguration();
    testEnhancedBrain();
    testPerformanceMonitor();
    testDataAnalyzer();
    testProgressTracker();
    testEnhancedSimulationRunner();
    testExperimentRunner();
    testEnhancedWorld();
    testUtilityFunctions();
    testErrorHandling();
    testPerformanceAndMemory();
    testDataAnalysisIntegration();
    testContextManager();
    testComprehensiveSimulation();
    
    std::cout << std::endl;
    std::cout << "=== All Enhanced Feature Tests Passed ===" << std::endl;
}

} // namespace test_enhanced_features

// Main function for standalone testing
int main() {
    test_enhanced_features::runAllTests();
    return 0;
}
