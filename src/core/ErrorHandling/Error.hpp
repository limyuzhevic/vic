namespace nlm {

// Error handling hierarchy
class NLMError : public std::exception {
protected:
    std::string message_;
    std::string context_;
    int code_;
    std::string sourceFile_;
    int sourceLine_;
    std::chrono::system_clock::time_point timestamp_;

public:
    NLMError(const std::string& message, const std::string& context = "", 
             int code = 0, const std::string& file = "", int line = 0)
        : message_(message), context_(context), code_(code),
          sourceFile_(file), sourceLine_(line), timestamp_(std::chrono::system_clock::now()) {}

    const char* what() const noexcept override {
        std::ostringstream oss;
        oss << "[NLM Error " << code_ << "] " << message_;
        if (!context_.empty()) oss << " (Context: " << context_ << ")";
        if (!sourceFile_.empty()) oss << " at " << sourceFile_;
        if (sourceLine_ > 0) oss << ":" << sourceLine_;
        cachedMessage_ = oss.str();
        return cachedMessage_.c_str();
    }

    std::string getMessage() const { return message_; }
    std::string getContext() const { return context_; }
    int getCode() const { return code_; }
    std::string getSourceFile() const { return sourceFile_; }
    int getSourceLine() const { return sourceLine_; }
    std::chrono::system_clock::time_point getTimestamp() const { return timestamp_; }
    std::string getFormattedTimestamp() const {
        auto time = std::chrono::system_clock::to_time_t(timestamp_);
        char buf[100];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&time));
        return buf;
    }

    virtual ~NLMError() = default;

private:
    mutable std::string cachedMessage_;
};

// Specific error types
class ConfigError : public NLMError {
public:
    enum class Code {
        InvalidKey = 1000,
        MissingRequiredValue = 1001,
        TypeMismatch = 1002,
        FileNotFound = 1003,
        ParseError =1004,
        ValidationFailed = 1005,
        CircularDependency = 1006
    };

    ConfigError(const std::string& message, const std::string& context = "", Code code = Code::ValidationFailed)
        : NLMError(message, context, static_cast<int>(code)) {}
};

class MemoryError : public NLMError {
public:
    enum class Code {
        AllocationFailed = 2000,
        InvalidPointer = 2001,
        BufferOverflow = 2002,
        ResourceExhausted = 2003,
        NotInitialized = 2004
    };

    MemoryError(const std::string& message, const std::string& context = "", Code code = Code::AllocationFailed)
        : NLMError(message, context, static_cast<int>(code)) {}
};

class BrainError : public NLMError {
public:
    enum class Code {
        InitializationFailed = 3000,
        SimulationError = 3001,
        InvalidConfiguration =3002,
        MemorySystemError =3003,
        IntegrationError = 3004
    };

    BrainError(const std::string& message, const std::string& context = "", Code code = Code::InitializationFailed)
        : NLMError(message, context, static_cast<int>(code)) {}
};

class LoggerError : public NLMError {
public:
    enum class Code {
        WriteFailed = 4000,
        FileError =4001,
        InitializationFailed = 4002
    };

    LoggerError(const std::string& message, const std::string& context = "", Code code = Code::WriteFailed)
        : NLMError(message, context, static_cast<int>(code)) {}
};

class WorldError : public NLMError {
public:
    enum class Code {
        BoundaryViolation = 5000,
        CollisionError = 5001,
        InvalidState = 5002,
        SimulationStepError = 5003
    };

    WorldError(const std::string& message, const std::string& context = "", Code code = Code::BoundaryViolation)
        : NLMError(message, context, static_cast<int>(code)) {}
};

class ValidationError : public NLMError {
public:
    enum class Code {
        InvalidInput = 6000,
        RangeViolation = 6001,
        MissingDependency = 6002,
        CrossValidationFailed = 6003
    };

    ValidationError(const std::string& message, const std::string& context = "", Code code = Code::InvalidInput)
        : NLMError(message, context, static_cast<int>(code)) {}
};

// Exception translator for thread-safe logging
void translateNLMException(const NLMError& e);

// Error context manager for tracking operation context
class ErrorContext {
public:
    static ErrorContext& getInstance() {
        static ErrorContext instance;
        return instance;
    }

    void pushContext(const std::string& operation) {
        std::lock_guard<std::mutex> lock(mutex_);
        contextStack_.push_back(operation);
    }

    void popContext() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!contextStack_.empty()) {
            contextStack_.pop_back();
        }
    }

    std::string getCurrentContext() const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (contextStack_.empty()) {
            return "";
        }
        std::ostringstream oss;
        for (size_t i = 0; i < contextStack_.size(); ++i) {
            if (i > 0) oss << ">";
            oss << contextStack_[i];
        }
        return oss.str();
    }

    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        contextStack_.clear();
    }

private:
    ErrorContext() = default;
    ~ErrorContext() = default;
    ErrorContext(const ErrorContext&) = delete;
    ErrorContext& operator=(const ErrorContext&) = delete;

    std::vector<std::string> contextStack_;
    mutable std::mutex mutex_;
};

