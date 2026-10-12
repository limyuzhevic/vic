#pragma once

#include <string>
#include <stdexcept>
#include <sstream>
#include <vector>
#include <memory>
#include <chrono>
#include "Types.hpp"

namespace nlm {

// Base exception class for all NLM errors
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) 
        : std::runtime_error(message), timestamp(std::chrono::system_clock::now()) {}
    
    std::chrono::system_clock::time_point timestamp;
    std::string errorCode;
    std::string context;
};

// Configuration-related errors
class ConfigError : public NLMException {
public:
    enum class Type {
        InvalidFile,
        InvalidFormat,
        MissingRequiredKey,
        InvalidValue,
        FileNotFound,
        PermissionDenied,
        CorruptedData
    };
    
    ConfigError(const std::string& message, Type type, const std::string& filepath = "")
        : NLMException(message), errorType(type), filePath(filepath) {}
    
    Type errorType;
    std::string filePath;
};

// Memory-related errors
class MemoryError : public NLMException {
public:
    enum class Type {
        AllocationFailure,
        OutOfMemory,
        BufferOverflow,
        InvalidMemoryAccess,
        NullPointerDereference,
        MemoryLeak,
        CorruptedMemory
    };
    
    MemoryError(const std::string& message, Type type)
        : NLMException(message), errorType(type) {}
    
    Type errorType;
};

// Simulation-related errors
class SimulationError : public NLMException {
public:
    enum class Type {
        InvalidStep,
        NegativeTime,
        ExceededMaxSteps,
        NumericalInstability,
        NeuronMalformed,
        SynapseMalformed,
        InvalidRegion,
        NetworkDisconnected,
        InvalidConfiguration
    };
    
    SimulationError(const std::string& message, Type type, SimulationStep step = 0)
        : NLMException(message), errorType(type), simulationStep(step) {}
    
    Type errorType;
    SimulationStep simulationStep;
};

// Random number generation errors
class RandomError : public NLMException {
public:
    enum class Type {
        InvalidSeed,
        InvalidRange,
        DivisionByZero,
        OutOfBounds
    };
    
    RandomError(const std::string& message, Type type)
        : NLMException(message), errorType(type) {}
    
    Type errorType;
};

// File operation errors
class FileError : public NLMException {
public:
    enum class Type {
        FileNotFound,
        PermissionDenied,
        DiskFull,
        InvalidFormat,
        ReadError,
        WriteError,
        CorruptedFile
    };
    
    FileError(const std::string& message, Type type, const std::string& filepath = "")
        : NLMException(message), errorType(type), filePath(filepath) {}
    
    Type errorType;
    std::string filePath;
};

// Exception factory class for creating and logging errors
class ExceptionManager {
public:
    static ExceptionManager& getInstance() {
        static ExceptionManager instance;
        return instance;
    }
    
    template<typename T, typename... Args>
    static void throwError(Args&&... args) {
        T exception(std::forward<Args>(args)...);
        logError(exception);
        throw exception;
    }
    
    template<typename T, typename... Args>
    static bool tryThrowError(Args&&... args) noexcept {
        try {
            throwError<T>(std::forward<Args>(args)...);
            return false;
        } catch (const NLMException&) {
            return true;
        } catch (...) {
            return false;
        }
    }
    
    static void logError(const NLMException& error) {
        std::ostringstream oss;
        oss << "[ERROR " << error.timestamp.time_since_epoch().count() << "] " << error.what();
        if (!error.errorCode.empty()) oss << " (code: " << error.errorCode << ")";
        if (!error.context.empty()) oss << " (context: " << error.context << ")";
        
        Logger::getGlobal().error(oss.str(), error.filePath.empty() ? "" : error.filePath.c_str());
    }
    
    static void setErrorHandler(std::function<void(const NLMException&)> handler) {
        errorHandler = handler;
    }
    
    static void clearErrorHandler() {
        errorHandler = nullptr;
    }
    
private:
    ExceptionManager() = default;
    
    static std::function<void(const NLMException&)> errorHandler;
};

// Validation helper functions
namespace Validation {
    inline bool isValidNeuronId(const NeuronId& id) {
        return id.value != UINT64_MAX && id.value != 0;
    }
    
    inline bool isValidSynapseId(const SynapseId& id) {
        return id.value != UINT64_MAX && id.value != 0;
    }
    
    inline bool isValidRegionId(const RegionId& id) {
        return id.value != UINT64_MAX && id.value != 0;
    }
    
    inline bool isValidPopulationId(const PopulationId& id) {
        return id.value != UINT64_MAX && id.value != 0;
    }
    
    inline bool isValidWeight(float weight) {
        return !std::isnan(weight) && !std::isinf(weight);
    }
    
    inline bool isValidMembranePotential(float potential) {
        return !std::isnan(potential) && !std::isinf(potential) && 
               potential >= -200.0f && potential <= 200.0f;
    }
    
    inline bool isValidTimestamp(Timestamp time) {
        return !std::isnan(time) && !std::isinf(time) && time >= 0.0;
    }
    
    inline bool isValidTimestepDuration(TimestepDuration dt) {
        return !std::isnan(dt) && !std::isinf(dt) && dt > 0.0;
    }
    
    inline bool isValidSeed(uint64_t seed) {
        return seed != 0;
    }
    
    template<typename T>
    inline bool isValidIndex(T index, size_t max) {
        return index < max;
    }
    
    template<typename Container>
    inline bool isNotEmpty(const Container& container) {
        return !container.empty();
    }
    
    inline bool isFilePathValid(const std::string& path) {
        return !path.empty() && path.find('\0') == std::string::npos;
    }
}

} // namespace nlm
