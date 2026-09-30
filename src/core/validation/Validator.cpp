#include "Validator.hpp"
#include "Logger/Logger.hpp"
#include <algorithm>
#include <stdexcept>

namespace nlm {

void Validator::validateConfigParameter(const std::string& key, 
                                       const std::string& value,
                                       const std::string& description) {
    if (key.empty()) {
        throw ConfigurationException("Configuration key cannot be empty", "Validator");
    }
    
    if (value.empty() && description.find("optional") == std::string::npos) {
        std::map<std::string, std::string> ctx;
        ctx["key"] = key;
        ctx["description"] = description;
        throw ConfigurationException(
            "Configuration parameter cannot be empty",
            ctx,
            "Validator"
        );
    }
}

void Validator::validateNeuronParameters(const NeuronState& state) {
    // Validate membrane potential range
    if (state.membranePotential < -100.0f || state.membranePotential > 50.0f) {
        std::map<std::string, std::string> ctx;
        ctx["membranePotential"] = std::to_string(state.membranePotential);
        ctx["valid_range"] = "[-100.0, 50.0] mV";
        throw MemoryException(
            "Invalid membrane potential value",
            ctx,
            "Validator"
        );
    }
    
    // Validate threshold
    VALIDATE_RANGE(state.threshold, -80.0f, 0.0f, "threshold");
    
    // Validate refractory period
    VALIDATE_POSITIVE(state.refractoryPeriod, "refractoryPeriod");
}

void Validator::validateSynapseParameters(const Synapse& synapse) {
    // Validate weight range
    VALIDATE_RANGE(synapse.getWeight(), -1.0f, 1.0f, "synapse weight");
    
    // Validate delay
    VALIDATE_POSITIVE(synapse.getDelay(), "synapse delay");
}

void Validator::validateRegionParameters(const NeuralRegion& region) {
    if (region.getNeuronCount() > 100000) {
        std::map<std::string, std::string> ctx;
        ctx["neuron_count"] = std::to_string(region.getNeuronCount());
        ctx["max_allowed"] = "100000";
        throw MemoryException(
            "Region exceeds maximum neuron count",
            ctx,
            "Validator"
        );
    }
}

void Validator::validateDevelopmentalStage(DevelopmentalStage from,
                                           DevelopmentalStage to) {
    if (to < from) {
        throw BrainInitializationException(
            "Invalid developmental stage transition",
            {{ "from_stage", std::to_string(static_cast<int>(from)) },
             { "to_stage", std::to_string(static_cast<int>(to)) }},
            "Validator"
        );
    }
}

void Validator::validateMemoryCapacity(size_t capacity, size_t currentUsage) {
    if (currentUsage > capacity) {
        std::map<std::string, std::string> ctx;
        ctx["current_usage"] = std::to_string(currentUsage);
        ctx["capacity"] = std::to_string(capacity);
        throw MemoryException(
            "Memory capacity exceeded",
            ctx,
            "Validator"
        );
    }
}

void Validator::validateNeuromodulatorLevels(const Neuromodulator& signal) {
    VALIDATE_RANGE(signal.dopamine, 0.0f, 10.0f, "dopamine_level");
    VALIDATE_RANGE(signal.curiosity, 0.0f, 10.0f, "curiosity_level");
    VALIDATE_RANGE(signal.novelty, 0.0f, 10.0f, "novelty_level");
}

void Validator::validatePlasticityCompatibility(const PlasticityFlags& flags1,
                                               const PlasticityFlags& flags2) {
    bool hebbian_conflict = flags1.hebbian && flags2.hebbian;
    bool stdp_conflict = flags1.stdp && flags2.stdp;
    bool reward_modulated_conflict = flags1.reward_modulated && flags2.reward_modulated;
    
    if (hebbian_conflict || stdp_conflict || reward_modulated_conflict) {
        std::map<std::string, std::string> ctx;
        ctx["hebbian_conflict"] = hebbian_conflict ? "true" : "false";
        ctx["stdp_conflict"] = stdp_conflict ? "true" : "false";
        ctx["reward_modulated_conflict"] = reward_modulated_conflict ? "true" : "false";
        throw PlasticityException(
            "Plasticity rule conflict detected",
            ctx,
            "Validator"
        );
    }
}

void Validator::validateMemoryAllocation(size_t size, const std::string& purpose) {
    if (size == 0) {
        throw MemoryException(
            "Cannot allocate zero bytes",
            {{ "purpose", purpose }},
            "Validator"
        );
    }
    
    // Check for potential overflow
    if (size > std::numeric_limits<size_t>::max() / 2) {
        throw MemoryException(
            "Memory allocation size would cause overflow",
            {{ "size", std::to_string(size) },
             { "purpose", purpose }},
            "Validator"
        );
    }
}

void Validator::validateMemoryAccess(const void* ptr, size_t size) {
    if (ptr == nullptr && size > 0) {
        throw MemoryException(
            "Attempt to access null pointer with size > 0",
            {{ "size", std::to_string(size) }},
            "Validator"
        );
    }
}

void Validator::validateSystemState(bool condition, const std::string& errorMessage,
                                   const std::map<std::string, std::string>& context) {
    if (!condition) {
        throw BrainRuntimeException(errorMessage, context, "Validator");
    }
}

bool Validator::canGracefulDegrade(const std::string& operation) {
    // Check if this operation can be degraded gracefully
    static const std::vector<std::string> degradableOperations = {
        "memory_management", "prediction", "cognition", "neuromodulation"
    };
    
    return std::find(degradableOperations.begin(), degradableOperations.end(), operation) != degradableOperations.end();
}

void Validator::rollbackState(const std::string& operation) {
    // Implementation would depend on specific operation
    // For now, just log the rollback attempt
    LOG_WARNING("Rolling back state for operation: " + operation);
}

std::string Validator::formatError(const BrainException& e) {
    std::ostringstream oss;
    oss << "Error " << static_cast<int>(e.getErrorCode()) << ": " << e.what();
    if (!e.getComponent().empty()) {
        oss << " (Component: " << e.getComponent() << ")";
    }
    if (!e.getContext().empty()) {
        oss << "\nContext:";
        for (const auto& pair : e.getContext()) {
            oss << "\n  " << pair.first << ": " << pair.second;
        }
    }
    return oss.str();
}

void Validator::logError(const BrainException& e) {
    if (e.getErrorCode() == BrainException::ErrorCode::BrainInitializationFailed) {
        LOG_ERROR("CRITICAL: Brain initialization failed: " + formatError(e));
    } else if (e.getErrorCode() == BrainException::ErrorCode::MemoryInvalidState) {
        LOG_ERROR("MEMORY: Memory system error: " + formatError(e));
    } else if (e.getErrorCode() == BrainException::ErrorCode::SystemFailure) {
        LOG_ERROR("SYSTEM: Runtime error: " + formatError(e));
    } else {
        LOG_WARNING("VALIDATION: Validation error: " + formatError(e));
    }
}

ErrorRecovery::RecoveryAction ErrorRecovery::getRecoveryStrategy(BrainException::ErrorCode errorCode) {
    switch (errorCode) {
        case BrainException::ErrorCode::BrainInitializationFailed:
        case BrainException::ErrorCode::MemoryCorruption:
            return RecoveryAction::Abort;
            
        case BrainException::ErrorCode::MemoryCapacityExceeded:
        case BrainException::ErrorCode::PlasticityRuleConflict:
        case BrainException::ErrorCode::NeuromodulationLevelInvalid:
            return RecoveryAction::Fallback;
            
        case BrainException::ErrorCode::ConfigurationError:
        case BrainException::ErrorCode::InvalidParameter:
            return RecoveryAction::Retry;
            
        case BrainException::ErrorCode::FileIOError:
            return RecoveryAction::Continue;  // Can continue with default file
            
        default:
            return RecoveryAction::Continue;
    }
}

bool ErrorRecovery::executeRecovery(RecoveryAction action, 
                                   const std::map<std::string, std::string>& context) {
    switch (action) {
        case RecoveryAction::Continue:
            LOG_WARNING("Continuing with degraded functionality");
            return true;
            
        case RecoveryAction::Rollback:
            LOG_WARNING("Attempting to rollback state");
            return true;
            
        case RecoveryAction::Fallback:
            LOG_WARNING("Using fallback/default behavior");
            return true;
            
        case RecoveryAction::Abort:
            LOG_ERROR("Aborting operation due to critical error");
            return false;
            
        case RecoveryAction::Retry:
            LOG_WARNING("Retrying operation");
            return true;
            
        default:
            return true;
    }
}

void ErrorRecovery::saveState(const std::string& operationName) {
    // Implementation would save current system state
    LOG_DEBUG("Saving state for rollback: " + operationName);
}

bool ErrorRecovery::restoreState(const std::string& operationName) {
    // Implementation would restore saved state
    LOG_DEBUG("Restoring state for rollback: " + operationName);
    return true;
}

void ErrorRecovery::clearState(const std::string& operationName) {
    // Implementation would clear saved state
    LOG_DEBUG("Clearing state for operation: " + operationName);
}

bool ErrorRecovery::isNonCriticalError(BrainException::ErrorCode errorCode) {
    switch (errorCode) {
        case BrainException::ErrorCode::FileIOError:
        case BrainException::ErrorCode::ConfigurationError:
        case BrainException::ErrorCode::PlasticityRuleConflict:
            return true;
            
        default:
            return false;
    }
}

void ErrorRecovery::enableGracefulDegradation(const std::string& component) {
    LOG_INFO("Enabling graceful degradation for component: " + component);
}

bool ErrorRecovery::isGracefulDegradationEnabled(const std::string& component) {
    // Implementation would check if degradation is enabled
    return true;
}

void ErrorContext::addContext(const std::string& operation, 
                             const std::string& component,
                             const std::map<std::string, std::string>& data) {
    ContextInfo info;
    info.operation = operation;
    info.component = component;
    info.timestamp = 0.0;  // Would get from SimulationClock
    info.step = 0;         // Would get from SimulationClock
    info.data = data;
    
    // Add to context history
    LOG_DEBUG("Adding error context: " + operation + " in " + component);
}

std::vector<ErrorContext::ContextInfo> ErrorContext::getRecentContext(size_t maxEntries) {
    // Implementation would return recent context
    return std::vector<ContextInfo>();
}

void ErrorContext::clearContext() {
    LOG_DEBUG("Clearing error context history");
}

void ErrorContext::correlateErrors(const std::string& error1, const std::string& error2) {
    LOG_DEBUG("Correlating errors: " + error1 + " and " + error2);
}

std::vector<std::string> ErrorContext::getCorrelatedErrors(const std::string& error) {
    // Implementation would return correlated errors
    return std::vector<std::string>();
}

std::string ErrorMessages::getFriendlyMessage(const BrainException& e) {
    std::string baseMessage = e.what();
    
    switch (e.getErrorCode()) {
        case BrainException::ErrorCode::BrainInitializationFailed:
            return "The brain system failed to initialize. " + baseMessage;
            
        case BrainException::ErrorCode::MemoryCapacityExceeded:
            return "Working memory is full. Try reducing the amount of stored information. " + baseMessage;
            
        case BrainException::ErrorCode::ConfigurationError:
            return "Configuration error detected. Check your settings. " + baseMessage;
            
        case BrainException::ErrorCode::PlasticityRuleConflict:
            return "Incompatible plasticity rules detected. Check your learning rules. " + baseMessage;
            
        case BrainException::ErrorCode::FileIOError:
            return "Could not read or write to file. Check file permissions and path. " + baseMessage;
            
        default:
            return "An error occurred: " + baseMessage;
    }
}

std::string ErrorMessages::getSuggestedRemedy(BrainException::ErrorCode errorCode) {
    switch (errorCode) {
        case BrainException::ErrorCode::BrainInitializationFailed:
            return "1. Check all configuration files\n";
        
        case BrainException::ErrorCode::MemoryCapacityExceeded:
            return "1. Reduce the number of stored items\n";
            
        case BrainException::ErrorCode::ConfigurationError:
            return "1. Validate configuration file format\n";
            
        case BrainException::ErrorCode::PlasticityRuleConflict:
            return "1. Disable conflicting plasticity rules\n";
            
        case BrainException::ErrorCode::FileIOError:
            return "1. Check file permissions\n2. Ensure disk space is available\n3. Verify file path is correct\n";
            
        default:
            return "1. Check system logs for details\n2. Contact support if error persists\n";
    }
}

std::string ErrorMessages::getTechnicalDetails(const BrainException& e) {
    std::ostringstream oss;
    oss << "Error Code: " << static_cast<int>(e.getErrorCode()) << "\n";
    oss << "Component: " << e.getComponent() << "\n";
    oss << "Message: " << e.what() << "\n";
    
    if (!e.getContext().empty()) {
        oss << "\nContext Information:\n";
        for (const auto& pair : e.getContext()) {
            oss << "  " << pair.first << ": " << pair.second << "\n";
        }
    }
    
    if (!e.getSuggestions().empty()) {
        oss << "\nSuggested Remedies:\n";
        for (const auto& sug : e.getSuggestions()) {
            oss << "  - " << sug << "\n";
        }
    }
    
    return oss.str();
}

ErrorMessages::Severity ErrorMessages::getSeverity(BrainException::ErrorCode errorCode) {
    switch (errorCode) {
        case BrainException::ErrorCode::BrainInitializationFailed:
        case BrainException::ErrorCode::MemoryCorruption:
        case BrainException::ErrorCode::SystemFailure:
            return Severity::Critical;
            
        case BrainException::ErrorCode::ConfigurationError:
        case BrainException::ErrorCode::InvalidParameter:
        case BrainException::ErrorCode::PlasticityRuleConflict:
            return Severity::Error;
            
        default:
            return Severity::Warning;
    }
}

} // namespace nlm
