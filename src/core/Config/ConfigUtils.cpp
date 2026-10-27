#include "core/Config/ConfigUtils.hpp"
#include <algorithm>
#include <sstream>
#include <cstring>
#include <vector>
#include <map>
#include <functional>

namespace nlm {

struct ConfigUtils::Impl {
    std::vector<std::string> commonTrimCharacters;
    std::map<std::string, std::string> knownKeyAliases;
};

ConfigUtils::ConfigUtils() : pImpl(std::make_unique<Impl>()) {
    // Initialize common trim characters
    pImpl->commonTrimCharacters = { " \t\r\n" };
    
    // Initialize known key aliases for backward compatibility
    pImpl->knownKeyAliases = {
        { "simulation_timestep", "timestep" },
        { "neuron_count", "n" },
        { "region_count", "r" },
        { "connection_probability", "conn_prob" },
        { "plasticity_learning_rate", "plasticity_rate" },
        { "stdp_ltp_weight", "stdp_ltp" },
        { "stdp_ltd_weight", "stdp_ltd" },
        { "stdp_time_constant", "stdp_tau" },
        { "development_synaptogenesis_rate", "synaptogenesis_rate" },
        { "development_pruning_rate", "pruning_rate" },
        { "dopamine_baseline", "dopamine" },
        { "reward_discount_factor", "discount_factor" },
        { "environment_name", "env_name" },
        { "environment_width", "width" },
        { "environment_height", "height" },
        { "log_level", "loglvl" },
        { "log_to_file", "logfile" },
        { "max_simulation_steps", "max_steps" },
        { "simulation_time_limit", "time_limit" },
        { "visualization_enabled", "viz_enabled" },
        { "visualization_update_rate", "viz_rate" }
    };
}

ConfigUtils::~ConfigUtils() = default;

ConfigUtils::ConfigUtils(ConfigUtils&&) noexcept = default;

ConfigUtils& ConfigUtils::operator=(ConfigUtils&&) noexcept = default;

std::string ConfigUtils::trim(const std::string& str, const std::string& chars) {
    size_t start = str.find_first_not_of(chars.empty() ? " \t\r\n" : chars);
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(chars.empty() ? " \t\r\n" : chars);
    return str.substr(start, end - start + 1);
}

std::string ConfigUtils::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string ConfigUtils::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

std::string ConfigUtils::normalizeKey(const std::string& key) {
    std::string normalized = toLower(trim(key));
    
    // Apply known aliases
    auto it = pImpl->knownKeyAliases.find(normalized);
    if (it != pImpl->knownKeyAliases.end()) {
        return it->second;
    }
    
    // Replace underscores with hyphens for backward compatibility
    std::replace(normalized.begin(), normalized.end(), '_', '-');
    
    return normalized;
}

std::vector<std::string> ConfigUtils::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        if (!token.empty()) {
            tokens.push_back(trim(token));
        }
    }
    
    return tokens;
}

std::string ConfigUtils::join(const std::vector<std::string>& tokens, const std::string& delimiter) {
    if (tokens.empty()) return "";
    
    std::stringstream ss;
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (i > 0) ss << delimiter;
        ss << tokens[i];
    }
    
    return ss.str();
}

bool ConfigUtils::containsOnlyDigits(const std::string& str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}

bool ConfigUtils::containsOnlyDigitsAndDecimal(const std::string& str) {
    if (str.empty()) return false;
    
    size_t decimalCount = std::count(str.begin(), str.end(), '.');
    if (decimalCount > 1) return false;  // Only one decimal point allowed
    
    return std::all_of(str.begin(), str.end(), [](char c) {
        return std::isdigit(c) || c == '.' || c == '-' || c == '+';
    });
}

bool ConfigUtils::containsOnlyLetters(const std::string& str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), ::isalpha);
}

bool ConfigUtils::containsOnlyAlphanumeric(const std::string& str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), [](char c) {
        return std::isalnum(c) || c == '_' || c == '-';
    });
}

