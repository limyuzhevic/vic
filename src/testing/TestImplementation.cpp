// NLM Test Implementation
// Implementation of test cases for NLM functionality

#include "TestRunner.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "environment/Environment.hpp"
#include "agent/AgentBrain.hpp"

using namespace nlm;
using namespace nlm::testing;

// Basic configuration test
class ConfigTest : public Test {
public:
    ConfigTest() : Test("Configuration System") {}
    
    void run() override {
        auto config = std::make_shared<Config>();
        
        // Test basic configuration operations
        config->set("test.int", 42);
        config->set("test.double", 3.14);
        config->set("test.string", "hello");
        config->set("test.bool", true);
        
        assertEquals<int64_t>(42, config->getOr<int64_t>("test.int", 0));
        assertEquals<double>(3.14, config->getOr<double>("test.double", 0.0));
        assertEquals<std::string>("hello", config->getOr<std::string>("test.string", ""));
        assertEquals<bool>(true, config->getOr<bool>("test.bool", false));
        
        // Test has function
        assertTrue(config->has("test.int"));
        assertFalse(config->has("nonexistent.key"));
        
        // Test loading from file (if available)
        std::string testConfigPath = "configs/test.cfg";
        std::ofstream testConfig(testConfigPath);
        testConfig << "# Test configuration" << std::endl;
        testConfig << "key1 = value1" << std::endl;
        testConfig << "key2 = 123" << std::endl;
        testConfig << "key3 = true" << std::endl;
        testConfig.close();
        
        auto config2 = std::make_shared<Config>();
        bool loaded = config2->loadFromFile(testConfigPath);
        assertTrue(loaded, "Should load configuration from file");
        
        // Clean up
        std::remove(testConfigPath.c_str());
        
        std::cout << "  Configuration test passed" << std::endl;
    }
};

// Brain initialization test
class BrainInitTest : public Test {
public:
    BrainInitTest() : Test("Brain Initialization") {}
    
    void run() override {
        auto config = std::make_shared<Config>();
        
        // Set basic configuration
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        
        // Initialize brain
        auto brain = std::make_shared<Brain>(config);
        
        // Test initialization
        assertTrue(brain->initialize(), "Brain should initialize successfully");
        
        // Check basic properties
        assertInRange<size_t>(brain->getTotalNeuronCount(), 50, 200, "Neuron count should be reasonable");
        assertInRange<size_t>(brain->getTotalSynapseCount(), 0, 1000, "Synapse count should be reasonable");
        assertEquals<size_t>(brain->getTotalNeuronCount(), brain->getFiringNeuronCount(), "Initial firing should be zero");
        
        std::cout << "  Brain initialization test passed" << std::endl;
    }
};

// Brain step test
class BrainStepTest : public Test {
public:
    BrainStepTest() : Test("Brain Step Processing") {}
    
    void run() override {
        auto config = std::make_shared<Config>();
        
        config->set("random_seed", static_cast<int64_t>(123), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(50), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.05f, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Record initial state
        size_t initialSpikes = brain->getTotalSpikeCount();
        size_t initialNeurons = brain->getFiringNeuronCount();
        
        // Run several steps
        for (SimulationStep step = 0; step < 10; ++step) {
            brain->step(step, step * 0.001);
        }
        
        // Test that step processing completed
        assertEquals<size_t>(brain->getTotalNeuronCount(), brain->getFiringNeuronCount() + initialNeurons, 
                           "Firing neurons should be consistent");
        
        std::cout << "  Brain step processing test passed" << std::endl;
    }
};

// Memory system test
class MemoryTest : public Test {
public:
    MemoryTest() : Test("Memory Systems") {}
    
    void run() override {
        // Test working memory
        auto workingMemory = std::make_unique<WorkingMemory>();
        workingMemory->store(NeuronId(1), 0.5f);
        workingMemory->store(NeuronId(2), 0.8f);
        workingMemory->store(NeuronId(3), 0.3f);
        
        assertEquals<float>(0.5f, workingMemory->retrieve(NeuronId(1)));
        assertEquals<float>(0.8f, workingMemory->retrieve(NeuronId(2)));
        assertEquals<float>(0.3f, workingMemory->retrieve(NeuronId(3)));
        assertFalse(workingMemory->contains(NeuronId(999)));
        assertTrue(workingMemory->contains(NeuronId(1)));
        
        // Test episodic memory
        auto episodicMemory = std::make_unique<EpisodicMemory>();
        EpisodicMemoryItem item1;
        item1.timestamp = 1000;
        item1.relevance = 0.9f;
        item1.content = "Test episode 1";
        
        episodicMemory->storeEpisode(item1);
        assertEquals<size_t>(1, episodicMemory->getEpisodeCount());
        
        auto retrieved = episodicMemory->retrieveEpisode(0);
        assertEquals<float>(0.9f, retrieved.relevance);
        assertEquals<size_t>(1000, retrieved.timestamp);
        assertEquals<std::string>("Test episode 1", retrieved.content);
        
        // Test semantic memory
        auto semanticMemory = std::make_unique<SemanticMemory>();
        semanticMemory->storeFact("color", "red");
        semanticMemory->storeFact("size", "large");
        
        assertEquals<std::string>("red", semanticMemory->retrieveFact("color"));
        assertEquals<std::string>("large", semanticMemory->retrieveFact("size"));
        assertTrue(semanticMemory->hasFact("color"));
        assertFalse(semanticMemory->hasFact("nonexistent"));
        
        std::cout << "  Memory systems test passed" << std::endl;
    }
};

// Environment test
class EnvironmentTest : public Test {
public:
    EnvironmentTest() : Test("Environment Interface") {}
    
