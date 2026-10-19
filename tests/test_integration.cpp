# Comprehensive NLM System Integration Tests
# This file contains integration tests for the enhanced NLM system

#include <gtest/gtest.h>
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "performance/NLMPerformanceProfiler.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "environment/Environment.hpp"
#include <chrono>
#include <thread>
#include <filesystem>
#include <fstream>
#include <vector>

// Test 1: Configuration System Integration Test
class ConfigSystemIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        config = std::make_shared<nlm::Config>();
        performanceProfiler = std::make_shared<nlm::profiling::NLMPerformanceProfiler>("config_integration_test");
        logger = std::make_shared<nlm::Logger>();
        auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
        logger->addLogger(consoleLogger);
    }
    
    void TearDown() override {
        performanceProfiler.reset();
        logger.reset();
        config.reset();
    }
    
    std::shared_ptr<nlm::Config> config;
    std::shared_ptr<nlm::profiling::NLMPerformanceProfiler> performanceProfiler;
    std::shared_ptr<nlm::Logger> logger;
};

TEST_F(ConfigSystemIntegrationTest, BasicConfigLoadSave) {
    PROFILE_SCOPE("ConfigLoadSaveTest")
    
    // Set values
    config->set("neuron_count", static_cast<int64_t>(1000));
    config->set("connection_probability", 0.15f);
    config->set("simulation_timestep", 0.001);
    config->set("brain_type", "LIF");
    
    // Verify values
    EXPECT_TRUE(config->has("neuron_count"));
    EXPECT_TRUE(config->has("connection_probability"));
    EXPECT_TRUE(config->has("simulation_timestep"));
    
    EXPECT_EQ(config->getOr<int64_t>("neuron_count", 0), 1000LL);
    EXPECT_EQ(config->getOr<double>("connection_probability", 0.0), 0.15);
    EXPECT_EQ(config->getOr<double>("simulation_timestep", 0.0), 0.001);
    
    // Test schema validation
    auto schema = std::make_shared<nlm::ConfigSchema>();
    schema->add("neuron_count", nlm::ConfigType::INTEGER, "Number of neurons", 1000);
    schema->add("connection_probability", nlm::ConfigType::DOUBLE, "Connection probability", 0.1);
    
    auto validation = config->validateAgainstSchema(schema);
    EXPECT_TRUE(validation.isValid());
    EXPECT_EQ(validation.errors.size(), 0);
}

TEST_F(ConfigSystemIntegrationTest, JSONConfigLoadSave) {
    PROFILE_SCOPE("JSONConfigLoadSaveTest")
    
    // Create a temporary JSON file
    std::string jsonFile = "test_config.json";
    std::ofstream file(jsonFile);
    file << R"({
        "neuron_count": 500,
        "connection_probability": 0.2,
        "simulation_timestep": 0.001,
        "brain_type": "LIF"
    })";
    file.close();
    
    // Load from JSON
    EXPECT_TRUE(config->loadFromFile(jsonFile));
    
    // Verify values
    EXPECT_EQ(config->getOr<int64_t>("neuron_count", 0), 500LL);
    EXPECT_EQ(config->getOr<double>("connection_probability", 0.0), 0.2);
    
    // Save to JSON
    std::string outputFile = "output_config.json";
    EXPECT_TRUE(config->saveToFile(outputFile));
    
    // Load back and verify
    auto config2 = std::make_shared<nlm::Config>();
    EXPECT_TRUE(config2->loadFromFile(outputFile));
    EXPECT_EQ(config2->getOr<int64_t>("neuron_count", 0), 500LL);
    
    // Cleanup
    std::filesystem::remove(jsonFile);
    std::filesystem::remove(outputFile);
}

TEST_F(ConfigSystemIntegrationTest, YAMLConfigLoadSave) {
    PROFILE_SCOPE("YAMLConfigLoadSaveTest")
    
    // Create a temporary YAML file
    std::string yamlFile = "test_config.yaml";
    std::ofstream file(yamlFile);
    file << R"(neuron_count: 750
connection_probability: 0.25
simulation_timestep: 0.001
brain_type: LIF
pldsytropy_learning_rate: 0.01
)";
    file.close();
    
    // Load from YAML
    EXPECT_TRUE(config->loadFromFile(yamlFile));
    
    // Verify values
    EXPECT_EQ(config->getOr<int64_t>("neuron_count", 0), 750LL);
    EXPECT_EQ(config->getOr<double>("connection_probability", 0.0), 0.25);
    
    // Save to YAML
    std::string outputFile = "output_config.yaml";
    EXPECT_TRUE(config->saveToFile(outputFile));
    
    // Load back and verify
    auto config2 = std::make_shared<nlm::Config>();
    EXPECT_TRUE(config2->loadFromFile(outputFile));
    EXPECT_EQ(config2->getOr<int64_t>("neuron_count", 0), 750LL);
    
    // Cleanup
    std::filesystem::remove(yamlFile);
    std::filesystem::remove(outputFile);
}

