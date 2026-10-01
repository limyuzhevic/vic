#include "NLMException.hpp"
#include "ConfigurationError.hpp"
#include "MemoryError.hpp"
#include "NeuralError.hpp"
#include "PlasticityError.hpp"
#include "IntegrationError.hpp"
#include "DevelopmentError.hpp"
#include "NeuromodulationError.hpp"
#include "MemorySystemError.hpp"
#include "PredictionError.hpp"
#include "CognitiveError.hpp"
#include "SensoryError.hpp"
#include "MotorError.hpp"
#include "WorldError.hpp"
#include "AgentError.hpp"
#include "ExceptionUtils.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <map>

// Example usage of the NLM exception hierarchy

void demonstrate_exception_handling() {
    std::cout << "=== NLM Exception Hierarchy Demonstration ===" << std::endl;
    
    // Example 1: Configuration error
    std::cout << "\n1. Configuration Error Example:" << std::endl;
    try {
        std::map<std::string, std::string> context = {
            {"file", "config.json"},
            {"line", "42"},
            {"key", "learning_rate"}
        };
        throw makeConfigurationError("Invalid learning rate value: -0.1", context);
    } catch (const ConfigurationError& e) {
        std::cout << "Caught ConfigurationError: " << e.getFullMessage() << std::endl;
    } catch (const NLMException& e) {
        std::cout << "Caught generic NLMException: " << e.getFullMessage() << std::endl;
    }
    
    // Example 2: Memory error
    std::cout << "\n2. Memory Error Example:" << std::endl;
    try {
        std::map<std::string, std::string> context = {
            {"operation", "allocate"},
            {"size", "1GB"},
            {"available", "512MB"}
        };
        throw makeMemoryError("Memory allocation failed", context);
    } catch (const MemoryError& e) {
        std::cout << "Caught MemoryError: " << e.getFullMessage() << std::endl;
    }
    
    // Example 3: Neural error
    std::cout << "\n3. Neural Error Example:" << std::endl;
    try {
        std::map<std::string, std::string> context = {
            {"neuron_id", "123"},
            {"operation", "compute_output"},
            {"region", "cortex"}
        };
        throw makeNeuralError("Neural computation overflow", context);
    } catch (const NeuralError& e) {
        std::cout << "Caught NeuralError: " << e.getFullMessage() << std::endl;
    }
    
    // Example 4: Integration error
    std::cout << "\n4. Integration Error Example:" << std::endl;
    try {
        std::map<std::string, std::string> context = {
            {"component", "PredictionSystem"},
            {"module", "Neuromodulation"},
            {"action", "initialize"}
        };
        throw makeIntegrationError("Failed to integrate neuromodulation with prediction system", context);
    } catch (const IntegrationError& e) {
        std::cout << "Caught IntegrationError: " << e.getFullMessage() << std::endl;
    }
    
    // Example 5: Development error
    std::cout << "\n5. Development Error Example:" << std::endl;
    try {
        std::map<std::string, std::string> context = {
            {"stage", "CriticalPeriod"},
            {"operation", "set_plasticity"},
            {"age", "30s"}
        };
        throw makeDevelopmentError("Development stage invalid for current age", context);
    } catch (const DevelopmentError& e) {
        std::cout << "Caught DevelopmentError: " << e.getFullMessage() << std::endl;
    }
    
    // Example 6: Neuromodulation error
    std::cout << "\n6. Neuromodulation Error Example:" << std::endl;
    try {
        std::map<std::string, std::string> context = {
            {"neuromodulator", "Dopamine"},
            {"operation", "apply_modulation"},
            {"reason", "invalid_concentration"}
        };
        throw makeNeuromodulationError("Invalid dopamine concentration for reward modulation", context);
    } catch (const NeuromodulationError& e) {
        std::cout << "Caught NeuromodulationError: " << e.getFullMessage() << std::endl;
    }
    
    // Example 7: Memory system error
    std::cout << "\n7. Memory System Error Example:" << std::endl;
    try {
        std::map<std::string, std::string> context = {
            {"memory_system", "EpisodicMemory"},
            {"operation", "encode"},
            {"event_id", "999"}
        };
        throw makeMemorySystemError("Failed to encode episodic memory event", context);
    } catch (const MemorySystemError& e) {
        std::cout << "Caught MemorySystemError: " << e.getFullMessage() << std::endl;
    }
    
    // Example 8: Prediction error
    std::cout << "\n8. Prediction Error Example:" << std::endl;
    try {
        std::map<std::string, std::string> context = {
            {"prediction_system", "NeuralPrediction"},
            {"operation", "generate_prediction"},
            {"step", "100"},
            {"error_magnitude", "0.85"}
        };
        throw makePredictionError("High prediction error - network instability detected", context);
    } catch (const PredictionError& e) {
        std::cout << "Caught PredictionError: " << e.getFullMessage() << std::endl;
    }
    
    // Example 9: Cognitive error
    std::cout << "\n9. Cognitive Error Example:" << std::endl;
    try {
        std::map<std::string, std::string> context = {
            {"cognitive_system", "NeuralPlanner"},
            {"operation", "plan_action"},
            {"reason", "infeasible_constraint"}
        };
        throw makeCognitiveError("Action planning failed - constraints infeasible", context);
    } catch (const CognitiveError& e) {
        std::cout << "Caught CognitiveError: " << e.getFullMessage() << std::endl;
    }
    
    // Example 10: Sensory error
    std::cout << "\n10. Sensory Error Example:" << std::endl;
    try {
        std::map<std::string, std::string> context = {
            {"sensory_system", "SensoryPercept"},
            {"operation", "process_vision"},
            {"sensor_id", "vision_0"},
            {"error", "corrupted_data"}
        };
        throw makeSensoryError("Sensory input corrupted - sensor malfunction", context);
    } catch (const SensoryError& e) {
        std::cout << "Caught SensoryError: " << e.getFullMessage() << std::endl;
    }
    
    // Example 11: Motor error
    std::cout << "\n11. Motor Error Example:" << std::endl;
    try {
        std::map<std::string, std::string> context = {
            {"motor_system", "MotorController"},
            {"operation", "execute_action"},
            {"action", "move_forward"},
            {"reason", "insufficient_power"}
        };
        throw makeMotorError("Motor action failed - insufficient power", context);
    } catch (const MotorError& e) {
        std::cout << "Caught MotorError: " << e.getFullMessage() << std::endl;
    }
    
    // Example 12: World error
    std::cout << "\n12. World Error Example:" << std::endl;
    try {
        std::map<std::string, std::string> context = {
            {"world_system", "SimpleWorld"},
            {"operation", "update_environment"},
            {"position", "x=100, y=200"},
            {"error", "collision_detected"}
        };
        throw makeWorldError("Environment collision detected - invalid world state", context);
    } catch (const WorldError& e) {
        std::cout << "Caught WorldError: " << e.getFullMessage() << std::endl;
    }
    
    // Example 13: Agent error
    std::cout << "\n13. Agent Error Example:" << std::endl;
    try {
        std::map<std::string, std::string> context = {
            {"agent_system", "AgentBrain"},
            {"operation", "initialize"},
            {"brain_id", "agent_001"},
            {"reason", "invalid_configuration"}
        };
        throw makeAgentError("Agent brain initialization failed - invalid configuration", context);
    } catch (const AgentError& e) {
        std::cout << "Caught AgentError: " << e.getFullMessage() << std::endl;
    }
    
    std::cout << "\n=== Exception Handling Summary ===" << std::endl;
    std::cout << "All 13 exception types demonstrated successfully!" << std::endl;
    std::cout << "Each exception includes: message, code, timestamp, context, and optional stack trace." << std::endl;
}

