#pragma once

#include <stdexcept>
#include <string>
#include <memory>
#include <source_location>
#include <sstream>

namespace nlm {

// Base exception
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message,
                          std::source_location location = std::source_location::current())
        : std::runtime_error(formatMessage(message, location)),
          location_(location) {}
    
    const std::source_location& location() const noexcept { return location_; }
    
    const char* what() const noexcept override {
        return std::runtime_error::what();
    }
    
protected:
    static std::string formatMessage(const std::string& message, std::source_location location) {
        std::ostringstream oss;
        oss << message;
        if (location.line() != 0) {
            oss << " (" << location.file_name() << ":" << location.line() << ")";
        }
        return oss.str();
    }
    
private:
    std::source_location location_;
};

// Initialization exceptions
class InitializationException : public NLMException {
public:
    explicit InitializationException(const std::string& message,
                                     std::source_location location = std::source_location::current())
        : NLMException("Initialization failed: " + message, location) {}
};

class ConfigurationException : public InitializationException {
public:
    explicit ConfigurationException(const std::string& message,
                                   std::source_location location = std::source_location::current())
        : InitializationException("Configuration error: " + message, location) {}
};

// Simulation exceptions
class SimulationException : public NLMException {
public:
    explicit SimulationException(const std::string& message,
                                std::source_location location = std::source_location::current())
        : NLMException("Simulation error: " + message, location) {}
};

// Memory exceptions
class MemoryException : public NLMException {
public:
    explicit MemoryException(const std::string& message,
                            std::source_location location = std::source_location::current())
        : NLMException("Memory error: " + message, location) {}
};

// Validation exceptions
class ValidationException : public NLMException {
public:
    explicit ValidationException(const std::string& message,
                                std::source_location location = std::source_location::current())
        : NLMException("Validation error: " + message, location) {}
};

// Neural network exceptions
class NeuralNetworkException : public NLMException {
public:
    explicit NeuralNetworkException(const std::string& message,
                                   std::source_location location = std::source_location::current())
        : NLMException("Neural network error: " + message, location) {}
};

// Signal processing exceptions
class SignalProcessingException : public NLMException {
public:
    explicit SignalProcessingException(const std::string& message,
                                      std::source_location location = std::source_location::current())
        : NLMException("Signal processing error: " + message, location) {}
};

// Experiment exceptions
class ExperimentException : public NLMException {
public:
    explicit ExperimentException(const std::string& message,
                                std::source_location location = std::source_location::current())
        : NLMException("Experiment error: " + message, location) {}
};

// Smart pointer factory functions
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T>
std::shared_ptr<T> make_shared() {
    return std::make_shared<T>();
}

template<typename T, typename U>
std::shared_ptr<T> static_pointer_cast(std::shared_ptr<U> ptr) {
    return std::static_pointer_cast<T>(ptr);
}

template<typename T, typename U>
std::shared_ptr<T> dynamic_pointer_cast(std::shared_ptr<U> ptr) {
    return std::dynamic_pointer_cast<T>(ptr);
}

template<typename T, typename U>
std::shared_ptr<T> const_pointer_cast(std::shared_ptr<U> ptr) {
    return std::const_pointer_cast<T>(ptr);
}

// Type traits for exception safety
template<typename T>
struct exception_safe {
    static constexpr bool value = std::is_nothrow_move_constructible<T>::value &&
                                   std::is_nothrow_move_assignable<T>::value;
};

// Safe assignment with exception safety
template<typename T>
void safe_assign(T& target, T&& source) noexcept(exception_safe<T>::value) {
    if constexpr (exception_safe<T>::value) {
        target = std::move(source);
    } else {
        T temp = std::move(source);
        target.~T();
        new(&target) T(std::move(temp));
    }
}

// Exception helper functions
template<typename F>
auto withExceptionHandling(F&& func) -> decltype(func()) {
    try {
        return func();
    } catch (const std::exception& e) {
        throw;
    } catch (...) {
        throw NLMException("Unknown error in exception handling");
    }
}

// Current exception capture
class ExceptionCapture {
public:
    ExceptionCapture() : exception_(), hasException_(false) {}
    
    template<typename F>
    void capture(F&& func) {
        try {
            func();
        } catch (...) {
            exception_ = std::current_exception();
            hasException_ = true;
        }
    }
    
    bool hasException() const { return hasException_; }
    
    void rethrow() {
        if (hasException_) {
            std::rethrow_exception(exception_);
        }
    }
    
private:
    std::exception_ptr exception_;
    bool hasException_;
};

} // namespace nlm
