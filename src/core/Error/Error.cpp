// Exception implementation for NLM error handling
#include "Error.hpp"
#include <sstream>
#include <cstring>

namespace nlm {

// Helper function to get stack trace
std::vector<std::string> getStackTrace() {
    std::vector<std::string> trace;
    
    // In a real implementation, this would use platform-specific stack trace APIs
    // For now, we'll provide a simple implementation that can be extended
    
    trace.push_back("Stack trace not available in this build");
    return trace;
}

// Error helper functions
namespace ErrorHelper {
    
    // Validate file path
    bool validateFilePath(const std::string& path) {
        if (path.empty()) {
            return false;
        }
        // Check for null bytes
        if (path.find('\0') != std::string::npos) {
            return false;
        }
        return true;
    }
    
    // Validate memory size
    bool validateMemorySize(size_t size) {
        // Check for reasonable memory limits
        const size_t MAX_REASONABLE_ALLOCATION = 1024 * 1024 * 1024; // 1GB
        return size <= MAX_REASONABLE_ALLOCATION && size > 0;
    }
    
    // Clamp value within bounds
    template<typename T>
    T clampValue(T value, T min, T max) {
        if (value < min) return min;
        if (value > max) return max;
        return value;
    }
    
    // Safe string conversion
    template<typename T>
    std::string toString(const T& value) {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }
}

// Platform-specific implementations
namespace Platform {
#ifdef _WIN32
    // Windows-specific implementations
#else
    // POSIX implementations
#endif

    // Get current memory usage
    size_t getCurrentMemoryUsage() {
        // Platform-specific implementation
        return 0;
    }
    
    // Check if memory is available
    bool checkMemoryAvailable(size_t required) {
        return true; // Placeholder
    }
}

// Error recovery manager
class ErrorRecoveryManager {
public:
    static ErrorRecoveryManager& getInstance() {
        static ErrorRecoveryManager instance;
        return instance;
    }
    
    void registerRecoveryHandler(ErrorCategory category, std::function<bool(const NLMError&)> handler) {
        recoveryHandlers_[category] = std::move(handler);
    }
    
    bool attemptRecovery(const NLMError& error) {
        auto it = recoveryHandlers_.find(error.getCategory());
        if (it != recoveryHandlers_.end()) {
            return it->second(error);
        }
        return false;
    }
    
private:
    ErrorRecoveryManager() = default;
    std::unordered_map<ErrorCategory, std::function<bool(const NLMError&)>> recoveryHandlers_;
};

// Global error recovery manager instance
inline ErrorRecoveryManager& getErrorRecoveryManager() {
    return ErrorRecoveryManager::getInstance();
}

// Error scope implementation
template<typename T>
void ErrorUtils::ErrorScope<T>::executeCleanup() {
    if (active_ && cleanup_) {
        try {
            cleanup_();
        } catch (...) {
            // Cleanup errors should not propagate in destructor
        }
    }
}

// Global error handler initialization
namespace {
    struct GlobalErrorInitializer {
        GlobalErrorInitializer() {
            // Register default error recovery handlers
            getErrorRecoveryManager().registerRecoveryHandler(
                ErrorCategory::MemoryError,
                [](const NLMError& error) {
                    // Memory recovery handler
                    return static_cast<const MemoryError&>(error).getBytesRequested() <= 1024 * 1024; // 1MB
                }
            );
            
            getErrorRecoveryManager().registerRecoveryHandler(
                ErrorCategory::FileError,
                [](const NLMError&) {
                    // File recovery handler - try alternative paths
                    return false; // No default file recovery
                }
            );
        }
    };
    
    GlobalErrorInitializer globalErrorInitializer;
}

} // namespace nlm
