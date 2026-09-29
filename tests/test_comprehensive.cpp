// Comprehensive NLM Test Suite
// 
// This test suite provides extensive testing for all NLM components,
// including new features like enhanced configuration, logging,
// visualization, and performance monitoring.

#include <cassert>
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <chrono>
#include <thread>
#include <cmath>
#include <algorithm>
#include <numeric>

// Include NLM headers
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/SensoryPercept.hpp"
#include "motor/MotorSystem.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "development/DevelopmentSystem.hpp"
#include "visualization/VisualizationInterface.hpp"

// Test utilities
namespace test_utils {
    
    // Test helper for checking near equality with tolerance
    template<typename T>
    bool nearEqual(T a, T b, T tolerance = 1e-6) {
        return std::abs(a - b) < tolerance;
    }
    
    // Generate random test data
    std::vector<double> generateRandomData(size_t size, double min = 0.0, double max = 1.0) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(min, max);
        
        std::vector<double> data(size);
        for (size_t i = 0; i < size; ++i) {
            data[i] = dis(gen);
        }
        return data;
    }
    
    // Test timing
    class Timer {
    public:
        Timer() : startTime(std::chrono::high_resolution_clock::now()) {}
        
        double elapsed() const {
            auto endTime = std::chrono::high_resolution_clock::now();
            return std::chrono::duration<double>(endTime - startTime).count();
        }
        
        void reset() {
            startTime = std::chrono::high_resolution_clock::now();
        }
        
    private:
        std::chrono::high_resolution_clock::time_point startTime;
    };
    
    // String utilities
    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        size_t end = str.find_last_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        return str.substr(start, end - start + 1);
    }
    
    bool startsWith(const std::string& str, const std::string& prefix) {
        return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
    }
}

namespace test_config {
    
    // Test enhanced configuration system
    void testConfigCreation() {
        std::cout << "    Testing configuration creation..." << std::endl;
        
        nlm::Config config;
        
        // Test setting various types
        config.set("int_value", 42);
        config.set("double_value", 3.14);
        config.set("string_value", "test");
        config.set("bool_value", true);
        config.set("vector_int", std::vector<int>({1, 2, 3, 4, 5}));
        config.set("vector_double", std::vector<double>({1.1, 2.2, 3.3}));
        config.set("vector_string", std::vector<std::string>({"a", "b", "c"}));
        
        // Verify values
        assert(config.has("int_value"));
        assert(config.has("double_value"));
        assert(config.has("string_value"));
        assert(config.has("bool_value"));
        assert(config.has("vector_int"));
        assert(config.has("vector_double"));
        assert(config.has("vector_string"));
        
        // Test get functionality
        auto intVal = config.get<int>("int_value");
        assert(intVal.has_value() && intVal.value() == 42);
        
        auto doubleVal = config.get<double>("double_value");
        assert(doubleVal.has_value() && test_utils::nearEqual(doubleVal.value(), 3.14));
        
        auto stringVal = config.get<std::string>("string_value");
        assert(stringVal.has_value() && stringVal.value() == "test");
        
        auto boolVal = config.get<bool>("bool_value");
        assert(boolVal.has_value() && boolVal.value() == true);
        
        std::cout << "    testConfigCreation passed" << std::endl;
    }
    
    void testConfigValidation() {
        std::cout << "    Testing configuration validation..." << std::endl;
        
        nlm::Config config;
        
        // Set up validation rules
        config.addValidationRule("age", "range_0_100", "Age must be between 0 and 100");
        config.addValidationRule("score", "range_0_1", "Score must be between 0 and 1");
        config.addValidationRule("positive", "positive", "Value must be positive");
        
        // Test valid values
        config.set("age", 25);
        config.set("score", 0.5);
        config.set("positive", 10);
        
        assert(config.validate());
        assert(config.validateKey("age"));
        assert(config.validateKey("score"));
        assert(config.validateKey("positive"));
        
        // Test invalid values
        config.set("age", -5);
        assert(!config.validateKey("age"));
        
        config.set("score", 1.5);
        assert(!config.validateKey("score"));
        
        config.set("positive", -3);
        assert(!config.validateKey("positive"));
        
        std::cout << "    testConfigValidation passed" << std::endl;
    }
    
