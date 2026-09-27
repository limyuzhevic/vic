// CheckpointTransaction.cpp

#include "src/performance/CheckpointTransaction.hpp"
#include <stdexcept>

namespace nlm {

CheckpointTransaction::CheckpointTransaction(const std::string& filepath, 
                                             CompressionLevel compression)
    : isActive_(false), targetFilePath_(filepath), state_(std::make_unique<TransactionState>()) {
    // Set compression level
    state_->compression = compression;
}

CheckpointTransaction::~CheckpointTransaction() {
    if (isActive_) {
        abort();
    }
}

bool CheckpointTransaction::begin() {
    if (isActive_) {
        return false; // Already active
    }
    
    // Check if target file already exists
    if (std::filesystem::exists(targetFilePath_)) {
        return false; // File already exists, cannot start new transaction
    }
    
    try {
        // Generate temporary file path
        tempFilePath_ = generateTempFilename(targetFilePath_);
        
        // Ensure target directory exists
        std::filesystem::path dir = std::filesystem::path(targetFilePath_).parent_path();
        if (!dir.empty() && !std::filesystem::exists(dir)) {
            if (!std::filesystem::create_directories(dir)) {
                throw Exception("Failed to create checkpoint directory: " + dir.string(),
                               Exception::ErrorCode::IO_ERROR);
            }
        }
        
        // Open temporary file for writing
        state_->stream.open(tempFilePath_, std::ios::binary | std::ios::trunc);
        if (!state_->stream.is_open()) {
            throw Exception("Failed to open temporary file: " + tempFilePath_,
                           Exception::ErrorCode::IO_ERROR);
        }
        
        state_->filepath = tempFilePath_;
        state_->isValid = true;
        isActive_ = true;
        
        return true;
    } catch (const Exception& e) {
        // Cleanup on exception
        abort();
        throw;
    } catch (const std::exception& e) {
        // Convert other exceptions to our exception type
        abort();
        throw Exception(std::string("Unexpected error starting checkpoint transaction: ") + e.what(),
                       Exception::ErrorCode::IO_ERROR);
    }
}

void CheckpointTransaction::abort() noexcept {
    if (!isActive_) {
        return;
    }
    
    try {
        // Close stream
        if (state_->stream.is_open()) {
            state_->stream.close();
        }
        
        // Remove temporary file if it exists
        cleanupTempFile();
        
        // Reset state
        isActive_ = false;
        state_->isValid = false;
        state_->errorMessage.clear();
    } catch (...) {
        // Ignore exceptions during cleanup
    }
}

bool CheckpointTransaction::commit() {
    if (!isActive_) {
        return false; // Not active
    }
    
    try {
        // Ensure stream is properly closed and flushed
        if (state_->stream.is_open()) {
            state_->stream.close();
            if (!state_->stream.good()) {
                throw Exception("Failed to properly close temporary file",
                               Exception::ErrorCode::IO_ERROR);
            }
        }
        
        // Atomic rename
        if (!renameToFinal()) {
            return false;
        }
        
        // Reset state
        isActive_ = false;
        return true;
    } catch (const Exception& e) {
        state_->errorMessage = e.what();
        abort();
        throw;
    } catch (const std::exception& e) {
        state_->errorMessage = std::string("Unexpected error committing transaction: ") + e.what();
        abort();
        throw Exception(state_->errorMessage, Exception::ErrorCode::IO_ERROR);
    }
}

bool CheckpointTransaction::writeToTempFile() {
    return false; // Placeholder for actual implementation
}

bool CheckpointTransaction::renameToFinal() {
    try {
        // Check if temporary file exists
        if (!fileExists(tempFilePath_)) {
            throw Exception("Temporary file does not exist for commit",
                           Exception::ErrorCode::IO_ERROR);
        }
        
        // Check if target file already exists (should be protected by begin())
        if (fileExists(targetFilePath_)) {
            throw Exception("Target file already exists, cannot overwrite",
                           Exception::ErrorCode::IO_ERROR);
        }
        
        // Atomic rename
        std::filesystem::rename(tempFilePath_, targetFilePath_);
        
        // Verify the final file exists and is not empty
        if (!fileExists(targetFilePath_) || std::filesystem::file_size(targetFilePath_) == 0) {
            throw Exception("Failed to create final checkpoint file",
                           Exception::ErrorCode::IO_ERROR);
        }
        
        return true;
    } catch (const std::exception& e) {
        throw Exception(std::string("Failed to rename checkpoint file: ") + e.what(),
                       Exception::ErrorCode::IO_ERROR);
    }
}

bool CheckpointTransaction::cleanupTempFile() noexcept {
    if (tempFilePath_.empty()) {
        return true;
    }
    
    try {
        return removeFile(tempFilePath_);
    } catch (...) {
        return false;
    }
}

std::string CheckpointTransaction::generateTempFilename(const std::string& original) const {
    std::filesystem::path path(original);
    return path.parent_path().string() + "/" + path.stem().string() + ".tmp" + path.extension().string();
}

bool CheckpointTransaction::fileExists(const std::string& path) const {
    return std::filesystem::exists(path);
}

bool CheckpointTransaction::removeFile(const std::string& path) const noexcept {
    try {
        if (fileExists(path)) {
            std::filesystem::remove(path);
        }
        return true;
    } catch (...) {
        return false;
    }
}

} // namespace nlm
