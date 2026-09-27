// Persistence Tests
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "core/Config/Config.hpp"
#include "persistence/PersistenceManager.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace test_persistence {

void cleanupTestFiles() {
    std::filesystem::remove_all("./test_brains");
    std::filesystem::remove_all("./test_checkpoints");
    std::filesystem::remove_all("./test_agent_brains");
    std::filesystem::remove_all("./persistence_test_backup");
}

void testPersistenceManagerCreation() {
    std::cout << "  testPersistenceManagerCreation..." << std::endl;
    
    nlm::PersistenceManager manager;
    bool initialized = manager.initialize("./test_brains");
    assert(initialized);
    
    std::string baseDir = manager.getBaseDirectory();
    assert(!baseDir.empty());
    
    std::cout << "    PASSED" << std::endl;
}

void testBrainStateSaveLoad() {
    std::cout << "  testBrainStateSaveLoad..." << std::endl;
    
    // Create config
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    config->set("random_seed", static_cast<int64_t>(123456), nlm::ConfigSource::Default);
    config->set("persistence_dir", "./test_brains", nlm::ConfigSource::Default);
    config->set("checkpoint_dir", "./test_checkpoints", nlm::ConfigSource::Default);
    
    // Create and initialize brain
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Test persistence
    std::string testFile = "test_brain.bml";
    bool saveSuccess = brain.saveBrainState(testFile);
    assert(saveSuccess);
    
    bool loadSuccess = brain.loadBrainState(testFile);
    assert(loadSuccess);
    
    std::cout << "    PASSED" << std::endl;
}

void testPersistenceStateInfo() {
    std::cout << "  testPersistenceStateInfo..." << std::endl;
    
    // Create config
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("persistence_dir", "./test_brains", nlm::ConfigSource::Default);
    
    // Create and initialize brain
    nlm::Brain brain(config);
    brain.initialize();
    
    std::string testFile = "test_brain_info.bml";
    brain.saveBrainState(testFile);
    
    // Get state info
    auto stateInfo = brain.getPersistentStateInfo(testFile);
    assert(!stateInfo.checksum.empty());
    assert(stateInfo.neuronCount > 0);
    assert(stateInfo.synapseCount >= 0);
    assert(stateInfo.versionMajor == 1);
    
    std::cout << "    PASSED" << std::endl;
}

void testBackupCreationRestore() {
    std::cout << "  testBackupCreationRestore..." << std::endl;
    
    // Create config
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("persistence_dir", "./test_brains", nlm::ConfigSource::Default);
    
    // Create and initialize brain
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create backup
    std::string backupName = brain.createBackup();
    assert(!backupName.empty());
    
    // Restore from backup
    bool restoreSuccess = brain.restoreFromBackup(backupName);
    assert(restoreSuccess);
    
    std::cout << "    PASSED" << std::endl;
}

void testListBrainStates() {
    std::cout << "  testListBrainStates..." << std::endl;
    
    // Create config
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("persistence_dir", "./test_brains", nlm::ConfigSource::Default);
    
    // Create and initialize brain
    nlm::Brain brain(config);
    brain.initialize();
    
    // Save a brain state
    std::string testFile = "test_list_brain.bml";
    brain.saveBrainState(testFile);
    
    // List brain states
    std::vector<std::string> brainStates = brain.listBrainStates();
    assert(!brainStates.empty());
    
    // Check if our test file is in the list
    bool found = false;
    for (const auto& filename : brainStates) {
        if (filename.find("test_list_brain") != std::string::npos) {
            found = true;
            break;
        }
    }
    assert(found);
    
    std::cout << "    PASSED" << std::endl;
}

void testPersistenceAvailability() {
    std::cout << "  testPersistenceAvailability..." << std::endl;
    
    // Create config
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("persistence_dir", "./test_brains", nlm::ConfigSource::Default);
    
    // Create and initialize brain
    nlm::Brain brain(config);
    brain.initialize();
    
    std::string testFile = "test_availability.bml";
    
    // Save brain state
    brain.saveBrainState(testFile);
    
    // Check availability
    bool isAvailable = brain.isPersistentStateAvailable(testFile);
    assert(isAvailable);
    
    // Clean up non-existent file check
    std::string nonexistent = "nonexistent.bml";
    bool notAvailable = brain.isPersistentStateAvailable(nonexistent);
    assert(!notAvailable);
    
    std::cout << "    PASSED" << std::endl;
}

void testAgentBrainPersistence() {
    std::cout << "  testAgentBrainPersistence..." << std::endl;
    
    // Create config
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    config->set("persistence_dir", "./test_agent_brains", nlm::ConfigSource::Default);
    
    // Create and initialize brain
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create agent brain
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    agentBrain.enablePersistence("./test_agent_brains");
    
    std::string agentFile = "agent_brain.bml";
    
    // Save agent brain
    bool saveSuccess = agentBrain.saveBrainState(agentFile);
    assert(saveSuccess);
    
    // Load agent brain
    bool loadSuccess = agentBrain.loadBrainState(agentFile);
    assert(loadSuccess);
    
    std::cout << "    PASSED" << std::endl;
}

void testCheckpointIntegration() {
    std::cout << "  testCheckpointIntegration..." << std::endl;
    
    // Create config
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("checkpoint_dir", "./test_checkpoints", nlm::ConfigSource::Default);
    config->set("save_interval_steps", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    
    // Create and initialize brain
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test checkpoint manager integration
    auto* persistenceManager = brain.getPersistenceManager();
    assert(persistenceManager != nullptr);
    
    auto* checkpointManager = persistenceManager->getCheckpointManager();
    assert(checkpointManager != nullptr);
    
    // Set checkpoint providers
    bool (*neuronProvider)(nlm::NeuronCheckpointData&) = [](nlm::NeuronCheckpointData&) {
        return true; // Simplified for test
    };
    bool (*synapseProvider)(nlm::SynapseCheckpointData&) = [](nlm::SynapseCheckpointData&) {
        return true; // Simplified for test
    };
    
    checkpointManager->setNeuronProvider(neuronProvider);
    checkpointManager->setSynapseProvider(synapseProvider);
    
    // Test checkpoint saving
    bool saveSuccess = checkpointManager->saveImmediately();
    assert(saveSuccess);
    
    // Test checkpoint listing
    auto checkpoints = checkpointManager->listCheckpoints();
    assert(!checkpoints.empty());
    
    std::cout << "    PASSED" << std::endl;
}

void runAll() {
    std::cout << "\n=== NLM Brain Persistence Tests ===" << std::endl;
    
    // Clean up any previous test runs
    cleanupTestFiles();
    
    testPersistenceManagerCreation();
    testBrainStateSaveLoad();
    testPersistenceStateInfo();
    testBackupCreationRestore();
    testListBrainStates();
    testPersistenceAvailability();
    testAgentBrainPersistence();
    testCheckpointIntegration();
    
    std::cout << "\n=== All Persistence Tests PASSED ===" << std::endl;
}

} // namespace test_persistence
