// Enhanced Config implementation with validation, templates, and environment support

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <iostream>
#include <unordered_map>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, std::string> envVariables;
};

Config::Config() : pImpl(std::make_unique<Impl>()), envPrefix("NLM_") {
    // Initialize with default configuration
    initializeDefaultConfig();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::initializeDefaultConfig() {
    // Set default values for essential configuration options
    set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    set("simulation_timestep", 0.001, ConfigSource::Default);
    set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    set("connection_probability", 0.1, ConfigSource::Default);
    set("plasticity_learning_rate", 0.01, ConfigSource::Default);
    set("stdp_ltp_weight", 0.01, ConfigSource::Default);
    set("stdp_ltd_weight", 0.012, ConfigSource::Default);
    set("stdp_time_constant", 20.0, ConfigSource::Default);
    set("development_synaptogenesis_rate", 0.001, ConfigSource::Default);
    set("development_pruning_rate", 0.0001, ConfigSource::Default);
    set("reward_discount_factor", 0.99, ConfigSource::Default);
    set("environment_name", "GridWorld", ConfigSource::Default);
    set("environment_width", 10, ConfigSource::Default);
    set("environment_height", 10, ConfigSource::Default);
    set("log_level", "INFO", ConfigSource::Default);
    set("log_to_file", false, ConfigSource::Default);
    set("log_filename", "nlm.log", ConfigSource::Default);
    set("max_simulation_steps", 10000, ConfigSource::Default);
    set("simulation_time_limit", 0.0, ConfigSource::Default);
    set("visualization_enabled", false, ConfigSource::Default);
    set("visualization_update_rate", 30.0, ConfigSource::Default);
}

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        lastError = "Failed to open configuration file: " + filepath;
        return false;
    }
    
    // Try to detect file format
    std::string line;
    bool isJSON = false;
    bool isYAML = false;
    
    // Read first few lines to detect format
    for (int i = 0; i < 5 && std::getline(file, line); ++i) {
        std::string trimmed = trim(line);
        if (trimmed.empty() || trimmed[0] == '#') continue;
        
        if (trimmed.find("{") != std::string::npos || 
            (trimmed.find("{") == 0 && trimmed.size() > 1)) {
            isJSON = true;
            break;
        }
        
        if (trimmed.find("-") == 0 || 
            (trimmed.find(" ") != std::string::npos && 
             (trimmed.find(":") != std::string::npos || 
              (trimmed.find(" - ") != std::string::npos && 
               (trimmed.find("key:") != std::string::npos ||
                trimmed.find("value:") != std::string::npos)))))
        {
            isYAML = true;
        }
    }
    
    file.clear();
    file.seekg(0);
    
    if (isJSON) {
        return loadFromJSON(filepath);
    } else if (isYAML) {
        return loadFromYAML(filepath);
    } else {
        // Fallback to simple key=value format
        return loadFromSimpleFile(filepath);
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    // Placeholder implementation for YAML loading
    std::cout << "YAML loading (placeholder) from: " << filepath << std::endl;
    
    // In a real implementation, this would use a YAML parser
    // For now, fallback to simple file format
    return loadFromSimpleFile(filepath);
}

bool Config::loadFromJSON(const std::string& filepath) {
    // Placeholder implementation for JSON loading
    std::cout << "JSON loading (placeholder) from: " << filepath << std::endl;
    
    // In a real implementation, this would use a JSON parser
    // For now, fallback to simple file format
    return loadFromSimpleFile(filepath);
}

bool Config::loadFromSimpleFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        lastError = "Failed to open configuration file: " + filepath;
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    bool success = true;
    
    while (std::getline(file, line)) {
        ++lineNumber;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            lastError = "Invalid configuration line " + std::to_string(lineNumber) + ": missing '='";
            success = false;
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        try {
            set(key, value, ConfigSource::File);
        } catch (const std::exception& e) {
            lastError = "Error parsing configuration line " + std::to_string(lineNumber) + ": " + e.what();
            success = false;
        }
    }
    
    if (success) {
        std::cout << "Loaded configuration from: " << filepath << " (" << lineNumber << " lines)" << std::endl;
    }
    
    return success;
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                try {
                    set(key, value, ConfigSource::CommandLine);
                } catch (const std::exception& e) {
                    lastError = "Error parsing command line argument '--" + key + "': " + e.what();
                    success = false;
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            try {
                set(key, value, ConfigSource::CommandLine);
            } catch (const std::exception& e) {
                lastError = "Error parsing command line argument '-" + key + "': " + e.what();
                success = false;
            }
        }
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        lastError = "Failed to open configuration file for writing: " + filepath;
        return false;
    }
    
    file << "# NLM Configuration File (JSON format)" << std::endl;
    file << "# Generated by NLM CLI" << std::endl;
    file << std::endl;
    
    // Output as JSON (placeholder implementation)
    file << "{" << std::endl;
    
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) {
            file << "," << std::endl;
        }
        first = false;
        
        file << "  \"" << entry.key << "\": " << valueToJSON(entry.value);
    }
    
    file << std::endl << "}" << std::endl;
    
    return true;
}