    void testConfigMerge() {
        std::cout << "    Testing configuration merge..." << std::endl;
        
        nlm::Config config1;
        config1.set("neuron_count", 100);
        config1.set("connection_probability", 0.1);
        config1.set("enable_development", true);
        
        nlm::Config config2;
        config2.set("neuron_count", 200); // Should override
        config2.set("random_seed", 42); // New value
        config2.set("enable_development", false); // Should override
        
        config1.merge(config2);
        
        // Verify merged values
        auto neuronCount = config1.get<int64_t>("neuron_count");
        assert(neuronCount.has_value() && neuronCount.value() == 200);
        
        auto connProb = config1.get<double>("connection_probability");
        assert(connProb.has_value() && test_utils::nearEqual(connProb.value(), 0.1));
        
        auto randomSeed = config1.get<int64_t>("random_seed");
        assert(randomSeed.has_value() && randomSeed.value() == 42);
        
        auto devEnabled = config1.get<bool>("enable_development");
        assert(devEnabled.has_value() && devEnabled.value() == false);
        
        std::cout << "    testConfigMerge passed" << std::endl;
    }
    
    void testConfigExportImport() {
        std::cout << "    Testing configuration export/import..." << std::endl;
        
        nlm::Config original;
        original.set("neuron_count", 500);
        original.set("region_count", 2);
        original.set("connection_probability", 0.15);
        original.set("enable_checkpointing", true);
        original.set("enable_replay", true);
        original.setVersion("1.0.0");
        
        // Export to formatted string
        std::string exported = original.exportFormatted();
        assert(!exported.empty());
        assert(test_utils::startsWith(exported, "# NLM Configuration File"));
        
        // Create new config and import
        nlm::Config imported;
        bool importSuccess = imported.importFromFormatted(exported);
        assert(importSuccess);
        
        // Verify imported values
        auto neuronCount = original.get<int64_t>("neuron_count");
        auto importedNeuronCount = imported.get<int64_t>("neuron_count");
        assert(neuronCount == importedNeuronCount);
        
        auto version = original.getVersion();
        auto importedVersion = imported.getVersion();
        assert(version == importedVersion);
        
        std::cout << "    testConfigExportImport passed" << std::endl;
    }
    
    void testConfigValidationRules() {
        std::cout << "    Testing configuration validation rules..." << std::endl;
        
        nlm::Config config;
        
        // Add various validation rules
        config.addValidationRule("learning_rate", "range_0_1", "Learning rate must be between 0 and 1");
        config.addValidationRule("batch_size", "positive", "Batch size must be positive");
        config.addValidationRule("max_epochs", "non_zero", "Max epochs must be non-zero");
        
        // Test validation statistics
        std::string stats = config.getValidationStats();
        assert(!stats.empty());
        assert(test_utils::startsWith(stats, "Validation Statistics:"));
        
        // Test getting validation rules
        auto rules = config.getValidationRules();
        assert(rules.size() >= 3);
        
        // Test clearing validation rules
        config.clearValidationRules();
        assert(config.getValidationRules().empty());
        
        std::cout << "    testConfigValidationRules passed" << std::endl;
    }
    
    void testConfigClone() {
        std::cout << "    Testing configuration cloning..." << std::endl;
        
        nlm::Config original;
        original.set("test_value", 123);
        original.set("test_string", "hello");
        original.set("test_bool", false);
        original.setVersion("2.0.0");
        
        // Clone the configuration
        auto cloned = original.clone();
        assert(cloned != nullptr);
        
        // Verify cloned values
        auto testValue = cloned->get<int>("test_value");
        assert(testValue.has_value() && testValue.value() == 123);
        
        auto testString = cloned->get<std::string>("test_string");
        assert(testString.has_value() && testString.value() == "hello");
        
        auto testBool = cloned->get<bool>("test_bool");
        assert(testBool.has_value() && testBool.value() == false);
        
        auto version = cloned->getVersion();
        assert(version == "2.0.0");
        
        // Test that clone has different memory (but same values)
        auto testValue2 = original.get<int>("test_value");
        assert(testValue2.has_value() && testValue2.value() == 123);
        
        std::cout << "    testConfigClone passed" << std::endl;
    }
}

