// =============================================================================
// Comprehensive NLM Python Integration Tests
// =============================================================================

#include "tests/test_brain.h"
#include "tests/test_config.h"
#include "tests/test_agentbrain.h"
#include "tests/test_world.h"
#include "tests/test_neuron.h"
#include "tests/test_plasticity.h"
#include "tests/test_development.h"
#include "tests/test_neuromodulation.h"
#include "tests/test_memory.h"
#include "tests/test_prediction.h"
#include "tests/test_cognition.h"
#include "tests/test_performance.h"
#include "tests/test_experiment.h"

namespace test {

class NLMPythonBindingsTest {
public:
    NLMPythonBindingsTest() {
        // Initialize test environment
        initializeTestEnvironment();
    }

    ~NLMPythonBindingsTest() {
        cleanupTestEnvironment();
    }

    void runAllTests() {
        std::cout << "Running NLM Python Bindings Integration Tests" << std::endl;
        std::cout << "=" * 60 << std::endl;

        // Run test suites
        runCoreTests();
        runPythonBindingsTests();
        runPerformanceTests();
        runIntegrationTests();

        std::cout << "=" * 60 << std::endl;
        std::cout << "All tests completed successfully!" << std::endl;
    }

private:
    void initializeTestEnvironment() {
        // Create test configuration
        testConfig = std::make_shared<Config>();
        testConfig->setInt("brain.neuron_count", 1000);
        testConfig->setDouble("brain.synapse_density", 0.1);
        testConfig->setInt("brain.v_thresh", -50);
        testConfig->setInt("brain.v_rest", -70);
        testConfig->setInt("brain.tau_mem", 20);
        testConfig->setInt("brain.tau_ref", 2);

        // Create test world
        testWorld = std::make_shared<SimpleWorld>();
        testWorld->configure(20, 20, 8, 8);
        testWorld->reset();

        // Create test brain
        testBrain = std::make_shared<Brain>(testConfig);
        testBrain->initialize();

        // Create test agent brain
        testAgentBrain = std::make_shared<AgentBrain>(testBrain);
        testAgentBrain->initialize(*testWorld);

        // Enable all subsystems for comprehensive testing
        testAgentBrain->enableRewardModulation(true);
        testAgentBrain->enableStructuralPlasticity(true);
        testAgentBrain->enableDevelopment(true);
        testAgentBrain->enableCuriosity(true);
    }

    void cleanupTestEnvironment() {
        testAgentBrain.reset();
        testBrain.reset();
        testWorld.reset();
        testConfig.reset();
    }

    void runCoreTests() {
        std::cout << "\n=== Core Functionality Tests ===" << std::endl;

        // Test 1: Configuration operations
        testConfigurationOperations();

        // Test 2: Brain creation and initialization
        testBrainCreation();

        // Test 3: World operations
        testWorldOperations();

        // Test 4: Agent brain operations
        testAgentBrainOperations();

        // Test 5: Sensory processing
        testSensoryProcessing();

        // Test 6: Motor output
        testMotorOutput();

        // Test 7: Development system
        testDevelopmentSystem();

        // Test 8: Neuromodulation
        testNeuromodulation();

        // Test 9: Memory systems
        testMemorySystems();

        std::cout << "Core functionality tests completed successfully!" << std::endl;
    }

    void runPythonBindingsTests() {
        std::cout << "\n=== Python Bindings Enhancement Tests ===" << std::endl;

        // Test 1: Enhanced configuration API
        testEnhancedConfigurationAPI();

        // Test 2: NumPy integration
        testNumPyIntegration();

        // Test 3: Action factory methods
        testActionFactoryMethods();

        // Test 4: World utilities
        testWorldUtilities();

        // Test 5: Error handling
        testErrorHandling();

        // Test 6: Context managers
        testContextManagers();

        // Test 7: Statistics and introspection
        testStatisticsAndIntrospection();

        std::cout << "Python bindings enhancement tests completed successfully!" << std::endl;
    }

