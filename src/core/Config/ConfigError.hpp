#pragma once

#include <string>
#include <stdexcept>
#include <chrono>
#include <sstream>

namespace nlm {

class ConfigError : public std::runtime_error {
public:
    ConfigError(const std::string& message) : std::runtime_error(message) {}
    ConfigError(const std::string& message, const std::string& file, size_t line) 
        : std::runtime_error(formatDetailedMessage(message, file, line)) {}
    ConfigError(const std::string& message, const std::string& file, size_t line, 
                const std::string& context)
        : std::runtime_error(formatDetailedMessage(message, file, line, context)) {}
    
    std::string getDetailedMessage() const { return what(); }
    static std::string formatDetailedMessage(const std::string& message, 
                                             const std::string& file, 
                                             size_t line,
                                             const std::string& context = "");
};

class ConfigSchemaError : public ConfigError {
public:
    ConfigSchemaError(const std::string& message, const std::string& file, size_t line)
        : ConfigError(message, file, line) {}
};

class ConfigValidationError : public ConfigError {
public:
    ConfigValidationError(const std::string& message, const std::string& file, size_t line,
                          const std::string& key)
        : ConfigError(message, file, line, "Key: " + key), key_(key) {}
    
    std::string getKey() const { return key_; }
private:
    std::string key_;
};

class ConfigFileError : public ConfigError {
public:
    ConfigFileError(const std::string& message, const std::string& file, size_t line,
                   const std::string& filepath)
        : ConfigError(message, file, line, "File: " + filepath), filepath_(filepath) {}
    
    std::string getFilePath() const { return filepath_; }
private:
    std::string filepath_;
};

class ConfigPermissionError : public ConfigError {
public:
    ConfigPermissionError(const std::string& message, const std::string& file, size_t line,
                         const std::string& filepath, const std::string& operation)
        : ConfigError(message, file, line, "File: " + filepath + ", Operation: " + operation),
          filepath_(filepath), operation_(operation) {}
    
    std::string getFilePath() const { return filepath_; }
    std::string getOperation() const { return operation_; }
private:
    std::string filepath_;
    std::string operation_;
};

} // namespace nlm