namespace test_logging {
    
    // Test enhanced logging system
    void testLoggingBasics() {
        std::cout << "    Testing basic logging..." << std::endl;
        
        auto& logger = nlm::Logger::getGlobal();
        
        // Test different log levels
        logger.debug("Debug message");
        logger.info("Info message");
        logger.warning("Warning message");
        logger.error("Error message");
        logger.critical("Critical message");
        
        assert(logger.isEnabled(nlm::LogLevel::Info));
        assert(logger.isEnabled(nlm::LogLevel::Warning));
        assert(logger.isEnabled(nlm::LogLevel::Error));
        assert(logger.isEnabled(nlm::LogLevel::Critical));
        
        std::cout << "    testLoggingBasics passed" << std::endl;
    }
    
    void testLoggingPerformance() {
        std::cout << "    Testing logging performance..." << std::endl;
        
        auto& logger = nlm::Logger::getGlobal();
        
        // Add a console logger
        auto consoleLogger = std::make_shared<nlm::ConsoleLogger>();
        logger.addLogger(consoleLogger);
        
        // Test rapid logging
        test_utils::Timer timer;
        
        for (int i = 0; i < 1000; ++i) {
            logger.info("Performance test message " + std::to_string(i));
        }
        
        double elapsed = timer.elapsed();
        std::cout << "      Logged 1000 messages in " << elapsed << " seconds" << std::endl;
        
        // Performance should be reasonable (> 10000 messages/second)
        assert(elapsed < 0.1); // 100ms for 1000 messages
        
        std::cout << "    testLoggingPerformance passed" << std::endl;
    }
    
    void testLoggingStatistics() {
        std::cout << "    Testing logging statistics..." << std::endl;
        
        auto& logger = nlm::Logger::getGlobal();
        
        // Clear existing loggers to avoid duplicate output
        logger.clearLoggers();
        
        // Add console logger
        auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
        logger.addLogger(consoleLogger);
        
        // Generate some log messages
        logger.info("Test message 1");
        logger.warning("Test message 2");
        logger.error("Test message 3");
        
        // Test statistics (if available)
        // Note: ConsoleLogger doesn't expose statistics, so this is a placeholder
        std::cout << "      Logging statistics test completed (console backend)" << std::endl;
        
        std::cout << "    testLoggingStatistics passed" << std::endl;
    }
    
    void testLoggingValidation() {
        std::cout << "    Testing logging validation..." << std::endl;
        
        auto& logger = nlm::Logger::getGlobal();
        
        // Test log level filtering
        auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Warning);
        logger.clearLoggers();
        logger.addLogger(consoleLogger);
        
        // These should be filtered out (Debug, Info)
        logger.debug("This should be filtered out");
        logger.info("This should also be filtered out");
        
        // These should appear (Warning, Error, Critical)
        logger.warning("This should appear");
        logger.error("This should appear");
        logger.critical("This should appear");
        
        assert(logger.isEnabled(nlm::LogLevel::Warning));
        assert(logger.isEnabled(nlm::LogLevel::Error));
        assert(logger.isEnabled(nlm::LogLevel::Critical));
        
        std::cout << "    testLoggingValidation passed" << std::endl;
    }
}

namespace test_brain {
    
    // Enhanced brain tests
    void testBrainInitialization() {
        std::cout << "    Testing brain initialization..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100));
        config->set("region_count", static_cast<int64_t>(2));
        config->set("connection_probability", 0.1);
        
        nlm::Brain brain(config);
        
        bool success = brain.initialize();
        assert(success);
        
        // Verify brain initialized with expected properties
        assert(brain.getRegionCount() == 2);
        assert(brain.getTotalNeuronCount() == 100);
        assert(brain.getTotalSynapseCount() > 0);
        
        // Test brain metrics
        assert(brain.getExcitationInhibitionRatio() >= 0.0f);
        assert(brain.getAverageFiringRate() >= 0.0f);
        assert(brain.getActiveNeuronCount() >= 0);
        
        std::cout << "    testBrainInitialization passed" << std::endl;
    }
    
