#pragma once

#include "Types/Types.hpp"
#include "Config/Config.hpp"
#include "brain/Brain.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <stdexcept>
#include <string>
#include <vector>

namespace nlm {

// ===================================================================
// VALIDATION AND ERROR HANDLING FUNCTIONS FOR PYTHON BINDINGS
// ===================================================================

// ===================================================================
// INPUT VALIDATION HELPERS
// ===================================================================

/**
 * @brief Validate a neuron ID for Python bindings
 * 
 * @param neuron_id The neuron ID to validate
 * @return bool True if the neuron ID is valid, False otherwise
 */
bool validateNeuronId(const NeuronId& neuron_id);

/**
 * @brief Validate a synapse ID for Python bindings
 * 
 * @param synapse_id The synapse ID to validate
 * @return bool True if the synapse ID is valid, False otherwise
 */
bool validateSynapseId(const SynapseId& synapse_id);

/**
 * @brief Validate a region ID for Python bindings
 * 
 * @param region_id The region ID to validate
 * @return bool True if the region ID is valid, False otherwise
 */
bool validateRegionId(const RegionId& region_id);

/**
 * @brief Validate a population ID for Python bindings
 * 
 * @param population_id The population ID to validate
 * @return bool True if the population ID is valid, False otherwise
 */
bool validatePopulationId(const PopulationId& population_id);

/**
 * @brief Validate neuron existence in a brain for Python bindings
 * 
 * @param brain The brain to check
 * @param neuron_id The neuron ID to validate
 * @return bool True if the neuron exists in the brain, False otherwise
 */
bool validateNeuronInBrain(const Brain* brain, const NeuronId& neuron_id);

/**
 * @brief Validate synapse existence in a brain for Python bindings
 * 
 * @param brain The brain to check
 * @param synapse_id The synapse ID to validate
 * @return bool True if the synapse exists in the brain, False otherwise
 */
bool validateSynapseInBrain(const Brain* brain, const SynapseId& synapse_id);

// ===================================================================
// CONFIGURATION VALIDATION
// ===================================================================

/**
 * @brief Validate configuration parameters for Python bindings
 * 
 * @param config The configuration to validate
 * @param warnings Optional dictionary to store validation warnings
 * @return bool True if configuration is valid, False otherwise
 */
bool validateConfig(const Config* config, py::dict warnings = py::dict());

/**
 * @brief Find conflicting configuration keys for Python bindings
 * 
 * @param config The configuration to check
 * @return std::vector<std::string> List of conflicting key names
 */
std::vector<std::string> findConflictingConfigs(const Config* config);

/**
 * @brief Validate simulation parameters for Python bindings
 * 
 * @param brain The brain to simulate
 * @param step The simulation step
 * @param time The simulation time
 * @return bool True if parameters are valid, False otherwise
 */
bool validateSimulationParameters(const Brain* brain, SimulationStep step, Timestamp time = 0.0);

// ===================================================================
// SAFE OPERATION WRAPPERS
// ===================================================================

/**
 * @brief Safely execute a brain step with error handling and validation
 * 
 * @param brain The brain to step
 * @param step The simulation step
 * @param time The simulation time
 * @return std::pair<bool, std::string> True if successful, error message if failed
 */
std::pair<bool, std::string> safeBrainStep(Brain* brain, SimulationStep step, Timestamp time = 0.0);

/**
 * @brief Safely reset brain state with error handling
 * 
 * @param brain The brain to reset
 * @return std::pair<bool, std::string> True if successful, error message if failed
 */
std::pair<bool, std::string> safeBrainReset(Brain* brain);

/**
 * @brief Safely save brain state with error handling
 * 
 * @param brain The brain to save
 * @param filepath The file path to save to
 * @return std::pair<bool, std::string> True if successful, error message if failed
 */
std::pair<bool, std::string> safeBrainSave(Brain* brain, const std::string& filepath);

/**
 * @brief Safely load brain state with error handling
 * 
 * @param brain The brain to load into
 * @param filepath The file path to load from
 * @return std::pair<bool, std::string> True if successful, error message if failed
 */
std::pair<bool, std::string> safeBrainLoad(Brain* brain, const std::string& filepath);

/**
 * @brief Safely inject sensory input with validation
 * 
 * @param brain The brain to inject into
 * @param sensory_input The sensory input to inject
 * @return std::pair<bool, std::string> True if successful, error message if failed
 */
std::pair<bool, std::string> safeSensoryInjection(Brain* brain, const SensoryInput& sensory_input);

// ===================================================================
// ERROR INFORMATION AND DIAGNOSTICS
// ===================================================================

/**
 * @brief Get detailed error information and diagnostic data for brain state
 * 
 * @param brain The brain to analyze
 * @return py::dict Dictionary containing diagnostic information and potential issues
 */
py::dict getBrainErrorInfo(const Brain* brain);

/**
 * @brief Get comprehensive brain state analysis
 * 
 * @param brain The brain to analyze
 * @return py::dict Dictionary containing analysis of brain state, health, and performance
 */
py::dict analyzeBrainState(const Brain* brain);

/**
 * @brief Compare two brain states and return differences
 * 
 * @param brain1 First brain for comparison
 * @param brain2 Second brain for comparison
 * @return py::dict Dictionary containing differences between the two brains
 */
py::dict compareBrains(const Brain* brain1, const Brain* brain2);

// ===================================================================
// CONFIGURATION UTILITIES
// ===================================================================

/**
 * @brief Merge two configurations with override precedence
 * 
 * @param base_config The base configuration
 * @param override_config The configuration to override base settings
 * @return std::shared_ptr<Config> Merged configuration
 */
std::shared_ptr<Config> mergeConfigs(const Config* base_config, const Config* override_config);

/**
 * @brief Extract subset of configuration keys
 * 
 * @param config The configuration to extract from
 * @param keys List of configuration keys to extract
 * @return std::shared_ptr<Config> Configuration subset
 */
std::shared_ptr<Config> extractConfigSubset(const Config* config, const std::vector<std::string>& keys);

/**
 * @brief Create a default validation report
 * 
 * @param errors List of error messages
 * @param warnings List of warning messages
 * @return py::dict Dictionary containing validation report
 */
py::dict createValidationReport(const std::vector<std::string>& errors, 
                               const std::vector<std::string>& warnings);

// ===================================================================
// SIMULATION CONTROL UTILITIES
// ===================================================================

/**
 * @brief Run a simulation with periodic state validation and error recovery
 * 
 * @param brain The brain to simulate
 * @param num_steps Number of simulation steps to run
 * @param check_interval Interval between state validations (steps)
 * @return py::dict Dictionary containing simulation results and any errors
 */
py::dict runSafeSimulation(Brain* brain, SimulationStep num_steps, SimulationStep check_interval = 100);

/**
 * @brief Validate brain state consistency
 * 
 * @param brain The brain to validate
 * @return bool True if brain state is consistent, False otherwise
 */
bool validateBrainState(const Brain* brain);

/**
 * @brief Get available configuration keys for validation
 * 
 * @return std::vector<std::string> List of available configuration keys
 */
std::vector<std::string> getAvailableConfigKeys();

/**
 * @brief Create a safe brain from configuration with validation
 * 
 * @param config The configuration to use
 * @return std::shared_ptr<Brain> Created brain, or nullptr if invalid
 */
std::shared_ptr<Brain> createSafeBrain(const std::shared_ptr<Config>& config);

/**
 * @brief Validate brain initialization parameters
 * 
 * @param config The configuration to validate
 * @return std::vector<std::string> List of validation errors (empty if valid)
 */
std::vector<std::string> validateBrainConfig(const Config* config);

/**
 * @brief Get detailed error information for configuration validation
 * 
 * @param config The configuration to validate
 * @return py::dict Dictionary containing detailed validation errors and suggestions
 */
py::dict getConfigValidationDetails(const Config* config);

} // namespace nlm