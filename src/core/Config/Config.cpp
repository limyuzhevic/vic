// Config.cpp - Enhanced configuration management for NLM Phase 6
#include "Config.hpp"
#include "JsonParser.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cmath>
#include <limits>
#include <iostream>

namespace nlm {

const std::string Config::JSON_EXT = ".json";
const std::string Config::YAML_EXT = ".yaml";
const std::string Config::CFG_EXT = ".cfg";

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::shared_ptr<ConfigSchema> schema;
    bool validationEnabled;
    size_t maxValueLength;
    double numericEpsilon;
    
    Impl() : schema(std::make_shared<ConfigSchema>()), validationEnabled(true), 
             maxValueLength(1000), numericEpsilon(1e-9) {
        // Set up default schema
        setupDefaultSchema();
    }
    
    void setupDefaultSchema() {
        schema->name = "NLM Configuration";
        schema->version = "2.0";
        schema->description = "Neural Learning Machine configuration schema (Phase 6)";
        
        // Core simulation parameters
        schema->addStringRule("simulation_timestep", 1, 20, true);
        schema->addNumericRule("neuron_count", 1, 1000000, true);
        schema->addNumericRule("region_count", 1, 1000, true);
        schema->addNumericRule("connection_probability", 0.0, 1.0, true);
        
        // Plasticity settings
        schema->addNumericRule("plasticity_learning_rate", 0.0, 10.0);
        schema->addNumericRule("stdp_ltp_weight", 0.0, 1.0);
        schema->addNumericRule("stdp_ltd_weight", 0.0, 1.0);
        schema->addNumericRule("stdp_time_constant", 1.0, 1000.0);
        
        // Development settings
        schema->addNumericRule("synaptogenesis_rate", 0.0, 1.0);
        schema->addNumericRule("pruning_rate", 0.0, 1.0);
        
        // Neuromodulation settings
        schema->addNumericRule("dopamine_baseline", -10.0, 10.0);
        schema->addNumericRule("reward_discount_factor", 0.0, 1.0);
        
        // Environment settings
        schema->addNumericRule("environment_width", 1, 10000);
        schema->addNumericRule("environment_height", 1, 10000);
        
        // Logging settings
        schema->addStringRule("log_level", 1, 50);
        schema->addStringRule("log_filename", 1, 500);
        
        // Simulation settings
        schema->addNumericRule("max_simulation_steps", 1, 1000000);
        schema->addNumericRule("simulation_time_limit", 0.0, 1000000.0);
        
        // Visualization settings
        schema->addNumericRule("visualization_update_rate", 1.0, 1000.0);
    }
    
    ValidationResult validateEntry(const ConfigEntry& entry) const {
        ValidationResult result;
        
        if (!validationEnabled) return result;
        
        const auto& rule = getValidationRule(entry.key);
        if (!rule) return result; // No validation rule defined, skip
        
        // Check if value type matches expected type
        bool typeValid = false;
        std::string expectedType = "unknown";
        
        if (std::holds_alternative<int>(entry.value)) {
            expectedType = "int";
            typeValid = true;
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            expectedType = "int64_t";
            typeValid = true;
        } else if (std::holds_alternative<double>(entry.value)) {
            expectedType = "double";
            typeValid = true;
        } else if (std::holds_alternative<bool>(entry.value)) {
            expectedType = "bool";
            typeValid = true;
        } else if (std::holds_alternative<std::string>(entry.value)) {
            expectedType = "string";
            if (entry.value.std::string::size() <= maxValueLength) {
                typeValid = true;
            } else {
                result.addError("String value too long for key '" + entry.key + "' (max " + 
                              std::to_string(maxValueLength) + " chars)");
            }
        }
        
        // Apply validation rule
        if (rule->validator) {
            if (!rule->validator(entry.value)) {
                result.addError("Validation failed for key '" + entry.key + "': " + rule->errorMessage);
            }
        }
        
        return result;
    }
    
    std::shared_ptr<ValidationRule> getValidationRule(const std::string& key) const {
        for (const auto& rule : schema->rules) {
            if (rule.key == key) {
                return std::make_shared<ValidationRule>(rule);
            }
        }
        return nullptr;
    }
    
    std::string formatValidationResult(const ValidationResult& result) const {
        if (result.errors.empty() && result.warnings.empty()) {
            return "Configuration is valid.";
        }
        
        std::string output = "Configuration validation:";
        
        if (!result.warnings.empty()) {
            output += "\nWarnings:";
            for (const auto& warning : result.warnings) {
                output += "\n  - " + warning;
            }
        }
        
        if (!result.errors.empty()) {
            output += "\nErrors:";
            for (const auto& error : result.errors) {
                output += "\n  - " + error;
            }
        }
        
        output += "\nIs valid: " + std::string(result.isValid ? "yes" : "no");
        return output;
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;
Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Determine file type by extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    json j;
    bool success = false;
    
    if (ext == JSON_EXT) {
        success = JsonParser::parseFile(filepath, j);
    } else if (ext == YAML_EXT) {
        // TODO: Implement YAML parser if needed
        NLM_LOG_ERROR("YAML configuration files not yet supported. Use .json extension.");
        return false;
    } else {
        // Try to parse as legacy format (simple key=value)
        success = loadFromLegacyFile(filepath);
        if (success) {
            NLM_LOG_INFO("Loaded legacy configuration from: " + filepath);
        }
    }
    
    if (success) {
        // Convert JSON to Config entries
        clear(); // Clear existing config
        fromJson(j);
        
        // Validate configuration
        ValidationResult validation = pImpl->validateAll();
        if (!validation.isValid) {
            NLM_LOG_ERROR(pImpl->formatValidationResult(validation));
            return false;
        }
        
        NLM_LOG_INFO("Loaded configuration from: " + filepath);
        return true;
    }
    
    return false;
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool success = loadFromLegacyArgs(argc, argv);
    if (success) {
        NLM_LOG_INFO("Loaded configuration from command line arguments");
    }
    return success;
}

bool Config::loadFromLegacyFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    bool success = true;
    
