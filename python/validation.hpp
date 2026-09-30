// NLM Python Bindings - Comprehensive Validation and Error Handling
// This file provides 36 validation and error handling functions for enhanced safety and robustness

#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <memory>
#include <optional>
#include <variant>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <mutex>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace nlm {

// Forward declarations for types used in validation
struct NeuronId;
struct SynapseId;
struct RegionId;
struct PopulationId;
using SimulationStep = uint64_t;
using Timestamp = double;
using TimestepDuration = double;
using SynapticWeight = float;
using MembranePotential = float;
using FiringRate = float;
using Delay = uint32_t;

enum class NeuronType;
enum class SynapseType;
enum class DevelopmentalStage;
enum class FiringState;
enum class ActionType;
enum class MotorCommand;
enum class WorldObjectType;

class Config;
class Brain;
class SimpleWorld;
class AgentBrain;
class SensoryInput;
class Action;
class WorldObject;
class AgentBody;

// Validation result structure
struct ValidationResult {
    bool valid;
    std::string message;
    std::vector<std::string> warnings;
    std::vector<std::string> errors;
    
    ValidationResult() : valid(true), message("Validation passed") {}
    ValidationResult(bool v, const std::string& msg, const std::vector<std::string>& warns = {}, const std::vector<std::string>& errs = {})
        : valid(v), message(msg), warnings(warns), errors(errs) {}
    
    bool isValid() const { return valid; }
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
    std::string getFullMessage() const {
        std::stringstream ss;
        ss << message;
        if (!warnings.empty()) {
            ss << " Warnings: ";
            for (const auto& w : warnings) ss << w << "; ";
        }
        if (!errors.empty()) {
            ss << " Errors: ";
            for (const auto& e : errors) ss << e << "; ";
        }
        return ss.str();
    }
};

// Validation level enum
enum class ValidationLevel {
    Basic,
    Thorough,
    Strict
};

// Validation exception classes
class ValidationException : public std::runtime_error {
public:
    ValidationException(const std::string& message) : std::runtime_error(message) {}
};

class ValidationWarning : public std::runtime_error {
public:
    ValidationWarning(const std::string& message) : std::runtime_error(message) {}
};

// Global validation configuration
struct ValidationConfig {
    ValidationLevel defaultLevel = ValidationLevel::Basic;
    bool enableLogging = false;
    bool throwOnError = true;
    bool throwOnWarning = false;
    std::string logFile = "validation.log";
    std::mutex loggingMutex;
    
    ValidationConfig() = default;
    ValidationConfig(ValidationLevel level, bool log, bool throwErrors, bool throwWarnings)
        : defaultLevel(level), enableLogging(log), throwOnError(throwErrors), throwOnWarning(throwWarnings) {}
};

namespace Validation {
    // Global validation configuration
    extern ValidationConfig g_config;
    
    // Utility functions
    std::string formatError(const std::string& context, const std::string& error);
    std::string formatWarning(const std::string& context, const std::string& warning);
    void logValidationMessage(const std::string& message, bool isError = false);
    
    // Validation helper functions
    template<typename T>
    bool isInRange(const T& value, const T& min, const T& max, const std::string& context = "") {
        if (value < min) {
            logValidationMessage(formatError(context, "Value " + std::to_string(value) + " is less than minimum " + std::to_string(min)), true);
            return false;
        }
        if (value > max) {
            logValidationMessage(formatError(context, "Value " + std::to_string(value) + " is greater than maximum " + std::to_string(max)), true);
            return false;
        }
        return true;
    }
    
    bool isValidString(const std::string& str, size_t minLength = 1, size_t maxLength = 1000, const std::string& context = "");
    bool isValidVector(const std::vector<float>& vec, size_t expectedSize = 0, const std::string& context = "");
    bool isValidMap(const std::map<std::string, std::string>& map, const std::string& context = "");
    
