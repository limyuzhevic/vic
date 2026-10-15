#pragma once

#include "Errors.hpp"
#include <string>
#include <stdexcept>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <cstring>
#include <regex>
#include <limits>

namespace nlm {

// Error handling macros

#define CHECK_CONDITION(condition, error_message, error_type, ...) \
    do { \
        if (!(condition)) { \
            std::string file = __FILE__; \
            std::string func = __func__; \
            int line = __LINE__; \
            switch (error_type) { \
                case ErrorType::Config: \
                    throw ConfigError(error_message, ConfigError::ErrorType::##__VA_ARGS__, file, line, func); \
                case ErrorType::File: \
                    throw FileError(error_message, FileError::ErrorType::##__VA_ARGS__, file, line, func); \
                case ErrorType::Memory: \
                    throw MemoryError(error_message, MemoryError::ErrorType::##__VA_ARGS__, file, line, func); \
                case ErrorType::Validation: \
                    throw ValidationError(error_message, ValidationError::ErrorType::##__VA_ARGS__, file, line, func); \
                case ErrorType::Network: \
                    throw NetworkError(error_message, NetworkError::ErrorType::##__VA_ARGS__, file, line, func); \
                case ErrorType::Performance: \
                    throw PerformanceError(error_message, PerformanceError::ErrorType::##__VA_ARGS__, file, line, func); \
                default: \
                    throw NLMBasicError(error_message, file, line, func); \
            } \
        } \
    } while(0)

#define CHECK_CONDITION_SIMPLE(condition, error_message) \
    if (!(condition)) { \
        throw ValidationError(error_message); \
    }

#define CHECK_FILE_EXISTS(path) \
    do { \
        if (!fileExists(path)) { \
            throw FileError("File not found: " + std::string(path), \
                          FileError::ErrorType::FileNotFound, \
                          __FILE__, __LINE__, __func__); \
        } \
    } while(0)

#define SAFE_ALLOC(ptr, size, type) \
    do { \
        (ptr) = new (std::nothrow) type[size]; \
        if (!(ptr)) { \
            throw MemoryError("Memory allocation failed: " #size " bytes of type " #type, \
                            MemoryError::ErrorType::AllocationFailed, \
                            __FILE__, __LINE__, __func__); \
        } \
    } while(0)

#define SAFE_DELETE(ptr) \
    do { \
        if ((ptr)) { \
            delete (ptr); \
            (ptr) = nullptr; \
        } \
    } while(0)

#define SAFE_DELETE_ARRAY(ptr) \
    do { \
        if ((ptr)) { \
            delete[] (ptr); \
            (ptr) = nullptr; \
        } \
    } while(0)

// Enum for error types (used by macros)
enum class ErrorType {
    Config,
    File,
    Memory,
    Validation,
    Network,
    Performance
};

// File operations with error handling
bool fileExists(const std::string& path);
std::string getFileExtension(const std::string& path);
bool isPathTraversal(const std::string& path);
std::string sanitizePath(const std::string& path);

// Validation functions
bool validateFilePath(const std::string& path);
bool validateConfigValue(const std::string& key, const std::string& value);
bool validateMemorySize(size_t size);
bool validateNeuronCount(size_t count);
bool validateLearningRate(float rate);
bool validateConnectionProbability(float prob);
bool validateTimeStep(double timestep);
bool validateLearningRateRange(float rate);
bool validateNeuronId(const NeuronId& id);
bool validateSynapseId(const SynapseId& id);
bool validateConfigKey(const std::string& key);
bool validateConfigValueType(const std::string& key, const ConfigValue& value);

// Configuration validation
bool validateConfig(const std::string& key, const std::string& value, ConfigSource source);

// Memory allocation safety
void* safeAlloc(size_t size);
template<typename T>
T* safeAllocTyped(size_t count);

// Error handling utilities
std::string getErrorMessage(const NLMBasicError& e);
void logError(const NLMBasicError& e);
void logWarning(const std::string& message);
void logInfo(const std::string& message);

// Exception-safe resource management
class AutoResource {
public:
    virtual ~AutoResource() {}
    virtual void cleanup() = 0;
};

// Template for exception-safe resource management
// Usage: AutoResourcePtr<Neuron> neuronPtr(new Neuron(...));
template<typename T>
class AutoResourcePtr {
public:
    explicit AutoResourcePtr(T* ptr = nullptr) : ptr_(ptr) {}
    ~AutoResourcePtr() { cleanup(); }
    
    AutoResourcePtr(const AutoResourcePtr&) = delete;
    AutoResourcePtr& operator=(const AutoResourcePtr&) = delete;
    
    AutoResourcePtr(AutoResourcePtr&& other) noexcept : ptr_(other.ptr_) {
        other.ptr_ = nullptr;
    }
    
    AutoResourcePtr& operator=(AutoResourcePtr&& other) noexcept {
        if (this != &other) {
            cleanup();
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
        }
        return *this;
    }
    
    T* get() const { return ptr_; }
    T* release() {
        T* temp = ptr_;
        ptr_ = nullptr;
        return temp;
    }
    
    void reset(T* ptr = nullptr) {
        if (ptr != ptr_) {
            cleanup();
            ptr_ = ptr;
        }
    }
    
    T& operator*() const { return *ptr_; }
    T* operator->() const { return ptr_; }
    
    explicit operator bool() const { return ptr_ != nullptr; }
    
private:
    void cleanup() {
        if (ptr_) {
            static_cast<AutoResource*>(ptr_)->cleanup();
            delete ptr_;
            ptr_ = nullptr;
        }
    }
    
    T* ptr_;
};

// Exception-safe file operations
class SafeFile {
public:
    enum class Mode { Read, Write, Append };
    
    SafeFile() : file_(nullptr), path_("") {}
    SafeFile(const std::string& path, Mode mode);
    ~SafeFile() { close(); }
    
    SafeFile(const SafeFile&) = delete;
    SafeFile& operator=(const SafeFile&) = delete;
    
    SafeFile(SafeFile&& other) noexcept : file_(other.file_), path_(other.path_) {
        other.file_ = nullptr;
    }
    
    SafeFile& operator=(SafeFile&& other) noexcept {
        if (this != &other) {
            close();
            file_ = other.file_;
            path_ = other.path_;
            other.file_ = nullptr;
        }
        return *this;
    }
    
    bool isOpen() const { return file_ != nullptr; }
    const std::string& getPath() const { return path_; }
    
    std::string readLine();
    bool readLine(std::string& line);
    bool writeLine(const std::string& line);
    bool write(const std::string& data);
    size_t write(const void* data, size_t size);
    size_t read(void* data, size_t size);
    bool seek(long offset, int whence = SEEK_SET);
    long tell();
    bool close();
    
    static SafeFile createBackup(const std::string& path);
    static SafeFile openForRead(const std::string& path);
    static SafeFile openForWrite(const std::string& path, bool createBackup = false);
    
private:
    std::FILE* file_;
    std::string path_;
};

// Atomic file operations
bool atomicWrite(const std::string& path, const std::string& content);
bool atomicWrite(const std::string& path, const std::vector<char>& data);
std::string atomicRead(const std::string& path);

} // namespace nlm
