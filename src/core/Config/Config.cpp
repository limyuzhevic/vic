#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

struct ValidationRule {
        std::string keyPattern;  // Regex or exact match
        std::string valueType;   // int, double, string, bool
        std::string description;
        bool required;
        std::string defaultValue;
        
        // Validation functions
        std::function<bool(const ConfigValue&)> validator;
    };
    
    std::vector<ValidationRule> validationRules;
    
    // Configuration schemas for different subsystems
    struct ConfigSchema {
        std::string name;
        std::vector<std::string> requiredKeys;
        std::map<std::string, std::string> keyDescriptions;
        bool validateTypes;
        bool validateRanges;
        
        ConfigSchema(const std::string& n, bool validateTypes = true, bool validateRanges = false)
            : name(n), validateTypes(validateTypes), validateRanges(validateRanges) {}
    };
    
    std::vector<ConfigSchema> schemas;
    
    // Configuration validation statistics
    struct ValidationStats {
        size_t totalValidations = 0;
        size_t passedValidations = 0;
        size_t failedValidations = 0;
        std::vector<std::string> errorMessages;
    };
    
    ValidationStats stats;
    
    // Validation helper functions
    static bool validateInteger(const ConfigValue& value, int min = INT_MIN, int max = INT_MAX) {
        if (!std::holds_alternative<int>(value) && !std::holds_alternative<int64_t>(value)) {
            return false;
        }
        int val = std::holds_alternative<int>(value) ? 
                  std::get<int>(value) : std::get<int64_t>(value);
        return val >= min && val <= max;
    }
    
    static bool validateDouble(const ConfigValue& value, double min = -std::numeric_limits<double>::max(), 
                               double max = std::numeric_limits<double>::max()) {
        if (!std::holds_alternative<double>(value)) {
            return false;
        }
        double val = std::get<double>(value);
        return val >= min && val <= max;
    }
    
    static bool validateString(const ConfigValue& value, size_t minLength = 0, size_t maxLength = SIZE_MAX) {
        if (!std::holds_alternative<std::string>(value)) {
            return false;
        }
        std::string val = std::get<std::string>(value);
        return val.length() >= minLength && val.length() <= maxLength;
    }
    
    static bool validateBoolean(const ConfigValue& value) {
        return std::holds_alternative<bool>(value);
    }
    
    static bool validateVectorSize(const ConfigValue& value, size_t minSize = 0, size_t maxSize = SIZE_MAX) {
        if (std::holds_alternative<std::vector<int>>(value)) {
            std::vector<int> val = std::get<std::vector<int>>(value);
            return val.size() >= minSize && val.size() <= maxSize;
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            std::vector<double> val = std::get<std::vector<double>>(value);
            return val.size() >= minSize && val.size() <= maxSize;
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            std::vector<std::string> val = std::get<std::vector<std::string>>(value);
            return val.size() >= minSize && val.size() <= maxSize;
        }
        return false;
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize default validation rules
    initializeDefaultValidationRules();
    
    // Initialize default schemas
    initializeDefaultSchemas();
}

Config::~Config() = default;

void Config::initializeDefaultValidationRules() {
    // Neuron count validation
    ValidationRule neuronRule;
    neuronRule.keyPattern = "neuron_count";
    neuronRule.valueType = "int";
    neuronRule.description = "Number of neurons in the brain";
    neuronRule.required = true;
    neuronRule.validator = [](const ConfigValue& v) { return validateInteger(v, 1, 100000); };
    pImpl->validationRules.push_back(neuronRule);
    
    // Region count validation
    ValidationRule regionRule;
    regionRule.keyPattern = "region_count";
    regionRule.valueType = "int";
    regionRule.description = "Number of neural regions";
    regionRule.required = true;
    regionRule.validator = [](const ConfigValue& v) { return validateInteger(v, 1, 1000); };
    pImpl->validationRules.push_back(regionRule);
    
    // Connection probability validation
    ValidationRule connRule;
    connRule.keyPattern = "connection_probability";
    connRule.valueType = "double";
    connRule.description = "Probability of synaptic connections (0.0-1.0)";
    connRule.required = false;
    connRule.validator = [](const ConfigValue& v) {
        if (!std::holds_alternative<double>(v)) return false;
        double val = std::get<double>(v);
        return val >= 0.0 && val <= 1.0;
    };
    pImpl->validationRules.push_back(connRule);
    
    // Random seed validation
    ValidationRule seedRule;
    seedRule.keyPattern = "random_seed";
    seedRule.valueType = "int64_t";
    seedRule.description = "Random seed for reproducibility";
    seedRule.required = false;
    seedRule.validator = [](const ConfigValue& v) { return validateInteger(v, 0, UINT64_MAX); };
    pImpl->validationRules.push_back(seedRule);
}

