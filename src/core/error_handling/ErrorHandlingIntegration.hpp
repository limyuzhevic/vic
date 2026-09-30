// Include the core exception hierarchy for brain error handling
#pragma once

#include "exceptions/BrainException.hpp"
#include "validation/Validator.hpp"
#include <map>
#include <vector>
#include <memory>
#include <string>
#include <vector>

namespace nlm {

/**
 * Comprehensive error handling and validation for the NLM brain system
 * 
 * This file contains error handling and validation implementations for:
 * 1. Configuration parameter validation
 * 2. Memory system state validation
 * 3. Neural system initialization validation
 * 4. Synaptic connectivity validation
 * 5. Developmental stage transition validation
 * 6. Error recovery mechanisms
 * 7. User-friendly error messages
 * 8. Integration into all major systems
 */

// Forward declarations
class Brain;
class NeuralRegion;
class Neuron;
class Synapse;

/**
 * Configuration validation and error handling
 */
class ConfigurationValidator {
public:
    static void validateConfigParameters(const std::shared_ptr<Config>& config);
    static void validateNeuronParameters(const std::map<std::string, double>& params);
    static void validateSynapseParameters(const std::map<std::string, double>& params);
    static void validateMemoryParameters(const std::map<std::string, size_t>& params);
    static void validatePlasticityParameters(const std::map<std::string, double>& params);
    static void validateNeuromodulationParameters(const std::map<std::string, double>& params);
};

/**
 * Memory system validation and error handling
 */
class MemoryValidator {
public:
    static void validateMemoryState(const Brain* brain);
    static void validateMemoryAccess(const void* ptr, size_t size);
    static void validateMemoryCapacity(size_t required, size_t available);
    static void validateMemoryAllocation(size_t size, const std::string& purpose);
    static void handleMemoryError(const std::exception& e);
};

/**
 * Neural system validation and error handling
 */
class NeuralValidator {
public:
    static void validateNeuralSystemInitialization(const Brain* brain);
    static void validateNeuronParameters(const Neuron& neuron);
    static void validateSynapticConnectivity(const Synapse& synapse);
    static void validateNeuralPopulation(const NeuralRegion& region);
    static void handleNeuralSystemError(const std::exception& e);
};

/**
 * Plasticity system validation and error handling
 */
class PlasticityValidator {
public:
    static void validatePlasticityRules(const PlasticityFlags& flags);
    static void validatePlasticityParameters(const std::map<std::string, double>& params);
    static void validateLearningRules(const std::vector<std::string>& rules);
    static void validateWeightChanges(const Synapse& synapse, double delta);
    static void handlePlasticityError(const std::exception& e);
};

/**
 * Neuromodulation system validation and error handling
 */
class NeuromodulationValidator {
public:
    static void validateNeuromodulatorLevels(const Neuromodulator& signal);
    static void validateNeuromodulatorParameters(const std::map<std::string, double>& params);
    static void validateNeuromodulationTiming(const std::map<std::string, double>& timing);
    static void handleNeuromodulationError(const std::exception& e);
};

/**
 * Developmental system validation and error handling
 */
class DevelopmentalValidator {
public:
    static void validateDevelopmentalStage(const DevelopmentalStage& stage);
    static void validateStageTransition(const DevelopmentalStage& from, const DevelopmentalStage& to);
    static void validateCriticalPeriod(const std::map<std::string, double>& period);
    static void handleDevelopmentalError(const std::exception& e);
};

/**
 * Prediction system validation and error handling
 */
class PredictionValidator {
public:
    static void validatePredictionSystem(const Brain* brain);
    static void validatePredictionAccuracy(const std::vector<double>& predictions);
    static void validatePredictionParameters(const std::map<std::string, double>& params);
    static void handlePredictionError(const std::exception& e);
};

/**
 * Error recovery and resilience management
 */
class ErrorRecoveryManager {
public:
    enum class RecoveryMode {
        GracefulDegradation,
        StateRollback,
        FallbackBehavior,
        ComponentIsolation,
        EmergencyStop
    };
    
    static RecoveryMode getRecoveryMode(const BrainException& error);
    static bool attemptRecovery(RecoveryMode mode, const std::map<std::string, std::string>& context);
    static void rollbackState(const std::string& operation);
    static void enableFallbackBehavior(const std::string& component);
    static void isolateComponent(const std::string& component);
    static void emergencyStop();
};

/**
 * User-friendly error message generator
 */
class UserFriendlyErrorMessages {
public:
    static std::string formatErrorMessage(const BrainException& error);
    static std::string getSuggestedRemedy(const BrainException::ErrorCode& errorCode);
    static std::string getContextInformation(const BrainException& error);
    static std::string getTroubleshootingGuide(const BrainException& error);
    static std::string getBeginnerFriendlyMessage(const BrainException& error);
    static std::string getAdvancedUserMessage(const BrainException& error);
};

/**
 * Cross-system error correlation and monitoring
 */
class SystemMonitor {
public:
    struct SystemState {
        std::string component;
        bool healthy;
        double healthScore;
        std::vector<std::string> activeErrors;
        std::map<std::string, std::string> metrics;
        Timestamp timestamp;
    };
    
    static void updateSystemState(const std::string& component, bool healthy, 
                                 const std::vector<std::string>& errors = {});
    static SystemState getSystemState(const std::string& component);
    static std::vector<SystemState> getAllSystemStates();
    static void correlateErrors(const std::string& error1, const std::string& error2);
    static std::vector<std::string> getErrorCorrelations(const std::string& error);
    static void generateHealthReport();
};

/**
 * Error handling integration utilities
 */
class ErrorHandlingIntegration {
public:
    static void integrateErrorHandlingIntoBrain(Brain* brain);
    static void integrateErrorHandlingIntoMemorySystem(MemorySystem* memory);
    static void integrateErrorHandlingIntoPlasticitySystem(PlasticitySystem* plasticity);
    static void integrateErrorHandlingIntoNeuromodulationSystem(NeuromodulationSystem* neuromodulation);
    static void integrateErrorHandlingIntoPredictionSystem(PredictionSystem* prediction);
    static void integrateErrorHandlingIntoDevelopmentSystem(DevelopmentSystem* development);
    static void setupErrorRecoveryMechanisms(Brain* brain);
    static void setupErrorReporting(Brain* brain);
};

} // namespace nlm
