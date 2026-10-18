// Python Bindings Unit Tests
// Test the new advanced features added to the NLM Python bindings

#include <iostream>
#include <cassert>
#include <filesystem>
#include "python/bindings.cpp"

namespace test_bindings {

void testConfigHelpers() {
    std::cout << "Testing Config Helpers..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("test_int", 42);
    config->set("test_string", "hello");
    config->set("test_double", 3.14);
    
    // Test getConfigValue
    int intVal = config_helpers::getConfigValue<int>(config, "test_int", 0);
    assert(intVal == 42);
    
    std::string strVal = config_helpers::getConfigValue<std::string>(config, "test_string", "");
    assert(strVal == "hello");
    
    double doubleVal = config_helpers::getConfigValue<double>(config, "test_double", 0.0);
    assert(doubleVal > 3.13 && doubleVal < 3.15);
    
    // Test batchSetConfig
    std::vector<std::pair<std::string, std::string>> batch = {
        {"batch1", "value1"},
        {"batch2", "value2"}
    };
    config_helpers::batchSetConfig(config, batch);
    assert(config->get<std::string>("batch1") == "value1");
    
    // Test getConfigStats
    auto stats = config_helpers::getConfigStats(config);
    assert(stats.find("total_entries") != stats.end());
    
    // Test validateConfig
    bool valid = config_helpers::validateConfig(config);
    assert(valid == true);  // Should be valid with our test values
    
    std::cout << "  PASSED" << std::endl;
}

void testConvenienceFunctions() {
    std::cout << "Testing Convenience Functions..." << std::endl;
    
    // Test createAgentSetup
    auto [brain, world, agent] = convenience::createAgentSetup(100, 10, 5, 5, true);
    assert(brain != nullptr);
    assert(world != nullptr);
    assert(agent != nullptr);
    assert(brain->getTotalNeuronCount() == 100);
    assert(agent->isRewardModulationEnabled() == true);
    
    // Test runAgentSimulation (quick test)
    convenience::runAgentSimulation(brain, world, agent, 10, false);
    assert(brain->getTotalNeuronCount() == 100);
    
    // Test getBrainStats
    auto brainStats = convenience::getBrainStats(brain);
    assert(brainStats.find("neuron_count") != brainStats.end());
    assert(brainStats.find("synapse_count") != brainStats.end());
    
    // Test getAgentStats
    auto agentStats = convenience::getAgentStats(agent);
    assert(agentStats.find("curiosity_level") != agentStats.end());
    assert(agentStats.find("novelty_level") != agentStats.end());
    
    std::cout << "  PASSED" << std::endl;
}

void testSerializationUtilities() {
    std::cout << "Testing Serialization Utilities..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("save_test", "value");
    config->set("save_number", 123);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Test saveAgentToDirectory
    std::string testDir = "/tmp/nlm_test_save";
    bool saveSuccess = serialization::saveAgentToDirectory(brain, config, testDir);
    assert(saveSuccess == true);
    
    // Verify files were created
    assert(std::filesystem::exists(testDir + "/brain.bin"));
    assert(std::filesystem::exists(testDir + "/config.json"));
    
    // Test backupState
    bool backupSuccess = serialization::backupState(brain, config, "/tmp/nlm_test_backup");
    assert(backupSuccess == true);
    
    // Clean up
    std::filesystem::remove_all("/tmp/nlm_test_save");
    std::filesystem::remove_all("/tmp/nlm_test_backup");
    
    std::cout << "  PASSED" << std::endl;
}

void testDebuggingTools() {
    std::cout << "Testing Debugging Tools..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("brain.neuron_count", 50);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Test printBrainState (should not crash)
    debugging::printBrainState(brain, false);
    
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(10, 10, 5, 5);
    world->reset();
    agent->initialize(*world);
    
    // Test printAgentState (should not crash)
    debugging::printAgentState(agent, false);
    
    // Test enableVerboseLogging
    debugging::enableVerboseLogging();
    
    std::cout << "  PASSED" << std::endl;
}

void testAdvancedAgentManagement() {
    std::cout << "Testing Advanced Agent Management..." << std::endl;
    
    // Test createAdvancedAgent
    auto [brain, world, agent] = advanced::createAdvancedAgent("advanced", "default", true);
    assert(brain != nullptr);
    assert(world != nullptr);
    assert(agent != nullptr);
    
    // Verify advanced configuration was applied
    assert(brain->getTotalNeuronCount() == 2000);
    assert(agent->isRewardModulationEnabled() == true);
    assert(agent->isCuriosityEnabled() == true);
    
    // Test runExperiment (quick test)
    advanced::runExperiment(brain, world, agent, 50, "test_experiment");
    
    std::cout << "  PASSED" << std::endl;
}

void runAll() {
    testConfigHelpers();
    testConvenienceFunctions();
    testSerializationUtilities();
    testDebuggingTools();
    testAdvancedAgentManagement();
    
    std::cout << "=== All Binding Tests PASSED ===" << std::endl;
}

} // namespace test_bindings