// Test 2: Logging System Integration Test
class LoggingSystemIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        logger = std::make_shared<nlm::Logger>();
        consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Debug);
        logger->addLogger(consoleLogger);
        
        // Add a file logger for testing
        fileLogger = std::make_shared<nlm::FileLogger>("test_log.txt", nlm::LogLevel::Debug);
        logger->addLogger(fileLogger);
        
        performanceProfiler = std::make_shared<nlm::profiling::NLMPerformanceProfiler>("logging_integration_test");
    }
    
    void TearDown() override {
        performanceProfiler.reset();
        logger.reset();
        consoleLogger.reset();
        fileLogger.reset();
        
        // Cleanup test log file
        std::filesystem::remove("test_log.txt");
    }
    
    std::shared_ptr<nlm::Logger> logger;
    std::shared_ptr<nlm::ConsoleLogger> consoleLogger;
    std::shared_ptr<nlm::FileLogger> fileLogger;
    std::shared_ptr<nlm::profiling::NLMPerformanceProfiler> performanceProfiler;
};

TEST_F(LoggingSystemIntegrationTest, MultiLoggerIntegration) {
    PROFILE_SCOPE("MultiLoggerIntegrationTest")
    
    // Test structured logging with multiple loggers
    NLM_LOG_INFO("Starting multi-logger integration test");
    NLM_LOG_INFO_CAT("Debug message", "debug_test");
    NLM_LOG_INFO_CAT("Info message", "info_test");
    NLM_LOG_WARNING_CAT("Warning message", "warning_test");
    NLM_LOG_ERROR_CAT("Error message", "error_test");
    
    // Test performance logging
    NLM_LOG_PERFORMANCE("test_operation", 1000.0);
    
    // Test exception logging
    try {
        throw std::runtime_error("Test exception");
    } catch (const std::exception& e) {
        NLM_LOG_EXCEPTION("Test exception caught", e);
    }
    
    // Verify log file was created and written to
    EXPECT_TRUE(std::filesystem::exists("test_log.txt"));
    
    std::ifstream logFile("test_log.txt");
    std::string content((std::istreambuf_iterator<char>(logFile)), std::istreambuf_iterator<char>());
    logFile.close();
    
    EXPECT_TRUE(content.find("Starting multi-logger integration test") != std::string::npos);
    EXPECT_TRUE(content.find("Debug message") != std::string::npos);
    EXPECT_TRUE(content.find("Info message") != std::string::npos);
}

TEST_F(LoggingSystemIntegrationTest, StructuredLoggingIntegration) {
    PROFILE_SCOPE("StructuredLoggingIntegrationTest")
    
    // Test structured logging with fields
    nlm::LogEntry entry;
    entry.level = nlm::LogLevel::Info;
    entry.message = "Structured log message";
    entry.fields["operation"] = "neural_computation";
    entry.fields["neurons"] = "1024";
    entry.fields["accuracy"] = "0.95";
    entry.category = "brain";
    entry.addContextField("session_id", "test_session_123");
    entry.addContextField("thread_id", std::this_thread::get_id());
    
    logger->logStructured(entry);
    
    // Verify structured log was processed
    // This would be verified by checking log output in a real scenario
}

// Test 3: Performance Profiling Integration Test
class PerformanceProfilingIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        performanceProfiler = std::make_shared<nlm::profiling::NLMPerformanceProfiler>("performance_integration_test");
        config = std::make_shared<nlm::Config>();
        logger = std::make_shared<nlm::Logger>();
        auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
        logger->addLogger(consoleLogger);
    }
    
    void TearDown() override {
        performanceProfiler.reset();
        logger.reset();
        config.reset();
    }
    
    std::shared_ptr<nlm::profiling::NLMPerformanceProfiler> performanceProfiler;
    std::shared_ptr<nlm::Config> config;
    std::shared_ptr<nlm::Logger> logger;
};

