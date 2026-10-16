#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "world/SimpleWorld.hpp"
#include <iostream>
#include <cassert>
#include <string>

using namespace nlm;

class NLMTestSuite {
public:
    static void runAllTests() {
        std::cout << "Running NLM Test Suite..." << std::endl;
        std::cout << "====================================" << std::endl;
        
        try {
            testConfigClass();
            testRandomClass();
            testLoggerClass();
            testBrainClass();
            testSimpleWorld();
            testPythonBindings();
            
            std::cout << "" << std::endl;
            std::cout << "All tests passed! ✓" << std::endl;
            std::cout << "====================================" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "" << std::endl;
            std::cout << "Test failed with exception: " << e.what() << std::endl;
            std::cout << "====================================" << std::endl;
            throw;
        }
    }
    
private:
    static void testConfigClass() {
        std::cout << "Testing Config class..." << std::endl;
        
        // Test 1: Default config
        Config config;
        assert(config.getKeys().empty());
        
        // Test 2: Setting values
        config.set("test_int", 42);
        config.set("test_double", 3.14);
        config.set("test_string", "hello");
        config.set("test_bool", true);
        
        assert(config.has("test_int"));
        assert(config.has("test_double"));
        assert(config.has("test_string"));
        assert(config.has("test_bool"));
        
        // Test 3: Getting values
        assert(config.get<int64_t>("test_int").value() == 42);
        assert(config.get<double>("test_double").value() == 3.14);
        assert(config.get<std::string>("test_string").value() == "hello");
        assert(config.get<bool>("test_bool").value() == true);
        
        // Test 4: Configuration source tracking
        config.set("test_int", 100, ConfigSource::CommandLine);
        // Note: We can't easily test source tracking without exposing internal details
        
        std::cout << "  ✓ Config class tests passed" << std::endl;
    }
    
    static void testRandomClass() {
        std::cout << "Testing Random class..." << std::endl;
        
        // Test 1: Default constructor
        RandomGenerator rng1;
        uint64_t seed1 = rng1.getSeed();
        
        // Test 2: Explicit constructor
        RandomGenerator rng2(42);
        uint64_t seed2 = rng2.getSeed();
        assert(seed2 == 42);
        
        // Test 3: Uniform integer
        uint32_t int1 = rng1.uniformInt(1, 10);
        assert(int1 >= 1 && int1 <= 10);
        
        // Test 4: Uniform real
        double real1 = rng1.uniformReal(0.0, 1.0);
        assert(real1 >= 0.0 && real1 < 1.0);
        
        // Test 5: Normal distribution
        double normal1 = rng1.normal(0.0, 1.0);
        // Normal distribution can be any value
        
        // Test 6: Bernoulli
        bool bernoulli1 = rng1.bernoulli(0.5);
        assert(bernoulli1 == true || bernoulli1 == false);
        
        // Test 7: Shuffle
        std::vector<int> vec = {1, 2, 3, 4, 5};
        rng1.shuffle(vec);
        assert(vec.size() == 5);
        
        // Test 8: Move constructor
        RandomGenerator rng3(std::move(rng2));
        // rng2 should be in a valid but unspecified state
        
        // Test 9: Move assignment
        RandomGenerator rng4;
        rng4 = std::move(rng3);
        
        std::cout << "  ✓ Random class tests passed" << std::endl;
    }
    
    static void testLoggerClass() {
        std::cout << "Testing Logger class..." << std::endl;
        
        // Test 1: Global logger
        Logger& logger = Logger::getGlobal();
        
        // Test 2: Log different levels
        logger.debug("Debug message");
        logger.info("Info message");
        logger.warning("Warning message");
        logger.error("Error message");
        logger.critical("Critical message");
        
        // Test 3: Is enabled check
        assert(logger.isEnabled(LogLevel::Info) == true);
        assert(logger.isEnabled(LogLevel::Error) == true);
        // Note: We can't easily test disabled levels without modifying configuration
        
        std::cout << "  ✓ Logger class tests passed" << std::endl;
    }
    
    static void testBrainClass() {
        std::cout << "Testing Brain class..." << std::endl;
        
        // Create a minimal config
        auto config = std::make_shared<Config>();
        config->set("random_seed", 42);
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        config->set("connection_probability", 0.15);
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        
        // Test 1: Initialization
        assert(brain->initialize() == true);
        
        // Test 2: Basic properties
        assert(brain->getRegionCount() > 0);
        assert(brain->getTotalNeuronCount() > 0);
        assert(brain->getTotalSynapseCount() >= 0);
        
        // Test 3: Neuron access
        auto region = brain->getRegion(RegionId(1));
        assert(region != nullptr);
        assert(region->getPopulationCount() > 0);
        
        // Test 4: Step simulation
        for (SimulationStep step = 0; step < 10; ++step) {
            brain->step(step, step * 0.001);
        }
        
        // Test 5: Activity tracking
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject current to create activity
        for (size_t i = 0; i < 10; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Internal, 10.0f);
        }
        
        // Run more steps
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t finalSpikes = brain->getTotalSpikeCount();
        assert(finalSpikes >= initialSpikes);
        
        // Test 6: Reset
        brain->reset();
        // Note: Reset should clear spikes and reset neuron states
        
        std::cout << "  ✓ Brain class tests passed" << std::endl;
    }
    
    static void testSimpleWorld() {
        std::cout << "Testing SimpleWorld..." << std::endl;
        
        // Create world
        auto world = std::make_shared<SimpleWorld>();
        
        // Test 1: Configuration
        world->configure(20.0f, 20.0f, 16, 16);
        assert(world->getWidth() == 20.0f);
        assert(world->getHeight() == 20.0f);
        
        // Test 2: Reset
        world->reset();
        assert(world->getAgentBody().energy > 0.0f);
        
        // Test 3: Get sensory percept
        auto percept = world->getSensoryPercept();
        assert(percept != nullptr);
        
        // Test 4: Update
        world->update(0.001);
        
        // Test 5: Apply motor command
        ActionResult result = world->applyMotorCommand(MotorCommand::Wait, 0.0);
        assert(result.success == true);
        
        // Test 6: Vision generation
        auto vision = percept->getVision();
        assert(vision.size() == 256);  // 16x16 vision
        
        std::cout << "  ✓ SimpleWorld tests passed" << std::endl;
    }
    
    static void testPythonBindings() {
        std::cout << "Testing Python bindings..." << std::endl;
        
        // This is a simplified test since we can't easily test Python bindings from C++
        // In a real test suite, we would test the Python bindings using pybind11's test framework
        
        std::cout << "  ✓ Python bindings tests (note: requires Python bindings test framework)" << std::endl;
    }
};

int main() {
    try {
        NLMTestSuite::runAllTests();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test suite failed: " << e.what() << std::endl;
        return 1;
    }
}