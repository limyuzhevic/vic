#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

// ConfigSchema implementation
void ConfigSchema::addEntry(const ConfigSchemaEntry& entry) {
    entries_.push_back(entry);
}

bool ConfigSchema::hasKey(const std::string& key) const {
    return std::any_of(entries_.begin(), entries_.end(),
        [&key](const ConfigSchemaEntry& e) { return e.key == key; });
}

const ConfigSchemaEntry* ConfigSchema::getEntry(const std::string& key) const {
    auto it = std::find_if(entries_.begin(), entries_.end(),
        [&key](const ConfigSchemaEntry& e) { return e.key == key; });
    return (it != entries_.end()) ? &(*it) : nullptr;
}

std::vector<std::string> ConfigSchema::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(entries_.size());
    for (const auto& entry : entries_) {
        keys.push_back(entry.key);
    }
    return keys;
}

std::vector<ConfigSchemaEntry> ConfigSchema::getEntriesForSource(ConfigSource source) const {
    std::vector<ConfigSchemaEntry> result;
    for (const auto& entry : entries_) {
        if (entry.source == source) {
            result.push_back(entry);
        }
    }
    return result;
}

void ConfigSchema::validate(const std::string& key, const ConfigValue& value) const {
    const ConfigSchemaEntry* entry = getEntry(key);
    if (!entry) {
        throw ConfigValidationError("Unknown configuration key", key, value);
    }
    
    // Type validation
    if (!checkType(entry->type, value)) {
        std::string expectedTypes;
        if (entry->type == "int") expectedTypes = "integer";
        else if (entry->type == "int64_t") expectedTypes = "64-bit integer";
        else if (entry->type == "double") expectedTypes = "floating-point number";
        else if (entry->type == "bool") expectedTypes = "boolean";
        else if (entry->type == "string") expectedTypes = "string";
        else if (entry->type == "vector<int>") expectedTypes = "list of integers";
        else if (entry->type == "vector<double>") expectedTypes = "list of floating-point numbers";
        else if (entry->type == "vector<string>") expectedTypes = "list of strings";
        
        throw ConfigValidationError(
            "Invalid value type for key \"" + key + "\": expected " + expectedTypes,
            key, value
        );
    }
    
    // Range validation
    validateRange(*entry, value);
    
    // Pattern validation for strings
    validatePattern(*entry, value);
}

void ConfigSchema::validateAll(const std::map<std::string, ConfigEntry>& entries) const {
    for (const auto& pair : entries) {
        validate(pair.first, pair.second.value);
    }
}

void ConfigSchema::applyDefaults(std::map<std::string, ConfigEntry>& entries) const {
    for (const auto& entry : entries_) {
        if (entry.default_value.has_value()) {
            std::string key = entry.key;
            if (entries.find(key) == entries.end()) {
                entries[key] = ConfigEntry(key, entry.default_value.value(), entry.source);
            }
        }
    }
}

std::string ConfigSchema::getSummary() const {
    std::ostringstream oss;
    oss << "Configuration Schema (" << entries_.size() << " entries):\n";
    for (const auto& entry : entries_) {
        oss << "  " << entry.key << ": ";
        oss << entry.type << " (";
        
        // Source description
        switch (entry.source) {
            case ConfigSource::Default: oss << "default"; break;
            case ConfigSource::File: oss << "file"; break;
            case ConfigSource::CommandLine: oss << "command-line"; break;
            case ConfigSource::Runtime: oss << "runtime"; break;
        }
        
        oss << ") - " << entry.description << "\n";
        
        // Show constraints
        if (entry.min_value.has_value() || entry.max_value.has_value()) {
            oss << "    Range: ";
            if (entry.min_value.has_value()) oss << "min=" << entry.min_value.value();
            oss << ", ";
            if (entry.max_value.has_value()) oss << "max=" << entry.max_value.value();
            oss << "\n";
        }
    }
    return oss.str();
}

bool ConfigSchema::checkType(const std::string& type, const ConfigValue& value) const {
    if (type == "int") return std::holds_alternative<int>(value);
    if (type == "int64_t") return std::holds_alternative<int64_t>(value);
    if (type == "double") return std::holds_alternative<double>(value);
    if (type == "bool") return std::holds_alternative<bool>(value);
    if (type == "string") return std::holds_alternative<std::string>(value);
    if (type == "vector<int>") return std::holds_alternative<std::vector<int>>(value);
    if (type == "vector<double>") return std::holds_alternative<std::vector<double>>(value);
    if (type == "vector<string>") return std::holds_alternative<std::vector<std::string>>(value);
    return false;
}

