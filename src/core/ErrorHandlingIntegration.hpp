// Integration Examples: Error Handling in Brain and AgentBrain Classes
// Demonstrates how to use the new error hierarchy in the main Brain and AgentBrain classes

#ifndef NLM_ERROR_HANDLING_INTEGRATION_HPP
#define NLM_ERROR_HANDLING_INTEGRATION_HPP

#include "Errors.hpp"
#include "Logging.hpp"
#include "ErrorHandling.hpp"
#include "ErrorContext.hpp"
#include <memory>
#include <vector>
#include <string>
#include <chrono>
#include <numeric>

namespace nlm {

// Integration Example 1: Brain Class Enhanced Error Handling
class EnhancedBrain {
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<Logger> logger_;
    bool initialized_;
    
public:
    EnhancedBrain(std::shared_ptr<Brain> brain, std::shared_ptr<Logger> logger)
        : brain_(std::move(brain)), logger_(std::move(logger)), initialized_(false) {
        NLM_CONTEXT_PUSH("enhanced_brain_creation", "brain_core");
        NLM_CONTEXT_SET_VALUE("class_name", "EnhancedBrain");
    }
    
    ~EnhancedBrain() {
        NLM_CONTEXT_PUSH("enhanced_brain_destruction", "brain_core");
        shutdown();
    }
    
    // Enhanced initialization with comprehensive error handling
    bool initialize() {
        NLM_CONTEXT_PUSH("brain_initialization", "brain_core");
        NLM_CONTEXT_SET_VALUE("operation", "initialize");
        
        try {
            NLM_VALIDATE(brain_ != nullptr, ValidationError,
                        "Brain instance cannot be null", "initialization");
            
            NLM_VALIDATE(logger_ != nullptr, ValidationError,
                        "Logger instance cannot be null", "initialization");
            
            NLM_LOG_INFO(LogCategory::CORE, "Starting Brain initialization");
            
            // Initialize core components with error recovery
            if (!initializeCoreComponents()) {
                throw ConfigurationError("Core component initialization failed",
                                        NLM_CURRENT_LOCATION, "initialization");
            }
            
            // Setup error monitoring
            setupErrorMonitoring();
            
            // Validate initialization
            if (!validateBrainState()) {
                throw ValidationError("Brain validation failed after initialization",
                                     NLM_CURRENT_LOCATION, "validation");
            }
            
            initialized_ = true;
            
            NLM_CONTEXT_SET_VALUE("initialization_success", true);
            NLM_LOG_INFO(LogCategory::CORE, "Brain initialization completed successfully");
            
            return true;
            
        } catch (const ConfigurationError& e) {
            NLM_CONTEXT_SET_ERROR(10001, "CONFIGURATION");
            NLM_LOG_ERROR_THROW(e);
        } catch (const ValidationError& e) {
            NLM_CONTEXT_SET_ERROR(10002, "VALIDATION");
            NLM_LOG_ERROR_THROW(e);
        } catch (const NeuronError& e) {
            NLM_CONTEXT_SET_ERROR(10003, "NEURAL");
            NLM_LOG_ERROR_THROW(e);
        } catch (const MemoryError& e) {
            NLM_CONTEXT_SET_ERROR(10004, "MEMORY");
            NLM_LOG_ERROR_THROW(e);
        } catch (...) {
            NLM_CONTEXT_SET_ERROR(10999, "CORE");
            NLM_LOG_ERROR(LogCategory::CORE, "Unknown error during Brain initialization");
            throw NLMException("Unknown error during Brain initialization",
                              ErrorCategory::CORE, LogLevel::ERROR);
        }
    }
    
