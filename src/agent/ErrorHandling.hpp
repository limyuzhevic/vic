#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <exception>
#include <stdexcept>
#include "AgentBrain.hpp"

namespace nlm {

// Exception hierarchy for NLM errors
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) 
        : std::runtime_error("NLM Error: " + message) {}
};

class AgentBrainException : public NLMException {
public:
    explicit AgentBrainException(const std::string& message)
        : NLMException("AgentBrain Error: " + message) {}
};

class CommandException : public NLMException {
public:
    explicit CommandException(const std::string& message)
        : NLMException("Command Error: " + message) {}
};

class ConfigException : public NLMException {
public:
    explicit ConfigException(const std::string& message)
        : NLMException("Configuration Error: " + message) {}
};

class MemoryException : public NLMException {
public:
    explicit MemoryException(const std::string& message)
        : NLMException("Memory Error: " + message) {}
};

// Result type for operations that can fail
enum class OperationResult {
    Success,
    Failure,
    InvalidInput,
    NotFound,
    OutOfMemory,
    Timeout,
    PermissionDenied,
    AlreadyExists,
    ResourceInUse,
    SystemError
};

// Error context for detailed error reporting
struct ErrorContext {
    std::string operation;
    std::string component;
    std::string details;
    std::map<std::string, std::string> metadata;
    std::vector<std::string> suggestions;
    
    ErrorContext(const std::string& op, const std::string& comp, const std::string& det)
        : operation(op), component(comp), details(det) {}
    
    void addMetadata(const std::string& key, const std::string& value) {
        metadata[key] = value;
    }
    
    void addSuggestion(const std::string& suggestion) {
        suggestions.push_back(suggestion);
    }
};

// Exception with context
class ContextException : public std::runtime_error {
public:
    ContextException(const std::string& message, const ErrorContext& context)
        : std::runtime_error(formatWithContext(message, context)) {}
    
private:
    static std::string formatWithContext(const std::string& message, const ErrorContext& context) {
        std::ostringstream oss;
        oss << "NLM " << context.component << " Error:";
        oss << "\n  Operation: " << context.operation;
        oss << "\n  Message: " << message;
        oss << "\n  Details: " << context.details;
        
        if (!context.metadata.empty()) {
            oss << "\n  Metadata:";
            for (const auto& pair : context.metadata) {
                oss << "\n    " << pair.first << " = " << pair.second;
            }
        }
        
        if (!context.suggestions.empty()) {
            oss << "\n  Suggestions:";
            for (const auto& suggestion : context.suggestions) {
                oss << "\n    - " << suggestion;
            }
        }
        
        return oss.str();
    }
};

// Result type for error handling
template<typename T>
class Result {
public:
    Result(const T& value, OperationResult status = OperationResult::Success)
        : value_(value), status_(status) {}
    
    Result(T&& value, OperationResult status = OperationResult::Success)
        : value_(std::move(value)), status_(status) {}
    
    Result(OperationResult status, const std::string& errorMessage)
        : status_(status), errorMessage_(errorMessage) {}
    
    Result(const std::string& errorMessage, OperationResult status = OperationResult::Failure)
        : status_(status), errorMessage_(errorMessage) {}
    
    bool isSuccess() const { return status_ == OperationResult::Success; }
    bool isFailure() const { return !isSuccess(); }
    
    OperationResult status() const { return status_; }
    const std::string& errorMessage() const { return errorMessage_; }
    const T& value() const { return value_; }
    T& value() { return value_; }
    
    // Access value with exception safety
    T& getValue() {
        if (status_ != OperationResult::Success) {
            throw ContextException("Cannot access value from failed result", 
                ErrorContext("get_value", "Result", errorMessage_));
        }
        return value_;
    }
    
    const T& getValue() const {
        if (status_ != OperationResult::Success) {
            throw ContextException("Cannot access value from failed result", 
                ErrorContext("get_value", "Result", errorMessage_));
        }
        return value_;
    }
    
    // Equality operators
    bool operator==(const Result& other) const {
        return status_ == other.status_ && value_ == other.value_ && errorMessage_ == other.errorMessage_;
    }
    
    bool operator!=(const Result& other) const {
        return !(*this == other);
    }
    
private:
    T value_;
    OperationResult status_;
    std::string errorMessage_;
};

