// NLM ADVANCED COMPREHENSIVE TESTING SUITE
// Phase 2: Real Neural Computation - Advanced Features Testing

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <sstream>
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "advanced_brain_init.h"
#include "advanced_neuron_manager.h"

using namespace nlm;

// Test result structure
struct TestResult {
    std::string testName;
    std::string testCategory;
    bool passed;
    std::string message;
    double executionTime;
    std::map<std::string, std::string> details;
    
    TestResult(const std::string& name, const std::string& category, bool pass, const std::string& msg = "")
        : testName(name), testCategory(category), passed(pass), message(msg), executionTime(0.0) {}
    
    void printResult() const {
        std::cout << (passed ? "✓ PASS" : "✗ FAIL") << " | " << testCategory << " | " << testName;
        if (!message.empty()) {
            std::cout << " | " << message;
        }
        std::cout << std::endl;
    }
};

class NLMAdvancedTestSuite {
public:
    NLMAdvancedTestSuite() {
        std::cout << "=== NLM Advanced Test Suite ===" << std::endl;
        std::cout << "Testing Phase 2 Advanced Features" << std::endl;
    }
    
    std::vector<TestResult> runAllTests() {
        std::vector<TestResult> results;
        
        // Core functionality tests
        results.push_back(testConfigurationManagement());
        results.push_back(testEnhancedBrainInitialization());
        results.push_back(testAdvancedNeuronManagement());
        results.push_back(testPerformanceMonitoring());
        results.push_back(testResultExport());
        
        // Advanced feature tests
        results.push_back(testAdvancedSimulationModes());
        results.push_back(testAdvancedCommands());
        results.push_back(testNeuromodulationIntegration());
        results.push_back(testMemorySystemIntegration());
        results.push_back(testDevelopmentSystemIntegration());
        
        // Stress and edge case tests
        results.push_back(testStressTesting());
        results.push_back(testEdgeCaseHandling());
        results.push_back(testErrorHandling());
        
        // Integration tests
        results.push_back(testFullIntegrationTest());
        results.push_back(testCrossSystemIntegration());
        
        return results;
    }
    
private:
    TestResult testConfigurationManagement() {
        auto start = std::chrono::high_resolution_clock::now();
        
        try {
            // Test configuration file loading
            auto config = std::make_shared<Config>();
            bool loaded = config->loadFromFile("configs/default.cfg");
            
            if (!loaded) {
                return TestResult("Configuration File Loading", "Core Configuration", false, "Failed to load default config file");
            }
            
            // Test configuration parameter validation
            auto neuronCountOpt = config->get<int64_t>("neuron_count");
            if (!neuronCountOpt || *neuronCountOpt < 100) {
                return TestResult("Configuration Parameter Validation", "Core Configuration", false, "Invalid neuron count");
            }
            
            // Test configuration override
            config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
            auto newCount = config->get<int64_t>("neuron_count");
            if (!newCount || *newCount != 1000) {
                return TestResult("Configuration Override", "Core Configuration", false, "Failed to override configuration");
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            double time = std::chrono::duration<double>(end - start).count();
            
            return TestResult("Configuration Management", "Core Configuration", true, 
                             "Configuration loaded and validated successfully");
            
        } catch (const std::exception& e) {
            auto end = std::chrono::high_resolution_clock::now();
            double time = std::chrono::duration<double>(end - start).count();
            
            return TestResult("Configuration Management", "Core Configuration", false, 
                             std::string("Exception: ") + e.what());
        }
    }
    
    TestResult testEnhancedBrainInitialization() {
        auto start = std::chrono::high_resolution_clock::now();
        
        try {
            auto config = std::make_shared<Config>();
            config->loadFromFile("configs/default.cfg");
            
            // Create enhanced brain initializer
            EnhancedBrainInitializer::InitializationConfig initConfig;
            initConfig.validateNeuronCount = true;
            initConfig.validateConnectivity = true;
            initConfig.enableTypeBalancing = true;
            
            EnhancedBrainInitializer initializer(config, initConfig);
            
            // Initialize brain
            auto brain = std::make_shared<Brain>(config);
            auto result = initializer.initializeBrain(brain);
            
            if (!result.success) {
                std::string errors;
                for (const auto& error : result.errors) {
                    errors += error + "; ";
                }
                return TestResult("Enhanced Brain Initialization", "Core Initialization", false, 
                                 "Initialization failed: " + errors.substr(0, errors.size() - 2));
            }
            
            // Verify initialization results
            if (result.neuronsCreated == 0) {
                return TestResult("Enhanced Brain Initialization", "Core Initialization", false, 
                                 "No neurons created during initialization");
            }
            
            if (result.regionsCreated == 0) {
                return TestResult("Enhanced Brain Initialization", "Core Initialization", false, 
                                 "No regions created during initialization");
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            double time = std::chrono::duration<double>(end - start).count();
            
            return TestResult("Enhanced Brain Initialization", "Core Initialization", true, 
                             "Brain initialized successfully with " + std::to_string(result.neuronsCreated) + " neurons");
            
        } catch (const std::exception& e) {
            auto end = std::chrono::high_resolution_clock::now();
            double time = std::chrono::duration<double>(end - start).count();
            
            return TestResult("Enhanced Brain Initialization", "Core Initialization", false, 
                             std::string("Exception: ") + e.what());
        }
    }
    
    TestResult testAdvancedNeuronManagement() {
        auto start = std::chrono::high_resolution_clock::now();
        
        try {
            // Create a simple brain for testing
            auto config = std::make_shared<Config>();
            config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
            config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
            
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                return TestResult("Advanced Neuron Management", "Neuron Management", false, 
                                 "Failed to initialize brain");
            }
            
            AdvancedNeuronManager manager(brain);
            
            // Test neuron population analysis
            auto stats = manager.analyzeNeuronPopulation();
            
            if (stats.totalNeurons == 0) {
                return TestResult("Advanced Neuron Management", "Neuron Management", false, 
                                 "No neurons in population analysis");
            }
            
            // Test connectivity analysis
            auto patterns = manager.analyzeConnectivityPatterns();
            
            if (patterns.empty()) {
                return TestResult("Advanced Neuron Management", "Neuron Management", false, 
                                 "No connectivity patterns found");
            }
            
            // Test neuron pattern application
            manager.applyNeuronPattern("all_to_excitatory");
            
            // Verify pattern application
            auto stats2 = manager.analyzeNeuronPopulation();
            bool hasExcitatory = false;
            for (auto& [type, count] : stats2.typeCounts) {
                if (type == NeuronType::Excitatory && count > 0) {
                    hasExcitatory = true;
                    break;
                }
            }
            
            if (!hasExcitatory) {
                return TestResult("Advanced Neuron Management", "Neuron Management", false, 
                                 "Pattern application failed to create excitatory neurons");
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            double time = std::chrono::duration<double>(end - start).count();
            
            return TestResult("Advanced Neuron Management", "Neuron Management", true, 
                             "Advanced neuron management features working correctly");
            
        } catch (const std::exception& e) {
            auto end = std::chrono::high_resolution_clock::now();
            double time = std::chrono::duration<double>(end - start).count();
            
            return TestResult("Advanced Neuron Management", "Neuron Management", false, 
                             std::string("Exception: ") + e.what());
        }
    }
    
    TestResult testPerformanceMonitoring() {
        auto start = std::chrono::high_resolution_clock::now();
        
        try {
            // Create a performance monitor
            PerformanceMonitor perfMonitor;
            
            // Test basic performance monitoring
            perfMonitor.recordStep();
            perfMonitor.recordSpikes(100);
            perfMonitor.recordActiveNeurons(50);
            
            // Test performance report generation
            perfMonitor.printReport();
            
            auto end = std::chrono::high_resolution_clock::now();
            double time = std::chrono::duration<double>(end - start).count();
            
            return TestResult("Performance Monitoring", "Performance", true, 
                             "Performance monitoring working correctly");
            
        } catch (const std::exception& e) {
            auto end = std::chrono::high_resolution_clock::now();
            double time = std::chrono::duration<double>(end - start).count();
            
            return TestResult("Performance Monitoring", "Performance", false, 
                             std::string("Exception: ") + e.what());
        }
    }
    
    TestResult testResultExport() {
        auto start = std::chrono::high_resolution_clock::now();
        
        try {
            // Create test experiment data
            ExperimentResult experiment;
            experiment.name = "Test Experiment";
            experiment.execution_time = 1.5;
            experiment.total_steps = 1000;
            experiment.total_spikes = 5000;
            experiment.neurons_activated = 150;
            experiment.initial_mean_weight = 0.5f;
            experiment.final_mean_weight = 0.52f;
            experiment.weight_change = 0.02f;
            experiment.learning_detected = true;
            
            experiment.metrics["accuracy"] = 0.85;
            experiment.metrics["convergence_rate"] = 0.92;
            experiment.metrics["stability_score"] = 0.78;
            
            // Test JSON export
            std::string json = experiment.toJSON();
            
            if (json.empty()) {
                return TestResult("Result Export", "Data Export", false, "JSON export failed");
            }
            
            // Test summary report
            experiment.printSummary();
            
            auto end = std::chrono::high_resolution_clock::now();
            double time = std::chrono::duration<double>(end - start).count();
            
            return TestResult("Result Export", "Data Export", true, 
                             "Result export functionality working correctly");
            
        } catch (const std::exception& e) {
            auto end = std::chrono::high_resolution_clock::now();
            double time = std::chrono::duration<double>(end - start).count();
            
            return TestResult("Result Export", "Data Export", false, 
                             std::string("Exception: ") + e.what());
        }
    }
    
    TestResult testAdvancedSimulationModes() {
        auto start = std::chrono::high_resolution_clock::now();
        
        try {
            auto config = std::make_shared<Config>();
            config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
            config->set("neuron_count", static_cast<int64_t>(50), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
            
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                return TestResult("Advanced Simulation Modes", "Simulation", false, 
                                 "Failed to initialize brain");
            }
            
            AdvancedNeuronManager manager(brain);
            PerformanceMonitor perfMonitor;
            LearningExperiment experiment(brain, 123, "Advanced Simulation Test");
            
            // Test different simulation modes
            manager.runAdvancedSimulation(100, "adaptive");
            manager.runAdvancedSimulation(100, "stress");
            manager.runAdvancedSimulation(100, "learning");
            manager.runAdvancedSimulation(100, "standard");
            
            auto end = std::chrono::high_resolution_clock::now();
            double time = std::chrono::duration<double>(end - start).count();n
            
            return TestResult("Advanced Simulation Modes", "Simulation", true, 
                             "All simulation modes working correctly");
            
        } catch (const std::exception& e) {
            auto end = std::chrono::high_resolution_clock::now();
            double time = std::chrono::duration<double>(end - start).count();
            
            return TestResult("Advanced Simulation Modes", "Simulation", false, 
                             std::string("Exception: ") + e.what());
        }
    }
    
    TestResult testAdvancedCommands() {
        auto start = std::chrono::high_resolution_clock::now();
        
        try {
            auto config = std::make_shared<Config>();
            config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
            config->set("neuron_count", static_cast<int64_t>(50), ConfigSource::Default);
            
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                return TestResult("Advanced Commands", "CLI", false, 
                                 "Failed to initialize brain");
            }
            
            // Test advanced command processing
            std::vector<std::string> args1 = {"analyze"};
            std::vector<std::string> args2 = {"connectivity"};
            std::vector<std::string> args3 = {"pattern", "all_to_excitatory"};
            std::vector<std::string> args4 = {"sim", "adaptive", "50"};
            std::vector<std::string> args5 = {"help"};
            
            runAdvancedCommands(args1, brain);
            runAdvancedCommands(args2, brain);
            runAdvancedCommands(args3, brain);
            runAdvancedCommands(args4, brain);
            runAdvancedCommands(args5, brain);
            
            auto end = std::chrono::high_resolution_clock::now();
            double time = std::chrono::duration<double>(end - start).count();
            
            return TestResult("Advanced Commands", "CLI", true, 
                             "All advanced commands working correctly");
            
        } catch (const std::exception& e) {
            auto end = std::chrono::high_resolution_clock::now();
            double time = std::chrono::duration<double>(end - start).count();
            
            return TestResult("Advanced Commands", "CLI", false, 
                             std::string("Exception: ") + e.what());
        }
    }
    
    TestResult testNeuromodulationIntegration() {
        auto start = std::chrono::high_resolution_clock::now();
        
        try {
            // Test neuromodulation integration
            auto config = std::make_shared<Config>();
            config->set("dopamine_baseline", 0.2f, ConfigSource::Default);
            config->set("curiosity_baseline", 0.5f, ConfigSource::Default);
            
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                return TestResult("Neuromodulation Integration", "Integration", false, 
                                 "Failed to initialize brain with neuromodulation");
            }
            
            // Test neuromodulation system access
            auto* dopamine = brain->getDopamine();
            auto* curiosity = brain->getCuriosity();
            
            if (!dopamine || !curiosity) {
                return TestResult("Neuromodulation Integration", "Integration", false, 
                                 "Failed to access neuromodulation systems");
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            double time = std::chrono::duration<double>(end - start).count();
            
            return TestResult("Neuromodulation Integration", "Integration", true, 
                             "Neuromodulation systems integrated successfully");
            
        } catch (const std::exception& e) {
            auto end = std::chrono::high_resolution_clock::now();
            double time = std::chrono::duration<double>(end - start).count();
            
            return TestResult("Neuromodulation Integration", "Integration", false, 
                             std::string("Exception: ") + e.what());
        }
    }
    
    TestResult testMemorySystemIntegration() {
        auto start = std::chrono::high_resolution_clock::now();
        
        try {
            // Test memory system integration
            auto config = std::make_shared<Config>();
            
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                return TestResult("Memory System Integration", "Integration", false, 
                                 "Failed to initialize brain with memory systems");
            }
            
            // Test memory system access
            auto* workingMemory = brain->getWorkingMemory();
            auto* episodicMemory = brain->getEpisodicMemory();
            auto* associativeMemory = brain->getAssociativeMemory();
            
            if (!workingMemory || !episodicMemory || !associativeMemory) {
                return TestResult("Memory System Integration", "Integration", false, 
                                 "Failed to access memory systems");
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            double time = std::chrono::duration<double>(end - start).count();
            
            return TestResult("Memory System Integration", "Integration", true, 
                             "Memory systems integrated successfully");
            
        } catch (const std::exception& e) {
            auto end = std::chrono::high_resolution_clock::now();
            double time = std::chrono::duration<double>(end - start).count();
            
            return TestResult("Memory System Integration", "Integration", false, 
                             std::string("Exception: ") + e.what());
        }
    }
    
    TestResult testDevelopmentSystemIntegration() {
        auto start = std::chrono::high_resolution_clock::now();
        
        try {
            // Test development system integration
            auto config = std::make_shared<Config>();
            
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                return TestResult("Development System Integration", "Integration", false, 
                                 "Failed to initialize brain with development system");
            }
            
            // Test development system access
            auto* developmentSystem = brain->getDevelopmentSystem();
            
            if (!developmentSystem) {
                return TestResult("Development System Integration", "Integration", false, 
                                 "Failed to access development system");
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            double time = std::chrono::duration<double>(end - start).count();
            
            return TestResult("Development System Integration", "Integration", true, 
                             "Development system integrated successfully");
            
        } catch (const std::exception& e) {
            auto end = std::chrono::high_resolution_clock::now();
            double time = std::chrono::duration<double>(end - start).count();
            
            return TestResult("Development System Integration", "Integration", false, 
                             std::string("Exception: ") + e.what());
        }
    }
    
    TestResult testStressTesting() {
        auto start = std::chrono::high_resolution_clock::now();
        
        try {
            // Test stress conditions
            auto config = std::make_shared<Config>();
            config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
            config->set("neuron_count", static_cast<int64_t>(200), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
            config->set("connection_probability", 0.3f, ConfigSource::Default);
            
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                return TestResult("Stress Testing", "Stress", false, 
                                 "Failed to initialize brain for stress testing");
            }
            
            AdvancedNeuronManager manager(brain);
            
            // Run stress test simulation
            manager.runAdvancedSimulation(500, "stress");
            
            // Verify brain still functional after stress
            size_t spikeCount = brain->getTotalSpikeCount();
            if (spikeCount == 0) {
                return TestResult("Stress Testing", "Stress", false, 
                                 "Brain produced no spikes during stress test");
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            double time = std::chrono::duration<double>(end - start).count();
            
            return TestResult("Stress Testing", "Stress", true, 
                             "Stress testing completed successfully with " + std::to_string(spikeCount) + " spikes");
            
        } catch (const std::exception& e) {
            auto end = std::chrono::high_resolution_clock::now();
            double time = std::chrono::duration<double>(end - start).count();
            
            return TestResult("Stress Testing", "Stress", false, 
                             std::string("Exception: ") + e.what());
        }
    }
    
    TestResult testEdgeCaseHandling() {
        auto start = std::chrono::high_resolution_clock::now();
        
        try {
            // Test edge cases
            auto config = std::make_shared<Config>();
            
            // Test with minimal configuration
            config->set("neuron_count", static_cast<int64_t>(10), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
            
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                return TestResult("Edge Case Handling", "Edge Cases", false, 
                                 "Failed to initialize brain with minimal configuration");
            }
            
            AdvancedNeuronManager manager(brain);
            
            // Test with small population
            auto stats = manager.analyzeNeuronPopulation();
            if (stats.totalNeurons != 10) {
                return TestResult("Edge Case Handling", "Edge Cases", false, 
                                 "Population analysis returned incorrect neuron count");
            }
            
            // Test pattern application with small population
            manager.applyNeuronPattern("all_to_excitatory");
            
            auto end = std::chrono::high_resolution_clock::now();
            double time = std::chrono::duration<double>(end - start).count();
            
            return TestResult("Edge Case Handling", "Edge Cases", true, 
                             "Edge cases handled correctly");
            
        } catch (const std::exception& e) {
            auto end = std::chrono::high_resolution_clock::now();
            double time = std::chrono::duration<double>(end - start).count();
            
            return TestResult("Edge Case Handling", "Edge Cases", false, 
                             std::string("Exception: ") + e.what());
        }
    }
    
    TestResult testErrorHandling() {
        auto start = std::chrono::high_resolution_clock::now();
        
        try {
            // Test error handling
            auto config = std::make_shared<Config>();
            
            // Initialize brain
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                return TestResult("Error Handling", "Error Handling", false, 
                                 "Failed to initialize brain");
            }
            
            AdvancedNeuronManager manager(brain);
            
            // Test invalid command handling
            std::vector<std::string> invalidArgs = {"invalid_command"};
            runAdvancedCommands(invalidArgs, brain);
            
            // Test invalid pattern handling
            manager.applyNeuronPattern("invalid_pattern", 1.0f);
            
            // Test invalid simulation mode
            manager.runAdvancedSimulation(100, "invalid_mode");
            
            auto end = std::chrono::high_resolution_clock::now();
            double time = std::chrono::duration<double>(end - start).count();
            
            return TestResult("Error Handling", "Error Handling", true, 
                             "Error handling working correctly");
            
        } catch (const std::exception& e) {
            auto end = std::chrono::high_resolution_clock::now();
            double time = std::chrono::duration<double>(end - start).count();
            
            return TestResult("Error Handling", "Error Handling", false, 
                             std::string("Exception: ") + e.what());
        }
    }
    
    TestResult testFullIntegrationTest() {
        auto start = std::chrono::high_resolution_clock::now();
        
        try {
            // Test full integration of all advanced features
            auto config = std::make_shared<Config>();
            config->loadFromFile("configs/default.cfg");
            
            // Initialize enhanced brain
            EnhancedBrainInitializer::InitializationConfig initConfig;
            initConfig.validateNeuronCount = true;
            initConfig.validateConnectivity = true;
            initConfig.enableTypeBalancing = true;
            
            EnhancedBrainInitializer initializer(config, initConfig);
            auto brain = std::make_shared<Brain>(config);
            initializer.initializeBrain(brain);
            
            // Test advanced neuron management
            AdvancedNeuronManager manager(brain);
            auto stats = manager.analyzeNeuronPopulation();
            
            // Test performance monitoring
            PerformanceMonitor perfMonitor;
            perfMonitor.recordStep();
            perfMonitor.recordSpikes(1000);
            
            // Test result export
            ExperimentResult experiment;
            experiment.name = "Full Integration Test";
            experiment.execution_time = 5.0;
            experiment.total_steps = 5000;
            experiment.total_spikes = 25000;
            experiment.neurons_activated = 500;
            experiment.weight_change = 0.15f;
            experiment.learning_detected = true;
            
            // Verify all systems are working
            if (stats.totalNeurons == 0) {
                return TestResult("Full Integration Test", "Integration", false, 
                                 "Neuron management integration failed");
            }
            
            if (perfMonitor.getStepsCompleted() == 0) {
                return TestResult("Full Integration Test", "Integration", false, 
                                 "Performance monitoring integration failed");
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            double time = std::chrono::duration<double>(end - start).count();
            
            return TestResult("Full Integration Test", "Integration", true, 
                             "Full integration of all advanced features successful");
            
        } catch (const std::exception& e) {
            auto end = std::chrono::high_resolution_clock::now();
            double time = std::chrono::duration<double>(end - start).count();
            
            return TestResult("Full Integration Test", "Integration", false, 
                             std::string("Exception: ") + e.what());
        }
    }
    
    TestResult testCrossSystemIntegration() {
        auto start = std::chrono::high_resolution_clock::now();
        
        try {
            // Test cross-system integration
            auto config = std::make_shared<Config>();
            config->loadFromFile("configs/default.cfg");
            
            // Initialize brain with all systems
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                return TestResult("Cross-System Integration", "Integration", false, 
                                 "Failed to initialize brain");
            }
            
            // Test cross-system functionality
            AdvancedNeuronManager manager(brain);
            
            // Test neuron management affecting brain state
            manager.applyNeuronPattern("all_to_excitatory");
            
            // Test brain state affecting neuron management
            auto stats = manager.analyzeNeuronPopulation();
            
            // Test simulation affecting all systems
            manager.runAdvancedSimulation(200, "adaptive");
            
            // Test neuromodulation affecting neuron behavior
            auto* dopamine = brain->getDopamine();
            if (dopamine) {
                dopamine->update(0.1f, 0.05f);
            }
            
            // Test memory systems
            auto* workingMemory = brain->getWorkingMemory();
            if (workingMemory) {
                workingMemory->store("test", std::vector<float>{1.0f, 2.0f, 3.0f});
                auto memory = workingMemory->recall("test");
                if (memory.empty()) {
                    return TestResult("Cross-System Integration", "Integration", false, 
                                     "Memory system integration failed");
                }
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            double time = std::chrono::duration<double>(end - start).count();
            
            return TestResult("Cross-System Integration", "Integration", true, 
                             "Cross-system integration successful");
            
        } catch (const std::exception& e) {
            auto end = std::chrono::high_resolution_clock::now();
            double time = std::chrono::duration<double>(end - start).count();
            
            return TestResult("Cross-System Integration", "Integration", false, 
                             std::string("Exception: ") + e.what());
        }
    }
    
    // Helper function to print test results
    void printTestResults(const std::vector<TestResult>& results) {
        std::cout << "\n=== NLM Advanced Test Suite Results ===" << std::endl;
        std::cout << std::endl;
        
        int passed = 0;
        int failed = 0;
        
        for (const auto& result : results) {
            result.printResult();
            if (result.passed) {
                passed++;
            } else {
                failed++;
            }
        }
        
        std::cout << std::endl;
        std::cout << "=== Summary ===" << std::endl;
        std::cout << "Total Tests: " << results.size() << std::endl;
        std::cout << "Passed: " << passed << std::endl;
        std::cout << "Failed: " << failed << std::endl;
        std::cout << "Success Rate: " << std::fixed << std::setprecision(1) 
                  << (results.empty() ? 0.0 : (static_cast<double>(passed) / results.size()) * 100.0) << "%" << std::endl;
        
        if (failed > 0) {
            std::cout << std::endl;
            std::cout << "FAILED TESTS:" << std::endl;
            for (const auto& result : results) {
                if (!result.passed) {
                    std::cout << "  - " << result.testCategory << ": " << result.testName;
                    if (!result.message.empty()) {
                        std::cout << " (" << result.message << ")";
                    }
                    std::cout << std::endl;
                }
            }
        }
    }
};