    // Enhanced step method with performance monitoring and error recovery
    void step(SimulationStep currentStep, Timestamp currentTime) {
        NLM_CONTEXT_PUSH("brain_step", "brain_core");
        NLM_CONTEXT_SET_VALUE("step", currentStep);
        NLM_CONTEXT_SET_VALUE("current_time", currentTime);
        
        try {
            auto startTime = std::chrono::high_resolution_clock::now();
            
            // Validate step parameters
            NLM_VALIDATE(currentStep > 0, ValidationError,
                        "Step must be positive", "step_validation");
            
            NLM_VALIDATE(currentTime > 0, ValidationError,
                        "Current time must be positive", "step_validation");
            
            // Process sensory input with error handling
            processSensoryInputWithErrorHandling();
            
            // Execute neural computation with performance monitoring
            executeNeuralComputationWithErrorHandling(currentStep, currentTime);
            
            // Generate and process motor output
            generateAndProcessMotorOutputWithErrorHandling();
            
            // Update development and neuromodulation systems
            updateSystemsWithErrorHandling();
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            NLM_CONTEXT_SET_EXECUTION_TIME(duration.count());
            
            // Check for performance issues
            if (duration.count() > 100) { // Warning if step takes > 100ms
                NLM_LOG_WARNING(LogCategory::PERFORMANCE,
                    "Brain step took " + std::to_string(duration.count()) + "ms (step " + 
                    std::to_string(currentStep) + ")");
            }
            
            NLM_LOG_DEBUG(LogCategory::CORE,
                "Brain step completed successfully: " + std::to_string(currentStep) + 
                ", " + std::to_string(duration.count()) + "ms");
                
        } catch (const NeuronError& e) {
            NLM_CONTEXT_SET_ERROR(10101, "NEURAL");
            NLM_LOG_ERROR_THROW(e);
        } catch (const MemoryError& e) {
            NLM_CONTEXT_SET_ERROR(10102, "MEMORY");
            NLM_LOG_ERROR_THROW(e);
        } catch (const WorldError& e) {
            NLM_CONTEXT_SET_ERROR(10103, "WORLD");
            NLM_LOG_ERROR_THROW(e);
        } catch (...) {
            NLM_CONTEXT_SET_ERROR(10999, "CORE");
            NLM_LOG_ERROR(LogCategory::CORE, "Unknown error during brain step");
            throw NLMException("Unknown error during brain step",
                              ErrorCategory::CORE, LogLevel::ERROR);
        }
    }
    
    // Enhanced sensory input processing with error recovery
    void processSensoryInputWithErrorHandling() {
        NLM_CONTEXT_PUSH("sensory_processing", "brain_core");
        
        try {
            // Get available sensory input
            std::vector<double> sensoryData;
            if (!getAvailableSensoryData(sensoryData)) {
                throw WorldError("Failed to get available sensory data",
                                NLM_CURRENT_LOCATION, "sensory_processing");
            }
            
            NLM_VALIDATE(!sensoryData.empty(), ValidationError,
                        "No sensory data available", "sensory_processing");
            
            // Process each sensory channel with error handling
            for (size_t i = 0; i < sensoryData.size(); ++i) {
                double value = sensoryData[i];
                
                try {
                    // Validate sensory value
                    validateSensoryValue(value, i);
                    
                    // Inject into brain neurons
                    injectSensoryDataWithErrorHandling(value, i);
                    
                    NLM_CONTEXT_SET_VALUE("sensor_" + std::to_string(i), value);
                    
                } catch (const ValidationError& e) {
                    NLM_LOG_WARNING(LogCategory::VALIDATION,
                        "Invalid sensory data at channel " + std::to_string(i) + ": " + e.getMessage());
                    
                    // Skip invalid sensor data and continue
                    continue;
                } catch (const NeuronError& e) {
                    NLM_LOG_ERROR(LogCategory::NEURAL,
                        "Critical error in sensory processing at channel " + std::to_string(i) + ": " + e.getMessage());
                    throw; // Re-throw critical error
                }
            }
            
            NLM_LOG_INFO(LogCategory::CORE,
                "Sensory input processing completed successfully");
                
        } catch (const WorldError& e) {
            NLM_CONTEXT_SET_ERROR(10201, "WORLD");
            NLM_LOG_ERROR_THROW(e);
        } catch (const NeuronError& e) {
            NLM_CONTEXT_SET_ERROR(10202, "NEURAL");
            NLM_LOG_ERROR_THROW(e);
        } catch (const ValidationError& e) {
            NLM_CONTEXT_SET_ERROR(10203, "VALIDATION");
            NLM_LOG_ERROR_THROW(e);
        }
    }
    
