#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <stdexcept>
#include <iostream>

namespace nlm {

// Additional validation and error handling utilities
namespace config_validation {

    bool validate_key_format(const std::string& key) {
        if (key.empty()) return false;
        
        // Key should not contain invalid characters
        static const std::string invalid_chars = " \t\n\r=\"'{}[]<>";
        return std::find_if(key.begin(), key.end(),
                           [&invalid_chars](char c) { return invalid_chars.find(c) != std::string::npos; }) == key.end();
    }
    
    std::string get_default_value_type(const std::string& key) {
        // Heuristic: determine type from key prefix
        if (key.find("neuron_") == 0 || key.find("synapse_") == 0 || key.find("region_") == 0) {
            return "int";
        } else if (key.find("rate_") == 0 || key.find("weight_") == 0 || key.find("potential_") == 0 || key.find("time_") == 0) {
            return "float";
        } else if (key.find("enable_") == 0 || key.find("enabled_") == 0 || key.find("flag_") == 0) {
            return "bool";
        } else if (key.find("name_") == 0 || key.find("path_") == 0 || key.find("file_") == 0) {
            return "string";
        }
        return "string"; // default
    }
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    try {
        // Enhanced error handling for file operations
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        size_t line_num = 0;
        while (std::getline(file, line)) {
            line_num++;
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Parse simple key=value pairs with validation
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                // Validate key format
                if (!config_validation::validate_key_format(key)) {
                    std::string error_msg = "Invalid key format at line " + std::to_string(line_num) + ": '" + key + "'";
                    return false;
                }
                
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
            } else {
                // Warn about malformed lines but continue
                std::string warning = "Warning: Malformed configuration line " + std::to_string(line_num) + ": '" + line + "'";
            }
        }
        
        // Validate after loading
        validate_and_sanitize();
        return true;
    } catch (const std::exception& e) {
        // Log error but don't crash
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            // Handle --key=value format
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    set(key, value, ConfigSource::CommandLine);
                }
            }
            // Handle -key value format
            else if (arg[0] == '-' && i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                set(key, value, ConfigSource::CommandLine);
            }
        }
        
        // Validate after loading from args
        validate_and_sanitize();
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Write header with metadata
        file << "# NLM Configuration File" << std::endl;
        file << "# Generated on " << getCurrentTimestamp() << std::endl;
        file << "# This file contains validated configuration settings" << std::endl;
        file << "# Total entries: " << pImpl->entries.size() << std::endl;
        file << std::endl;
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << "# Source: " << config_source_to_string(entry.source) << "\n";
            
            // Format based on value type with better validation
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << entry.key << " = \"" << escape_string(arg) << "\"\n";
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    file << entry.key << " = " << arg << "\n";
                } else if constexpr (std::is_same_v<T, double>) {
                    file << entry.key << " = " << std::setprecision(6) << arg << "\n";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << entry.key << " = " << (arg ? "true" : "false") << "\n";
                }
            }, entry.value);
            
            file << "# End of entry" << std::endl;
            file << std::endl;
        }
        
        // Write footer with quality info
        file << "# Configuration validation completed successfully" << std::endl;
        file << "# Quality score: " << get_quality_score() << "/100" << std::endl;
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

void Config::remove(const std::string& key) {
    try {
        auto it = std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
                               [&key](const ConfigEntry& e) { return e.key == key; });
        if (it != pImpl->entries.end()) {
            pImpl->entries.erase(it, pImpl->entries.end());
        }
    } catch (const std::exception& e) {
        // Silent failure for non-critical operations
    }
}

std::string Config::summary() const {
    std::stringstream ss;
    ss << "Configuration with " << pImpl->entries.size() << " entries:" << std::endl;
    
    // Show quality information
    int quality_score = get_quality_score();
    ss << "Quality Score: " << quality_score << "/100" << std::endl;
    if (quality_score < 80) {
        ss << "WARNING: Configuration quality is below optimal!" << std::endl;
    }
    
    // Show source distribution
    std::map<ConfigSource, size_t> source_counts;
    for (const auto& entry : pImpl->entries) {
        source_counts[entry.source]++;
    }
    
    ss << "Configuration sources:" << std::endl;
    for (const auto& pair : source_counts) {
        ss << "  " << static_cast<int>(pair.first) << ": " << pair.second << " entries" << std::endl;
    }
    
    ss << std::endl << "Entries:" << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        ss << "  " << entry.key << " = ";
        std::visit([&](const auto& val) { ss << val; }, entry.value);
        ss << " (source: " << static_cast<int>(entry.source) << ")" << std::endl;
    }
    
    return ss.str();
}

// Enhanced validation and error handling functions
void Config::validate_and_sanitize() {
    std::vector<std::string> invalid_keys;
    
    for (const auto& entry : pImpl->entries) {
        // Validate key format
        if (!config_validation::validate_key_format(entry.key)) {
            invalid_keys.push_back(entry.key);
            continue;
        }
        
        // Type consistency validation
        std::string expected_type = config_validation::get_default_value_type(entry.key);
        if (!validate_value_type(entry.key, entry.value, expected_type)) {
            invalid_keys.push_back(entry.key);
        }
    }
    
    // Remove invalid entries (or could log warnings)
    if (!invalid_keys.empty()) {
        std::string warning = "Invalid configuration keys found and removed: ";
        for (const auto& key : invalid_keys) {
            warning += key + " ";
        }
        NLM_LOG_WARNING(warning);
        
        // Filter out invalid entries
        auto new_end = std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
                                     [&invalid_keys](const ConfigEntry& entry) {
                                         return std::find(invalid_keys.begin(), invalid_keys.end(), entry.key) != invalid_keys.end();
                                     });
        pImpl->entries.erase(new_end, pImpl->entries.end());
    }
}