bool Config::saveToYAML(const std::string& filepath) const {
    // Placeholder implementation for YAML saving
    std::cout << "YAML saving (placeholder) to: " << filepath << std::endl;
    
    // In a real implementation, this would output YAML format
    // For now, fallback to JSON format
    return saveToFile(filepath + ".json");
}

std::string Config::valueToJSON(const ConfigValue& value) const {
    std::string result;
    
    std::visit([&result](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            result = "\"" + arg + "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            result = arg ? "true" : "false";
        } else if constexpr (std::is_same_v<T, double> || 
                            std::is_same_v<T, int64_t> ||
                            std::is_same_v<T, int>) {
            result = std::to_string(arg);
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(arg[i]);
            }
            result += "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(arg[i]);
            }
            result += "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result = "\"" + arg[i] + "\"";
            }
            result += "]";
        } else {
            result = "null";
        }
    }, value);
    
    return result;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        // Validate new value if source is CommandLine or Runtime
        if (source == ConfigSource::CommandLine || source == ConfigSource::Runtime) {
            validateKeyValue(key, value);
        }
        
        it->value = value;
        it->source = source;
    } else {
        // Validate new value
        validateKeyValue(key, value);
        
        pImpl->entries.emplace_back(key, value, source);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    try {
        // Try to infer type from string
        if (value == "true" || value == "false") {
            set(key, stringToBool(value), source);
        } else {
            // Try to parse as integer
            try {
                size_t pos;
                int64_t intValue = std::stoll(value, &pos);
                if (pos == value.size()) {
                    set(key, intValue, source);
                    return;
                }
            } catch (...) {}
            
            // Try to parse as double
            try {
                size_t pos;
                double doubleValue = std::stod(value, &pos);
                if (pos == value.size()) {
                    set(key, doubleValue, source);
                    return;
                }
            } catch (...) {}
            
            // Fall back to string
            set(key, value, source);
        }
    } catch (const std::exception& e) {
        lastError = "Error setting configuration key '" + key + "': " + e.what();
        throw;
    }
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::validateKeyValue(const std::string& key, const ConfigValue& value) const {
    // Validate key name
    if (key.empty()) {
        throw std::invalid_argument("Configuration key cannot be empty");
    }
    
    // Check for invalid characters in key
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '-' && c != '.') {
            throw std::invalid_argument("Invalid character in configuration key: '" + 
                                       std::string(1, c) + "'. Keys must contain only alphanumeric characters, underscores, hyphens, or dots.");
        }
    }
    
    // Validate value based on key
    if (key == "neuron_count") {
        auto intValue = std::get_if<int64_t>(&value);
        if (intValue && (*intValue < 10 || *intValue > 1000000)) {
            throw std::invalid_argument("neuron_count must be between 10 and 1,000,000");
        }
    } else if (key == "region_count") {
        auto intValue = std::get_if<int64_t>(&value);
        if (intValue && *intValue < 1) {
            throw std::invalid_argument("region_count must be at least 1");
        }
    } else if (key == "simulation_timestep") {
        auto doubleValue = std::get_if<double>(&value);
        if (doubleValue && (*doubleValue <= 0.0 || *doubleValue > 1.0)) {
            throw std::invalid_argument("simulation_timestep must be between 0 and 1.0");
        }
    } else if (key == "connection_probability") {
        auto doubleValue = std::get_if<double>(&value);
        if (doubleValue && (*doubleValue < 0.0 || *doubleValue > 1.0)) {
            throw std::invalid_argument("connection_probability must be between 0.0 and 1.0");
        }
    } else if (key == "plasticity_learning_rate") {
        auto doubleValue = std::get_if<double>(&value);
        if (doubleValue && *doubleValue < 0.0) {
            throw std::invalid_argument("plasticity_learning_rate must be non-negative");
        }
    }
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):" << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        oss << valueToString(entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ") " << entry.description << std::endl;
    }
    
    return oss.str();
}