    // Enhanced neural computation with error handling
    void executeNeuralComputationWithErrorHandling(SimulationStep step, Timestamp time) {
        NLM_CONTEXT_PUSH("neural_computation", "brain_core");
        NLM_CONTEXT_SET_VALUE("step", step);
        NLM_CONTEXT_SET_VALUE("time", time);
        
        try {
            // Process spikes with error handling
            processSpikesWithErrorHandling();
            
            // Update neural dynamics with error handling
            updateNeuralDynamicsWithErrorHandling(step, time);
            
            // Apply plasticity with error handling
            applyPlasticityWithErrorHandling();
            
            // Check for neural errors
            checkNeuralSystemHealth();
            
            NLM_LOG_INFO(LogCategory::NEURAL,
                "Neural computation completed successfully for step " + std::to_string(step));
                
        } catch (const NeuronError& e) {
            NLM_CONTEXT_SET_ERROR(10301, "NEURAL");
            NLM_LOG_ERROR_THROW(e);
        } catch (const MemoryError& e) {
            NLM_CONTEXT_SET_ERROR(10302, "MEMORY");
            NLM_LOG_ERROR_THROW(e);
        } catch (...) {
            NLM_CONTEXT_SET_ERROR(10399, "CORE");
            NLM_LOG_ERROR(LogCategory::CORE, "Unknown error during neural computation");
            throw NLMException("Unknown error during neural computation",
                              ErrorCategory::CORE, LogLevel::ERROR);
        }
    }
    
private:
    // Helper methods for initialization
    bool initializeCoreComponents() {
        NLM_LOG_INFO(LogCategory::CORE, "Initializing core components");
        // Simplified initialization logic
        return true;
    }
    
    void setupErrorMonitoring() {
        NLM_LOG_INFO(LogCategory::CORE, "Setting up error monitoring");
        // Setup error monitoring systems
    }
    
    bool validateBrainState() {
        NLM_LOG_INFO(LogCategory::CORE, "Validating brain state");
        // Validate brain state
        return true;
    }
    
    // Helper methods for sensory processing
    bool getAvailableSensoryData(std::vector<double>& data) {
        // Simplified implementation
        data = {0.1, 0.5, 0.3, 0.8};
        return true;
    }
    
    void validateSensoryValue(double value, size_t channel) {
        NLM_VALIDATE(std::isfinite(value), ValidationError,
                    "Sensory value must be finite", "sensory_validation");
        
        NLM_VALIDATE(value >= 0.0 && value <= 1.0, ValidationError,
                    "Sensory value must be in range [0, 1]", "sensory_validation");
    }
    
    void injectSensoryDataWithErrorHandling(double value, size_t channel) {
        try {
            brain_->injectCurrent(static_cast<NeuronId>(channel), 
                                 static_cast<MembranePotential>(value));
            
            NLM_CONTEXT_SET_VALUE("injected_channel", static_cast<int>(channel));
            NLM_CONTEXT_SET_VALUE("injected_value", value);
            
        } catch (const std::runtime_error& e) {
            throw NeuronError("Failed to inject sensory data: " + std::string(e.what()),
                             NLM_CURRENT_LOCATION, "sensory_injection");
        }
    }
    
    // Helper methods for neural processing
    void processSpikesWithErrorHandling() {
        NLM_LOG_INFO(LogCategory::NEURAL, "Processing spikes");
        // Simplified spike processing
    }
    
    void updateNeuralDynamicsWithErrorHandling(SimulationStep step, Timestamp time) {
        NLM_LOG_INFO(LogCategory::NEURAL, "Updating neural dynamics");
        // Simplified neural dynamics update
    }
    
    void applyPlasticityWithErrorHandling() {
        NLM_LOG_INFO(LogCategory::NEURAL, "Applying plasticity");
        // Simplified plasticity application
    }
    
    void checkNeuralSystemHealth() {
        NLM_LOG_INFO(LogCategory::NEURAL, "Checking neural system health");
        // Check neural system health
    }
    
