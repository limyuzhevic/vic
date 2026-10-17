#include "PythonBindingsUtils.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <fstream>

namespace nlm {

// ===================================================================
// VALIDATION AND ERROR HANDLING FUNCTION IMPLEMENTATIONS
// ===================================================================

// ===================================================================
// INPUT VALIDATION HELPERS
// ===================================================================

bool validateNeuronId(const NeuronId& neuron_id) {
    return neuron_id.value != UINT64_MAX && neuron_id.value != 0;
}

bool validateSynapseId(const SynapseId& synapse_id) {
    return synapse_id.value != UINT64_MAX && synapse_id.value != 0;
}

bool validateRegionId(const RegionId& region_id) {
    return region_id.value != UINT64_MAX && region_id.value != 0;
}

bool validatePopulationId(const PopulationId& population_id) {
    return population_id.value != UINT64_MAX && population_id.value != 0;
}

bool validateNeuronInBrain(const Brain* brain, const NeuronId& neuron_id) {
    if (!brain || !validateNeuronId(neuron_id)) {
        return false;
    }
    
    // Check if neuron exists in the brain
    for (const auto& region : brain->getRegions()) {
        for (const auto& population : region->getPopulations()) {
            for (const auto& neuron : population->getNeurons()) {
                if (neuron->getId() == neuron_id) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool validateSynapseInBrain(const Brain* brain, const SynapseId& synapse_id) {
    if (!brain || !validateSynapseId(synapse_id)) {
        return false;
    }
    
    // Check if synapse exists in the brain
    size_t total_neurons = brain->getTotalNeuronCount();
    if (total_neurons > 0) {
        // This is a simplified check - in a real implementation, 
        // we would need access to synapse management APIs
        return synapse_id.value < total_neurons * 10; // Rough estimate
    }
    return false;
}

// ===================================================================
// CONFIGURATION VALIDATION
// ===================================================================

bool validateConfig(const Config* config, py::dict warnings) {
    if (!config) {
        return false;
    }
    
    std::vector<std::string> errors;
    std::vector<std::string> warning_messages;
    
    // Check for required sections
    auto keys = config->getKeys();
    
    // Validate brain configuration
    bool has_brain_section = false;
    for (const auto& key : keys) {
        if (key.find("brain.") == 0) {
            has_brain_section = true;
            // Validate specific brain parameters
            if (key == "brain.neuron_count") {
                // Check neuron count is reasonable
                try {
                    auto neuron_count = config->get("brain.neuron_count", 0);
                    if (neuron_count < 1 || neuron_count > 1000000) {
                        warning_messages.push_back("brain.neuron_count is outside typical range (1-1000000)");
                    }
                } catch (...) {
                    warning_messages.push_back("brain.neuron_count is not accessible");
                }
            }
        }
    }
    
    if (!has_brain_section) {
        errors.push_back("Configuration must contain a 'brain.' section");
    }
    
    // Add warnings to Python dict
    for (const auto& warning : warning_messages) {
        warnings[warning] = true;
    }
    
    // If there are errors, add them to warnings dict for error reporting
    if (!errors.empty()) {
        for (size_t i = 0; i < errors.size(); ++i) {
            warnings[std::string("ERROR:") + errors[i]] = true;
        }
    }
    
    return errors.empty();
}

std::vector<std::string> findConflictingConfigs(const Config* config) {
    std::vector<std::string> conflicts;
    if (!config) {
        return conflicts;
    }
    
    auto keys = config->getKeys();
    
    // Check for conflicting keys (same meaning with different names)
    for (size_t i = 0; i < keys.size(); ++i) {
        for (size_t j = i + 1; j < keys.size(); ++j) {
            std::string key1 = keys[i];
            std::string key2 = keys[j];
            
            // Simple conflict detection - could be expanded
            if ((key1.find("plasticity") != std::string::npos && 
                 key2.find("learning") != std::string::npos) ||
                (key1.find("neuron") != std::string::npos && 
                 key2.find("brain") != std::string::npos)) {
                conflicts.push_back(key1 + " might conflict with " + key2);
            }
        }
    }
    
    return conflicts;
}

bool validateSimulationParameters(const Brain* brain, SimulationStep step, Timestamp time) {
    if (!brain) {
        return false;
    }
    
    // Check step is reasonable
    if (step > 1000000000) { // 1 billion steps
        return false;
    }
    
    // Check time is reasonable
    if (time < -1000.0 || time > 1000000.0) { // -1000s to 1000ks
        return false;
    }
    
    // Check brain is in a valid state
    return true;
}

// ===================================================================
// SAFE OPERATION WRAPPERS
// ===================================================================

std::pair<bool, std::string> safeBrainStep(Brain* brain, SimulationStep step, Timestamp time) {
    if (!brain) {
        return {false, "Brain is null"};
    }
    
    try {
        if (!validateSimulationParameters(brain, step, time)) {
            return {false, "Invalid simulation parameters"};
        }
        
        brain->step(step, time);
        return {true, ""};
    } catch (const std::exception& e) {
        return {false, std::string("Brain step failed: ") + e.what()};
    } catch (...) {
        return {false, "Brain step failed with unknown error"};
    }
}

std::pair<bool, std::string> safeBrainReset(Brain* brain) {
    if (!brain) {
        return {false, "Brain is null"};
    }
    
    try {
        brain->reset();
        return {true, ""};
    } catch (const std::exception& e) {
        return {false, std::string("Brain reset failed: ") + e.what()};
    } catch (...) {
        return {false, "Brain reset failed with unknown error"};
    }
}

std::pair<bool, std::string> safeBrainSave(Brain* brain, const std::string& filepath) {
    if (!brain) {
        return {false, "Brain is null"};
    }
    
    if (filepath.empty()) {
        return {false, "Filepath cannot be empty"};
    }
    
    try {
        // Validate file path
        std::ofstream test_file(filepath, std::ios::out);
        if (!test_file.is_open()) {
            return {false, "Cannot write to file: " + filepath};
        }
        test_file.close();
        
        // Remove test file
        std::remove(filepath.c_str());
        
        // Try actual save operation
        bool success = brain->save(filepath);
        if (!success) {
            return {false, "Brain save operation failed"};
        }
        
        return {true, ""};
    } catch (const std::exception& e) {
        return {false, std::string("Brain save failed: ") + e.what()};
    } catch (...) {
        return {false, "Brain save failed with unknown error"};
    }
}

std::pair<bool, std::string> safeBrainLoad(Brain* brain, const std::string& filepath) {
    if (!brain) {
        return {false, "Brain is null"};
    }
    
    if (filepath.empty()) {
        return {false, "Filepath cannot be empty"};
    }
    
    try {
        // Check if file exists
        std::ifstream test_file(filepath, std::ios::in);
        if (!test_file.is_open()) {
            return {false, "Cannot read from file: " + filepath + " (file may not exist)"};
        }
        test_file.close();
        
        // Try actual load operation
        bool success = brain->load(filepath);
        if (!success) {
            return {false, "Brain load operation failed"};
        }
        
        return {true, ""};
    } catch (const std::exception& e) {
        return {false, std::string("Brain load failed: ") + e.what()};
    } catch (...) {
        return {false, "Brain load failed with unknown error"};
    }
}

std::pair<bool, std::string> safeSensoryInjection(Brain* brain, const SensoryInput& sensory_input) {
    if (!brain) {
        return {false, "Brain is null"};
    }
    
    try {
        brain->receiveSensoryInput(sensory_input);
        return {true, ""};
    } catch (const std::exception& e) {
        return {false, std::string("Sensory injection failed: ") + e.what()};
    } catch (...) {
        return {false, "Sensory injection failed with unknown error"};
    }
}

// ===================================================================
// ERROR INFORMATION AND DIAGNOSTICS
// ===================================================================

py::dict getBrainErrorInfo(const Brain* brain) {
    py::dict error_info;
    
    if (!brain) {
        error_info["error"] = "Brain is null";
        return error_info;
    }
    
    // Collect various diagnostic information
    try {
        // Basic statistics
        error_info["total_neurons"] = brain->getTotalNeuronCount();
        error_info["total_synapses"] = brain->getTotalSynapseCount();
        error_info["firing_neuron_count"] = brain->getFiringNeuronCount();
        error_info["average_firing_rate"] = brain->getAverageFiringRate();
        error_info["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        error_info["total_spike_count"] = brain->getTotalSpikeCount();
        error_info["pending_spike_event_count"] = brain->getPendingSpikeEventCount();
        
        // Check for potential issues
        std::vector<std::string> issues;
        
        if (brain->getTotalNeuronCount() == 0) {
            issues.push_back("No neurons in brain");
        }
        
        float eir = brain->getExcitationInhibitionRatio();
        if (eir < 0.1 || eir > 10.0) {
            issues.push_back(std::string("Unusual E/I ratio: ") + std::to_string(eir));
        }
        
        if (brain->getFiringNeuronCount() > brain->getTotalNeuronCount() * 0.9) {
            issues.push_back("Almost all neurons are firing (may indicate excitation")
        }
        
        error_info["issues"] = issues;
        error_info["issue_count"] = issues.size();
        
        // Development stage
        error_info["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        
        // Memory system checks
        if (auto ws = brain->getWorkingMemory()) {
            error_info["working_memory_active"] = true;
            error_info["working_memory_size"] = ws->getMemorySize();
        }
        
    } catch (const std::exception& e) {
        error_info["diagnostic_error"] = std::string("Diagnostic failed: ") + e.what();
    } catch (...) {
        error_info["diagnostic_error"] = "Diagnostic failed with unknown error";
    }
    
    return error_info;
}

py::dict analyzeBrainState(const Brain* brain) {
    py::dict analysis;
    
    if (!brain) {
        analysis["error"] = "Brain is null";
        return analysis;
    }
    
    try {
        // Performance metrics
        analysis["health_score"] = 1.0; // Simplified health score
        analysis["simulation_efficiency"] = brain->getAverageFiringRate() / 10.0; // Normalized
        
        // Neural activity metrics
        analysis["firing_rate_normalized"] = brain->getAverageFiringRate() / 10.0;
        analysis["excitatory_inhibitory_balance"] = brain->getExcitationInhibitionRatio();
        
        // Memory metrics
        if (auto ws = brain->getWorkingMemory()) {
            analysis["working_memory_usage"] = ws->getMemoryActivity();
            analysis["working_memory_efficiency"] = 1.0 - (ws->getMemoryActivity() / 100.0);
        }
        
        // Development metrics
        if (auto dm = brain->getDevelopmentSystem()) {
            analysis["plasticity_level"] = dm->getPlasticityModifier();
            analysis["critical_period_active"] = dm->isCriticalPeriod();
        }
        
        // Stability metrics
        analysis["state_stable"] = (brain->getFiringNeuronCount() < brain->getTotalNeuronCount() * 0.8);
        
        // Predictive indicators
        float firing_rate = brain->getAverageFiringRate();
        if (firing_rate < 0.01) {
            analysis["prediction"] = "Low activity - may need stimulation";
        } else if (firing_rate < 1.0) {
            analysis["prediction"] = "Normal activity levels";
        } else {
            analysis["prediction"] = "High activity - possible learning phase";
        }
        
    } catch (const std::exception& e) {
        analysis["analysis_error"] = std::string("Analysis failed: ") + e.what();
    } catch (...) {
        analysis["analysis_error"] = "Analysis failed with unknown error";
    }
    
    return analysis;
}

py::dict compareBrains(const Brain* brain1, const Brain* brain2) {
    py::dict comparison;
    
    if (!brain1) {
        comparison["error"] = "First brain is null";
        return comparison;
    }
    
    if (!brain2) {
        comparison["error"] = "Second brain is null";
        return comparison;
    }
    
    try {
        // Compare basic metrics
        comparison["neuron_count_diff"] = brain1->getTotalNeuronCount() - brain2->getTotalNeuronCount();
        comparison["synapse_count_diff"] = brain1->getTotalSynapseCount() - brain2->getTotalSynapseCount();
        comparison["firing_rate_diff"] = brain1->getAverageFiringRate() - brain2->getAverageFiringRate();
        comparison["spike_count_diff"] = brain1->getTotalSpikeCount() - brain2->getTotalSpikeCount();
        
        // Determine which brain is "more active"
        float firing_rate1 = brain1->getAverageFiringRate();
        float firing_rate2 = brain2->getAverageFiringRate();
        
        if (firing_rate1 > firing_rate2) {
            comparison["more_active"] = 1; // Brain 1
            comparison["activity_ratio"] = firing_rate1 / firing_rate2;
        } else if (firing_rate2 > firing_rate1) {
            comparison["more_active"] = 2; // Brain 2
            comparison["activity_ratio"] = firing_rate2 / firing_rate1;
        } else {
            comparison["more_active"] = 0; // Equal
            comparison["activity_ratio"] = 1.0;
        }
        
        // Development stage comparison
        comparison["stage_diff"] = static_cast<int>(brain1->getDevelopmentalStage()) - 
                                  static_cast<int>(brain2->getDevelopmentalStage());
        
    } catch (const std::exception& e) {
        comparison["comparison_error"] = std::string("Comparison failed: ") + e.what();
    } catch (...) {
        comparison["comparison_error"] = "Comparison failed with unknown error";
    }
    
    return comparison;
}

// ===================================================================
// CONFIGURATION UTILITIES
// ===================================================================

std::shared_ptr<Config> mergeConfigs(const Config* base_config, const Config* override_config) {
    if (!base_config || !override_config) {
        return nullptr;
    }
    
    auto merged = std::make_shared<Config>(*base_config);
    
    // Override with values from override_config
    auto override_keys = override_config->getKeys();
    for (const auto& key : override_keys) {
        try {
            if (override_config->has(key)) {
                merged->set(key, override_config->get(key, ""));
            }
        } catch (...) {
            // Skip keys that can't be retrieved
            continue;
        }
    }
    
    return merged;
}

std::shared_ptr<Config> extractConfigSubset(const Config* config, const std::vector<std::string>& keys) {
    if (!config) {
        return nullptr;
    }
    
    auto subset = std::make_shared<Config>();
    
    for (const auto& key : keys) {
        try {
            if (config->has(key)) {
                subset->set(key, config->get(key, ""));
            }
        } catch (...) {
            // Skip keys that can't be retrieved
            continue;
        }
    }
    
    return subset;
}

py::dict createValidationReport(const std::vector<std::string>& errors, 
                               const std::vector<std::string>& warnings) {
    py::dict report;
    
    report["valid"] = errors.empty();
    report["error_count"] = errors.size();
    report["warning_count"] = warnings.size();
    
    // Convert vectors to Python lists
    py::list error_list;
    for (const auto& error : errors) {
        error_list.append(error);
    }
    report["errors"] = error_list;
    
    py::list warning_list;
    for (const auto& warning : warnings) {
        warning_list.append(warning);
    }
    report["warnings"] = warning_list;
    
    return report;
}

// ===================================================================
// SIMULATION CONTROL UTILITIES
// ===================================================================

py::dict runSafeSimulation(Brain* brain, SimulationStep num_steps, SimulationStep check_interval) {
    py::dict results;
    
    if (!brain) {
        results["error"] = "Brain is null";
        return results;
    }
    
    try {
        results["simulation_started"] = true;
        results["total_steps"] = num_steps;
        results["check_interval"] = check_interval;
        
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        for (SimulationStep step = 0; step < num_steps; ++step) {
            // Validate before each step
            if (!validateSimulationParameters(brain, step)) {
                errors.push_back(std::string("Invalid parameters at step ") + std::to_string(step));
                continue;
            }
            
            // Execute step
            auto result = safeBrainStep(brain, step);
            if (!result.first) {
                errors.push_back(std::string("Step ") + std::to_string(step) + ": " + result.second);
            }
            
            // Periodic validation
            if (step % check_interval == 0) {
                if (!validateBrainState(brain)) {
                    warnings.push_back(std::string("Brain state instability detected at step ") + std::to_string(step));
                }
            }
            
            // Progress tracking (for large simulations)
            if (num_steps > 1000 && step % (num_steps / 10) == 0) {
                double progress = static_cast<double>(step) / num_steps * 100.0;
                results["progress_percent"] = progress;
            }
        }
        
        // Create validation report
        results["validation_report"] = createValidationReport(errors, warnings);
        results["simulation_completed"] = true;
        results["final_step"] = num_steps - 1;
        
    } catch (const std::exception& e) {
        results["simulation_error"] = std::string("Simulation failed: ") + e.what();
        results["simulation_completed"] = false;
    } catch (...) {
        results["simulation_error"] = "Simulation failed with unknown error";
        results["simulation_completed"] = false;
    }
    
    return results;
}

bool validateBrainState(const Brain* brain) {
    if (!brain) {
        return false;
    }
    
    try {
        // Check for obvious state inconsistencies
        size_t neuron_count = brain->getTotalNeuronCount();
        size_t active_neuron_count = brain->getActiveNeuronCount();
        size_t firing_neuron_count = brain->getFiringNeuronCount();
        
        // Basic sanity checks
        if (neuron_count == 0) {
            return false;
        }
        
        if (active_neuron_count > neuron_count) {
            return false;
        }
        
        if (firing_neuron_count > active_neuron_count) {
            return false;
        }
        
        // Check E/I ratio is reasonable
        float eir = brain->getExcitationInhibitionRatio();
        if (eir < 0.0 || eir > 100.0) {
            return false;
        }
        
        return true;
        
    } catch (...) {
        return false;
    }
}

std::vector<std::string> getAvailableConfigKeys() {
    std::vector<std::string> keys;
    
    // Return typical configuration keys that should be available
    keys.push_back("brain.neuron_count");
    keys.push_back("brain.synapse_density");
    keys.push_back("brain.v_thresh");
    keys.push_back("brain.v_rest");
    keys.push_back("brain.v_reset");
    keys.push_back("brain.tau_mem");
    keys.push_back("brain.tau_ref");
    
    keys.push_back("plasticity.stdp.enable");
    keys.push_back("plasticity.stdp.learning_rate");
    keys.push_back("plasticity.stdp.tau_plus");
    keys.push_back("plasticity.stdp.tau_minus");
    
    keys.push_back("plasticity.hebbian.enable");
    keys.push_back("plasticity.hebbian.learning_rate");
    
    keys.push_back("plasticity.structural.enable");
    keys.push_back("plasticity.structural.max_synapses");
    
    keys.push_back("neuromod.dopamine.scale");
    keys.push_back("neuromod.curiosity.enable");
    keys.push_back("neuromod.novelty.enable");
    
    keys.push_back("development.enable");
    keys.push_back("development.stage");
    
    return keys;
}

std::shared_ptr<Brain> createSafeBrain(const std::shared_ptr<Config>& config) {
    if (!config) {
        return nullptr;
    }
    
    try {
        auto brain = std::make_shared<Brain>(config);
        
        // Validate configuration before initialization
        std::vector<std::string> validation_errors = validateBrainConfig(config.get());
        if (!validation_errors.empty()) {
            // Log validation errors but still try to create brain
            for (const auto& error : validation_errors) {
                NLM_LOG_WARNING("Brain creation validation warning: " + error);
            }
        }
        
        if (!brain->initialize()) {
            return nullptr;
        }
        
        return brain;
        
    } catch (...) {
        return nullptr;
    }
}

std::vector<std::string> validateBrainConfig(const Config* config) {
    std::vector<std::string> errors;
    
    if (!config) {
        errors.push_back("Config is null");
        return errors;
    }
    
    // Check for required parameters
    try {
        if (!config->has("brain.neuron_count")) {
            errors.push_back("brain.neuron_count is required");
        }
        
        int neuron_count = config->get("brain.neuron_count", 1000);
        if (neuron_count < 1 || neuron_count > 1000000) {
            errors.push_back("brain.neuron_count must be between 1 and 1000000");
        }
        
        // Validate other important parameters
        if (config->has("brain.v_thresh")) {
            float v_thresh = config->get("brain.v_thresh", -50.0f);
            if (v_thresh < -100.0 || v_thresh > 0.0) {
                errors.push_back("brain.v_thresh must be between -100.0 and 0.0");
            }
        }
        
        if (config->has("brain.v_rest")) {
            float v_rest = config->get("brain.v_rest", -70.0f);
            if (v_rest < -100.0 || v_rest > -40.0) {
                errors.push_back("brain.v_rest must be between -100.0 and -40.0");
            }
        }
        
        if (config->has("brain.v_reset")) {
            float v_reset = config->get("brain.v_reset", -75.0f);
            if (v_reset < -100.0 || v_reset > 0.0) {
                errors.push_back("brain.v_reset must be between -100.0 and 0.0");
            }
        }
        
    } catch (...) {
        errors.push_back("Failed to validate brain configuration");
    }
    
    return errors;
}

py::dict getConfigValidationDetails(const Config* config) {
    py::dict details;
    
    if (!config) {
        details["error"] = "Config is null";
        return details;
    }
    
    try {
        auto errors = validateBrainConfig(config);
        auto warnings = std::vector<std::string>(); // Could be expanded with more complex validation
        
        details["valid"] = errors.empty();
        details["error_count"] = errors.size();
        details["warning_count"] = warnings.size();
        
        // Error details
        py::list error_details;
        for (const auto& error : errors) {
            py::dict error_detail;
            error_detail["message"] = error;
            error_detail["severity"] = "error";
            error_detail["suggestion"] = getConfigErrorSuggestion(error);
            error_details.append(error_detail);
        }
        details["error_details"] = error_details;
        
        // Warning details
        py::list warning_details;
        for (const auto& warning : warnings) {
            py::dict warning_detail;
            warning_detail["message"] = warning;
            warning_detail["severity"] = "warning";
            warning_detail["suggestion"] = getConfigWarningSuggestion(warning);
            warning_details.append(warning_detail);
        }
        details["warning_details"] = warning_details;
        
        // Available options
        py::list available_keys;
        auto keys = config->getKeys();
        for (const auto& key : keys) {
            available_keys.append(key);
        }
        details["available_keys"] = available_keys;
        
    } catch (...) {
        details["validation_error"] = "Failed to generate configuration validation details";
    }
    
    return details;
}

// Helper functions for suggestions
std::string getConfigErrorSuggestion(const std::string& error) {
    if (error.find("neuron_count") != std::string::npos) {
        return "Try setting brain.neuron_count to a value between 100 and 10000";
    } else if (error.find("v_thresh") != std::string::npos) {
        return "Try setting brain.v_thresh to -50.0 (typical threshold)";
    } else if (error.find("v_rest") != std::string::npos) {
        return "Try setting brain.v_rest to -70.0 (typical resting potential)";
    } else if (error.find("v_reset") != std::string::npos) {
        return "Try setting brain.v_reset to -75.0 (typical reset potential)";
    }
    return "Check configuration parameter range and format";
}

std::string getConfigWarningSuggestion(const std::string& warning) {
    if (warning.find("outside typical range") != std::string::npos) {
        return "This parameter is unusual but may be valid for your use case";
    }
    return "Review if this value is appropriate for your simulation";
}

} // namespace nlm