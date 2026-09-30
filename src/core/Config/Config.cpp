#include "Config.hpp"
#include "Schema.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <cctype>
#include <regex>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ValidationRule> validationRules;
    std::vector<Schema> appliedSchemas;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    try {
        parseJSON(content);
        return true;
    } catch (const std::exception& e) {
        // Fallback to simple key=value format
        std::ifstream file2(filepath);
        std::string line;
        bool success = false;
        
        while (std::getline(file2, line)) {
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
                    ((value.front() == '\"' && value.back() == '\"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
                success = true;
            }
        }
        return success;
    }
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
    // Validate against applied schemas
    for (const auto& schema : pImpl->appliedSchemas) {
        if (!schema.validate(value)) {
            throw ValidationError(key, "Value does not conform to schema \"" + schema.getName() + "\"");
        }
    }
    
    // Validate against individual validation rules
    for (const auto& rule : pImpl->validationRules) {
        if (rule.key == key) {
            if (!validateValue(value, rule)) {
                throw ValidationError(key, rule.errorMessage);
            }
            break;
        }
    }
    
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
    // Also remove validation rules for this key
    pImpl->validationRules.erase(
        std::remove_if(pImpl->validationRules.begin(), pImpl->validationRules.end(),
            [&key](const ValidationRule& rule) { return rule.key == key; }),
        pImpl->validationRules.end()
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
    pImpl->validationRules.clear();
    pImpl->appliedSchemas.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries, " 
        << pImpl->validationRules.size() << " validation rules, "
        << pImpl->appliedSchemas.size() << " applied schemas):\n";
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

void Config::addValidationRule(const ValidationRule& rule) {
    pImpl->validationRules.push_back(rule);
}

void Config::removeValidationRule(const std::string& key) {
    pImpl->validationRules.erase(
        std::remove_if(pImpl->validationRules.begin(), pImpl->validationRules.end(),
            [&key](const ValidationRule& rule) { return rule.key == key; }),
        pImpl->validationRules.end()
    );
}

void Config::clearValidationRules() {
    pImpl->validationRules.clear();
}

bool Config::validate() const {
    return validateAll().empty();
}

std::vector<ValidationError> Config::validateAll() const {
    std::vector<ValidationError> errors;
    
    for (const auto& rule : pImpl->validationRules) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&rule](const ConfigEntry& e) { return e.key == rule.key; });
        
        if (it != pImpl->entries.end()) {
            if (!validateValue(it->value, rule)) {
                errors.emplace_back(rule.key, rule.errorMessage);
            }
        }
    }
    
    return errors;
}

bool Config::isValid(const std::string& key) const {
    auto errors = getValidationErrors(key);
    return errors.empty();
}

std::vector<ValidationError> Config::getValidationErrors(const std::string& key) const {
    std::vector<ValidationError> errors;
    
    for (const auto& rule : pImpl->validationRules) {
        if (rule.key == key) {
            auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                [&rule](const ConfigEntry& e) { return e.key == rule.key; });
            
            if (it != pImpl->entries.end()) {
                if (!validateValue(it->value, rule)) {
                    errors.emplace_back(rule.key, rule.errorMessage);
                }
            }
        }
    }
    
    return errors;
}

void Config::applySchema(const Schema& schema) {
    pImpl->appliedSchemas.push_back(schema);
}

void Config::removeSchema(const std::string& schemaName) {
    pImpl->appliedSchemas.erase(
        std::remove_if(pImpl->appliedSchemas.begin(), pImpl->appliedSchemas.end(),
            [&schemaName](const Schema& schema) { return schema.getName() == schemaName; }),
        pImpl->appliedSchemas.end()
    );
}

bool Config::isSchemaValid(const std::string& schemaName) const {
    for (const auto& schema : pImpl->appliedSchemas) {
        if (schema.getName() == schemaName) {
            // For schema-based validation, we need to check all values against the schema
            // This is a simplified implementation
            return true;
        }
    }
    return false;
}

std::vector<ValidationError> Config::getSchemaValidationErrors(const std::string& schemaName) const {
    std::vector<ValidationError> errors;
    
    for (const auto& schema : pImpl->appliedSchemas) {
        if (schema.getName() == schemaName) {
            // For schema-based validation, we need to check all values against the schema
            // This is a simplified implementation that returns generic errors
            errors.emplace_back("SCHEMA", "Schema validation not fully implemented");
        }
    }
    
    return errors;
}

ValidationRule Config::createRangeValidation(const std::string& key, double min, double max,
                                            const std::string& errorMessage) {
    ValidationRule rule(key, ValidationType::Range, errorMessage);
    rule.validationData = RangeValidation(min, max);
    return rule;
}