// Example of production code with try-catch patterns
void demonstrate_production_patterns() {
    std::cout << "\n=== Production Code Patterns ===" << std::endl;
    
    // Pattern 1: Try-catch with specific exception handlers
    std::cout << "\nPattern 1: Specific Exception Handlers" << std::endl;
    try {
        // Simulate some operation that might throw different exceptions
        int choice = 1; // 1=configuration, 2=memory, 3=neural
        if (choice == 1) {
            std::map<std::string, std::string> context = {
                {"file", "config.yaml"},
                {"line", "15"}
            };
            throw makeConfigurationError("Configuration file parsing failed", context);
        } else if (choice == 2) {
            throw makeMemoryError("Memory pool exhausted");
        } else {
            throw makeNeuralError("Neural network initialization failed");
        }
    } catch (const ConfigurationError& e) {
        std::cout << "Configuration error handled: " << e.getMessage() << std::endl;
    } catch (const MemoryError& e) {
        std::cout << "Memory error handled: " << e.getMessage() << std::endl;
    } catch (const NeuralError& e) {
        std::cout << "Neural error handled: " << e.getMessage() << std::endl;
    } catch (const NLMException& e) {
        std::cout << "Generic NLM error handled: " << e.getMessage() << std::endl;
    }
    
    // Pattern 2: Try-catch with error logging (using NLM logging)
    std::cout << "\nPattern 2: Error Logging Pattern" << std::endl;
    try {
        // Simulate an operation
        throw makeIntegrationError("Prediction system integration failed", {
            {"source", "prediction"},
            {"target", "neuromodulation"},
            {"step", "phase3"}
        });
    } catch (const NLMException& e) {
        NLM_LOG_ERROR(formatException(e));
        std::cout << "Exception logged and handled gracefully" << std::endl;
    }
    
    // Pattern 3: Exception propagation
    std::cout << "\nPattern 3: Exception Propagation" << std::endl;
    try {
        try {
            throw makeDevelopmentError("Development initialization failed", {
                {"stage", "Maturation"},
                {"dependency", "plasticity_system"}
            });
        } catch (const DevelopmentError& e) {
            // Add context before re-throwing
            std::map<std::string, std::string> context = e.getContext();
            context["caller"] = "brain_initializer";
            context["retry_attempt"] = "0";
            throw DevelopmentError(e.getMessage(), e.getCode(), context);
        }
    } catch (const DevelopmentError& e) {
        std::cout << "Propagated DevelopmentError: " << e.getFullMessage() << std::endl;
    }
}