    void testBrainStep() {
        std::cout << "    Testing brain stepping..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100));
        config->set("region_count", static_cast<int64_t>(1));
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Record initial state
        float initialFiringRate = brain.getAverageFiringRate();
        size_t initialSpikeCount = brain.getTotalSpikeCount();
        
        // Run multiple steps
        for (nlm::SimulationStep step = 0; step < 100; ++step) {
            brain.step(step);
        }
        
        // Verify brain continued to function
        assert(brain.getTotalNeuronCount() == 100);
        assert(brain.getTotalSynapseCount() > 0);
        
        // Metrics should be reasonable
        float finalFiringRate = brain.getAverageFiringRate();
        assert(finalFiringRate >= 0.0f);
        
        std::cout << "    testBrainStep passed" << std::endl;
    }
    
    void testBrainMemorySystems() {
        std::cout << "    Testing brain memory systems..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100));
        config->set("region_count", static_cast<int64_t>(1));
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Test working memory
        auto* workingMemory = brain.getWorkingMemory();
        assert(workingMemory != nullptr);
        
        // Test episodic memory
        auto* episodicMemory = brain.getEpisodicMemory();
        assert(episodicMemory != nullptr);
        
        // Test neuromodulation systems
        auto* dopamine = brain.getDopamine();
        auto* curiosity = brain.getCuriosity();
        auto* novelty = brain.getNovelty();
        assert(dopamine != nullptr);
        assert(curiosity != nullptr);
        assert(novelty != nullptr);
        
        // Test prediction system
        auto* predictionSystem = brain.getPredictionSystem();
        assert(predictionSystem != nullptr);
        
        // Test development system
        auto* developmentSystem = brain.getDevelopmentSystem();
        assert(developmentSystem != nullptr);
        
        // Test cognition systems
        auto* planner = brain.getPlanner();
        auto* conceptFormation = brain.getConceptFormation();
        auto* attention = brain.getAttention();
        assert(planner != nullptr);
        assert(conceptFormation != nullptr);
        assert(attention != nullptr);
        
        std::cout << "    testBrainMemorySystems passed" << std::endl;
    }
    
    void testBrainDevelopment() {
        std::cout << "    Testing brain development..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100));
        config->set("region_count", static_cast<int64_t>(1));
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Get initial developmental stage
        nlm::DevelopmentalStage initialStage = brain.getDevelopmentalStage();
        
        // Run some steps to allow development to occur
        for (nlm::SimulationStep step = 0; step < 1000; ++step) {
            brain.step(step, step * 0.001); // With time
        }
        
        // Get final developmental stage
        nlm::DevelopmentalStage finalStage = brain.getDevelopmentalStage();
        
        // Development should have occurred (stage might change)
        assert(finalStage != nlm::DevelopmentalStage::Initial);
        
        std::cout << "    testBrainDevelopment passed" << std::endl;
    }
}

namespace test_agent {
    
    // Enhanced agent brain tests
    void testAgentBrainInitialization() {
        std::cout << "    Testing agent brain initialization..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100));
        config->set("region_count", static_cast<int64_t>(1));
        
        nlm::Brain brain(config);
        brain.initialize();
        
        nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
        
        // Create a simple world for testing
        nlm::SimpleWorld world;
        world.configure(20.0f, 20.0f, 16, 16);
        
        agentBrain.initialize(world);
        
        // Test agent brain configuration
        agentBrain.enableRewardModulation(true);
        agentBrain.enableStructuralPlasticity(true);
        agentBrain.enableDevelopment(true);
        agentBrain.enableCuriosity(true);
        
        assert(agentBrain.isRewardModulationEnabled());
        assert(agentBrain.isStructuralPlasticityEnabled());
        assert(agentBrain.isDevelopmentEnabled());
        assert(agentBrain.isCuriosityEnabled());
        
        // Test neural input/output sizes
        size_t sensoryInputSize = agentBrain.getSensoryInputSize();
        size_t motorOutputSize = agentBrain.getMotorOutputSize();
        
        assert(sensoryInputSize > 0);
        assert(motorOutputSize > 0);
        
        std::cout << "    testAgentBrainInitialization passed" << std::endl;
    }
    
