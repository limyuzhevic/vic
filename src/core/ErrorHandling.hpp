// ============================================================================
// ERROR HANDLING AND EXCEPTION HIERARCHY
// ============================================================================

#pragma once

#include <system_error>
#include <string>
#include <memory>
#include <optional>
#include <format>

namespace nlm {

// Error codes for neural system
enum class NeuralErrorCode {
    // Configuration errors
    ConfigInvalid = 1,
    ConfigValidationFailed = 2,
    ConfigFileNotFound = 3,
    ConfigParseError = 4,
    
    // Brain initialization errors
    BrainInitializationFailed = 10,
    InvalidConfiguration = 11,
    NeuronCreationFailed = 12,
    SynapseCreationFailed = 13,
    
    // Simulation errors
    SimulationNotInitialized = 20,
    InvalidStepParameters = 21,
    SpikeSystemError = 22,
    PlasticityUpdateFailed = 23,
    
    // Memory system errors
    MemoryAllocationFailed = 30,
    MemoryConsolidationFailed = 31,
    MemoryReplayFailed = 32,
    
    // Plasticity errors
    PlasticityRuleError = 40,
    STDPUpdateFailed = 41,
    HebbianUpdateFailed = 42,
    
    // Performance errors
    SIMDNotAvailable = 50,
    MemoryPoolExhausted = 51,
    LockFreeOperationFailed = 52,
    
    // Internal errors
    InternalLogicError = 100,
    ResourceExhausted = 101,
    OperationCancelled = 102
};

// Custom exception hierarchy
class NeuralException : public std::runtime_error {
public:
    explicit NeuralException(const std::string& message)
        : std::runtime_error(message) {}
    
    NeuralException(NeuralErrorCode code, const std::string& message)
        : std::runtime_error(message), errorCode_(code) {}
    
    NeuralErrorCode getErrorCode() const { return errorCode_; }
    
    virtual const char* what() const noexcept override {
        return std::runtime_error::what();
    }
    
protected:
    NeuralErrorCode errorCode_ = NeuralErrorCode::InternalLogicError;
};

class ConfigException : public NeuralException {
public:
    explicit ConfigException(const std::string& message)
        : NeuralException(NeuralErrorCode::ConfigInvalid, message) {}
    
    ConfigException(NeuralErrorCode code, const std::string& message)
        : NeuralException(code, message) {}
};

class BrainException : public NeuralException {
public:
    explicit BrainException(const std::string& message)
        : NeuralException(NeuralErrorCode::BrainInitializationFailed, message) {}
    
    BrainException(NeuralErrorCode code, const std::string& message)
        : NeuralException(code, message) {}
};

class SimulationException : public NeuralException {
public:
    explicit SimulationException(const std::string& message)
        : NeuralException(NeuralErrorCode::SimulationNotInitialized, message) {}
    
    SimulationException(NeuralErrorCode code, const std::string& message)
        : NeuralException(code, message) {}
};

// Transaction-based operation support
class Transaction {
public:
    virtual ~Transaction() = default;
    virtual bool commit() = 0;
    virtual void rollback() = 0;
    virtual bool isActive() const = 0;
};

// Exception factory for consistent error creation
class ExceptionFactory {
public:
    static std::unique_ptr<NeuralException> createConfigError(
        NeuralErrorCode code, const std::string& message) {
        return std::make_unique<ConfigException>(code, message);
    }
    
    static std::unique_ptr<NeuralException> createBrainError(
        NeuralErrorCode code, const std::string& message) {
        return std::make_unique<BrainException>(code, message);
    }
    
    static std::unique_ptr<NeuralException> createSimulationError(
        NeuralErrorCode code, const std::string& message) {
        return std::make_unique<SimulationException>(code, message);
    }
    
    static std::string formatError(const std::string& format, std::format_args args) {
        return std::vformat(format, args);
    }
};

// Transaction manager for undo/redo operations
class TransactionManager {
public:
    static TransactionManager& getInstance() {
        static TransactionManager instance;
        return instance;
    }
    
    template<typename T>
    static bool executeWithTransaction(T&& operation) {
        TransactionManager& tm = getInstance();
        try {
            auto result = operation();
            if (result) {
                tm.commitCurrentTransaction();
                return true;
            }
            return false;
        } catch (const std::exception& e) {
            tm.rollbackCurrentTransaction();
            return false;
        }
    }
    
private:
    TransactionManager() = default;
    void commitCurrentTransaction() {}
    void rollbackCurrentTransaction() {}
};

// Error reporting interface
class ErrorReporter {
public:
    virtual ~ErrorReporter() = default;
    virtual void reportError(const NeuralException& error) = 0;
    virtual void reportWarning(const std::string& message) = 0;
};

} // namespace nlm
