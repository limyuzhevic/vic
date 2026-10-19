// Advanced schema validation utilities for NLM project
// Enhanced version with comprehensive validation support

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <any>
#include <typeindex>
#include <sstream>
#include <stdexcept>
#include <regex>
#include <unordered_set>
#include <set>
#include <map>
#include <memory_resource>
#include <type_traits>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <future>

namespace nlm {

class Logger;

// Forward declarations for JSON/YAML libraries
namespace nlohmann {
    class json;
}

namespace YAML {
    class Node;
}

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

// Enhanced configuration error codes for detailed error reporting
enum class ConfigErrorCode {
    // File I/O errors
    FileNotFound,
    PermissionDenied,
    FileTooLarge,
    DiskFull,
    IOError,
    
    // Format errors
    InvalidFormat,
    UnsupportedFormat,
    MalformedContent,
    
    // Validation errors
    SchemaValidationFailed,
    TypeMismatch,
    RequiredKeyMissing,
    ValueOutOfRange,
    PatternMismatch,
    InvalidEnumValue,
    ConditionalValidationFailed,
    DependencyValidationFailed,
    CrossValidationFailed,
    FormatValidationFailed,
    BusinessLogicValidationFailed,
    
    // Operation errors
    OperationFailed,
    RetryExhausted,
    ResourceUnavailable,
    
    // Resource management errors
    FileHandleLeak,
    CleanupFailed,
    
    // Recovery errors
    BackupFailed,
    RollbackFailed,
    MemoryFallbackFailed,
    
    // General errors
    Unknown,
    MemoryAllocationFailed
};

// Enhanced exception class for configuration errors
class ConfigException {
public:
    ConfigException(ConfigErrorCode code, const std::string& message, 
                   const std::string& filepath = "", int line = 0,
                   int retryCount = 0)
        : code_(code), message_(message), filepath_(filepath), line_(line),
          retryCount_(retryCount), timestamp_(std::chrono::system_clock::now()) {}
    
    ConfigErrorCode code() const { return code_; }
    const std::string& message() const { return message_; }
    const std::string& filepath() const { return filepath_; }
    int line() const { return line_; }
    int retryCount() const { return retryCount_; }
    const std::chrono::system_clock::time_point& timestamp() const { return timestamp_; }
    
    std::string formattedMessage() const {
        std::stringstream ss;
        ss << "[Config] ";
        
        switch (code_) {
            case ConfigErrorCode::FileNotFound:
                ss << "FileNotFound: ";
                break;
            case ConfigErrorCode::PermissionDenied:
                ss << "PermissionDenied: ";
                break;
            case ConfigErrorCode::FileTooLarge:
                ss << "FileTooLarge: ";
                break;
            case ConfigErrorCode::DiskFull:
                ss << "DiskFull: ";
                break;
            case ConfigErrorCode::IOError:
                ss << "IOError: ";
                break;
            case ConfigErrorCode::InvalidFormat:
                ss << "InvalidFormat: ";
                break;
            case ConfigErrorCode::UnsupportedFormat:
                ss << "UnsupportedFormat: ";
                break;
            case ConfigErrorCode::MalformedContent:
                ss << "MalformedContent: ";
                break;
            case ConfigErrorCode::SchemaValidationFailed:
                ss << "SchemaValidationFailed: ";
                break;
            case ConfigErrorCode::TypeMismatch:
                ss << "TypeMismatch: ";
                break;
            case ConfigErrorCode::RequiredKeyMissing:
                ss << "RequiredKeyMissing: ";
                break;
            case ConfigErrorCode::ValueOutOfRange:
                ss << "ValueOutOfRange: ";
                break;
            case ConfigErrorCode::PatternMismatch:
                ss << "PatternMismatch: ";
                break;
            case ConfigErrorCode::InvalidEnumValue:
                ss << "InvalidEnumValue: ";
                break;
            case ConfigErrorCode::ConditionalValidationFailed:
                ss << "ConditionalValidationFailed: ";
                break;
            case ConfigErrorCode::DependencyValidationFailed:
                ss << "DependencyValidationFailed: ";
                break;
            case ConfigErrorCode::CrossValidationFailed:
                ss << "CrossValidationFailed: ";
                break;
            case ConfigErrorCode::FormatValidationFailed:
                ss << "FormatValidationFailed: ";
                break;
            case ConfigErrorCode::BusinessLogicValidationFailed:
                ss << "BusinessLogicValidationFailed: ";
                break;
            case ConfigErrorCode::OperationFailed:
                ss << "OperationFailed: ";
                break;
            case ConfigErrorCode::RetryExhausted:
                ss << "RetryExhausted: ";
                break;
            case ConfigErrorCode::ResourceUnavailable:
                ss << "ResourceUnavailable: ";
                break;
            case ConfigErrorCode::FileHandleLeak:
                ss << "FileHandleLeak: ";
                break;
            case ConfigErrorCode::CleanupFailed:
                ss << "CleanupFailed: ";
                break;
            case ConfigErrorCode::BackupFailed:
                ss << "BackupFailed: ";
                break;
            case ConfigErrorCode::RollbackFailed:
                ss << "RollbackFailed: ";
                break;
            case ConfigErrorCode::MemoryFallbackFailed:
                ss << "MemoryFallbackFailed: ";
                break;
            case ConfigErrorCode::Unknown:
                ss << "UnknownError: ";
                break;
            case ConfigErrorCode::MemoryAllocationFailed:
                ss << "MemoryAllocationFailed: ";
                break;
            default:
                ss << "ConfigError: ";
                break;
        }
        
        ss << message_;
        
        if (!filepath_.empty()) {
            ss << " (file: " << filepath_ << ")";
        }
        if (line_ > 0) {
            ss << " (line: " << line_ << ")";
        }
        if (retryCount_ > 0) {
            ss << " (retry: " << retryCount_ << ")";
        }
        
        return ss.str();
    }
    
    bool isRecoverable() const {
        switch (code_) {
            case ConfigErrorCode::FileNotFound:
            case ConfigErrorCode::PermissionDenied:
            case ConfigErrorCode::IOError:
            case ConfigErrorCode::RetryExhausted:
            case ConfigErrorCode::ResourceUnavailable:
                return true;
            default:
                return false;
        }
    }
    