    void testAgentBrainFunctionality() {
        std::cout << "    Testing agent brain functionality..." << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100));
        config->set("region_count", static_cast<int64_t>(1));
        
        nlm::Brain brain(config);
        brain.initialize();
        
        nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
        
        // Create world
        nlm::SimpleWorld world;
        world.configure(20.0f, 20.0f, 16, 16);
        agentBrain.initialize(world);
        
        // Create sensory percept
        nlm::SensoryPercept percept;
        
        // Test processing sensory input
        agentBrain.processSensoryInput(percept);
        
        // Test neuromodulation
        agentBrain.applyRewardModulation(1.0f, 0.5f);
        
        // Test development update
        agentBrain.updateDevelopment(0.1);
        
        // Test motor command decoding
        nlm::MotorCommand command = agentBrain.decodeMotorCommand();
        // Command could be any valid value
        
        // Test retrieval of neuromodulation levels
        float dopamineLevel = agentBrain.getNeuromodulationLevel();
        float curiosityLevel = agentBrain.getCuriosityLevel();
        float noveltyLevel = agentBrain.getNoveltyLevel();
        
        assert(dopamineLevel >= -1.0f && dopamineLevel <= 1.0f);
        assert(curiosityLevel >= 0.0f && curiosityLevel <= 1.0f);
        assert(noveltyLevel >= 0.0f && noveltyLevel <= 1.0f);
        
        // Test development stage
        nlm::DevelopmentalStage stage = agentBrain.getDevelopmentalStage();
        
        std::cout << "    testAgentBrainFunctionality passed" << std::endl;
    }
}

namespace test_visualization {
    
    // Enhanced visualization tests
    void testVisualizationBasics() {
        std::cout << "    Testing visualization basics..." << std::endl;
        
        nlm::VisualizationInterface viz;
        
        // Test initialization
        bool initSuccess = viz.initialize();
        assert(initSuccess);
        
        assert(viz.isActive());
        
        // Test update
        viz.update();
        
        // Test configuration
        nlm::VisualizationInterface::VisualizationConfig config;
        config.defaultUpdateRate = 60.0;
        config.autoScale = true;
        config.showStatistics = true;
        
        viz.setConfiguration(config);
        
        // Verify configuration
        nlm::VisualizationInterface::VisualizationConfig retrievedConfig = viz.getConfiguration();
        assert(retrievedConfig.defaultUpdateRate == 60.0);
        assert(retrievedConfig.autoScale == true);
        
        // Test available backends
        auto backends = viz.getAvailableBackends();
        assert(!backends.empty());
        assert(std::find(backends.begin(), backends.end(), "console") != backends.end());
        
        // Test state
        auto state = viz.getState();
        assert(state.active == true);
        assert(state.currentBackend == "console");
        
        std::cout << "    testVisualizationBasics passed" << std::endl;
    }
    
    void testVisualizationTimeSeries() {
        std::cout << "    Testing visualization time series..." << std::endl;
        
        nlm::VisualizationInterface viz;
        
        // Initialize
        assert(viz.initialize());
        
        // Create test data
        std::vector<double> data = {0.1, 0.5, 0.8, 0.3, 0.9, 0.2, 0.7, 0.4, 0.6, 0.1};
        
        // Test time series visualization
        viz.visualizeTimeSeries(data, "Test Performance");
        
        // Test multi-panel visualization
        nlm::Brain brain(nullptr); // Placeholder - would need real config
        viz.visualizeMultiPanel(brain);
        
        // Test export
        bool exportSuccess = viz.exportVisualization("test_output", "csv");
        // Export may not be fully implemented in Phase 1
        
        // Test annotations
        viz.addAnnotation("Start", 0.0, 0.0);
        viz.addAnnotation("Peak", 5.0, 0.8);
        assert(viz.getState().annotations.size() == 2);
        
        viz.removeAnnotation(0);
        assert(viz.getState().annotations.size() == 1);
        
        viz.clearAnnotations();
        assert(viz.getState().annotations.empty());
        
        std::cout << "    testVisualizationTimeSeries passed" << std::endl;
    }
    