void ConfigSchema::validateRange(const ConfigSchemaEntry& entry, const ConfigValue& value) const {
    if (entry.min_value.has_value() || entry.max_value.has_value()) {
        double val;
        
        // Extract numeric value for range checking
        if (std::holds_alternative<int>(value)) {
            val = static_cast<double>(std::get<int>(value));
        } else if (std::holds_alternative<int64_t>(value)) {
            val = static_cast<double>(std::get<int64_t>(value));
        } else if (std::holds_alternative<double>(value)) {
            val = std::get<double>(value);
        } else if (std::holds_alternative<std::vector<int>>(value)) {
            auto& vec = std::get<std::vector<int>>(value);
            if (vec.empty()) return; // Skip empty vectors
            val = *std::min_element(vec.begin(), vec.end(),
                [](int a, int b) { return a < b; });
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            auto& vec = std::get<std::vector<double>>(value);
            if (vec.empty()) return; // Skip empty vectors
            val = *std::min_element(vec.begin(), vec.end(),
                [](double a, double b) { return a < b; });
        } else {
            return; // Non-numeric types skip range validation
        }
        
        if (entry.min_value.has_value() && val < entry.min_value.value()) {
            throw ConfigValidationError(
                "Value " + std::to_string(val) + " for key \"" + entry.key + 
                " is less than minimum allowed value " + std::to_string(entry.min_value.value()),
                entry.key, value
            );
        }
        
        if (entry.max_value.has_value() && val > entry.max_value.value()) {
            throw ConfigValidationError(
                "Value " + std::to_string(val) + " for key \"" + entry.key + 
                " is greater than maximum allowed value " + std::to_string(entry.max_value.value()),
                entry.key, value
            );
        }
    }
}

void ConfigSchema::validatePattern(const ConfigSchemaEntry& entry, const ConfigValue& value) const {
    if (entry.pattern.has_value() && std::holds_alternative<std::string>(value)) {
        std::string str = std::get<std::string>(value);
        std::regex pattern(entry.pattern.value());
        if (!std::regex_match(str, pattern)) {
            throw ConfigValidationError(
                "String value \"" + str + "\" for key \"" + entry.key + 
                " does not match pattern \"" + entry.pattern.value() + "\"",
                entry.key, value
            );
        }
    }
}