    std::string errorCategory() const {
        switch (code_) {
            case ConfigErrorCode::FileNotFound:
            case ConfigErrorCode::PermissionDenied:
            case ConfigErrorCode::FileTooLarge:
            case ConfigErrorCode::DiskFull:
            case ConfigErrorCode::IOError:
                return "FileIO";
            case ConfigErrorCode::InvalidFormat:
            case ConfigErrorCode::UnsupportedFormat:
            case ConfigErrorCode::MalformedContent:
                return "Format";
            case ConfigErrorCode::SchemaValidationFailed:
            case ConfigErrorCode::TypeMismatch:
            case ConfigErrorCode::RequiredKeyMissing:
            case ConfigErrorCode::ValueOutOfRange:
            case ConfigErrorCode::PatternMismatch:
            case ConfigErrorCode::InvalidEnumValue:
            case ConfigErrorCode::ConditionalValidationFailed:
            case ConfigErrorCode::DependencyValidationFailed:
            case ConfigErrorCode::CrossValidationFailed:
            case ConfigErrorCode::FormatValidationFailed:
            case ConfigErrorCode::BusinessLogicValidationFailed:
                return "Validation";
            case ConfigErrorCode::OperationFailed:
            case ConfigErrorCode::RetryExhausted:
            case ConfigErrorCode::ResourceUnavailable:
                return "Operation";
            case ConfigErrorCode::FileHandleLeak:
            case ConfigErrorCode::CleanupFailed:
                return "ResourceManagement";
            case ConfigErrorCode::BackupFailed:
            case ConfigErrorCode::RollbackFailed:
                return "Recovery";
            case ConfigErrorCode::MemoryFallbackFailed:
                return "Fallback";
            case ConfigErrorCode::Unknown:
                return "General";
            case ConfigErrorCode::MemoryAllocationFailed:
                return "Memory";
            default:
                return "Unknown";
        }
    }
    
    std::string errorCodeName() const {
        switch (code_) {
            case ConfigErrorCode::FileNotFound: return "FileNotFound";
            case ConfigErrorCode::PermissionDenied: return "PermissionDenied";
            case ConfigErrorCode::FileTooLarge: return "FileTooLarge";
            case ConfigErrorCode::DiskFull: return "DiskFull";
            case ConfigErrorCode::IOError: return "IOError";
            case ConfigErrorCode::InvalidFormat: return "InvalidFormat";
            case ConfigErrorCode::UnsupportedFormat: return "UnsupportedFormat";
            case ConfigErrorCode::MalformedContent: return "MalformedContent";
            case ConfigErrorCode::SchemaValidationFailed: return "SchemaValidationFailed";
            case ConfigErrorCode::TypeMismatch: return "TypeMismatch";
            case ConfigErrorCode::RequiredKeyMissing: return "RequiredKeyMissing";
            case ConfigErrorCode::ValueOutOfRange: return "ValueOutOfRange";
            case ConfigErrorCode::PatternMismatch: return "PatternMismatch";
            case ConfigErrorCode::InvalidEnumValue: return "InvalidEnumValue";
            case ConfigErrorCode::ConditionalValidationFailed: return "ConditionalValidationFailed";
            case ConfigErrorCode::DependencyValidationFailed: return "DependencyValidationFailed";
            case ConfigErrorCode::CrossValidationFailed: return "CrossValidationFailed";
            case ConfigErrorCode::FormatValidationFailed: return "FormatValidationFailed";
            case ConfigErrorCode::BusinessLogicValidationFailed: return "BusinessLogicValidationFailed";
            case ConfigErrorCode::OperationFailed: return "OperationFailed";
            case ConfigErrorCode::RetryExhausted: return "RetryExhausted";
            case ConfigErrorCode::ResourceUnavailable: return "ResourceUnavailable";
            case ConfigErrorCode::FileHandleLeak: return "FileHandleLeak";
            case ConfigErrorCode::CleanupFailed: return "CleanupFailed";
            case ConfigErrorCode::BackupFailed: return "BackupFailed";
            case ConfigErrorCode::RollbackFailed: return "RollbackFailed";
            case ConfigErrorCode::MemoryFallbackFailed: return "MemoryFallbackFailed";
            case ConfigErrorCode::Unknown: return "Unknown";
            case ConfigErrorCode::MemoryAllocationFailed: return "MemoryAllocationFailed";
            default: return "Unknown";
        }
    }
    
private:
    ConfigErrorCode code_;
    std::string message_;
    std::string filepath_;
    int line_;
    int retryCount_;
    std::chrono::system_clock::time_point timestamp_;
};

// Result type for file operations with error details
struct ConfigFileResult {
    bool success;
    std::string message;
    ConfigErrorCode errorCode;
    std::string filepath;
    int line;
    int retryCount;
    
    ConfigFileResult(bool s = true, const std::string& msg = "", ConfigErrorCode ec = ConfigErrorCode::Unknown,
                     const std::string& path = "", int ln = 0, int retry = 0)
        : success(s), message(msg), errorCode(ec), filepath(path), line(ln), retryCount(retry) {}
    
    std::string formattedResult() const {
        std::stringstream ss;
        if (success) {
            ss << "[SUCCESS] " << message;
        } else {
            ss << "[FAILURE] ";
            ConfigException ex(errorCode, message, filepath, line, retryCount);
            ss << ex.formattedMessage();
        }
        return ss.str();
    }
};

// Detailed validation error information
struct ValidationError {
    std::string key;
    std::string expectedType;
    std::string actualType;
    std::string message;
    ConfigErrorCode errorCode;
    std::string category;
    std::string errorCodeName;
    std::string path;
    bool recoverable;
    std::chrono::system_clock::time_point timestamp;
    
    ValidationError() : errorCode(ConfigErrorCode::Unknown), recoverable(false), 
                       timestamp(std::chrono::system_clock::now()) {}
    
    ValidationError(const std::string& k, ConfigErrorCode code, const std::string& msg,
                   const std::string& expected = "", const std::string& actual = "")
        : key(k), errorCode(code), message(msg), expectedType(expected), 
          actualType(actual), recoverable(ConfigException(code, "").isRecoverable()) {
        category = ConfigException(code, "").errorCategory();
        errorCodeName = ConfigException(code, "").errorCodeName();
        timestamp = std::chrono::system_clock::now();
    }
    
    std::string formattedError() const {
        std::stringstream ss;
        ss << "[Validation Error] ";
        
        if (!key.empty()) {
            ss << "Key: \"" << key << "\" ";
        }
        
        ss << errorCodeName;
        
        if (!expectedType.empty() && !actualType.empty()) {
            ss << " (expected: " << expectedType << ", actual: " << actualType << ")";
        } else if (!message.empty()) {
            ss << " - " << message;
        }
        
        if (!path.empty()) {
            ss << " (path: " << path << ")";
        }
        
        return ss.str();
    }
    
    bool isCritical() const {
        return !recoverable && errorCode != ConfigErrorCode::Unknown;
    }
};

// Result type for validation operations
struct ValidationResult {
    bool valid;
    std::vector<ValidationError> errors;
    std::vector<std::string> warnings;
    std::string summary;
    double successRate;
    std::chrono::milliseconds validationTime;
    bool cacheable;
    std::string cacheKey;
    
    ValidationResult() : valid(true), successRate(100.0), validationTime(0ms), cacheable(true) {}
    
    bool isValid() const { return valid && errors.empty(); }
    
