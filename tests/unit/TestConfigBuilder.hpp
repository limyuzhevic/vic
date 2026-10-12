#ifndef TEST_CONFIG_BUILDER_HPP
#define TEST_CONFIG_BUILDER_HPP

#include "tests/Framework/TestFramework.hpp"
#include "src/core/Config/ConfigBuilder.hpp"

class TestConfigBuilder : public TestCase {
public:
    TestConfigBuilder() : TestCase("ConfigBuilder") {}
    
    void run() override {
        ConfigBuilder::Config config = createTestConfig();
        
        // Test basic config creation
        ASSERT_TRUE(config.learning_rate > 0.0, "Learning rate should be positive");
        ASSERT_TRUE(config.plasticity_rate > 0.0, "Plasticity rate should be positive");
        ASSERT_TRUE(config.memory_capacity > 0.0, "Memory capacity should be positive");
        ASSERT_TRUE(config.max_neurons > 0, "Max neurons should be positive");
        ASSERT_TRUE(config.max_synapses > 0, "Max synapses should be positive");
        ASSERT_TRUE(config.max_populations > 0, "Max populations should be positive");
        ASSERT_TRUE(config.max_regions > 0, "Max regions should be positive");
        ASSERT_TRUE(config.dt > 0.0, "Time step should be positive");
        ASSERT_TRUE(config.max_steps > 0, "Max steps should be positive");
        ASSERT_TRUE(config.checkpoint_interval > 0, "Checkpoint interval should be positive");
        ASSERT_TRUE(config.num_threads > 0, "Number of threads should be positive");
        
        // Test config builder functionality
        ConfigBuilder::Config config2 = ConfigBuilder::createDefaultConfig();
        ASSERT_TRUE(config2.learning_rate == 0.01, "Default learning rate should be 0.01");
        
        // Test config validation
        ConfigBuilder::Config invalid_config;
        invalid_config.learning_rate = -1.0;
        ConfigBuilder::validateConfig(invalid_config);
        
        // Test config copying
        ConfigBuilder::Config config_copy = config;
        ASSERT_EQUAL(config_copy.learning_rate, config.learning_rate, "Config copy should match original");
    }
};

#endif // TEST_CONFIG_BUILDER_HPP