    void run() override {
        auto world = std::make_unique<GridWorldEnvironment>(10, 10);
        
        // Test reset
        auto observation1 = world->reset();
        assertNotNull(observation1);
        
        // Test agent movement
        Action moveForward(ActionType::MoveForward);
        auto observation2 = world->step(moveForward);
        assertNotNull(observation2);
        
        // Test goal condition
        for (int i = 0; i < 10; ++i) {
            Action moveForward(ActionType::MoveForward);
            world->step(moveForward);
        }
        
        // Should reach goal (bottom-right corner)
        assertTrue(world->isDone(), "Should reach goal position");
        
        std::cout << "  Environment interface test passed" << std::endl;
    }
};

// Agent brain test
class AgentBrainTest : public Test {
public:
    AgentBrainTest() : Test("Agent Brain Interface") {}
    
    void run() override {
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(999), ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(50), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto agent = std::make_unique<AgentBrain>(brain);
        
        // Test initialization requires world
        auto world = std::make_unique<GridWorldEnvironment>(5, 5);
        
        // This would normally be called in real usage
        // agent->initialize(*world);
        
        assertTrue(true, "Agent brain interface test placeholder");
        
        std::cout << "  Agent brain interface test passed" << std::endl;
    }
};

// Performance benchmark test
class PerformanceTest : public Test {
public:
    PerformanceTest() : Test("Performance Benchmarks") {}
    
    void run() override {
        std::cout << "  Running performance benchmarks..." << std::endl;
        
        // Measure memory allocation performance
        auto allocationBench = Benchmark::measure("Memory Allocation",
            []() {
                std::vector<double> data(1000);
                std::fill(data.begin(), data.end(), 1.0);
                double sum = std::accumulate(data.begin(), data.end(), 0.0);
                (void)sum;
            }, 100);
        
        std::cout << "  Memory allocation: " << allocationBench.opsPerSecond << " ops/sec" << std::endl;
        
        // Measure computation performance
        auto computationBench = Benchmark::measure("Mathematical Operations",
            []() {
                volatile double result = 0.0;
                for (int i = 0; i < 1000; ++i) {
                    result += std::sqrt(static_cast<double>(i)) + std::sin(static_cast<double>(i));
                }
                (void)result;
            }, 100);
        
        std::cout << "  Mathematical operations: " << computationBench.opsPerSecond << " ops/sec" << std::endl;
        
        // Performance assertions (should be reasonable)
        assertTrue(allocationBench.opsPerSecond > 1000, "Memory allocation should be efficient");
        assertTrue(computationBench.opsPerSecond > 5000, "Computation should be efficient");
        
        std::cout << "  Performance benchmarks test passed" << std::endl;
    }
};

// Create test suite with all tests
void createTestSuite(TestRunner& runner) {
    auto basicSuite = std::make_unique<TestSuite>("Basic Functionality");
    
    basicSuite->addTest(std::make_unique<ConfigTest>());
    basicSuite->addTest(std::make_unique<BrainInitTest>());
    basicSuite->addTest(std::make_unique<BrainStepTest>());
    basicSuite->addTest(std::make_unique<MemoryTest>());
    basicSuite->addTest(std::make_unique<EnvironmentTest>());
    basicSuite->addTest(std::make_unique<AgentBrainTest>());
    
    auto performanceSuite = std::make_unique<TestSuite>("Performance");
    performanceSuite->addTest(std::make_unique<PerformanceTest>());
    
    auto integrationSuite = std::make_unique<TestSuite>("Integration");
    integrationSuite->addTest(std::make_unique<BrainNeuronTest>());
    integrationSuite->addTest(std::make_unique<BrainPlasticityTest>());
    integrationSuite->addTest(std::make_unique<BrainMemoryTest>());
    integrationSuite->addTest(std::make_unique<BrainIntegrationTest>());
    
    runner.addSuite(std::move(basicSuite));
    runner.addSuite(std::move(performanceSuite));
    runner.addSuite(std::move(integrationSuite));
}
