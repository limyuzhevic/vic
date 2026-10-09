// Input validation utilities for NLM Python API
// Comprehensive validation for configuration, neural networks, simulation, and memory

#pragma once

#include "NLMExceptions.hpp"
#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Synapse.hpp"
#include "../world/SimpleWorld.hpp"
#include "../memory/Memory.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <sstream>

namespace nlm {

// Configuration validation utilities
class ConfigValidator {
public:
    // Validate configuration for brain parameters
    static bool validateBrainConfig(const Config& config, std::string& errorMessage);
    
    // Validate neural network configuration
    static bool validateNeuralNetworkConfig(const Config& config, std::string& errorMessage);
    
    // Validate simulation parameters
    static bool validateSimulationParams(const Config& config, std::string& errorMessage);
    
    // Validate memory capacity parameters
    static bool validateMemoryCapacity(const Config& config, std::string& errorMessage);
    
    // Validate configuration against schema
    static bool validateConfigurationSchema(const Config& config, std::string& errorMessage);
    
    // Validate configuration values
    static bool validateConfigValue(const ConfigValue& value, const std::string& key, std::string& errorMessage);
    
    // Get suggested corrections for configuration errors
    static std::vector<std::string> getConfigErrorSuggestions(const std::string& errorMessage);
};

// Neural network validation utilities
class NeuralNetworkValidator {
public:
    // Validate neural connectivity
    static bool validateNeuralConnectivity(const Brain& brain, std::string& errorMessage);
    
    // Validate region connectivity
    static bool validateRegionConnectivity(const NeuralRegion& region, std::string& errorMessage);
    
    // Validate neuron properties
    static bool validateNeuron(const Neuron& neuron, std::string& errorMessage);
    
    // Validate synapse properties
    static bool validateSynapse(const Synapse& synapse, std::string& errorMessage);
    
    // Validate network topology
    static bool validateNetworkTopology(const Brain& brain, std::string& errorMessage);
    
    // Check for isolated neurons or regions
    static bool checkNetworkIsolation(const Brain& brain, std::string& errorMessage);
    
    // Validate neuron types
    static bool validateNeuronTypes(const Brain& brain, std::string& errorMessage);
};

// Simulation parameter validation utilities
class SimulationValidator {
public:
    // Validate simulation timestep
    static bool validateTimestep(Timestamp timestep, std::string& errorMessage);
    
    // Validate simulation duration
    static bool validateDuration(int steps, Timestamp timestep, std::string& errorMessage);
    
    // Validate world dimensions
    static bool validateWorldDimensions(size_t width, size_t height, std::string& errorMessage);
    
    // Validate sensory input dimensions
    static bool validateSensoryInput(const class SensoryInput& input, std::string& errorMessage);
    
    // Validate action parameters
    static bool validateAction(const class Action& action, std::string& errorMessage);
    
    // Validate simulation state
    static bool validateSimulationState(const Brain& brain, Timestamp currentTime, std::string& errorMessage);
    
    // Validate batch simulation parameters
    static bool validateBatchParams(size_t batchSize, size_t maxThreads, std::string& errorMessage);
    
    // Get validation error suggestions
    static std::vector<std::string> getSimulationErrorSuggestions(const std::string& errorMessage);
};

// State validation utilities
class StateValidator {
public:
    // Validate brain state before operations
    static bool validateBrainState(const Brain& brain, const std::string& operation, std::string& errorMessage);
    
    // Validate agent state
    static bool validateAgentState(const class AgentBrain& agent, Timestamp currentTime, std::string& errorMessage);
    
    // Validate world state
    static bool validateWorldState(const class SimpleWorld& world, std::string& errorMessage);
    
    // Validate memory system state
    static bool validateMemoryState(const WorkingMemory& workingMem, const EpisodicMemory& episodicMem, 
                                   const SemanticMemory& semanticMem, const ProceduralMemory& proceduralMem,
                                   const AssociativeMemory& associativeMem, std::string& errorMessage);
    
    // Validate neural connectivity state
    static bool validateConnectivityState(const Brain& brain, std::string& errorMessage);
    
    // Check for consistency between brain components
    static bool checkComponentConsistency(const Brain& brain, std::string& errorMessage);
    
    // Validate developmental stage consistency
    static bool validateDevelopmentalStage(const Brain& brain, std::string& errorMessage);
};

// Error recovery utilities
class ErrorRecovery {
public:
    // Attempt to recover from invalid brain state
    static bool recoverBrainState(Brain& brain, const std::string& errorContext, std::string& recoveryMessage);
    
    // Attempt to recover from invalid configuration
    static bool recoverConfiguration(Config& config, const std::string& errorContext, std::string& recoveryMessage);
    