    while (std::getline(file, line)) {
        ++lineNum;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        } else {
            NLM_LOG_WARNING("Invalid line " + std::to_string(lineNum) + " in config file: " + line);
        }
    }
    
    return success;
}

bool Config::loadFromLegacyArgs(int argc, char** argv) {
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
    return true;
}

ValidationResult Config::validateAll() const {
    ValidationResult result;
    
    for (const auto& entry : pImpl->entries) {
        ValidationResult entryResult = pImpl->validateEntry(entry);
        
        if (!entryResult.isValid) {
            result.isValid = false;
            for (const auto& error : entryResult.errors) {
                result.addError("Key '" + entry.key + "': " + error);
            }
        }
        
        for (const auto& warning : entryResult.warnings) {
            result.addWarning("Key '" + entry.key + "': " + warning);
        }
    }
    
    return result;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate before setting if validation is enabled
    if (pImpl->validationEnabled && pImpl->schema) {
        ConfigEntry tempEntry(key, value, source);
        ValidationResult validation = pImpl->validateEntry(tempEntry);
        if (!validation.isValid) {
            throw std::invalid_argument(pImpl->formatValidationResult(validation));
        }
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        it->timestamp = std::chrono::system_clock::now();
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
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

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(),
                      pImpl->entries.end(),
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
    oss << "Configuration (Phase 6 - Enhanced) - " << pImpl->entries.size() << " entries:\n";
    
    if (pImpl->schema) {
        oss << "Schema: " << pImpl->schema->name << " v" << pImpl->schema->version << "\n";
        oss << "Description: " << pImpl->schema->description << "\n";
    }
    
    oss << "Validation: " << (pImpl->validationEnabled ? "Enabled" : "Disabled") << "\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ") " << entry.getFormattedTimestamp() << "\n";
    }
    
    // Add validation status
    ValidationResult validation = validateAll();
    if (!validation.isValid) {
        oss << "\nVALIDATION FAILED:\n";
        for (const auto& error : validation.errors) {
            oss << "  - " << error << "\n";
        }
    } else if (!validation.warnings.empty()) {
        oss << "\nValidation warnings:\n";
        for (const auto& warning : validation.warnings) {
            oss << "  - " << warning << "\n";
        }
    }
    
    return oss.str();
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

void Config::fromJson(const json& j) {
    clear();
    
    if (!j.is_object()) {
        throw std::invalid_argument("JSON root must be an object");
    }
    
    for (auto it = j.begin(); it != j.end(); ++it) {
        std::string key = it.key();
        const json& value = *it;
        
        ConfigSource source = ConfigSource::File;
        if (it.key() == "random_seed" || it.key() == "simulation_timestep" || 
            it.key() == "neuron_count" || it.key() == "region_count") {
            source = ConfigSource::Default;
        }
        
        setFromJsonValue(key, value, source);
    }
}

void Config::setFromJsonValue(const std::string& key, const json& j, ConfigSource source) {
    if (j.is_number_integer()) {
        set(key, j.get<int64_t>(), source);
    } else if (j.is_number_float()) {
        set(key, j.get<double>(), source);
    } else if (j.is_boolean()) {
        set(key, j.get<bool>(), source);
    } else if (j.is_string()) {
        set(key, j.get<std::string>(), source);
    } else if (j.is_array()) {
        // Handle array types
        if (!j.empty()) {
            if (std::all_of(j.begin(), j.end(), [](const json& v) { return v.is_number_integer(); })) {
                std::vector<int64_t> intArray;
                for (const auto& v : j) {
                    intArray.push_back(v.get<int64_t>());
                }
                set(key, intArray, source);
            } else if (std::all_of(j.begin(), j.end(), [](const json& v) { return v.is_number_float(); })) {
                std::vector<double> doubleArray;
                for (const auto& v : j) {
                    doubleArray.push_back(v.get<double>());
                }
                set(key, doubleArray, source);
            } else if (std::all_of(j.begin(), j.end(), [](const json& v) { return v.is_string(); })) {
                std::vector<std::string> stringArray;
                for (const auto& v : j) {
                    stringArray.push_back(v.get<std::string>());
                }
                set(key, stringArray, source);
            } else {
                // Mixed array - store as string representation
                set(key, j.dump(), source);
            }
        } else {
            set(key, std::vector<double>(), source); // Empty array
        }
    } else {
        // For complex objects or null, store as JSON string
        set(key, j.dump(), source);
    }
}

// Explicit template instantiations
#include "Config.cpp.templates"

} // namespace nlm