bool Config::validate(const ConfigTemplate& template) const {
    bool valid = true;
    std::string errors;
    
    for (const auto& rule : template.rules) {
        if (!rule.required) continue;
        
        if (!has(rule.key)) {
            valid = false;
            errors += "Missing required configuration: " + rule.key + " (" + rule.description + ")\n";
        } else {
            // Get the value and validate it
            // This would require accessing the value directly, simplified for now
            auto value = get<std::string>(rule.key);
            if (value) {
                // Check if the value is of the expected type
                if (rule.type == "int" || rule.type == "integer") {
                    auto intVal = get<int64_t>(rule.key);
                    if (!intVal) {
                        valid = false;
                        errors += "Configuration " + rule.key + " must be an integer\n";
                    }
                } else if (rule.type == "float" || rule.type == "double") {
                    auto doubleVal = get<double>(rule.key);
                    if (!doubleVal) {
                        valid = false;
                        errors += "Configuration " + rule.key + " must be a number\n";
                    }
                } else if (rule.type == "bool" || rule.type == "boolean") {
                    auto boolVal = get<bool>(rule.key);
                    if (!boolVal) {
                        valid = false;
                        errors += "Configuration " + rule.key + " must be a boolean\n";
                    }
                }
                // Additional validation can be added here
            }
        }
    }
    
    if (!valid && !errors.empty()) {
        lastError = errors;
    }
    
    return valid;
}

std::string Config::validateAll(const std::vector<ConfigTemplate>& templates) const {
    std::string allErrors;
    
    for (const auto& template : templates) {
        if (!validate(template)) {
            allErrors += "Template " + template.name + ":\n" + lastError + "\n\n";
        }
    }
    
    if (allErrors.empty()) {
        return "All configuration templates validated successfully.";
    }
    
    return allErrors;
}

void Config::loadFromEnvironment(const std::map<std::string, std::string>& envVars) {
    std::cout << "Loading configuration from environment variables (prefix: " << envPrefix << ")" << std::endl;
    
    for (const auto& pair : envVars) {
        std::string envName = pair.first;
        
        // Check if this environment variable should be loaded
        if (envPrefix.empty() || envName.find(envPrefix) == 0) {
            std::string key = envVarToKey(envName, envPrefix);
            try {
                set(key, pair.second, ConfigSource::File);
                pImpl->envVariables[envName] = pair.second;
            } catch (const std::exception& e) {
                lastError = "Error loading environment variable " + envName + ": " + e.what();
            }
        }
    }
}

void Config::setEnvironmentPrefix(const std::string& prefix) {
    envPrefix = prefix;
}

void Config::addTemplate(const ConfigTemplate& template) {
    templates[template.name] = template;
}

bool Config::applyTemplate(const std::string& templateName) {
    auto it = templates.find(templateName);
    if (it == templates.end()) {
        lastError = "Configuration template not found: " + templateName;
        return false;
    }
    
    const ConfigTemplate& template = it->second;
    
    // Clear current configuration (but preserve runtime values?)
    // For now, clear all
    clear();
    
    // Apply template defaults
    for (const auto& pair : template.defaults) {
        set(pair.first, pair.second, ConfigSource::Default);
    }
    
    return true;
}

std::vector<std::string> Config::getAvailableTemplates() const {
    std::vector<std::string> templateNames;
    templateNames.reserve(templates.size());
    for (const auto& pair : templates) {
        templateNames.push_back(pair.first);
    }
    return templateNames;
}

std::string Config::getLastError() const {
    return lastError;
}

bool Config::hasError() const {
    return !lastError.empty();
}

void Config::clearError() {
    lastError.clear();
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

bool Config::stringToBool(const std::string& str) {
    std::string lower = toLower(str);
    if (lower == "true" || lower == "1" || lower == "yes" || lower == "on") {
        return true;
    } else if (lower == "false" || lower == "0" || lower == "no" || lower == "off") {
        return false;
    } else {
        throw std::invalid_argument("Invalid boolean value: " + str);
    }
}

std::string Config::boolToString(bool value) {
    return value ? "true" : "false";
}

std::string Config::doubleToString(double value) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6) << value;
    return oss.str();
}

std::string Config::int64ToString(int64_t value) {
    return std::to_string(value);
}

std::string Config::valueToString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, double> || 
                            std::is_same_v<T, int64_t> ||
                            std::is_same_v<T, int>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << arg[i] << "\"";
            }
            oss << "]";
        } else {
            oss << "[unknown]";
        }
    }, value);
    return oss.str();
}

std::string Config::getEnvVar(const std::string& name) {
    const char* value = std::getenv(name.c_str());
    return value ? value : "";
}

std::string Config::envVarToKey(const std::string& envName, const std::string& prefix) {
    std::string key = envName;
    
    // Remove prefix
    if (!prefix.empty() && key.find(prefix) == 0) {
        key = key.substr(prefix.size());
    }
    
    // Convert to lowercase
    for (char& c : key) {
        c = std::tolower(c);
    }
    
    // Replace underscores with dots for namespace
    std::replace(key.begin(), key.end(), '_', '.');
    
    return key;
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