ValidationRule Config::createEnumValidation(const std::string& key,
                                           const std::set<std::string>& allowedValues,
                                           const std::string& errorMessage) {
    ValidationRule rule(key, ValidationType::Enum, errorMessage);
    rule.validationData = EnumValidation(allowedValues);
    return rule;
}

ValidationRule Config::createStringPatternValidation(const std::string& key,
                                                   const std::string& pattern,
                                                   bool caseSensitive,
                                                   const std::string& errorMessage) {
    ValidationRule rule(key, ValidationType::StringPattern, errorMessage);
    rule.validationData = StringPatternValidation(pattern, caseSensitive);
    return rule;
}

ValidationRule Config::createCustomValidation(const std::string& key,
                                             CustomValidator validator,
                                             const std::string& errorMessage) {
    ValidationRule rule(key, ValidationType::Custom, errorMessage);
    rule.validationData = validator;
    return rule;
}

Schema Config::createBrainSchema() {
    Schema schema("BrainConfiguration", "Configuration for NLM brain simulation", "1.0");
    
    // Neuron count
    SchemaProperty neuronCount("neuron_count", SchemaValueType::Integer, "Number of neurons in the brain");
    neuronCount.defaultValue = "500";
    neuronCount.constraints.push_back(SchemaConstraint("range", "1,10000", "Neuron count must be between 1 and 10000"));
    schema.addProperty(neuronCount);
    
    // Region count
    SchemaProperty regionCount("region_count", SchemaValueType::Integer, "Number of neural regions");
    regionCount.defaultValue = "1";
    regionCount.constraints.push_back(SchemaConstraint("range", "1,10", "Region count must be between 1 and 10"));
    schema.addProperty(regionCount);
    
    // Connection probability
    SchemaProperty connectionProb("connection_probability", SchemaValueType::Double, "Probability of synapse formation");
    connectionProb.defaultValue = "0.1";
    connectionProb.constraints.push_back(SchemaConstraint("range", "0.0,1.0", "Connection probability must be between 0 and 1"));
    schema.addProperty(connectionProb);
    
    // Simulation timestep
    SchemaProperty timestep("simulation_timestep", SchemaValueType::Double, "Simulation time step in seconds");
    timestep.defaultValue = "0.001";
    timestep.constraints.push_back(SchemaConstraint("range", "0.0001,1.0", "Time step must be between 0.0001s and 1.0s"));
    schema.addProperty(timestep);
    
    // STDP parameters
    SchemaProperty stdpLTP("stdp_ltp_weight", SchemaValueType::Double, "STDP long-term potentiation weight");
    stdpLTP.defaultValue = "0.01";
    schema.addProperty(stdpLTP);
    
    SchemaProperty stdpLTD("stdp_ltd_weight", SchemaValueType::Double, "STDP long-term depression weight");
    stdpLTD.defaultValue = "0.012";
    schema.addProperty(stdpLTD);
    
    SchemaProperty stdpTau("stdp_tau", SchemaValueType::Double, "STDP time constant");
    stdpTau.defaultValue = "20.0";
    schema.addProperty(stdpTau);
    
    // Plasticity rates
    SchemaProperty synaptogenesis("synaptogenesis_rate", SchemaValueType::Double, "Rate of new synapse formation");
    synaptogenesis.defaultValue = "0.0001";
    schema.addProperty(synaptogenesis);
    
    SchemaProperty pruning("pruning_rate", SchemaValueType::Double, "Rate of synapse pruning");
    pruning.defaultValue = "0.00001";
    schema.addProperty(pruning);
    
    return schema;
}

Schema Config::createWorldSchema() {
    Schema schema("WorldConfiguration", "Configuration for NLM world simulation", "1.0");
    
    SchemaProperty width("environment_width", SchemaValueType::Integer, "Width of the simulation world");
    width.defaultValue = "20";
    width.constraints.push_back(SchemaConstraint("range", "10,100", "World width must be between 10 and 100"));
    schema.addProperty(width);
    
    SchemaProperty height("environment_height", SchemaValueType::Integer, "Height of the simulation world");
    height.defaultValue = "20";
    height.constraints.push_back(SchemaConstraint("range", "10,100", "World height must be between 10 and 100"));
    schema.addProperty(height);
    
    SchemaProperty visionWidth("vision_width", SchemaValueType::Integer, "Vision width in pixels");
    visionWidth.defaultValue = "16";
    schema.addProperty(visionWidth);
    
    SchemaProperty visionHeight("vision_height", SchemaValueType::Integer, "Vision height in pixels");
    visionHeight.defaultValue = "16";
    schema.addProperty(visionHeight);
    
    return schema;
}

