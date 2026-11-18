#pragma once

#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace nlm {

// Validation functions for NLM components

class NLMValidator {
public:
    // Validation constants
    static constexpr size_t MAX_NEURONS = 1000000;
    static constexpr size_t MAX_SYNAPSES = 10000000;
    static constexpr size_t MAX_REGIONS = 1000;
    static constexpr size_t MAX_POPULATIONS = 1000;
    static constexpr double MIN_TIME_STEP = 0.0;
    static constexpr double MAX_TIME_STEP = 1.0;
    static constexpr int64_t MIN_NEURON_COUNT = 1;
    static constexpr int64_t MIN_REGION_COUNT = 1;
    static constexpr uint64_t MIN_NEURON_ID = 0;
    static constexpr uint64_t MAX_NEURON_ID = static_cast<uint64_t>(std::numeric_limits<uint64_t>::max());
    
    // Configuration validation
    static void validate_config(const std::map<std::string, std::any>& config);
    static void validate_neuron_id(uint64_t neuron_id);
    static void validate_memory_bounds(size_t current_usage, size_t max_capacity, const std::string& component_name);
    static void validate_simulation_step(int step, size_t max_steps = 1000000);
    static void validate_plasticity_parameters(const std::string& rule_name, const std::map<std::string, double>& parameters);
    static void validate_developmental_stage(const std::string& current_stage, const std::string& required_stage = "");
    static void validate_neuromodulation(const std::string& neuromodulator, const std::string& target_neuron, double signal_strength);
    
    // Type validation
    static void validate_parameter_type(const std::string& key, const std::any& value, const std::type_info& expected_type);
    static void validate_parameter_bounds(const std::string& key, double value, double min_val, double max_val);
    static void validate_parameter_range(const std::string& key, int64_t value, int64_t min_val, int64_t max_val);
    
    // Component-specific validation
    static void validate_brain_parameters(const std::map<std::string, std::any>& config);
    static void validate_world_parameters(const std::map<std::string, std::any>& config);
    static void validate_agent_parameters(const std::map<std::string, std::any>& config);
    
    // Error checking
    static bool check_file_exists(const std::string& filepath);
    static bool check_directory_writable(const std::string& directory);
    static bool check_config_completeness(const std::map<std::string, std::any>& config);
    static std::vector<std::string> get_missing_config_keys(const std::map<std::string, std::any>& config);
    
    // Error formatting
    static std::string format_validation_error(const std::string& component, const std::string& message, const std::string& details = "");
    static std::string format_parameter_error(const std::string& key, const std::any& value, const std::string& expected_type);
    static std::string format_bounds_error(const std::string& key, double value, double min_val, double max_val);
    
    // Validation state
    static void reset_validation_state();
    static size_t get_validation_error_count();
    static std::vector<std::string> get_validation_errors();
    
private:
    static std::map<std::string, std::any> default_config_;
    static std::vector<std::string> validation_errors_;
    static size_t validation_error_count_;
};

} // namespace nlm