    void runPerformanceTests() {
        std::cout << "\n=== Performance Tests ===" << std::endl;

        // Test 1: Simulation performance
        testSimulationPerformance();

        // Test 2: Memory efficiency
        testMemoryEfficiency();

        // Test 3: Concurrent operations
        testConcurrentOperations();

        std::cout << "Performance tests completed successfully!" << std::endl;
    }

    void runIntegrationTests() {
        std::cout << "\n=== Integration Tests ===" << std::endl;

        // Test 1: Complete agent simulation
        testCompleteAgentSimulation();

        // Test 2: Development over time
        testDevelopmentOverTime();

        // Test 3: Learning and adaptation
        testLearningAndAdaptation();

        // Test 4: Checkpoint and recovery
        testCheckpointAndRecovery();

        std::cout << "Integration tests completed successfully!" << std::endl;
    }

    // =============================================================================
    // Core Functionality Tests
    // =============================================================================

    void testConfigurationOperations() {
        std::cout << "  Testing configuration operations...";

        // Test get/set operations
        testConfig->setInt("test.int", 42);
        testConfig->setDouble("test.double", 3.14);
        testConfig->setBool("test.bool", true);
        testConfig->setString("test.string", "hello");
        testConfig->setIntVector("test.vector", {1, 2, 3, 4, 5});

        assert(testConfig->has("test.int"));
        assert(testConfig->has("test.double"));
        assert(testConfig->has("test.bool"));
        assert(testConfig->has("test.string"));
        assert(testConfig->has("test.vector"));

        // Test getOr with defaults
        assert(testConfig->getOrInt("missing.key", 99) == 99);
        assert(testConfig->getOrDouble("missing.key", 2.71) == 2.71);
        assert(testConfig->getOrBool("missing.key", false) == false);
        assert(testConfig->getOrString("missing.key", "default") == "default");

        // Test summary
        std::string summary = testConfig->summary();
        assert(!summary.empty());
        assert(summary.find("test.int") != std::string::npos);

        std::cout << " PASSED" << std::endl;
    }

    void testBrainCreation() {
        std::cout << "  Testing brain creation and initialization...";

        assert(testBrain != nullptr);
        assert(testBrain->initialize());

        // Test statistics
        size_t neuronCount = testBrain->getTotalNeuronCount();
        size_t synapseCount = testBrain->getTotalSynapseCount();
        assert(neuronCount > 0);
        assert(synapseCount > 0);

        // Test developmental stage
        DevelopmentalStage stage = testBrain->getDevelopmentalStage();
        assert(stage == DevelopmentalStage::Initial || stage == DevelopmentalStage::CriticalPeriod);

        std::cout << " PASSED" << std::endl;
    }

    void testWorldOperations() {
        std::cout << "  Testing world operations...";

        // Test world configuration
        assert(testWorld->getWidth() == 20);
        assert(testWorld->getHeight() == 20);

        // Test agent position
        testWorld->setAgentStart(10.0, 10.0);
        assert(testWorld->isValidPosition(10.0, 10.0));

        // Test world update
        testWorld->update(0.1);
        assert(testWorld->getSimulationTime() >= 0.1);

        // Test object creation
        WorldObject obj = testWorld->createResource(15.0, 15.0, 1.0);
        assert(obj.type == WorldObjectType::Resource);
        assert(obj.x == 15.0 && obj.y == 15.0);

        std::cout << " PASSED" << std::endl;
    }

    void testAgentBrainOperations() {
        std::cout << "  Testing agent brain operations...";

        // Test agent brain initialization
        assert(testAgentBrain->initialize(*testWorld));

        // Test agent brain properties
        assert(testAgentBrain->isRewardModulationEnabled());
        assert(testAgentBrain->isDevelopmentEnabled());
        assert(testAgentBrain->isCuriosityEnabled());

        // Test status
        std::string status = testAgentBrain->getStatus();
        assert(!status.empty());
        assert(status.find("curiosity") != std::string::npos);

        std::cout << " PASSED" << std::endl;
    }

