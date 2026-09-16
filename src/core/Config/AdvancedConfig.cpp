#include "AdvancedConfig.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <unordered_set>
#include <future>
#include <chrono>

namespace nlm {

AdvancedConfig::AdvancedConfig() {}

AdvancedConfig::~AdvancedConfig() {}

bool AdvancedConfig::loadFromFile(const std::string& filepath) {
    std::string format = detectFileFormat(filepath);
    
    if (format == "json") {
        return loadJsonFile(filepath);
    } else if (format == "yaml") {
        return loadYamlFile(filepath);
    } else {
        return loadKeyValueFile(filepath);
    }
}

bool AdvancedConfig::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Try to parse value type
                if (value.find('.') != std::string::npos) {
                    // Try double
                    try {
                        double doubleVal = std::stod(value);
                        set(key, doubleVal);
                    } catch (...) {
                        set(key, value);
                    }
                } else {
                    // Try int
                    try {
                        long long intVal = std::stoll(value);
                        set(key, intVal);
                    } catch (...) {
                        // Try bool
                        if (value == "true" || value == "false") {
                            bool boolVal = (value == "true");
                            set(key, boolVal);
                        } else {
                            set(key, value);
                        }
                    }
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value);
        }
        // Handle --key value (when value doesn't contain =)
        else if (arg.substr(0, 2) == "--" && !arg.empty()) {
            // Check if next arg exists and doesn't start with -
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                std::string key = arg.substr(2);
                std::string value = argv[++i];
                set(key, value);
            }
        }
    }
    return true;
}

bool AdvancedConfig::process(const InheritanceConfig& inheritance) {
    if (inheritance.baseConfigFile.empty()) {
        return true;
    }
    
    json baseConfig;
    if (loadJsonFile(inheritance.baseConfigFile)) {
        mergeConfigs(configData, baseConfig);
        return true;
    }
    return false;
}

AdvancedConfig::ValidationResult AdvancedConfig::validate() const {
    ValidationResult result;
    
    // Schema validation
    auto schemaResult = validateSchema();
    if (!schemaResult.isValid) {
        result.isValid = false;
        result.errors = schemaResult.errors;
        result.warnings = schemaResult.warnings;
        return result;
    }
    
    // Cross-component validation
    auto crossResult = validateCrossComponent();
    if (!crossResult.isValid) {
        result.isValid = false;
        result.errors = crossResult.errors;
        result.warnings = crossResult.warnings;
        return result;
    }
    
    result.warnings = crossResult.warnings;
    return result;
}

void AdvancedConfig::applyToBaseConfig(std::shared_ptr<Config> baseConfig) const {
    this->baseConfig = baseConfig;
    
    for (const auto& [key, value] : configData.items()) {
        if (value.is_number_integer()) {
            baseConfig->set(key, value.get<int64_t>(), ConfigSource::File);
        } else if (value.is_number_float()) {
            baseConfig->set(key, value.get<double>(), ConfigSource::File);
        } else if (value.is_boolean()) {
            baseConfig->set(key, value.get<bool>(), ConfigSource::File);
        } else if (value.is_string()) {
            baseConfig->set(key, value.get<std::string>(), ConfigSource::File);
        } else if (value.is_array()) {
            // Handle arrays (simplified)
            if (!value.empty()) {
                if (value[0].is_number_integer()) {
                    std::vector<int64_t> intArray;
                    for (const auto& item : value) {
                        intArray.push_back(item.get<int64_t>());
                    }
                    baseConfig->set(key, intArray, ConfigSource::File);
                }
            }
        }
    }
}

template<typename T>
T AdvancedConfig::get(const std::string& key, const T& defaultValue) const {
    if (configData.contains(key)) {
        return configData[key].get<T>();
    }
    return defaultValue;
}

json AdvancedConfig::getRaw(const std::string& key) const {
    if (configData.contains(key)) {
        return configData[key];
    }
    return json();
}

void AdvancedConfig::set(const std::string& key, const json& value, ConfigSource source) {
    configData[key] = value;
}

