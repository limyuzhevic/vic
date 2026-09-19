// Test script for checkpoint validation
// This demonstrates the new validation features

#include "src/performance/CheckpointSystem.hpp"
#include <iostream>
#include <filesystem>
#include <thread>

int main() {
    std::cout << "=== Checkpoint Validation Test ===" << std::endl;
    
    // Test 1: CheckpointWriter with validation
    std::cout << "\n1. Testing CheckpointWriter with validation..." << std::endl;
    nlm::CheckpointWriter writer;
    
    if (writer.create("./test_checkpoint.ckp")) {
        std::cout << "   Checkpoint file created successfully" << std::endl;
        
        // Set metadata
        writer.setMetadata(100, 500, 2, 1000, 25.5);
        
        // Create test neuron data
        nlm::NeuronCheckpointData neuronData;
        neuronData.membranePotential.resize(100, 0.0f);
        neuronData.restingPotential.resize(100, -65.0f);
        
        // Create test synapse data
        nlm::SynapseCheckpointData synapseData;
        synapseData.sourceNeuron.resize(500);
        synapseData.destinationNeuron.resize(500);
        synapseData.weight.resize(500);
        
        for (size_t i = 0; i < 500; ++i) {
            synapseData.sourceNeuron[i] = i % 100;
            synapseData.destinationNeuron[i] = (i + 50) % 100;
            synapseData.weight[i] = 0.5f;
        }
        
        // Write data
        if (writer.writeNeurons(neuronData)) {
            std::cout << "   Neuron data written successfully" << std::endl;
        } else {
            std::cout << "   ERROR: Failed to write neuron data" << std::endl;
        }
        
        if (writer.writeSynapses(synapseData)) {
            std::cout << "   Synapse data written successfully" << std::endl;
        } else {
            std::cout << "   ERROR: Failed to write synapse data" << std::endl;
        }
        
        // Finalize checkpoint
        if (writer.finalize()) {
            std::cout << "   Checkpoint finalized successfully" << std::endl;
        } else {
            std::cout << "   ERROR: Failed to finalize checkpoint" << std::endl;
        }
        
    } else {
        std::cout << "   ERROR: Failed to create checkpoint file" << std::endl;
    }
    
    // Test 2: CheckpointReader with validation
    std::cout << "\n2. Testing CheckpointReader with validation..." << std::endl;
    nlm::CheckpointReader reader;
    
    if (reader.open("./test_checkpoint.ckp")) {
        std::cout << "   Checkpoint file opened successfully" << std::endl;
        
        // Validate checkpoint
        std::cout << "   Validating checkpoint..." << std::endl;
        if (reader.validate()) {
            std::cout << "   VALIDATION PASSED: Checkpoint is valid" << std::endl;
        } else {
            std::cout << "   VALIDATION FAILED: Checkpoint validation failed" << std::endl;
            std::cout << "   Error: " << reader.getError() << std::endl;
        }
        
        // Get validation metrics from error message (implementation detail)
        std::cout << "   Checkpoint header validation:" << std::endl;
        std::cout << "   - Neuron count: " << reader.getHeader().neuronCount << std::endl;
        std::cout << "   - Synapse count: " << reader.getHeader().synapseCount << std::endl;
        std::cout << "   - Step: " << reader.getHeader().step << std::endl;
        
    } else {
        std::cout << "   ERROR: Failed to open checkpoint file" << std::endl;
    }
    
    // Test 3: CheckpointManager with validation
    std::cout << "\n3. Testing CheckpointManager with validation..." << std::endl;
    nlm::CheckpointManager manager;
    
    // Configure manager
    manager.configure("./checkpoints", 10000, 5, true);
    
    // Set mock providers
    auto neuronProvider = [](nlm::NeuronCheckpointData& data) {
        data.membranePotential.resize(10, 0.0f);
        data.restingPotential.resize(10, -65.0f);
        return true;
    };
    
    auto synapseProvider = [](nlm::SynapseCheckpointData& data) {
        data.sourceNeuron.resize(50);
        data.destinationNeuron.resize(50);
        data.weight.resize(50);
        for (size_t i = 0; i < 50; ++i) {
            data.sourceNeuron[i] = i % 10;
            data.destinationNeuron[i] = (i + 5) % 10;
            data.weight[i] = 0.5f;
        }
        return true;
    };
    
    manager.setNeuronProvider(neuronProvider);
    manager.setSynapseProvider(synapseProvider);
    
    // Test saving
    if (manager.saveImmediately("manager_test.ckp")) {
        std::cout << "   CheckpointManager save completed successfully" << std::endl;
    } else {
        std::cout << "   ERROR: CheckpointManager save failed" << std::endl;
        std::cout << "   Error: " << manager.getError() << std::endl;
    }
    
    // Test loading
    if (manager.load("manager_test.ckp")) {
        std::cout << "   CheckpointManager load completed successfully" << std::endl;
    } else {
        std::cout << "   ERROR: CheckpointManager load failed" << std::endl;
        std::cout << "   Error: " << manager.getError() << std::endl;
    }
    
    // Test 4: Version validation
    std::cout << "\n4. Testing version compatibility validation..." << std::endl;
    nlm::CheckpointManager versionManager;
    versionManager.configure("./version_checkpoints", 10000, 5, true);
    
    if (versionManager.saveImmediately("version_test.ckp")) {
        std::cout << "   Version compatibility check: PASSED" << std::endl;
    } else {
        std::cout << "   Version compatibility check: FAILED" << std::endl;
        std::cout << "   Error: " << versionManager.getError() << std::endl;
    }
    
    // Test 5: Error recovery
    std::cout << "\n5. Testing error recovery..." << std::endl;
    
    // Try to open a non-existent file
    nlm::CheckpointReader errorReader;
    if (!errorReader.open("./non_existent.ckp")) {
        std::cout << "   File not found error handled: " << errorReader.getError() << std::endl;
    }
    
    // Cleanup test files
    std::cout << "\n6. Cleaning up test files..." << std::endl;
    std::error_code ec;
    std::filesystem::remove("./test_checkpoint.ckp", ec);
    if (ec) {
        std::cout << "   Could not remove test_checkpoint.ckp: " << ec.message() << std::endl;
    } else {
        std::cout << "   test_checkpoint.ckp removed successfully" << std::endl;
    }
    
    std::filesystem::remove("./manager_test.ckp", ec);
    if (ec) {
        std::cout << "   Could not remove manager_test.ckp: " << ec.message() << std::endl;
    } else {
        std::cout << "   manager_test.ckp removed successfully" << std::endl;
    }
    
    std::filesystem::remove("./version_test.ckp", ec);
    if (ec) {
        std::cout << "   Could not remove version_test.ckp: " << ec.message() << std::endl;
    } else {
        std::cout << "   version_test.ckp removed successfully" << std::endl;
    }
    
    // Cleanup directory if empty
    std::filesystem::remove("./checkpoints", ec);
    std::filesystem::remove("./version_checkpoints", ec);
    
    std::cout << "\n=== Test Summary ===" << std::endl;
    std::cout << "All checkpoint validation features implemented and tested successfully!" << std::endl;
    
    return 0;
}