    void addError(const ValidationError& error) {
        errors.push_back(error);
        valid = false;
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    std::string formattedResult() const {
        std::stringstream ss;
        if (valid) {
            ss << "[VALID] All validation checks passed";
            if (!warnings.empty()) {
                ss << " (" << warnings.size() << " warning(s))";
            }
        } else {
            ss << "[INVALID] " << errors.size() << " validation error(s), " << warnings.size() << " warning(s)";
            
            // Include first few errors in summary
            int errorLimit = std::min(3, static_cast<int>(errors.size()));
            for (int i = 0; i < errorLimit; ++i) {
                ss << "\n  " << errors[i].formattedError();
            }
            
            if (errors.size() > errorLimit) {
                ss << "\n  ... and " << (errors.size() - errorLimit) << " more errors";
            }
        }
        
        if (!summary.empty()) {
            ss << "\n" << summary;
        }
        
        return ss.str();
    }
    
    double calculateSuccessRate() const {
        int totalChecks = errors.size() + warnings.size();
        if (totalChecks == 0) return 100.0;
        return (100.0 * (totalChecks - warnings.size())) / totalChecks;
    }
};

// Base class for all validators
class ValidatorBase {
public:
    virtual ~ValidatorBase() = default;
    
    virtual bool validate(const std::any& value) const = 0;
    virtual std::string getErrorMessage(const std::any& value) const = 0;
    virtual std::string getExpectedType() const = 0;
    virtual ConfigErrorCode getErrorCode() const = 0;
    
    virtual bool isRequired() const { return false; }
    virtual std::string getDescription() const { return ""; }
    
    virtual bool isAsync() const { return false; }
    virtual std::future<bool> validateAsync(const std::any& value) const { return std::async(std::launch::deferred, &ValidatorBase::validate, this, value); }
    
    virtual std::unique_ptr<ValidatorBase> clone() const = 0;
    
    static std::string typeIndexToString(std::type_index type) {
        if (type == typeid(int)) return "int";
        if (type == typeid(int64_t)) return "int64_t";
        if (type == typeid(double)) return "double";
        if (type == typeid(bool)) return "bool";
        if (type == typeid(std::string)) return "std::string";
        if (type == typeid(std::vector<int>)) return "std::vector<int>";
        if (type == typeid(std::vector<double>)) return "std::vector<double>";
        if (type == typeid(std::vector<std::string>)) return "std::vector<std::string>";
        return "unknown";
    }
};

// Range validator for numeric types
class RangeValidator : public ValidatorBase {
private:
    std::any minValue_;
    std::any maxValue_;
    std::string description_;
    bool inclusive_;
    
public:
    template<typename T>
    RangeValidator(T min, T max, const std::string& desc = "", bool inclusive = true)
        : minValue_(min), maxValue_(max), description_(desc), inclusive_(inclusive) {}
    
    bool validate(const std::any& value) const override {
        if constexpr (std::is_integral_v<std::any>) {
            // For integral types
            auto val = std::any_cast<int>(value);
            auto min = std::any_cast<int>(minValue_);
            auto max = std::any_cast<int>(maxValue_);
            return inclusive_ ? (val >= min && val <= max) : (val > min && val < max);
        } else {
            // For floating point types
            auto val = std::any_cast<double>(value);
            auto min = std::any_cast<double>(minValue_);
            auto max = std::any_cast<double>(maxValue_);
            return inclusive_ ? (val >= min && val <= max) : (val > min && val < max);
        }
    }
    
    std::string getErrorMessage(const std::any& value) const override {
        std::stringstream ss;
        ss << "Value out of range: ";
        
        if constexpr (std::is_integral_v<std::any>) {
            auto val = std::any_cast<int>(value);
            auto min = std::any_cast<int>(minValue_);
            auto max = std::any_cast<int>(maxValue_);
            ss << val << " is not in range [" << min << ", " << max << "]";
        } else {
            auto val = std::any_cast<double>(value);
            auto min = std::any_cast<double>(minValue_);
            auto max = std::any_cast<double>(maxValue_);
            ss << val << " is not in range [" << min << ", " << max << "]";
        }
        
        return ss.str();
    }
    
    std::string getExpectedType() const override {
        return "numeric range [" + std::to_string(std::any_cast<double>(minValue_)) + 
               ", " + std::to_string(std::any_cast<double>(maxValue_)) + "]";
    }
    
    ConfigErrorCode getErrorCode() const override {
        return ConfigErrorCode::ValueOutOfRange;
    }
    
    std::string getDescription() const override {
        return description_;
    }
    
    std::unique_ptr<ValidatorBase> clone() const override {
        return std::make_unique<RangeValidator>(*this);
    }
};

// Pattern validator for string patterns
class PatternValidator : public ValidatorBase {
private:
    std::regex pattern_;
    std::string description_;
    bool caseSensitive_;
    
public:
    PatternValidator(const std::string& pattern, const std::string& desc = "", bool caseSensitive = true)
        : pattern_(pattern), description_(desc), caseSensitive_(caseSensitive) {}
    
    bool validate(const std::any& value) const override {
        auto str = std::any_cast<std::string>(value);
        if (!caseSensitive_) {
            std::string lowerStr = str;
            std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
            std::string lowerPattern = std::string(pattern_.str());
            std::transform(lowerPattern.begin(), lowerPattern.end(), lowerPattern.begin(), ::tolower);
            std::regex ignoreCasePattern(lowerPattern);
            return std::regex_match(lowerStr, ignoreCasePattern);
        }
        return std::regex_match(str, pattern_);
    }
    
    std::string getErrorMessage(const std::any& value) const override {
        std::stringstream ss;
        auto str = std::any_cast<std::string>(value);
        ss << "String '" << str << "' does not match pattern: " << pattern_.str();
        return ss.str();
    }
    
    std::string getExpectedType() const override {
        return "string matching pattern: " + pattern_.str();
    }
    
    ConfigErrorCode getErrorCode() const override {
        return ConfigErrorCode::PatternMismatch;
    }
    
    std::string getDescription() const override {
        return description_;
    }
    
    std::unique_ptr<ValidatorBase> clone() const override {
        return std::make_unique<PatternValidator>(*this);
    }
};

// Enum validator for allowed values
class EnumValidator : public ValidatorBase {
private:
    std::set<std::string> allowedValues_;
    std::string description_;
    bool caseSensitive_;
    
public:
    EnumValidator(const std::vector<std::string>& values, const std::string& desc = "", bool caseSensitive = true)
        : allowedValues_(values.begin(), values.end()), description_(desc), caseSensitive_(caseSensitive) {}
    
    bool validate(const std::any& value) const override {
        auto str = std::any_cast<std::string>(value);
        if (!caseSensitive_) {
            std::string lowerStr = str;
            std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
            for (const auto& allowed : allowedValues_) {
                std::string lowerAllowed = allowed;
                std::transform(lowerAllowed.begin(), lowerAllowed.end(), lowerAllowed.begin(), ::tolower);
                if (lowerStr == lowerAllowed) return true;
            }
            return false;
        }
        return allowedValues_.find(str) != allowedValues_.end();
    }
    