    // Attempt to recover from memory issues
    static bool recoverMemoryIssues(WorkingMemory& workingMem, EpisodicMemory& episodicMem,
                                   const std::string& errorContext, std::string& recoveryMessage);
    
    // Attempt to recover from simulation issues
    static bool recoverSimulationState(Brain& brain, class SimpleWorld& world,
                                       const std::string& errorContext, std::string& recoveryMessage);
    
    // Safe reset procedures
    static bool safeResetBrain(Brain& brain, std::string& resetMessage);
    static bool safeResetWorld(class SimpleWorld& world, std::string& resetMessage);
    
    // Configuration repair utilities
    static bool repairInvalidConfig(Config& config, const std::string& errorContext, std::string& repairMessage);
    
    // Fallback behavior for critical errors
    static bool activateFallbackMode(Brain& brain, const std::string& errorContext, std::string& fallbackMessage);
};

// Enhanced Debugging and diagnostics utilities with comprehensive logging
class DebugDiagnostics {
public:
    // Original diagnostics utilities (maintained for backward compatibility)
    static std::string generateErrorReport(const std::exception& error);
    static bool detectMemoryLeak(const Brain& brain, size_t sampleSize, std::string& leakType, size_t& detectedBytes);
    static bool detectPerformanceBottleneck(const Brain& brain, const std::string& bottleneckType, 
                                           double& thresholdExceeded, double& thresholdLimit);
    static bool monitorSystemHealth(Brain& brain, class SimpleWorld& world, 
                                   std::unordered_map<std::string, double>& metrics);
    static bool getPerformanceMetrics(const Brain& brain, std::unordered_map<std::string, double>& metrics);
    static std::string generateValidationSummary(const Brain& brain, const Config& config, 
                                                const class SimpleWorld& world);
    static bool checkCommonIssues(const Brain& brain, const Config& config, 
                                 const class SimpleWorld& world, std::vector<std::string>& issues);
    
    // Enhanced logging and debugging utilities
    static std::string generateDebugReport(const Brain& brain, const Config& config, 
                                          const class SimpleWorld& world, 
                                          const std::string& reportType = "comprehensive");
    
    // Neural network debugging with connection visualization
    static std::string visualizeNeuralConnections(const Brain& brain, 
                                                 const std::string& format = "graphviz");
    static bool validateNeuralConnectivity(const Brain& brain, 
                                          std::vector<std::string>& issues);
    
    // State inspection utilities
    static std::string inspectBrainState(const Brain& brain, 
                                        const std::vector<std::string>& focusAreas = {});
    static std::string inspectAgentState(const AgentBrain& agent, 
                                        const class SimpleWorld& world);
    static std::string inspectWorldState(const SimpleWorld& world);
    
    // Memory debugging tools
    static std::string analyzeMemoryUsage(const Brain& brain);
    static bool detectMemoryLeaksDetailed(const Brain& brain, 
                                         std::vector<std::string>& leakDetails);
    static std::string getMemoryAllocationHistory(const Brain& brain);
    
    // Performance profiling utilities
    static std::string profileNeuralComputation(const Brain& brain, 
                                               uint64_t regionId = INVALID_REGION_ID);
    static std::string analyzeFiringPatterns(const Brain& brain, 
                                            const std::chrono::system_clock::time_point& start,
                                            const std::chrono::system_clock::time_point& end);
    static bool profileMemoryAccessPatterns(const Brain& brain);
    
    // Error traceback utilities
    static std::string generateDetailedErrorTrace(const std::exception& error);
    static std::string traceErrorPropagation(const Brain& brain, 
                                            const std::exception& error);
    
    // Real-time system monitoring
    static std::string getRealTimeMetrics(const Brain& brain, const SimpleWorld& world,
                                         const std::vector<std::string>& metricTypes = {});
    static bool monitorResourceUsage(const Brain& brain, 
                                   std::unordered_map<std::string, double>& resourceUsage);
    static std::string monitorSimulationProgress(const Brain& brain, 
                                                 const SimpleWorld& world,
                                                 uint64_t currentStep);
    
    // Health checks and alerts
    static bool performHealthCheck(const Brain& brain, const Config& config,
                                  const SimpleWorld& world,
                                  std::vector<std::string>& healthIssues);
    static std::string generateHealthReport(const Brain& brain, const Config& config,
                                           const SimpleWorld& world);
    
    // Analysis tools
    static std::string analyzeNeuralActivityPatterns(const Brain& brain);
    static std::string analyzeMemoryUsagePatterns(const Brain& brain);
    static std::string analyzeLearningTrajectories(const Brain& brain,
                                                   const std::vector<std::string>& learningMetrics);
    static std::vector<std::string> getOptimizationRecommendations(const Brain& brain,
                                                                 const Config& config,
                                                                 const SimpleWorld& world);
    