Config::Config() : pImpl(std::make_unique<Impl>()) {
    initializeSchema();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::initializeSchema() {
    // Core configuration schema
    schema_.addEntry(ConfigSchemaEntry(
        "random_seed", "int64_t", ConfigSource::Default,
        "Random seed for reproducible experiments (0 = use time-based seed)",
        std::optional<ConfigValue>(static_cast<int64_t>(42))
    ));
    
    schema_.addEntry(ConfigSchemaEntry(
        "simulation_timestep", "double", ConfigSource::Default,
        "Simulation timestep in seconds",
        std::optional<ConfigValue>(0.001)
    ));
    
    schema_.addEntry(ConfigSchemaEntry(
        "neuron_count", "int64_t", ConfigSource::Default,
        "Number of neurons in the brain (must be >= 1)",
        std::optional<ConfigValue>(static_cast<int64_t>(1000)),
        std::optional<double>(1.0)
    ));
    
    schema_.addEntry(ConfigSchemaEntry(
        "region_count", "int64_t", ConfigSource::Default,
        "Number of brain regions (must be >= 1)",
        std::optional<ConfigValue>(static_cast<int64_t>(2)),
        std::optional<double>(1.0)
    ));
    
    schema_.addEntry(ConfigSchemaEntry(
        "connection_probability", "double", ConfigSource::Default,
        "Connection probability for random connectivity (0.0 to 1.0)",
        std::optional<ConfigValue>(0.1),
        std::optional<double>(0.0),
        std::optional<double>(1.0)
    ));
    
    // Plasticity settings
    schema_.addEntry(ConfigSchemaEntry(
        "plasticity_learning_rate", "double", ConfigSource::Default,
        "Global learning rate for plasticity rules (0.0 to 1.0)",
        std::optional<ConfigValue>(0.01),
        std::optional<double>(0.0),
        std::optional<double>(1.0)
    ));
    
    schema_.addEntry(ConfigSchemaEntry(
        "stdp_ltp_weight", "double", ConfigSource::Default,
        "STDP LTP weight factor (positive value)",
        std::optional<ConfigValue>(0.01),
        std::optional<double>(0.0)
    ));
    
    schema_.addEntry(ConfigSchemaEntry(
        "stdp_ltd_weight", "double", ConfigSource::Default,
        "STDP LTD weight factor (positive value)",
        std::optional<ConfigValue>(0.012),
        std::optional<double>(0.0)
    ));
    
    schema_.addEntry(ConfigSchemaEntry(
        "stdp_tau", "double", ConfigSource::Default,
        "STDP time constant in milliseconds (positive value)",
        std::optional<ConfigValue>(20.0),
        std::optional<double>(0.0)
    ));
    
    // Development settings
    schema_.addEntry(ConfigSchemaEntry(
        "synaptogenesis_rate", "double", ConfigSource::Default,
        "Developmental synaptogenesis rate (0.0 to 1.0)",
        std::optional<ConfigValue>(0.001),
        std::optional<double>(0.0),
        std::optional<double>(1.0)
    ));
    
    schema_.addEntry(ConfigSchemaEntry(
        "pruning_rate", "double", ConfigSource::Default,
        "Developmental pruning rate (0.0 to 1.0)",
        std::optional<ConfigValue>(0.0001),
        std::optional<double>(0.0),
        std::optional<double>(1.0)
    ));
    
    // Neuromodulation settings
    schema_.addEntry(ConfigSchemaEntry(
        "dopamine_baseline", "double", ConfigSource::Default,
        "Baseline dopamine level (0.0 to 1.0)",
        std::optional<ConfigValue>(0.1),
        std::optional<double>(0.0),
        std::optional<double>(1.0)
    ));
    
    schema_.addEntry(ConfigSchemaEntry(
        "dopamine_pulse_rate", "double", ConfigSource::Default,
        "Rate of dopamine pulses (events per second)",
        std::optional<ConfigValue>(0.1),
        std::optional<double>(0.0)
    ));
    
    // Reward settings
    schema_.addEntry(ConfigSchemaEntry(
        "reward_discount_factor", "double", ConfigSource::Default,
        "Discount factor for reward prediction error (0.0 to 1.0)",
        std::optional<ConfigValue>(0.99),
        std::optional<double>(0.0),
        std::optional<double>(1.0)
    ));
    
    // Environment settings
    schema_.addEntry(ConfigSchemaEntry(
        "environment_name", "string", ConfigSource::Default,
        "Name of the environment to simulate",
        std::optional<ConfigValue>(std::string("GridWorld"))
    ));
    
    schema_.addEntry(ConfigSchemaEntry(
        "environment_width", "int64_t", ConfigSource::Default,
        "Width of the environment in grid cells (must be >= 1)",
        std::optional<ConfigValue>(static_cast<int64_t>(10)),
        std::optional<double>(1.0)
    ));
    
    schema_.addEntry(ConfigSchemaEntry(
        "environment_height", "int64_t", ConfigSource::Default,
        "Height of the environment in grid cells (must be >= 1)",
        std::optional<ConfigValue>(static_cast<int64_t>(10)),
        std::optional<double>(1.0)
    ));
    
    // Logging settings
    schema_.addEntry(ConfigSchemaEntry(
        "log_level", "string", ConfigSource::Default,
        "Logging level (DEBUG, INFO, WARN, ERROR)",
        std::optional<ConfigValue>(std::string("INFO"))
    ));
    
    schema_.addEntry(ConfigSchemaEntry(
        "log_to_file", "bool", ConfigSource::Default,
        "Whether to log to file as well as console",
        std::optional<ConfigValue>(false)
    ));
    
    schema_.addEntry(ConfigSchemaEntry(
        "log_filename", "string", ConfigSource::Default,
        "Filename for log file",
        std::optional<ConfigValue>(std::string("nlm.log"))
    ));
    
    // Simulation settings
    schema_.addEntry(ConfigSchemaEntry(
        "max_simulation_steps", "int64_t", ConfigSource::Default,
        "Maximum number of simulation steps (must be >= 0)",
        std::optional<ConfigValue>(static_cast<int64_t>(10000)),
        std::optional<double>(0.0)
    ));
    
    schema_.addEntry(ConfigSchemaEntry(
        "simulation_time_limit", "double", ConfigSource::Default,
        "Simulation time limit in seconds (0 = unlimited)",
        std::optional<ConfigValue>(0.0),
        std::optional<double>(0.0)
    ));
    
    // Visualization settings
    schema_.addEntry(ConfigSchemaEntry(
        "visualization_enabled", "bool", ConfigSource::Default,
        "Whether to enable visualization",
        std::optional<ConfigValue>(false)
    ));
    
    schema_.addEntry(ConfigSchemaEntry(
        "visualization_update_rate", "double", ConfigSource::Default,
        "Visualization update rate in Hz (must be >= 0)",
        std::optional<ConfigValue>(30.0),
        std::optional<double>(0.0)
    ));
}

const ConfigSchema& Config::getSchema() const {
    return schema_;
}

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
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
            std::string valueStr = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (valueStr.size() >= 2 && 
                ((valueStr.front() == '"' && valueStr.back() == '"') ||
                 (valueStr.front() == '\'' && valueStr.back() == '\''))) {
                valueStr = valueStr.substr(1, valueStr.size() - 2);
            }
            
            // Parse value based on schema type
            const ConfigSchemaEntry* entry = schema_.getEntry(key);
            if (entry) {
                try {
                    ConfigValue value = parseValue(valueStr, entry->type);
                    set(key, value, ConfigSource::File);
                } catch (const ConfigValidationError& e) {
                    // Log validation error but continue processing other entries
                    std::cerr << "Warning: Invalid value in config file: " << e.what() << std::endl;
                } catch (const std::exception& e) {
                    std::cerr << "Warning: Error parsing config value for key '" << key << "': " << e.what() << std::endl;
                }
            } else {
                // Unknown key - we could either ignore it or treat it as runtime
                set(key, valueStr, ConfigSource::File);
            }
        }
    }
    
    return true;
}