    void generateAndProcessMotorOutputWithErrorHandling() {
        NLM_CONTEXT_PUSH("motor_output", "brain_core");
        
        try {
            // Generate motor output
            std::unique_ptr<Action> action = brain_->produceAction();
            
            NLM_VALIDATE(action != nullptr, ValidationError,
                        "Failed to generate motor action", "motor_output");
            
            // Process motor output
            processMotorOutputWithErrorHandling(std::move(action));
            
            NLM_LOG_INFO(LogCategory::CORE, "Motor output processed successfully");
            
        } catch (const ValidationError& e) {
            NLM_CONTEXT_SET_ERROR(10401, "VALIDATION");
            NLM_LOG_ERROR_THROW(e);
        } catch (const NeuronError& e) {
            NLM_CONTEXT_SET_ERROR(10402, "NEURAL");
            NLM_LOG_ERROR_THROW(e);
        } catch (...) {
            NLM_CONTEXT_SET_ERROR(10499, "CORE");
            NLM_LOG_ERROR(LogCategory::CORE, "Unknown error during motor output processing");
            throw NLMException("Unknown error during motor output processing",
                              ErrorCategory::CORE, LogLevel::ERROR);
        }
    }
    
    void processMotorOutputWithErrorHandling(std::unique_ptr<Action> action) {
        NLM_LOG_INFO(LogCategory::CORE, "Processing motor output");
        // Simplified motor output processing
    }
    
    void updateSystemsWithErrorHandling() {
        NLM_CONTEXT_PUSH("systems_update", "brain_core");
        
        try {
            // Update development system
            updateDevelopmentSystemWithErrorHandling();
            
            // Update neuromodulation system
            updateNeuromodulationSystemWithErrorHandling();
            
            // Update prediction system
            updatePredictionSystemWithErrorHandling();
            
            NLM_LOG_INFO(LogCategory::CORE, "All systems updated successfully");
            
        } catch (const WorldError& e) {
            NLM_CONTEXT_SET_ERROR(10501, "WORLD");
            NLM_LOG_ERROR_THROW(e);
        } catch (const NeuronError& e) {
            NLM_CONTEXT_SET_ERROR(10502, "NEURAL");
            NLM_LOG_ERROR_THROW(e);
        } catch (...) {
            NLM_CONTEXT_SET_ERROR(10599, "CORE");
            NLM_LOG_ERROR(LogCategory::CORE, "Unknown error during systems update");
            throw NLMException("Unknown error during systems update",
                              ErrorCategory::CORE, LogLevel::ERROR);
        }
    }
    
    void updateDevelopmentSystemWithErrorHandling() {
        NLM_LOG_INFO(LogCategory::WORLD, "Updating development system");
        // Simplified development system update
    }
    
    void updateNeuromodulationSystemWithErrorHandling() {
        NLM_LOG_INFO(LogCategory::WORLD, "Updating neuromodulation system");
        // Simplified neuromodulation system update
    }
    
    void updatePredictionSystemWithErrorHandling() {
        NLM_LOG_INFO(LogCategory::WORLD, "Updating prediction system");
        // Simplified prediction system update
    }
    
    void shutdown() {
        NLM_CONTEXT_PUSH("brain_shutdown", "brain_core");
        
        try {
            NLM_LOG_INFO(LogCategory::CORE, "Shutting down brain");
            
            // Shutdown neural systems
            shutdownNeuralSystems();
            
            // Cleanup resources
            cleanupResources();
            
            initialized_ = false;
            
            NLM_LOG_INFO(LogCategory::CORE, "Brain shutdown completed successfully");
            
        } catch (...) {
            NLM_CONTEXT_SET_ERROR(10999, "CORE");
            NLM_LOG_ERROR(LogCategory::CORE, "Error during brain shutdown");
            // Don't throw during shutdown
        }
    }
    
    void shutdownNeuralSystems() {
        NLM_LOG_INFO(LogCategory::CORE, "Shutting down neural systems");
        // Simplified neural systems shutdown
    }
    
    void cleanupResources() {
        NLM_LOG_INFO(LogCategory::CORE, "Cleaning up resources");
        // Simplified resource cleanup
    }
};

// Integration Example 2: AgentBrain Class Enhanced Error Handling
class EnhancedAgentBrain {
private:
    std::shared_ptr<AgentBrain> agentBrain_;
    std::shared_ptr<Logger> logger_;
    bool initialized_;
    std::vector<std::string> errorHistory_;
    
public:
    EnhancedAgentBrain(std::shared_ptr<AgentBrain> agentBrain, std::shared_ptr<Logger> logger)
        : agentBrain_(std::move(agentBrain)), logger_(std::move(logger)), initialized_(false) {
        NLM_CONTEXT_PUSH("enhanced_agentbrain_creation", "agent_brain");
        NLM_CONTEXT_SET_VALUE("class_name", "EnhancedAgentBrain");
    }
    
