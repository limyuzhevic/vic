// Checkpoint Validation Tests
// Comprehensive tests for checkpoint integrity validation

#include "Checkpoint/CheckpointValidator.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>

namespace test_checkpoint {

void testCheckpointMetadataValidation() {
    std::cout << "    Testing checkpoint metadata validation..." << std::endl;
    
    nlm::CheckpointValidator::CheckpointMetadata metadata;
    metadata.version = "1.0.0";
    metadata.format = "binary";
    metadata.timestamp = std::chrono::system_clock::now();
    metadata.createdBy = "NLM";
    metadata.neuronCount = 1000;
    metadata.synapseCount = 10000;
    metadata.regionCount = 10;
    metadata.checksum = "abc123";
    metadata.hashAlgorithm = "SHA256";
    
    auto result = nlm::CheckpointValidator::validateMetadata(metadata);
    assert(result.valid == true);
    assert(result.checksumValid == true);
    
    std::cout << "      ✓ Checkpoint metadata validation test passed" << std::endl;
}

void testCheckpointVersionCompatibility() {
    std::cout << "    Testing checkpoint version compatibility..." << std::endl;
    
    auto result = nlm::CheckpointValidator::validateVersionCompatibility("1.0.0", "1.0");
    assert(result.versionCompatible == true);
    
    result = nlm::CheckpointValidator::validateVersionCompatibility("0.9.0", "1.0");
    assert(result.versionCompatible == false);
    assert(!result.valid);
    
    std::cout << "      ✓ Checkpoint version compatibility test passed" << std::endl;
}

void testCheckpointFileIntegrity() {
    std::cout << "    Testing checkpoint file integrity..." << std::endl;
    
    // Create a valid checkpoint file
    std::string validFile = "test_valid_checkpoint.nchk";
    std::ofstream outFile(validFile, std::ios::binary);
    if (outFile) {
        // Write a simple valid header
        outFile.write("NLM_CHECKPOINT_V1", 16);
        outFile.write("valid data", 11);
        outFile.close();
        
        auto result = nlm::CheckpointValidator::validateFileIntegrity(validFile);
        assert(result.valid == true);
        assert(result.structureValid == true);
        
        std::cout << "      ✓ Valid checkpoint file integrity test passed" << std::endl;
        
        // Clean up
        std::remove(validFile.c_str());
    }
    
    // Create an invalid (empty) checkpoint file
    std::string emptyFile = "test_empty_checkpoint.nchk";
    std::ofstream emptyOut(emptyFile, std::ios::binary);
    emptyOut.close();
    
    auto emptyResult = nlm::CheckpointValidator::validateFileIntegrity(emptyFile);
    assert(emptyResult.valid == false);
    assert(emptyResult.errors.size() > 0);
    
    std::cout << "      ✓ Invalid checkpoint file integrity test passed" << std::endl;
    
    // Clean up
    std::remove(emptyFile.c_str());
}

void testNeuralDataConsistency() {
    std::cout << "    Testing neural data consistency..." << std::endl;
    
    nlm::CheckpointValidator::CheckpointMetadata metadata;
    metadata.neuronCount = 1000;
    metadata.synapseCount = 10000;
    metadata.regionCount = 10;
    metadata.averageFiringRate = 5.0;
    metadata.averageSynapticWeight = 0.5;
    
    auto result = nlm::CheckpointValidator::validateNeuralDataConsistency(metadata);
    assert(result.valid == true);
    assert(result.dataConsistency == true);
    
    // Test with unrealistic values
    metadata.averageFiringRate = 1000.0; // Unrealistic
    metadata.averageSynapticWeight = 50.0; // Unrealistic
    
    result = nlm::CheckpointValidator::validateNeuralDataConsistency(metadata);
    assert(result.valid == true); // Still valid, just warnings
    assert(!result.warnings.empty());
    
    std::cout << "      ✓ Neural data consistency test passed" << std::endl;
}

void testCompleteCheckpointValidation() {
    std::cout << "    Testing complete checkpoint validation..." << std::endl;
    
    // Create a test checkpoint file
    std::string testFile = "test_checkpoint.nchk";
    std::ofstream outFile(testFile, std::ios::binary);
    if (outFile) {
        outFile.write("NLM_CHECKPOINT_V1", 16);
        outFile.write("test data for checkpoint", 23);
        outFile.close();
        
        auto result = nlm::CheckpointValidator::validateCheckpoint(testFile);
        assert(result.valid == true);
        assert(result.versionCompatible == true);
        assert(result.structureValid == true);
        assert(result.dataConsistency == true);
        
        std::cout << "      ✓ Complete checkpoint validation test passed" << std::endl;
        
        // Clean up
        std::remove(testFile.c_str());
    }
}

void testCheckpointRecovery() {
    std::cout << "    Testing checkpoint recovery..." << std::endl;
    
    // Create a test checkpoint file
    std::string testFile = "test_recovery_checkpoint.nchk";
    std::ofstream outFile(testFile, std::ios::binary);
    if (outFile) {
        outFile.write("NLM_CHECKPOINT_V1", 16);
        outFile.write("data for recovery test", 23);
        outFile.close();
        
        auto result = nlm::CheckpointValidator::recoverCheckpoint(testFile);
        assert(result.valid == true);
        
        std::cout << "      ✓ Checkpoint recovery test passed" << std::endl;
        
        // Clean up
        std::remove(testFile.c_str());
    }
}

void testCheckpointBatchValidation() {
    std::cout << "    Testing checkpoint batch validation..." << std::endl;
    
    // Create multiple test checkpoint files
    std::vector<std::string> testFiles;
    for (int i = 0; i < 3; ++i) {
        std::string testFile = "test_batch_checkpoint_" + std::to_string(i) + ".nchk";
        std::ofstream outFile(testFile, std::ios::binary);
        if (outFile) {
            outFile.write("NLM_CHECKPOINT_V1", 16);
            outFile.write(("data for batch " + std::to_string(i)).c_str(), 22);
            outFile.close();
            testFiles.push_back(testFile);
        }
    }
    
    auto results = nlm::CheckpointValidation::validateBatch(testFiles);
    assert(results.size() == 3);
    
    std::string summary = nlm::CheckpointValidation::getValidationSummary(results);
    assert(summary.find("Checkpoint Validation Batch Summary") != std::string::npos);
    
    std::cout << "      ✓ Checkpoint batch validation test passed" << std::endl;
    
    // Clean up
    for (const auto& testFile : testFiles) {
        std::remove(testFile.c_str());
    }
}

void runAll() {
    std::cout << "\n=== Running Checkpoint Validation Tests ===\n" << std::endl;
    
    testCheckpointMetadataValidation();
    testCheckpointVersionCompatibility();
    testCheckpointFileIntegrity();
    testNeuralDataConsistency();
    testCompleteCheckpointValidation();
    testCheckpointRecovery();
    testCheckpointBatchValidation();
    
    std::cout << "\n=== All Checkpoint Validation Tests Passed! ===\n" << std::endl;
}

} // namespace test_checkpoint