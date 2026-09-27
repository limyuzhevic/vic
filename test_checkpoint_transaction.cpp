#include "src/performance/CheckpointTransaction.hpp"
#include "core/Exception.hpp"
#include <fstream>
#include <filesystem>
#include <iostream>

void testAtomicFileOperation() {
    std::cout << "Testing atomic file operations..." << std::endl;
    
    const std::string testFile = "./test_checkpoint_1.bin";
    
    try {
        // Create a transaction
        nlm::CheckpointTransaction transaction(testFile);
        
        // Begin transaction
        if (!transaction.begin()) {
            throw nlm::NLMTestException("Transaction begin failed");
        }
        
        // Verify temporary file exists
        if (!transaction.isActive()) {
            throw nlm::NLMTestException("Transaction should be active after begin");
        }
        if (transaction.getTempFilePath().empty()) {
            throw nlm::NLMTestException("Temporary file path should not be empty");
        }
        if (transaction.getTargetFilePath().empty()) {
            throw nlm::NLMTestException("Target file path should not be empty");
        }
        
        // Write some data to the temporary file
        std::ofstream tempFile(transaction.getTempFilePath(), std::ios::binary | std::ios::app);
        if (!tempFile.is_open()) {
            throw nlm::NLMTestException("Failed to open temporary file for writing");
        }
        tempFile.write("test data", 9);
        tempFile.close();
        
        // Verify temporary file exists and has data
        if (!transaction.isActive()) {
            throw nlm::NLMTestException("Transaction should remain active after write");
        }
        
        // Commit transaction
        if (!transaction.commit()) {
            throw nlm::NLMTestException("Transaction commit failed");
        }
        
        // Verify final file exists
        if (!std::filesystem::exists(testFile)) {
            throw nlm::NLMTestException("Final checkpoint file should exist after commit");
        }
        if (transaction.isActive()) {
            throw nlm::NLMTestException("Transaction should not be active after commit");
        }
        
        // Verify data was written correctly
        std::ifstream finalFile(testFile, std::ios::binary);
        if (!finalFile.is_open()) {
            throw nlm::NLMTestException("Failed to open final file for reading");
        }
        
        char buffer[10];
        finalFile.read(buffer, 9);
        buffer[9] = '\0';
        if (std::string(buffer) != "test data") {
            throw nlm::NLMTestException("Data in final file doesn't match: expected 'test data', got '" + 
                                       std::string(buffer) + "'");
        }
        
        finalFile.close();
        
        // Cleanup
        std::filesystem::remove(testFile);
        
        std::cout << "✓ Atomic file operation test passed" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "✗ Atomic file operation test failed: " << e.what() << std::endl;
        throw;
    }
}

void testTransactionAbort() {
    std::cout << "Testing transaction abort..." << std::endl;
    
    const std::string testFile = "./test_checkpoint_2.bin";
    
    try {
        // Create a transaction
        nlm::CheckpointTransaction transaction(testFile);
        
        // Begin transaction
        if (!transaction.begin()) {
            throw nlm::NLMTestException("Transaction begin failed in abort test");
        }
        
        // Write some data
        std::ofstream tempFile(transaction.getTempFilePath(), std::ios::binary | std::ios::app);
        tempFile.write("abort test", 10);
        tempFile.close();
        
        // Abort transaction
        transaction.abort();
        
        // Verify temporary file was cleaned up
        if (transaction.isActive()) {
            throw nlm::NLMTestException("Transaction should not be active after abort");
        }
        if (std::filesystem::exists(testFile)) {
            throw nlm::NLMTestException("Test file should not exist after abort");
        }
        if (std::filesystem::exists(transaction.getTempFilePath())) {
            throw nlm::NLMTestException("Temporary file should not exist after abort");
        }
        
        std::cout << "✓ Transaction abort test passed" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "✗ Transaction abort test failed: " << e.what() << std::endl;
        throw;
    }
}

void testCheckpointTransactionException() {
    std::cout << "Testing checkpoint transaction exceptions..." << std::endl;
    
    try {
        // Test exception types
        nlm::CheckpointTransaction::Exception ex1("Test error");
        if (ex1.getErrorCode() != nlm::CheckpointTransaction::Exception::ErrorCode::IO_ERROR) {
            throw nlm::NLMTestException("CheckpointTransaction exception test failed: ex1 should have IO_ERROR code");
        }
        if (std::string(ex1.what()) != "Test error") {
            throw nlm::NLMTestException("CheckpointTransaction exception test failed: ex1 message should be 'Test error', got '" + 
                                       std::string(ex1.what()) + "'");
        }
        
        nlm::CheckpointTransaction::Exception ex2("Test validation error", 
                                                 nlm::CheckpointTransaction::Exception::ErrorCode::VALIDATION_ERROR);
        if (ex2.getErrorCode() != nlm::CheckpointTransaction::Exception::ErrorCode::VALIDATION_ERROR) {
            throw nlm::NLMTestException("CheckpointTransaction exception test failed: ex2 should have VALIDATION_ERROR code");
        }
        
        std::cout << "✓ Checkpoint transaction exception test passed" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "✗ Checkpoint transaction exception test failed: " << e.what() << std::endl;
        throw;
    }
}

int main() {
    std::cout << "Running transaction-based checkpointing tests..." << std::endl;
    std::cout << "===============================================" << std::endl;
    
    try {
        testAtomicFileOperation();
        testTransactionAbort();
        testCheckpointTransactionException();
        
        std::cout << "\n===============================================" << std::endl;
        std::cout << "All tests passed! ✓" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cout << "\nTest suite failed! ✗" << std::endl;
        std::cout << "Error: " << e.what() << std::endl;
        return 1;
    }
}