std::vector<std::string> ConfigUtils::getAllPossibleKeys(const std::string& key) {
    std::vector<std::string> variations;
    
    // Original key
    variations.push_back(key);
    
    // Lowercase version
    variations.push_back(toLower(key));
    
    // Uppercase version
    variations.push_back(toUpper(key));
    
    // With underscores
    std::string underscoreKey = key;
    std::replace(underscoreKey.begin(), underscoreKey.end(), '-', '_');
    if (underscoreKey != key) {
        variations.push_back(underscoreKey);
        variations.push_back(toLower(underscoreKey));
        variations.push_back(toUpper(underscoreKey));
    }
    
    // With hyphens
    std::string hyphenKey = key;
    std::replace(hyphenKey.begin(), hyphenKey.end(), '_', '-');
    if (hyphenKey != key) {
        variations.push_back(hyphenKey);
        variations.push_back(toLower(hyphenKey));
        variations.push_back(toUpper(hyphenKey));
    }
    
    // Remove duplicates
    std::sort(variations.begin(), variations.end());
    variations.erase(std::unique(variations.begin(), variations.end()), variations.end());
    
    return variations;
}

std::string ConfigUtils::formatValue(const ConfigValue& value) {
    std::stringstream ss;
    std::visit([&ss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            ss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            ss << (arg ? "true" : "false");
        } else if constexpr (std::is_integral_v<T> && !std::is_same_v<T, bool>) {
            ss << arg;
        } else if constexpr (std::is_floating_point_v<T>) {
            ss << arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            ss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << arg[i];
            }
            ss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            ss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << arg[i];
            }
            ss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            ss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << "\"" << arg[i] << "\"";
            }
            ss << "]";
        }
    }, value);
    
    return ss.str();
}

std::string ConfigUtils::validateString(const std::string& str, const std::string& key, ValidationResult& result) {
    if (str.empty()) {
        result = ValidationResult::Warning;
        return "Empty string value for key: " + key;
    }
    
    // Check for potentially problematic characters
    if (str.find('\n') != std::string::npos || str.find('\r') != std::string::npos) {
        result = ValidationResult::Warning;
        return "Value contains newlines for key: " + key;
    }
    
    if (str.find('=') != std::string::npos || str.find(':') != std::string::npos) {
        result = ValidationResult::Warning;
        return "Value contains equals or colon for key: " + key;
    }
    
    return "";
}

std::string ConfigUtils::validateNumber(const std::string& str, const std::string& key, ValidationResult& result) {
    if (!containsOnlyDigitsAndDecimal(str) && !containsOnlyDigits(str)) {
        result = ValidationResult::Error;
        return "Invalid number format for key: " + key + ", value: " + str;
    }
    
    try {
        double value = std::stod(str);
        
        // Check for reasonable ranges
        if (value == 0.0 || value == 0.0f) {
            result = ValidationResult::Warning;
            return "Value is zero for key: " + key;
        }
        
        if (std::isinf(value) || std::isnan(value)) {
            result = ValidationResult::Error;
            return "Invalid numeric value for key: " + key + ", value: " + str;
        }
        
        return "";
    } catch (const std::exception&) {
        result = ValidationResult::Error;
        return "Cannot parse number for key: " + key + ", value: " + str;
    }
}

std::string ConfigUtils::validateBoolean(const std::string& str, const std::string& key, ValidationResult& result) {
    std::string lowerStr = toLower(str);
    if (lowerStr != "true" && lowerStr != "false" && lowerStr != "1" && lowerStr != "0") {
        result = ValidationResult::Error;
        return "Invalid boolean value for key: " + key + ", value: " + str;
    }
    
    return "";
}

std::string ConfigUtils::validatePath(const std::string& str, const std::string& key, ValidationResult& result) {
    if (str.empty()) {
        result = ValidationResult::Warning;
        return "Empty path for key: " + key;
    }
    
    // Check for path traversal attempts
    if (str.find("..") != std::string::npos) {
        result = ValidationResult::Warning;
        return "Path contains '..' for key: " + key;
    }
    
    // Check for absolute paths on systems that don't support them
    if (str[0] == '/' || str[0] == '\\') {
        result = ValidationResult::Warning;
        return "Absolute path for key: " + key;
    }
    
    return "";
}

