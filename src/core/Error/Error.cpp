#include "Error.hpp"
#include <stdexcept>
#include <cerrno>

namespace nlm {

// ErrorReporter implementation
ErrorReporter::ErrorReporter() : errors() {}

// Factory methods for common errors
NLMError ErrorReporter::nullPointerError(const std::string& name, 
                                         const std::string& file, 
                                         int line, 
                                         const std::string& function) {
    return NLMError(ErrorCode::NullPointer, 
                   "Null pointer access to: " + name, 
                   file, line, function);
}

NLMError ErrorReporter::fileNotFoundError(const std::string& path,
                                          const std::string& file,
                                          int line,
                                          const std::string& function) {
    return NLMError(ErrorCode::FileNotFound, 
                   "File not found: " + path,
                   file, line, function);
}

NLMError ErrorReporter::fileIOError(const std::string& path,
                                    const std::string& operation,
                                    const std::string& file,
                                    int line,
                                    const std::string& function) {
    return NLMError(ErrorCode::FileIOError,
                   "File I/O error during " + operation + 
                   " on: " + path,
                   file, line, function);
}

NLMError ErrorReporter::memoryAllocationError(const std::string& object,
                                               const std::string& file,
                                               int line,
                                               const std::string& function) {
    return NLMError(ErrorCode::MemoryAllocationFailed,
                   "Memory allocation failed for: " + object,
                   file, line, function);
}

NLMError ErrorReporter::validationError(const std::string& field,
                                        const std::string& value,
                                        const std::string& file,
                                        int line,
                                        const std::string& function) {
    return NLMError(ErrorCode::ValidationFailed,
                   "Validation failed for " + field + ": " + value,
                   file, line, function);
}

} // namespace nlm