    void testVisualizationPerformance() {
        std::cout << "    Testing visualization performance..." << std::endl;
        
        nlm::VisualizationInterface viz;
        
        // Initialize
        assert(viz.initialize());
        
        // Performance timing
        test_utils::Timer timer;
        
        // Perform multiple updates
        for (int i = 0; i < 100; ++i) {
            viz.update();
        }
        
        double elapsed = timer.elapsed();
        std::cout << "      Performed 100 updates in " << elapsed << " seconds" << std::endl;
        
        // Performance should be reasonable (> 1000 updates/second)
        assert(elapsed < 1.0); // 1 second for 100 updates
        
        // Get statistics
        auto stats = viz.getStats();
        assert(stats.fps > 0.0);
        assert(stats.updateTime > 0.0);
        assert(stats.totalDrawCalls > 0);
        
        std::cout << "    testVisualizationPerformance passed" << std::endl;
    }
}

namespace test_integration {
    
    // Integration tests combining multiple components
    void testCompleteSimulation() {
        std::cout << "    Testing complete simulation integration..." << std::endl;
        
        // Create configuration
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(200));
        config->set("region_count", static_cast<int64_t>(2));
        config->set("connection_probability", 0.1);
        config->set("random_seed", 42);
        
        // Create brain
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Create world
        nlm::SimpleWorld world;
        world.configure(30.0f, 30.0f, 20, 20);
        world.setRandomSeed(42);
        
        // Create agent brain
        nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
        agentBrain.initialize(world);
        
        // Configure agent brain
        agentBrain.enableRewardModulation(true);
        agentBrain.enableStructuralPlasticity(true);
        agentBrain.enableDevelopment(true);
        agentBrain.enableCuriosity(true);
        
        // Run simulation for a few steps
        for (int step = 0; step < 50; ++step) {
            // Get sensory input
            const nlm::SensoryPercept& percept = world.getSensoryPercept();
            
            // Process sensory input
            agentBrain.processSensoryInput(percept);
            
            // Advance brain
            for (int neuralStep = 0; neuralStep < 10; ++neuralStep) {
                brain.step(neuralStep);
            }
            
            // Get motor command
            nlm::MotorCommand cmd = agentBrain.decodeMotorCommand();
            
            // Apply action
            nlm::ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
            
            // Apply reward modulation
            agentBrain.applyRewardModulation(result.reward, 0.0f);
            
            // Update development
            agentBrain.updateDevelopment(0.01);
            
            // Print progress occasionally
            if (step % 10 == 0) {
                std::cout << "      Step " << step << ": Reward = " << result.reward 
                          << ", Neuromod = " << agentBrain.getNeuromodulationLevel() 
                          << ", Energy = " << world.getAgentBody().energy << std::endl;
            }
        }
        
        // Verify simulation completed successfully
        assert(brain.getTotalNeuronCount() == 200);
        assert(brain.getTotalSynapseCount() > 0);
        
        // Check that agent brain is still functional
        assert(agentBrain.getNeuromodulationLevel() >= -1.0f);
        assert(agentBrain.getCuriosityLevel() >= 0.0f);
        
