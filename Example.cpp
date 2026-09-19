#include "NLMError.hpp"
#include "ErrorHandler.hpp"
#include "Logger.hpp"

namespace nlm {

// Example function demonstrating error handling
void demonstrateErrorHandling() {
    try {
        // Example 1: Basic error with null pointer
        const std::string* ptr = nullptr;
        NLM_CHECK_WITH_CODE(ptr != nullptr, ConfigError, 
                           "Pointer is null, cannot dereference", 
                           ErrorCode::ERROR_NULL_POINTER);
        
    } catch (const NLMError& e) {
        // Handle error with context
        ErrorContext ctx = ErrorUtils::createContext("demonstrateErrorHandling");
        ctx.addParameter("pointer_value", "null");
        
        NLMError enhancedError("Error in demonstrateErrorHandling: " + std::string(e.what()),
                            e.getLocation());
        enhancedError.setContext(ctx);
        enhancedError.setCause(e);
        
        // Handle the enhanced error
        ErrorHandler::getInstance().handleError(enhancedError);
    }
    
    try {
        // Example 2: Error with recovery suggestion
        float division = 0.0f;
        NLM_CHECK_WITH_CODE(division != 0.0f, ConfigError,
                           "Cannot divide by zero",
                           ErrorCode::ERROR_DIVISION_BY_ZERO);
        
        float result = 10.0f / division;
        
    } catch (const NLMError& e) {
        // Get recovery suggestion
        std::string recovery = e.getRecoverySuggestion();
        Logger::getInstance().warning("Recovery suggestion: " + recovery);
        
        ErrorHandler::getInstance().handleWarning(e);
    }
    
    try {
        // Example 3: Error with exception chaining
        std::vector<int> numbers = {1, 2, 3};
        size_t index = 5;
        NLM_CHECK_WITH_CODE(index < numbers.size(), OutOfBoundsError,
                           "Index out of bounds",
                           ErrorCode::ERROR_OUT_OF_BOUNDS);
        
        int value = numbers[index];
        
    } catch (const NLMError& e) {
        // Demonstrate exception chaining
        try {
            throw std::out_of_range("Index " + std::to_string(index) + " is out of range");
        } catch (const std::exception& cause) {
            throwWithCause(NLMError("Wrapper error with chained exception"), cause);
        }
    }
}

// Function that can recover from errors
bool recoverFromError(const std::string& operation) {
    ErrorContext ctx = ErrorUtils::createContext("recoverFromError");
    ctx.addParameter("operation", operation);
    
    try {
        if (operation == "network_training") {
            NLM_CHECK_WITH_CODE(false, BrainError,
                               "Simulating training failure",
                               ErrorCode::ERROR_CONVERGENCE_FAILED);
        } else if (operation == "memory_allocation") {
            NLM_CHECK_WITH_CODE(false, MemoryError,
                               "Simulating memory allocation failure",
                               ErrorCode::ERROR_OUT_OF_MEMORY);
        }
        
        return true;
        
    } catch (const NLMError& e) {
        // Check if error is recoverable
        if (ErrorHandler::isRecoverable(e)) {
            Logger::getInstance().info("Attempting recovery for: " + operation);
            
            // Simulate recovery
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
            // Try again with different parameters
            if (operation == "network_training") {
                Logger::getInstance().info("Retrying with smaller learning rate");
                return recoverFromError("network_training_small");
            } else if (operation == "memory_allocation") {
                Logger::getInstance().info("Clearing cache and retrying");
                return recoverFromError("memory_allocation_cleared");
            }
        }
        
        ErrorHandler::getInstance().handleError(e);
        return false;
    }
}

// Example of state rollback
class StateRollbackExample {
private:
    struct InternalState {
        float learningRate = 0.01f;
        int iterations = 0;
        float lastError = 0.0f;
        bool isTraining = false;
    };
    
    InternalState state_;
    std::stack<InternalState> rollbackStack_;
    
public:
    void startTraining(float learningRate) {
        // Save current state for rollback
        rollbackStack_.push(state_);
        state_.learningRate = learningRate;
        state_.iterations = 0;
        state_.isTraining = true;
        
        Logger::getInstance().info("Training started with learning rate: " + 
                                 std::to_string(learningRate));
    }
    
    void rollbackToPreviousState() {
        if (!rollbackStack_.empty()) {
            state_ = rollbackStack_.top();
            rollbackStack_.pop();
            Logger::getInstance().warning("State rolled back to previous configuration");
        }
    }
    
    void completeTraining(float error) {
        state_.lastError = error;
        state_.iterations++;
        state_.isTraining = false;
        
        Logger::getInstance().info("Training completed. Error: " + std::to_string(error));
    }
    
