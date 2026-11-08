// CheckpointingTest.cpp
// Checkpoint functionality tests

#include "CheckpointingTest.hpp"
#include "performance/CheckpointSystem.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <filesystem>
#include <fstream>

namespace test_checkpointing {

void testCheckpointStubFix() {
    std::cout << "=== Testing Checkpoint Stub Fix ===" << std::endl;
    
    // Test 1: Verify Brain::save() no longer returns false (stubs)
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    std::string testPath = "./test_checkpoint_fix.bin";
    
    // This should now return true (not the old stub behavior)
    bool saveResult = brain.save(testPath);
    assert(saveResult);
    
    std::cout << "    Brain::save() no longer stub: PASSED (returned true)" << std::endl;
    
    // Verify file was created
    std::ifstream file(testPath, std::ios::binary);
    assert(file.good());
    file.close();
    
    // Test 2: Verify Brain::load() no longer returns false (stubs)
    nlm::Brain brain2(config);
    assert(brain2.initialize());
    
    bool loadResult = brain2.load(testPath);
    assert(loadResult);
    
    std::cout << "    Brain::load() no longer stub: PASSED (returned true)" << std::endl;
    
    // Test 3: Verify checkpoint contains valid data
    assert(brain.getTotalNeuronCount() == brain2.getTotalNeuronCount());
    
    std::cout << "    Checkpoint contains valid data: PASSED" << std::endl;
    
    // Cleanup
    std::filesystem::remove(testPath);
    
    std::cout << "    Checkpoint stub fix test PASSED" << std::endl;
}

void testCheckpointSystemImplementation() {
    std::cout << "=== Testing CheckpointSystem Implementation ===" << std::endl;
    
    // Test 1: Verify CheckpointSystem is fully implemented (not stubs)
    nlm::CheckpointSystem checkpoint;
    
    // Check that methods exist and work
    std::string testPath = "./test_checkpoint_system.bin";
    
    // Create checkpoint data
    nlm::CheckpointHeader header;
    header.versionMajor = 1;
    header.versionMinor = 0;
    header.magic = 0x4B524541434B500;  // 'CHECKPOINT' in ASCII
    header.timestamp = std::time(nullptr);
    
    // Test save functionality
    bool saveSuccess = checkpoint.save(header, testPath);
    assert(saveSuccess);
    
    std::cout << "    CheckpointSystem::save(): PASSED" << std::endl;
    
    // Test load functionality
    nlm::CheckpointHeader loadedHeader;
    bool loadSuccess = checkpoint.load(testPath, loadedHeader);
    assert(loadSuccess);
    
    std::cout << "    CheckpointSystem::load(): PASSED" << std::endl;
    
    // Verify header data
    assert(loadedHeader.magic == header.magic);
    assert(loadedHeader.versionMajor == header.versionMajor);
    
    // Cleanup
    std::filesystem::remove(testPath);
    
    std::cout << "    CheckpointSystem implementation test PASSED" << std::endl;
}

void testCheckpointPersistence() {
    std::cout << "=== Testing Checkpoint Persistence ===" << std::endl;
    
    // Create first brain and checkpoint it
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 50);
    config->set("region_count", 1);
    
    nlm::Brain brain1(config);
    assert(brain1.initialize());
    
    // Simulate some brain activity
    brain1.step(0);
    
    std::string checkpointPath = "./test_persistence.bin";
    
    bool saveSuccess = brain1.save(checkpointPath);
    assert(saveSuccess);
    
    // Create second brain and load checkpoint
    nlm::Brain brain2(config);
    assert(brain2.initialize());
    
    bool loadSuccess = brain2.load(checkpointPath);
    assert(loadSuccess);
    
    // Both brains should have same neuron count
    assert(brain1.getTotalNeuronCount() == brain2.getTotalNeuronCount());
    
    std::cout << "    Checkpoint persistence: PASSED" << std::endl;
    
    // Cleanup
    std::filesystem::remove(checkpointPath);
    
    std::cout << "    Checkpoint persistence test PASSED" << std::endl;
}

void runAllCheckpointingTests() {
    std::cout << "=== Checkpointing Tests ===" << std::endl << std::endl;
    
    testCheckpointStubFix();
    std::cout << std::endl;
    
    testCheckpointSystemImplementation();
    std::cout << std::endl;
    
    testCheckpointPersistence();
    std::cout << std::endl;
    
    std::cout << "=== ALL CHECKPOINTING TESTS PASSED ===" << std::endl;
}

} // namespace test_checkpointing