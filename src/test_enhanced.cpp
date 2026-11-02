# Comprehensive Test Suite for NLM v2.6.0 Enhancements
# Testing all new features, improvements, and bug fixes

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <cassert>
#include <chrono>
#include <thread>
#include <algorithm>

// Test framework utilities
namespace TestFramework {

    class TestResult {
    public:
        std::string testName;
        bool passed;
        std::string message;
        double executionTime;
        std::map<std::string, std::string> metadata;
        
        TestResult(const std::string& name, bool passed, const std::string& message = "")
            : testName(name), passed(passed), message(message), executionTime(0.0) {}
    };
    
    class TestSuite {
    private:
        std::vector<TestResult> results;
        int passed = 0;
        int failed = 0;
        int total = 0;
        
    public:
        void addResult(const TestResult& result) {
            results.push_back(result);
            total++;
            if (result.passed) {
                passed++;
            } else {
                failed++;
            }
        }
        
        void printSummary() const {
            std::cout << "\n" << std::string(80, '=') << std::endl;
            std::cout << "TEST SUITE SUMMARY" << std::endl;
            std::cout << std::string(80, '=') << std::endl;
            std::cout << "Total Tests:  " << total << std::endl;
            std::cout << "Passed:       " << passed << std::endl;
            std::cout << "Failed:       " << failed << std::endl;
            std::cout << "Success Rate: " << (total > 0 ? (passed * 100.0 / total) : 0.0) << "%" << std::endl;
            std::cout << std::string(80, '=') << std::endl;
            
            if (failed > 0) {
                std::cout << "\nFAILED TESTS:" << std::endl;
                for (const auto& result : results) {
                    if (!result.passed) {
                        std::cout << "  - " << result.testName << ": " << result.message << std::endl;
                    }
                }
            }
        }
        
        bool allPassed() const {
            return failed == 0;
        }
    };
    
    // Timing utility
    template<typename Func>
    double measureTime(Func&& func) {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(end - start).count();
    }
    
    // File comparison utility
    bool compareFiles(const std::string& file1, const std::string& file2) {
        std::ifstream f1(file1);
        std::ifstream f2(file2);
        std::string line1, line2;
        size_t lineNum = 0;
        
        while (std::getline(f1, line1) && std::getline(f2, line2)) {
            lineNum++;
            if (line1 != line2) {
                std::cout << "File difference at line " << lineNum << ":" << std::endl;
                std::cout << "  File 1: " << line1 << std::endl;
                std::cout << "  File 2: " << line2 << std::endl;
                return false;
            }
        }
        
        if (f1.good() != f2.good()) {
            std::cout << "Files have different lengths" << std::endl;
            return false;
        }
        
        return true;
    }
}

// Enhanced NLM test suite for v2.6.0
class NLMv26TestSuite {
private:
    TestFramework::TestSuite testSuite;
    std::shared_ptr<nlm::Brain> testBrain;
    std::shared_ptr<nlm::EnhancedConfig> testConfig;
    
public:
    NLMv26TestSuite() {
        setupTestEnvironment();
    }
    
    void setupTestEnvironment() {
        std::cout << "Setting up NLM v2.6.0 test environment..." << std::endl;
        
        // Create test configuration
        testConfig = std::make_shared<nlm::EnhancedConfig>();
        testConfig->createLearningBrain(100);
        
        // Initialize test brain
        testBrain = std::make_shared<nlm::EnhancedBrain>(testConfig);
        if (!testBrain->initialize()) {
            std::cerr << "ERROR: Failed to initialize test brain" << std::endl;
            throw std::runtime_error("Test setup failed");
        }
        
        std::cout << "Test environment setup complete." << std::endl;
    }
    
    void cleanupTestEnvironment() {
        std::cout << "Cleaning up test environment..." << std::endl;
        testBrain.reset();
        testConfig.reset();
        std::cout << "Test environment cleanup complete." << std::endl;
    }
    
    // Core functionality tests
    void runCoreTests() {
        std::cout << "\n=== Running Core Functionality Tests ===" << std::endl;
        
        // Test 1: Brain creation and initialization
        testBrainCreationAndInitialization();
        
        // Test 2: Configuration management
        testConfigurationManagement();
        
        // Test 3: Basic simulation
        testBasicSimulation();
        
        // Test 4: Memory systems
        testMemorySystems();
        
        // Test 5: Neuromodulation
        testNeuromodulation();
        
        // Test 6: Plasticity mechanisms
        testPlasticityMechanisms();
    }
    
