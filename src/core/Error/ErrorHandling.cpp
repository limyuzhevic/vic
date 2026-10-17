#include "core/Error/ErrorHandling.hpp"
#include <system_error>
#include <filesystem>
#include <cerrno>

namespace nlm {
namespace ErrorHandling {
    
    void checkCondition(bool condition, const std::string& message,
                       const std::string& field,
                       ValidationError::ErrorType type,
                       const std::string& file,
                       int line,
                       const std::string& function) {
        if (!condition) {
            throw ValidationError(message, field, type, file, line, function);
        }
    }
    
    FileError convertFileError(const std::string& operation,
                              const std::string& filepath,
                              const std::string& message,
                              FileError::ErrorType type,
                              const std::string& file,
                              int line,
                              const std::string& function) {
        std::string fullMessage = "File error during " + operation + ": " + message;
        return FileError(fullMessage, filepath, type, file, line, function);
    }
    
    ConfigError convertConfigError(const std::string& key,
                                  const std::string& message,
                                  const std::string& file,
                                  int line,
                                  const std::string& function) {
        return ConfigError(message, key, file, line, function);
    }
    
    InitializationError convertInitializationError(const std::string& component,
                                                    const std::string& message,
                                                    const std::string& file,
                                                    int line,
                                                    const std::string& function) {
        return InitializationError(message, component, file, line, function);
    }
    
} // namespace ErrorHandling
} // namespace nlm