#include "ConfigValidator.hpp"
#include <sstream>
#include <algorithm>
#include <cmath>

namespace nlm {

std::vector<ConfigValidationError> ConfigValidator::validate(const std::vector<ConfigEntry>& entries) {
    std::vector<ConfigValidationError> errors;
    
    for (const auto& entry : entries) {
        auto paramErrors = validateParameter(entry.key, entry.value);
        errors.insert(errors.end(), paramErrors.begin(), paramErrors.end());
    }
    
    auto depErrors = validateParameterDependencies(entries);
    errors.insert(errors.end(), depErrors.begin(), depErrors.end());
    
    return errors;
}

std::vector<ConfigValidationError> ConfigValidator::validateParameter(const std::string& key, const ConfigValue& value) {
    std::vector<ConfigValidationError> errors;
    
    // Basic range validation for common parameters
    double val = 0.0;
    
    try {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            
            if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>) {
                val = static_cast<double>(arg);
                
                // Define parameter ranges
                if (key == "neuron_count") {
                    if (val < 1 || val > 1000000) {
                        ConfigValidationError error;
                        error.code = ConfigValidationError::ErrorCode::InvalidRange;
                        error.parameter = key;
                        error.value = std::to_string(val);
                        error.message = "Value is outside valid range [1, 1000000]";
                        error.suggestion = "Adjust neuron count to be within 1-1000000";
                        errors.push_back(error);
                    }
                }
                else if (key == "region_count") {
                    if (val < 1 || val > 100) {
                        ConfigValidationError error;
                        error.code = ConfigValidationError::ErrorCode::InvalidRange;
                        error.parameter = key;
                        error.value = std::to_string(val);
                        error.message = "Value is outside valid range [1, 100]";
                        error.suggestion = "Adjust region count to be within 1-100";
                        errors.push_back(error);
                    }
                }
                else if (key == "connection_probability") {
                    if (val < 0.0 || val > 1.0) {
                        ConfigValidationError error;
                        error.code = ConfigValidationError::ErrorCode::InvalidRange;
                        error.parameter = key;
                        error.value = std::to_string(val);
                        error.message = "Value is outside valid range [0.0, 1.0]";
                        error.suggestion = "Adjust connection probability to be within 0.0-1.0";
                        errors.push_back(error);
                    }
                }
                else if (key == "stdp_ltp_weight") {
                    if (val < 0.0 || val > 1.0) {
                        ConfigValidationError error;
                        error.code = ConfigValidationError::ErrorCode::InvalidRange;
                        error.parameter = key;
                        error.value = std::to_string(val);
                        error.message = "Value is outside valid range [0.0, 1.0]";
                        error.suggestion = "Adjust LTP weight to be within 0.0-1.0";
                        errors.push_back(error);
                    }
                }
                else if (key == "stdp_ltd_weight") {
                    if (val < 0.0 || val > 1.0) {
                        ConfigValidationError error;
                        error.code = ConfigValidationError::ErrorCode::InvalidRange;
                        error.parameter = key;
                        error.value = std::to_string(val);
                        error.message = "Value is outside valid range [0.0, 1.0]";
                        error.suggestion = "Adjust LTD weight to be within 0.0-1.0";
                        errors.push_back(error);
                    }
                }
                else if (key == "stdp_tau") {
                    if (val < 1.0 || val > 1000.0) {
                        ConfigValidationError error;
                        error.code = ConfigValidationError::ErrorCode::InvalidRange;
                        error.parameter = key;
                        error.value = std::to_string(val);
                        error.message = "Value is outside valid range [1.0, 1000.0]";
                        error.suggestion = "Adjust STDP time constant to be within 1.0-1000.0";
                        errors.push_back(error);
                    }
                }
                else if (key == "synaptogenesis_rate") {
                    if (val < 0.0 || val > 0.1) {
                        ConfigValidationError error;
                        error.code = ConfigValidationError::ErrorCode::InvalidRange;
                        error.parameter = key;
                        error.value = std::to_string(val);
                        error.message = "Value is outside valid range [0.0, 0.1]";
                        error.suggestion = "Adjust synaptogenesis rate to be within 0.0-0.1";
                        errors.push_back(error);
                    }
                }
                else if (key == "pruning_rate") {
                    if (val < 0.0 || val > 0.1) {
                        ConfigValidationError error;
                        error.code = ConfigValidationError::ErrorCode::InvalidRange;
                        error.parameter = key;
                        error.value = std::to_string(val);
                        error.message = "Value is outside valid range [0.0, 0.1]";
                        error.suggestion = "Adjust pruning rate to be within 0.0-0.1";
                        errors.push_back(error);
                    }
                }
                else if (key == "simulation_timestep") {
                    if (val < 0.0001 || val > 0.1) {
                        ConfigValidationError error;
                        error.code = ConfigValidationError::ErrorCode::InvalidRange;
                        error.parameter = key;
                        error.value = std::to_string(val);
                        error.message = "Value is outside valid range [0.0001, 0.1]";
                        error.suggestion = "Adjust simulation timestep to be within 0.0001-0.1";
                        errors.push_back(error);
                    }
                }
                else if (key == "replay_interval") {
                    if (val < 1 || val > 100000) {
                        ConfigValidationError error;
                        error.code = ConfigValidationError::ErrorCode::InvalidRange;
                        error.parameter = key;
                        error.value = std::to_string(val);
                        error.message = "Value is outside valid range [1, 100000]";
                        error.suggestion = "Adjust replay interval to be within 1-100000";
                        errors.push_back(error);
                    }
                }
                else if (key == "consolidation_interval") {
                    if (val < 1 || val > 100000) {
                        ConfigValidationError error;
                        error.code = ConfigValidationError::ErrorCode::InvalidRange;
                        error.parameter = key;
                        error.value = std::to_string(val);
                        error.message = "Value is outside valid range [1, 100000]";
                        error.suggestion = "Adjust consolidation interval to be within 1-100000";
                        errors.push_back(error);
                    }
                }
            }
            else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                val = static_cast<double>(arg);
                
                if (key == "neuron_count") {
                    if (val < 1 || val > 1000000) {
                        ConfigValidationError error;
                        error.code = ConfigValidationError::ErrorCode::InvalidRange;
                        error.parameter = key;
                        error.value = std::to_string(val);
                        error.message = "Value is outside valid range [1, 1000000]";
                        error.suggestion = "Adjust neuron count to be within 1-1000000";
                        errors.push_back(error);
                    }
                }
                else if (key == "region_count") {
                    if (val < 1 || val > 100) {
                        ConfigValidationError error;
                        error.code = ConfigValidationError::ErrorCode::InvalidRange;
                        error.parameter = key;
                        error.value = std::to_string(val);
                        error.message = "Value is outside valid range [1, 100]";
                        error.suggestion = "Adjust region count to be within 1-100";
                        errors.push_back(error);
                    }
                }
                else if (key == "replay_interval") {
                    if (val < 1 || val > 100000) {
                        ConfigValidationError error;
                        error.code = ConfigValidationError::ErrorCode::InvalidRange;
                        error.parameter = key;
                        error.value = std::to_string(val);
                        error.message = "Value is outside valid range [1, 100000]";
                        error.suggestion = "Adjust replay interval to be within 1-100000";
                        errors.push_back(error);
                    }
                }
                else if (key == "consolidation_interval") {
                    if (val < 1 || val > 100000) {
                        ConfigValidationError error;
                        error.code = ConfigValidationError::ErrorCode::InvalidRange;
                        error.parameter = key;
                        error.value = std::to_string(val);
                        error.message = "Value is outside valid range [1, 100000]";
                        error.suggestion = "Adjust consolidation interval to be within 1-100000";
                        errors.push_back(error);
                    }
                }
            }
        }, value);
    }
    catch (const std::bad_variant_access&) {
        ConfigValidationError error;
        error.code = ConfigValidationError::ErrorCode::InvalidType;
        error.parameter = key;
        error.value = "";
        error.message = "Invalid type for configuration parameter";
        error.suggestion = "Ensure the value matches the expected type";
        errors.push_back(error);
    }
    
    return errors;
}