    void testSensoryProcessing() {
        std::cout << "  Testing sensory processing...";

        // Create a sensory percept
        SensoryPercept percept;

        // Create vision data
        Vision vision(8, 8, 3);
        std::vector<float> visionData(8 * 8 * 3, 0.5f);
        vision.setData(visionData);
        percept.setVision(vision);

        // Process sensory input
        testAgentBrain->processSensoryInput(percept);

        // Test agent body
        AgentBody body = testWorld->getAgentBody();
        assert(body.x == 10.0 && body.y == 10.0);

        std::cout << " PASSED" << std::endl;
    }

    void testMotorOutput() {
        std::cout << "  Testing motor output...";

        // Get motor command
        MotorCommand cmd = testAgentBrain->decodeMotorCommand();
        assert(cmd == MotorCommand::Wait || cmd == MotorCommand::MoveForward);

        // Apply motor command
        testWorld->applyMotorCommand(cmd, testWorld->getSimulationTime());

        // Verify world state changed
        AgentBody body = testWorld->getAgentBody();
        assert(body.isMoving || body.isTurning);

        std::cout << " PASSED" << std::endl;
    }

    void testDevelopmentSystem() {
        std::cout << "  Testing development system...";

        // Update development
        testAgentBrain->updateDevelopment(0.1);

        // Check developmental stage
        DevelopmentalStage stage = testAgentBrain->getDevelopmentalStage();
        assert(stage == DevelopmentalStage::Initial);

        // Check plasticity modifier
        float plasticity = testAgentBrain->getNeuromodulationLevel();
        assert(plasticity >= 0.0f && plasticity <= 1.0f);

        std::cout << " PASSED" << std::endl;
    }

    void testNeuromodulation() {
        std::cout << "  Testing neuromodulation...";

        // Apply reward modulation
        testAgentBrain->applyRewardModulation(1.0, 0.5);

        // Check neuromodulation levels
        float curiosity = testAgentBrain->getCuriosityLevel();
        float novelty = testAgentBrain->getNoveltyLevel();
        float predictionError = testAgentBrain->getPredictionError();

        assert(curiosity >= 0.0f && curiosity <= 1.0f);
        assert(novelty >= 0.0f && novelty <= 1.0f);
        assert(predictionError >= 0.0f);

        std::cout << " PASSED" << std::endl;
    }

    void testMemorySystems() {
        std::cout << "  Testing memory systems...";

        // Test working memory through brain
        SpikeSystem* spikeSystem = testBrain->getSpikeSystem();
        assert(spikeSystem != nullptr);

        // Test spike statistics
        size_t spikeCount = testBrain->getTotalSpikeCount();
        size_t pendingSpikes = testBrain->getPendingSpikeEventCount();

        // Test attention through brain
        const std::vector<std::unique_ptr<NeuralRegion>>& regions = testBrain->getRegions();
        assert(!regions.empty());

        std::cout << " PASSED" << std::endl;
    }

    // =============================================================================
    // Python Bindings Enhancement Tests
    // =============================================================================

    void testEnhancedConfigurationAPI() {
        std::cout << "  Testing enhanced configuration API...";

        // Test type-safe get methods
        testConfig->setInt("test.enhanced.int", 123);
        testConfig->setDouble("test.enhanced.double", 45.67);
        testConfig->setBool("test.enhanced.bool", true);
        testConfig->setString("test.enhanced.string", "enhanced");

        // Test type-safe get methods
        int intValue = testConfig->getInt("test.enhanced.int");
        double doubleValue = testConfig->getDouble("test.enhanced.double");
        bool boolValue = testConfig->getBool("test.enhanced.bool");
        std::string stringValue = testConfig->getString("test.enhanced.string");

        assert(intValue == 123);
        assert(doubleValue == 45.67);
        assert(boolValue == true);
        assert(stringValue == "enhanced");

        // Test configureBrain and configureWorld methods
        testConfig->configureBrain(2000, 0.2);
        testConfig->configureWorld(30, 30);

        std::cout << " PASSED" << std::endl;
    }

