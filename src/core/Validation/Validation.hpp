// Validation framework for NLM neural simulation project
// Comprehensive validation functions for error checking and safe fallbacks

#ifndef NLM_VALIDATION_HPP
#define NLM_VALIDATION_HPP

#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <any>
#include <memory>
#include <mutex>
#include <sstream>
#include <iomanip>
#include "ErrorHandling/Error.hpp"
#include "Exception.hpp"

namespace nlm {

// Validation result structure
struct ValidationResult {
    bool valid;
    std::string message;
    std::vector<std::string> warnings;
    std::vector<std::string> errors;
    
    ValidationResult() : valid(true) {}
    
    ValidationResult(bool v, const std::string& msg) : valid(v), message(msg) {}
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
        valid = valid && false;
    }
    
    void addError(const std::string& error) {
        errors.push_back(error);
        valid = valid && false;
    }
    
    bool isValid() const { return valid; }
    std::string getSummary() const {
        std::ostringstream oss;
        oss << "Validation: " << (valid ? "PASS" : "FAIL");
        if (!message.empty()) oss << " - " << message;
        return oss.str();
    }
};

// Type definition for status
using Status = ValidationResult;

// Neural value validation
class NeuralValueValidator {
public:
    // Validate neural membrane potential
    static Status validatePotential(MembranePotential potential, const std::string& context = "") {
        ValidationResult result;
        
        if (std::isnan(potential)) {
            result.addError("Invalid potential value: NaN detected");
            result.addWarning("NaN potential encountered in context: " + context);
        }
        
        if (potential > MAX_POTENTIAL) {
            result.addError("Potential exceeds maximum: " + std::to_string(potential) + " > " + std::to_string(MAX_POTENTIAL));
            result.addWarning("Clamping potential to valid range");
        }
        
        if (potential < MIN_POTENTIAL) {
            result.addError("Potential below minimum: " + std::to_string(potential) + " < " + std::to_string(MIN_POTENTIAL));
            result.addWarning("Clamping potential to valid range");
        }
        
        return result;
    }
    
    // Validate synaptic weight
    static Status validateWeight(SynapticWeight weight, const std::string& context = "") {
        ValidationResult result;
        
        if (std::isnan(weight)) {
            result.addError("Invalid weight value: NaN detected");
            result.addWarning("NaN weight encountered in context: " + context);
        }
        
        if (weight > MAX_WEIGHT) {
            result.addError("Weight exceeds maximum: " + std::to_string(weight) + " > " + std::to_string(MAX_WEIGHT));
            result.addWarning("Clamping weight to valid range");
        }
        
        if (weight < MIN_WEIGHT) {
            result.addError("Weight below minimum: " + std::to_string(weight) + " < " + std::to_string(MIN_WEIGHT));
            result.addWarning("Clamping weight to valid range");
        }
        
        return result;
    }
    
    // Validate probability value
    static Status validateProbability(float value, const std::string& context = "") {
        ValidationResult result;
        
        if (value < 0.0f || value > 1.0f) {
            result.addError("Probability out of range [0,1]: " + std::to_string(value));
            result.addWarning("Clamping probability to valid range");
        }
        
        if (std::isnan(value)) {
            result.addError("Invalid probability value: NaN detected");
            result.addWarning("NaN probability encountered in context: " + context);
        }
        
        return result;
    }
    
    // Validate neuron ID
    static Status validateNeuronId(NeuronId id, const std::string& context = "") {
        ValidationResult result;
        
        if (id == INVALID_NEURON_ID) {
            result.addError("Invalid neuron ID detected");
            result.addWarning("Using default neuron ID");
        }
        
        return result;
    }
    
    // Validate region ID
    static Status validateRegionId(RegionId id, const std::string& context = "") {
        ValidationResult result;
        
        if (id == INVALID_REGION_ID) {
            result.addError("Invalid region ID detected");
            result.addWarning("Using default region ID");
        }
        
        return result;
    }
    
