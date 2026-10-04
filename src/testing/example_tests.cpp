// Example: Brain Unit Tests using the New Test Framework
// This file demonstrates how to use the enhanced test framework

#include "testing/TestFramework.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

// Example test case using the new test framework
class BrainUnitTest : public TestCase {
public:
    void runAllTests() override {
        std::cout << "Running BrainUnitTest..." << std::endl;
        
        // Test 1: Brain Creation
        {
            Timer timer("testBrainCreation");
            auto config = std::make_shared<nlm::Config>();
            config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
            
            nlm::Brain brain(config);
            
            // Use the new assertion macros
            ASSERT_TRUE(brain.getTotalNeuronCount() > 0, "Brain should have neurons");
            ASSERT_EQUAL(brain.getTotalNeuronCount(), 100, "Brain should have 100 neurons");
            ASSERT_EQUAL(brain.getRegionCount(), 1, "Brain should have 1 region");
        }
        
        // Test 2: Brain Initialization
        {
            Timer timer("testBrainInitialization");
            auto config = std::make_shared<nlm::Config>();
            config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
            config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
            
            nlm::Brain brain(config);
            
            bool success = brain.initialize();
            ASSERT_TRUE(success, "Brain initialization should succeed");
            ASSERT_TRUE(brain.getRegionCount() == 2, "Brain should have 2 regions");
            ASSERT_TRUE(brain.getTotalNeuronCount() == 50, "Brain should have 50 neurons");
            ASSERT_TRUE(brain.getTotalSynapseCount() > 0, "Brain should have synapses");
        }
        
        // Test 3: Brain Step
        {
            Timer timer("testBrainStep");
            auto config = std::make_shared<nlm::Config>();
            config->set("neuron_count", static_cast<int64_t>(25), nlm::ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
            
            nlm::Brain brain(config);
            brain.initialize();
            
            // Run multiple steps
            for (nlm::SimulationStep step = 0; step < 20; ++step) {
                brain.step(step);
            }
            
            ASSERT_TRUE(brain.getTotalNeuronCount() == 25, "Brain should maintain neuron count");
        }
        
        // Test 4: Brain Reset
        {
            Timer timer("testBrainReset");
            auto config = std::make_shared<nlm::Config>();
            config->set("neuron_count", static_cast<int64_t>(30), nlm::ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
            
            nlm::Brain brain(config);
            brain.initialize();
            
            brain.step(0);
            brain.reset();
            
            ASSERT_TRUE(brain.getTotalNeuronCount() == 30, "Reset should restore neuron count");
            ASSERT_TRUE(!brain.isInitialized(), "Brain should be re-initialized after reset");
        }
        
        // Test 5: Brain Regions
        {
            Timer timer("testBrainRegions");
            auto config = std::make_shared<nlm::Config>();
            config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
            
            nlm::Brain brain(config);
            brain.initialize();
            
            auto regionIds = brain.getRegionIds();
            ASSERT_EQUAL(regionIds.size(), 3, "Should have 3 regions");
            
            for (auto rid : regionIds) {
                auto* region = brain.getRegion(rid);
                ASSERT_TRUE(region != nullptr, "All regions should be valid");
            }
        }
        
        // Test 6: Config Operations
        {
            Timer timer("testConfigOperations");
            auto config = std::make_shared<nlm::Config>();
            
            config->set("neuron_count", static_cast<int64_t>(75), nlm::ConfigSource::Default);
            ASSERT_TRUE(config->has("neuron_count"), "Config should have neuron_count");
            ASSERT_EQUAL(config->getOr<int64_t>("neuron_count", 0), 75, "Config should return correct value");
            
            std::vector<std::string> keys = config->getKeys();
            ASSERT_TRUE(!keys.empty(), "Config should have keys");
        }
    }
};

// Register the test case using the new macro
REGISTER_TEST_CASE(BrainUnitTest)

// Additional example test cases can be added here
class ConfigUnitTest : public TestCase {
public:
    void runAllTests() override {
        std::cout << "Running ConfigUnitTest..." << std::endl;
        
        // Test Config creation and basic operations
        {
            Timer timer("testConfigCreation");
            auto config = std::make_shared<nlm::Config>();
            
            config->set("test_int", static_cast<int64_t>(42), nlm::ConfigSource::Default);
            config->set("test_float", 3.14f, nlm::ConfigSource::Default);
            config->set("test_string", "hello", nlm::ConfigSource::Default);
            
            ASSERT_TRUE(config->has("test_int"), "Config should have test_int");
            ASSERT_EQUAL(config->getOr<int64_t>("test_int", 0), 42, "Config should return correct int");
            ASSERT_EQUAL(config->getOr<float>("test_float", 0.0f), 3.14f, "Config should return correct float");
            ASSERT_EQUAL(config->getOr<std::string>("test_string", ""), std::string("hello"), "Config should return correct string");
        }
    }
};

REGISTER_TEST_CASE(ConfigUnitTest)