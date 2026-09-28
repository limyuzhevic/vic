#pragma once

#include <string>
#include <stdexcept>
#include <cstdint>

namespace nlm {

// Error code hierarchy
enum class ErrorCode : uint32_t {
    // Success
    None = 0,
    
    // General errors
    Unknown = 1,
    NotImplemented = 2,
    InvalidArgument = 3,
    OutOfMemory = 4,
    FileNotFound = 5,
    FileError = 6,
    PermissionDenied = 7,
    
    // Configuration errors
    ConfigParseError = 100,
    ConfigInvalidKey = 101,
    ConfigMissingRequired = 102,
    
    // Memory system errors
    MemoryCapacityExceeded = 200,
    MemoryCorruption = 201,
    MemoryAccessOutOfBounds = 202,
    
    // Neural system errors
    NeuronInvalidState = 300,
    SynapseError = 301,
    RegionError = 302,
    PopulationError = 303,
    
    // Simulation errors
    SimulationNotInitialized = 400,
    SimulationAlreadyRunning = 401,
    InvalidTimestep = 402,
    SimulationStepError = 403,
    
    // Memory system integration errors
    MemoryIntegrationError = 500,
    WorkingMemoryError = 501,
    EpisodicMemoryError = 502,
    SemanticMemoryError = 503,
    ProceduralMemoryError = 504,
    AssociativeMemoryError = 505,
    
    // Neuromodulation errors
    NeuromodulationError = 600,
    DopamineError = 601,
    CuriosityError = 602,
    NoveltyError = 603,
    PredictionError = 604,
    
    // Cognition errors
    CognitionError = 700,
    PlannerError = 701,
    ConceptFormationError = 702,
    AttentionError = 703,
    
    // World/simulation errors
    WorldError = 800,
    SimpleWorldError = 801,
    EnvironmentError = 802,
    ActionError = 803,
    
    // Development errors
    DevelopmentError = 900,
    SynaptogenesisError = 901,
    PruningError = 902,
    MaturationError = 903,
    
    // Performance errors
    PerformanceError = 1000,
    MemoryPoolError = 1001,
    EventQueueError = 1002,
    
    // Checkpoint/persistence errors
    CheckpointError = 1100,
    SaveError = 1101,
    LoadError = 1102,
    
    // Experiment errors
    ExperimentError = 1200,
    PhaseError = 1201,
    MetricsError = 1202,
    
    // System initialization errors
    InitializationError = 1300,
    ComponentInitializationError = 1301,
    DependencyError = 1302,
    
    // External dependency errors
    ExternalDependencyError = 1400,
    PythonBindingError = 1401,
    VisualizationError = 1402,
};

// Base NLM exception class
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message, ErrorCode code = ErrorCode::Unknown)
        : std::runtime_error(message), code_(code) {}
    
    virtual ~NLMError() = default;
    
    ErrorCode getCode() const { return code_; }
    
protected:
    ErrorCode code_;
};

// Specific exception types

class ConfigError : public NLMError {
public:
    explicit ConfigError(const std::string& message, ErrorCode code = ErrorCode::ConfigParseError)
        : NLMError(message, code) {}
};

class MemoryError : public NLMError {
public:
    explicit MemoryError(const std::string& message, ErrorCode code = ErrorCode::MemoryCapacityExceeded)
        : NLMError(message, code) {}
};

class WorkingMemoryError : public MemoryError {
public:
    explicit WorkingMemoryError(const std::string& message, ErrorCode code = ErrorCode::WorkingMemoryError)
        : MemoryError(message, code) {}
};

class EpisodicMemoryError : public MemoryError {
public:
    explicit EpisodicMemoryError(const std::string& message, ErrorCode code = ErrorCode::EpisodicMemoryError)
        : MemoryError(message, code) {}
};

class NeuronError : public NLMError {
public:
    explicit NeuronError(const std::string& message, ErrorCode code = ErrorCode::NeuronInvalidState)
        : NLMError(message, code) {}
};

class SynapseError : public NLMError {
public:
    explicit SynapseError(const std::string& message, ErrorCode code = ErrorCode::SynapseError)
        : NLMError(message, code) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& message, ErrorCode code = ErrorCode::SimulationNotInitialized)
        : NLMError(message, code) {}
};

class NeuromodulationError : public NLMError {
public:
    explicit NeuromodulationError(const std::string& message, ErrorCode code = ErrorCode::NeuromodulationError)
        : NLMError(message, code) {}
};

class DopamineError : public NeuromodulationError {
public:
    explicit DopamineError(const std::string& message, ErrorCode code = ErrorCode::DopamineError)
        : NeuromodulationError(message, code) {}
};

class CuriosityError : public NeuromodulationError {
public:
    explicit CuriosityError(const std::string& message, ErrorCode code = ErrorCode::CuriosityError)
        : NeuromodulationError(message, code) {}
};

class CognitionError : public NLMError {
public:
    explicit CognitionError(const std::string& message, ErrorCode code = ErrorCode::CognitionError)
        : NLMError(message, code) {}
};

class WorldError : public NLMError {
public:
    explicit WorldError(const std::string& message, ErrorCode code = ErrorCode::WorldError)
        : NLMError(message, code) {}
};

class DevelopmentError : public NLMError {
public:
    explicit DevelopmentError(const std::string& message, ErrorCode code = ErrorCode::DevelopmentError)
        : NLMError(message, code) {}
};

class CheckpointError : public NLMError {
public:
    explicit CheckpointError(const std::string& message, ErrorCode code = ErrorCode::CheckpointError)
        : NLMError(message, code) {}
};

class PerformanceError : public NLMError {
public:
    explicit PerformanceError(const std::string& message, ErrorCode code = ErrorCode::PerformanceError)
        : NLMError(message, code) {}
};

// Template function for error creation

template<typename T, typename... Args>
T makeError(const std::string& message, Args&&... args) {
    return T(message + std::vformat(" (Details: {})", std::make_format_args(std::forward<Args>(args)...));
}

// Exception safety macros

#define NLM_TRY noexcept(true)
#define NLM_THROW(exc_type, msg) throw exc_type(msg)
#define NLM_THROW_WITH_CODE(exc_type, msg, code) throw exc_type(msg, code)

// Error checking macros

#define NLM_CHECK(condition, exc_type, msg) \
    do { \
        if (!(condition)) { \
            throw exc_type(msg); \
        } \
    } while(0)

#define NLM_CHECK_WITH_CODE(condition, exc_type, msg, code) \
    do { \
        if (!(condition)) { \
            throw exc_type(msg, code); \
        } \
    } while(0)

// Error handling utility functions

inline std::string errorCodeToString(ErrorCode code) {
    switch (code) {
        case ErrorCode::None: return "None";
        case ErrorCode::Unknown: return "Unknown";
        case ErrorCode::NotImplemented: return "NotImplemented";
        case ErrorCode::InvalidArgument: return "InvalidArgument";
        case ErrorCode::OutOfMemory: return "OutOfMemory";
        case ErrorCode::FileNotFound: return "FileNotFound";
        case ErrorCode::ConfigParseError: return "ConfigParseError";
        case ErrorCode::MemoryCapacityExceeded: return "MemoryCapacityExceeded";
        case ErrorCode::NeuronInvalidState: return "NeuronInvalidState";
        case ErrorCode::SimulationNotInitialized: return "SimulationNotInitialized";
        case ErrorCode::DopamineError: return "DopamineError";
        case ErrorCode::CuriosityError: return "CuriosityError";
        default: return "UnknownError";
    }
}

} // namespace nlm