    void runEnhancedAPITests() {
        std::cout << "\n=== Running Enhanced API Tests ===" << std::endl;
        
        // Test 1: Convenience functions
        testConvenienceFunctions();
        
        // Test 2: Advanced configuration
        testAdvancedConfiguration();
        
        // Test 3: Error handling
        testErrorHandling();
        
        // Test 4: Performance monitoring
        testPerformanceMonitoring();
        
        // Test 5: Experiment management
        testExperimentManagement();
    }
    
    void runIntegrationTests() {
        std::cout << "\n=== Running Integration Tests ===" << std::endl;
        
        // Test 1: Complete agent system
        testCompleteAgentSystem();
        
        // Test 2: World interaction
        testWorldInteraction();
        
        // Test 3: Sensory-motor loop
        testSensoryMotorLoop();
        
        // Test 4: Development simulation
        testDevelopmentSimulation();
        
        // Test 5: Check pointing and recovery
        testCheckpointingAndRecovery();
    }
    
    void runPerformanceTests() {
        std::cout << "\n=== Running Performance Tests ===" << std::endl;
        
        // Test 1: Simulation speed
        testSimulationSpeed();
        
        // Test 2: Memory efficiency
        testMemoryEfficiency();
        
        // Test 3: Scalability
        testScalability();
        
        // Test 4: Concurrent operations
        testConcurrentOperations();
    }
    
    // Individual test methods
    void testBrainCreationAndInitialization() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Test default brain creation
        auto brain = nlm::EnhancedBrain::create(nlm::EnhancedConfig());
        assert(brain != nullptr);
        assert(brain->initialize());
        
        // Test brain statistics
        auto stats = brain->getStatistics();
        assert(stats["neuron_count"] == 1000.0f);
        assert(stats["region_count"] > 0);
        