    // Parameter validation
    ValidationResult validateParameters(const std::vector<float>& parameters, const std::string& context = "");
    ValidationResult validateSimulationStep(const SimulationStep& step, const SimulationStep& maxStep = 1000000, const std::string& context = "");
    ValidationResult validateTimestamp(const Timestamp& timestamp, const Timestamp& minTime = 0.0, const Timestamp& maxTime = 1e9, const std::string& context = "");
    
    // Configuration validation
    ValidationResult validateBrainConfig(const Config& config, ValidationLevel level = ValidationLevel::Basic);
    ValidationResult validateWorldConfig(const Config& config, ValidationLevel level = ValidationLevel::Basic);
    ValidationResult validateConfigCompleteness(const Config& config);
    ValidationResult validateConfigConsistency(const Config& config);
    ValidationResult validateConfigValues(const Config& config, ValidationLevel level = ValidationLevel::Basic);
    
    // ID validation
    ValidationResult validateNeuronIds(const std::vector<NeuronId>& ids, const std::string& context = "");
    ValidationResult validateSynapseIds(const std::vector<SynapseId>& ids, const std::string& context = "");
    ValidationResult validateRegionIds(const std::vector<RegionId>& ids, const std::string& context = "");
    
    // Data validation
    ValidationResult validateSensoryData(const SensoryInput& data, const std::string& context = "");
    ValidationResult validateMotorCommand(const Action& command, const std::string& context = "");
    ValidationResult validateAction(const Action& action, const std::string& context = "");
    ValidationResult validateObjectData(const WorldObject& obj, const std::string& context = "");
    ValidationResult validateMemoryData(const std::string& data, const std::string& context = "");
    
    // State validation
    ValidationResult validateBrainState(const Brain& brain, ValidationLevel level = ValidationLevel::Basic);
    ValidationResult validateAgentState(const AgentBrain& agent, ValidationLevel level = ValidationLevel::Basic);
    ValidationResult validateWorldState(const SimpleWorld& world, ValidationLevel level = ValidationLevel::Basic);
    ValidationResult validateDevelopmentState(const DevelopmentalStage& stage, const std::string& context = "");
    
    // Performance validation
    ValidationResult validatePerformanceParameters(const Config& config, ValidationLevel level = ValidationLevel::Basic);
    ValidationResult validateSimulationParameters(const Config& config, ValidationLevel level = ValidationLevel::Basic);
    ValidationResult validatePlasticityParameters(const Config& config, ValidationLevel level = ValidationLevel::Basic);
    ValidationResult validateNeuromodulationParameters(const Config& config, ValidationLevel level = ValidationLevel::Basic);
    
    // Safety and security validation
    ValidationResult checkResourceUsage(const std::string& context = "");
    ValidationResult validateMemoryBounds(size_t memoryUsage, size_t maxMemory = 1024 * 1024 * 1024, const std::string& context = "");
    ValidationResult checkSafetyConstraints(const Config& config, const std::string& context = "");
    ValidationResult validateNeuralConnectivity(const Brain& brain, ValidationLevel level = ValidationLevel::Basic);
    ValidationResult validatePlasticityConstraints(const Config& config, const std::string& context = "");
    
    // Error recovery functions
    bool safeStep(Brain& brain, SimulationStep step, Timestamp time = 0.0, ValidationLevel level = ValidationLevel::Basic);
    bool safeBrainOperation(Brain& brain, std::function<void()> operation, ValidationLevel level = ValidationLevel::Basic);
    
    ValidationResult validateAndSafeStep(Brain& brain, SimulationStep step, Timestamp time = 0.0, ValidationLevel level = ValidationLevel::Basic);
    ValidationResult validateAndSafeBrainOperation(Brain& brain, std::function<void()> operation, ValidationLevel level = ValidationLevel::Basic);
    