    // Log-based utilities (integration with logging system)
    static bool enableLogging(const std::string& logLevel = "debug",
                             const std::string& logFormat = "json",
                             const std::string& logOutput = "file");
    static std::vector<std::string> analyzeLogFiles(const std::string& logDirectory,
                                                   const std::string& analysisType = "trends");
    static std::string correlateSystemLogs(const Brain& brain, const SimpleWorld& world,
                                          const std::vector<std::string>& logSources);
    
    // Debugging utilities
    static bool activateDebugMode(const std::string& debugLevel = "full");
    static void deactivateDebugMode();
    static bool isDebugModeActive();
    
    // Simulation debugging
    static std::string debugSimulationStep(const Brain& brain,
                                           const SimpleWorld& world,
                                           uint64_t step,
                                           const std::vector<std::string>& debugDetails = {});
    
    // Neural event debugging
    static std::string traceNeuralEvent(const Brain& brain,
                                       uint64_t neuronId,
                                       const std::chrono::system_clock::time_point& timestamp);
    
    // Configuration debugging
    static std::string debugConfiguration(const Config& config,
                                          const std::vector<std::string>& configKeys = {});
    
    // Performance bottleneck analysis
    static std::string analyzePerformanceBottlenecks(const Brain& brain,
                                                     const std::vector<std::string>& bottleneckTypes);
    
    // Memory leak detection (enhanced)
    static bool detectMemoryLeaksComprehensive(const Brain& brain,
                                              std::unordered_map<std::string, std::vector<std::string>>& leakAnalysis);
    
    // Error recovery debugging
    static std::string debugErrorRecovery(const Brain& brain,
                                          const std::string& errorContext);
    
    // Development utilities
    static std::string generateDevelopmentReport(const Brain& brain,
                                                 const Config& config,
                                                 const SimpleWorld& world);
    
    // Visualization utilities
    static std::string generateVisualizationData(const Brain& brain,
                                                 const std::string& visualizationType = "network");
    static bool exportDebugData(const Brain& brain, const Config& config,
                               const SimpleWorld& world,
                               const std::string& outputPath);
};

// Validation result structure
struct ValidationResult {
    bool isValid;
    std::string message;
    std::vector<std::string> suggestions;
    std::shared_ptr<NLMError> error;
    
    ValidationResult() : isValid(true) {}
    ValidationResult(bool valid, const std::string& msg) : isValid(valid), message(msg) {}
    
    bool hasError() const { return error != nullptr; }
    
    std::string getFullMessage() const {
        std::stringstream ss;
        ss << message;
        if (error) {
            ss << "\nError type: " << error->getErrorType();
            ss << "\nError details: " << error->getDetails();
        }
        if (!suggestions.empty()) {
            ss << "\nSuggestions:";
            for (const auto& suggestion : suggestions) {
                ss << "\n  - " << suggestion;
            }
        }
        return ss.str();
    }
};

// Convenience validation functions
inline ValidationResult validateBrain(const Brain& brain, const std::string& operation = "") {
    ValidationResult result;
    std::string errorMessage;
    if (!StateValidator::validateBrainState(brain, operation, errorMessage)) {
        result.isValid = false;
        result.message = "Brain state validation failed: " + errorMessage;
    }
    return result;
}

inline ValidationResult validateConfig(const Config& config) {
    ValidationResult result;
    std::string errorMessage;
    if (!ConfigValidator::validateBrainConfig(config, errorMessage)) {
        result.isValid = false;
        result.message = "Configuration validation failed: " + errorMessage;
        result.suggestions = ConfigValidator::getConfigErrorSuggestions(errorMessage);
    }
    return result;
}

inline ValidationResult validateWorld(const class SimpleWorld& world) {
    ValidationResult result;
    std::string errorMessage;
    if (!StateValidator::validateWorldState(world, errorMessage)) {
        result.isValid = false;
        result.message = "World state validation failed: " + errorMessage;
    }
    return result;
}

inline ValidationResult validateSimulation(const Brain& brain, const class SimpleWorld& world, Timestamp currentTime) {
    ValidationResult result;
    std::string errorMessage;
    if (!SimulationValidator::validateSimulationState(brain, currentTime, errorMessage)) {
        result.isValid = false;
        result.message = "Simulation state validation failed: " + errorMessage;
        result.suggestions = SimulationValidator::getSimulationErrorSuggestions(errorMessage);
    }
    return result;
}

} // namespace nlm