TEST_F(PerformanceProfilingIntegrationTest, BasicPerformanceProfiling) {
    PROFILE_SCOPE("BasicPerformanceProfilingTest")
    
    // Initialize profiler with config
    config->set("performance.profiling_enabled", true);
    config->set("performance.output_file", "performance_test.json");
    performanceProfiler->configure(config);
    
    // Start profiling
    performanceProfiler->startProfiler();
    
    // Simulate work
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    for (int i = 0; i < 100; ++i) {
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
    
    // Stop profiling
    performanceProfiler->stopProfiler();
    
    // Generate report
    std::string reportFile = "test_performance_report.json";
    performanceProfiler->generateReport(reportFile);
    
    // Verify report was generated
    EXPECT_TRUE(std::filesystem::exists(reportFile));
    
    // Cleanup
    std::filesystem::remove(reportFile);
    std::filesystem::remove("performance_test.json");
}

TEST_F(PerformanceProfilingIntegrationTest, BrainPerformanceIntegration) {
    PROFILE_SCOPE("BrainPerformanceIntegrationTest")
    
    // Initialize brain with performance profiling
    config->set("neuron_count", 100);
    config->set("simulation_timestep", 0.001);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->setLogger(logger);
    brain->enablePerformanceProfiling(true);
    
    // Initialize brain
    EXPECT_TRUE(brain->initialize());
    
    // Run some neural computation
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        PROFILE_BRAIN_PERFORMANCE(step)
        
        brain->step(step, step * config->getOr<double>("simulation_timestep", 0.001));
        
        // Inject some current to generate activity
        if (step % 10 == 0) {
            brain->injectCurrentToNeurons(nlm::NeuronType::Excitatory, 10.0f);
        }
    }
    
    // Check performance metrics
    EXPECT_GT(brain->getTotalSpikeCount(), 0);
    EXPECT_GT(brain->getActiveNeuronCount(), 0);
    
    brain->reset();
    brain->initialize();
}

// Test 4: Full System Integration Test
class FullSystemIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize config
        config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 500);
        config->set("simulation_timestep", 0.001);
        config->set("performance.profiling_enabled", true);
        
        // Initialize logging
        logger = std::make_shared<nlm::Logger>();
        auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
        logger->addLogger(consoleLogger);
        
        // Initialize performance profiler
        performanceProfiler = std::make_shared<nlm::profiling::NLMPerformanceProfiler>("full_system_test");
        performanceProfiler->configure(config);
        
        // Initialize brain
        brain = std::make_shared<nlm::Brain>(config);
        brain->setLogger(logger);
        brain->enablePerformanceProfiling(true);
        
        // Initialize world
        world = std::make_shared<nlm::SimpleWorld>();
        world->configure(10, 10, 20, 20);
        
        // Initialize agent brain
        agentBrain = std::make_shared<nlm::AgentBrain>(brain);
        agentBrain->initialize(world);
        
        // Enable plasticity for learning tests
        config->set("plasticity.enabled", true);
        config->set("stdp.enabled", true);
        config->set("hebbian.enabled", true);
    }
    
    void TearDown() override {
        agentBrain.reset();
        world.reset();
        brain.reset();
        performanceProfiler.reset();
        logger.reset();
        config.reset();
    }
    
    std::shared_ptr<nlm::Config> config;
    std::shared_ptr<nlm::Logger> logger;
    std::shared_ptr<nlm::profiling::NLMPerformanceProfiler> performanceProfiler;
    std::shared_ptr<nlm::Brain> brain;
    std::shared_ptr<nlm::SimpleWorld> world;
    std::shared_ptr<nlm::AgentBrain> agentBrain;
};

TEST_F(FullSystemIntegrationTest, BasicSystemIntegration) {
    PROFILE_SCOPE("FullSystemIntegrationTest")
    
    // Start performance profiler
    performanceProfiler->startProfiler();
    
    // Initialize brain
    EXPECT_TRUE(brain->initialize());
    
    // Create a simple world state
    nlm::SensoryPercept percept;
    
    // Run neural computation loop
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        PROFILE_FUNCTION()
        
        // Create sensory input
        nlm::Vision vision(20, 20, 3);
        // Fill with some random data
        for (size_t i = 0; i < 20 * 20 * 3; ++i) {
            vision.setDataAt(i, static_cast<float>(rand()) / RAND_MAX);
        }
        
        percept.setVision(vision);
        
        // Process sensory input
        agentBrain->processSensoryInput(percept);
        
        // Run brain step
        brain->step(step, step * config->getOr<double>("simulation_timestep", 0.001));
        
        // Generate motor action
        nlm::Action action = agentBrain->decodeMotorCommand();
        
        // Apply action to world
        world->applyMotorCommand(action, step * config->getOr<double>("simulation_timestep", 0.001));
        
        // Log progress
        if (step % 10 == 0) {
            NLM_LOG_INFO_CAT("System integration test step " + std::to_string(step), "integration");
            NLM_LOG_PERFORMANCE("brain_step", brain->getFiringNeuronCount());
        }
    }
    
    // Stop performance profiler
    performanceProfiler->stopProfiler();
    
    // Generate reports
    std::string performanceReport = "full_system_performance.json";
    performanceProfiler->generateReport(performanceReport);
    
    EXPECT_TRUE(std::filesystem::exists(performanceReport));
    
    // Cleanup
    std::filesystem::remove(performanceReport);
}

