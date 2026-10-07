#ifndef NLM_ERROR_HANDLING_HPP
#define NLM_ERROR_HANDLING_HPP

#include <stdexcept>
#include <string>
#include <sstream>

namespace nlm {

/**
 * Error types for NLM (Neural Learning Machine)
 */
enum class ErrorType {
    None = 0,
    Configuration = 1,
    Memory = 2,
    InvalidParameter = 3,
    FileIO = 4,
    Simulation = 5,
    Plasticity = 6,
    Serialization = 7
};

/**
 * Base NLM exception class
 */
class NLMException : public std::runtime_error {
protected:
    ErrorType errorType_;
    std::string component_;
    
public:
    explicit NLMException(ErrorType type, const std::string& message, 
                          const std::string& component = "")
        : std::runtime_error(formatMessage(type, message, component)),
          errorType_(type), component_(component) {}
    
    virtual ~NLMException() noexcept = default;
    
    ErrorType getErrorType() const noexcept { return errorType_; }
    const std::string& getComponent() const noexcept { return component_; }
    
protected:
    std::string formatMessage(ErrorType type, const std::string& message, 
                            const std::string& component) {
        std::ostringstream oss;
        oss << "NLM[" << static_cast<int>(type) << "]";
        if (!component.empty()) {
            oss << "[" << component << "]";
        }
        oss << ": " << message;
        return oss.str();
    }
};

/**
 * Configuration error - invalid configuration or missing parameters
 */
class ConfigurationException : public NLMException {
public:
    explicit ConfigurationException(const std::string& message, 
                                   const std::string& component = "Config")
        : NLMException(ErrorType::Configuration, message, component) {}
    
    ConfigurationException(const std::string& message, 
                          const std::string& paramName,
                          const std::string& component = "Config")
        : NLMException(ErrorType::Configuration, 
                       "Invalid parameter '" + paramName + "': " + message, 
                       component) {}
};

/**
 * Memory error - memory allocation, pool, or access violations
 */
class MemoryException : public NLMException {
public:
    explicit MemoryException(const std::string& message,
                           const std::string& component = "Memory")
        : NLMException(ErrorType::Memory, message, component) {}
    
    MemoryException(size_t size, const std::string& component = "Memory")
        : NLMException(ErrorType::Memory, 
                       "Failed to allocate " + std::to_string(size) + " bytes", 
                       component) {}
};

/**
 * Simulation error - errors during neural simulation
 */
class SimulationException : public NLMException {
public:
    explicit SimulationException(const std::string& message,
                                const std::string& component = "Simulation")
        : NLMException(ErrorType::Simulation, message, component) {}
    
    SimulationException(SimulationStep step, const std::string& message,
                       const std::string& component = "Simulation")
        : NLMException(ErrorType::Simulation, 
                       "Step " + std::to_string(step) + ": " + message, 
                       component) {}
};

/**
 * Plasticity error - plasticity rule failures
 */
class PlasticityException : public NLMException {
public:
    explicit PlasticityException(const std::string& message,
                               const std::string& component = "Plasticity")
        : NLMException(ErrorType::Plasticity, message, component) {}
    
    PlasticityException(SynapseId synapseId, const std::string& message,
                       const std::string& component = "Plasticity")
        : NLMException(ErrorType::Plasticity, 
                       "Synapse " + std::to_string(synapseId) + ": " + message, 
                       component) {}
};

/**
 * Error handling utilities
 */
class ErrorHandler {
public:
    /**
     * Validate that a parameter is within a specified range
     */
    template<typename T>
    static void validateParameter(const std::string& paramName, T value,
                                 T min, T max, const std::string& component = "Validation") {
        if (value < min || value > max) {
            throw ConfigurationException(
                "Value " + std::to_string(value) + " is outside valid range [" + 
                std::to_string(min) + ", " + std::to_string(max) + "]",
                paramName, component
            );
        }
    }
    
    /**
     * Validate that a pointer is not null
     */
    template<typename T>
    static T* validatePointer(T* ptr, const std::string& paramName,
                             const std::string& component = "Validation") {
        if (!ptr) {
            throw MemoryException(
                "Null pointer passed for parameter '" + paramName + "'",
                component
            );
        }
        return ptr;
    }
    
    /**
     * Validate that an object is initialized
     */
    static void validateInitialized(bool initialized, const std::string& objectName,
                                   const std::string& component = "Validation") {
        if (!initialized) {
            throw SimulationException(
                "Object '" + objectName + "' is not initialized",
                component
            );
        }
    }
    
    /**
     * Validate simulation parameters
     */
    static void validateSimulationParameters(SimulationStep step, Timestamp time,
                                            const std::string& component = "Simulation") {
        if (step < 0) {
            throw SimulationException(
                "Invalid simulation step: " + std::to_string(step),
                component
            );
        }
        if (time < 0.0) {
            throw SimulationException(
                "Invalid simulation time: " + std::to_string(time),
                component
            );
        }
    }
    
    /**
     * Validate neuron parameters
     */
    static void validateNeuronParameters(const NeuronState& state,
                                        const std::string& component = "Neuron") {
        if (state.membranePotential > 50.0f) {
            throw ConfigurationException(
                "Membrane potential exceeds maximum stable value",
                "membrane_potential", component
            );
        }
        
        if (state.threshold < -100.0f || state.threshold > 50.0f) {
            throw ConfigurationException(
                "Invalid threshold value",
                "threshold", component
            );
        }
    }
    
    /**
     * Validate synaptic parameters
     */
    static void validateSynapticParameters(SynapticWeight weight, Delay delay,
                                          const std::string& component = "Synapse") {
        validateParameter("weight", weight, -2.0f, 2.0f, component);
        validateParameter("delay", delay, 0, 1000, component);
    }
};

} // namespace nlm

#endif // NLM_ERROR_HANDLING_HPP
