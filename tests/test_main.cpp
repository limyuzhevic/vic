// Main Tests
#include <iostream>
#include <cassert>
#include <memory>
#include "core/Config/Config.hpp"

namespace test_main {

void testConfigFromArgs() {
    // Test basic argument parsing
    int argc = 5;
    char* argv[] = {
        (char*)"test_program",
        (char*)"--neurons", (char*)"100",
        (char*)"--steps", (char*)"1000",
        (char*)"--learning-rate", (char*)"0.05"
    };
    
    auto cfg = std::make_shared<nlm::Config>();
    cfg->loadFromArgs(argc, argv);
    
    // Check that arguments were parsed correctly
    assert(cfg->getOr<int>("neurons", 0) == 100);
    assert(cfg->getOr<int>("steps", 0) == 1000);
    assert(cfg->getOr<double>("learning_rate", 0.0) == 0.05);
    
    std::cout << "    testConfigFromArgs passed" << std::endl;
}

void testConfigDefaultValues() {
    auto cfg = std::make_shared<nlm::Config>();
    
    // Test default values from Config.hpp
    cfg->set("neuron_count", 500, nlm::ConfigSource::Default);
    cfg->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
    cfg->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    assert(cfg->getOr<int>("neuron_count", 0) == 500);
    assert(cfg->getOr<double>("simulation_timestep", 0.0) == 0.001);
    assert(cfg->getOr<double>("connection_probability", 0.0) == 0.1);
    
    std::cout << "    testConfigDefaultValues passed" << std::endl;
}

void testConfigSourcePriority() {
    auto cfg = std::make_shared<nlm::Config>();
    
    // Set values from different sources
    cfg->set("learning_rate", 0.01, nlm::ConfigSource::Default);
    cfg->set("learning_rate", 0.02, nlm::ConfigSource::File);
    cfg->set("learning_rate", 0.03, nlm::ConfigSource::CommandLine);
    
    // The source should reflect the last set operation
    // For now, we check that it exists
    assert(cfg->has("learning_rate"));
    assert(cfg->getOr<double>("learning_rate", 0.0) == 0.03);
    
    std::cout << "    testConfigSourcePriority passed" << std::endl;
}

void testConfigFilePaths() {
    // Test that config file path handling works
    auto cfg = std::make_shared<nlm::Config>();
    
    // This should not crash
    cfg->loadFromFile("/nonexistent/path/config.txt");
    
    // It should return false for non-existent files
    // (the implementation should handle this gracefully)
    
    std::cout << "    testConfigFilePaths passed" << std::endl;
}

void runAll() {
    testConfigFromArgs();
    testConfigDefaultValues();
    testConfigSourcePriority();
    testConfigFilePaths();
}

} // namespace test_main
