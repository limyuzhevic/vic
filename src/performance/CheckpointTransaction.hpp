#pragma once

#include <string>
#include <fstream>
#include <memory>
#include <vector>
#include <exception>
#include <filesystem>
#include "src/performance/CheckpointSystem.hpp"

namespace nlm {

class CheckpointTransaction {
public:
    CheckpointTransaction(const std::string& filepath, 
                         CompressionLevel compression = CompressionLevel::Balanced);
    ~CheckpointTransaction();
    
    // Disable copying and moving
    CheckpointTransaction(const CheckpointTransaction&) = delete;
    CheckpointTransaction& operator=(const CheckpointTransaction&) = delete;
    CheckpointTransaction(CheckpointTransaction&&) = delete;
    CheckpointTransaction& operator=(CheckpointTransaction&&) = delete;
    
    // Start the transaction
    bool begin();
    
    // Abort the transaction (cleanup)
    void abort() noexcept;
    
    // Commit the transaction (atomic rename)
    bool commit();
    
    // Check if transaction is active
    bool isActive() const { return isActive_; }
    
    // Get the temporary file path
    const std::string& getTempFilePath() const { return tempFilePath_; }
    
    // Get the target file path
    const std::string& getTargetFilePath() const { return targetFilePath_; }
    
    // Exception safety
    class Exception : public std::runtime_error {
    public:
        enum class ErrorCode {
            IO_ERROR = 1,
            VALIDATION_ERROR = 2,
            ENCODING_ERROR = 3,
            RESOURCE_ERROR = 4,
            CHECKSUM_ERROR = 5
        };
        
        Exception(const std::string& message, ErrorCode code = ErrorCode::IO_ERROR)
            : std::runtime_error(message), errorCode_(code) {}
        
        ErrorCode getErrorCode() const { return errorCode_; }
        
    private:
        ErrorCode errorCode_;
    };
    
    // Transaction state
    struct TransactionState {
        std::ofstream stream;
        std::string filepath;
        CompressionLevel compression;
        CheckpointHeader header;
        std::vector<SectionHeader> sections;
        uint64_t bytesWritten;
        std::vector<uint8_t> compressBuffer;
        bool isValid;
        std::string errorMessage;
        
        TransactionState() : bytesWritten(0), isValid(false) {}
    };
    
private:
    bool isActive_;
    std::string targetFilePath_;
    std::string tempFilePath_;
    std::unique_ptr<TransactionState> state_;
    
    // Atomic file operations
    bool writeToTempFile();
    bool renameToFinal();
    bool cleanupTempFile() noexcept;
    
    // File system helpers
    std::string generateTempFilename(const std::string& original) const;
    bool fileExists(const std::string& path) const;
    bool removeFile(const std::string& path) const noexcept;
};

} // namespace nlm
