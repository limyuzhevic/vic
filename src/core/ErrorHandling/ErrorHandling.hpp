// Error handling utilities for NLM
#ifndef NLM_ERROR_HANDLING_H
#define NLM_ERROR_HANDLING_H

#include <string>
#include <stdexcept>
#include <map>
#include <memory>
#include <sstream>
#include <functional>

namespace nlm {

// NLM Exception hierarchy
class NLMException : public std::runtime_error {
public:
    NLMException(const std::string& message) : std::runtime_error(message) {}
    virtual ~NLMException() = default;
};

class ConfigurationException : public NLMException {
public:
    ConfigurationException(const std::string& message) : NLMException(message) {}
};

class BrainInitializationException : public NLMException {
public:
    BrainInitializationException(const std::string& message) : NLMException(message) {}
};

class MemorySystemException : public NLMException {
public:
    MemorySystemException(const std::string& message) : NLMException(message) {}
};

class SimulationException : public NLMException {
public:
    SimulationException(const std::string& message) : NLMException(message) {}
};

class CheckpointException : public NLMException {
public:
    CheckpointException(const std::string& message) : NLMException(message) {}
};

// Error codes for different failure modes
enum class ErrorCode {
    Success = 0,
    InvalidArgument = 1,
    ConfigurationError = 2,
    InitializationError = 3,
    MemoryError = 4,
    SimulationError = 5,
    CheckpointError = 6,
    FileError = 7,
    ResourceExhausted = 8,
    NotImplemented = 9,
    UnknownError = 10
};

// Error context information
struct ErrorContext {
    ErrorCode code;
    std::string message;
    std::string file;
    int line;
    std::map<std::string, std::string> details;
    std::chrono::system_clock::time_point timestamp;
    
    ErrorContext(ErrorCode code, const std::string& message) 
        : code(code), message(message), line(0), timestamp(std::chrono::system_clock::now()) {}
    
    std::string getFormattedMessage() const {
        std::stringstream ss;
        ss << "Error " << static_cast<int>(code) << ": " << message;
        if (!file.empty()) {
            ss << " (at " << file << ":" << line << ")";
        }
        if (!details.empty()) {
            ss << " Details:";
            for (const auto& detail : details) {
                ss << " " << detail.first << "=" << detail.second;
            }
        }
        return ss.str();
    }
};

// Error handling and reporting utilities
class ErrorHandler {
public:
    using ErrorCallback = std::function<void(const ErrorContext&)>;
    
    static ErrorHandler& getInstance() {
        static ErrorHandler instance;
        return instance;
    }
    
    void setErrorCallback(ErrorCallback callback) {
        errorCallback_ = callback;
    }
    
    bool handleError(const ErrorContext& error) {
        if (errorCallback_) {
            errorCallback_(error);
        }
        
        // Check if error should be treated as fatal
        if (isFatalError(error.code)) {
            logError(error, true);
            return false; // Cannot recover
        } else {
            logError(error, false);
            return true;  // Can recover
        }
    }
    
    void logError(const ErrorContext& error, bool isFatal) {
        std::stringstream ss;
        ss << "[" << std::chrono::system_clock::now() << "] ";
        if (isFatal) {
            ss << "FATAL ";
        } else {
            ss << "ERROR ";
        }
        ss << error.getFormattedMessage();
        
        // Output to stderr
        std::cerr << ss.str() << std::endl;
        
        // Also log via Logger if available
        try {
            auto logger = Logger::getInstance();
            logger->logError(error.message);
        } catch (...) {
            // Logger not initialized, ignore
        }
    }
    
    void setFailFast(bool failFast) {
        failFast_ = failFast;
    }
    
    bool getFailFast() const {
        return failFast_;
    }
    
    void clear() {
        errorCount_ = 0;
        lastError_.reset();
    }
    
    size_t getErrorCount() const {
        return errorCount_;
    }
    
    std::optional<ErrorContext> getLastError() const {
        return lastError_;
    }
    
    void throwOnError() {
        if (lastError_) {
            throw NLMException(lastError_->getFormattedMessage());
        }
    }
    