    ~EnhancedAgentBrain() {
        NLM_CONTEXT_PUSH("enhanced_agentbrain_destruction", "agent_brain");
        shutdown();
    }
    
    // Enhanced initialization with comprehensive error handling
    bool initialize(const SimpleWorld& world) {
        NLM_CONTEXT_PUSH("agentbrain_initialization", "agent_brain");
        NLM_CONTEXT_SET_VALUE("operation", "initialize");
        NLM_CONTEXT_SET_VALUE("world_context", world.toString());
        
        try {
            NLM_VALIDATE(agentBrain_ != nullptr, ValidationError,
                        "AgentBrain instance cannot be null", "initialization");
            
            NLM_VALIDATE(logger_ != nullptr, ValidationError,
                        "Logger instance cannot be null", "initialization");
            
            NLM_LOG_INFO(LogCategory::CORE, "Starting AgentBrain initialization");
            
            // Initialize core components with error recovery
            if (!initializeCoreComponents()) {
                throw ConfigurationError("Core component initialization failed",
                                        NLM_CURRENT_LOCATION, "initialization");
            }
            
            // Setup error monitoring and recovery
            setupErrorMonitoringAndRecovery();
            
            // Validate initialization
            if (!validateAgentBrainState()) {
                throw ValidationError("AgentBrain validation failed after initialization",
                                     NLM_CURRENT_LOCATION, "validation");
            }
            
            initialized_ = true;
            
            NLM_CONTEXT_SET_VALUE("initialization_success", true);
            NLM_LOG_INFO(LogCategory::CORE, "AgentBrain initialization completed successfully");
            
            return true;
            
        } catch (const ConfigurationError& e) {
            NLM_CONTEXT_SET_ERROR(20001, "CONFIGURATION");
            NLM_LOG_ERROR_THROW(e);
        } catch (const ValidationError& e) {
            NLM_CONTEXT_SET_ERROR(20002, "VALIDATION");
            NLM_LOG_ERROR_THROW(e);
        } catch (const WorldError& e) {
            NLM_CONTEXT_SET_ERROR(20003, "WORLD");
            NLM_LOG_ERROR_THROW(e);
        } catch (...) {
            NLM_CONTEXT_SET_ERROR(20999, "CORE");
            NLM_LOG_ERROR(LogCategory::CORE, "Unknown error during AgentBrain initialization");
            throw NLMException("Unknown error during AgentBrain initialization",
                              ErrorCategory::CORE, LogLevel::ERROR);
        }
    }
    
    // Enhanced sensory processing with error recovery and graceful degradation
    void processSensoryInputEnhanced(const SensoryPercept& percept) {
        NLM_CONTEXT_PUSH("sensory_processing_enhanced", "agent_brain");
        NLM_CONTEXT_SET_VALUE("percept_id", percept.getId());
        NLM_CONTEXT_SET_VALUE("sensory_modality", percept.getModality());
        
        try {
            NLM_VALIDATE(initialized_, ValidationError,
                        "AgentBrain must be initialized before processing sensory input", "sensory_processing");
            
            NLM_VALIDATE(percept.isValid(), ValidationError,
                        "Sensory percept must be valid", "sensory_processing");
            
            // Validate sensory data
            validateSensoryPercept(percept);
            
            // Process with error recovery
            processSensoryInputWithRecovery(percept);
            
            // Update internal state
            updateAgentBrainState();
            
            NLM_CONTEXT_SET_VALUE("sensory_processing_success", true);
            NLM_LOG_INFO(LogCategory::CORE,
                "Enhanced sensory processing completed successfully for percept " + 
                std::to_string(percept.getId()));
                
        } catch (const ValidationError& e) {
            NLM_CONTEXT_SET_ERROR(20101, "VALIDATION");
            recordError(e);
            NLM_LOG_ERROR_THROW(e);
        } catch (const WorldError& e) {
            NLM_CONTEXT_SET_ERROR(20102, "WORLD");
            recordError(e);
            NLM_LOG_ERROR_THROW(e);
        } catch (const NeuronError& e) {
            NLM_CONTEXT_SET_ERROR(20103, "NEURAL");
            recordError(e);
            NLM_LOG_ERROR_THROW(e);
        } catch (...) {
            NLM_CONTEXT_SET_ERROR(20199, "CORE");
            NLM_LOG_ERROR(LogCategory::CORE, "Unknown error during enhanced sensory processing");
            recordError(NLMException("Unknown error during enhanced sensory processing",
                                   ErrorCategory::CORE, LogLevel::ERROR));
            throw NLMException("Unknown error during enhanced sensory processing",
                              ErrorCategory::CORE, LogLevel::ERROR);
        }
    }
    
