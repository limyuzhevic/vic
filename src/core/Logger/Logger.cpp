// Performance monitoring implementation
void Logger::startPerformanceTimer(const std::string& operation) {
    std::lock_guard<std::mutex> lock(pImpl->performanceMutex);
    PerformanceRecord record;
    record.operation = operation;
    record.startTime = std::chrono::steady_clock::now();
    record.success = true;
    record.duration = 0.0;
    pImpl->performanceRecords.push_back(record);
}

void Logger::stopPerformanceTimer(const std::string& operation, bool success) {
    std::lock_guard<std::mutex> lock(pImpl->performanceMutex);
    auto now = std::chrono::steady_clock::now();
    
    for (auto& record : pImpl->performanceRecords) {
        if (record.operation == operation && record.duration == 0.0) {
            auto duration = std::chrono::duration<double>(now - record.startTime).count();
            record.duration = duration;
            record.success = success;
            break;
        }
    }
}

double Logger::getAverageOperationTime(const std::string& operation) const {
    std::lock_guard<std::mutex> lock(pImpl->performanceMutex);
    double totalDuration = 0.0;
    size_t count = 0;
    
    for (const auto& record : pImpl->performanceRecords) {
        if (record.operation == operation && record.duration > 0.0) {
            totalDuration += record.duration;
            count++;
        }
    }
    
    return count > 0 ? totalDuration / count : 0.0;
}

void Logger::enableDebugMode(bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->debugMode = enabled;
    if (enabled) {
        NLM_LOG_INFO("Debug mode enabled");
    }
}

bool Logger::isDebugModeEnabled() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->debugMode;
}

void Logger::setErrorHandler(std::shared_ptr<IErrorHandler> handler) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->errorHandler = handler;
}

bool Logger::handleError(const std::exception& error) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->errorHandler) {
        try {
            // Try to convert to NLMException for better handling
            auto& nlmError = dynamic_cast<const NLMException&>(error);
            RecoveryAction action = pImpl->errorHandler->handleError(nlmError);
            return action == RecoveryAction::Retry;
        } catch (const std::bad_cast&) {
            // Not an NLMException, wrap it
            SystemError sysError(error.what());
            RecoveryAction action = pImpl->errorHandler->handleError(sysError);
            return action == RecoveryAction::Retry;
        }
    }
    return false; // No error handler, don't rethrow
}

// Global error handler management
namespace {
    // Global error metrics instance
    ErrorMetrics& getGlobalErrorMetricsInstance() {
        static ErrorMetrics metrics;
        return metrics;
    }
    
    // Global error handler instance
    std::shared_ptr<IErrorHandler> g_globalErrorHandler;
    std::mutex g_errorHandlerMutex;
}

// Global error metrics accessor
ErrorMetrics& getGlobalErrorMetrics() {
    return getGlobalErrorMetricsInstance();
}

// Global error handler management
void setGlobalErrorHandler(std::shared_ptr<IErrorHandler> handler) {
    std::lock_guard<std::mutex> lock(g_errorHandlerMutex);
    g_globalErrorHandler = handler;
}

std::shared_ptr<IErrorHandler> getGlobalErrorHandler() {
    std::lock_guard<std::mutex> lock(g_errorHandlerMutex);
    return g_globalErrorHandler;
}

// DefaultErrorHandler implementation
struct DefaultErrorHandler::Impl {
    LogLevel errorLevel;
    std::unordered_map<std::string, RecoveryAction> errorRecoveries;
    std::unordered_set<std::string> persistentErrors;
    std::unordered_map<std::string, size_t> errorCounts;
    std::chrono::system_clock::time_point lastErrorTime;
};

DefaultErrorHandler::DefaultErrorHandler()
    : recoveryEnabled_(true), fallbackMode_(true), gracefulDegradation_(true) {
    pImpl = std::make_unique<Impl>();
    pImpl->errorLevel = LogLevel::Error;
    
    // Initialize error recovery mappings
    pImpl->errorRecoveries["InitializationFailed"] = RecoveryAction::Retry;
    pImpl->errorRecoveries["MemoryAllocationFailed"] = RecoveryAction::GracefulDegrade;
    pImpl->errorRecoveries["InvalidConfiguration"] = RecoveryAction::Fallback;
    pImpl->errorRecoveries["SynapseWeightError"] = RecoveryAction::Retry;
    pImpl->errorRecoveries["NeuronParameterError"] = RecoveryAction::GracefulDegrade;
    pImpl->errorRecoveries["WorkingMemoryFull"] = RecoveryAction::Skip;
    pImpl->errorRecoveries["CheckpointWriteFailed"] = RecoveryAction::Retry;
    pImpl->errorRecoveries["PredictionError"] = RecoveryAction::Fallback;
    pImpl->errorRecoveries["NeuronDynamicsError"] = RecoveryAction::GracefulDegrade;
    pImpl->errorRecoveries["PlasticityRuleError"] = RecoveryAction::Retry;
}