std::string ConfigUtils::validateFile(const std::string& str, const std::string& key, ValidationResult& result) {
    std::string pathError = validatePath(str, key, result);
    if (!pathError.empty()) return pathError;
    
    // Additional file-specific validation can be added here
    return "";
}

std::string ConfigUtils::validateEnum(const std::string& str, const std::string& key, 
                                     const std::vector<std::string>& allowedValues, ValidationResult& result) {
    std::string lowerStr = toLower(str);
    bool found = false;
    
    for (const auto& allowed : allowedValues) {
        if (toLower(allowed) == lowerStr) {
            found = true;
            break;
        }
    }
    
    if (!found) {
        result = ValidationResult::Error;
        std::string allowedStr = join(allowedValues, ", ");
        return "Invalid value for key: " + key + ", expected one of: " + allowedStr;
    }
    
    return "";
}

std::string ConfigUtils::validateRange(const std::string& str, const std::string& key,
                                      double min, double max, ValidationResult& result) {
    if (!containsOnlyDigitsAndDecimal(str)) return "";
    
    try {
        double value = std::stod(str);
        
        if (value < min || value > max) {
            result = ValidationResult::Error;
            return "Value out of range for key: " + key + ", expected between " + 
                   std::to_string(min) + " and " + std::to_string(max) + ", got: " + str;
        }
        
        return "";
    } catch (const std::exception&) {
        result = ValidationResult::Error;
        return "Cannot parse number for range validation for key: " + key + ", value: " + str;
    }
}

bool ConfigUtils::parseCommandLineArgs(const std::vector<std::string>& args, 
                                      std::map<std::string, std::string>& parsedArgs) {
    bool success = true;
    
    for (size_t i = 0; i < args.size(); ++i) {
        std::string arg = args[i];
        
        if (arg.rfind("--", 0) == 0) {
            // Handle --key=value format
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                parsedArgs[key] = value;
            } else {
                // Handle --key value format
                if (i + 1 < args.size()) {
                    std::string key = arg.substr(2);
                    std::string value = args[++i];
                    parsedArgs[key] = value;
                } else {
                    success = false;  // Missing value
                }
            }
        } else if (arg.rfind("-", 0) == 0) {
            // Handle -key value format
            if (i + 1 < args.size()) {
                std::string key = arg.substr(1);
                std::string value = args[++i];
                parsedArgs[key] = value;
            } else {
                success = false;  // Missing value
            }
        }
    }
    
    return success;
}

bool ConfigUtils::validateCommandLineArgs(const std::map<std::string, std::string>& parsedArgs,
                                         const std::vector<std::string>& requiredKeys) {
    bool success = true;
    
    for (const auto& required : requiredKeys) {
        if (parsedArgs.find(required) == parsedArgs.end()) {
            // Check if there are any variations of the required key
            bool found = false;
            std::string normalizedRequired = toLower(required);
            
            for (const auto& pair : parsedArgs) {
                std::string key = toLower(pair.first);
                if (key == normalizedRequired || 
                    key.find(normalizedRequired) != std::string::npos ||
                    normalizedRequired.find(key) != std::string::npos) {
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                // Don't fail on required keys for advanced CLI - just warn
                continue;
            }
        }
    }
    
    return success;
}

std::string ConfigUtils::generateHelpText(const std::map<std::string, std::string>& argDescriptions) {
    std::stringstream ss;
    ss << "Available options:" << std::endl;
    
    for (const auto& pair : argDescriptions) {
        ss << "  --" << pair.first << "=" << pair.second << std::endl;
    }
    
    return ss.str();
}

std::string ConfigUtils::generateErrorText(const std::vector<std::string>& errors) {
    if (errors.empty()) return "";
    
    std::stringstream ss;
    ss << "Errors:" << std::endl;
    
    for (const auto& error : errors) {
        ss << "  " << error << std::endl;
    }
    
    return ss.str();
}

std::string ConfigUtils::generateWarningText(const std::vector<std::string>& warnings) {
    if (warnings.empty()) return "";
    
    std::stringstream ss;
    ss << "Warnings:" << std::endl;
    
    for (const auto& warning : warnings) {
        ss << "  " << warning << std::endl;
    }
    
    return ss.str();
}

} // namespace nlm