bool AdvancedConfig::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Save in JSON format
    file << std::setw(2) << configData << std::endl;
    return true;
}

std::string AdvancedConfig::exportToJson() const {
    std::ostringstream oss;
    oss << std::setw(2) << configData;
    return oss.str();
}

bool AdvancedConfig::loadTemplate(TemplateType type) {
    configData = getTemplateJson(type);
    return !configData.empty();
}

json AdvancedConfig::getTemplateJson(TemplateType type) {
    switch (type) {
        case TemplateType::PHASE1_BASIC:
            return {
                {"simulation", {
                    {"random_seed", 42},
                    {"simulation_timestep", 0.001},
                    {"max_simulation_steps", 10000}
                }},
                {"neural", {
                    {"neuron_count", 100},
                    {"region_count", 1},
                    {"connection_probability", 0.1}
                }},
                {"plasticity", {
                    {"plasticity_learning_rate", 0.01},
                    {"stdp_ltp_weight", 0.01},
                    {"stdp_ltd_weight", 0.012}
                }},
                {"development", {
                    {"development_synaptogenesis_rate", 0.001},
                    {"development_pruning_rate", 0.0001}
                }},
                {"neuromodulation", {
                    {"dopamine_baseline", 0.1}
                }},
                {"reward", {
                    {"reward_discount_factor", 0.99}
                }},
                {"environment", {
                    {"environment_name", "GridWorld"},
                    {"environment_width", 10},
                    {"environment_height", 10}
                }},
                {"logging", {
                    {"log_level", "INFO"},
                    {"log_to_file", false},
                    {"log_filename", "nlm.log"}
                }},
                {"visualization", {
                    {"visualization_enabled", false},
                    {"visualization_update_rate", 30.0}
                }}
            };
        
        case TemplateType::PHASE2_NEURAL_COMPUTATION:
            return {
                {"simulation", {
                    {"random_seed", 42},
                    {"simulation_timestep", 0.001},
                    {"max_simulation_steps", 10000}
                }},
                {"neural", {
                    {"neuron_count", 500},
                    {"region_count", 1},
                    {"connection_probability", 0.15},
                    {"neuron_type", "LIF"}
                }},
                {"plasticity", {
                    {"plasticity_learning_rate", 0.01},
                    {"stdp_ltp_weight", 0.02},
                    {"stdp_ltd_weight", 0.015},
                    {"stdp_time_constant", 20.0}
                }},
                {"development", {
                    {"development_synaptogenesis_rate", 0.0001},
                    {"development_pruning_rate", 0.00001}
                }},
                {"neuromodulation", {
                    {"dopamine_baseline", 0.1},
                    {"curiosity_baseline", 0.5}
                }},
                {"reward", {
                    {"reward_discount_factor", 0.99},
                    {"reward_scale", 1.0}
                }},
                {"environment", {
                    {"environment_name", "GridWorld"},
                    {"environment_width", 10},
                    {"environment_height", 10}
                }},
                {"logging", {
                    {"log_level", "INFO"},
                    {"log_to_file", true},
                    {"log_filename", "nlm_phase2.log"},
                    {"log_file_append", true}
                }},
                {"visualization", {
                    {"visualization_enabled", true},
                    {"visualization_update_rate", 30.0}
                }}
            };
        
        case TemplateType::PHASE3_AGENT_WORLD_INTERACTION:
            return {
                {"simulation", {
                    {"random_seed", 42},
                    {"simulation_timestep", 0.001},
                    {"max_simulation_steps", 50000}
                }},
                {"neural", {
                    {"neuron_count", 1000},
                    {"region_count", 2},
                    {"connection_probability", 0.2},
                    {"neuron_type", "LIF"}
                }},
                {"plasticity", {
                    {"plasticity_learning_rate", 0.02},
                    {"stdp_ltp_weight", 0.025},
                    {"stdp_ltd_weight", 0.02},
                    {"stdp_time_constant", 30.0}
                }},
                {"development", {
                    {"development_synaptogenesis_rate", 0.0005},
                    {"development_pruning_rate", 0.00005},
                    {"maturation_rate", 0.001}
                }},
                {"neuromodulation", {
                    {"dopamine_baseline", 0.2},
                    {"curiosity_baseline", 1.0},
                    {"novelty_threshold", 0.3}
                }},
                {"reward", {
                    {"reward_discount_factor", 0.95},
                    {"reward_scale", 2.0}
                }},
                {"environment", {
                    {"environment_name", "GridWorld"},
                    {"environment_width", 20},
                    {"environment_height", 20},
                    {"world_bounds_x", 100.0},
                    {"world_bounds_y", 100.0}
                }},
                {"logging", {
                    {"log_level", "INFO"},
                    {"log_to_file", true},
                    {"log_filename", "nlm_phase3.log"},
                    {"log_file_append", true}
                }},
                {"visualization", {
                    {"visualization_enabled", true},
                    {"visualization_update_rate", 30.0},
                    {"visualization_window_width", 1200},
                    {"visualization_window_height", 800}
                }}
            };
        
        case TemplateType::PHASE4_COGNITIVE_DEVELOPMENT:
            return {
                {"simulation", {
                    {"random_seed", 42},
                    {"simulation_timestep", 0.001},
                    {"max_simulation_steps", 100000}
                }},
                {"neural", {
                    {"neuron_count", 2000},
                    {"region_count", 3},
                    {"connection_probability", 0.25},
                    {"neuron_type", "Izhikevich"}
                }},
                {"plasticity", {
                    {"plasticity_learning_rate", 0.03},
                    {"stdp_ltp_weight", 0.03},
                    {"stdp_ltd_weight", 0.025},
                    {"stdp_time_constant", 40.0}
                }},
                {"development", {
                    {"development_synaptogenesis_rate", 0.001},
                    {"development_pruning_rate", 0.0002},
                    {"maturation_rate", 0.002}
                }},
                {"neuromodulation", {
                    {"dopamine_baseline", 0.3},
                    {"curiosity_baseline", 1.5},
                    {"novelty_threshold", 0.5}
                }},
                {"reward", {
                    {"reward_discount_factor", 0.9},
                    {"reward_scale", 3.0}
                }},
                {"environment", {
                    {"environment_name", "ComplexWorld"},
                    {"environment_width", 30},
                    {"environment_height", 30},
                    {"world_bounds_x", 200.0},
                    {"world_bounds_y", 200.0}
                }},
                {"logging", {
                    {"log_level", "DEBUG"},
                    {"log_to_file", true},
                    {"log_filename", "nlm_phase4.log"},
                    {"log_file_append", true}
                }},
                {"visualization", {
                    {"visualization_enabled", true},
                    {"visualization_update_rate", 30.0},
                    {"visualization_window_width", 1600},
                    {"visualization_window_height", 1200}
                }}
            };
        
        case TemplateType::PHASE5_PERFORMANCE_OPTIMIZATION:
            return {
                {"simulation", {
                    {"random_seed", 42},
                    {"simulation_timestep", 0.0005},
                    {"max_simulation_steps", 200000}
                }},
                {"neural", {
                    {"neuron_count", 5000},
                    {"region_count", 4},
                    {"connection_probability", 0.3},
                    {"neuron_type", "LIF"}
                }},
                {"plasticity", {
                    {"plasticity_learning_rate", 0.04},
                    {"stdp_ltp_weight", 0.04},
                    {"stdp_ltd_weight", 0.035},
                    {"stdp_time_constant", 50.0}
                }},
                {"development", {
                    {"development_synaptogenesis_rate", 0.002},
                    {"development_pruning_rate", 0.0003},
                    {"maturation_rate", 0.003}
                }},
                {"neuromodulation", {
                    {"dopamine_baseline", 0.5},
                    {"curiosity_baseline", 2.0},
                    {"novelty_threshold", 0.7}
                }},
                {"reward", {
                    {"reward_discount_factor", 0.8},
                    {"reward_scale", 5.0}
                }},
                {"environment", {
                    {"environment_name", "HighPerformanceWorld"},
                    {"environment_width", 50},
                    {"environment_height", 50},
                    {"world_bounds_x", 500.0},
                    {"world_bounds_y", 500.0}
                }},
                {"logging", {
                    {"log_level", "WARNING"},
                    {"log_to_file", true},
                    {"log_filename", "nlm_phase5.log"},
                    {"log_file_append", true}
                }},
                {"visualization", {
                    {"visualization_enabled", false},
                    {"visualization_update_rate", 10.0}
                }}
            };
        
        case TemplateType::PHASE6_FULL_INTEGRATION:
            return {
                {"simulation", {
                    {"random_seed", 42},
                    {"simulation_timestep", 0.0005},
                    {"max_simulation_steps", 500000}
                }},
                {"neural", {
                    {"neuron_count", 10000},
                    {"region_count", 5},
                    {"connection_probability", 0.35},
                    {"neuron_type", "LIF"}
                }},
                {"plasticity", {
                    {"plasticity_learning_rate", 0.05},
                    {"stdp_ltp_weight", 0.05},
                    {"stdp_ltd_weight", 0.045},
                    {"stdp_time_constant", 60.0}
                }},
                {"development", {
                    {"development_synaptogenesis_rate", 0.003},
                    {"development_pruning_rate", 0.0005},
                    {"maturation_rate", 0.005}
                }},
                {"neuromodulation", {
                    {"dopamine_baseline", 0.8},
                    {"curiosity_baseline", 3.0},
                    {"novelty_threshold", 0.8}
                }},
                {"reward", {
                    {"reward_discount_factor", 0.7},
                    {"reward_scale", 10.0}
                }},
                {"environment", {
                    {"environment_name", "FullIntegrationWorld"},
                    {"environment_width", 100},
                    {"environment_height", 100},
                    {"world_bounds_x", 1000.0},
                    {"world_bounds_y", 1000.0}
                }},
                {"logging", {
                    {"log_level", "ERROR"},
                    {"log_to_file", true},
                    {"log_filename", "nlm_phase6.log"},
                    {"log_file_append", true},
                    {"log_max_size", 100 * 1024 * 1024}
                }},
                {"visualization", {
                    {"visualization_enabled", true},
                    {"visualization_update_rate", 10.0},
                    {"visualization_window_width", 1920},
                    {"visualization_window_height", 1080}
                }}
            };
    }
}