void demonstrate_error_codes_and_context() {
    std::cout << "\n=== Error Codes and Context Information ===" << std::endl;
    
    // Create exceptions with various context information
    std::cout << "\nCreating exceptions with rich context information:" << std::endl;
    
    ConfigurationError configErr("Failed to load configuration", "CONFIG_LOAD_FAILED", {
        {"file", "brain_config.json"},
        {"format", "JSON"},
        {"size", "2.5MB"},
        {"checksum", "invalid"}
    });
    
    std::cout << "Configuration Error Details:" << std::endl;
    std::cout << "  Message: " << configErr.getMessage() << std::endl;
    std::cout << "  Code: " << configErr.getCode() << std::endl;
    std::cout << "  Timestamp: " << configErr.getTimestampString() << std::endl;
    std::cout << "  Context: ";
    const auto& configContext = configErr.getContext();
    for (const auto& pair : configContext) {
        std::cout << pair.first << "=" << pair.second << " ";
    }
    std::cout << std::endl;
    
    // Demonstrate stack trace
    std::cout << "\nDemonstrating stack trace tracking:" << std::endl;
    std::vector<std::string> trace = {
        "at main.cpp:42",
        "at brain_initializer.cpp:123",
        "at config_loader.cpp:89",
        "at file_reader.cpp:56"
    };
    configErr.addStackTrace(trace);
    std::cout << "Stack trace:" << std::endl << configErr.getStackTrace();
}

void demonstrate_exception_serialization() {
    std::cout << "\n=== Exception Serialization Example ===" << std::endl;
    
    // Demonstrate exception that could be serialized for logging/persisting
    std::cout << "Serializing exception to file:" << std::endl;
    
    std::map<std::string, std::string> context = {
        {"error_id", "ERR_2024_001"},
        {"severity", "HIGH"},
        {"component", "neuroscience_simulation"},
        {"subsystem", "prediction_engine"}
    };
    
    PredictionError predErr("Prediction divergence exceeds tolerance", "PREDICTION_TOLERANCE_EXCEEDED", context);
    
    // Write exception details to a string (simulating serialization)
    std::ostringstream serialized;
    serialized << "=== NLM EXCEPTION SERIALIZATION ===\n";
    serialized << "Timestamp: " << predErr.getTimestampString() << "\n";
    serialized << "Error Code: " << predErr.getCode() << "\n";
    serialized << "Message: " << predErr.getMessage() << "\n";
    serialized << "Context:\n";
    for (const auto& pair : predErr.getContext()) {
        serialized << "  " << pair.first << ": " << pair.second << "\n";
    }
    serialized << "Stack Trace:\n" << predErr.getStackTrace();
    
    std::cout << serialized.str();
    std::cout << "Exception serialized successfully (could be written to file/log system)" << std::endl;
}

int main() {
    std::cout << "NLM Exception Hierarchy Demonstration" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    demonstrate_exception_handling();
    demonstrate_production_patterns();
    demonstrate_error_codes_and_context();
    demonstrate_exception_serialization();
    
    std::cout << "\n=== All demonstrations completed ===" << std::endl;
    std::cout << "The NLM exception hierarchy provides comprehensive error handling capabilities" << std::endl;
    std::cout << "for all components of the neural learning machine system." << std::endl;
    
    return 0;
}