    template<typename T, typename Func>
    T tryBrainOperation(Func operation, T defaultValue = T(), ValidationLevel level = ValidationLevel::Basic) {
        try {
            ValidationResult validation = validateBrainState(*static_cast<Brain*>(nullptr), level);
            if (!validation.isValid()) {
                throw ValidationException("Brain state validation failed: " + validation.getFullMessage());
            }
            return operation();
        } catch (const ValidationException& e) {
            Validation::logValidationMessage(std::string("ValidationException in brain operation: ") + e.what(), true);
            if (g_config.throwOnError) throw;
            return defaultValue;
        } catch (const std::exception& e) {
            Validation::logValidationMessage(std::string("Exception in brain operation: ") + e.what(), true);
            if (g_config.throwOnError) throw;
            return defaultValue;
        }
    }
    
    bool validateAndSafeAgentOperation(AgentBrain& agent, std::function<void()> operation, ValidationLevel level = ValidationLevel::Basic);
    
    // Configuration issue fixing
    bool fixConfigIssues(Config& config, const std::string& context = "");
    
    // World completeness validation
    ValidationResult validateWorldCompleteness(const SimpleWorld& world, ValidationLevel level = ValidationLevel::Basic);
}

// Batch Processing Functions
ValidationResult batch_step_simulation(Brain& brain, const std::vector<SimulationStep>& steps, const std::vector<Timestamp>& times = {}, ValidationLevel level = ValidationLevel::Basic);
std::vector<bool> batch_process_multiple_brains(const std::vector<std::shared_ptr<Brain>>& brains, const std::vector<std::shared_ptr<SimpleWorld>>& worlds, const std::vector<std::shared_ptr<Config>>& configs);
batch_process_multiple_brains_result batch_train_multiple_agents(const std::vector<std::shared_ptr<AgentBrain>>& agents, const std::vector<std::shared_ptr<SimpleWorld>>& worlds, const std::vector<size_t>& episodesPerAgent);
batch_episode_result batch_run_episodes(const std::vector<std::shared_ptr<Brain>>& brains, const std::vector<size_t>& episodesPerBrain, const std::vector<std::shared_ptr<SimpleWorld>>& worlds = {});
batch_save_result batch_save_states(const std::vector<std::shared_ptr<Brain>>& brains, const std::vector<std::string>& filepaths);
batch_load_result batch_load_states(const std::vector<std::shared_ptr<Brain>>& brains, const std::vector<std::string>& filepaths);
batch_validation_result batch_validate_configurations(const std::vector<std::shared_ptr<Config>>& configs, ValidationLevel level = ValidationLevel::Basic);
batch_comparison_result batch_compare_results(const std::vector<batch_process_multiple_brains_result>& results, const std::string& comparisonMetric = "total_spikes");

// Utility Functions
std::vector<SimulationStep> parallel_step_simulation(Brain& brain, const std::vector<SimulationStep>& steps, size_t maxThreads = 0, ValidationLevel level = ValidationLevel::Basic);
parallel_batch_result parallel_batch_training(const std::vector<std::shared_ptr<AgentBrain>>& agents, const std::vector<size_t>& episodesPerAgent, size_t maxThreads = 0);
neural_statistics compute_neural_statistics(const Brain& brain, const std::string& includeMetrics = "all");
spike_pattern_analysis analyze_spike_patterns(const Brain& brain, const std::string& analysisType = "comprehensive");
float compute_connection_density(const Brain& brain, double threshold = 0.1, const std::string& metric = "average");
neural_report generate_neural_report(const Brain& brain, const std::string& reportType = "comprehensive", const std::map<std::string, std::string>& options = {});
optimized_config optimize_brain_configuration(const Config& config, const std::map<std::string, double>& performanceWeights = {});
config_benchmark benchmark_configuration(const Config& config, const std::vector<std::shared_ptr<Config>>& referenceConfigs = {});