// Error handling utilities
namespace ErrorHandling {
    // Safe execution wrapper
    template<typename F, typename... Args>
    Result<typename std::result_of<F(Args...)>::type> safeExecute(F&& func, Args&&... args) {
        try {
            auto result = func(std::forward<Args>(args)...);
            return Result<typename std::result_of<F(Args...)>::type>(std::move(result));
        } catch (const ContextException& e) {
            return Result<typename std::result_of<F(Args...)>::type>(OperationResult::SystemError, e.what());
        } catch (const std::bad_alloc&) {
            return Result<typename std::result_of<F(Args...)>::type>(OperationResult::OutOfMemory, "Memory allocation failed");
        } catch (const std::exception& e) {
            return Result<typename std::result_of<F(Args...)>::type>(OperationResult::Failure, e.what());
        }
    }
    
    // Resource management with cleanup
    template<typename T, typename CleanupFunc>
    class ScopedResource {
    public:
        ScopedResource(T resource, CleanupFunc cleanup)
            : resource_(std::move(resource)), cleanup_(std::move(cleanup)), owned_(true) {}
        
        ScopedResource(T* resource, CleanupFunc cleanup)
            : resource_(*resource), cleanup_(std::move(cleanup)), owned_(false) {
            // Detach ownership from original pointer
            if (resource) {
                // Note: Original ownership remains with caller
            }
        }
        
        ~ScopedResource() {
            if (owned_) {
                cleanup_(resource_);
            }
        }
        
        // Disable copying
        ScopedResource(const ScopedResource&) = delete;
        ScopedResource& operator=(const ScopedResource&) = delete;
        
        // Enable moving
        ScopedResource(ScopedResource&& other) noexcept
            : resource_(std::move(other.resource_)), cleanup_(std::move(other.cleanup_)), owned_(other.owned_) {
            other.owned_ = false;
        }
        
        ScopedResource& operator=(ScopedResource&& other) noexcept {
            if (this != &other) {
                if (owned_) {
                    cleanup_(resource_);
                }
                resource_ = std::move(other.resource_);
                cleanup_ = std::move(other.cleanup_);
                owned_ = other.owned_;
                other.owned_ = false;
            }
            return *this;
        }
        
        T& get() { return resource_; }
        const T& get() const { return resource_; }
        
        T* operator->() { return &resource_; }
        const T* operator->() const { return &resource_; }
        
        T& operator*() { return resource_; }
        const T& operator*() const { return resource_; }
        
        bool isOwned() const { return owned_; }
        
        void release() { owned_ = false; }
        
    private:
        T resource_;
        CleanupFunc cleanup_;
        bool owned_;
    };
    
    // Exception converter
    template<typename T>
    Result<T> convertException(const std::function<T()>& func) {
        return safeExecute(func);
    }
}

// Template implementation for Result
template<typename T>
Result<T> operator|(const Result<T>& result, const std::function<T(Result<T>)>& func) {
    if (result.isSuccess()) {
        return func(result);
    } else {
        return Result<T>(result.status(), result.errorMessage());
    }
}

// Factory pattern for creating error contexts
class ErrorFactory {
public:
    static ErrorContext createAgentBrainError(const std::string& operation, 
                                             const std::string& details,
                                             const std::map<std::string, std::string>& metadata = {}) {
        ErrorContext context(operation, "AgentBrain", details);
        for (const auto& pair : metadata) {
            context.addMetadata(pair.first, pair.second);
        }
        return context;
    }
    
    static ErrorContext createCommandError(const std::string& operation,
                                           const std::string& details,
                                           const std::vector<std::string>& suggestions = {}) {
        ErrorContext context(operation, "Command", details);
        for (const auto& suggestion : suggestions) {
            context.addSuggestion(suggestion);
        }
        return context;
    }
    
    static ErrorContext createMemoryError(const std::string& operation,
                                          const std::string& details,
                                          const std::map<std::string, std::string>& metadata = {}) {
        ErrorContext context(operation, "Memory", details);
        for (const auto& pair : metadata) {
            context.addMetadata(pair.first, pair.second);
        }
        return context;
    }
};

// Resource management for memory pools
class MemoryPool {
public:
    template<typename T>
    class Allocator {
    public:
        using value_type = T;
        using pointer = T*;
        using const_pointer = const T*;
        using void_pointer = void*;
        using const_void_pointer = const void*;
        using propagate_on_container_copy_assignment = std::true_type;
        using propagate_on_container_move_assignment = std::true_type;
        using propagate_on_container_swap = std::true_type;
        using is_always_equal = std::false_type;
        