    template<typename T>
    T throwOnError(const T& value) {
        if (lastError_) {
            throw NLMException(lastError_->getFormattedMessage());
        }
        return value;
    }
    
private:
    ErrorHandler() : errorCount_(0), failFast_(false), errorCallback_(nullptr) {}
    
    bool isFatalError(ErrorCode code) {
        switch (code) {
            case ErrorCode::InvalidArgument:
            case ErrorCode::ConfigurationError:
            case ErrorCode::InitializationError:
            case ErrorCode::MemoryError:
                return true;
            default:
                return false;
        }
    }
    
    size_t errorCount_;
    std::optional<ErrorContext> lastError_;
    bool failFast_;
    ErrorCallback errorCallback_;
};

// Macro for easy error handling in NLM code
#define NLM_THROW_ERROR(code, message) \
    do { \
        ErrorContext error(code, message); \
        error.file = __FILE__; \
        error.line = __LINE__; \
        if (!nlm::ErrorHandler::getInstance().handleError(error)) { \
            throw nlm::NLMException(error.getFormattedMessage()); \
        } \
    } while(0)

#define NLM_VALIDATE(condition, code, message) \
    do { \
        if (!(condition)) { \
            NLM_THROW_ERROR(code, message); \
        } \
    } while(0)

// Memory management utilities
class MemoryManager {
public:
    static MemoryManager& getInstance() {
        static MemoryManager instance;
        return instance;
    }
    
    template<typename T>
    std::unique_ptr<T> allocateUnique(const std::string& name = "") {
        auto ptr = std::make_unique<T>();
        recordAllocation(ptr.get(), sizeof(T), name);
        return ptr;
    }
    
    template<typename T, typename... Args>
    std::unique_ptr<T> allocateUnique(Args&&... args) {
        auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
        recordAllocation(ptr.get(), sizeof(T), "");
        return ptr;
    }
    
    void deallocate(void* ptr) {
        recordDeallocation(ptr);
        std::free(ptr);
    }
    
    void* allocate(size_t size, const std::string& name = "") {
        void* ptr = std::malloc(size);
        if (!ptr && size > 0) {
            NLM_THROW_ERROR(ErrorCode::ResourceExhausted, 
                          "Failed to allocate " + std::to_string(size) + " bytes");
        }
        recordAllocation(ptr, size, name);
        return ptr;
    }
    
    void free(void* ptr) {
        if (ptr) {
            recordDeallocation(ptr);
            std::free(ptr);
        }
    }
    
    void recordAllocation(void* ptr, size_t size, const std::string& name) {
        // Record for memory leak detection
        auto& stats = getInstance().stats_;
        stats.totalAllocated += size;
        stats.allocationCount++;
        if (!name.empty()) {
            stats.allocationsByName[name]++;
        }
    }
    
    void recordDeallocation(void* ptr) {
        // In a real implementation, this would track the allocation
        auto& stats = getInstance().stats_;
        stats.deallocationCount++;
    }
    
    struct MemoryStats {
        size_t totalAllocated = 0;
        size_t allocationCount = 0;
        size_t deallocationCount = 0;
        std::map<std::string, size_t> allocationsByName;
        
        size_t getCurrentUsage() const {
            // Estimate current usage (would need more sophisticated tracking)
            return totalAllocated * 0.8; // Simplified estimate
        }
        
        std::string getReport() const {
            std::stringstream ss;
            ss << "Memory Statistics:" << std::endl;
            ss << "  Total allocated: " << (totalAllocated / 1024) << " KB" << std::endl;
            ss << "  Allocation count: " << allocationCount << std::endl;
            ss << "  Deallocation count: " << deallocationCount << std::endl;
            ss << "  Current estimated usage: " << (getCurrentUsage() / 1024) << " KB" << std::endl;
            
            if (!allocationsByName.empty()) {
                ss << "  Allocations by category:" << std::endl;
                for (const auto& pair : allocationsByName) {
                    ss << "    " << pair.first << ": " << pair.second << std::endl;
                }
            }
            
            return ss.str();
        }
    };
    
    MemoryStats getMemoryStats() const {
        return stats_;
    }
    
