// ConfigManager Tests
#include "core/ConfigManager/ConfigManager.hpp"
#include <cassert>
#include <iostream>

namespace test_config_manager {

void testConfigManagerCreation() {
    nlm::ConfigManager configManager;
    
    // Initialize with minimal argc/argv (just program name)
    int argc = 1;
    char* argv[1];
    argv[0] = (char*)"nlm";
    
    assert(configManager.initialize(argc, argv));
    
    // Check default values
    const auto& expOpts = configManager.getExperimentOptions();
    assert(expOpts.phase == 1);
    assert(expOpts.experimentType.empty());
    assert(expOpts.demoMode == false);
    
    const auto& simOpts = configManager.getSimulationOptions();
    assert(simOpts.steps == 1000);
    assert(simOpts.timestep == 0.001);
    
    const auto& outOpts = configManager.getOutputOptions();
    assert(outOpts.verbose == false);
    assert(outOpts.quiet == false);
    assert(outOpts.noBanner == false);
    
    const auto& debugOpts = configManager.getDebugOptions();
    assert(debugOpts.debug == false);
    
    std::cout << "    testConfigManagerCreation passed" << std::endl;
}

void testConfigManagerHelp() {
    nlm::ConfigManager configManager;
    
    // Test help doesn't crash
    configManager.printHelp();
    
    std::cout << "    testConfigManagerHelp passed" << std::endl;
}

void runAll() {
    testConfigManagerCreation();
    testConfigManagerHelp();
}

} // namespace test_config_manager
