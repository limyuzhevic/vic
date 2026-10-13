// NLM Error Handling Utilities
// Provides comprehensive error handling and validation for NLM applications

#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <sstream>

namespace nlm {
namespace utils {

// Custom exception classes
class NLMException : public std::runtime_error {
public:
    NLMException(const std::string& message) : std::runtime_error(message) {}
};

class ConfigException : public NLMException {
public:
    ConfigException(const std::string& message) : NLMException("Configuration Error: " + message) {}
};

class ValidationException : public NLMException {
public:
    ValidationException(const std::string& message) : NLMException("Validation Error: " + message) {}
};

class BenchmarkException : public NLMException {
public:
    BenchmarkException(const std::string& message) : NLMException("Benchmark Error: " + message) {}
};

// Error reporting utilities
class ErrorReporter {
public:
    enum class Level { INFO, WARNING, ERROR, FATAL };
    
    static void report(Level level, const std::string& component, const std::string& message) {
        std::string levelStr;
        switch (level) {
            case Level::INFO:    levelStr = "INFO"; break;
            case Level::WARNING: levelStr = "WARNING"; break;
            case Level::ERROR:   levelStr = "ERROR"; break;
            case Level::FATAL:   levelStr = "FATAL"; break;
        }
        
        std::cerr << "[" << levelStr << "] " << component << ": " << message << std::endl;
    }
    
    static void reportError(const std::string& component, const std::string& message) {
        report(Level::ERROR, component, message);
    }
    
    static void reportWarning(const std::string& component, const std::string& message) {
        report(Level::WARNING, component, message);
    }
    
    static void reportFatal(const std::string& component, const std::string& message) {
        report(Level::FATAL, component, message);
        std::exit(1);
    }
};

// Validation utilities
class Validator {
public:
    struct ValidationResult {
        bool valid;
        std::string errorMessage;
        std::vector<std::string> warnings;
        
        ValidationResult(bool v = true, const std::string& error = "", const std::vector<std::string>& warns = {})
            : valid(v), errorMessage(error), warnings(warns) {}
    };
    
    static ValidationResult validateConfig(const std::string& configFile) {
        // Validate configuration file
        if (configFile.empty()) {
            return ValidationResult(false, "Configuration file path is empty");
        }
        
        // TODO: Add file existence and format validation
        
        return ValidationResult(true, "", {"Configuration file validation skipped - file existence check not implemented"});
    }
    
    static ValidationResult validateNumericRange(int value, int min, int max, const std::string& name) {
        if (value < min || value > max) {
            return ValidationResult(false, name + " must be between " + std::to_string(min) + 
                                   " and " + std::to_string(max) + ", got " + std::to_string(value));
        }
        return ValidationResult(true);
    }
    
    static ValidationResult validateStringNotEmpty(const std::string& value, const std::string& name) {
        if (value.empty()) {
            return ValidationResult(false, name + " cannot be empty");
        }
        return ValidationResult(true);
    }
    
    static ValidationResult validatePositive(int value, const std::string& name) {
        if (value <= 0) {
            return ValidationResult(false, name + " must be positive, got " + std::to_string(value));
        }
        return ValidationResult(true);
    }
    
    static ValidationResult validateNonNegative(int value, const std::string& name) {
        if (value < 0) {
            return ValidationResult(false, name + " cannot be negative, got " + std::to_string(value));
        }
        return ValidationResult(true);
    }
};

// Progress reporting utilities
class ProgressReporter {
public:
    static void showProgress(int current, int total, const std::string& description = "") {
        std::cout << "\rProgress: " << current << "/" << total;
        if (!description.empty()) {
            std::cout << " - " << description;
        }
        std::cout << std::flush;
    }
    
    static void finishProgress() {
        std::cout << std::endl;
    }
    
    static void showStep(int step, const std::string& description) {
        std::cout << "Step " << step << ": " << description << std::endl;
    }
};

// Performance monitoring utilities
class PerformanceMonitor {
public:
    struct Metrics {
        double cpuUsage = 0.0;
        double memoryUsage = 0.0;
        double wallClockTime = 0.0;
        size_t peakMemoryUsage = 0;
        int cpuCores = 0;
        
        std::string toString() const {
            std::stringstream ss;
            ss << "CPU Usage: " << cpuUsage << "%\n";
            ss << "Memory Usage: " << memoryUsage << " MB\n";
            ss << "Wall Clock Time: " << wallClockTime << " s\n";
            ss << "Peak Memory: " << peakMemoryUsage << " MB\n";
            ss << "CPU Cores: " << cpuCores << "\n";
            return ss.str();
        }
    };
    
    static Metrics measurePerformance() {
        // TODO: Implement actual performance measurement
        Metrics metrics;
        metrics.cpuCores = 1; // TODO: Detect actual core count
        return metrics;
    }
};

} // namespace utils
} // namespace nlm
