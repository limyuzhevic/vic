#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>

namespace nlm {

class ErrorHandlingUtils {
public:
    // Enhanced error handling with stack traces
    static void logError(const std::string& component, const std::string& message,
                        const std::string& file = __FILE__,
                        int line = __LINE__,
                        const std::string& function = __func__,
                        const std::vector<std::string>& details = {});
    
    // Validate parameter with detailed error messages
    static bool validate(const std::string& name, const std::string& value,
                        const std::string& type = "string",
                        const std::vector<std::string>& validValues = {});
    
    // Safe conversion functions
    static int toInt(const std::string& str, int defaultValue = 0);
    static double toDouble(const std::string& str, double defaultValue = 0.0);
    static bool toBool(const std::string& str);
    
    // Check file existence
    static bool fileExists(const std::string& path);
    
    // Create backup directory
    static bool createBackupDir(const std::string& basePath);
};

// Global error handling instance
static ErrorHandlingUtils errorHandler;
}