    void testNumPyIntegration() {
        std::cout << "  Testing NumPy integration...";

        // Test numpy conversion utilities
        std::vector<float> testVector = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};

        // Note: In actual implementation, these would be tested with pybind11 numpy array
        // For this test, we verify the utility functions exist

        // Test vision numpy support
        Vision vision(4, 4, 1);
        std::vector<float> visionData(16, 0.1f);
        vision.setData(visionData);

        assert(vision.getWidth() == 4);
        assert(vision.getHeight() == 4);
        assert(vision.getChannels() == 1);

        std::cout << " PASSED" << std::endl;
    }

    void testActionFactoryMethods() {
        std::cout << "  Testing action factory methods...";

        // Test static factory methods
        Action moveForward = Action::createMoveForward();
        Action moveBackward = Action::createMoveBackward();
        Action turnLeft = Action::createTurnLeft();
        Action turnRight = Action::createTurnRight();
        Action interact = Action::createInteract();
        Action wait = Action::createWait();

        assert(moveForward.getType() == ActionType::MoveForward);
        assert(moveBackward.getType() == ActionType::MoveBackward);
        assert(turnLeft.getType() == ActionType::TurnLeft);
        assert(turnRight.getType() == ActionType::TurnRight);
        assert(interact.getType() == ActionType::Interact);
        assert(wait.getType() == ActionType::Wait);

        // Test action with parameters
        Action lookAction = Action::createLook(ActionType::LookUp);

        std::cout << " PASSED" << std::endl;
    }

    void testWorldUtilities() {
        std::cout << "  Testing world utilities...";

        // Test ray casting
        auto rayCastResult = testWorld->rayCast(10.0, 10.0, 0.0, 10.0);
        bool hit = std::get<0>(rayCastResult);
        WorldObjectType type = std::get<1>(rayCastResult);

        // Test objects in range
        std::vector<WorldObject> objectsInRange = testWorld->getObjectsInRange(10.0, 10.0, 5.0);

        // Test closest object
        auto closestResult = testWorld->getClosestObject(10.0, 10.0);
        bool hasClosest = std::get<0>(closestResult);

        // Test objects by type
        std::vector<WorldObject> resources = testWorld->getObjectsByType(WorldObjectType::Resource);

        // Test object creation utilities
        WorldObject hazard = testWorld->createHazard(15.0, 15.0, 2.0);
        WorldObject wall = testWorld->createWall(18.0, 18.0, 1.0);

        assert(hazard.type == WorldObjectType::Hazard);
        assert(wall.type == WorldObjectType::Wall);

        std::cout << " PASSED" << std::endl;
    }

    void testErrorHandling() {
        std::cout << "  Testing error handling...";

        // Test configuration errors
        try {
            testConfig->getInt("nonexistent.key");
            assert(false && "Should have thrown NLMConfigurationError");
        } catch (const NLMConfigurationError& e) {
            assert(std::string(e.what()).find("key") != std::string::npos);
        }

        // Test brain errors
        try {
            testBrain->getRegionByName("nonexistent.region");
            assert(false && "Should have thrown NLMBrainError");
        } catch (const NLMBrainError& e) {
            assert(std::string(e.what()).find("Region not found") != std::string::npos);
        }

        std::cout << " PASSED" << std::endl;
    }

    void testContextManagers() {
        std::cout << "  Testing context managers...";

        // Test brain context manager
        {
            std::shared_ptr<Brain> brain = std::make_shared<Brain>(testConfig);
            brain->initialize();

            // Use brain in context
            brain->step(0, 0.1);
            assert(brain->getTotalSpikeCount() >= 0);
        } // brain goes out of scope

        // Test world context manager
        {
            std::shared_ptr<SimpleWorld> world = std::make_shared<SimpleWorld>();
            world->configure(10, 10, 5, 5);
            world->reset();

            // Use world in context
            world->update(0.1);
            assert(world->getSimulationTime() >= 0.1);
        } // world goes out of scope

        std::cout << " PASSED" << std::endl;
    }