RecoveryAction DefaultErrorHandler::handleError(const NLMException& error) {
    // Update error metrics
    getGlobalErrorMetrics().increment("Error", 0.0); // We'll need more detailed tracking
    
    // Determine error type from exception type
    std::string errorType;
    if (dynamic_cast<const InitializationError*>(&error)) {
        errorType = "InitializationError";
    } else if (dynamic_cast<const ConfigurationError*>(&error)) {
        errorType = "ConfigurationError";
    } else if (dynamic_cast<const MemoryError*>(&error)) {
        errorType = "MemoryError";
    } else if (dynamic_cast<const NeuralError*>(&error)) {
        errorType = "NeuralError";
    } else if (dynamic_cast<const SynapseError*>(&error)) {
        errorType = "SynapseError";
    } else if (dynamic_cast<const IntegrationError*>(&error)) {
        errorType = "IntegrationError";
    } else if (dynamic_cast<const PersistenceError*>(&error)) {
        errorType = "PersistenceError";
    } else if (dynamic_cast<const CheckpointError*>(&error)) {
        errorType = "CheckpointError";
    } else if (dynamic_cast<const ValidationError*>(&error)) {
        errorType = "ValidationError";
    } else {
        errorType = "SystemError";
    }
    
    // Check if this is a persistent error
    auto it = pImpl->persistentErrors.find(errorType);
    if (it != pImpl->persistentErrors.end()) {
        NLM_LOG_CRITICAL(error.getFullMessage());
        return RecoveryAction::None;
    }
    
    // Check error count for potential persistent error
    pImpl->errorCounts[errorType]++;
    if (pImpl->errorCounts[errorType] >= 5) { // 5 consecutive errors
        pImpl->persistentErrors.insert(errorType);
        NLM_LOG_CRITICAL("Persistent error detected: " + errorType + ". No recovery possible.");
        return RecoveryAction::None;
    }
    
    pImpl->lastErrorTime = std::chrono::system_clock::now();
    
    // Log the error
    logError(error);
    
    // Attempt recovery if enabled
    if (recoveryEnabled_) {
        RecoveryAction recovery = RecoveryAction::None;
        
        if (dynamic_cast<const InitializationError*>(&error)) {
            recovery = recoverFromSystemError(static_cast<const SystemError&>(error));
        } else if (dynamic_cast<const ConfigurationError*>(&error)) {
            recovery = recoverFromSystemError(static_cast<const SystemError&>(error));
        } else if (dynamic_cast<const MemoryError*>(&error)) {
            recovery = recoverFromMemoryError(static_cast<const MemoryError&>(error));
        } else if (dynamic_cast<const NeuralError*>(&error)) {
            recovery = recoverFromNeuralError(static_cast<const NeuralError&>(error));
        } else if (dynamic_cast<const SynapseError*>(&error)) {
            recovery = recoverFromNeuralError(static_cast<const NeuralError&>(error));
        } else if (dynamic_cast<const IntegrationError*>(&error)) {
            recovery = recoverFromIntegrationError(static_cast<const IntegrationError&>(error));
        } else if (dynamic_cast<const PersistenceError*>(&error)) {
            recovery = recoverFromPersistenceError(static_cast<const PersistenceError&>(error));
        } else if (dynamic_cast<const CheckpointError*>(&error)) {
            recovery = recoverFromPersistenceError(static_cast<const PersistenceError&>(error));
        } else if (dynamic_cast<const ValidationError*>(&error)) {
            recovery = recoverFromValidationError(static_cast<const ValidationError&>(error));
        }
        
        if (recovery != RecoveryAction::None) {
            NLM_LOG_WARNING("Error recovered with action: " + std::to_string(static_cast<int>(recovery)));
            return recovery;
        }
    }
    
    // No recovery, return skip
    return RecoveryAction::Skip;
}