bool AdvancedConfig::processBatch(const std::vector<std::string>& configFiles) {
    bool allSuccess = true;
    for (const auto& file : configFiles) {
        if (!loadFromFile(file)) {
            allSuccess = false;
        }
    }
    return allSuccess;
}

void AdvancedConfig::interactiveSetup() {
    std::cout << "=== Advanced NLM Configuration Setup ===" << std::endl;
    
    std::cout << "Enter simulation timestep (default: 0.001): ";
    double timestep;
    std::cin >> timestep;
    set("simulation_timestep", timestep);
    
    std::cout << "Enter neuron count (default: 1000): ";
    int64_t neuronCount;
    std::cin >> neuronCount;
    set("neuron_count", neuronCount);
    
    std::cout << "Enter log level (DEBUG, INFO, WARNING, ERROR, CRITICAL): ";
    std::string logLevel;
    std::cin >> logLevel;
    set("log_level", logLevel);
    
    std::cout << "Enable visualization? (y/n, default: n): ";
    char vizChoice;
    std::cin >> vizChoice;
    set("visualization_enabled", (vizChoice == 'y' || vizChoice == 'Y'));
    
    std::cout << "Configuration setup complete!" << std::endl;
}

void AdvancedConfig::configureLogging(const LoggingConfig& logging) {
    loggingConfig = logging;
    
    // Update config data
    set("log_level", logging.level);
    set("log_to_file", logging.file);
    set("log_filename", logging.filename);
    set("log_file_append", logging.append);
    set("log_max_size", logging.maxSize);
}

