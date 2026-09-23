#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <fstream>
#include <iostream>

namespace nlm {

// Error handling utilities for validation and safety checks

namespace Validation {
    
    // Generic validation functions
    template<typename T>
    bool isValid(const T& value, const std::string& errorMsg = "Invalid value") {
        return true;  // Placeholder for actual validation logic
    }
    
    bool isFinite(double value, const std::string& errorMsg = "Value is not finite") {
        if (std::isnan(value) || std::isinf(value)) {
            std::cerr << "ERROR: " << errorMsg << " (value: " << value << ")" << std::endl;
            return false;
        }
        return true;
    }
    
    bool isPositive(double value, const std::string& errorMsg = "Value must be positive") {
        if (value <= 0.0) {
            std::cerr << "ERROR: " << errorMsg << " (value: " << value << ")" << std::endl;
            return false;
        }
        return true;
    }
    
    bool isNonNegative(double value, const std::string& errorMsg = "Value must be non-negative") {
        if (value < 0.0) {
            std::cerr << "ERROR: " << errorMsg << " (value: " << value << ")" << std::endl;
            return false;
        }
        return true;
    }
    
    bool isInRange(double value, double min, double max, 
                   const std::string& errorMsg = "Value outside valid range") {
        if (value < min || value > max) {
            std::cerr << "ERROR: " << errorMsg << " (value: " << value 
                     << ", range: [" << min << ", " << max << "])" << std::endl;
            return false;
        }
        return true;
    }
    
    // Configuration validation
    bool validateConfig(const std::unordered_map<std::string, std::string>& config,
                       const std::string& key, double min = -std::numeric_limits<double>::infinity(),
                       double max = std::numeric_limits<double>::infinity(),
                       const std::string& errorMsg = "Invalid configuration") {
        auto it = config.find(key);
        if (it == config.end()) {
            std::cerr << "ERROR: " << errorMsg << " (missing key: " << key << ")" << std::endl;
            return false;
        }
        
        try {
            double value = std::stod(it->second);
            if (!isFinite(value, "Invalid value in config key: " + key) ||
                !isInRange(value, min, max, "Config value out of range: " + key)) {
                return false;
            }
            return true;
        } catch (const std::exception& e) {
            std::cerr << "ERROR: " << errorMsg << " (failed to parse value for key: " << key 
                     << ", error: " << e.what() << ")" << std::endl;
            return false;
        }
    }
    
    // Memory safety validation
    template<typename T>
    bool validateMemoryAccess(const std::vector<T>& container, size_t index,
                              const std::string& context = "memory access") {
        if (index >= container.size()) {
            std::cerr << "ERROR: " << context << " - index " << index 
                     << " out of bounds (container size: " << container.size() << ")" << std::endl;
            return false;
        }
        return true;
    }
    
    template<typename T>
    bool validateMapAccess(const std::unordered_map<std::string, T>& map,
                           const std::string& key,
                           const std::string& context = "map access") {
        if (map.find(key) == map.end()) {
            std::cerr << "ERROR: " << context << " - key not found: " << key << std::endl;
            return false;
        }
        return true;
    }
    
    // File I/O validation
    bool validateFilePath(const std::string& filepath,
                          const std::string& context = "file operation") {
        if (filepath.empty()) {
            std::cerr << "ERROR: " << context << " - empty file path" << std::endl;
            return false;
        }
        // Additional file validation can be added here
        return true;
    }
    
    // Resource management validation
    bool validateResourceHandle(void* handle,
                               const std::string& context = "resource handle") {
        if (!handle) {
            std::cerr << "ERROR: " << context << " - null handle" << std::endl;
            return false;
        }
        return true;
    }
    
    // Thread safety validation (basic)
    class ThreadSafetyGuard {
    public:
        ThreadSafetyGuard(std::mutex& mutex, const std::string& context = "thread operation")
            : mutex_(mutex), context_(context) {
            mutex_.lock();
        }
        
        ~ThreadSafetyGuard() {
            mutex_.unlock();
        }
        
    private:
        std::mutex& mutex_;
        std::string context_;
    };
    
} // namespace Validation

} // namespace nlm
