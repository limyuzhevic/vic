// Include the core exception hierarchy and necessary headers
#pragma once

#include "exceptions/BrainException.hpp"
#include <map>
#include <vector>
#include <string>

namespace nlm {

/**
 * Error recovery management
 */
class ErrorRecoveryManager {
public:
    enum class RecoveryAction {
        Continue,           // Continue with degraded functionality
        Rollback,           // Rollback to previous state
        Fallback,           // Use fallback/default behavior
        Abort,              // Stop operation
        Retry              // Retry operation
    };
    
    // Recovery strategy for different error types
    static RecoveryAction getRecoveryStrategy(BrainException::ErrorCode errorCode);
    
    // Execute recovery action
    static bool executeRecovery(RecoveryAction action, 
                               const std::map<std::string, std::string>& context = {});
    
    // State management for rollback
    static void saveState(const std::string& operationName);
    static bool restoreState(const std::string& operationName);
    static void clearState(const std::string& operationName);
    
    // Graceful degradation
    static bool isNonCriticalError(BrainException::ErrorCode errorCode);
    static void enableGracefulDegradation(const std::string& component);
    static bool isGracefulDegradationEnabled(const std::string& component);
};

} // namespace nlm