    // Validate memory size
    static Status validateMemorySize(size_t size, const std::string& context = "") {
        ValidationResult result;
        
        if (size == 0) {
            result.addWarning("Zero-sized memory allocation detected");
        }
        
        if (size > MAX_MEMORY_ALLOCATION) {
            result.addError("Memory allocation size exceeds maximum: " + std::to_string(size));
        }
        
        return result;
    }
    
    // Safe clamping for potential
    static MembranePotential clampPotential(MembranePotential potential) {
        if (std::isnan(potential)) return 0.0f;
        return std::max(std::min(potential, MAX_POTENTIAL), MIN_POTENTIAL);
    }
    
    // Safe clamping for weight
    static SynapticWeight clampWeight(SynapticWeight weight) {
        if (std::isnan(weight)) return 0.0f;
        return std::max(std::min(weight, MAX_WEIGHT), MIN_WEIGHT);
    }
    
    // Safe clamping for probability
    static float clampProbability(float value) {
        if (std::isnan(value)) return 0.5f;
        return std::max(std::min(value, 1.0f), 0.0f);
    }

private:
    static constexpr MembranePotential MAX_POTENTIAL = 100.0f;
    static constexpr MembranePotential MIN_POTENTIAL = -100.0f;
    static constexpr SynapticWeight MAX_WEIGHT = 1.0f;
    static constexpr SynapticWeight MIN_WEIGHT = -1.0f;
    static constexpr size_t MAX_MEMORY_ALLOCATION = 1024 * 1024 * 1024;  // 1GB
};

// Neural system validation
class NeuralSystemValidator {
public:
    // Validate brain initialization parameters
    static Status validateBrainInitialization(const std::shared_ptr<Config>& config) {
        ValidationResult result;
        
        if (!config) {
            result.addError("Null configuration pointer");
            result.addWarning("Using default configuration");
        }
        
        // Validate simulation parameters
        if (auto steps = config->get<size_t>("max_steps")) {
            if (*steps == 0) {
                result.addWarning("Zero max steps specified");
            }
        }
        
        if (auto dt = config->get<TimestepDuration>("timestep")) {
            if (*dt <= 0.0) {
                result.addError("Invalid timestep: " + std::to_string(*dt));
            }
        }
        
        return result;
    }
    
    // Validate neural region
    static Status validateNeuralRegion(const NeuralRegion* region) {
        ValidationResult result;
        
        if (!region) {
            result.addError("Null neural region pointer");
            return result;
        }
        
        // Validate region properties
        if (region->getRegionId() == INVALID_REGION_ID) {
            result.addError("Region has invalid ID");
        }
        
        const auto& neurons = region->getAllNeurons();
        if (!neurons || neurons->empty()) {
            result.addWarning("Region has no neurons");
        }
        
        return result;
    }
    
    // Validate neuron connectivity
    static Status validateNeuronConnectivity(const Neuron* neuron, size_t maxIncoming) {
        ValidationResult result;
        
        if (!neuron) {
            result.addError("Null neuron pointer");
            return result;
        }
        
        // Validate incoming synapses
        size_t incomingCount = neuron->getIncomingSynapseCount();
        if (incomingCount > maxIncoming) {
            result.addError("Neuron has too many incoming synapses: " + std::to_string(incomingCount));
        }
        
        return result;
    }
    
    // Validate synapse
    static Status validateSynapse(const Synapse* synapse) {
        ValidationResult result;
        
        if (!synapse) {
            result.addError("Null synapse pointer");
            return result;
        }
        
        // Validate synapse properties
        auto sourceId = synapse->getSourceNeuron();
        auto destId = synapse->getDestinationNeuron();
        
        if (sourceId == destId) {
            result.addError("Synapse source and destination are the same neuron");
        }
        
        SynapticWeight weight = synapse->getWeight();
        if (std::isnan(weight)) {
            result.addError("Synapse has invalid weight");
        }
        
        return result;
    }
    
