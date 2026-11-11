// DemoFramework Integration Tests
// Tests the new DemoFramework base class functionality

#include "experiments/DemoFramework.hpp"
#include <cassert>
#include <iostream>
#include <string>

namespace test_demo_framework {

void testCommandLineParser() {
    std::cout << "Testing CommandLineParser..." << std::endl;
    
    // Test parsing default arguments
    int argc = 1;
    char* argv[] = { (char*)"test_demo" };
    
    nlm::DemoOptions options = nlm::CommandLineParser::parse(argc, argv, "test_demo");
    
    assert(options.numEpisodes == 1);
    assert(options.stepsPerEpisode == 100);
    assert(options.maxNeurons == 500);
    assert(options.maxSynapses == 500);
    assert(options.seed == 42);
    assert(options.timestep == 0.001f);
    assert(!options.showHelp);
    assert(options.verbose);
    
    // Test help option
    argc = 2;
    char* argv_help[] = { (char*)"test_demo", (char*)"--help" };
    options = nlm::CommandLineParser::parse(argc, argv_help, "test_demo");
    assert(options.showHelp);
    
    // Test custom values
    argc = 4;
    char* argv_custom[] = { (char*)"test_demo", (char*)"--episodes", (char*)"10", (char*)"--steps", (char*)"200" };
    options = nlm::CommandLineParser::parse(argc, argv_custom, "test_demo");
    assert(options.numEpisodes == 10);
    assert(options.stepsPerEpisode == 200);
    
    std::cout << "  PASSED" << std::endl;
}

void testDemoFramework() {
    std::cout << "Testing DemoFramework..." << std::endl;
    
    // Test that we can create a DemoFramework instance
    nlm::DemoFramework demo("Test Demo", "Test demo description");
    
    // Test validation
    nlm::DemoOptions options;
    options.numEpisodes = 1;
    options.stepsPerEpisode = 100;
    options.maxNeurons = 500;
    options.maxSynapses = 500;
    options.timestep = 0.001f;
    
    // Should pass validation
    assert(demo.validateOptions(options));
    
    // Test invalid options
    options.maxNeurons = 0;
    assert(!demo.validateOptions(options));
    
    options.maxNeurons = 500;
    options.maxSynapses = -1;
    assert(!demo.validateOptions(options));
    
    options.maxSynapses = 500;
    options.timestep = -1.0f;
    assert(!demo.validateOptions(options));
    
    options.timestep = 0.001f;
    options.stepsPerEpisode = 0;
    assert(!demo.validateOptions(options));
    
    options.stepsPerEpisode = 100;
    options.numEpisodes = 0;
    assert(!demo.validateOptions(options));
    
    std::cout << "  PASSED" << std::endl;
}

void testConfigurationHelpers() {
    std::cout << "Testing configuration helpers..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    
    nlm::DemoOptions options;
    options.numEpisodes = 5;
    options.stepsPerEpisode = 200;
    options.maxNeurons = 1000;
    options.maxSynapses = 1000;
    options.seed = 123;
    options.timestep = 0.002f;
    
    nlm::DemoFramework demo("Test", "Test");
    demo.applyStandardConfig(config, options);
    
    // Check that configuration was applied
    assert(config->has("neuron_count"));
    assert(config->has("region_count"));
    assert(config->has("connection_probability"));
    assert(config->has("random_seed"));
    assert(config->has("simulation_timestep"));
    
    // Check values
    assert(config->getOr<int64_t>("neuron_count", 0) == 1000);
    assert(config->getOr<int64_t>("region_count", 0) == 1);
    assert(config->getOr<float>("connection_probability", 0.0f) == 0.1f);
    assert(config->getOr<int64_t>("random_seed", 0) == 123);
    assert(config->getOr<double>("simulation_timestep", 0.0) == 0.002);
    
    std::cout << "  PASSED" << std::endl;
}

void testMetrics() {
    std::cout << "Testing metrics..." << std::endl;
    
    nlm::DemoFramework demo("Test", "Test");
    
    // Record some metrics
    demo.recordMetric("reward", 1.5);
    demo.recordMetric("reward", 2.0);
    demo.recordMetric("reward", 1.0);
    demo.recordMetric("spikes", 100);
    demo.recordMetric("spikes", 150);
    
    // Check metric values
    auto& values = demo.metrics_.getValues("reward");
    assert(values.size() == 3);
    assert(values[0] == 1.5);
    assert(values[1] == 2.0);
    assert(values[2] == 1.0);
    
    double mean = demo.metrics_.getMean("reward");
    assert(std::abs(mean - 1.5) < 0.01); // (1.5 + 2.0 + 1.0) / 3 = 1.5
    
    double stddev = demo.metrics_.getStdDev("reward");
    // Standard deviation of [1.5, 2.0, 1.0] = sqrt(((1.5-1.5)^2 + (2.0-1.5)^2 + (1.0-1.5)^2) / 2) = sqrt((0 + 0.25 + 0.25) / 2) = sqrt(0.25) = 0.5
    assert(std::abs(stddev - 0.5) < 0.01);
    
    std::cout << "  PASSED" << std::endl;
}

void runAll() {
    testCommandLineParser();
    testDemoFramework();
    testConfigurationHelpers();
    testMetrics();
    
    std::cout << std::endl;
    std::cout << "=== All DemoFramework Tests PASSED ===" << std::endl;
}

} // namespace test_demo_framework