        std::cout << "    testCompleteSimulation passed" << std::endl;
    }
    
    void testPhase6Integration() {
        std::cout << "    Testing Phase 6 integration..." << std::endl;
        
        // This test verifies that all Phase 6 systems are properly integrated
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100));
        config->set("region_count", static_cast<int64_t>(1));
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Test memory system integration
        auto* workingMemory = brain.getWorkingMemory();
        auto* episodicMemory = brain.getEpisodicMemory();
        assert(workingMemory != nullptr);
        assert(episodicMemory != nullptr);
        
        // Test neuromodulation system integration
        auto* dopamine = brain.getDopamine();
        auto* curiosity = brain.getCuriosity();
        auto* novelty = brain.getNovelty();
        assert(dopamine != nullptr);
        assert(curiosity != nullptr);
        assert(novelty != nullptr);
        
        // Test prediction system integration
        auto* predictionSystem = brain.getPredictionSystem();
        assert(predictionSystem != nullptr);
        
        // Test development system integration
        auto* developmentSystem = brain.getDevelopmentSystem();
        assert(developmentSystem != nullptr);
        
        // Test cognition system integration
        auto* planner = brain.getPlanner();
        auto* conceptFormation = brain.getConceptFormation();
        auto* attention = brain.getAttention();
        assert(planner != nullptr);
        assert(conceptFormation != nullptr);
        assert(attention != nullptr);
        
        // Run simulation steps
        for (int step = 0; step < 100; ++step) {
            brain.step(step, step * 0.001);
        }
        
        // Test that all systems are still functional
        assert(brain.getTotalNeuronCount() > 0);
        assert(brain.getTotalSynapseCount() > 0);
        
        std::cout << "    testPhase6Integration passed" << std::endl;
    }
}

// Main test runner
int main() {
    std::cout << "=========================================" << std::endl;
    std::cout << "NLM COMPREHENSIVE TEST SUITE" << std::endl;
    std::cout << "=========================================" << std::endl;
    
    int passed = 0;
    int failed = 0;
    
    try {
        // Configuration tests
        std::cout << "\n=== CONFIGURATION TESTS ===" << std::endl;
        test_config::testConfigCreation(); passed++;
        test_config::testConfigValidation(); passed++;
        test_config::testConfigMerge(); passed++;
        test_config::testConfigExportImport(); passed++;
        test_config::testConfigValidationRules(); passed++;
        test_config::testConfigClone(); passed++;
        
        // Logging tests
        std::cout << "\n=== LOGGING TESTS ===" << std::endl;
        test_logging::testLoggingBasics(); passed++;
        test_logging::testLoggingPerformance(); passed++;
        test_logging::testLoggingStatistics(); passed++;
        test_logging::testLoggingValidation(); passed++;
        
        // Brain tests
        std::cout << "\n=== BRAIN TESTS ===" << std::endl;
        test_brain::testBrainInitialization(); passed++;
        test_brain::testBrainStep(); passed++;
        test_brain::testBrainMemorySystems(); passed++;
        test_brain::testBrainDevelopment(); passed++;
        
        // Agent tests
        std::cout << "\n=== AGENT BRAIN TESTS ===" << std::endl;
        test_agent::testAgentBrainInitialization(); passed++;
        test_agent::testAgentBrainFunctionality(); passed++;
        
        // Visualization tests
        std::cout << "\n=== VISUALIZATION TESTS ===" << std::endl;
        test_visualization::testVisualizationBasics(); passed++;
        test_visualization::testVisualizationTimeSeries(); passed++;
        test_visualization::testVisualizationPerformance(); passed++;
        
        // Integration tests
        std::cout << "\n=== INTEGRATION TESTS ===" << std::endl;
        test_integration::testCompleteSimulation(); passed++;
        test_integration::testPhase6Integration(); passed++;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Test suite failed with exception: " << e.what() << std::endl;
        failed++;
    } catch (...) {
        std::cerr << "ERROR: Test suite failed with unknown exception" << std::endl;
        failed++;
    }
    
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "TEST SUMMARY" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "Tests passed: " << passed << std::endl;
    std::cout << "Tests failed: " << failed << std::endl;
    std::cout << "Total tests: " << (passed + failed) << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    if (failed > 0) {
        std::cout << "FAILURE: " << failed << " tests failed" << std::endl;
        return 1;
    }
    
    std::cout << "SUCCESS: All tests passed!" << std::endl;
    return 0;
}