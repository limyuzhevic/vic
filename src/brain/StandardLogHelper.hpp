// StandardLogHelper.hpp - Standardized logging helper for all modules
#pragma once

#include "../core/Logger/Logger.hpp"
#include <string>
#include <sstream>

namespace nlm {

class StandardLogHelper {
public:
    // Standardized info log with context
    static void logInfo(const std::string& module, const std::string& message) {
        std::ostringstream oss;
        oss << "[" << module << "] " << message;
        NLM_LOG_INFO(oss.str());
    }
    
    // Standardized warning log with context
    static void logWarning(const std::string& module, const std::string& message) {
        std::ostringstream oss;
        oss << "[" << module << "] " << message;
        NLM_LOG_WARNING(oss.str());
    }
    
    // Standardized error log with context
    static void logError(const std::string& module, const std::string& message) {
        std::ostringstream oss;
        oss << "[" << module << "] " << message;
        NLM_LOG_ERROR(oss.str());
    }
    
    // Standardized debug log with context
    static void logDebug(const std::string& module, const std::string& message) {
        std::ostringstream oss;
        oss << "[" << module << "] " << message;
        NLM_LOG_DEBUG(oss.str());
    }
    
    // Log with structured data
    static void logStructured(const std::string& module, 
                             const std::string& level,
                             const std::string& message,
                             const std::string& file = "",
                             int line = 0,
                             const std::string& function = "") {
        NLM_LOG_INFO("STRUCTURED " << level << " " << message << " module:" << module);
    }
};

} // namespace nlm