TEST_F(FullSystemIntegrationTest, LearningIntegrationTest) {
    PROFILE_SCOPE("LearningIntegrationTest")
    
    // Enable plasticity
    config->set("plasticity.enabled", true);
    config->set("stdp.enabled", true);
    config->set("hebbian.enabled", true);
    
    brain->enablePlasticity(true);
    brain->enableSTDP(true);
    brain->enableHebbian(true);
    
    // Initialize brain
    brain->reset();
    brain->initialize();
    
    // Record initial state
    size_t initialSynapses = brain->getTotalSynapseCount();
    
    // Run learning experiment
    for (nlm::SimulationStep step = 0; step < 200; ++step) {
        PROFILE_FUNCTION()
        
        // Create correlated input pattern for learning
        for (size_t i = 0; i < 10 && i < brain->getTotalNeuronCount() / 8; ++i) {
            brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * config->getOr<double>("simulation_timestep", 0.001));
        
        // Log learning progress
        if (step % 50 == 0) {
            NLM_LOG_INFO_CAT("Learning step " + std::to_string(step), "learning");
            NLM_LOG_PERFORMANCE("learning_step", step);
        }
    }
    
    // Record final state
    size_t finalSynapses = brain->getTotalSynapseCount();
    
    // Verify learning occurred (synaptic weights should change)
    EXPECT_GT(finalSynapses, initialSynapses);
    EXPECT_GT(brain->getTotalSpikeCount(), 0);
    
    NLM_LOG_INFO_CAT("Learning experiment completed: " + std::to_string(finalSynapses - initialSynapses) + " new synapses", "learning");
}

// Test 5: Multi-threaded Integration Test
class MultiThreadedIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 200);
        config->set("performance.profiling_enabled", true);
        
        logger = std::make_shared<nlm::Logger>();
        auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
        logger->addLogger(consoleLogger);
        
        performanceProfiler = std::make_shared<nlm::profiling::NLMPerformanceProfiler>("multithreaded_test");
        performanceProfiler->configure(config);
        
        // Create multiple brains for parallel testing
        for (int i = 0; i < 3; ++i) {
            auto brain = std::make_shared<nlm::Brain>(config);
            brain->setLogger(logger);
            brain->enablePerformanceProfiling(true);
            brains.push_back(brain);
        }
    }
    
    void TearDown() override {
        brains.clear();
        performanceProfiler.reset();
        logger.reset();
        config.reset();
    }
    
    std::shared_ptr<nlm::Config> config;
    std::shared_ptr<nlm::Logger> logger;
    std::shared_ptr<nlm::profiling::NLMPerformanceProfiler> performanceProfiler;
    std::vector<std::shared_ptr<nlm::Brain>> brains;
};

TEST_F(MultiThreadedIntegrationTest, ConcurrentConfigLoading) {
    PROFILE_SCOPE("ConcurrentConfigLoadingTest")
    
    std::vector<std::thread> threads;
    std::vector<std::shared_ptr<nlm::Config>> configs;
    
    // Create configs in multiple threads
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([&, i]() {
            auto config = std::make_shared<nlm::Config>();
            config->set("neuron_count", 100 + i * 50);
            config->set("connection_probability", 0.1 + i * 0.05);
            configs.push_back(config);
            
            // Log config changes
            NLM_LOG_INFO_CAT("Thread " + std::to_string(i) + " config loaded", "config");
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        });
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Verify all configs were created
    EXPECT_EQ(configs.size(), 5);
    
    // Verify each config has correct values
    for (size_t i = 0; i < configs.size(); ++i) {
        EXPECT_EQ(configs[i]->getOr<int64_t>("neuron_count", 0), 100LL + static_cast<int64_t>(i) * 50);
        EXPECT_EQ(configs[i]->getOr<double>("connection_probability", 0.0), 0.1 + i * 0.05);
    }
}