ConfigValue Config::parseValue(const std::string& valueStr, const std::string& type) const {
    if (type == "int") {
        return std::stoi(valueStr);
    } else if (type == "int64_t") {
        return static_cast<int64_t>(std::stoll(valueStr));
    } else if (type == "double") {
        return std::stod(valueStr);
    } else if (type == "bool") {
        std::string lower = toLower(valueStr);
        if (lower == "true" || lower == "1" || lower == "yes" || lower == "on") {
            return true;
        } else if (lower == "false" || lower == "0" || lower == "no" || lower == "off") {
            return false;
        } else {
            throw std::runtime_error("Invalid boolean value: " + valueStr);
        }
    } else if (type == "string") {
        return valueStr;
    } else if (type == "vector<int>") {
        // Parse comma-separated list
        std::vector<int> result;
        std::stringstream ss(valueStr);
        std::string token;
        while (std::getline(ss, token, ',')) {
            result.push_back(std::stoi(trim(token)));
        }
        return result;
    } else if (type == "vector<double>") {
        // Parse comma-separated list
        std::vector<double> result;
        std::stringstream ss(valueStr);
        std::string token;
        while (std::getline(ss, token, ',')) {
            result.push_back(std::stod(trim(token)));
        }
        return result;
    } else if (type == "vector<string>") {
        // Parse comma-separated list
        std::vector<std::string> result;
        std::stringstream ss(valueStr);
        std::string token;
        while (std::getline(ss, token, ',')) {
            result.push_back(trim(token));
        }
        return result;
    } else {
        throw std::runtime_error("Unknown type for value parsing: " + type);
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

void Config::loadFromKV(const std::map<std::string, std::string>& kvPairs) {
    for (const auto& pair : kvPairs) {
        set(pair.first, pair.second, ConfigSource::Runtime);
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        // Check if there's a default value in the schema
        const ConfigSchemaEntry* schemaEntry = schema_.getEntry(key);
        if (schemaEntry && schemaEntry->default_value.has_value()) {
            try {
                return std::get<T>(schemaEntry->default_value.value());
            } catch (const std::bad_variant_access&) {
                // Default value doesn't match requested type
            }
        }
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
    // Validate against schema if key is known
    try {
        schema_.validate(key, value);
    } catch (const ConfigValidationError& e) {
        throw ConfigValidationError(
            "Validation failed for key \"" + key + "\": " + e.what(),
            e.getKey(), e.getValue()
        );
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        // Find description from schema if available
        const ConfigSchemaEntry* schemaEntry = schema_.getEntry(key);
        std::string description = "";
        if (schemaEntry) {
            description = schemaEntry->description;
        }
        
        pImpl->entries.emplace_back(key, value, source, description);
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
    
    // Show all entries with schema information
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
        oss << "] (";
        
        // Source description
        switch (entry.source) {
            case ConfigSource::Default: oss << "default"; break;
            case ConfigSource::File: oss << "file"; break;
            case ConfigSource::CommandLine: oss << "command-line"; break;
            case ConfigSource::Runtime: oss << "runtime"; break;
        }
        
        oss << ")";
        
        // Show validation status
        const ConfigSchemaEntry* schemaEntry = schema_.getEntry(entry.key);
        if (schemaEntry) {
            oss << " ✓";
        } else {
            oss << " ⚠ (unknown key)";
        }
        
        oss << "\n";
    }
    
    // Show schema summary
    oss << "\n" << schema_.getSummary();
    
    return oss.str();
}

std::vector<ConfigEntry> Config::getEntriesBySource(ConfigSource source) const {
    std::vector<ConfigEntry> result;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            result.push_back(entry);
        }
    }
    return result;
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
// config.cpp (continued)
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