void AdvancedConfig::configureDebug(const DebugConfig& debug) {
    debugConfig = debug;
    
    // Update config data
    set("debug_enabled", debug.enabled);
    set("debug_profile", debug.profile);
    set("debug_breakpoints", debug.breakpoints);
    set("debug_validate", debug.validate);
    set("debug_verbose", debug.verbose);
}

bool AdvancedConfig::executeParallel(const std::vector<std::string>& configFiles) {
    std::vector<std::future<bool>> futures;
    
    for (const auto& file : configFiles) {
        futures.push_back(std::async(std::launch::async, [this, file]() {
            return loadFromFile(file);
        }));
    }
    
    bool allSuccess = true;
    for (auto& future : futures) {
        if (!future.get()) {
            allSuccess = false;
        }
    }
    
    return allSuccess;
}

AdvancedConfig::ValidationResult AdvancedConfig::validateAndExport(const std::string& outputPath) const {
    ValidationResult result = validate();
    
    if (result.isValid) {
        if (saveToFile(outputPath)) {
            result.message = "Configuration validated and exported successfully to: " + outputPath;
        } else {
            result.isValid = false;
            result.errors["export"] = "Failed to save configuration to file";
        }
    }
    
    return result;
}

bool AdvancedConfig::loadJsonFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        json::parse(file, configData, true, true);
        return true;
    } catch (const json::parse_error& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return false;
    }
}

