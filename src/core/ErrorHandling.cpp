// ErrorHandling.cpp - Implementation of the error handling framework

#include "ErrorHandling.hpp"
#include "Logger/Logger.hpp"
#include <fstream>
#include <cstring>
#include <algorithm>
#include <sstream>

namespace nlm {

// Static member initialization
ErrorHandler::ErrorCallback ErrorHandler::errorCallback_ = nullptr;

// File operations implementation
std::string ErrorHandler::readFile(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::in | std::ios::binary);
    if (!file) {
        throw NLMSystemError(ErrorCode::System_FileNotFound,
                           "Cannot open file: " + filepath);
    }
    
    // Get file size
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    // Read entire file
    std::string content;
    content.resize(fileSize);
    if (!file.read(&content[0], fileSize)) {
        throw NLMSystemError(ErrorCode::System_IOError,
                           "Failed to read file: " + filepath);
    }
    
    return content;
}

bool ErrorHandler::writeFile(const std::string& filepath, const std::string& content) {
    try {
        std::ofstream file(filepath, std::ios::out | std::ios::binary);
        if (!file) {
            throw NLMSystemError(ErrorCode::System_PermissionDenied,
                               "Cannot write to file: " + filepath);
        }
        
        file.write(content.c_str(), content.size());
        if (!file) {
            throw NLMSystemError(ErrorCode::System_IOError,
                               "Failed to write to file: " + filepath);
        }
        
        file.flush();
        if (!file) {
            throw NLMSystemError(ErrorCode::System_IOError,
                               "Failed to flush file: " + filepath);
        }
        
        return true;
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        logError(ErrorCode::System_IOError,
                "File write operation failed: " + std::string(e.what()));
        return false;
    }
}

bool ErrorHandler::fileExists(const std::string& filepath) {
    std::ifstream file(filepath);
    return file.good();
}

// Error reporting implementation
std::string ErrorReporter::generateReport(const NLMError& error) {
    std::stringstream ss;
    ss << "=== NLM Error Report ===\n";
    ss << "Type: " << error.type() << "\n";
    ss << "Code: " << static_cast<int>(error.code()) << "\n";
    ss << "Message: " << error.what() << "\n";
    
    if (!error.file().empty()) {
        ss << "Location: " << error.file();
        if (error.line() > 0) ss << ":" << error.line();
        ss << "\n";
    }
    
    if (!error.function().empty()) {
        ss << "Function: " << error.function() << "\n";
    }
    
    auto timestamp = std::chrono::system_clock::to_time_t(error.timestamp_);
    ss << "Timestamp: " << std::ctime(&timestamp);
    
    const auto& context = error.getContext();
    if (!context.empty()) {
        ss << "Context:\n";
        for (const auto& pair : context) {
            ss << "  " << pair.first << ": " << pair.second << "\n";
        }
    }
    
    return ss.str();
}

std::string ErrorReporter::generateSystemSummary() {
    // This would typically collect and summarize all errors in the system
    return "=== System Error Summary (Feature not yet implemented) ===";
}

bool ErrorReporter::exportErrors(const std::string& filepath,
                                 const std::vector<std::reference_wrapper<const NLMError>>& errors) {
    try {
        std::ofstream file(filepath);
        if (!file) {
            throw NLMSystemError(ErrorCode::System_PermissionDenied,
                               "Cannot write error report to: " + filepath);
        }
        
        file << "=== NLM Error Log ===\n";
        file << "Total errors: " << errors.size() << "\n\n";
        
        for (size_t i = 0; i < errors.size(); ++i) {
            file << "Error " << i + 1 << ":\n";
            file << errors[i].get().what() << "\n\n";
        }
        
        file.flush();
        return file.good();
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        // Log the error but don't throw from export function
        return false;
    }
}

std::string ErrorReporter::extractErrorContext(const std::exception& e) {
    std::stringstream ss;
    ss << "Exception type: " << typeid(e).name() << "\n";
    ss << "Exception message: " << e.what() << "\n";
    
    try {
        // Try to extract nested exceptions if any
        // This is a simplified implementation
        ss << "Context: Standard C++ exception (details available in what())";
    } catch (...) {
        ss << "Context: Unable to extract detailed context";
    }
    
    return ss.str();
}

// Error compatibility implementation
ErrorCode ErrorCompatibility::mapException(const std::exception& e) {
    const std::string what = e.what();n
    if (what.find("bad_variant_access") != std::string::npos) {
        return ErrorCode::Component_FormatError;
    } else if (what.find("invalid_argument") != std::string::npos) {
        return ErrorCode::Component_InvalidParameter;
    } else if (what.find("runtime_error") != std::string::npos) {
        return ErrorCode::System_IOError;
    } else if (what.find("out_of_range") != std::string::npos) {
        return ErrorCode::Component_OutOfRange;
    } else if (what.find("length_error") != std::string::npos) {
        return ErrorCode::System_OutOfMemory;
    }
    
    return ErrorCode::UnknownError;
}

ErrorCode ErrorCompatibility::getFileErrorCode(int errnum) {
    switch (errnum) {
        case ENOENT: return ErrorCode::System_FileNotFound;
        case EACCES: return ErrorCode::System_PermissionDenied;
        case EIO: return ErrorCode::System_IOError;
        case ENOSPC: return ErrorCode::System_MemoryAllocationFailed;
        case EINVAL: return ErrorCode::Component_InvalidParameter;
        default: return ErrorCode::System_IOError;
    }
}

ErrorCode ErrorCompatibility::getMemoryErrorCode(int errnum) {
    switch (errnum) {
        case ENOMEM: return ErrorCode::System_MemoryAllocationFailed;
        case EOVERFLOW: return ErrorCode::System_OutOfMemory;
        default: return ErrorCode::System_MemoryAllocationFailed;
    }
}

std::unique_ptr<NLMError> ErrorCompatibility::toNLMError(const std::exception& e) {
    ErrorCode code = mapException(e);
    return std::make_unique<NLMError>(code, e.what());
}

} // namespace nlm