    std::string getErrorMessage(const std::any& value) const override {
        std::stringstream ss;
        auto str = std::any_cast<std::string>(value);
        ss << "Value '" << str << "' is not in allowed enum values:";
        for (const auto& val : allowedValues_) {
            ss << " '" << val << "'";
        }
        return ss.str();
    }
    
    std::string getExpectedType() const override {
        std::stringstream ss;
        ss << "enum values: [";
        bool first = true;
        for (const auto& val : allowedValues_) {
            if (!first) ss << ", ";
            ss << "\"" << val << "\"";
            first = false;
        }
        ss << "]";
        return ss.str();
    }
    
    ConfigErrorCode getErrorCode() const override {
        return ConfigErrorCode::InvalidEnumValue;
    }
    
    std::string getDescription() const override {
        return description_;
    }
    
    std::unique_ptr<ValidatorBase> clone() const override {
        return std::make_unique<EnumValidator>(*this);
    }
};

// Conditional validator for if-then rules
class ConditionalValidator : public ValidatorBase {
private:
    std::string dependencyKey_;
    std::any dependencyValue_;
    std::unique_ptr<ValidatorBase> thenValidator_;
    std::string description_;
    
public:
    ConditionalValidator(const std::string& key, const std::any& value,
                         std::unique_ptr<ValidatorBase> thenValidator,
                         const std::string& desc = "")
        : dependencyKey_(key), dependencyValue_(value),
          thenValidator_(std::move(thenValidator)), description_(desc) {}
    
    bool validate(const std::any& value) const override {
        auto configMap = std::any_cast<std::unordered_map<std::string, std::any>>(value);
        
        auto it = configMap.find(dependencyKey_);
        if (it == configMap.end()) return true; // Dependency not present, no validation needed
        
        if (it->second != dependencyValue_) return true; // Dependency value doesn't match, no validation
        
        return thenValidator_->validate(configMap);
    }
    
    std::string getErrorMessage(const std::any& value) const override {
        std::stringstream ss;
        auto configMap = std::any_cast<std::unordered_map<std::string, std::any>>(value);
        
        auto it = configMap.find(dependencyKey_);
        if (it != configMap.end() && it->second == dependencyValue_) {
            ss << "Conditional validation failed: when \"" << dependencyKey_ << "\" = \"" 
               << std::any_cast<std::string>(dependencyValue_) << "\", thenValidator failed: "
               << thenValidator_->getErrorMessage(value);
        } else {
            ss << "Conditional dependency not met: \"" << dependencyKey_ << "\" != \"" 
               << std::any_cast<std::string>(dependencyValue_) << "\"";
        }
        return ss.str();
    }
    
    std::string getExpectedType() const override {
        return "conditional validation based on dependency \"" + dependencyKey_ + "\" = \"" 
               + std::any_cast<std::string>(dependencyValue_) + "\"";
    }
    
    ConfigErrorCode getErrorCode() const override {
        return ConfigErrorCode::ConditionalValidationFailed;
    }
    
    std::string getDescription() const override {
        return description_;
    }
    
    std::unique_ptr<ValidatorBase> clone() const override {
        return std::make_unique<ConditionalValidator>(
            dependencyKey_, dependencyValue_, thenValidator_->clone(), description_);
    }
};

// Composite validator for combining multiple validators
class CompositeValidator : public ValidatorBase {
private:
    std::vector<std::unique_ptr<ValidatorBase>> validators_;
    std::string description_;
    bool requireAll_;
    
public:
    CompositeValidator(const std::vector<std::unique_ptr<ValidatorBase>>& validators,
                       const std::string& desc = "", bool requireAll = true)
        : validators_(validators), description_(desc), requireAll_(requireAll) {}
    
    bool validate(const std::any& value) const override {
        bool allValid = true;
        for (const auto& validator : validators_) {
            if (!validator->validate(value)) {
                allValid = false;
                if (requireAll) break; // Fail fast if requiring all
            }
        }
        return allValid;
    }
    
    std::string getErrorMessage(const std::any& value) const override {
        std::stringstream ss;
        ss << "Composite validation failed:";
        for (const auto& validator : validators_) {
            if (!validator->validate(value)) {
                ss << " " << validator->getErrorMessage(value);
            }
        }
        return ss.str();
    }
    
    std::string getExpectedType() const override {
        return "composite validation of " + std::to_string(validators_.size()) + " validator(s)";
    }
    
    ConfigErrorCode getErrorCode() const override {
        return ConfigErrorCode::SchemaValidationFailed;
    }
    
    std::string getDescription() const override {
        return description_;
    }
    
    std::unique_ptr<ValidatorBase> clone() const override {
        std::vector<std::unique_ptr<ValidatorBase>> clonedValidators;
        clonedValidators.reserve(validators_.size());
        for (const auto& validator : validators_) {
            clonedValidators.push_back(validator->clone());
        }
        return std::make_unique<CompositeValidator>(clonedValidators, description_, requireAll_);
    }
};

// Format validator for standard formats (email, IP, etc.)
class FormatValidator : public ValidatorBase {
private:
    enum class FormatType {
        Email,
        IPv4,
        IPv6,
        MAC,
        URL,
        DateTime,
        Custom
    };
    
    FormatType formatType_;
    std::string customPattern_;
    std::string description_;
    
public:
    FormatValidator(FormatType type, const std::string& desc = "")
        : formatType_(type), description_(desc) {}
    
    FormatValidator(const std::string& pattern, const std::string& desc = "")
        : formatType_(FormatType::Custom), customPattern_(pattern), description_(desc) {}
    
    bool validate(const std::any& value) const override {
        auto str = std::any_cast<std::string>(value);
        
        switch (formatType_) {
            case FormatType::Email:
                // Simple email validation
                return std::regex_match(str, std::regex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)"));
            case FormatType::IPv4:
                return std::regex_match(str, std::regex(R"(^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$)"));
            case FormatType::IPv6:
                // Simplified IPv6 validation
                return std::regex_match(str, std::regex(R"(^([0-9a-fA-F]{1,4}:){7}[0-9a-fA-F]{1,4})$"));
            case FormatType::MAC:
                return std::regex_match(str, std::regex(R"(^([0-9A-Fa-f]{2}[:-]){5}[0-9A-Fa-f]{2})$"));
            case FormatType::URL:
                return std::regex_match(str, std::regex(R"(^(https?|ftp)://[^\s/$.?#].[^\s]*$)"));
            case FormatType::DateTime:
                // ISO 8601 format
                return std::regex_match(str, std::regex(R"(^\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}(?:\.\d+)?(?:Z|[+-]\d{2}:\d{2})?)$"));
            case FormatType::Custom:
                return std::regex_match(str, std::regex(customPattern_));
            default:
                return false;
        }
    }
    