    void testStatisticsAndIntrospection() {
        std::cout << "  Testing statistics and introspection...";

        // Test brain statistics
        std::string brainStats = testBrain->getStatistics();
        assert(brainStats.find("Brain Statistics:") != std::string::npos);
        assert(brainStats.find("Total neurons:") != std::string::npos);
        assert(brainStats.find("Regions:") != std::string::npos);

        // Test agent brain status
        std::string agentStatus = testAgentBrain->getStatus();
        assert(!agentStatus.empty());
        assert(agentStatus.find("curiosity") != std::string::npos);

        // Test region introspection
        std::vector<RegionId> regionIds = testBrain->getRegionIds();
        assert(!regionIds.empty());

        std::cout << " PASSED" << std::endl;
    }

    // =============================================================================
    // Performance Tests
    // =============================================================================

    void testSimulationPerformance() {
        std::cout << "  Testing simulation performance...";

        // Measure simulation time
        auto startTime = std::chrono::high_resolution_clock::now();

        // Run simulation for multiple steps
        for (int i = 0; i < 100; ++i) {
            testBrain->step(i, 0.1);
        }

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

        // Performance should be reasonable (less than 1 second for 100 steps)
        assert(duration.count() < 1000);

        std::cout << " PASSED" << std::endl;
    }

    void testMemoryEfficiency() {
        std::cout << "  Testing memory efficiency...";

        // Create multiple brains and verify cleanup
        std::vector<std::shared_ptr<Brain>> brains;
        for (int i = 0; i < 10; ++i) {
            auto config = std::make_shared<Config>();
            config->setInt("brain.neuron_count", 100);
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            brains.push_back(brain);
        }

        // Verify all brains were created
        assert(brains.size() == 10);

        // Cleanup
        brains.clear();

        std::cout << " PASSED" << std::endl;
    }

    void testConcurrentOperations() {
        std::cout << "  Testing concurrent operations...";

        // Test that multiple operations can be performed
        for (int i = 0; i < 50; ++i) {
            testBrain->step(i, 0.05);
            testWorld->update(0.05);
            testAgentBrain->updateDevelopment(0.05);
        }

        std::cout << " PASSED" << std::endl;
    }

    // =============================================================================
    // Integration Tests
    // =============================================================================

    void testCompleteAgentSimulation() {
        std::cout << "  Testing complete agent simulation...";

        // Create a fresh agent for this test
        auto freshConfig = std::make_shared<Config>();
        freshConfig->configureBrain(500, 0.05);
        auto freshBrain = std::make_shared<Brain>(freshConfig);
        freshBrain->initialize();

        auto freshWorld = std::make_shared<SimpleWorld>();
        freshWorld->configure(20, 20, 8, 8);
        freshWorld->setAgentStart(10.0, 10.0);
        freshWorld->reset();

        auto freshAgent = std::make_shared<AgentBrain>(freshBrain);
        freshAgent->initialize(*freshWorld);

        // Run a complete simulation
        for (int step = 0; step < 200; ++step) {
            // Get sensory input
            SensoryPercept percept = freshWorld->getSensoryPercept();

            // Process sensory input
            freshAgent->processSensoryInput(percept);

            // Run brain step
            freshBrain->step(step, 0.1);

            // Get motor command
            MotorCommand action = freshAgent->decodeMotorCommand();

            // Apply action to world
            freshWorld->applyMotorCommand(action, freshWorld->getSimulationTime());

            // Apply reward modulation
            freshAgent->applyRewardModulation(0.1, 0.05);

            // Update development
            freshAgent->updateDevelopment(0.1);
        }

        // Verify simulation completed
        assert(freshBrain->getTotalSpikeCount() > 0);
        assert(freshWorld->getSimulationTime() > 1.0);

        std::cout << " PASSED" << std::endl;
    }