// Enhanced error reporting interface
class ErrorReporter {
public:
    virtual ~ErrorReporter() = default;
    virtual void reportError(const NLMError& error) = 0;
    virtual void reportWarning(const std::string& message, const std::string& context = "") = 0;
};

// Global error reporter
ErrorReporter& getErrorReporter();
void setErrorReporter(std::unique_ptr<ErrorReporter> reporter);

// Thread-safe error handling macros
#define NLM_THROW_ERROR(type, message, context, code) \
    do { \
        std::string ctx = nlm::ErrorContext::getInstance().getCurrentContext(); \
        if (!ctx.empty()) ctx = ctx + " -> " + context; else ctx = context; \
        nlm::getErrorReporter().reportError(type(message, ctx, code, __FILE__, __LINE__)); \
        throw type(message, ctx, code, __FILE__, __LINE__); \
    } while(0)

#define NLM_THROW_NLM_ERROR(message, context, code) \
    NLM_THROW_ERROR(nlm::NLMError, message, context, code)

#define NLM_THROW_CONFIG_ERROR(message, context, code) \
    NLM_THROW_ERROR(nlm::ConfigError, message, context, code)

#define NLM_THROW_MEMORY_ERROR(message, context, code) \
    NLM_THROW_ERROR(nlm::MemoryError, message, context, code)

#define NLM_THROW_BRAIN_ERROR(message, context, code) \
    NLM_THROW_ERROR(nlm::BrainError, message, context, code)

#define NLM_THROW_LOGGER_ERROR(message, context, code) \
    NLM_THROW_ERROR(nlm::LoggerError, message, context, code)

#define NLM_THROW_WORLD_ERROR(message, context, code) \
    NLM_THROW_ERROR(nlm::WorldError, message, context, code)

#define NLM_THROW_VALIDATION_ERROR(message, context, code) \
    NLM_THROW_ERROR(nlm::ValidationError, message, context, code)

#define NLM_LOG_AND_THROW_ERROR(type, message, context, code) \
    do { \
        std::string ctx = nlm::ErrorContext::getInstance().getCurrentContext(); \
        if (!ctx.empty()) ctx = ctx + " -> " + context; else ctx = context; \
        nlm::getErrorReporter().reportError(type(message, ctx, code, __FILE__, __LINE__)); \
        NLM_LOG_ERROR(message); \
        throw type(message, ctx, code, __FILE__, __LINE__); \
    } while(0)

// Safe pointer wrapper with null checking
template<typename T>
class SafePtr {
public:
    SafePtr(T* ptr = nullptr, const std::string& operation = "") : ptr_(ptr), operation_(operation) {}
    ~SafePtr() = default;

    T* get() const { return ptr_; }
    T* operator->() const { 
        if (!ptr_) {
            throw NLMError("Null pointer access", operation_ + " -> SafePtr dereference"); 
        }
        return ptr_; 
    }
    T& operator*() const { 
        if (!ptr_) {
            throw NLMError("Null pointer dereference", operation_ + " -> SafePtr dereference"); 
        }
        return *ptr_; 
    }
    operator bool() const { return ptr_ != nullptr; }
    bool operator!() const { return ptr_ == nullptr; }

private:
    T* ptr_;
    std::string operation_;
};

// Memory allocation wrapper
void* safe_malloc(size_t size, const std::string& context = "");
void safe_free(void* ptr, const std::string& context = "");

template<typename T, typename... Args>
T* safe_new(Args&&... args) {
    void* ptr = std::malloc(sizeof(T));
    if (!ptr) {
        throw MemoryError("Memory allocation failed", "safe_new");
    }
    try {
        return new(ptr) T(std::forward<Args>(args)...);
    } catch (...) {
        std::free(ptr);
        throw;
    }
}

template<typename T>
void safe_delete(T* ptr, const std::string& context = "") {
    if (ptr) {
        ptr->~T();
        std::free(ptr);
    }
}

// Input validation utilities
class InputValidator {
public:
    static bool isPositive(const std::string& key, float value);
    static bool isWithinRange(const std::string& key, float value, float min, float max);
    static bool isNonEmpty(const std::string& key, const std::string& value);
    static bool isValidIndex(const std::string& key, size_t index, size_t maxSize);
    static bool isFinite(const std::string& key, double value);
    static bool matchesPattern(const std::string& key, const std::string& value, const std::string& pattern);
};

// Error recovery utilities
class ErrorRecovery {
public:
    static bool attemptRecovery(const std::string& operation, float maxRetries = 3.0f);
    static void setRetryDelay(double seconds);
    static bool isOperationCritical(const std::string& operation);
};

// Thread-safe error handling
class ThreadSafeErrorHandler {
public:
    static void setThreadSafetyEnabled(bool enabled);
    static bool isThreadSafetyEnabled();
    static void lockForErrorHandling();
    static void unlockFromErrorHandling();
    static std::mutex& getErrorMutex();
};

// Error reporting utilities
void logError(const NLMError& error);
void logWarning(const std::string& message, const std::string& context = "");

} // namespace nlm