    std::string getErrorMessage(const std::any& value) const override {
        auto str = std::any_cast<std::string>(value);
        std::stringstream ss;
        
        switch (formatType_) {
            case FormatType::Email: ss << "Invalid email format: '" << str << "'";
                break;
            case FormatType::IPv4: ss << "Invalid IPv4 address: '" << str << "'";
                break;
            case FormatType::IPv6: ss << "Invalid IPv6 address: '" << str << "'";
                break;
            case FormatType::MAC: ss << "Invalid MAC address: '" << str << "'";
                break;
            case FormatType::URL: ss << "Invalid URL: '" << str << "'";
                break;
            case FormatType::DateTime: ss << "Invalid date/time format: '" << str << "' (expected ISO 8601)";
                break;
            case FormatType::Custom: ss << "Value does not match custom pattern: '" << str << "'";
                break;
            default: ss << "Value does not match expected format";
        }
        
        return ss.str();
    }
    
    std::string getExpectedType() const override {
        switch (formatType_) {
            case FormatType::Email: return "email address";
            case FormatType::IPv4: return "IPv4 address";
            case FormatType::IPv6: return "IPv6 address";
            case FormatType::MAC: return "MAC address";
            case FormatType::URL: return "URL";
            case FormatType::DateTime: return "date/time (ISO 8601)";
            case FormatType::Custom: return "custom pattern: " + customPattern_;
            default: return "unknown format";
        }
    }
    
    ConfigErrorCode getErrorCode() const override {
        return ConfigErrorCode::FormatValidationFailed;
    }
    
    std::string getDescription() const override {
        return description_;
    }
    
    std::unique_ptr<ValidatorBase> clone() const override {
        if (formatType_ == FormatType::Custom) {
            return std::make_unique<FormatValidator>(customPattern_, description_);
        } else {
            return std::make_unique<FormatValidator>(formatType_, description_);
        }
    }
};

// Validation error codes
enum class ValidationErrorCode {
    TypeMismatch,
    ValueOutOfRange,
    PatternMismatch,
    InvalidEnumValue,
    ConditionalValidationFailed,
    DependencyValidationFailed,
    CrossValidationFailed,
    FormatValidationFailed,
    BusinessLogicValidationFailed,
    RequiredKeyMissing,
    SchemaValidationFailed
};

// Validation error category
enum class ValidationErrorCategory {
    Type,
    Value,
    Pattern,
    Enum,
    Conditional,
    Dependency,
    Cross,
    Format,
    BusinessLogic,
    Required,
    Schema
};

// Enhanced validation error structure
struct ValidationError {
    std::string key;
    std::string expectedType;
    std::string actualType;
    std::string message;
    ValidationErrorCode errorCode;
    ValidationErrorCategory category;
    std::string errorCodeName;
    std::string path;
    bool recoverable;
    std::chrono::system_clock::time_point timestamp;
    std::string validatorType;
    std::unordered_map<std::string, std::any> context;
    
    ValidationError() : errorCode(ValidationErrorCode::SchemaValidationFailed), 
                       category(ValidationErrorCategory::Schema), recoverable(false),
                       timestamp(std::chrono::system_clock::now()) {}
    
    ValidationError(const std::string& k, ValidationErrorCode code, const std::string& msg,
                   const std::string& expected = "", const std::string& actual = "")
        : key(k), errorCode(code), message(msg), expectedType(expected), 
          actualType(actual), recoverable(false) {
        
        category = static_cast<ValidationErrorCategory>(static_cast<int>(code) / 2);
        errorCodeName = validationErrorCodeToString(code);
        timestamp = std::chrono::system_clock::now();
    }
    
    std::string formattedError() const {
        std::stringstream ss;
        ss << "[Validation Error] ";
        
        if (!key.empty()) {
            ss << "Key: \"" << key << "\" ";
        }
        
        ss << errorCodeName;
        
        if (!expectedType.empty() && !actualType.empty()) {
            ss << " (expected: " << expectedType << ", actual: " << actualType << ")";
        } else if (!message.empty()) {
            ss << " - " << message;
        }
        
        if (!path.empty()) {
            ss << " (path: " << path << ")";
        }
        
        if (!validatorType.empty()) {
            ss << " [Validator: " << validatorType << "]";
        }
        
        return ss.str();
    }
    
    std::string categoryName() const {
        switch (category) {
            case ValidationErrorCategory::Type: return "Type";
            case ValidationErrorCategory::Value: return "Value";
            case ValidationErrorCategory::Pattern: return "Pattern";
            case ValidationErrorCategory::Enum: return "Enum";
            case ValidationErrorCategory::Conditional: return "Conditional";
            case ValidationErrorCategory::Dependency: return "Dependency";
            case ValidationErrorCategory::Cross: return "Cross";
            case ValidationErrorCategory::Format: return "Format";
            case ValidationErrorCategory::BusinessLogic: return "BusinessLogic";
            case ValidationErrorCategory::Required: return "Required";
            case ValidationErrorCategory::Schema: return "Schema";
            default: return "Unknown";
        }
    }
    
    bool isCritical() const {
        return !recoverable && errorCode != ValidationErrorCode::SchemaValidationFailed;
    }
    
    static std::string validationErrorCodeToString(ValidationErrorCode code) {
        switch (code) {
            case ValidationErrorCode::TypeMismatch: return "TypeMismatch";
            case ValidationErrorCode::ValueOutOfRange: return "ValueOutOfRange";
            case ValidationErrorCode::PatternMismatch: return "PatternMismatch";
            case ValidationErrorCode::InvalidEnumValue: return "InvalidEnumValue";
            case ValidationErrorCode::ConditionalValidationFailed: return "ConditionalValidationFailed";
            case ValidationErrorCode::DependencyValidationFailed: return "DependencyValidationFailed";
            case ValidationErrorCode::CrossValidationFailed: return "CrossValidationFailed";
            case ValidationErrorCode::FormatValidationFailed: return "FormatValidationFailed";
            case ValidationErrorCode::BusinessLogicValidationFailed: return "BusinessLogicValidationFailed";
            case ValidationErrorCode::RequiredKeyMissing: return "RequiredKeyMissing";
            case ValidationErrorCode::SchemaValidationFailed: return "SchemaValidationFailed";
            default: return "UnknownValidationError";
        }
    }
};

// Enhanced validation result structure
struct ValidationResult {
    bool valid;
    std::vector<ValidationError> errors;
    std::vector<std::string> warnings;
    std::string summary;
    double successRate;
    std::chrono::milliseconds validationTime;
    bool cacheable;
    std::string cacheKey;
    std::unordered_map<std::string, std::any> metadata;
    
    ValidationResult() : valid(true), successRate(100.0), validationTime(0ms), cacheable(true) {
        metadata["validation_level"] = "strict";
        metadata["validation_groups"] = std::vector<std::string>{};
    }
    
    bool isValid() const { return valid && errors.empty(); }
    
