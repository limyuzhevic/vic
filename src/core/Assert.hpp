#pragma once

#include <cassert>
#include <stdexcept>
#include <string>
#include <source_location>
#include <stdexcept>
#include <string>
#include <sstream>

namespace nlm {

// Global assertion configuration
class AssertConfig {
public:
    static void setExceptionMode(bool enabled) { exceptionMode_ = enabled; }
    static bool isExceptionMode() { return exceptionMode_; }
    
    static void setAbortOnFailure(bool enabled) { abortOnFailure_ = enabled; }
    static bool isAbortOnFailure() { return abortOnFailure_; }
    
    static void setLogToConsole(bool enabled) { logToConsole_ = enabled; }
    static bool isLogToConsole() { return logToConsole_; }
    
    static void setFailureMessage(const std::string& message) { failureMessage_ = message; }
    static const std::string& getFailureMessage() { return failureMessage_; }
    
private:
    static inline bool exceptionMode_ = true;
    static inline bool abortOnFailure_ = false;
    static inline bool logToConsole_ = true;
    static inline std::string failureMessage_ = "Assertion failed";
};

// Assertion failure reporter
class AssertReporter {
public:
    static void reportFailure(const std::string& condition,
                             const std::string& message,
                             std::source_location location = std::source_location::current()) {
        std::ostringstream oss;
        oss << "Assertion failed: " << condition;
        if (!message.empty()) {
            oss << " (" << message << ")";
        }
        oss << "\n";
        oss << "File: " << location.file_name() << "\n";
        oss << "Line: " << location.line() << "\n";
        oss << "Function: " << location.function_name() << "\n";
        oss << "Expression: " << condition << "\n";
        
        std::string failureMsg = oss.str();
        
        if (AssertConfig::isLogToConsole()) {
            std::cerr << failureMsg << std::endl;
        }
        
        if (AssertConfig::isAbortOnFailure()) {
            std::abort();
        } else if (AssertConfig::isExceptionMode()) {
            throw std::invalid_argument(failureMsg);
        } else {
            assert(false && failureMsg.c_str());
        }
    }
};

// Runtime assertion class
class Assert {
public:
    // Basic assertions
    template<typename T>
    static void requires(bool condition, 
                         const std::string& message = "",
                         std::source_location location = std::source_location::current()) {
        if (!condition) {
            AssertReporter::reportFailure(
                "requires(" + std::to_string(static_cast<int>(condition)) + ")",
                message, location);
        }
    }
    
    template<typename T>
    static void ensures(bool condition,
                        const std::string& message = "",
                        std::source_location location = std::source_location::current()) {
        requires(condition, message, location);
    }
    
    template<typename T>
    static void invariants(bool condition,
                           const std::string& message = "",
                           std::source_location location = std::source_location::current()) {
        requires(condition, message, location);
    }
    
    // Parameter validation
    template<typename T>
    static void validates(const T& value, bool condition,
                          const std::string& message = "",
                          std::source_location location = std::source_location::current()) {
        if (!condition) {
            AssertReporter::reportFailure(
                "validates(value, " + std::to_string(static_cast<int>(condition)) + ")",
                message, location);
        }
    }
    
    // Range validation
    template<typename T>
    static void inRange(const T& value, const T& min, const T& max,
                        const std::string& message = "",
                        std::source_location location = std::source_location::current()) {
        requires(value >= min && value <= max, 
                 "Value " + std::to_string(static_cast<double>(value)) +
                 " is out of range [" + std::to_string(static_cast<double>(min)) +
                 ", " + std::to_string(static_cast<double>(max)) + "] " +
                 message, location);
    }
    
    // Pointer validation
    template<typename T>
    static void notNull(const T* ptr,
                        const std::string& message = "",
                        std::source_location location = std::source_location::current()) {
        validates(ptr, ptr != nullptr,
                  "Pointer is null: " + message, location);
    }
    
    template<typename T>
    static void notNull(T* ptr,
                        const std::string& message = "",
                        std::source_location location = std::source_location::current()) {
        notNull(const_cast<const T*>(ptr), message, location);
    }
    
    // Container validation
    template<typename Container>
    static void notEmpty(const Container& container,
                         const std::string& message = "",
                         std::source_location location = std::source_location::current()) {
        requires(!container.empty(),
                 "Container is empty: " + message, location);
    }
    
    // State validation
    template<typename T>
    static void validState(const T& state,
                           const std::string& message = "",
                           std::source_location location = std::source_location::current()) {
        requires(state.isValid(),
                 "Invalid state: " + message, location);
    }
    