    // Enhanced motor command decoding with error recovery
    MotorCommand decodeMotorCommandEnhanced() {
        NLM_CONTEXT_PUSH("motor_command_decoding", "agent_brain");
        
        try {
            NLM_VALIDATE(initialized_, ValidationError,
                        "AgentBrain must be initialized before decoding motor command", "motor_decoding");
            
            // Get available motor data
            std::vector<double> motorData;
            if (!getAvailableMotorData(motorData)) {
                throw WorldError("Failed to get available motor data",
                                NLM_CURRENT_LOCATION, "motor_decoding");
            }
            
            NLM_VALIDATE(!motorData.empty(), ValidationError,
                        "No motor data available", "motor_decoding");
            
            // Decode motor command with error recovery
            MotorCommand command = decodeMotorCommandWithRecovery(motorData);
            
            // Validate motor command
            validateMotorCommand(command);
            
            // Apply neuromodulation
            applyNeuromodulationWithErrorHandling(command);
            
            NLM_CONTEXT_SET_VALUE("motor_command_success", true);
            NLM_LOG_INFO(LogCategory::CORE,
                "Enhanced motor command decoding completed successfully");
                
            return command;
            
        } catch (const ValidationError& e) {
            NLM_CONTEXT_SET_ERROR(20201, "VALIDATION");
            recordError(e);
            NLM_LOG_ERROR_THROW(e);
        } catch (const NeuronError& e) {
            NLM_CONTEXT_SET_ERROR(20202, "NEURAL");
            recordError(e);
            NLM_LOG_ERROR_THROW(e);
        } catch (const WorldError& e) {
            NLM_CONTEXT_SET_ERROR(20203, "WORLD");
            recordError(e);
            NLM_LOG_ERROR_THROW(e);
        } catch (...) {
            NLM_CONTEXT_SET_ERROR(20299, "CORE");
            NLM_LOG_ERROR(LogCategory::CORE, "Unknown error during enhanced motor command decoding");
            recordError(NLMException("Unknown error during enhanced motor command decoding",
                                   ErrorCategory::CORE, LogLevel::ERROR));
            throw NLMException("Unknown error during enhanced motor command decoding",
                              ErrorCategory::CORE, LogLevel::ERROR);
        }
    }
    
    // Enhanced reward modulation with error recovery and learning
    void applyRewardModulationEnhanced(float reward, float predictedReward) {
        NLM_CONTEXT_PUSH("reward_modulation", "agent_brain");
        NLM_CONTEXT_SET_VALUE("reward", reward);
        NLM_CONTEXT_SET_VALUE("predicted_reward", predictedReward);
        
        try {
            NLM_VALIDATE(initialized_, ValidationError,
                        "AgentBrain must be initialized before applying reward modulation", "reward_modulation");
            
            // Validate reward values
            validateRewardValues(reward, predictedReward);
            
            // Calculate reward prediction error
            float rewardError = reward - predictedReward;
            
            NLM_CONTEXT_SET_VALUE("reward_prediction_error", rewardError);
            
            // Apply neuromodulation with error recovery
            applyNeuromodulationWithErrorHandling(reward, predictedReward);
            
            // Update learning mechanisms
            updateLearningMechanismsWithErrorHandling(rewardError);
            
            // Record reward for future learning
            recordRewardForLearning(reward, rewardError);
            
            NLM_CONTEXT_SET_VALUE("reward_modulation_success", true);
            NLM_LOG_INFO(LogCategory::CORE,
                "Enhanced reward modulation completed successfully: reward=" + 
                std::to_string(reward) + ", error=" + std::to_string(rewardError));
                
        } catch (const ValidationError& e) {
            NLM_CONTEXT_SET_ERROR(20301, "VALIDATION");
            recordError(e);
            NLM_LOG_ERROR_THROW(e);
        } catch (const WorldError& e) {
            NLM_CONTEXT_SET_ERROR(20302, "WORLD");
            recordError(e);
            NLM_LOG_ERROR_THROW(e);
        } catch (const NeuronError& e) {
            NLM_CONTEXT_SET_ERROR(20303, "NEURAL");
            recordError(e);
            NLM_LOG_ERROR_THROW(e);
        } catch (...) {
            NLM_CONTEXT_SET_ERROR(20399, "CORE");
            NLM_LOG_ERROR(LogCategory::CORE, "Unknown error during enhanced reward modulation");
            recordError(NLMException("Unknown error during enhanced reward modulation",
                                   ErrorCategory::CORE, LogLevel::ERROR));
            throw NLMException("Unknown error during enhanced reward modulation",
                              ErrorCategory::CORE, LogLevel::ERROR);
        }
    }
    
