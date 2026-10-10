#include <iostream>
#include <fstream>
#include <string>
#include "CheckpointSystem.hpp"
#include "PersistenceManager.hpp"
#include "MemoryPersistence.hpp"
#include "NeuralStatePersistence.hpp"
#include "ConfigurationPersistence.hpp"
#include "NeuromodulationPersistence.hpp"
#include "DevelopmentPersistence.hpp"
#include "CheckpointValidator.hpp"
#include "CheckpointIndexer.hpp"
#include "CheckpointCompressor.hpp"
#include "CheckpointEncryptor.hpp"
#include "ResumeManager.hpp"
#include "VersionControl.hpp"
#include "DiffTracker.hpp"

namespace nlm {

// Simple test functions
void testCheckpointSystem() {
    std::cout << "Testing Checkpoint System..." << std::endl;
    
    // Test basic checkpoint creation
    CheckpointWriter writer;
    if (!writer.create("./test_checkpoint.bin")) {
        std::cout << "Failed to create checkpoint file" << std::endl;
        return;
    }
    
    // Set metadata
    writer.setMetadata(1000, 5000, 1, 100, 123.45);
    
    // Create test data
    NeuronCheckpointData neuronData;
    neuronData.membranePotential.resize(100, 0.0f);
    neuronData.restingPotential.resize(100, -65.0f);
    neuronData.threshold.resize(100, -50.0f);
    neuronData.resetPotential.resize(100, -65.0f);
    neuronData.leakConductance.resize(100, 0.1f);
    
    for (int i = 0; i < 10; ++i) {
        neuronData.membranePotential[i] = -70.0f + static_cast<float>(i * 5);
        neuronData.restingPotential[i] = -65.0f;
        neuronData.threshold[i] = -50.0f;
        neuronData.resetPotential[i] = -65.0f;
        neuronData.leakConductance[i] = 0.1f;
    }
    
    if (!writer.writeNeurons(neuronData)) {
        std::cout << "Failed to write neurons" << std::endl;
        writer.abort();
        return;
    }
    
    // Write some synapses
    SynapseCheckpointData synapseData;
    synapseData.sourceNeuron.resize(50);
    synapseData.destinationNeuron.resize(50);
    synapseData.weight.resize(50, 0.1f);
    synapseData.delay.resize(50, 1);
    synapseData.synapseType.resize(50, 0);
    synapseData.plasticityFlags.resize(50, 0);
    synapseData.eligibilityTrace.resize(50, 0.0f);
    synapseData.efficacy.resize(50, 0.1f);
    synapseData.shortTermDepression.resize(50, 0.0f);
    synapseData.shortTermFacilitation.resize(50, 0.0f);
    
    for (int i = 0; i < 50; ++i) {
        synapseData.sourceNeuron[i] = i;
        synapseData.destinationNeuron[i] = (i + 1) % 50;
    }
    
    if (!writer.writeSynapses(synapseData)) {
        std::cout << "Failed to write synapses" << std::endl;
        writer.abort();
        return;
    }
    
    // Finalize
    if (!writer.finalize()) {
        std::cout << "Failed to finalize checkpoint" << std::endl;
        writer.abort();
        return;
    }
    
    writer.close();
    
    // Test reading
    CheckpointReader reader;
    if (!reader.open("./test_checkpoint.bin")) {
        std::cout << "Failed to open checkpoint for reading" << std::endl;
        std::remove("./test_checkpoint.bin");
        return;
    }
    
    if (!reader.validate()) {
        std::cout << "Checkpoint validation failed: " << reader.getError() << std::endl;
        reader.close();
        std::remove("./test_checkpoint.bin");
        return;
    }
    
    // Test reading neurons
    NeuronCheckpointData readNeurons;
    if (!reader.readNeurons(readNeurons)) {
        std::cout << "Failed to read neurons from checkpoint" << std::endl;
        reader.close();
        std::remove("./test_checkpoint.bin");
        return;
    }
    
    // Test reading synapses
    SynapseCheckpointData readSynapses;
    if (!reader.readSynapses(readSynapses)) {
        std::cout << "Failed to read synapses from checkpoint" << std::endl;
        reader.close();
        std::remove("./test_checkpoint.bin");
        return;
    }
    
    reader.close();
    std::remove("./test_checkpoint.bin");
    
    std::cout << "Checkpoint system test passed!" << std::endl;
}

void testPersistenceManager() {
    std::cout << "Testing Persistence Manager..." << std::endl;
    
    // Note: PersistenceManager requires Config, which needs to be initialized
    // For this test, we'll just verify the class exists
    
    std::cout << "PersistenceManager test skipped (requires Config initialization)" << std::endl;
}

void testCheckpointValidator() {
    std::cout << "Testing Checkpoint Validator..." << std::endl;
    
    CheckpointValidator validator;
    
    // Test quick validation on non-existent file
    if (validator.quickValidate("./nonexistent.bin")) {
        std::cout << "ERROR: Should have failed validation for non-existent file" << std::endl;
        return;
    }
    
    std::cout << "Checkpoint validator test passed!" << std::endl;
}

void testAll() {
    std::cout << "=== Testing NLM Persistence System ===" << std::endl;
    
    testCheckpointSystem();
    testPersistenceManager();
    testCheckpointValidator();
    
    std::cout << "\n=== All Tests Completed ===" << std::endl;
}

} // namespace nlm

// Main function for testing
int main() {
    nlm::testAll();
    return 0;
}