        Allocator() = default;
        
        template<typename U>
        Allocator(const Allocator<U>&) {}
        
        T* allocate(std::size_t n) {
            if (n == 0) return nullptr;
            
            void* ptr = std::malloc(n * sizeof(T));
            if (!ptr) {
                throw std::bad_alloc();
            }
            
            return static_cast<T*>(ptr);
        }
        
        void deallocate(T* p, std::size_t n) {
            if (p) {
                std::free(p);
            }
        }
        
        bool operator==(const Allocator&) const { return true; }
        bool operator!=(const Allocator&) const { return false; }
    };
    
    template<typename T>
    using vector = std::vector<T, Allocator<T>>;
    
    template<typename T>
    using unique_ptr = std::unique_ptr<T, std::function<void(T*)>>;
    
    template<typename T>
    using shared_ptr = std::shared_ptr<T, Allocator<T>>;
    
    MemoryPool() = default;
    ~MemoryPool() = default;
    
    // Disable copying and assignment
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;
    
    // Get default instance
    static MemoryPool& getInstance() {
        static MemoryPool instance;
        return instance;
    }
    
private:
    // Private constructor for singleton
    MemoryPool() = default;
};

// Design pattern implementations
namespace DesignPatterns {
    // Observer pattern for state changes
    class Observer {
    public:
        virtual ~Observer() = default;
        virtual void onStateChanged(const std::string& state) = 0;
    };
    
    class Observable {
    public:
        virtual ~Observable() = default;
        
        void addObserver(std::shared_ptr<Observer> observer) {
            observers_.push_back(observer);
        }
        
        void removeObserver(std::shared_ptr<Observer> observer) {
            observers_.erase(std::remove(observers_.begin(), observers_.end(), observer),
                           observers_.end());
        }
        
        void notifyObservers(const std::string& state) {
            for (const auto& observer : observers_) {
                observer->onStateChanged(state);
            }
        }
        
    protected:
        std::vector<std::shared_ptr<Observer>> observers_;
    };
    
    // Strategy pattern for command execution
    class CommandStrategy {
    public:
        virtual ~CommandStrategy() = default;
        virtual Result<std::string> execute(const std::map<std::string, std::string>& params) = 0;
    };
    
    // Factory pattern for object creation
    class BrainFactory {
    public:
        virtual ~BrainFactory() = default;
        virtual std::shared_ptr<AgentBrain> createBrain(const std::map<std::string, std::string>& config) = 0;
    };
    
    // Iterator pattern for traversing brain components
    class ComponentIterator {
    public:
        virtual ~ComponentIterator() = default;
        virtual bool hasNext() const = 0;
        virtual std::string next() = 0;
    };
    
    // Composite pattern for command sequences
    class CompositeCommand : public Command {
    public:
        void addCommand(std::unique_ptr<Command> command) {
            commands_.push_back(std::move(command));
        }
        
        CommandResult execute(AgentBrain& agent, 
                             const std::map<std::string, std::string>& context) override {
            std::string log = "Composite command execution:\n";
            for (size_t i = 0; i < commands_.size(); ++i) {
                log += std::to_string(i) + ". " + commands_[i]->getDescription() + "\n";
                CommandResult result = commands_[i]->execute(agent, context);
                log += "  Result: " + (result.success ? "SUCCESS" : "FAILED") + "\n";
                if (!result.success) {
                    return CommandResult(false, log + "Command failed at step " + std::to_string(i));
                }
            }
            return CommandResult(true, log + "All commands executed successfully");
        }
        
        std::string getDescription() const override {
            return "Composite command with " + std::to_string(commands_.size()) + " subcommands";
        }
        
    private:
        std::vector<std::unique_ptr<Command>> commands_;
    };
    
    // State pattern for agent states
    class AgentState {
    public:
        virtual ~AgentState() = default;
        virtual Result<std::string> handle(AgentBrain& agent, 
                                         const std::map<std::string, std::string>& context) = 0;
        virtual std::string getStateName() const = 0;
    };
    
    class IdleState : public AgentState {
    public:
        Result<std::string> handle(AgentBrain& agent,
                                   const std::map<std::string, std::string>& context) override {
            return Result<std::string>("Agent is idle", OperationResult::Success);
        }
        