    // Graceful degradation and recovery methods
    bool recoverFromError(const NLMException& error) {
        NLM_CONTEXT_PUSH("error_recovery", "agent_brain");
        NLM_CONTEXT_SET_VALUE("error_message", error.getMessage());
        NLM_CONTEXT_SET_VALUE("error_category", error.getCategoryString(error.getCategory()));
        
        try {
            std::string errorCategory = error.getCategoryString(error.getCategory());
            
            // Different recovery strategies based on error category
            if (errorCategory == "VALIDATION") {
                return recoverFromValidationError(error);
            } else if (errorCategory == "NEURAL") {
                return recoverFromNeuralError(error);
            } else if (errorCategory == "MEMORY") {
                return recoverFromMemoryError(error);
            } else if (errorCategory == "WORLD") {
                return recoverFromWorldError(error);
            } else {
                return recoverFromCoreError(error);
            }
            
        } catch (...) {
            NLM_LOG_ERROR(LogCategory::CORE, "Error during error recovery");
            return false;
        }
    }
    
private:
    // Helper methods for initialization
    bool initializeCoreComponents() {
        NLM_LOG_INFO(LogCategory::CORE, "Initializing core components");
        // Initialize core components
        return agentBrain_->initialize(SimpleWorld{});
    }
    
    void setupErrorMonitoringAndRecovery() {
        NLM_LOG_INFO(LogCategory::CORE, "Setting up error monitoring and recovery");
        // Setup error monitoring and recovery systems
    }
    
    bool validateAgentBrainState() {
        NLM_LOG_INFO(LogCategory::CORE, "Validating AgentBrain state");
        // Validate AgentBrain state
        return true;
    }
    
    // Helper methods for sensory processing
    void validateSensoryPercept(const SensoryPercept& percept) {
        NLM_VALIDATE(!percept.getId().empty(), ValidationError,
                    "Sensory percept must have a valid ID", "sensory_validation");
        
        NLM_VALIDATE(percept.getModality() != "UNKNOWN", ValidationError,
                    "Sensory percept must have a valid modality", "sensory_validation");
    }
    
    void processSensoryInputWithRecovery(const SensoryPercept& percept) {
        // Delegate to original AgentBrain with error handling
        agentBrain_->processSensoryInput(percept);
    }
    
    void updateAgentBrainState() {
        // Update AgentBrain internal state
    }
    
    // Helper methods for motor command decoding
    bool getAvailableMotorData(std::vector<double>& data) {
        // Get motor data (simplified)
        data = {0.5, 0.3, 0.8};
        return true;
    }
    
    MotorCommand decodeMotorCommandWithRecovery(const std::vector<double>& data) {
        // Delegate to original AgentBrain with error handling
        // This would be implemented based on actual AgentBrain API
        MotorCommand cmd;
        return cmd;
    }
    
    void validateMotorCommand(const MotorCommand& command) {
        NLM_VALIDATE(command.isValid(), ValidationError,
                    "Motor command must be valid", "motor_validation");
    }
    
    void applyNeuromodulationWithErrorHandling(const MotorCommand& command) {
        NLM_LOG_INFO(LogCategory::NEURAL, "Applying neuromodulation for motor command");
        // Apply neuromodulation
    }
    