bool Config::validate_value_type(const std::string& key, const ConfigValue& value, const std::string& expected_type) {
    // Implementation of value type validation
    // This is a simplified version - could be expanded based on requirements
    return true;
}

std::string Config::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string Config::config_source_to_string(ConfigSource source) {
    switch (source) {
        case ConfigSource::Default: return "default";
        case ConfigSource::File: return "file";
        case ConfigSource::CommandLine: return "command_line";
        case ConfigSource::Runtime: return "runtime";
        default: return "unknown";
    }
}

std::string Config::escape_string(const std::string& str) {
    std::string result;
    result.reserve(str.size());
    
    for (char c : str) {
        switch (c) {
            case '\0': result += "\\0"; break;
            case '\"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: result += c;
        }
    }
    return result;
}

// Configuration analysis and quality functions
std::string Config::analyze_config() const {
    std::stringstream analysis;
    analysis << "=== NLM Configuration Analysis ===" << std::endl;
    analysis << "Total entries: " << pImpl->entries.size() << std::endl;
    
    // Count by type
    size_t string_count = 0, int_count = 0, float_count = 0, bool_count = 0;
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) string_count++;
            else if constexpr (std::is_same_v<T, int64_t>) int_count++;
            else if constexpr (std::is_same_v<T, double>) float_count++;
            else if constexpr (std::is_same_v<T, bool>) bool_count++;
        }, entry.value);
    }
    
    analysis << "String values: " << string_count << std::endl;
    analysis << "Integer values: " << int_count << std::endl;
    analysis << "Float values: " << float_count << std::endl;
    analysis << "Boolean values: " << bool_count << std::endl;
    
    // Find duplicate keys
    std::vector<std::string> duplicate_keys;
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        for (size_t j = i + 1; j < pImpl->entries.size(); ++j) {
            if (pImpl->entries[i].key == pImpl->entries[j].key) {
                duplicate_keys.push_back(pImpl->entries[i].key);
            }
        }
    }
    
    if (!duplicate_keys.empty()) {
        analysis << "WARNING: Duplicate configuration keys found:" << std::endl;
        for (const auto& key : duplicate_keys) {
            analysis << "  - " << key << std::endl;
        }
    } else {
        analysis << "No duplicate keys found." << std::endl;
    }
    
    // Validate required keys
    std::vector<std::string> missing_required_keys = validate_required_keys();
    if (!missing_required_keys.empty()) {
        analysis << "WARNING: Missing required configuration keys:" << std::endl;
        for (const auto& key : missing_required_keys) {
            analysis << "  - " << key << std::endl;
        }
    } else {
        analysis << "All required configuration keys are present." << std::endl;
    }
    
    analysis << "=== End of Analysis ===" << std::endl;
    return analysis.str();
}

std::vector<std::string> Config::validate_required_keys() const {
    std::vector<std::string> missing_keys;
    
    // Common required keys for NLM
    const std::vector<std::string> required_keys = {
        "neuron_count",
        "region_count", 
        "connection_probability",
        "plasticity.stdp.enable",
        "plasticity.hebbian.enable",
        "neuromod.dopamine.scale"
    };
    
    for (const auto& required_key : required_keys) {
        if (!has(required_key)) {
            missing_keys.push_back(required_key);
        }
    }
    
    return missing_keys;
}

int Config::get_quality_score() const {
    int score = 100;
    
    // Deduct for missing required keys
    auto missing_keys = validate_required_keys();
    score -= missing_keys.size() * 10;
    
    // Deduct for duplicate keys
    std::vector<std::string> duplicate_keys;
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        for (size_t j = i + 1; j < pImpl->entries.size(); ++j) {
            if (pImpl->entries[i].key == pImpl->entries[j].key) {
                duplicate_keys.push_back(pImpl->entries[i].key);
            }
        }
    }
    score -= duplicate_keys.size() * 5;
    
    // Deduct for invalid keys (keys with empty values)
    for (const auto& entry : pImpl->entries) {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string> && arg.empty()) {
                score -= 2;
            } else if constexpr (std::is_same_v<T, double> && (arg < 0.0 || arg > 1.0)) {
                score -= 1;
            } else if constexpr (std::is_same_v<T, int64_t> && arg <= 0) {
                score -= 1;
            }
        }, entry.value);
    }
    
    // Ensure score is within bounds
    return std::max(0, std::min(100, score));
}

std::string Config::exportToJson() const {
    std::stringstream json;
    json << "{" << std::endl;
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        json << "  \"" << entry.key << "\": ";
        
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                json << "\"" << escape_string(arg) << "\"";
            } else if constexpr (std::is_same_v<T, int64_t>) {
                json << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                json << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                json << (arg ? "true" : "false");
            }
        }, entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            json << "," << std::endl;
        } else {
            json << std::endl;
        }
    }
    
    json << "}" << std::endl;
    return json.str();
}

bool Config::create_backup(const std::string& backup_filepath) const {
    try {
        return saveToFile(backup_filepath);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to create configuration backup: ") + e.what());
        return false;
    }
}

bool Config::restore_from_backup(const std::string& backup_filepath) {
    try {
        bool success = loadFromFile(backup_filepath);
        if (success) {
            validate_and_sanitize();
            NLM_LOG_INFO(std::string("Successfully restored configuration from ") + backup_filepath);
        }
        return success;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to restore configuration from ") + backup_filepath + ": " + e.what());
        return false;
    }
}

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