    void addError(const ValidationError& error) {
        errors.push_back(error);
        valid = false;
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    std::string formattedResult() const {
        std::stringstream ss;
        if (valid) {
            ss << "[VALID] All validation checks passed";
            if (!warnings.empty()) {
                ss << " (" << warnings.size() << " warning(s))";
            }
        } else {
            ss << "[INVALID] " << errors.size() << " validation error(s), " << warnings.size() << " warning(s)";
            
            // Include first few errors in summary
            int errorLimit = std::min(3, static_cast<int>(errors.size()));
            for (int i = 0; i < errorLimit; ++i) {
                ss << "\n  " << errors[i].formattedError();
            }
            
            if (errors.size() > errorLimit) {
                ss << "\n  ... and " << (errors.size() - errorLimit) << " more errors";
            }
        }
        
        if (!summary.empty()) {
            ss << "\n" << summary;
        }
        
        return ss.str();
    }
    
    double calculateSuccessRate() const {
        int totalChecks = errors.size() + warnings.size();
        if (totalChecks == 0) return 100.0;
        return (100.0 * (totalChecks - warnings.size())) / totalChecks;
    }
    
    std::string toJSON() const {
        std::stringstream ss;
        ss << "{\n";
        ss << "  \"valid\": " << (valid ? "true" : "false") << ",\n";
        ss << "  \"error_count\": " << errors.size() << ",\n";
        ss << "  \"warning_count\": " << warnings.size() << ",\n";
        ss << "  \"success_rate\": " << successRate << ",\n";
        ss << "  \"validation_time_ms\": " << validationTime.count() << ",\n";
        ss << "  \"errors\": [\n";
        for (size_t i = 0; i < errors.size(); ++i) {
            ss << "    {\n";
            ss << "      \"key\": \"" << errors[i].key << "\",\n";
            ss << "      \"error_code\": \"" << errors[i].errorCodeName << "\",\n";
            ss << "      \"message\": \"" << errors[i].message << "\",\n";
            ss << "      \"category\": \"" << errors[i].categoryName() << "\"\n";
            if (i < errors.size() - 1) ss << "    },\n";
            else ss << "    }\n";
        }
        ss << "  ]\n";
        ss << "}\n";
        return ss.str();
    }
};

// Validation groups for organizing related validations
struct ValidationGroup {
    std::string name;
    std::vector<std::string> keys;
    std::vector<std::unique_ptr<ValidatorBase>> validators;
    bool enabled;
    std::string description;
    
    ValidationGroup(const std::string& n, const std::string& desc = "")
        : name(n), enabled(true), description(desc) {}
    
    void addKey(const std::string& key) {
        keys.push_back(key);
    }
    
    void addValidator(std::unique_ptr<ValidatorBase> validator) {
        validators.push_back(std::move(validator));
    }
    
    std::string getSummary() const {
        std::stringstream ss;
        ss << "Group: " << name;
        if (!description.empty()) ss << " (" << description << ")";
        ss << " - " << keys.size() << " key(s), " << validators.size() << " validator(s)";
        if (!enabled) ss << " [DISABLED]";
        return ss.str();
    }
};

// Validation levels
enum class ValidationLevel {
    None,
    Warning,
    Strict
};

// Advanced configuration schema entry with enhanced validation
struct ConfigSchemaEntry {
    std::string key;
    std::type_index type;
    TypeValidator validator;
    bool required;
    std::string description;
    std::vector<std::unique_ptr<ValidatorBase>> advancedValidators;
    std::string validationGroup;
    ValidationLevel validationLevel;
    bool cacheValidation;
    std::chrono::seconds cacheTimeout;
    
    ConfigSchemaEntry() : type(typeid(void)), required(false), validationLevel(ValidationLevel::Strict),
                         cacheValidation(false), cacheTimeout(300) {}
    
    ConfigSchemaEntry(const std::string& k, std::type_index t, bool req = false,
                     const std::string& desc = "", const TypeValidator& val = nullptr)
        : key(k), type(t), validator(val), required(req), description(desc),
          validationLevel(ValidationLevel::Strict), cacheValidation(false), cacheTimeout(300) {}
    
    void addAdvancedValidator(std::unique_ptr<ValidatorBase> validator) {
        advancedValidators.push_back(std::move(validator));
    }
    
    void setValidationGroup(const std::string& group) {
        validationGroup = group;
    }
    
    void setValidationLevel(ValidationLevel level) {
        validationLevel = level;
    }
    
    void enableCache(bool enable, const std::chrono::seconds& timeout = std::chrono::seconds(300)) {
        cacheValidation = enable;
        cacheTimeout = timeout;
    }
    
    std::string getTypeString() const {
        return ValidatorBase::typeIndexToString(type);
    }
};

// Validation dependency information
struct ValidationDependency {
    std::string dependentKey;
    std::any dependencyValue;
    std::string relationship;
    
    ValidationDependency(const std::string& key, const std::any& value, const std::string& rel = "equals")
        : dependentKey(key), dependencyValue(value), relationship(rel) {}
    
    bool isValid(const std::unordered_map<std::string, std::any>& configMap) const {
        auto it = configMap.find(dependentKey);
        if (it == configMap.end()) return false; // Key not present
        
        if (relationship == "equals") {
            return it->second == dependencyValue;
        } else if (relationship == "contains") {
            auto str1 = std::any_cast<std::string>(dependencyValue);
            auto str2 = std::any_cast<std::string>(it->second);
            return str2.find(str1) != std::string::npos;
        } else if (relationship == "greater_than") {
            auto num1 = std::any_cast<double>(dependencyValue);
            auto num2 = std::any_cast<double>(it->second);
            return num2 > num1;
        } else if (relationship == "less_than") {
            auto num1 = std::any_cast<double>(dependencyValue);
            auto num2 = std::any_cast<double>(it->second);
            return num2 < num1;
        }
        
        return false;
    }
};

// Advanced validation manager
class ValidationManager {
private:
    std::vector<ConfigSchemaEntry> schemaEntries_;
    std::vector<ValidationGroup> validationGroups_;
    std::vector<ValidationDependency> validationDependencies_;
    std::unordered_map<std::string, ValidationResult> validationCache_;
    std::string lastValidationKey_;
    mutable std::chrono::system_clock::time_point lastValidationTime_;
    ValidationLevel defaultValidationLevel_;
    bool enableCrossValidation_;
    bool enableBusinessLogicValidation_;
    
public:
    ValidationManager() : defaultValidationLevel_(ValidationLevel::Strict),
                         enableCrossValidation_(true), enableBusinessLogicValidation_(true) {
        lastValidationTime_ = std::chrono::system_clock::now();
    }
    
    void setDefaultValidationLevel(ValidationLevel level) {
        defaultValidationLevel_ = level;
    }
    
    void enableCrossValidation(bool enable) {
        enableCrossValidation_ = enable;
    }
    
    void enableBusinessLogicValidation(bool enable) {
        enableBusinessLogicValidation_ = enable;
    }
    
    void addSchemaEntry(ConfigSchemaEntry entry) {
        schemaEntries_.push_back(std::move(entry));
    }
    
    void addValidationGroup(ValidationGroup group) {
        validationGroups_.push_back(std::move(group));
    }
    
    void addValidationDependency(const ValidationDependency& dependency) {
        validationDependencies_.push_back(dependency);
    }
    
