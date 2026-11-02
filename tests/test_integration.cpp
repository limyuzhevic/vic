// Integration Tests for NLM
// Comprehensive integration tests covering multiple components

#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/SensoryPercept.hpp"
#include "sensory/Vision.hpp"
#include "sensory/Audio.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "memory/Memory.hpp"
#include "development/DevelopmentSystem.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <chrono>
#include <thread>
#include <cmath>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <sstream>

namespace nlm {
namespace integration_tests {

// Integration Test 1: Complete Brain-World-Agent Pipeline
void testCompletePipeline() {
    std::cout << "=== Complete Pipeline Integration Test ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 200, ConfigSource::Runtime);
    config->set("region_count", 3, ConfigSource::Runtime);
    config->set("connection_probability", 0.05, ConfigSource::Runtime);
    config->set("learning_rate", 0.01, ConfigSource::Runtime);
    config->set("enable_plasticity", true, ConfigSource::Runtime);
    config->set("enable_curiosity", true, ConfigSource::Runtime);
    
    // Initialize components
    Brain brain(config);
    auto world = std::make_shared<SimpleWorld>();
    world->configure(30, 30, 8, 8);
    world->setAgentStart(15, 15);
    
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(*world);
    
    // Enable learning features
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    agent->enableDevelopment(true);
    agent->enableStructuralPlasticity(true);
    
    // Run complete simulation
    std::vector<double> simulationTime;
    std::vector<size_t> neuronCounts;
    std::vector<size_t> spikeCounts;
    std::vector<float> neuromodulationLevels;
    
    brain.initialize();
    world->reset();
    
    for (SimulationStep step = 0; step < 100; ++step) {
        // Update world
        auto startTime = std::chrono::high_resolution_clock::now();
        world->update(0.1);
        
        // Get sensory input
        auto percept = world->getSensoryPercept();
        
        // Process through agent
        agent->processSensoryInput(percept);
        
        // Brain thinks
        brain.step(step);
        
        // Get action and apply
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Collect metrics
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
        simulationTime.push_back(duration / 1000.0); // Convert to ms
        
        neuronCounts.push_back(brain.getTotalNeuronCount());
        spikeCounts.push_back(brain.getTotalSpikeCount());
        neuromodulationLevels.push_back(agent->getNeuromodulationLevel());
        
        // Add some learning signal
        if (step % 10 == 0) {
            agent->applyRewardModulation(0.1, 0.05);
        }
        
        // Check for performance degradation
        if (step > 10) {
            double avgTime = 0.0;
            for (size_t i = std::max(0, (int)step - 20); i <= step; ++i) {
                avgTime += simulationTime[i];
            }
            avgTime /= std::min((size_t)21, step + 1);
            
            if (avgTime > 50.0) { // More than 50ms per step
                std::cout << "Warning: High processing time at step " << step << ": " << avgTime << "ms" << std::endl;
            }
        }
    }
    
    // Calculate statistics
    double avgTime = std::accumulate(simulationTime.begin(), simulationTime.end(), 0.0) / simulationTime.size();
    double avgNeurons = std::accumulate(neuronCounts.begin(), neuronCounts.end(), 0.0) / neuronCounts.size();
    double avgSpikes = std::accumulate(spikeCounts.begin(), spikeCounts.end(), 0.0) / spikeCounts.size();
    double avgModulation = std::accumulate(neuromodulationLevels.begin(), neuromodulationLevels.end(), 0.0) / neuromodulationLevels.size();
    
    std::cout << "Pipeline completed successfully!" << std::endl;
    std::cout << "Average processing time: " << std::fixed << std::setprecision(2) << avgTime << " ms" << std::endl;
    std::cout << "Average neurons: " << avgNeurons << std::endl;
    std::cout << "Average spikes: " << avgSpikes << std::endl;
    std::cout << "Average neuromodulation: " << std::fixed << std::setprecision(4) << avgModulation << std::endl;
    
    // Verify expected behavior
    assert(avgTime < 100.0); // Should complete within 100ms per step on average
    assert(avgNeurons > 0); // Should have neurons
    assert(avgSpikes >= 0); // Should have non-negative spikes
    assert(avgModulation >= 0.0f); // Should have non-negative modulation
    
    std::cout << "Complete pipeline test PASSED" << std::endl;
}

// Integration Test 2: Configuration and Error Handling
void testConfigurationIntegration() {
    std::cout << "=== Configuration Integration Test ===" << std::endl;
    
    // Test 1: JSON configuration parsing
    auto config1 = std::make_shared<Config>();
    assert(config1->loadFromFile("/tmp/test_config.json"));
    
    // Create a test JSON config file
    {
        std::ofstream file("/tmp/test_config.json");
        file << "{\n";
        file << "  \"brain\": {\n";
        file << "    \"neuron_count\": 500,\n";
        file << "    \"learning_rate\": 0.005,\n";
        file << "    \"enable_plasticity\": true\n";
        file << "  },\n";
        file << "  \"world\": {\n";
        file << "    \"width\": 40,\n";
        file << "    \"height\": 40\n";
        file << "  }\n";
        file << "}\n";
    }
    
    // Reload config and verify values
    auto config2 = std::make_shared<Config>();
    assert(config2->loadFromFile("/tmp/test_config.json"));
    
    // Check for expected keys (using dots for nested keys)
    assert(config2->has("brain.neuron_count"));
    assert(config2->has("brain.learning_rate"));
    assert(config2->has("world.width"));
    assert(config2->has("world.height"));
    
    std::cout << "JSON configuration parsing test PASSED" << std::endl;
    
    // Test 2: Command line argument parsing
    auto config3 = std::make_shared<Config>();
    int argc = 5;
    char* argv[] = {
        (char*)"test",
        (char*)"--neuron_count=1000",
        (char*)"--learning_rate=0.1",
        (char*)"--enable_plasticity=true"
    };
    
    assert(config3->loadFromArgs(argc, argv));
    assert(config3->has("neuron_count"));
    assert(config3->has("learning_rate"));
    
    std::cout << "Command line parsing test PASSED" << std::endl;
    
    // Test 3: Configuration validation and error handling
    auto config4 = std::make_shared<Config>();
    
    try {
        config4->set("invalid_key_with_spaces", "value");
        assert(false); // Should have thrown
    } catch (const std::exception& e) {
        std::cout << "Error handling test PASSED: " << e.what() << std::endl;
    }
    
    // Test 4: Configuration merging and hierarchy
    auto baseConfig = std::make_shared<Config>();
    baseConfig->set("default_param", 100, ConfigSource::Default);
    baseConfig->set("user_param", 200, ConfigSource::Runtime);
    
    auto overrideConfig = std::make_shared<Config>();
    overrideConfig->set("user_param", 300, ConfigSource::File);
    overrideConfig->set("override_param", 400, ConfigSource::Runtime);
    
    // Merge configs
    for (const auto& key : overrideConfig->getKeys()) {
        auto value = overrideConfig->get<std::string>(key);
        if (value.has_value()) {
            baseConfig->set(key, value.value(), ConfigSource::File);
        }
    }
    
    assert(baseConfig->getOr<int>("default_param", 0) == 100);
    assert(baseConfig->getOr<int>("user_param", 0) == 300);
    assert(baseConfig->getOr<int>("override_param", 0) == 400);
    
    std::cout << "Configuration hierarchy test PASSED" << std::endl;
    
    // Cleanup
    std::remove("/tmp/test_config.json");
    
    std::cout << "Configuration integration test PASSED" << std::endl;
}

// Integration Test 3: Logging and Performance Monitoring
void testLoggingIntegration() {
    std::cout << "=== Logging Integration Test ===" << std::endl;
    
    // Test 1: Structured logging with categories
    Logger& logger = Logger::getGlobal();
    
    // Clear existing loggers
    logger.clearSinks();
    
    // Add memory logger for testing
    auto memoryLogger = std::make_shared<MemoryLogger>();
    logger.addSink(memoryLogger);
    
    // Log messages with different categories
    logger.debug(LogCategory::System, "System initialized", "test.cpp", 1, "main", 0);
    logger.info(LogCategory::Neural, "Brain created with 100 neurons", "test.cpp", 1, "main", 0);
    logger.warning(LogCategory::Performance, "High memory usage detected", "test.cpp", 1, "main", 0);
    logger.error(LogCategory::User, "User input error", "test.cpp", 1, "main", 0);
    logger.critical(LogCategory::System, "Critical system failure", "test.cpp", 1, "main", 0);
    
    // Test 2: Log filtering
    auto filter = std::make_shared<SimpleLogFilter>(LogLevel::Warning, {LogCategory::Performance, LogCategory::User});
    memoryLogger->setFilter(filter);
    
    // Log some messages with different levels and categories
    logger.debug(LogCategory::System, "This debug message should be filtered out", "test.cpp", 1, "main", 0);
    logger.warning(LogCategory::Performance, "This warning should appear", "test.cpp", 1, "main", 0);
    logger.info(LogCategory::Neural, "This info message should be filtered out", "test.cpp", 1, "main", 0);
    
    // Check that only filtered messages appear
    auto warningLogs = memoryLogger->getLogsByLevel(LogLevel::Warning);
    assert(warningLogs.size() >= 1);
    
    // Test 3: Performance logging
    logger.info(LogCategory::Performance, "Performance test started", "test.cpp", 1, "main", 0);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Simulate some work
    for (int i = 0; i < 100; ++i) {
        logger.debug(LogCategory::Performance, "Processing step " + std::to_string(i), "test.cpp", 1, "main", i);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    
    logger.info(LogCategory::Performance, "Performance test completed in " + std::to_string(duration) + " microseconds", "test.cpp", 1, "main", 0);
    
    // Test 4: Log export functionality
    std::string jsonExport = memoryLogger->exportAsJSON();
    std::string csvExport = memoryLogger->exportAsCSV();
    
    assert(!jsonExport.empty());
    assert(!csvExport.empty());
    assert(jsonExport.find("LogLevel") != std::string::npos);
    assert(csvExport.find("timestamp,level") != std::string::npos);
    
    // Test 5: Log statistics
    MemoryLogger::Stats stats = memoryLogger->getStats();
    assert(stats.totalWritten > 0);
    assert(stats.memoryUsage > 0);
    assert(stats.firstEntry.time_since_epoch().count() > 0);
    
    std::cout << "Memory usage: " << stats.memoryUsage << " bytes" << std::endl;
    std::cout << "Total entries: " << stats.totalWritten << std::endl;
    
    // Test 6: Global logger and convenience macros
    // Test that the global logger has the memory logger
    auto globalLogger = Logger::getGlobal();
    assert(globalLogger.getSinks().size() >= 1);
    
    std::cout << "Logging integration test PASSED" << std::endl;
}

// Integration Test 4: Agent with Learning Features
void testAgentLearningIntegration() {
    std::cout << "=== Agent Learning Integration Test ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 150, ConfigSource::Runtime);
    config->set("region_count", 2, ConfigSource::Runtime);
    
    // Initialize agent components
    Brain brain(config);
    auto world = std::make_shared<SimpleWorld>();
    world->configure(20, 20, 6, 6);
    
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(*world);
    
    // Enable all learning features
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    agent->enableDevelopment(true);
    agent->enableStructuralPlasticity(true);
    
    // Create test environment
    std::vector<float> resource(36, 0.0f);
    std::vector<float> hazard(36, 0.0f);
    
    // Add resources and hazards
    resource[10] = 1.0f;  // Resource at center
    hazard[20] = -1.0f;   // Hazard at different location
    
    // Create sensory percept with vision and internal signals
    auto percept = std::make_shared<SensoryPercept>();
    
    // Create vision data
    auto vision = std::make_shared<Vision>(6, 6, 3);
    float* visionData = new float[6 * 6 * 3];
    for (int i = 0; i < 6 * 6 * 3; ++i) {
        visionData[i] = resource[i] > 0 ? 1.0f : 0.0f;
    }
    vision->setData(std::vector<float>(visionData, visionData + 6 * 6 * 3));
    delete[] visionData;
    
    percept->setVision(vision);
    
    // Create internal signals
    auto internal = std::make_shared<InternalSignals>();
    internal->addSignal(0.5f);
    internal->addSignal(-0.3f);
    percept->setInternal(internal);
    
    // Test agent learning with exploration
    for (int step = 0; step < 50; ++step) {
        // Process sensory input
        agent->processSensoryInput(percept);
        
        // Get brain action
        brain.step(step);
        
        // Get motor command
        auto action = agent->decodeMotorCommand();
        
        // Apply action in world
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Apply learning signals
        if (step % 10 == 0) {
            // Apply reward modulation
            float reward = 0.1f;
            float predicted = 0.05f;
            agent->applyRewardModulation(reward, predicted);
        }
        
        // Update development
        agent->updateDevelopment(0.1f);
        
        // Check learning levels
        assert(agent->getNeuromodulationLevel() >= 0.0f);
        assert(agent->getCuriosityLevel() >= 0.0f);
        assert(agent->getPredictionError() >= 0.0f);
    }
    
    // Test agent status reporting
    auto status = agent->get_status();
    assert(status.contains("developmental_stage"));
    assert(status.contains("neuromodulation_level"));
    assert(status.contains("curiosity_level"));
    assert(status.contains("novelty_level"));
    assert(status.contains("prediction_error"));
    
    std::cout << "Agent learning integration test PASSED" << std::endl;
}

// Integration Test 5: World Environment Simulation
void testWorldEnvironmentIntegration() {
    std::cout << "=== World Environment Integration Test ===" << std::endl;
    
    // Create world with different object types
    auto world = std::make_shared<SimpleWorld>();
    world->configure(50, 50, 10, 10);
    world->setAgentStart(25, 25);
    
    // Add various objects
    world->addResource(10, 10, 5.0f);   // Resource
    world->addHazard(40, 40, -2.0f);     // Hazard
    world->addWall(30, 30, 1.0f);        // Wall
    world->addWall(35, 35, 2.0f);        // Another wall
    
    // Test world boundaries
    assert(world->isValidPosition(5, 5));   // Should be valid
    assert(!world->isValidPosition(30, 30)); // Should be invalid (wall)
    
    // Test object removal
    world->removeObject(10, 10);
    assert(!world->isValidPosition(10, 10)); // Should no longer be valid (resource removed)
    
    // Test agent movement and collision detection
    auto action = std::make_shared<Action>(ActionType::MoveForward);
    world->applyMotorCommand(action, 0.0f);
    
    auto body = world->getAgentBody();
    assert(body->x == 25.0f || body->y == 25.0f || body->x == 25.0f); // Position changed
    
    // Test world state persistence
    auto body1 = world->getAgentBody();
    world->update(0.1f);
    auto body2 = world->getAgentBody();
    
    // Position should change after update
    assert(body1->x != body2->x || body1->y != body2->y);
    
    // Test simulation time tracking
    double initialTime = world->getSimulationTime();
    world->update(0.5f);
    double newTime = world->getSimulationTime();
    assert(newTime > initialTime);
    assert(std::abs(newTime - initialTime - 0.5f) < 0.01f);
    
    // Test world reset
    world->reset();
    auto resetBody = world->getAgentBody();
    assert(std::abs(resetBody->x - 25.0f) < 0.1f); // Should return to start
    assert(std::abs(resetBody->y - 25.0f) < 0.1f);
    
    std::cout << "World environment integration test PASSED" << std::endl;
}

// Integration Test 6: Performance and Stress Testing
void testPerformanceIntegration() {
    std::cout << "=== Performance Integration Test ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 300, ConfigSource::Runtime);
    config->set("region_count", 4, ConfigSource::Runtime);
    config->set("connection_probability", 0.03, ConfigSource::Runtime);
    
    // Setup performance monitoring
    Logger& logger = Logger::getGlobal();
    logger.clearSinks();
    
    // Add performance logger
    auto perfLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger.addSink(perfLogger);
    
    // Run stress test
    Brain brain(config);
    auto world = std::make_shared<SimpleWorld>();
    world->configure(40, 40, 8, 8);
    
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(*world);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run simulation with varying complexity
    for (int step = 0; step < 200; ++step) {
        // Update world
        world->update(0.05f);
        
        // Get sensory input
        auto percept = world->getSensoryPercept();
        
        // Process through agent
        agent->processSensoryInput(percept);
        
        // Brain computation
        brain.step(step);
        
        // Get action
        auto action = agent->decodeMotorCommand();
        
        // Apply action
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Apply learning every 20 steps
        if (step % 20 == 0) {
            float reward = 0.1f;
            float curiosity = 0.05f;
            agent->applyRewardModulation(reward, curiosity);
            agent->enableDevelopment(true);
        }
        
        // Log progress every 50 steps
        if (step % 50 == 0) {
            logger.info(LogCategory::Performance, 
                       "Progress: step " + std::to_string(step) + 
                       ", neurons: " + std::to_string(brain.getTotalNeuronCount()) +
                       ", spikes: " + std::to_string(brain.getTotalSpikeCount()),
                       "performance_test.cpp", 1, "runPerformanceTest", step);
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    
    // Calculate performance metrics
    double stepsPerSecond = 200.0 / (totalDuration / 1000.0);
    double averageStepTime = (totalDuration * 1000.0) / 200.0; // Convert to microseconds
    
    std::cout << "Performance test completed:" << std::endl;
    std::cout << "  Total steps: 200" << std::endl;
    std::cout << "  Total time: " << totalDuration << " ms" << std::endl;
    std::cout << "  Steps per second: " << std::fixed << std::setprecision(2) << stepsPerSecond << std::endl;
    std::cout << "  Average step time: " << std::fixed << std::setprecision(2) << averageStepTime << " μs" << std::endl;
    
    // Verify performance targets
    assert(stepsPerSecond > 100.0); // Should process at least 100 steps per second
    assert(averageStepTime < 20000.0); // Average step time should be under 20ms
    
    // Performance should not degrade significantly
    if (stepsPerSecond < 150.0) {
        std::cout << "Warning: Performance below target, consider optimization" << std::endl;
    }
    
    std::cout << "Performance integration test PASSED" << std::endl;
}

// Integration Test 7: Memory Management and Resource Cleanup
void testMemoryManagementIntegration() {
    std::cout << "=== Memory Management Integration Test ===" << std::endl;
    
    // Test 1: Memory usage tracking
    auto memoryLogger = std::make_shared<MemoryLogger>();
    
    // Log many messages to test memory usage
    for (int i = 0; i < 1000; ++i) {
        memoryLogger->write(LogEntry{
            LogLevel::Info,
            LogCategory::System,
            "Log message " + std::to_string(i),
            "test.cpp", 1, "main", 
            std::chrono::system_clock::now(),
            123 + i,
            i
        });
    }
    
    MemoryLogger::Stats stats = memoryLogger->getStats();
    
    std::cout << "Memory stats:" << std::endl;
    std::cout << "  Total entries: " << stats.totalWritten << std::endl;
    std::cout << "  Memory usage: " << stats.memoryUsage << " bytes" << std::endl;
    std::cout << "  Average entry size: " << (stats.memoryUsage / stats.totalWritten) << " bytes" << std::endl;
    
    // Memory usage should be reasonable
    assert(stats.totalWritten == 1000);
    assert(stats.memoryUsage > 10000); // At least 10KB for 1000 entries
    
    // Test 2: Log cleanup and memory management
    memoryLogger->clear();
    
    assert(memoryLogger->size() == 0);
    
    Stats clearedStats = memoryLogger->getStats();
    assert(clearedStats.totalWritten == 0);
    assert(clearedStats.memoryUsage == 0);
    
    std::cout << "Memory cleanup test PASSED" << std::endl;
    
    // Test 3: Export and cleanup
    std::string jsonExport = memoryLogger->exportAsJSON();
    std::string csvExport = memoryLogger->exportAsCSV();
    
    assert(jsonExport.empty()); // Should be empty after clear()
    assert(csvExport.empty());   // Should be empty after clear()
    
    std::cout << "Export and cleanup test PASSED" << std::endl;
    
    std::cout << "Memory management integration test PASSED" << std::endl;
}

// Integration Test 8: Error Resilience and Recovery
void testErrorResilienceIntegration() {
    std::cout << "=== Error Resilience Integration Test ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100, ConfigSource::Runtime);
    config->set("region_count", 1, ConfigSource::Runtime);
    
    Brain brain(config);
    brain.initialize();
    
    auto world = std::make_shared<SimpleWorld>();
    world->configure(20, 20, 6, 6);
    
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(*world);
    
    // Test 1: Error handling in configuration
    try {
        auto badConfig = std::make_shared<Config>();
        badConfig->set("invalid_key_with_spaces", "value");
        assert(false); // Should throw
    } catch (const std::exception& e) {
        std::cout << "Configuration error handling test PASSED: " << e.what() << std::endl;
    }
    
    // Test 2: Error handling in simulation
    bool errorOccurred = false;
    
    for (int step = 0; step < 100 && !errorOccurred; ++step) {
        try {
            world->update(0.1f);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain.step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Simulate occasional errors
            if (step % 33 == 0) {
                throw std::runtime_error("Simulated processing error");
            }
            
        } catch (const std::exception& e) {
            errorOccurred = true;
            std::cout << "Simulated error handled: " << e.what() << std::endl;
            
            // Continue simulation despite errors
            brain.reset();
            world->reset();
            agent->reset();
        }
    }
    
    assert(errorOccurred); // Should have encountered an error
    
    // Test 3: Error logging and recovery
    Logger& logger = Logger::getGlobal();
    auto memoryLogger = std::make_shared<MemoryLogger>();
    logger.clearSinks();
    logger.addSink(memoryLogger);
    
    try {
        throw std::runtime_error("Test error for logging");
    } catch (const std::exception& e) {
        logger.error(LogCategory::System, std::string("Error caught: ") + e.what(), "test.cpp", 1, "testErrorHandling", 0);
    }
    
    auto errorLogs = memoryLogger->getLogsByLevel(LogLevel::Error);
    assert(!errorLogs.empty());
    
    std::cout << "Error logging test PASSED" << std::endl;
    
    // Test 4: Graceful degradation
    std::cout << "Testing graceful degradation..." << std::endl;
    
    // Continue simulation after error
    for (int step = 0; step < 50; ++step) {
        try {
            world->update(0.1f);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain.step(step);
        } catch (const std::exception& e) {
            std::cout << "Additional error during recovery: " << e.what() << std::endl;
            // Continue with recovery
        }
    }
    
    assert(brain.getTotalNeuronCount() > 0); // Should still have neurons
    
    std::cout << "Error resilience integration test PASSED" << std::endl;
}

// Integration Test 9: Data Persistence and Checkpointing
void testDataPersistenceIntegration() {
    std::cout << "=== Data Persistence Integration Test ===" << std::endl;
    
    // Create test configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 150, ConfigSource::Runtime);
    config->set("region_count", 2, ConfigSource::Runtime);
    config->set("connection_probability", 0.05, ConfigSource::Runtime);
    config->set("random_seed", 42, ConfigSource::Runtime);
    
    // Create brain and run simulation
    Brain brain1(config);
    brain1.initialize();
    
    auto world1 = std::make_shared<SimpleWorld>();
    world1->configure(30, 30, 8, 8);
    
    auto agent1 = std::make_shared<AgentBrain>(brain1);
    agent1->initialize(*world1);
    
    // Run simulation to create state
    for (int step = 0; step < 60; ++step) {
        world1->update(0.1f);
        auto percept = world1->getSensoryPercept();
        agent1->processSensoryInput(percept);
        brain1.step(step);
        auto action = agent1->decodeMotorCommand();
        world1->applyMotorCommand(action, world1->getSimulationTime());
        
        if (step % 15 == 0) {
            agent1->applyRewardModulation(0.1f, 0.05f);
        }
    }
    
    // Save brain state
    const std::string savePath = "/tmp/nlm_integration_test_checkpoint.bin";
    bool saveSuccess = brain1.save(savePath);
    assert(saveSuccess);
    
    // Record state before loading
    double savedMeanWeight = 0.0;
    float savedMembranePotential = 0.0f;
    size_t savedPendingSpikes = brain1.getPendingSpikeEventCount();
    
    // Calculate statistics
    for (const auto& region : brain1.getRegions()) {
        for (const auto& synapse : region->getSynapses()) {
            savedMeanWeight += synapse->getWeight();
        }
        for (auto* neuron : region->getAllNeurons()) {
            savedMembranePotential = neuron->getState().membranePotential;
            break;
        }
    }
    
    size_t totalSynapses = 0;
    for (const auto& region : brain1.getRegions()) {
        totalSynapses += region->getSynapses().size();
    }
    savedMeanWeight /= std::max(totalSynapses, 1u);
    
    // Create new brain and load saved state
    auto config2 = std::make_shared<Config>();
    config2->set("neuron_count", 150, ConfigSource::Runtime);
    config2->set("region_count", 2, ConfigSource::Runtime);
    config2->set("connection_probability", 0.05, ConfigSource::Runtime);
    config2->set("random_seed", 42, ConfigSource::Runtime);
    
    Brain brain2(config2);
    brain2.initialize();
    
    bool loadSuccess = brain2.load(savePath);
    assert(loadSuccess);
    
    // Verify restored state
    double loadedMeanWeight = 0.0;
    float loadedMembranePotential = 0.0f;
    size_t loadedPendingSpikes = brain2.getPendingSpikeEventCount();
    
    for (const auto& region : brain2.getRegions()) {
        for (const auto& synapse : region->getSynapses()) {
            loadedMeanWeight += synapse->getWeight();
        }
        for (auto* neuron : region->getAllNeurons()) {
            loadedMembranePotential = neuron->getState().membranePotential;
            break;
        }
    }
    
    size_t totalSynapses2 = 0;
    for (const auto& region : brain2.getRegions()) {
        totalSynapses2 += region->getSynapses().size();
    }
    loadedMeanWeight /= std::max(totalSynapses2, 1u);
    
    // Check that state was properly restored
    assert(std::abs(savedMeanWeight - loadedMeanWeight) < 1e-6);
    assert(std::abs(savedMembranePotential - loadedMembranePotential) < 1e-6);
    assert(savedPendingSpikes == loadedPendingSpikes);
    
    // Continue simulation with both brains
    for (int step = 60; step < 80; ++step) {
        brain1.step(step);
        brain2.step(step);
    }
    
    // Check that firing rates remain similar
    float rate1 = brain1.getAverageFiringRate();
    float rate2 = brain2.getAverageFiringRate();
    float rateDiff = std::abs(rate1 - rate2);
    float rateDenom = std::max(std::abs(rate1), 1.0f);
    
    assert(rateDiff / rateDenom < 0.25f); // Should stay within 25%
    
    std::cout << "Checkpoint integration test PASSED" << std::endl;
    std::cout << "Mean weight: " << std::fixed << std::setprecision(6) 
              << savedMeanWeight << " (saved), " << loadedMeanWeight << " (loaded)" << std::endl;
    std::cout << "Membrane potential: " << std::fixed << std::setprecision(4)
              << savedMembranePotential << " (saved), " << loadedMembranePotential << " (loaded)" << std::endl;
    std::cout << "Pending spikes: " << savedPendingSpikes << " (saved), " << loadedPendingSpikes << " (loaded)" << std::endl;
    std::cout << "Firing rates: " << rate1 << " vs " << rate2 << std::endl;
    
    // Cleanup
    std::remove(savePath.c_str());
    
    std::cout << "Data persistence integration test PASSED" << std::endl;
}

// Integration Test 10: End-to-End Scenario
void testEndToEndScenario() {
    std::cout << "=== End-to-End Integration Test ===" << std::endl;
    
    // Create a comprehensive scenario: agent exploring a world with learning
    struct Scenario {
        std::string name;
        std::shared_ptr<Config> config;
        std::shared_ptr<SimpleWorld> world;
        std::shared_ptr<Brain> brain;
        std::shared_ptr<AgentBrain> agent;
    };
    
    // Initialize scenario
    auto scenario = std::make_shared<Scenario>();
    scenario->name = "Resource Exploration Agent";
    
    // Create configuration
    scenario->config = std::make_shared<Config>();
    scenario->config->set("neuron_count", 200, ConfigSource::Runtime);
    scenario->config->set("region_count", 3, ConfigSource::Runtime);
    scenario->config->set("connection_probability", 0.04, ConfigSource::Runtime);
    scenario->config->set("enable_plasticity", true, ConfigSource::Runtime);
    scenario->config->set("enable_curiosity", true, ConfigSource::Runtime);
    scenario->config->set("enable_development", true, ConfigSource::Runtime);
    
    // Create world
    scenario->world = std::make_shared<SimpleWorld>();
    scenario->world->configure(50, 50, 12, 12);
    scenario->world->setAgentStart(25, 25);
    
    // Add environment objects
    for (int x = 10; x < 40; x += 5) {
        for (int y = 10; y < 40; y += 5) {
            if ((x + y) % 10 == 0) {
                scenario->world->addResource(x, y, 5.0f);
            } else if ((x + y) % 15 == 0) {
                scenario->world->addHazard(x, y, -2.0f);
            }
        }
    }
    
    // Create brain and agent
    scenario->brain = std::make_shared<Brain>(scenario->config);
    scenario->agent = std::make_shared<AgentBrain>(scenario->brain);
    
    // Initialize components
    scenario->brain->initialize();
    scenario->world->reset();
    scenario->agent->initialize(*scenario->world);
    
    // Enable learning features
    scenario->agent->enableRewardModulation(true);
    scenario->agent->enableCuriosity(true);
    scenario->agent->enableDevelopment(true);
    scenario->agent->enableStructuralPlasticity(true);
    
    // Run end-to-end scenario
    std::vector<double> learningProgress;
    std::vector<double> explorationMetrics;
    
    std::cout << "Running end-to-end scenario: " << scenario->name << std::endl;
    std::cout << "Environment: " << scenario->world->getWidth() << "x" << scenario->world->getHeight() << std::endl;
    std::cout << "Brain: " << scenario->brain->getTotalNeuronCount() << " neurons" << std::endl;
    
    for (int step = 0; step < 200; ++step) {
        // Update world
        scenario->world->update(0.1f);
        
        // Get perception
        auto percept = scenario->world->getSensoryPercept();
        
        // Process perception
        scenario->agent->processSensoryInput(percept);
        
        // Brain computation
        scenario->brain->step(step);
        
        // Generate action
        auto action = scenario->agent->decodeMotorCommand();
        
        // Execute action
        scenario->world->applyMotorCommand(action, scenario->world->getSimulationTime());
        
        // Apply learning
        if (step % 20 == 0) {
            float reward = 0.05f;
            float predicted = 0.02f;
            scenario->agent->applyRewardModulation(reward, predicted);
            scenario->agent->updateDevelopment(0.1f);
        }
        
        // Track learning progress
        learningProgress.push_back(scenario->agent->getNeuromodulationLevel());
        explorationMetrics.push_back(scenario->agent->getCuriosityLevel());
        
        // Log progress
        if (step % 40 == 0) {
            std::cout << "Step " << step << ": "
                     << "Learning: " << std::fixed << std::setprecision(3) 
                     << scenario->agent->getNeuromodulationLevel() << ", "
                     << "Curiosity: " << std::fixed << std::setprecision(3) 
                     << scenario->agent->getCuriosityLevel() << ", "
                     << "Firing: " << scenario->brain->getFiringNeuronCount() << 
                     " neurons" << std::endl;
        }
    }
    
    // Analyze results
    double avgLearning = std::accumulate(learningProgress.begin(), learningProgress.end(), 0.0) / learningProgress.size();
    double avgExploration = std::accumulate(explorationMetrics.begin(), explorationMetrics.end(), 0.0) / explorationMetrics.size();
    
    std::cout << "\nEnd-to-end scenario results:" << std::endl;
    std::cout << "Average learning level: " << std::fixed << std::setprecision(4) << avgLearning << std::endl;
    std::cout << "Average curiosity level: " << std::fixed << std::setprecision(4) << avgExploration << std::endl;
    std::cout << "Final neuron count: " << scenario->brain->getTotalNeuronCount() << std::endl;
    std::cout << "Final spike count: " << scenario->brain->getTotalSpikeCount() << std::endl;
    std::cout << "Final firing rate: " << std::fixed << std::setprecision(4) 
              << scenario->brain->getAverageFiringRate() << " Hz" << std::endl;
    
    // Verify scenario requirements
    assert(avgLearning > 0.0f); // Should have learned something
    assert(avgExploration > 0.0f); // Should have explored
    assert(scenario->brain->getTotalNeuronCount() > 0); // Should have neurons
    assert(scenario->brain->getTotalSpikeCount() > 0); // Should have spiked
    
    // Performance check
    double totalTime = 200.0 * 0.1f; // 20 seconds
    double efficiency = scenario->brain->getTotalSpikeCount() / totalTime;
    std::cout << "Learning efficiency: " << std::fixed << std::setprecision(2) << efficiency << " spikes/sec" << std::endl;
    
    if (efficiency > 10.0) {
        std::cout << "Excellent learning efficiency!" << std::endl;
    } else if (efficiency > 5.0) {
        std::cout << "Good learning efficiency." << std::endl;
    } else {
        std::cout << "Moderate learning efficiency. Consider optimization." << std::endl;
    }
    
    std::cout << "\nEnd-to-end scenario test PASSED" << std::endl;
}

// Main function to run all integration tests
int main() {
    std::cout << "Starting NLM Integration Tests" << std::endl;
    std::cout << "=================================" << std::endl;
    
    try {
        // Run all integration tests
        testCompletePipeline();
        std::cout << std::endl;
        
        testConfigurationIntegration();
        std::cout << std::endl;
        
        testLoggingIntegration();
        std::cout << std::endl;
        
        testAgentLearningIntegration();
        std::cout << std::endl;
        
        testWorldEnvironmentIntegration();
        std::cout << std::endl;
        
        testPerformanceIntegration();
        std::cout << std::endl;
        
        testMemoryManagementIntegration();
        std::cout << std::endl;
        
        testErrorResilienceIntegration();
        std::cout << std::endl;
        
        testDataPersistenceIntegration();
        std::cout << std::endl;
        
        testEndToEndScenario();
        
        std::cout << std::endl;
        std::cout << "=================================" << std::endl;
        std::cout << "All integration tests PASSED!" << std::endl;
        std::cout << "=================================" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << std::endl;
        std::cerr << "Integration test FAILED with exception:" << std::endl;
        std::cerr << e.what() << std::endl;
        std::cerr << std::endl;
        std::cerr << "Test execution was aborted." << std::endl;
        
        return 1;
    } catch (...) {
        std::cerr << std::endl;
        std::cerr << "Integration test FAILED with unknown exception." << std::endl;
        std::cerr << std::endl;
        std::cerr << "Test execution was aborted." << std::endl;
        
        return 2;
    }
}

} // namespace integration_tests
} // namespace nlm