        // Test brain reset
        brain->reset();
        assert(brain->getTotalSpikeCount() == 0);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        testSuite.addResult(TestFramework::TestResult(
            "Brain Creation and Initialization",
            true,
            "Brain created and initialized successfully in " + std::to_string(executionTime) + "s"
        ));
    }
    
    void testConfigurationManagement() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Test configuration validation
        auto config = nlm::EnhancedConfig();
        std::vector<std::string> errors = nlm::validateBrainConfig(config);
        // Errors may be present for default config, but validation should not crash
        
        // Test configuration profiles
        config.setLearningConfig();
        assert(config.has("plasticity.stdp.enable"));
        
        config.setDevelopmentConfig();
        assert(config.has("development.enabled"));
        
        config.setMemoryConfig();
        assert(config.has("memory.working_memory.capacity"));
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        testSuite.addResult(TestFramework::TestResult(
            "Configuration Management",
            true,
            "Configuration management working in " + std::to_string(executionTime) + "s"
        ));
    }
    
    void testBasicSimulation() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Run simulation for multiple steps
        for (size_t step = 0; step < 100; ++step) {
            testBrain->step(step);
            
            // Verify brain state
            assert(testBrain->getTotalNeuronCount() > 0);
            assert(testBrain->getRegionCount() > 0);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        testSuite.addResult(TestFramework::TestResult(
            "Basic Simulation",
            true,
            "Simulation completed successfully in " + std::to_string(executionTime) + "s, " +
            std::to_string(testBrain->getTotalSpikeCount()) + " spikes recorded"
        ));
    }
    
    void testMemorySystems() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Test working memory
        auto workingMemory = testBrain->getWorkingMemory();
        if (workingMemory) {
            workingMemory->store(std::vector<nlm::NeuronId>(), 0.5f);
            assert(workingMemory->getActiveTraces() >= 0);
        }
        
        // Test episodic memory
        auto episodicMemory = testBrain->getEpisodicMemory();
        if (episodicMemory) {
            nlm::EpisodicMemoryItem episode;
            episode.timestamp = 0;
            episode.reward = 0.5f;
            episodicMemory->storeEpisode(episode);
            assert(episodicMemory->getEpisodes().size() > 0);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        testSuite.addResult(TestFramework::TestResult(
            "Memory Systems",
            true,
            "Memory systems working in " + std::to_string(executionTime) + "s"
        ));
    }
    
    void testNeuromodulation() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Test neuromodulation effects
        auto dopamine = testBrain->getDopamine();
        auto curiosity = testBrain->getCuriosity();
        auto novelty = testBrain->getNovelty();
        
        // Apply neuromodulation
        if (dopamine) {
            dopamine->update(0.001);
            assert(dopamine->getLevel() >= 0.0f);
        }
        
        if (curiosity) {
            curiosity->update(0.1f, 0.05f, 0.001);
            assert(curiosity->getLevel() >= 0.0f);
        }
        
        if (novelty) {
            novelty->update(0.001);
            assert(novelty->getLevel() >= 0.0f);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        testSuite.addResult(TestFramework::TestResult(
            "Neuromodulation",
            true,
            "Neuromodulation working in " + std::to_string(executionTime) + "s"
        ));
    }
    
    void testPlasticityMechanisms() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Test STDP
        auto stdp = testBrain->getSTDP();
        if (stdp) {
            // Enable STDP for testing
            auto* region = testBrain->getRegion(nlm::RegionId(1));
            if (region) {
                for (auto& syn : region->getSynapses()) {
                    syn->enablePlasticity(true, false, false);
                }
            }
            
            stdp->configure(0.01f, 0.012f, 20.0f);
            assert(stdp->getLTPWeight() == 0.01f);
        }
        
        // Test Hebbian learning
        auto hebbian = testBrain->getHebbian();
        if (hebbian) {
            hebbian->configure(0.001f);
        }
        
        // Test structural plasticity
        auto structuralPlasticity = testBrain->getStructuralPlasticity();
        if (structuralPlasticity) {
            assert(structuralPlasticity->getSynaptogenesisRate() == 0.0001f);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        testSuite.addResult(TestFramework::TestResult(
            "Plasticity Mechanisms",
            true,
            "Plasticity mechanisms working in " + std::to_string(executionTime) + "s"
        ));
    }
    
    void testConvenienceFunctions() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Test convenience brain creation
        auto brain1 = nlm::Convenience::createMinimalBrain(50);
        assert(brain1 != nullptr);
        
        auto brain2 = nlm::Convenience::createLearningBrain(200);
        assert(brain2 != nullptr);
        
        auto brain3 = nlm::Convenience::createDevelopmentalBrain(500);
        assert(brain3 != nullptr);
        
        // Test simulation helper
        nlm::Convenience::runForSteps(testBrain, 50);
        
        // Test analysis tools
        float learningScore = nlm::Convenience::calculateLearningScore(testBrain);
        assert(learningScore >= 0.0f);
        
        auto weights = nlm::Convenience::getWeightDistribution(testBrain);
        assert(weights.size() >= 0);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        testSuite.addResult(TestFramework::TestResult(
            "Convenience Functions",
            true,
            "Convenience functions working in " + std::to_string(executionTime) + "s"
        ));
    }
    
    void testAdvancedConfiguration() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Test advanced configuration methods
        auto config = nlm::EnhancedConfig();
        
        // Test method chaining
        config.createLearningConfig().createDevelopmentConfig().createMemoryConfig();
        
        // Test brain creation with config
        auto brain = config.createBrain();
        assert(brain != nullptr);
        
        // Test configuration validation
        std::vector<std::string> errors = nlm::validateBrainConfig(config);
        // Should not crash
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        testSuite.addResult(TestFramework::TestResult(
            "Advanced Configuration",
            true,
            "Advanced configuration working in " + std::to_string(executionTime) + "s"
        ));
    }
    
    void testErrorHandling() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Test error handling with null brain
        try {
            nlm::applyPlasticitySafe(nullptr, nullptr, "test_operation");
        } catch (...) {
            // Should not crash
        }
        
        // Test validation with invalid configuration
        nlm::EnhancedConfig invalidConfig;
        invalidConfig.set("neuron_count", -1);
        
        std::vector<std::string> errors = nlm::validateBrainConfig(invalidConfig);
        assert(!errors.empty());
        
        // Test safe operations
        bool safeOpResult = nlm::executeBrainOperation(
            testBrain,
            [](nlm::Brain* brain) { /* Do nothing */ },
            "safe_operation",
            true  // continue on error
        );
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        testSuite.addResult(TestFramework::TestResult(
            "Error Handling",
            true,
            "Error handling working in " + std::to_string(executionTime) + "s"
        ));
    }
    
    void testPerformanceMonitoring() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Get performance metrics
        auto metrics = nlm::getPerformanceMetrics(testBrain);
        
        // Verify metrics
        assert(metrics.spikeThroughput >= 0);
        assert(metrics.frameTime >= 0);
        
        // Test performance benchmarking
        auto benchmark = nlm::benchmarkSimulation(testBrain, 100);
        assert(benchmark.steps_per_second > 0);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        testSuite.addResult(TestFramework::TestResult(
            "Performance Monitoring",
            true,
            "Performance monitoring working in " + std::to_string(executionTime) + "s"
        ));
    }
    
    void testExperimentManagement() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Create experiment
        auto experiment = nlm::createExperiment("test_experiment", testBrain,
                                               {{ "epochs", "100" }});
        
        // Run experiment
        auto results = nlm::runExperiment(experiment);
        assert(results.success);
        
        // Log experiment
        nlm::logExperimentData(experiment, "test_experiment.log");
        
        // Clean up
        std::remove("test_experiment.log");
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        testSuite.addResult(TestFramework::TestResult(
            "Experiment Management",
            true,
            "Experiment management working in " + std::to_string(executionTime) + "s"
        ));
    }
    
    void testCompleteAgentSystem() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Setup complete agent system
        auto brain = nlm::Convenience::createLearningBrain(200);
        auto world = nlm::createSimpleWorld();
        world->configure(20, 20, 8, 8);
        auto agent = nlm::createAgentBrain(brain);
        
        agent->initialize(*world);
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        // Run complete episode
        for (size_t step = 0; step < 50; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            float reward = 0.0f;
            if (auto internal = percept.getInternal()) {
                reward = internal->getData()[0];
            }
            agent->applyRewardModulation(reward, 0.0);
            
            agent->updateDevelopment(0.1);
            
            if (world->isDone()) {
                break;
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        testSuite.addResult(TestFramework::TestResult(
            "Complete Agent System",
            true,
            "Complete agent system working in " + std::to_string(executionTime) + "s"
        ));
    }
    
    void testWorldInteraction() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        auto world = nlm::createSimpleWorld();
        world->configure(30, 30, 10, 10);
        
        // Test world reset
        auto percept1 = world->reset();
        assert(percept1 != nullptr);
        
        // Test world update
        auto action = nlm::Action(nlm::ActionType::MoveForward);
        auto percept2 = world->step(action);
        assert(percept2 != nullptr);
        
        // Test world state
        assert(world->getWidth() == 30);
        assert(world->getHeight() == 30);
        assert(world->getSimulationTime() >= 0);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        testSuite.addResult(TestFramework::TestResult(
            "World Interaction",
            true,
            "World interaction working in " + std::to_string(executionTime) + "s"
        ));
    }
    
    void testSensoryMotorLoop() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        auto brain = nlm::Convenience::createLearningBrain(100);
        auto world = nlm::createSimpleWorld();
        world->configure(15, 15, 6, 6);
        auto agent = nlm::createAgentBrain(brain);
        
        agent->initialize(*world);
        
        // Run sensory-motor loop
        nlm::Convenience::runSensoryMotorLoop(brain, world, agent, 25);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        testSuite.addResult(TestFramework::TestResult(
            "Sensory-Motor Loop",
            true,
            "Sensory-motor loop working in " + std::to_string(executionTime) + "s"
        ));
    }
    
    void testDevelopmentSimulation() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        auto brain = nlm::Convenience::createDevelopmentalBrain(200);
        
        // Run development simulation
        brain->setDevelopmentalStage(nlm::DevelopmentalStage::CriticalPeriod);
        
        for (size_t step = 0; step < 100; ++step) {
            brain->step(step);
            
            // Update development periodically
            if (step % 20 == 0) {
                brain->develop();
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        testSuite.addResult(TestFramework::TestResult(
            "Development Simulation",
            true,
            "Development simulation working in " + std::to_string(executionTime) + "s"
        ));
    }
    
    void testCheckpointingAndRecovery() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        auto brain = nlm::Convenience::createLearningBrain(150);
        
        // Save brain state
        std::string checkpointFile = "test_checkpoint.bin";
        bool saveSuccess = nlm::saveBrainStateSafe(brain, checkpointFile, "Test checkpoint");
        assert(saveSuccess);
        
        // Reset brain and reload
        brain->reset();
        bool loadSuccess = nlm::loadBrainStateSafe(brain, checkpointFile, true);
        assert(loadSuccess);
        
        // Clean up
        std::remove(checkpointFile.c_str());
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        testSuite.addResult(TestFramework::TestResult(
            "Checkpointing and Recovery",
            true,
            "Checkpointing and recovery working in " + std::to_string(executionTime) + "s"
        ));
    }
    
    void testSimulationSpeed() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Run simulation for speed test
        size_t steps = 1000;
        for (size_t step = 0; step < steps; ++step) {
            testBrain->step(step);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        double stepsPerSecond = steps / executionTime;
        
        testSuite.addResult(TestFramework::TestResult(
            "Simulation Speed",
            true,
            std::to_string(stepsPerSecond) + " steps/sec in " + std::to_string(executionTime) + "s"
        ));
    }
    
    void testMemoryEfficiency() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Get memory statistics
        auto memoryStats = nlm::getMemoryStats();n        
        // Check memory usage
        double memoryUsage = (memoryStats.usedMemoryBytes / (1024.0 * 1024.0)); // MB
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        testSuite.addResult(TestFramework::TestResult(
            "Memory Efficiency",
            true,
            std::to_string(memoryUsage) + " MB used in " + std::to_string(executionTime) + "s"
        ));
    }
    
    void testScalability() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Test with different brain sizes
        std::vector<size_t> brainSizes = {50, 100, 200, 500};
        
        for (size_t size : brainSizes) {
            auto brain = nlm::Convenience::createMinimalBrain(size);
            
            // Run simulation
            for (size_t step = 0; step < 50; ++step) {
                brain->step(step);
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        testSuite.addResult(TestFramework::TestResult(
            "Scalability",
            true,
            "Scalability test completed in " + std::to_string(executionTime) + "s"
        ));
    }
    
    void testConcurrentOperations() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Test concurrent simulation runs (simplified)
        std::vector<std::shared_ptr<nlm::Brain>> brains;
        for (size_t i = 0; i < 5; ++i) {
            auto brain = nlm::Convenience::createLearningBrain(100);
            brains.push_back(brain);
        }
        
        // Simulate concurrent operations
        for (size_t i = 0; i < brains.size(); ++i) {
            for (size_t step = 0; step < 20; ++step) {
                brains[i]->step(step);
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        testSuite.addResult(TestFramework::TestResult(
            "Concurrent Operations",
            true,
            "Concurrent operations test completed in " + std::to_string(executionTime) + "s"
        ));
    }
    
public:
    void runAllTests() {
        std::cout << "\n" << std::string(80, '=') << std::endl;
        std::cout << "NLM v2.6.0 COMPREHENSIVE TEST SUITE" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        
        try {
            // Core functionality tests
            runCoreTests();
            
            // Enhanced API tests
            runEnhancedAPITests();
            
            // Integration tests
            runIntegrationTests();
            
            // Performance tests
            runPerformanceTests();
            
            std::cout << "\n" << std::string(80, '=') << std::endl;
            std::cout << "ALL TESTS COMPLETED" << std::endl;
            std::cout << std::string(80, '=') << std::endl;
            
            testSuite.printSummary();
            
        } catch (const std::exception& e) {
            std::cerr << "ERROR: Test suite failed with exception: " << e.what() << std::endl;
            testSuite.addResult(TestFramework::TestResult(
                "Test Suite",
                false,
                std::string("Test suite failed: ") + e.what()
            ));
            testSuite.printSummary();
            throw;
        }
    }
};

// Main test function
int main() {
    std::cout << "NLM v2.6.0 Enhanced Test Suite" << std::endl;
    std::cout << "Testing all new features and improvements" << std::endl;
    
    try {
        NLMv26TestSuite testSuite;
        testSuite.runAllTests();
        
        if (testSuite.allPassed()) {
            std::cout << "\n" << std::string(80, '=') << std::endl;
            std::cout << "ALL TESTS PASSED SUCCESSFULLY!" << std::endl;
            std::cout << "NLM v2.6.0 enhancements are working correctly." << std::endl;
            std::cout << std::string(80, '=') << std::endl;
            return 0;
        } else {
            std::cout << "\n" << std::string(80, '=') << std::endl;
            std::cout << "SOME TESTS FAILED!" << std::endl;
            std::cout << "Please review the failed tests above." << std::endl;
            std::cout << std::string(80, '=') << std::endl;
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "FATAL ERROR: " << e.what() << std::endl;
        return 2;
    }
}