    void trainWithRecovery(float learningRate, float targetError) {
        startTraining(learningRate);
        
        try {
            for (int i = 0; i < 100 && state_.lastError > targetError; ++i) {
                // Simulate training step
                state_.lastError *= 0.95f; // Simulate error reduction
                state_.iterations++;
                
                // Check for convergence failure
                if (state_.iterations > 50 && state_.lastError > targetError * 1.1f) {
                    NLM_THROW(BrainError,
                             "Training did not converge within expected iterations",
                             ErrorCode::ERROR_CONVERGENCE_FAILED);
                }
            }
            
            completeTraining(state_.lastError);
            
        } catch (const NLMError& e) {
            if (ErrorHandler::isRecoverable(e)) {
                Logger::getInstance().warning("Training failed: " + e.getUserMessage());
                Logger::getInstance().info("Attempting recovery: rolling back state");
                rollbackToPreviousState();
                
                // Try again with different parameters
                float newLearningRate = learningRate * 0.5f;
                Logger::getInstance().info("Retrying with learning rate: " + 
                                         std::to_string(newLearningRate));
                trainWithRecovery(newLearningRate, targetError);
            } else {
                throw; // Re-throw non-recoverable errors
            }
        }
    }
};

// Example of error metrics collection
class ErrorMetricsCollector {
private:
    struct ErrorStats {
        size_t total = 0;
        size_t recoverable = 0;
        size_t critical = 0;
        std::map<ErrorCode, size_t> byCode;
        std::chrono::steady_clock::time_point firstError;
        std::chrono::steady_clock::time_point lastError;
    };
    
    ErrorStats stats_;
    
public:
    void recordError(const NLMError& error) {
        stats_.total++;
        stats_.lastError = std::chrono::steady_clock::now();
        
        if (!stats_.firstError.time_since_epoch().count()) {
            stats_.firstError = stats_.lastError;
        }
        
        if (ErrorHandler::isRecoverable(error)) {
            stats_.recoverable++;
        }
        
        if (error.getSeverity() == ErrorSeverity::Critical || 
            error.getSeverity() == ErrorSeverity::Fatal) {
            stats_.critical++;
        }
        
        stats_.byCode[error.getCode()]++;
        
        // Log periodically
        if (stats_.total % 100 == 0) {
            logMetrics();
        }
    }
    
    void recordSuccess() {
        // In a real implementation, you'd track successful operations too
    }
    
    void logMetrics() {
        Logger::getInstance().info("Error Metrics: Total=" + std::to_string(stats_.total) +
                                 ", Recoverable=" + std::to_string(stats_.recoverable) +
                                 ", Critical=" + std::to_string(stats_.critical));
        
        if (!stats_.byCode.empty()) {
            Logger::getInstance().info("Errors by code:");
            for (const auto& pair : stats_.byCode) {
                Logger::getInstance().info("  " + ErrorUtils::getErrorCodeString(pair.first) +
                                         ": " + std::to_string(pair.second));
            }
        }
    }
    
    float getErrorRate(size_t totalOperations) const {
        if (totalOperations == 0) return 0.0f;
        return static_cast<float>(stats_.total) / static_cast<float>(totalOperations);
    }
    
    float getRecoverableRate() const {
        if (stats_.total == 0) return 0.0f;
        return static_cast<float>(stats_.recoverable) / static_cast<float>(stats_.total);
    }
};

// Test function to demonstrate all features
void testErrorFramework() {
    Logger& logger = Logger::getInstance();
    logger.setLevel(LogLevel::Info);
    
    logger.info("Testing NLM Error Handling Framework");
    
    // Test 1: Basic error handling
    logger.info("Test 1: Basic error handling");
    demonstrateErrorHandling();
    
    // Test 2: Error recovery
    logger.info("Test 2: Error recovery");
    bool success = recoverFromError("network_training");
    logger.info("Recovery result: " + std::string(success ? "Success" : "Failed"));
    
    // Test 3: State rollback
    logger.info("Test 3: State rollback");
    StateRollbackExample example;
    example.trainWithRecovery(0.1f, 0.01f);
    
    // Test 4: Error metrics
    logger.info("Test 4: Error metrics collection");
    ErrorMetricsCollector collector;
    
    // Simulate some errors
    try {
        NLM_THROW(ConfigError, "Simulated configuration error");
    } catch (const NLMError& e) {
        collector.recordError(e);
    }
    
    try {
        NLM_THROW(MemoryError, "Simulated memory error");
    } catch (const NLMError& e) {
        collector.recordError(e);
    }
    
    // Log final metrics
    collector.logMetrics();
    
    logger.info("All tests completed");
}

} // namespace nlm