void Config::initializeDefaultSchemas() {
    // Brain configuration schema
    ConfigSchema brainSchema("brain", true, true);
    brainSchema.requiredKeys = {"neuron_count", "region_count", "connection_probability"};
    brainSchema.keyDescriptions = {
        {"neuron_count", "Number of neurons in the brain (1-100000)"},
        {"region_count", "Number of neural regions (1-1000)"},
        {"connection_probability", "Probability of synaptic connections (0.0-1.0)"},
        {"random_seed", "Random seed for reproducibility"},
        {"simulation_timestep", "Simulation timestep in seconds (0.001-1.0)"},
        {"stdp_ltp_weight", "STDP long-term potentiation weight (0.0-1.0)"},
        {"stdp_ltd_weight", "STDP long-term depression weight (0.0-1.0)"},
        {"synaptogenesis_rate", "Rate of new synapse formation (0.0-0.1)"},
        {"pruning_rate", "Rate of synapse pruning (0.0-0.001)"}
    };
    pImpl->schemas.push_back(brainSchema);
}

bool Config::validate(const std::string& key, const ConfigValue& value) const {
    ++pImpl->stats.totalValidations;
    
    // Find matching validation rules
    bool anyRuleMatched = false;
    for (const auto& rule : pImpl->validationRules) {
        if (keyPatternMatches(rule.keyPattern, key)) {
            anyRuleMatched = true;
            if (rule.validator) {
                bool valid = rule.validator(value);
                if (valid) {
                    ++pImpl->stats.passedValidations;
                    return true;
                } else {
                    ++pImpl->stats.failedValidations;
                    pImpl->stats.errorMessages.push_back(
                        "Validation failed for key '" + key + "': " + rule.description
                    );
                    return false;
                }
            }
        }
    }
    
    // If no validation rule matched, assume valid
    if (!anyRuleMatched) {
        ++pImpl->stats.passedValidations;
        return true;
    }
    
    return false;
}

bool Config::validateAll() const {
    bool allValid = true;
    for (const auto& entry : pImpl->entries) {
        if (!validate(entry.key, entry.value)) {
            allValid = false;
        }
    }
    return allValid;
}

std::string Config::getValidationErrors() const {
    return pImpl->stats.errorMessages.empty() ? "" : 
           "Configuration validation errors:\n" + 
           joinStrings(pImpl->stats.errorMessages, "\n");
}

ValidationStats Config::getValidationStats() const {
    return pImpl->stats;
}

void Config::clearValidationStats() {
    pImpl->stats = ValidationStats();
}

bool Config::validateKey(const std::string& key) const {
    // Check if key exists
    if (!has(key)) {
        pImpl->stats.errorMessages.push_back("Key not found: '" + key + "'");
        return false;
    }
    
    // Get key value
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    if (it == pImpl->entries.end()) {
        return false;
    }
    
    return validate(key, it->value);
}

bool Config::isValid(const std::string& key) const {
    return has(key) && validateKey(key);
}

std::vector<std::string> Config::getInvalidKeys() const {
    std::vector<std::string> invalidKeys;
    for (const auto& entry : pImpl->entries) {
        if (!validate(entry.key, entry.value)) {
            invalidKeys.push_back(entry.key);
        }
    }
    return invalidKeys;
}

std::vector<std::string> Config::getValidKeys() const {
    std::vector<std::string> validKeys;
    for (const auto& entry : pImpl->entries) {
        if (validate(entry.key, entry.value)) {
            validKeys.push_back(entry.key);
        }
    }
    return validKeys;
}

bool Config::validateSchema(const std::string& schemaName) const {
    // Find schema
    for (const auto& schema : pImpl->schemas) {
        if (schema.name == schemaName) {
            return validateSchema(schema);
        }
    }
    return false;
}