// Data Processing Functions
sensory_data_batch_result process_sensory_data_batch(const std::vector<std::shared_ptr<SensoryInput>>& inputs, const std::string& processingType = "normalized");
motor_data_batch_result process_motor_commands_batch(const std::vector<std::shared_ptr<Action>>& actions, const std::string& processingType = "smoothed");
aggregated_simulation_result aggregate_simulation_results(const std::vector<std::shared_ptr<Brain>>& brains, const std::vector<std::shared_ptr<SimpleWorld>>& worlds, const std::string& aggregationMethod = "weighted_average");
interpolated_simulation_result interpolate_simulation_data(const std::vector<std::shared_ptr<Brain>>& brains, double targetTime, const std::string& interpolationMethod = "linear");
smooth_simulation_result smooth_simulation_data(const std::vector<std::shared_ptr<Brain>>& brains, double smoothingFactor = 0.5, const std::string& method = "moving_average");
anomaly_detection_result detect_anomalies(const std::vector<std::shared_ptr<Brain>>& brains, double threshold = 2.0, const std::string& detectionMethod = "zscore");
normalized_data_result normalize_simulation_data(const std::vector<std::shared_ptr<Brain>>& brains, const std::string& normalizationMethod = "minmax");
standardized_data_result standardize_simulation_data(const std::vector<std::shared_ptr<Brain>>& brains, double mean = 0.0, double stddev = 1.0);

// Resource Management Functions
resource_allocation_result manage_memory_usage(const std::vector<std::shared_ptr<Brain>>& brains, size_t maxMemoryMB = 0, const std::string& strategy = "adaptive");
resource_allocation_result manage_cpu_usage(const std::vector<std::shared_ptr<Brain>>& brains, double maxCPUTime = 0.0, const std::string& strategy = "dynamic");
cleanup_result cleanup_simulation_resources(const std::vector<std::shared_ptr<Brain>>& brains, const std::string& cleanupLevel = "comprehensive");
resource_monitor_result monitor_simulation_resources(const std::vector<std::shared_ptr<Brain>>& brains, double monitorInterval = 1.0);
resource_schedule_result schedule_simulation_resources(const std::vector<std::shared_ptr<Brain>>& brains, const std::string& schedulingStrategy = "fifo");
resource_allocation_result allocate_simulation_resources(const std::vector<std::shared_ptr<Brain>>& brains, const std::map<std::string, size_t>& resourceRequirements);
deallocation_result deallocate_simulation_resources(const std::vector<std::shared_ptr<Brain>>& brains);
resource_validation_result validate_simulation_resources(const std::vector<std::shared_ptr<Brain>>& brains, const std::string& validationMode = "strict");

// Performance Optimization Functions
performance_result optimize_brain_performance(Brain& brain, const std::map<std::string, double>& optimizationWeights = {});
performance_result optimize_memory_performance(std::vector<std::shared_ptr<Brain>>& brains, const std::string& optimizationStrategy = "aggressive");
performance_result optimize_cpu_performance(std::vector<std::shared_ptr<Brain>>& brains, const std::string& optimizationStrategy = "dynamic");
performance_result optimize_network_performance(Brain& brain, const std::string& optimizationMode = "balanced");
performance_result optimize_disk_performance(const std::string& storagePath = ".", const std::string& optimizationStrategy = "sequential");
performance_result optimize_storage_performance(const std::vector<std::shared_ptr<Brain>>& brains, const std::string& optimizationStrategy = "compression");
performance_result optimize_bandwidth_performance(Brain& brain, double targetBandwidth = 0.0, const std::string& optimizationMethod = "adaptive");
performance_result optimize_latency_performance(Brain& brain, double targetLatency = 0.0, const std::string& optimizationMethod = "pipeline");

// Python binding functions for new batch and utility operations
void bindBatchOperations(py::module_& m);
void bindUtilityFunctions(py::module_& m);
void bindDataProcessingFunctions(py::module_& m);
void bindResourceManagementFunctions(py::module_& m);
void bindPerformanceOptimizationFunctions(py::module_& m);
} // namespace nlm