std::vector<ConfigValidationError> ConfigValidator::validateParameterDependencies(const std::vector<ConfigEntry>& entries) {
    std::vector<ConfigValidationError> errors;
    
    // Validate region count doesn't exceed neuron count
    size_t neuronCount = 0;
    size_t regionCount = 0;
    
    for (const auto& entry : entries) {
        if (entry.key == "neuron_count") {
            try {
                neuronCount = std::get<size_t>(entry.value);
            } catch (...) {
                continue;
            }
        }
        else if (entry.key == "region_count") {
            try {
                regionCount = std::get<size_t>(entry.value);
            } catch (...) {
                continue;
            }
        }
    }
    
    if (neuronCount > 0 && regionCount > 0 && regionCount > neuronCount) {
        ConfigValidationError error;
        error.code = ConfigValidationError::ErrorCode::InvalidRange;
        error.parameter = "region_count";
        error.value = std::to_string(regionCount);
        error.message = "Region count cannot exceed neuron count";
        error.suggestion = "Reduce region count or increase neuron count";
        errors.push_back(error);
    }
    
    return errors;
}

std::vector<ConfigValidationError> ConfigValidator::validateFile(const std::string& filepath) {
    std::vector<ConfigValidationError> errors;
    
    if (!std::filesystem::exists(filepath)) {
        ConfigValidationError error;
        error.code = ConfigValidationError::ErrorCode::FileNotFound;
        error.parameter = filepath;
        error.value = "";
        error.message = "Configuration file not found";
        error.suggestion = "Ensure the file path is correct and the file exists";
        errors.push_back(error);
        return errors;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        ConfigValidationError error;
        error.code = ConfigValidationError::ErrorCode::InvalidFileFormat;
        error.parameter = filepath;
        error.value = "";
        error.message = "Cannot open configuration file";
        error.suggestion = "Check file permissions and ensure the file is readable";
        errors.push_back(error);
        return errors;
    }
    
    std::string line;
    size_t lineNum = 0;
    while (std::getline(file, line)) {
        lineNum++;
        line = Config::Impl().trim(line);
        
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            ConfigValidationError error;
            error.code = ConfigValidationError::ErrorCode::InvalidFileFormat;
            error.parameter = filepath;
            error.value = line;
            error.message = "Invalid configuration line format (expected key=value)";
            error.suggestion = "Fix line " + std::to_string(lineNum) + ": 'key = value' format";
            errors.push_back(error);
        }
    }
    
    return errors;
}

std::string ConfigValidator::errorCodeToString(ConfigValidationError::ErrorCode code) {
    switch (code) {
        case ConfigValidationError::ErrorCode::InvalidRange: return "InvalidRange";
        case ConfigValidationError::ErrorCode::InvalidType: return "InvalidType";
        case ConfigValidationError::ErrorCode::MissingParameter: return "MissingParameter";
        case ConfigValidationError::ErrorCode::InvalidFileFormat: return "InvalidFileFormat";
        case ConfigValidationError::ErrorCode::FileNotFound: return "FileNotFound";
        case ConfigValidationError::ErrorCode::InvalidValue: return "InvalidValue";
        case ConfigValidationError::ErrorCode::ParameterDependency: return "ParameterDependency";
        default: return "Unknown";
    }
}

} // namespace nlm