bool AdvancedConfig::loadYamlFile(const std::string& filepath) {
    // YAML parsing would require a YAML library
    // For now, assume it's not YAML format
    return false;
}

bool AdvancedConfig::loadKeyValueFile(const std::string& filepath) {
    return baseConfig->loadFromFile(filepath);
}

AdvancedConfig::ValidationResult AdvancedConfig::validateSchema() const {
    ValidationResult result;
    
    // Simple schema validation - can be expanded
    for (const auto& [key, value] : configData.items()) {
        if (key == "random_seed" && !value.is_number_integer()) {
            result.errors[key] = "random_seed must be an integer";
            result.isValid = false;
        } else if (key == "simulation_timestep" && !value.is_number()) {
            result.errors[key] = "simulation_timestep must be a number";
            result.isValid = false;
        } else if (key == "neuron_count" && !value.is_number_integer()) {
            result.errors[key] = "neuron_count must be an integer";
            result.isValid = false;
        } else if (key == "connection_probability") {
            if (!value.is_number()) {
                result.errors[key] = "connection_probability must be a number";
                result.isValid = false;
            } else if (value.get<double>() < 0 || value.get<double>() > 1) {
                result.errors[key] = "connection_probability must be between 0 and 1";
                result.isValid = false;
            }
        }
    }
    
    // Check required sections
    std::unordered_set<std::string> requiredSections = {"simulation", "neural", "plasticity"};
    for (const auto& section : requiredSections) {
        if (!configData.contains(section)) {
            result.warnings["schema"] += "Missing required section: " + section + "\n";
        }
    }
    
    return result;
}

AdvancedConfig::ValidationResult AdvancedConfig::validateCrossComponent() const {
    ValidationResult result;
    
    if (configData.contains("max_simulation_steps") && configData.contains("simulation_time_limit")) {
        if (configData["simulation_time_limit"].get<double>() > 0 && 
            configData["max_simulation_steps"].get<int64_t>() <= 0) {
            result.errors["cross"] = "max_simulation_steps must be positive when simulation_time_limit > 0";
            result.isValid = false;
        }
    }
    
    return result;
}

void AdvancedConfig::mergeConfigs(json& target, const json& source, const std::unordered_map<std::string, json>& inheritanceRules) {
    for (const auto& [key, value] : source.items()) {
        if (target.contains(key)) {
            if (target[key].is_object() && value.is_object()) {
                mergeConfigs(target[key], value, inheritanceRules);
            } else {
                target[key] = value;
            }
        } else {
            target[key] = value;
        }
    }
}

std::string AdvancedConfig::detectFileFormat(const std::string& filepath) {
    if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".json") {
        return "json";
    } else if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".yaml") {
        return "yaml";
    } else if (filepath.size() >= 4 && filepath.substr(filepath.size() - 4) == ".cfg") {
        return "keyvalue";
    } else {
        return "keyvalue"; // Default assumption
    }
}

} // namespace nlm
