// Error Context System for NLM
// Comprehensive error context tracking for better debugging and error reporting

#ifndef NLM_ERROR_CONTEXT_HPP
#define NLM_ERROR_CONTEXT_HPP

#include <string>
#include <unordered_map>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <ctime>
#include <mutex>
#include <stack>
#include "Errors.hpp"

// Error Context Categories
enum class ContextCategory {
    OPERATION,
    STATE,
    VALUE,
    SOURCE,
    PERFORMANCE,
    CONFIGURATION,
    MEMORY,
    NEURAL,
    WORLD
};

// Context Value Types
enum class ContextValueType {
    STRING,
    INTEGER,
    FLOAT,
    BOOLEAN,
    VECTOR,
    MAP,
    TIMESTAMP
};

// Structured context value
struct ContextValue {
    std::string key;
    std::string stringValue;
    int intValue;
    double floatValue;
    bool boolValue;
    std::vector<std::string> vectorValue;
    std::map<std::string, std::string> mapValue;
    std::chrono::system_clock::time_point timestampValue;
    ContextValueType type;
    
    ContextValue() : type(ContextValueType::STRING) {}
    
    std::string toString() const {
        switch (type) {
            case ContextValueType::STRING: return stringValue;
            case ContextValueType::INTEGER: return std::to_string(intValue);
            case ContextValueType::FLOAT: return std::to_string(floatValue);
            case ContextValueType::BOOLEAN: return boolValue ? "true" : "false";
            case ContextValueType::VECTOR: {
                std::stringstream ss;
                ss << "[";
                for (size_t i = 0; i < vectorValue.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << vectorValue[i];
                }
                ss << "]";
                return ss.str();
            }
            case ContextValueType::MAP: {
                std::stringstream ss;
                ss << "{";
                for (auto it = mapValue.begin(); it != mapValue.end(); ++it) {
                    if (it != mapValue.begin()) ss << ", ";
                    ss << it->first << "=" << it->second;
                }
                ss << "}";
                return ss.str();
            }
            case ContextValueType::TIMESTAMP: {
                auto time = std::chrono::system_clock::to_time_t(timestampValue);
                std::stringstream ss;
                ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
                return ss.str();
            }
            default: return "";
        }
    }
};

// Enhanced Error Context Structure
struct EnhancedErrorContext {
    std::string operation;
    std::string component;
    std::chrono::system_clock::time_point timestamp;
    std::vector<ContextValue> values;
    std::unordered_map<std::string, std::string> state;
    std::string stackTrace;
    int errorCode;
    std::string errorCategory;
    double executionTime;
    std::string threadId;
    bool recoverable;
    std::string suggestedAction;
    
    EnhancedErrorContext() 
        : timestamp(std::chrono::system_clock::now()), 
          errorCode(0), executionTime(0.0), recoverable(false) {
        threadId = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    }
    
    void addValue(const std::string& key, const std::string& value, ContextValueType type = ContextValueType::STRING) {
        ContextValue val;
        val.key = key;
        val.stringValue = value;
        val.type = type;
        values.push_back(val);
    }
    
    void addValue(const std::string& key, int value, ContextValueType type = ContextValueType::INTEGER) {
        ContextValue val;
        val.key = key;
        val.intValue = value;
        val.type = type;
        values.push_back(val);
    }
    
    void addValue(const std::string& key, double value, ContextValueType type = ContextValueType::FLOAT) {
        ContextValue val;
        val.key = key;
        val.floatValue = value;
        val.type = type;
        values.push_back(val);
    }
    
    void addValue(const std::string& key, bool value, ContextValueType type = ContextValueType::BOOLEAN) {
        ContextValue val;
        val.key = key;
        val.boolValue = value;
        val.type = type;
        values.push_back(val);
    }
    
    void addValue(const std::string& key, const std::vector<std::string>& value, ContextValueType type = ContextValueType::VECTOR) {
        ContextValue val;
        val.key = key;
        val.vectorValue = value;
        val.type = type;
        values.push_back(val);
    }
    
    void addValue(const std::string& key, const std::map<std::string, std::string>& value, ContextValueType type = ContextValueType::MAP) {
        ContextValue val;
        val.key = key;
        val.mapValue = value;
        val.type = type;
        values.push_back(val);
    }
    