        std::string getStateName() const override {
            return "Idle";
        }
    };
    
    class ExecutingState : public AgentState {
    public:
        Result<std::string> handle(AgentBrain& agent,
                                   const std::map<std::string, std::string>& context) override {
            // In a real implementation, this would execute the current command
            return Result<std::string>("Agent is executing command", OperationResult::Success);
        }
        
        std::string getStateName() const override {
            return "Executing";
        }
    };
}

// Error recovery utilities
namespace ErrorRecovery {
    // Retry mechanism
    template<typename F, typename... Args>
    Result<typename std::result_of<F(Args...)>::type> withRetry(F&& func, Args&&... args,
                                                             int maxRetries = 3,
                                                             std::chrono::milliseconds delay = std::chrono::milliseconds(100)) {
        int retryCount = 0;
        
        while (true) {
            auto result = ErrorHandling::safeExecute(func, std::forward<Args>(args)...);
            
            if (result.isSuccess() || retryCount >= maxRetries) {
                return result;
            }
            
            // Exponential backoff
            std::this_thread::sleep_for(delay * (1 << retryCount));
            retryCount++;
        }
    }
    
    // Circuit breaker pattern
    class CircuitBreaker {
    public:
        CircuitBreaker(int failureThreshold = 5, std::chrono::seconds timeout = std::chrono::seconds(60))
            : failureThreshold_(failureThreshold), timeout_(timeout),
              consecutiveFailures_(0), lastFailureTime_(0), state_(CircuitState::Closed) {}
        
        bool canExecute() const {
            if (state_ == CircuitState::Open) {
                if (std::chrono::steady_clock::now() - lastFailureTime_ > timeout_) {
                    state_ = CircuitState::HalfOpen;
                    return true;
                }
                return false;
            }
            return true;
        }
        
        void recordSuccess() {
            if (state_ == CircuitState::HalfOpen) {
                state_ = CircuitState::Closed;
            }
            consecutiveFailures_ = 0;
        }
        
        void recordFailure() {
            consecutiveFailures_++;
            if (consecutiveFailures_ >= failureThreshold_) {
                state_ = CircuitState::Open;
                lastFailureTime_ = std::chrono::steady_clock::now();
            }
        }
        
        bool isOpen() const { return state_ == CircuitState::Open; }
        
    private:
        enum class CircuitState {
            Closed,
            Open,
            HalfOpen
        };
        
        int failureThreshold_;
        std::chrono::seconds timeout_;
        int consecutiveFailures_;
        std::chrono::steady_clock::time_point lastFailureTime_;
        CircuitState state_;
    };
}

} // namespace DesignPatterns

// Global error handling configuration
class ErrorHandler {
public:
    static void setLogLevel(int level) { logLevel_ = level; }
    static void setVerbose(bool verbose) { verbose_ = verbose; }
    static void setThrowOnError(bool throwOnError) { throwOnError_ = throwOnError; }
    
    static void log(const std::string& message, OperationResult status = OperationResult::Success) {
        if (verbose_ || status != OperationResult::Success) {
            std::cout << "[" << getStatusString(status) << "] " << message << std::endl;
        }
    }
    
    static void handleError(const std::string& message, const ErrorContext& context = {}) {
        if (throwOnError_) {
            throw ContextException(message, context);
        } else {
            std::cerr << "ERROR: " << message << std::endl;
        }
    }
    
private:
    static std::string getStatusString(OperationResult status) {
        switch (status) {
            case OperationResult::Success: return "SUCCESS";
            case OperationResult::Failure: return "FAILURE";
            case OperationResult::InvalidInput: return "INVALID_INPUT";
            case OperationResult::NotFound: return "NOT_FOUND";
            case OperationResult::OutOfMemory: return "OUT_OF_MEMORY";
            case OperationResult::Timeout: return "TIMEOUT";
            case OperationResult::PermissionDenied: return "PERMISSION_DENIED";
            case OperationResult::AlreadyExists: return "ALREADY_EXISTS";
            case OperationResult::ResourceInUse: return "RESOURCE_IN_USE";
            case OperationResult::SystemError: return "SYSTEM_ERROR";
            default: return "UNKNOWN";
        }
    }
    
    static inline int logLevel_;
    static inline bool verbose_;
    static inline bool throwOnError_;
};

} // namespace nlm