bool Config::validateSchema(const ConfigSchema& schema) const {
    bool allValid = true;
    
    // Check required keys
    for (const auto& requiredKey : schema.requiredKeys) {
        if (!has(requiredKey)) {
            pImpl->stats.errorMessages.push_back(
                "Schema validation failed: required key missing: '" + requiredKey + "'"
            );
            allValid = false;
        } else {
            // Validate the key's value
            if (!validateKey(requiredKey)) {
                allValid = false;
            }
        }
    }
    
    // Validate all keys in schema
    for (const auto& entry : pImpl->entries) {
        bool foundInSchema = false;
        for (const auto& schemaKey : schema.keyDescriptions) {
            if (keyPatternMatches(schemaKey.first, entry.key)) {
                foundInSchema = true;
                break;
            }
        }
        
        if (foundInSchema && schema.validateTypes) {
            // Additional type validation
            if (!validateTypeConsistency(entry.key, entry.value)) {
                allValid = false;
            }
        }
    }
    
    return allValid;
}

bool Config::validateTypeConsistency(const std::string& key, const ConfigValue& value) const {
    // Check if value type matches expected type based on key name
    std::string keyLower = toLower(key);
    
    if (keyLower.find("count") != std::string::npos || 
        keyLower.find("size") != std::string::npos) {
        return std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value);
    }
    
    if (keyLower.find("probability") != std::string::npos ||
        keyLower.find("rate") != std::string::npos ||
        keyLower.find("ratio") != std::string::npos ||
        keyLower.find("weight") != std::string::npos) {
        return std::holds_alternative<double>(value);
    }
    
    if (keyLower.find("enabled") != std::string::npos ||
        keyLower.find("active") != std::string::npos ||
        keyLower.find("flag") != std::string::npos) {
        return std::holds_alternative<bool>(value);
    }
    
    if (keyLower.find("name") != std::string::npos ||
        keyLower.find("path") != std::string::npos ||
        keyLower.find("file") != std::string::npos) {
        return std::holds_alternative<std::string>(value);
    }
    
    return true; // No specific type validation for other keys
}

bool Config::keyPatternMatches(const std::string& pattern, const std::string& key) const {
    if (pattern == key) {
        return true; // Exact match
    }
    
    // Simple wildcard matching
    if (pattern.find('*') != std::string::npos) {
        // Convert to regex-like matching
        std::string regexPattern = "^";
        for (char c : pattern) {
            if (c == '*') {
                regexPattern += ".*";
            } else if (c == '?') {
                regexPattern += ".";
            } else if (c == '.') {
                regexPattern += "\\.";
            } else {
                regexPattern += c;
            }
        }
        regexPattern += "$";
        
        std::regex regex(regexPattern);
        return std::regex_match(key, regex);
    }
    
    return false;
}

std::string Config::joinStrings(const std::vector<std::string>& strings, const std::string& separator) const {
    std::ostringstream oss;
    for (size_t i = 0; i < strings.size(); ++i) {
        if (i > 0) oss << separator;
        oss << strings[i];
    }
    return oss.str();
}

void Config::addValidationRule(const ValidationRule& rule) {
    pImpl->validationRules.push_back(rule);
}

void Config::removeValidationRule(const std::string& keyPattern) {
    pImpl->validationRules.erase(
        std::remove_if(pImpl->validationRules.begin(), pImpl->validationRules.end(),
            [&keyPattern](const ValidationRule& rule) {
                return keyPatternMatches(rule.keyPattern, keyPattern);
            }),
        pImpl->validationRules.end()
    );
}

void Config::addSchema(const ConfigSchema& schema) {
    pImpl->schemas.push_back(schema);
}

void Config::removeSchema(const std::string& schemaName) {
    pImpl->schemas.erase(
        std::remove_if(pImpl->schemas.begin(), pImpl->schemas.end(),
            [&schemaName](const ConfigSchema& schema) {
                return schema.name == schemaName;
            }),
        pImpl->schemas.end()
    );
}

std::vector<std::string> Config::getSchemaNames() const {
    std::vector<std::string> names;
    for (const auto& schema : pImpl->schemas) {
        names.push_back(schema.name);
    }
    return names;
}