    std::string getFormattedContext() const {
        std::stringstream ss;
        
        if (!operation.empty()) {
            ss << "Operation: " << operation << "\n";
        }
        if (!component.empty()) {
            ss << "Component: " << component << "\n";
        }
        
        ss << "Timestamp: " << getFormattedTimestamp() << "\n";
        ss << "Thread: " << threadId << "\n";
        ss << "Error Code: " << errorCode << "\n";
        ss << "Error Category: " << errorCategory << "\n";
        ss << "Execution Time: " << executionTime << " ms\n";
        ss << "Recoverable: " << (recoverable ? "true" : "false") << "\n";
        
        if (!suggestedAction.empty()) {
            ss << "Suggested Action: " << suggestedAction << "\n";
        }
        
        if (!state.empty()) {
            ss << "State:\n";
            for (const auto& pair : state) {
                ss << "  " << pair.first << " = " << pair.second << "\n";
            }
        }
        
        if (!values.empty()) {
            ss << "Values:\n";
            for (const auto& val : values) {
                ss << "  " << val.key << " = " << val.toString() << "\n";
            }
        }
        
        if (!stackTrace.empty()) {
            ss << "Stack Trace:\n" << stackTrace << "\n";
        }
        
        return ss.str();
    }
    
private:
    std::string getFormattedTimestamp() const {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

// Context Manager for tracking error contexts
class ErrorContextManager {
private:
    static ErrorContextManager* instance;
    std::stack<EnhancedErrorContext> contextStack;
    std::mutex mutex;
    bool enabled;
    
    ErrorContextManager() : enabled(true) {}
    
public:
    static ErrorContextManager& getInstance() {
        if (instance == nullptr) {
            instance = new ErrorContextManager();
        }
        return *instance;
    }
    
    static void enable(bool enable) {
        getInstance().enableInternal(enable);
    }
    
    void enableInternal(bool enable) {
        std::lock_guard<std::mutex> lock(mutex);
        this->enabled = enable;
    }
    
    static void pushContext(const EnhancedErrorContext& context) {
        getInstance().pushContextInternal(context);
    }
    
    void pushContextInternal(const EnhancedErrorContext& context) {
        std::lock_guard<std::mutex> lock(mutex);
        if (enabled) {
            contextStack.push(context);
        }
    }
    
    static EnhancedErrorContext popContext() {
        return getInstance().popContextInternal();
    }
    
    EnhancedErrorContext popContextInternal() {
        std::lock_guard<std::mutex> lock(mutex);
        if (!contextStack.empty()) {
            EnhancedErrorContext context = contextStack.top();
            contextStack.pop();
            return context;
        }
        return EnhancedErrorContext();
    }
    
    static EnhancedErrorContext peekContext() {
        return getInstance().peekContextInternal();
    }
    
    EnhancedErrorContext peekContextInternal() {
        std::lock_guard<std::mutex> lock(mutex);
        if (!contextStack.empty()) {
            return contextStack.top();
        }
        return EnhancedErrorContext();
    }
    
    static size_t getContextDepth() {
        return getInstance().getContextDepthInternal();
    }
    
    size_t getContextDepthInternal() {
        std::lock_guard<std::mutex> lock(mutex);
        return contextStack.size();
    }
    
    static void clearContext() {
        getInstance().clearContextInternal();
    }
    
    void clearContextInternal() {
        std::lock_guard<std::mutex> lock(mutex);
        while (!contextStack.empty()) {
            contextStack.pop();
        }
    }
    
    // Convenience methods for common operations
    static void setOperation(const std::string& operation) {
        EnhancedErrorContext context = getInstance().peekContextInternal();
        context.operation = operation;
        getInstance().pushContextInternal(context);
    }
    
    static void setComponent(const std::string& component) {
        EnhancedErrorContext context = getInstance().peekContextInternal();
        context.component = component;
        getInstance().pushContextInternal(context);
    }
    
    static void setErrorCode(int code) {
        EnhancedErrorContext context = getInstance().peekContextInternal();
        context.errorCode = code;
        getInstance().pushContextInternal(context);
    }
    
    static void setErrorCategory(const std::string& category) {
        EnhancedErrorContext context = getInstance().peekContextInternal();
        context.errorCategory = category;
        getInstance().pushContextInternal(context);
    }
    
    static void setExecutionTime(double timeMs) {
        EnhancedErrorContext context = getInstance().peekContextInternal();
        context.executionTime = timeMs;
        getInstance().pushContextInternal(context);
    }
    
    static void addContextValue(const std::string& key, const std::string& value) {
        EnhancedErrorContext context = getInstance().peekContextInternal();
        context.addValue(key, value);
        getInstance().pushContextInternal(context);
    }
    
    static void addContextValue(const std::string& key, int value) {
        EnhancedErrorContext context = getInstance().peekContextInternal();
        context.addValue(key, value);
        getInstance().pushContextInternal(context);
    }
    
    static void addContextValue(const std::string& key, double value) {
        EnhancedErrorContext context = getInstance().peekContextInternal();
        context.addValue(key, value);
        getInstance().pushContextInternal(context);
    }
    
    static void addContextValue(const std::string& key, bool value) {
        EnhancedErrorContext context = getInstance().peekContextInternal();
        context.addValue(key, value);
        getInstance().pushContextInternal(context);
    }
    
    static std::string getFormattedContext() {
        return getInstance().getFormattedContextInternal();
    }
    
    std::string getFormattedContextInternal() {
        std::lock_guard<std::mutex> lock(mutex);
        std::string result;
        for (const auto& context : contextStack) {
            result += context.getFormattedContext() + "\n===================\n";
        }
        return result;
    }
    
    // Context-aware error creation
    static NLMException createContextAwareError(const std::string& message, 
                                                ErrorCategory category,
                                                LogLevel severity = LogLevel::ERROR) {
        EnhancedErrorContext context = getInstance().peekContextInternal();
        
        NLMException error(message, category, severity, 
                          SourceLocation(), context.operation);
        
        // Add context values to error
        for (const auto& val : context.values) {
            error.getContext().values[val.key] = val.toString();
        }
        
        return error;
    }
};

// Context management macros
#define NLM_CONTEXT_PUSH(operation, component) \
    do { \
        EnhancedErrorContext ctx; \
        ctx.operation = operation; \
        ctx.component = component; \
        ErrorContextManager::pushContext(ctx); \
    } while(0)

#define NLM_CONTEXT_POP \
    ErrorContextManager::popContext()

#define NLM_CONTEXT_SET_VALUE(key, value) \
    ErrorContextManager::addContextValue(key, value)

#define NLM_CONTEXT_SET_VALUE_INT(key, value) \
    ErrorContextManager::addContextValue(key, value)

#define NLM_CONTEXT_SET_VALUE_DOUBLE(key, value) \
    ErrorContextManager::addContextValue(key, value)

#define NLM_CONTEXT_SET_VALUE_BOOL(key, value) \
    ErrorContextManager::addContextValue(key, value)

#define NLM_CONTEXT_SET_ERROR(code, category) \
    do { \
        ErrorContextManager::setErrorCode(code); \
        ErrorContextManager::setErrorCategory(category); \
    } while(0)

#define NLM_CONTEXT_SET_EXECUTION_TIME(time_ms) \
    ErrorContextManager::setExecutionTime(time_ms)

#define NLM_CONTEXT_ENABLE(enable) \
    ErrorContextManager::enable(enable)

#define NLM_GET_FORMATTED_CONTEXT \
    ErrorContextManager::getFormattedContext()

// Utility functions for error reporting
inline std::string formatErrorReport(const NLMException& error) {
    std::stringstream ss;
    ss << "=== NLM Error Report ===\n";
    ss << "Message: " << error.getMessage() << "\n";
    ss << "Category: " << error.getCategoryString(error.getCategory()) << "\n";
    ss << "Severity: " << error.getSeverityString(error.getSeverity()) << "\n";
    ss << "Context:\n" << error.getContext().getFormattedMessage() << "\n";
    ss << "Full Details:\n" << error.what() << "\n";
    return ss.str();
}

#endif // NLM_ERROR_CONTEXT_HPP