void DefaultErrorHandler::logError(const NLMException& error) {
    LogLevel level = LogLevel::Error;
    
    if (dynamic_cast<const SystemError*>(&error)) {
        level = LogLevel::Critical;
    } else if (dynamic_cast<const MemoryError*>(&error)) {
        level = LogLevel::Critical;
    } else if (dynamic_cast<const ValidationError*>(&error)) {
        level = LogLevel::Warning;
    } else if (dynamic_cast<const IntegrationError*>(&error)) {
        level = LogLevel::Warning;
    } else if (dynamic_cast<const PersistenceError*>(&error)) {
        level = LogLevel::Warning;
    }
    
    if (level < pImpl->errorLevel) {
        return; // Skip logging for lower priority errors
    }
    
    // Log error with full context
    if (level <= LogLevel::Critical) {
        NLM_LOG_CRITICAL(error.getFullMessage());
    } else if (level <= LogLevel::Error) {
        NLM_LOG_ERROR(error.getFullMessage());
    } else if (level <= LogLevel::Warning) {
        NLM_LOG_WARNING(error.getFullMessage());
    } else if (level <= LogLevel::Info) {
        NLM_LOG_INFO(error.getFullMessage());
    }
}

void DefaultErrorHandler::setErrorLevel(LogLevel level) {
    pImpl->errorLevel = level;
}

// Error recovery implementations
RecoveryAction DefaultErrorHandler::recoverFromSystemError(const SystemError& error) {
    if (!fallbackMode_) return RecoveryAction::None;
    
    std::string errorMsg = error.what();
    if (errorMsg.find("InitializationFailed") != std::string::npos ||
        errorMsg.find("MemoryAllocationFailed") != std::string::npos) {
        NLM_LOG_WARNING("System error recovered: using reduced functionality mode");
        return RecoveryAction::GracefulDegrade;
    } else if (errorMsg.find("InvalidConfiguration") != std::string::npos) {
        NLM_LOG_WARNING("System error recovered: using default configuration");
        return RecoveryAction::Fallback;
    }
    
    return RecoveryAction::GracefulDegrade;
}

RecoveryAction DefaultErrorHandler::recoverFromNeuralError(const NeuralError& error) {
    std::string errorMsg = error.what();
    if (errorMsg.find("SynapseWeightError") != std::string::npos ||
        errorMsg.find("NeuronParameterError") != std::string::npos) {
        NLM_LOG_WARNING("Neural error recovered: resetting to safe defaults");
        return RecoveryAction::Retry;
    }
    
    return RecoveryAction::GracefulDegrade;
}

RecoveryAction DefaultErrorHandler::recoverFromMemoryError(const MemoryError& error) {
    std::string errorMsg = error.what();
    if (errorMsg.find("WorkingMemoryFull") != std::string::npos) {
        NLM_LOG_WARNING("Memory error recovered: clearing working memory and continuing");
        return RecoveryAction::Skip;
    }
    
    return RecoveryAction::GracefulDegrade;
}

RecoveryAction DefaultErrorHandler::recoverFromIntegrationError(const IntegrationError& error) {
    if (!gracefulDegradation_) return RecoveryAction::None;
    
    std::string errorMsg = error.what();
    if (errorMsg.find("BrainLoopError") != std::string::npos) {
        NLM_LOG_WARNING("Integration error recovered: reducing loop complexity");
        return RecoveryAction::GracefulDegrade;
    }
    
    return RecoveryAction::Skip;
}

RecoveryAction DefaultErrorHandler::recoverFromPersistenceError(const PersistenceError& error) {
    std::string errorMsg = error.what();
    if (errorMsg.find("CheckpointWriteFailed") != std::string::npos) {
        NLM_LOG_WARNING("Persistence error recovered: skipping checkpoint");
        return RecoveryAction::Skip;
    }
    
    return RecoveryAction::GracefulDegrade;
}

RecoveryAction DefaultErrorHandler::recoverFromValidationError(const ValidationError& error) {
    // Validation errors are typically configuration or parameter issues
    // that can be handled with fallback modes
    NLM_LOG_WARNING("Validation error recovered: using safe defaults");
    return RecoveryAction::Fallback;
}

} // namespace nlm