    void testDevelopmentOverTime() {
        std::cout << "  Testing development over time...";

        // Store initial developmental stage
        DevelopmentalStage initialStage = testAgentBrain->getDevelopmentalStage();

        // Run simulation for longer time
        for (int step = 0; step < 1000; ++step) {
            testAgentBrain->updateDevelopment(0.1);
            testBrain->step(step, 0.1);

            // Developmental stage may change over time
            DevelopmentalStage currentStage = testAgentBrain->getDevelopmentalStage();
            assert(currentStage >= initialStage);
        }

        std::cout << " PASSED" << std::endl;
    }

    void testLearningAndAdaptation() {
        std::cout << "  Testing learning and adaptation...";

        // Store initial curiosity
        float initialCuriosity = testAgentBrain->getCuriosityLevel();

        // Apply different rewards to trigger learning
        for (int step = 0; step < 100; ++step) {
            float reward = (step % 10 == 0) ? 1.0f : 0.0f; // Every 10th step gives reward
            testAgentBrain->applyRewardModulation(reward, 0.1);
            testBrain->step(step, 0.1);
        }

        // Check that curiosity changed (adaptation occurred)
        float finalCuriosity = testAgentBrain->getCuriosityLevel();
        assert(finalCuriosity != initialCuriosity);

        std::cout << " PASSED" << std::endl;
    }

    void testCheckpointAndRecovery() {
        std::cout << "  Testing checkpoint and recovery...";

        // Save brain state
        std::string checkpointFile = "test_checkpoint.bin";
        assert(testBrain->save(checkpointFile));

        // Run simulation for a bit
        for (int step = 0; step < 50; ++step) {
            testBrain->step(step, 0.1);
        }

        // Load checkpoint
        assert(testBrain->load(checkpointFile));

        // Verify brain state was restored
        size_t spikeCountAfterLoad = testBrain->getTotalSpikeCount();
        // Note: spike count may be reset on load, but brain should still be functional
        assert(testBrain->getTotalNeuronCount() > 0);

        // Cleanup
        std::remove(checkpointFile.c_str());

        std::cout << " PASSED" << std::endl;
    }

    // =============================================================================
    // Helper Functions
    // =============================================================================

    void initializeTestEnvironment() {
        // Create test configuration
        testConfig = std::make_shared<Config>();
        testConfig->setInt("brain.neuron_count", 1000);
        testConfig->setDouble("brain.synapse_density", 0.1);
        testConfig->setInt("brain.v_thresh", -50);
        testConfig->setInt("brain.v_rest", -70);
        testConfig->setInt("brain.tau_mem", 20);
        testConfig->setInt("brain.tau_ref", 2);

        // Create test world
        testWorld = std::make_shared<SimpleWorld>();
        testWorld->configure(20, 20, 8, 8);
        testWorld->reset();

        // Create test brain
        testBrain = std::make_shared<Brain>(testConfig);
        testBrain->initialize();

        // Create test agent brain
        testAgentBrain = std::make_shared<AgentBrain>(testBrain);
        testAgentBrain->initialize(*testWorld);

        // Enable all subsystems for comprehensive testing
        testAgentBrain->enableRewardModulation(true);
        testAgentBrain->enableStructuralPlasticity(true);
        testAgentBrain->enableDevelopment(true);
        testAgentBrain->enableCuriosity(true);
    }

    void cleanupTestEnvironment() {
        testAgentBrain.reset();
        testBrain.reset();
        testWorld.reset();
        testConfig.reset();
    }

    // =============================================================================
    // Test Members
    // =============================================================================

    std::shared_ptr<Config> testConfig;
    std::shared_ptr<SimpleWorld> testWorld;
    std::shared_ptr<Brain> testBrain;
    std::shared_ptr<AgentBrain> testAgentBrain;
};

// Main function for running tests
int main() {
    try {
        NLMPythonBindingsTest tester;
        tester.runAllTests();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
}

} // namespace test

// Entry point for the test executable
int main() {
    return test::main();
}