    // Validate memory system
    static Status validateMemorySystem(const NeuralWorkingMemory* memory) {
        ValidationResult result;
        
        if (!memory) {
            result.addError("Null memory system pointer");
            return result;
        }
        
        size_t capacity = memory->getCapacity();
        if (capacity == 0) {
            result.addError("Memory system has zero capacity");
        }
        
        return result;
    }
    
    // Validate developmental stage
    static Status validateDevelopmentalStage(DevelopmentalStage stage) {
        ValidationResult result;
        
        if (stage == DevelopmentalStage::Unknown) {
            result.addError("Development stage is unknown");
        }
        
        return result;
    }

private:
    static constexpr size_t MAX_INCOMING_SYNAPSES = 10000;
};

// Error handling utilities
class ErrorHandlingUtils {
public:
    // Safe neural operation with error handling
    template<typename T>
    static T safeOperation(const std::function<T()>& operation, 
                          const std::string& context = "",
                          const std::string& operationName = "operation") {
        try {
            return operation();
        } catch (const NeuralException& e) {
            // Error already has context
            throw;
        } catch (const std::exception& e) {
            // Convert std::exception to NeuralException
            throw NeuralComputationException(
                "Safe operation failed in " + operationName + ": " + std::string(e.what()),
                context
            );
        } catch (...) {
            throw NeuralComputationException(
                "Unknown error in safe operation " + operationName, context
            );
        }
    }
    
    // Safe memory operation with error handling and fallback
    template<typename T>
    static std::shared_ptr<T> safeAllocate(size_t count, const std::string& context = "") {
        if (count == 0) {
            return nullptr;
        }
        
        try {
            auto ptr = std::shared_ptr<T>(new T[count]);
            return ptr;
        } catch (const std::bad_alloc&) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralMemoryAllocationException>(
                    sizeof(T) * count, "Memory allocation failed", context
                )
            );
            return nullptr;
        } catch (...) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralMemoryAccessException>(
                    "Allocation", "Unexpected error during memory allocation", context
                )
            );
            return nullptr;
        }
    }
    
    // Safe vector access with bounds checking
    template<typename T>
    static bool safeAccess(const std::vector<T>& container, 
                          size_t index, T& result,
                          const std::string& context = "",
                          const std::string& operation = "access") {
        if (index >= container.size()) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralBoundaryException>(
                    "Vector", operation, "Index out of bounds: " + std::to_string(index), context
                )
            );
            return false;
        }
        result = container[index];
        return true;
    }
    
    // Safe map access with error handling
    template<typename K, typename V>
    static bool safeAccess(const std::map<K, V>& container,
                          const K& key, V& result,
                          const std::string& context = "",
                          const std::string& operation = "access") {
        auto it = container.find(key);
        if (it == container.end()) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralBoundaryException>(
                    "Map", operation, "Key not found: " + std::to_string(key), context
                )
            );
            return false;
        }
        result = it->second;
        return true;
    }
    
    // Safe operation with fallback
    template<typename T>
    static T safeWithFallback(const std::function<T()>& operation,
                             const T& fallback,
                             const std::string& context = "",
                             const std::string& operationName = "operation") {
        try {
            return operation();
        } catch (...) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralComputationException>(
                    "Operation failed, using fallback in " + operationName, context
                )
            );
            return fallback;
        }
    }
    
    // Create error context with metadata
    static ErrorMetadata createErrorContext(const std::string& operation,
                                           const std::string& component,
                                           const std::string& file = "",
                                           size_t line = 0) {
        return ErrorMetadata(operation, component, file, line);
    }
    
    // Handle error with recovery if possible
    static bool handleWithRecovery(const std::shared_ptr<NeuralException>& error) {
        if (error->isRecoverable()) {
            return error->recover();
        }
        return false;
    }
    
    // Log error with validation
    static void logError(const std::shared_ptr<NeuralException>& error,
                        const std::string& context = "") {
        if (error) {
            ErrorReporter::getInstance().log(error, ErrorLevel::ERROR, context);
        }
    }
    
    // Log warning with validation
    static void logWarning(const std::shared_ptr<NeuralException>& error,
                          const std::string& context = "") {
        if (error) {
            ErrorReporter::getInstance().log(error, ErrorLevel::WARNING, context);
        }
    }
    
    // Log info with validation
    static void logInfo(const std::shared_ptr<NeuralException>& error,
                       const std::string& context = "") {
        if (error) {
            ErrorReporter::getInstance().log(error, ErrorLevel::INFO, context);
        }
    }

