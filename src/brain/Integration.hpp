// Integration utility - cross-system coordination and error handling
#pragma once

#include <memory>
#include <string>
#include <functional>

namespace nlm {

class Integration {
public:
    Integration();
    ~Integration();
    
    // Validate system integration
    static bool validateSystemHealth(const std::string& systemName);
    
    // Handle cross-system errors
    static void handleCrossSystemError(const std::string& errorContext, const std::exception& e);
    
    // Log system status
    static void logSystemStatus(const std::string& systemName);
    
    // Validate memory pool usage
    static bool validateMemoryPoolUsage();
    
    // Validate configuration
    static bool validateConfiguration();
    
private:
    static bool validateMemoryUsage();
    static bool validatePerformanceCounters();
};

} // namespace nlm