std::string Config::exportToJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        oss << "  \"" << entry.key << "\": ";
        
        if (std::holds_alternative<int>(entry.value)) {
            oss << std::get<int>(entry.value);
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            oss << std::get<int64_t>(entry.value);
        } else if (std::holds_alternative<double>(entry.value)) {
            oss << std::get<double>(entry.value);
        } else if (std::holds_alternative<bool>(entry.value)) {
            oss << (std::get<bool>(entry.value) ? "true" : "false");
        } else if (std::holds_alternative<std::string>(entry.value)) {
            oss << "\"" << escapeJSON(std::get<std::string>(entry.value)) << "\"";
        }
        
        if (i < pImpl->entries.size() - 1) {
            oss << ",";
        }
        oss << "\n";
    }
    oss << "}\n";
    return oss.str();
}

std::string Config::escapeJSON(const std::string& str) const {
    std::ostringstream oss;
    for (char c : str) {
        switch (c) {
            case '\\': oss << "\\\\"; break;
            case '"': oss << "\\\""; break;
            case '\b': oss << "\\b"; break;
            case '\f': oss << "\\f"; break;
            case '\n': oss << "\\n"; break;
            case '\r': oss << "\\r"; break;
            case '\t': oss << "\\t"; break;
            default:
                if (c < ' ') {
                    oss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)c;
                } else {
                    oss << c;
                }
        }
    }
    return oss.str();
}

bool Config::importFromJSON(const std::string& json) {
    // Simple JSON parser - in production, use a proper JSON library
    std::istringstream iss(json);
    std::string line;
    std::vector<std::string> lines;
    
    // Extract key-value pairs
    size_t pos = 0;
    while (pos < json.size()) {
        pos = json.find('"', pos);
        if (pos == std::string::npos) break;
        
        size_t endKey = json.find('"', pos + 1);
        if (endKey == std::string::npos) break;
        
        std::string key = json.substr(pos + 1, endKey - pos - 1);
        
        pos = json.find(':', endKey);
        if (pos == std::string::npos) break;
        
        pos = json.find_first_not_of(' ', pos + 1);
        if (pos == std::string::npos) break;
        
        if (json[pos] == '"') {
            size_t endValue = json.find('"', pos + 1);
            if (endValue == std::string::npos) break;
            std::string value = json.substr(pos + 1, endValue - pos - 1);
            unescapeJSON(value);
            set(key, value, ConfigSource::File);
        } else if (json[pos] == 't' || json[pos] == 'f') {
            size_t endValue = json.find_first_of(",}", pos);
            if (endValue == std::string::npos) break;
            std::string value = json.substr(pos, endValue - pos);
            set(key, value == "true", ConfigSource::File);
        } else if (json[pos] == 'n' && json[pos + 1] == 'u' && json[pos + 2] == 'l') {
            // null value - skip
            continue;
        } else {
            size_t endValue = json.find_first_of(",}", pos);
            if (endValue == std::string::npos) break;
            std::string value = json.substr(pos, endValue - pos);
            // Try to convert to number
            try {
                size_t decimalPos = value.find('.');
                if (decimalPos != std::string::npos) {
                    set(key, std::stod(value), ConfigSource::File);
                } else {
                    set(key, std::stoll(value), ConfigSource::File);
                }
            } catch (...) {
                set(key, value, ConfigSource::File);
            }
        }
        
        pos = json.find(',', endValue);
        if (pos == std::string::npos) break;
        pos++;
    }
    
    return true;
}

void Config::unescapeJSON(std::string& str) const {
    size_t pos = 0;
    while ((pos = str.find('\\', pos)) != std::string::npos) {
        if (pos + 1 < str.size()) {
            char next = str[pos + 1];
            switch (next) {
                case '\"': str.replace(pos, 2, "\""); break;
                case '\\\\': str.replace(pos, 2, "\\"); break;
                case '\\/': str.replace(pos, 2, "/"); break;
                case '\\b': str.replace(pos, 2, "\b"); break;
                case '\\f': str.replace(pos, 2, "\f"); break;
                case '\\n': str.replace(pos, 2, "\n"); break;
                case '\\r': str.replace(pos, 2, "\r"); break;
                case '\\t': str.replace(pos, 2, "\t"); break;
                default: pos += 2; break;
            }
        } else {
            break;
        }
    }
}

