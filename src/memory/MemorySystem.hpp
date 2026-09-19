#pragma once

#include "../core/Types/Types.hpp"
#include "../core/ErrorHandling/ErrorHandling.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include <memory>
#include <vector>
#include <string>

namespace nlm {

class MemorySystem {
public:
    virtual ~MemorySystem() = default;
    
    // Initialize with brain reference - common pattern across all memory systems
    virtual void initialize(Brain* brain) {
        if (!brain) {
            ErrorHandler::reportError(getNullPointerError("Brain", "MemorySystem::initialize"));
            return;
        }
        brain_ = brain;
        NLM_LOG_INFO("MemorySystem initialized");
    }
    
    // Clear all memory - common cleanup operation
    virtual void clear() {
        // Default implementation - derived classes override
        NLM_LOG_INFO("MemorySystem cleared");
    }
    
    // Check if system has been initialized
    bool isInitialized() const { return brain_ != nullptr; }
    
    // Get the brain reference
    Brain* getBrain() const { return brain_; }
    
    // Basic error checking pattern - common utility
    template<typename T>
    bool safeExecute(T func) {
        if (!brain_) {
            ErrorHandler::reportWarning("MemorySystem not initialized", "MemorySystem::safeExecute");
            return false;
        }
        try {
            func();
            return true;
        } catch (const NLMError& e) {
            ErrorHandler::reportError(e);
            return false;
        } catch (const std::exception& e) {
            ErrorHandler::reportError(NLMError("Exception in safeExecute: " + std::string(e.what()), ErrorSeverity::Error, ErrorCategory::System, "MemorySystem::safeExecute"));
            return false;
        }
    }
    
    // Memory statistics tracking - common pattern
    virtual void updateStats() {
        // Default implementation - derived classes override
    }
    
    virtual size_t getMemoryUsage() const { return 0; }
    virtual size_t getMaxMemoryUsage() const { return 0; }
    virtual float getUsagePercentage() const { return 0.0f; }
    
    // Error handling utility methods
    void reportOperationError(const std::string& operation, const std::string& error) {
        ErrorHandler::reportError(NLMError("Operation " + operation + " failed: " + error, ErrorSeverity::Error, ErrorCategory::System, "MemorySystem", __LINE__, "reportOperationError"));
    }
    
    void reportOperationWarning(const std::string& operation, const std::string& warning) {
        ErrorHandler::reportWarning("Operation " + operation + ": " + warning, "MemorySystem");
    }
    
    void reportOperationInfo(const std::string& operation, const std::string& info) {
        ErrorHandler::reportInfo("Operation " + operation + ": " + info, "MemorySystem");
    }
    
protected:
    Brain* brain_ = nullptr;
};

} // namespace nlm