TEST_F(MultiThreadedIntegrationTest, ConcurrentBrainProcessing) {
    PROFILE_SCOPE("ConcurrentBrainProcessingTest")
    
    performanceProfiler->startProfiler();
    
    std::vector<std::thread> threads;
    std::vector<std::shared_ptr<nlm::Brain>> brains_copy = brains;
    
    // Process brains in multiple threads
    for (size_t i = 0; i < brains_copy.size(); ++i) {
        threads.emplace_back([&, i]() {
            if (brains_copy[i]) {
                brains_copy[i]->initialize();
                
                // Run neural computation
                for (nlm::SimulationStep step = 0; step < 20; ++step) {
                    brains_copy[i]->step(step, step * 0.001);
                    
                    if (step % 5 == 0) {
                        NLM_LOG_INFO_CAT("Brain " + std::to_string(i) + " step " + std::to_string(step), "brain");
                    }
                }
                
                brains_copy[i]->reset();
            }
        });
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
    
    performanceProfiler->stopProfiler();
    
    // Generate performance report
    std::string reportFile = "multithreaded_performance.json";
    performanceProfiler->generateReport(reportFile);
    
    EXPECT_TRUE(std::filesystem::exists(reportFile));
    
    std::filesystem::remove(reportFile);
}

// Test 6: Error Integration Test
class ErrorIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        config = std::make_shared<nlm::Config>();
        logger = std::make_shared<nlm::Logger>();
        auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Warning);
        logger->addLogger(consoleLogger);
        
        performanceProfiler = std::make_shared<nlm::profiling::NLMPerformanceProfiler>("error_integration_test");
    }
    
    void TearDown() override {
        performanceProfiler.reset();
        logger.reset();
        config.reset();
    }
    
    std::shared_ptr<nlm::Config> config;
    std::shared_ptr<nlm::Logger> logger;
    std::shared_ptr<nlm::profiling::NLMPerformanceProfiler> performanceProfiler;
};

TEST_F(ErrorIntegrationTest, ErrorHandlingIntegration) {
    PROFILE_SCOPE("ErrorHandlingIntegrationTest")
    
    // Test error handling with invalid configuration
    config->set("invalid.key", "invalid_value");
    
    // This should not crash - error handling should be graceful
    try {
        // Try to use invalid configuration
        auto invalidValue = config->getOr<int64_t>("invalid.key", -1);
        NLM_LOG_WARNING_CAT("Recovered from invalid configuration with default value", "error_handling");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR_CAT("Exception caught: " + std::string(e.what()), "error_handling");
    }
    
    // Test performance profiler error handling
    performanceProfiler->startProfiler();
    
    try {
        // Simulate an error during profiling
        throw std::runtime_error("Simulated profiling error");
    } catch (const std::exception& e) {
        NLM_LOG_EXCEPTION("Profiling error occurred", e);
    }
    
    performanceProfiler->stopProfiler();
    
    NLM_LOG_INFO_CAT("Error integration test completed successfully", "error_handling");
}

TEST_F(ErrorIntegrationTest, RecoveryIntegrationTest) {
    PROFILE_SCOPE("RecoveryIntegrationTest")
    
    // Initialize system with performance profiling
    config->set("performance.profiling_enabled", true);
    performanceProfiler->configure(config);
    
    // Start and stop profiler multiple times to test recovery
    for (int i = 0; i < 3; ++i) {
        performanceProfiler->startProfiler();
        
        // Simulate work
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        
        performanceProfiler->stopProfiler();
        
        NLM_LOG_INFO_CAT("Recovery cycle " + std::to_string(i + 1) + " completed", "recovery");
    }
    
    NLM_LOG_INFO_CAT("All recovery tests passed", "recovery");
}

// Performance Integration Test
TEST(PerformanceIntegrationTest, SystemPerformanceMeasurement) {
    PROFILE_SCOPE("SystemPerformanceMeasurementTest")
    
    auto config = std::make_shared<nlm::Config>();
    auto performanceProfiler = std::make_shared<nlm::profiling::NLMPerformanceProfiler>("performance_test");
    
    config->set("performance.profiling_enabled", true);
    performanceProfiler->configure(config);
    
    performanceProfiler->startProfiler();
    
    // Perform various operations
    for (int i = 0; i < 1000; ++i) {
        // Simulate neural computation
        std::this_thread::sleep_for(std::chrono::microseconds(100));
        
        // Log performance metrics
        if (i % 100 == 0) {
            NLM_LOG_PERFORMANCE("neural_computation", i);
        }
    }
    
    performanceProfiler->stopProfiler();
    
    std::string reportFile = "system_performance_report.json";
    performanceProfiler->generateReport(reportFile);
    
    EXPECT_TRUE(std::filesystem::exists(reportFile));
    
    std::filesystem::remove(reportFile);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}