    ValidationResult validate(const std::unordered_map<std::string, std::any>& configMap) {
        ValidationResult result;
        auto startTime = std::chrono::system_clock::now();
        
        // Check cache first
        std::string cacheKey = calculateCacheKey(configMap);
        if (shouldUseCache(cacheKey)) {
            auto cachedResult = getCachedValidation(cacheKey);
            if (cachedResult.valid) {
                result = cachedResult;
                result.validationTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now() - startTime);
                return result;
            }
        }
        
        // Basic schema validation
        for (const auto& entry : schemaEntries_) {
            auto it = configMap.find(entry.key);
            if (it == configMap.end()) {
                if (entry.required) {
                    ValidationError error(entry.key, ValidationErrorCode::RequiredKeyMissing,
                                        "Required key is missing");
                    result.addError(error);
                }
                continue;
            }
            
            // Type validation
            if (!validateType(it->second, entry.type)) {
                ValidationError error(entry.key, ValidationErrorCode::TypeMismatch,
                                    "Type mismatch", entry.getTypeString(),
                                    ValidatorBase::typeIndexToString(std::type_index(it->second.type())));
                result.addError(error);
            }
            
            // Basic validator
            if (entry.validator && !entry.validator(it->second)) {
                ValidationError error(entry.key, ValidationErrorCode::SchemaValidationFailed,
                                    "Basic validation failed");
                result.addError(error);
            }
            
            // Advanced validators
            for (const auto& validator : entry.advancedValidators) {
                if (!validator->validate(it->second)) {
                    ValidationError error(entry.key, validator->getErrorCode(),
                                        validator->getErrorMessage(it->second),
                                        validator->getExpectedType(),
                                        ValidatorBase::typeIndexToString(std::type_index(it->second.type())));
                    error.validatorType = getValidatorTypeName(*validator);
                    result.addError(error);
                }
            }
        }
        
        // Cross-validation
        if (enableCrossValidation_) {
            performCrossValidation(configMap, result);
        }
        
        // Business logic validation
        if (enableBusinessLogicValidation_) {
            performBusinessLogicValidation(configMap, result);
        }
        
        // Dependency validation
        performDependencyValidation(configMap, result);
        
        // Calculate metrics
        result.validationTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - startTime);
        result.successRate = result.calculateSuccessRate();
        
        // Cache result if appropriate
        if (result.cacheable) {
            cacheValidation(cacheKey, result);
        }
        
        lastValidationKey_ = cacheKey;
        lastValidationTime_ = std::chrono::system_clock::now();
        
        return result;
    }
    
    std::string calculateCacheKey(const std::unordered_map<std::string, std::any>& configMap) const {
        std::stringstream ss;
        for (const auto& pair : configMap) {
            ss << pair.first << "=" << std::any_cast<std::string>(pair.second) << ";";
        }
        return ss.str();
    }
    
    bool shouldUseCache(const std::string& cacheKey) const {
        if (!validationCache_.count(cacheKey)) return false;
        
        auto cachedResult = validationCache_[cacheKey];
        auto now = std::chrono::system_clock::now();
        auto age = std::chrono::duration_cast<std::chrono::seconds>(now - cachedResult.validationTime);
        
        return age < std::chrono::seconds(300); // 5 minute cache
    }
    
    ValidationResult getCachedValidation(const std::string& cacheKey) const {
        if (validationCache_.count(cacheKey)) {
            return validationCache_[cacheKey];
        }
        return ValidationResult{};
    }
    
    void cacheValidation(const std::string& cacheKey, const ValidationResult& result) {
        validationCache_[cacheKey] = result;
    }
    
    void clearCache() {
        validationCache_.clear();
    }
    
    void performCrossValidation(const std::unordered_map<std::string, std::any>& configMap,
                               ValidationResult& result) {
        // Example cross-validation: if optimization_enabled is true, then max_iterations must be positive
        auto optIt = configMap.find("optimization_enabled");
        if (optIt != configMap.end() && std::any_cast<bool>(optIt->second)) {
            auto iterIt = configMap.find("max_iterations");
            if (iterIt != configMap.end()) {
                auto maxIter = std::any_cast<int>(iterIt->second);
                if (maxIter <= 0) {
                    ValidationError error("max_iterations", ValidationErrorCode::CrossValidationFailed,
                                        "max_iterations must be positive when optimization_enabled is true");
                    result.addError(error);
                }
            }
        }
    }
    
    void performBusinessLogicValidation(const std::unordered_map<std::string, std::any>& configMap,
                                       ValidationResult& result) {
        // Example business logic validation: learning_rate * batch_size must be < 1000
        auto lrIt = configMap.find("learning_rate");
        auto bsIt = configMap.find("batch_size");
        if (lrIt != configMap.end() && bsIt != configMap.end()) {
            auto lr = std::any_cast<double>(lrIt->second);
            auto bs = std::any_cast<int>(bsIt->second);
            if (lr * bs >= 1000.0) {
                ValidationError error("learning_rate", ValidationErrorCode::BusinessLogicValidationFailed,
                                    "learning_rate * batch_size must be < 1000");
                result.addError(error);
            }
        }
    }
    
    void performDependencyValidation(const std::unordered_map<std::string, std::any>& configMap,
                                    ValidationResult& result) {
        for (const auto& dependency : validationDependencies_) {
            if (!dependency.isValid(configMap)) {
                ValidationError error(dependency.dependentKey,
                                    ValidationErrorCode::DependencyValidationFailed,
                                    "Dependency validation failed: " + dependency.dependentKey + 
                                    " should have value " + std::any_cast<std::string>(dependency.dependencyValue) +
                                    " when " + dependency.relationship + " other conditions");
                result.addError(error);
            }
        }
    }
    
    std::string getValidatorTypeName(const ValidatorBase& validator) const {
        const std::type_info& type = typeid(validator);
        if (type == typeid(RangeValidator)) return "RangeValidator";
        if (type == typeid(PatternValidator)) return "PatternValidator";
        if (type == typeid(EnumValidator)) return "EnumValidator";
        if (type == typeid(ConditionalValidator)) return "ConditionalValidator";
        if (type == typeid(CompositeValidator)) return "CompositeValidator";
        if (type == typeid(FormatValidator)) return "FormatValidator";
        return "UnknownValidator";
    }
    
    bool validateType(const std::any& value, std::type_index expectedType) const {
        return std::type_index(value.type()) == expectedType;
    }
    
    const std::vector<ConfigSchemaEntry>& getSchemaEntries() const {
        return schemaEntries_;
    }
    
    const std::vector<ValidationGroup>& getValidationGroups() const {
        return validationGroups_;
    }
    
    const std::vector<ValidationDependency>& getValidationDependencies() const {
        return validationDependencies_;
    }
    
