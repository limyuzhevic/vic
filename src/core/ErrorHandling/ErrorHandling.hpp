#pragma once

#include <string>
#include <memory>
#include <system_error>
#include <format>
#include "Logger.hpp"

namespace nlm {

// Error codes for NLM operations
enum class ErrorCode : int {
    // Success
    Success = 0,
    
    // General errors
    Unknown = 1,
    InvalidArgument = 2,
    InvalidState = 3,
    NotFound = 4,
    PermissionDenied = 5,
    Timeout = 6,
    
    // File I/O errors
    FileNotFound = 100,
    FileOpenFailed,
    FileReadFailed,
    FileWriteFailed,
    FileInvalidFormat,
    
    // Brain errors
    BrainNotInitialized = 200,
    BrainInvalidConfiguration,
    BrainSaveFailed,
    BrainLoadFailed,
    BrainRegionNotFound,
    BrainInvalidRegion,
    
    // Memory errors
    MemoryAllocationFailed,
    MemoryCorruption,
    
    // Validation errors
    ValidationFailed,
    ValidationOutOfRange,
    ValidationNullPointer,
    
    // Checkpoint errors
    CheckpointInvalid,
    CheckpointCorruption,
    CheckpointWriteFailed,
    CheckpointReadFailed
};

// Exception class for NLM errors
class NLMError : public std::runtime_error {
public:
    NLMError(ErrorCode code, const std::string& message)
        : std::runtime_error(formatError(code, message)),
          code_(code), message_(message) {}
    
    ErrorCode code() const noexcept { return code_; }
    const std::string& message() const noexcept { return message_; }
    
    static std::string formatError(ErrorCode code, const std::string& message) {
        return "NLM Error [" + std::to_string(static_cast<int>(code)) + "]: " + message;
    }
    
private:
    ErrorCode code_;
    std::string message_;
};

// Error reporting utilities
class ErrorReporter {
public:
    static void reportError(ErrorCode code, const std::string& message,
                           const std::string& file = "", int line = 0,
                           const std::string& function = "") {
        std::string fullMessage = std::format("{}", message);
        if (!file.empty()) {
            fullMessage += " at " + file;
            if (line > 0) {
                fullMessage += ":" + std::to_string(line);
            }
        }
        
        Logger::getGlobal().error(fullMessage);
        
        // Also throw exception for critical errors
        if (code >= ErrorCode::FileOpenFailed && code <= ErrorCode::CheckpointReadFailed) {
            throw NLMError(code, message);
        }
    }
    
    static bool checkCondition(bool condition, ErrorCode code, const std::string& message,
                              const std::string& file = "", int line = 0,
                              const std::string& function = "") {
        if (!condition) {
            reportError(code, message, file, line, function);
            return false;
        }
        return true;
    }
    
    static bool checkNullptr(const void* ptr, ErrorCode code, const std::string& message,
                           const std::string& file = "", int line = 0,
                           const std::string& function = "") {
        if (ptr == nullptr) {
            reportError(code, message, file, line, function);
            return false;
        }
        return true;
    }
    
    static bool checkBounds(size_t index, size_t size, ErrorCode code,
                          const std::string& message = "Index out of bounds",
                          const std::string& file = "", int line = 0,
                          const std::string& function = "") {
        if (index >= size) {
            reportError(code, message + ": " + std::to_string(index) + " >= " + std::to_string(size),
                       file, line, function);
            return false;
        }
        return true;
    }
};

// Helper macros for error checking
#define NLM_CHECK(cond, code, msg) do { \
    if (!(cond)) { \
        NLM::ErrorReporter::reportError(code, msg, __FILE__, __LINE__, __func__); \
        return false; \
    } \
} while(0)

#define NLM_CHECK_NULL(ptr, code, msg) do { \
    if (!(ptr)) { \
        NLM::ErrorReporter::reportError(code, msg, __FILE__, __LINE__, __func__); \
        return false; \
    } \
} while(0)

#define NLM_CHECK_BOUNDS(index, size, code, msg) do { \
    if ((index) >= (size)) { \
        NLM::ErrorReporter::reportError(code, msg + ": " + std::to_string(index) + " >= " + std::to_string(size), __FILE__, __LINE__, __func__); \
        return false; \
    } \
} while(0)

// Result type for operations that can fail
template<typename T>
class Result {
public:
    Result(T value) : value_(std::make_unique<Storage<T>>(value)), success_(true) {}
    Result(ErrorCode code, const std::string& message) 
        : error_code_(code), error_message_(message), success_(false) {}
    
    bool success() const noexcept { return success_; }
    ErrorCode errorCode() const noexcept { return error_code_; }
    const std::string& errorMessage() const noexcept { return error_message_; }
    
    const T& value() const & { return *value_; }
    T& value() & { return *value_; }
    const T&& value() const && { return std::move(*value_); }
    T&& value() && { return std::move(*value_); }
    
private:
    struct BaseStorage {
        virtual ~BaseStorage() = default;
    };
    
    template<typename U>
    struct Storage : BaseStorage {
        Storage(U val) : value(std::move(val)) {}
        U value;
    };
    
    bool success_;
    ErrorCode error_code_;
    std::string error_message_;
    std::unique_ptr<BaseStorage> value_;
};

// Utility functions for common operations
inline std::string toString(ErrorCode code) {
    switch (code) {
        case ErrorCode::Success: return "Success";
        case ErrorCode::Unknown: return "Unknown";
        case ErrorCode::InvalidArgument: return "InvalidArgument";
        case ErrorCode::InvalidState: return "InvalidState";
        case ErrorCode::NotFound: return "NotFound";
        case ErrorCode::PermissionDenied: return "PermissionDenied";
        case ErrorCode::Timeout: return "Timeout";
        case ErrorCode::FileNotFound: return "FileNotFound";
        case ErrorCode::FileOpenFailed: return "FileOpenFailed";
        case ErrorCode::FileReadFailed: return "FileReadFailed";
        case ErrorCode::FileWriteFailed: return "FileWriteFailed";
        case ErrorCode::FileInvalidFormat: return "FileInvalidFormat";
        case ErrorCode::BrainNotInitialized: return "BrainNotInitialized";
        case ErrorCode::BrainInvalidConfiguration: return "BrainInvalidConfiguration";
        case ErrorCode::BrainSaveFailed: return "BrainSaveFailed";
        case ErrorCode::BrainLoadFailed: return "BrainLoadFailed";
        case ErrorCode::BrainRegionNotFound: return "BrainRegionNotFound";
        case ErrorCode::BrainInvalidRegion: return "BrainInvalidRegion";
        case ErrorCode::MemoryAllocationFailed: return "MemoryAllocationFailed";
        case ErrorCode::MemoryCorruption: return "MemoryCorruption";
        case ErrorCode::ValidationFailed: return "ValidationFailed";
        case ErrorCode::ValidationOutOfRange: return "ValidationOutOfRange";
        case ErrorCode::ValidationNullPointer: return "ValidationNullPointer";
        case ErrorCode::CheckpointInvalid: return "CheckpointInvalid";
        case ErrorCode::CheckpointCorruption: return "CheckpointCorruption";
        case ErrorCode::CheckpointWriteFailed: return "CheckpointWriteFailed";
        case ErrorCode::CheckpointReadFailed: return "CheckpointReadFailed";
        default: return "Unknown";
    }
}

} // namespace nlm