Schema Config::createAgentSchema() {
    Schema schema("AgentConfiguration", "Configuration for NLM agent", "1.0");
    
    SchemaProperty maxEnergy("max_energy", SchemaValueType::Double, "Maximum energy level");
    maxEnergy.defaultValue = "100.0";
    maxEnergy.constraints.push_back(SchemaConstraint("range", "10.0,1000.0", "Max energy must be between 10.0 and 1000.0"));
    schema.addProperty(maxEnergy);
    
    SchemaProperty energyDecay("energy_decay_rate", SchemaValueType::Double, "Energy decay rate per timestep");
    energyDecay.defaultValue = "0.01";
    energyDecay.constraints.push_back(SchemaConstraint("range", "0.0,0.1", "Energy decay rate must be between 0.0 and 0.1"));
    schema.addProperty(energyDecay);
    
    return schema;
}

SchemaBasedConfig::SchemaBasedConfig(const Schema& schema) : schema_(schema) {}

bool SchemaBasedConfig::validate(const ConfigValue& value) const {
    return schema_.validate(value);
}

std::string SchemaBasedConfig::getErrorMessage(const ConfigValue& value) const {
    return "Validation failed for schema \"" + schema_.getName() + "\"";
}

std::string SchemaBasedConfig::schemaToJSON() const {
    return schema_.toJSON();
}

SchemaRegistry::SchemaRegistry() : schemas_() {}

void SchemaRegistry::registerSchema(const Schema& schema) {
    schemas_[schema.getName()] = SchemaBasedConfig(schema);
}

SchemaBasedConfig* SchemaRegistry::getSchema(const std::string& name) {
    auto it = schemas_.find(name);
    if (it != schemas_.end()) {
        return &it->second;
    }
    return nullptr;
}

bool SchemaRegistry::hasSchema(const std::string& name) const {
    return schemas_.find(name) != schemas_.end();
}

std::vector<std::string> SchemaRegistry::getSchemaNames() const {
    std::vector<std::string> names;
    for (const auto& pair : schemas_) {
        names.push_back(pair.first);
    }
    return names;
}

void SchemaRegistry::unregisterSchema(const std::string& name) {
    schemas_.erase(name);
}

// JSON parsing functions
void Config::parseJSON(const std::string& json) {
    // Simple JSON parser for NLM configuration
    // Supports: { "key": value, "array_key": [1, 2, 3], "bool_key": true }
    // Note: This is a simplified parser for production use
    
    size_t pos = 0;
    skipWhitespace(json, pos);
    
    if (json[pos] != '{') {
        throw std::runtime_error("Invalid JSON: expected object start '{'");
    }
    
    pos++; // skip '{'
    skipWhitespace(json, pos);
    
    while (pos < json.size() && json[pos] != '}') {
        // Parse key
        skipWhitespace(json, pos);
        if (json[pos] != '\"') {
            throw std::runtime_error("Invalid JSON: expected quoted key");
        }
        
        size_t keyStart = ++pos;
        while (pos < json.size() && json[pos] != '\"') {
            if (json[pos] == '\\') pos++; // skip escaped character
            pos++;
        }
        
        if (pos >= json.size()) {
            throw std::runtime_error("Invalid JSON: unterminated string");
        }
        
        std::string key = json.substr(keyStart, pos - keyStart);
        pos++; // skip closing quote
        
        skipWhitespace(json, pos);
        
        if (pos >= json.size() || json[pos] != ':') {
            throw std::runtime_error("Invalid JSON: expected ':' after key");
        }
        
        pos++; // skip ':'
        skipWhitespace(json, pos);
        
        // Parse value
        ConfigValue value;
        if (json[pos] == '\"') {
            value = parseString(json, pos);
        } else if (json[pos] == 't' || json[pos] == 'f') {
            value = parseBoolean(json, pos);
        } else if (json[pos] == 'n') {
            value = parseNull(json, pos);
        } else if (json[pos] == '[') {
            value = parseArray(json, pos);
        } else {
            value = parseNumber(json, pos);
        }
        
        set(key, value, ConfigSource::File);
        
        skipWhitespace(json, pos);
        if (pos < json.size() && json[pos] == ',') {
            pos++; // skip ','
        }
        skipWhitespace(json, pos);
    }
    
    if (pos >= json.size() || json[pos] != '}') {
        throw std::runtime_error("Invalid JSON: expected object end '}'");
    }
}

