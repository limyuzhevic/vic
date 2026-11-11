#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>

namespace nlm {

class ErrorHandling {
public:
    static void initialize();
    
    // Log error with details
    static void logError(const std::string& component, const std::string& message, 
                        const std::vector<std::string>& details = {});
    
    // Validate parameter
    static bool validateParameter(const std::string& name, const std::string& value,
                                  const std::string& type = "string",
                                  const std::vector<std::string>& validValues = {});
    
    // Create error report
    static std::string createErrorReport(const std::string& component, 
                                         const std::string& context = "");
    
    // Check for critical errors
    static bool hasCriticalErrors() const;
    
    // Reset error state
    static void reset();
};