bool Config::loadFromFile(const std::string& filepath) {
    // First, validate the file exists and is readable
    if (!std::filesystem::exists(filepath)) {
        NLM_LOG_ERROR("Configuration file does not exist: " + filepath);
        return false;
    }
    
    if (!std::filesystem::is_regular_file(filepath)) {
        NLM_LOG_ERROR("Configuration path is not a file: " + filepath);
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open configuration file: " + filepath);
        return false;
    }
    
    // Try to detect file format
    std::string line;
    bool isJSON = false;
    bool isINI = false;
    int lineCount = 0;
    
    while (std::getline(file, line) && lineCount < 100) {
        line = trim(line);
        if (line.empty()) continue;
        
        // Check for JSON format
        if (line.find('{') != std::string::npos && line.find('}') != std::string::npos) {
            isJSON = true;
            break;
        }
        
        // Check for INI/format format
        if (line.find('=') != std::string::npos) {
            isINI = true;
        }
        
        lineCount++;
    }
    
    file.clear();
    file.seekg(0);
    
    bool success = false;
    if (isJSON) {
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string json = buffer.str();
        success = importFromJSON(json);
    } else {
        // Original simple parser
        while (std::getline(file, line)) {
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
                
                // Convert numeric values
                if (isNumeric(value)) {
                    if (value.find('.') != std::string::npos) {
                        set(key, std::stod(value), ConfigSource::File);
                    } else {
                        set(key, std::stoll(value), ConfigSource::File);
                    }
                } else if (value == "true" || value == "false") {
                    set(key, value == "true", ConfigSource::File);
                } else {
                    set(key, value, ConfigSource::File);
                }
            }
        }
        success = true;
    }
    
    // Validate loaded configuration
    if (success && !validateAll()) {
        NLM_LOG_ERROR("Loaded configuration validation failed: " + getValidationErrors());
        // Optionally, clear invalid entries or return error
    }
    
    return success;
}

bool Config::isNumeric(const std::string& str) const {
    if (str.empty()) return false;
    
    size_t start = 0;
    if (str[0] == '+' || str[0] == '-') {
        start = 1;
    }
    
    bool hasDecimal = false;
    bool hasExponent = false;
    for (size_t i = start; i < str.size(); ++i) {
        if (std::isdigit(str[i])) {
            continue;
        } else if (str[i] == '.' && !hasDecimal && !hasExponent) {
            hasDecimal = true;
        } else if ((str[i] == 'e' || str[i] == 'E') && !hasExponent) {
            hasExponent = true;
        } else {
            return false;
        }
    }
    
    return true;
}

std::string Config::getValidationError() const {
    return pImpl->stats.errorMessages.empty() ? "" : pImpl->stats.errorMessages.back();
}

std::vector<std::string> Config::getAllValidationErrors() const {
    return pImpl->stats.errorMessages;
}

void Config::clearValidationErrors() {
    pImpl->stats.errorMessages.clear();
}

Config::Impl::Impl() {
    validationRules.clear();
    schemas.clear();
    stats = ValidationStats();
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.find("--config=") == 0) {
            // Special config file argument
            std::string configFile = arg.substr(9);
            if (!loadFromFile(configFile)) {
                NLM_LOG_ERROR("Failed to load config file from --config=" + configFile);
                success = false;
            }
        }
        else if (arg.find("--validate=") == 0) {
            // Validation-specific arguments
            std::string key = arg.substr(10);
            if (has(key)) {
                std::cout << "Key: " << key << " = ";
                auto val = get<std::string>(key);
                if (val) std::cout << *val;
                std::cout << std::endl;
            }
        }
        // Handle --key=value format
        else if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Convert value based on type
                if (isNumeric(value)) {
                    if (value.find('.') != std::string::npos) {
                        set(key, std::stod(value), ConfigSource::CommandLine);
                    } else {
                        set(key, std::stoll(value), ConfigSource::CommandLine);
                    }
                } else if (value == "true" || value == "false") {
                    set(key, value == "true", ConfigSource::CommandLine);
                } else {
                    set(key, value, ConfigSource::CommandLine);
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            if (isNumeric(value)) {
                if (value.find('.') != std::string::npos) {
                    set(key, std::stod(value), ConfigSource::CommandLine);
                } else {
                    set(key, std::stoll(value), ConfigSource::CommandLine);
                }
            } else if (value == "true" || value == "false") {
                set(key, value == "true", ConfigSource::CommandLine);
            } else {
                set(key, value, ConfigSource::CommandLine);
            }
        }
    }
    
    return success;
}

bool Config::loadFromArgs(int argc, char** argv) {
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

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
    return true;
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
        it->value = value;
        it->source = source;
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
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
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