std::string Config::parseString(const std::string& json, size_t& pos) {
    if (json[pos] != '\"') {
        throw std::runtime_error("Invalid JSON: expected string start \"\"");
    }
    
    pos++; // skip opening quote
    std::string result;
    
    while (pos < json.size() && json[pos] != '\"') {
        if (json[pos] == '\\\\') {
            pos++; // skip backslash
            if (pos < json.size()) {
                result += json[pos];
                pos++;
            }
        } else if (json[pos] == '\\"') {
            result += '\"';
            pos++;
        } else if (json[pos] == '\\\\') {
            result += '\\\\';
            pos++;
        } else {
            result += json[pos];
            pos++;
        }
    }
    
    if (pos >= json.size()) {
        throw std::runtime_error("Invalid JSON: unterminated string");
    }
    
    pos++; // skip closing quote
    return result;
}

bool Config::parseBoolean(const std::string& json, size_t& pos) {
    if (json.compare(pos, 4, "true") == 0) {
        pos += 4;
        return true;
    } else if (json.compare(pos, 5, "false") == 0) {
        pos += 5;
        return false;
    } else {
        throw std::runtime_error("Invalid JSON: expected boolean value");
    }
}

ConfigValue Config::parseNull(const std::string& json, size_t& pos) {
    if (json.compare(pos, 4, "null") == 0) {
        pos += 4;
        return std::string(); // Empty string for null
    } else {
        throw std::runtime_error("Invalid JSON: expected null value");
    }
}

ConfigValue Config::parseNumber(const std::string& json, size_t& pos) {
    size_t start = pos;
    
    // Parse sign
    if (json[pos] == '+' || json[pos] == '-') {
        pos++;
    }
    
    // Parse integer part
    while (pos < json.size() && std::isdigit(json[pos])) {
        pos++;
    }
    
    // Parse decimal part
    if (pos < json.size() && json[pos] == '.') {
        pos++;
        while (pos < json.size() && std::isdigit(json[pos])) {
            pos++;
        }
    }
    
    // Parse exponent
    if (pos < json.size() && (json[pos] == 'e' || json[pos] == 'E')) {
        pos++;
        if (pos < json.size() && (json[pos] == '+' || json[pos] == '-')) {
            pos++;
        }
        while (pos < json.size() && std::isdigit(json[pos])) {
            pos++;
        }
    }
    
    if (pos == start) {
        throw std::runtime_error("Invalid JSON: expected number");
    }
    
    std::string numStr = json.substr(start, pos - start);
    
    // Try to parse as int64_t first
    try {
        size_t endpos;
        int64_t intVal = std::stoll(numStr, &endpos);
        if (endpos == numStr.size()) {
            return intVal;
        }
    } catch (...) {}
    
    // Try to parse as double
    try {
        size_t endpos;
        double doubleVal = std::stod(numStr, &endpos);
        if (endpos == numStr.size()) {
            return doubleVal;
        }
    } catch (...) {}
    
    // Parse as float
    return std::stof(numStr);
}

ConfigValue Config::parseArray(const std::string& json, size_t& pos) {
    if (json[pos] != '[') {
        throw std::runtime_error("Invalid JSON: expected array start '['");
    }
    
    pos++; // skip '['
    std::vector<double> array;
    
    skipWhitespace(json, pos);
    
    while (pos < json.size() && json[pos] != ']') {
        // Parse element
        ConfigValue value;
        if (json[pos] == '\"') {
            value = parseString(json, pos);
            if (std::holds_alternative<std::string>(value)) {
                array.push_back(0.0); // Convert string to placeholder
            }
        } else if (json[pos] == 't' || json[pos] == 'f') {
            value = parseBoolean(json, pos);
            if (std::holds_alternative<bool>(value)) {
                array.push_back(value == true ? 1.0 : 0.0);
            }
        } else if (json[pos] == 'n') {
            value = parseNull(json, pos);
            array.push_back(0.0); // Convert null to placeholder
        } else if (json[pos] == '[') {
            // Nested array - not supported in this simple parser
            throw std::runtime_error("Nested arrays not supported in simple JSON parser");
        } else {
            value = parseNumber(json, pos);
            if (std::holds_alternative<double>(value)) {
                array.push_back(std::get<double>(value));
            }
        }
        
        skipWhitespace(json, pos);
        if (pos < json.size() && json[pos] == ',') {
            pos++; // skip ','
        }
        skipWhitespace(json, pos);
    }
    
    if (pos >= json.size() || json[pos] != ']') {
        throw std::runtime_error("Invalid JSON: expected array end ']'");
    }
    
    pos++; // skip ']'
    return array;
}

void Config::skipWhitespace(const std::string& json, size_t& pos) {
    while (pos < json.size() && std::isspace(json[pos])) {
        pos++;
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