    void logMemoryStats() {
        std::cout << getMemoryStats().getReport() << std::endl;
    }
    
    void checkMemoryLeaks() {
        auto stats = getMemoryStats();
        if (stats.allocationCount != stats.deallocationCount) {
            std::cerr << "WARNING: Possible memory leak detected!" << std::endl;
            std::cerr << "  Allocations: " << stats.allocationCount << std::endl;
            std::cerr << "  Deallocations: " << stats.deallocationCount << std::endl;
        }
    }
    
private:
    MemoryManager() {}
    
    MemoryStats stats_;
};

// Resource management with automatic cleanup
class ScopedResource {
public:
    template<typename T, typename Deleter>
    ScopedResource(T* ptr, Deleter deleter) : ptr_(ptr), deleter_(deleter) {}
    
    ~ScopedResource() {
        if (ptr_) {
            deleter_(ptr_);
        }
    }
    
    T* get() const { return ptr_; }
    T* release() {
        T* temp = ptr_;
        ptr_ = nullptr;
        return temp;
    }
    
    void reset() {
        if (ptr_) {
            deleter_(ptr_);
            ptr_ = nullptr;
        }
    }
    
private:
    T* ptr_;
    std::function<void(T*)> deleter_;
};

// Parameter validation utilities
class ParameterValidator {
public:
    static bool validate(const std::string& name, const std::string& value, 
                        const std::string& type, bool required = true) {
        if (required && value.empty()) {
            return false;
        }
        
        if (type == "integer") {
            try {
                std::stoll(value);
                return true;
            } catch (...) {
                return false;
            }
        } else if (type == "float") {
            try {
                std::stof(value);
                return true;
            } catch (...) {
                return false;
            }
        } else if (type == "positive") {
            if (value.empty()) return false;
            try {
                double val = std::stod(value);
                return val > 0.0;
            } catch (...) {
                return false;
            }
        } else if (type == "range") {
            // Parse range like "1-10"
            size_t dash = value.find('-');
            if (dash == std::string::npos) return false;
            try {
                double min = std::stod(value.substr(0, dash));
                double max = std::stod(value.substr(dash + 1));
                return min < max;
            } catch (...) {
                return false;
            }
        }
        
        return true; // Unknown type, assume valid
    }
    
    static std::string getValidationError(const std::string& name, 
                                          const std::string& value,
                                          const std::string& type) {
        if (value.empty()) {
            return "Parameter '" + name + "' is required";
        }
        
        if (type == "integer") {
            try {
                std::stoll(value);
                return "";
            } catch (...) {
                return "Parameter '" + name + "' must be an integer";
            }
        } else if (type == "float") {
            try {
                std::stof(value);
                return "";
            } catch (...) {
                return "Parameter '" + name + "' must be a float";
            }
        } else if (type == "positive") {
            try {
                double val = std::stod(value);
                if (val <= 0.0) {
                    return "Parameter '" + name + "' must be positive";
                }
            } catch (...) {
                return "Parameter '" + name + "' must be a number";
            }
        }
        
        return "";
    }
};

// File I/O utilities with error handling
class FileIO {
public:
    static bool fileExists(const std::string& path) {
        std::ifstream file(path);
        return file.good();
    }
    
    static std::string readFile(const std::string& path) {
        std::ifstream file(path);
        if (!file) {
            NLM_THROW_ERROR(ErrorCode::FileError, "Cannot open file: " + path);
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
    
    static bool writeFile(const std::string& path, const std::string& content) {
        std::ofstream file(path);
        if (!file) {
            NLM_THROW_ERROR(ErrorCode::FileError, "Cannot write to file: " + path);
        }
        
        file << content;
        return file.good();
    }
    
    static bool ensureDirectory(const std::string& path) {
        size_t pos = 0;
        while ((pos = path.find('/', pos)) != std::string::npos) {
            std::string dir = path.substr(0, pos++);
            struct stat st;
            if (stat(dir.c_str(), &st) != 0) {
                if (mkdir(dir.c_str(), 0755) != 0) {
                    return false;
                }
            }
        }
        return true;
    }
};

} // namespace nlm

#endif // NLM_ERROR_HANDLING_H