    // Macro-style assertions (convenience functions)
    inline static void requires(bool condition, const std::string& message = "") {
        requires<bool>(condition, message);
    }
    
    inline static void ensures(bool condition, const std::string& message = "") {
        ensures<bool>(condition, message);
    }
    
    inline static void invariants(bool condition, const std::string& message = "") {
        invariants<bool>(condition, message);
    }
    
    inline static void validates(const std::string& value, bool condition,
                                 const std::string& message = "") {
        validates<std::string>(value, condition, message);
    }
    
    // Specialized validation for common types
    template<typename NumericType>
    static void isPositive(NumericType value,
                           const std::string& message = "",
                           std::source_location location = std::source_location::current()) {
        requires(value > 0,
                 "Value must be positive: " + message, location);
    }
    
    template<typename NumericType>
    static void isNonNegative(NumericType value,
                              const std::string& message = "",
                              std::source_location location = std::source_location::current()) {
        requires(value >= 0,
                 "Value must be non-negative: " + message, location);
    }
    
    template<typename NumericType>
    static void isLessThan(NumericType value, NumericType max,
                           const std::string& message = "",
                           std::source_location location = std::source_location::current()) {
        requires(value < max,
                 "Value must be less than " + std::to_string(static_cast<double>(max)) +
                 ": " + message, location);
    }
    
    template<typename NumericType>
    static void isGreaterThan(NumericType value, NumericType min,
                              const std::string& message = "",
                              std::source_location location = std::source_location::current()) {
        requires(value > min,
                 "Value must be greater than " + std::to_string(static_cast<double>(min)) +
                 ": " + message, location);
    }
    
    // Configuration setters
    static void setExceptionMode(bool enabled) {
        AssertConfig::setExceptionMode(enabled);
    }
    
    static void setAbortOnFailure(bool enabled) {
        AssertConfig::setAbortOnFailure(enabled);
    }
    
    static void setLogToConsole(bool enabled) {
        AssertConfig::setLogToConsole(enabled);
    }
    
    static void setFailureMessage(const std::string& message) {
        AssertConfig::setFailureMessage(message);
    }
    
    // Convenience macros (commented out to avoid macro issues)
    // #define NLM_ASSERT(condition) nlm::Assert::requires(condition, #condition)
    // #define NLM_ENSURE(condition) nlm::Assert::ensures(condition, #condition)
    // #define NLM_INVARIANT(condition) nlm::Assert::invariants(condition, #condition)
    // #define NLM_VALIDATE(value, condition) nlm::Assert::validates(value, condition, #condition)
    // #define NLM_REQUIRE(condition, message) nlm::Assert::requires(condition, message)
    // #define NLM_ENSURE(condition, message) nlm::Assert::ensures(condition, message)
};

// Convenience assertion functions (standalone)
inline void requires(bool condition, const std::string& message = "") {
    nlm::Assert::requires(condition, message);
}

inline void ensures(bool condition, const std::string& message = "") {
    nlm::Assert::ensures(condition, message);
}

inline void invariants(bool condition, const std::string& message = "") {
    nlm::Assert::invariants(condition, message);
}

inline void validates(const std::string& value, bool condition,
                      const std::string& message = "") {
    nlm::Assert::validates(value, condition, message);
}

// Specialized validation functions
template<typename NumericType>
inline void isPositive(NumericType value,
                       const std::string& message = "") {
    nlm::Assert::isPositive(value, message);
}

template<typename NumericType>
inline void isNonNegative(NumericType value,
                          const std::string& message = "") {
    nlm::Assert::isNonNegative(value, message);
}

template<typename NumericType>
inline void isLessThan(NumericType value, NumericType max,
                       const std::string& message = "") {
    nlm::Assert::isLessThan(value, max, message);
}

template<typename NumericType>
inline void isGreaterThan(NumericType value, NumericType min,
                          const std::string& message = "") {
    nlm::Assert::isGreaterThan(value, min, message);
}

// Contextual validation helper
class ValidationContext {
public:
    ValidationContext(const std::string& context) : context_(context) {}
    
    template<typename T>
    void requires(bool condition, const std::string& message = "") const {
        if (!condition) {
            std::string fullMessage = context_ + ": " + message;
            nlm::Assert::requires(condition, fullMessage);
        }
    }
    
    template<typename T>
    void validates(const T& value, bool condition,
                   const std::string& message = "") const {
        if (!condition) {
            std::string fullMessage = context_ + ": " + message;
            nlm::Assert::validates(value, condition, fullMessage);
        }
    }
    
private:
    std::string context_;
};

} // namespace nlm