private:
    static ErrorReporter* getErrorReporter() {
        static ErrorReporter reporter;
        return &reporter;
    }
};

// Main validation and error checking functions
class Validator {
public:
    // Comprehensive validation for brain operation
    static Status validateBrainOperation(const Brain* brain,
                                        const std::string& operation,
                                        const std::string& context = "") {
        ValidationResult result;
        
        if (!brain) {
            result.addError("Null brain pointer for operation: " + operation);
            return result;
        }
        
        // Additional validation based on operation
        if (operation.find("step") != std::string::npos) {
            // Validate simulation state for stepping
            auto currentStep = brain->getCurrentStep();
            if (currentStep == INVALID_SIMULATION_STEP) {
                result.addWarning("Invalid simulation step");
            }
        }
        
        return result;
    }
    
    // Comprehensive validation for neuron operation
    static Status validateNeuronOperation(const Neuron* neuron,
                                         const std::string& operation,
                                         const std::string& context = "") {
        ValidationResult result;
        
        if (!neuron) {
            result.addError("Null neuron pointer for operation: " + operation);
            return result;
        }
        
        // Validate based on operation type
        if (operation.find("potential") != std::string::npos) {
            MembranePotential potential = neuron->getMembranePotential();
            Status potResult = NeuralValueValidator::validatePotential(potential, context);
            if (!potResult.isValid()) {
                result.addWarning(potResult.getSummary());
            }
        }
        
        return result;
    }
    
    // Comprehensive validation for synapse operation
    static Status validateSynapseOperation(const Synapse* synapse,
                                          const std::string& operation,
                                          const std::string& context = "") {
        ValidationResult result;
        
        if (!synapse) {
            result.addError("Null synapse pointer for operation: " + operation);
            return result;
        }
        
        // Validate based on operation type
        if (operation.find("weight") != std::string::npos) {
            SynapticWeight weight = synapse->getWeight();
            Status weightResult = NeuralValueValidator::validateWeight(weight, context);
            if (!weightResult.isValid()) {
                result.addWarning(weightResult.getSummary());
            }
        }
        
        return result;
    }
    
    // Safe operation wrapper
    template<typename T>
    static T safeOperation(const std::function<T()>& operation,
                          const std::string& operationName,
                          const std::string& context = "") {
        return ErrorHandlingUtils::safeOperation(operation, context, operationName);
    }
    
    // Safe with fallback wrapper
    template<typename T>
    static T safeOperationWithFallback(const std::function<T()>& operation,
                                       const T& fallback,
                                       const std::string& operationName,
                                       const std::string& context = "") {
        return ErrorHandlingUtils::safeWithFallback(operation, fallback, context, operationName);
    }
};

// Global validation and error handling instances
extern std::unique_ptr<ValidationUtils> g_validator;
extern std::unique_ptr<ErrorHandlingUtils> g_errorHandler;

// Initialize validation and error handling systems
inline void initializeValidationAndErrorHandling() {
    g_validator = std::make_unique<ValidationUtils>();
    g_errorHandler = std::make_unique<ErrorHandlingUtils>();
}

// Shutdown validation and error handling systems
inline void shutdownValidationAndErrorHandling() {
    g_validator.reset();
    g_errorHandler.reset();
}

} // namespace nlm

#endif // NLM_VALIDATION_HPP