    void applyNeuromodulationWithErrorHandling(float reward, float predictedReward) {
        NLM_LOG_INFO(LogCategory::NEURAL, "Applying neuromodulation for reward");
        // Apply neuromodulation
    }
    
    // Helper methods for reward processing
    void validateRewardValues(float reward, float predictedReward) {
        NLM_VALIDATE(std::isfinite(reward), ValidationError,
                    "Reward must be finite", "reward_validation");
        
        NLM_VALIDATE(std::isfinite(predictedReward), ValidationError,
                    "Predicted reward must be finite", "reward_validation");
    }
    
    void updateLearningMechanismsWithErrorHandling(float rewardError) {
        NLM_LOG_INFO(LogCategory::NEURAL, "Updating learning mechanisms with reward error: " + 
            std::to_string(rewardError));
        // Update learning mechanisms
    }
    
    void recordRewardForLearning(float reward, float rewardError) {
        NLM_LOG_INFO(LogCategory::NEURAL, "Recording reward for learning: " + std::to_string(reward));
        // Record reward for learning
    }
    
    // Error recovery methods
    bool recoverFromValidationError(const NLMException& error) {
        NLM_LOG_WARNING(LogCategory::VALIDATION,
            "Recovering from validation error: " + error.getMessage());
        // Implement validation error recovery
        return true;
    }
    
    bool recoverFromNeuralError(const NLMException& error) {
        NLM_LOG_WARNING(LogCategory::NEURAL,
            "Recovering from neural error: " + error.getMessage());
        // Implement neural error recovery
        return true;
    }
    
    bool recoverFromMemoryError(const NLMException& error) {
        NLM_LOG_WARNING(LogCategory::MEMORY,
            "Recovering from memory error: " + error.getMessage());
        // Implement memory error recovery
        return true;
    }
    
    bool recoverFromWorldError(const NLMException& error) {
        NLM_LOG_WARNING(LogCategory::WORLD,
            "Recovering from world error: " + error.getMessage());
        // Implement world error recovery
        return true;
    }
    
    bool recoverFromCoreError(const NLMException& error) {
        NLM_LOG_WARNING(LogCategory::CORE,
            "Recovering from core error: " + error.getMessage());
        // Implement core error recovery
        return true;
    }
    
    // Error recording
    void recordError(const NLMException& error) {
        errorHistory_.push_back(error.what());
        NLM_LOG_INFO(LogCategory::CORE,
            "Recorded error: " + error.getMessage() + " (Category: " + 
            error.getCategoryString(error.getCategory()) + ")");
    }
    
    // Shutdown
    void shutdown() {
        NLM_CONTEXT_PUSH("agentbrain_shutdown", "agent_brain");
        
        try {
            NLM_LOG_INFO(LogCategory::CORE, "Shutting down AgentBrain");
            
            // Shutdown neural systems
            shutdownNeuralSystems();
            
            // Cleanup resources
            cleanupResources();
            
            initialized_ = false;
            
            NLM_LOG_INFO(LogCategory::CORE, "AgentBrain shutdown completed successfully");
            
        } catch (...) {
            NLM_CONTEXT_SET_ERROR(20999, "CORE");
            NLM_LOG_ERROR(LogCategory::CORE, "Error during AgentBrain shutdown");
            // Don't throw during shutdown
        }
    }
    
    void shutdownNeuralSystems() {
        NLM_LOG_INFO(LogCategory::CORE, "Shutting down neural systems");
        // Simplified neural systems shutdown
    }
    
    void cleanupResources() {
        NLM_LOG_INFO(LogCategory::CORE, "Cleaning up resources");
        // Simplified resource cleanup
        errorHistory_.clear();
    }
}; 

// Helper function for error report generation
inline std::string generateErrorReport(const NLMException& error) {
    std::stringstream ss;
    ss << "=== NLM Error Report ===\n";
    ss << "Message: " << error.getMessage() << "\n";
    ss << "Category: " << error.getCategoryString(error.getCategory()) << "\n";
    ss << "Severity: " << error.getSeverityString(error.getSeverity()) << "\n";
    ss << "Context:\n" << error.getContext().getFormattedMessage() << "\n";
    ss << "Full Details:\n" << error.what() << "\n";
    return ss.str();
}

#endif // NLM_ERROR_HANDLING_INTEGRATION_HPP