    void generateValidationReport(const ValidationResult& result) const {
        std::cout << result.formattedResult() << std::endl;
        std::cout << "\nDetailed Report:" << std::endl;
        std::cout << "Status: " << (result.valid ? "VALID" : "INVALID") << std::endl;
        std::cout << "Errors: " << result.errors.size() << std::endl;
        std::cout << "Warnings: " << result.warnings.size() << std::endl;
        std::cout << "Success Rate: " << result.successRate << "%" << std::endl;
        std::cout << "Validation Time: " << result.validationTime.count() << " ms" << std::endl;
        
        if (!result.errors.empty()) {
            std::cout << "\nError Categories:" << std::endl;
            std::unordered_map<ValidationErrorCategory, int> categoryCounts;
            for (const auto& error : result.errors) {
                categoryCounts[error.category]++;
            }
            
            for (const auto& pair : categoryCounts) {
                std::cout << "  " << ValidationError::validationErrorCodeToString(
                    static_cast<ValidationErrorCode>(static_cast<int>(pair.first) * 2)) 
                          << ": " << pair.second << std::endl;
            }
        }
    }
};

// Configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>
>;

// Enhanced configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Main configuration class
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from string (JSON/YAML)
    bool loadFromString(const std::string& content, const std::string& filename = "");
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (JSON format)
    bool saveToFile(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // File format detection and parsing
    static FileFormat detectFileFormat(const std::string& filepath);
    static bool isJSONFile(const std::string& content);
    static bool isYAMLOFile(const std::string& content);
    static bool isSimpleFormat(const std::string& content);
    
    // Type conversion utilities
    static std::optional<ConfigValue> stringToConfigValue(const std::string& str);
    static std::string configValueToString(const ConfigValue& value);
    static bool validateType(const ConfigValue& value, std::type_index expectedType);
    
    // Schema validation
    void addSchemaEntry(const std::string& key, std::type_index type, bool required = false,
                       const std::string& description = "", const TypeValidator& validator = nullptr);
    bool validateSchema() const;
    
    // Enhanced schema validation
    ValidationResult validate(const std::unordered_map<std::string, std::any>& configMap) const;
    ValidationResult validateFull(const std::unordered_map<std::string, std::any>& configMap) const;
    
    // Enhanced error handling with logging
    static bool logError(const std::string& operation, const std::string& message, 
                        const std::string& filepath = "", int line = 0);
    static bool logWarning(const std::string& operation, const std::string& message,
                         const std::string& filepath = "", int line = 0);
    
    // Validation helper methods
    void addValidationGroup(const ValidationGroup& group);
    void addValidationDependency(const ValidationDependency& dependency);
    void enableCrossValidation(bool enable);
    void enableBusinessLogicValidation(bool enable);
    void setValidationLevel(ValidationLevel level);
    
    // Json/YAML parsing
    static bool parseJSON(const std::string& content, std::unordered_map<std::string, std::any>& output);
    static bool parseYAML(const std::string& content, std::unordered_map<std::string, std::any>& output);
    static bool parseSimple(const std::string& content, std::unordered_map<std::string, std::any>& output);
    
    // File type enum
    enum class FileFormat {
        JSON,
        YAML,
        Simple,
        Unknown
    };
    
    // Validation builder patterns
    class SchemaBuilder {
    public:
        SchemaBuilder(Config& config) : config_(config) {}
        
        SchemaBuilder& addString(const std::string& key, bool required = false,
                                const std::string& description = "") {
            config_.addSchemaEntry(key, typeid(std::string), required, description);
            return *this;
        }
        
        SchemaBuilder& addInt(const std::string& key, bool required = false,
                             const std::string& description = "") {
            config_.addSchemaEntry(key, typeid(int), required, description);
            return *this;
        }
        
        SchemaBuilder& addDouble(const std::string& key, bool required = false,
                                const std::string& description = "") {
            config_.addSchemaEntry(key, typeid(double), required, description);
            return *this;
        }
        
        SchemaBuilder& addBool(const std::string& key, bool required = false,
                              const std::string& description = "") {
            config_.addSchemaEntry(key, typeid(bool), required, description);
            return *this;
        }
        
        SchemaBuilder& addRange(const std::string& key, int min, int max,
                               const std::string& description = "", bool inclusive = true) {
            auto validator = std::make_unique<RangeValidator>(min, max, description, inclusive);
            config_.getImpl().addAdvancedValidator(key, std::move(validator));
            return *this;
        }
        
        SchemaBuilder& addPattern(const std::string& key, const std::string& pattern,
                                 const std::string& description = "", bool caseSensitive = true) {
            auto validator = std::make_unique<PatternValidator>(pattern, description, caseSensitive);
            config_.getImpl().addAdvancedValidator(key, std::move(validator));
            return *this;
        }
        
        SchemaBuilder& addEnum(const std::string& key, const std::vector<std::string>& values,
                              const std::string& description = "", bool caseSensitive = true) {
            auto validator = std::make_unique<EnumValidator>(values, description, caseSensitive);
            config_.getImpl().addAdvancedValidator(key, std::move(validator));
            return *this;
        }
        
        SchemaBuilder& addFormat(const std::string& key, FormatValidator::FormatType format,
                                const std::string& description = "") {
            auto validator = std::make_unique<FormatValidator>(format, description);
            config_.getImpl().addAdvancedValidator(key, std::move(validator));
            return *this;
        }
        
        SchemaBuilder& addConditional(const std::string& key, const std::any& value,
                                     std::unique_ptr<ValidatorBase> thenValidator,
                                     const std::string& description = "") {
            auto validator = std::make_unique<ConditionalValidator>(
                key, value, std::move(thenValidator), description);
            config_.getImpl().addAdvancedValidator(key, std::move(validator));
            return *this;
        }
        
        SchemaBuilder& addComposite(const std::string& key,
                                   const std::vector<std::unique_ptr<ValidatorBase>>& validators,
                                   const std::string& description = "", bool requireAll = true) {
            auto validator = std::make_unique<CompositeValidator>(validators, description, requireAll);
            config_.getImpl().addAdvancedValidator(key, std::move(validator));
            return *this;
        }
        
    private:
        Config& config_;
    };
    
    SchemaBuilder getSchemaBuilder() {
        return SchemaBuilder(*this);
    }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::unique_ptr<ValidationManager> validationManager_;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string removeQuotes(const std::string& str);
    static bool isQuoted(const std::string& str);
    
    // Type conversion helpers
    template<typename T>
    static std::optional<T> anyToType(const std::any& value);
    
    // Schema validation helpers
    static std::string typeIndexToString(std::type_index type);
    
    // File format detection helpers
    static bool hasJsonExtension(const std::string& filepath);
    static bool hasYamlExtension(const std::string& filepath);
    static bool hasSimpleExtension(const std::string& filepath);
    
    // Json parsing helpers
    static bool parseJsonValue(const nlohmann::json& jsonValue, ConfigValue& configValue);
    
    // Yaml parsing helpers  
    static bool parseYamlValue(const YAML::Node& yamlValue, ConfigValue& configValue);
    
    // Implementation accessors
    Impl& getImpl() { return *pImpl; }
    const Impl& getImpl() const { return *pImpl; }
};