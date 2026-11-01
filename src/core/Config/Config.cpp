// Helper function to normalize key names (e.g., "plasticity_learning_rate" -> "plasticity_learning_rate")
static std::string normalizeKeyName(const std::string& key) {
    // Currently just returns the key as-is, but this can be extended to normalize
    // different naming conventions (camelCase to snake_case, etc.)
    return key;
}

// Function to get all available parameter definitions for validation
static std::vector<ConfigParameter> getDefaultConfigParameters() {
    std::vector<ConfigParameter> parameters;
    
    // Core simulation parameters
    parameters.emplace_back("neuron_count", "int", "Number of neurons in the brain", true, "1000");
    parameters.emplace_back("region_count", "int", "Number of brain regions", true, "1");
    parameters.emplace_back("connection_probability", "float", "Connection probability for random connectivity", true, "0.1");
    parameters.emplace_back("random_seed", "int", "Random seed for reproducible experiments", true, "42");
    parameters.emplace_back("simulation_timestep", "float", "Simulation timestep in seconds", true, "0.001");
    
    // Plasticity parameters
    parameters.emplace_back("plasticity_learning_rate", "float", "Global plasticity learning rate", true, "0.01");
    parameters.emplace_back("stdp_ltp_weight", "float", "STDP LTP weight", true, "0.01");
    parameters.emplace_back("stdp_ltd_weight", "float", "STDP LTD weight", true, "0.012");
    parameters.emplace_back("stdp_time_constant", "float", "STDP time constant", true, "20.0");
    parameters.emplace_back("stdp_tau", "float", "STDP tau parameter", true, "20.0");
    
    // Development parameters
    parameters.emplace_back("development_synaptogenesis_rate", "float", "Synaptogenesis rate during development", true, "0.001");
    parameters.emplace_back("development_pruning_rate", "float", "Pruning rate during development", true, "0.0001");
    parameters.emplace_back("synaptogenesis_rate", "float", "Synaptogenesis rate (Phase 6)", true, "0.0001");
    parameters.emplace_back("pruning_rate", "float", "Pruning rate (Phase 6)", true, "0.00001");
    
    // Neuromodulation parameters
    parameters.emplace_back("dopamine_baseline", "float", "Dopamine baseline level", true, "0.1");
    parameters.emplace_back("reward_discount_factor", "float", "Reward discount factor", true, "0.99");
    
    // Environment parameters
    parameters.emplace_back("environment_name", "string", "Environment name", true, "GridWorld");
    parameters.emplace_back("environment_width", "int", "Environment width", true, "10");
    parameters.emplace_back("environment_height", "int", "Environment height", true, "10");
    
    // Simulation parameters
    parameters.emplace_back("max_simulation_steps", "int", "Maximum simulation steps", true, "10000");
    parameters.emplace_back("simulation_time_limit", "float", "Simulation time limit", false, "0.0");
    
    // Logging parameters
    parameters.emplace_back("log_level", "string", "Log level (DEBUG, INFO, WARN, ERROR)", true, "INFO");
    parameters.emplace_back("log_to_file", "bool", "Log to file", true, "false");
    parameters.emplace_back("log_filename", "string", "Log filename", true, "nlm.log");
    
    // Visualization parameters
    parameters.emplace_back("visualization_enabled", "bool", "Enable visualization", true, "false");
    parameters.emplace_back("visualization_update_rate", "float", "Visualization update rate", true, "30.0");
    
    // Development integration parameters
    parameters.emplace_back("replay_interval", "int", "Replay interval (steps)", true, "100");
    parameters.emplace_back("consolidation_interval", "int", "Consolidation interval (steps)", true, "1000");
    parameters.emplace_back("checkpoint_dir", "string", "Checkpoint directory", true, "./checkpoints");
    parameters.emplace_back("checkpoint_save_interval", "int", "Checkpoint save interval (steps)", true, "10000");
    parameters.emplace_back("max_checkpoints", "int", "Maximum number of checkpoints to keep", true, "10");
    parameters.emplace_back("compress_checkpoints", "bool", "Compress checkpoints", true, "true");
    
    // Experiment configuration
    parameters.emplace_back("enable_developmental_plasticity", "bool", "Enable developmental plasticity", true, "true");
    parameters.emplace_back("enable_structural_plasticity", "bool", "Enable structural plasticity", true, "true");
    parameters.emplace_back("enable_neuromodulation", "bool", "Enable neuromodulation", true, "true");
    
    return parameters;
}

// Function to validate configuration against schema
static bool validateConfigAgainstSchema(const std::vector<ConfigEntry>& entries, 
                                       const std::vector<ConfigParameter>& schema) {
    std::unordered_set<std::string> seenKeys;
    
    for (const auto& entry : entries) {
        // Check for duplicate keys
        if (seenKeys.find(entry.key) != seenKeys.end()) {
            return false;  // Duplicate key
        }
        seenKeys.insert(entry.key);
        
        // Find parameter in schema
        auto schemaIt = std::find_if(schema.begin(), schema.end(),
            [&entry](const ConfigParameter& p) { return p.key == entry.key; });
        
        if (schemaIt == schema.end()) {
            // Key not in schema (unknown parameter)
            return false;
        }
        
        // Validate parameter type
        bool typeValid = false;
        if (std::holds_alternative<int>(entry.value) && (schemaIt->type == "int" || schemaIt->type == "int64" || schemaIt->type == "number")) {
            typeValid = true;
        } else if (std::holds_alternative<int64_t>(entry.value) && (schemaIt->type == "int" || schemaIt->type == "int64" || schemaIt->type == "number")) {
            typeValid = true;
        } else if (std::holds_alternative<double>(entry.value) && (schemaIt->type == "float" || schemaIt->type == "double" || schemaIt->type == "number")) {
            typeValid = true;
        } else if (std::holds_alternative<bool>(entry.value) && schemaIt->type == "bool") {
            typeValid = true;
        } else if (std::holds_alternative<std::string>(entry.value) && schemaIt->type == "string") {
            typeValid = true;
        } else if (std::holds_alternative<std::vector<int>>(entry.value) && (schemaIt->type == "int[]" || schemaIt->type == "array")) {
            typeValid = true;
        } else if (std::holds_alternative<std::vector<double>>(entry.value) && (schemaIt->type == "float[]" || schemaIt->type == "array")) {
            typeValid = true;
        } else if (std::holds_alternative<std::vector<std::string>>(entry.value) && (schemaIt->type == "string[]" || schemaIt->type == "array")) {
            typeValid = true;
        }
        
        if (!typeValid) {
            return false;  // Invalid type
        }
    }
    
    return true;
}

// Function to get configuration error messages
static std::string getConfigErrorMessage(const std::vector<ConfigEntry>& entries, 
                                       const std::vector<ConfigParameter>& schema) {
    std::unordered_set<std::string> seenKeys;
    std::vector<std::string> errors;
    
    for (const auto& entry : entries) {
        // Check for duplicate keys
        if (seenKeys.find(entry.key) != seenKeys.end()) {
            errors.push_back("Duplicate configuration key: " + entry.key);
        }
        seenKeys.insert(entry.key);
        
        // Find parameter in schema
        auto schemaIt = std::find_if(schema.begin(), schema.end(),
            [&entry](const ConfigParameter& p) { return p.key == entry.key; });
        
        if (schemaIt == schema.end()) {
            // Key not in schema (unknown parameter)
            errors.push_back("Unknown configuration parameter: " + entry.key);
        } else {
            // Validate parameter type
            bool typeValid = false;
            if (std::holds_alternative<int>(entry.value) && (schemaIt->type == "int" || schemaIt->type == "int64" || schemaIt->type == "number")) {
                typeValid = true;
            } else if (std::holds_alternative<int64_t>(entry.value) && (schemaIt->type == "int" || schemaIt->type == "int64" || schemaIt->type == "number")) {
                typeValid = true;
            } else if (std::holds_alternative<double>(entry.value) && (schemaIt->type == "float" || schemaIt->type == "double" || schemaIt->type == "number")) {
                typeValid = true;
            } else if (std::holds_alternative<bool>(entry.value) && schemaIt->type == "bool") {
                typeValid = true;
            } else if (std::holds_alternative<std::string>(entry.value) && schemaIt->type == "string") {
                typeValid = true;
            } else if (std::holds_alternative<std::vector<int>>(entry.value) && (schemaIt->type == "int[]" || schemaIt->type == "array")) {
                typeValid = true;
            } else if (std::holds_alternative<std::vector<double>>(entry.value) && (schemaIt->type == "float[]" || schemaIt->type == "array")) {
                typeValid = true;
            } else if (std::holds_alternative<std::vector<std::string>>(entry.value) && (schemaIt->type == "string[]" || schemaIt->type == "array")) {
                typeValid = true;
            }
            
            if (!typeValid) {
                errors.push_back("Invalid configuration value for key " + entry.key + ": expected type " + schemaIt->type);
            }
        }
    }
    
    if (errors.empty()) {
        return "";
    }
    
    std::string errorMsg = "Configuration validation failed:\n";
    for (const auto& error : errors) {
        errorMsg += "  " + error + "\n";
    }
    return errorMsg;
}

// Function to get normalized key name
static std::string getNormalizedKey(const std::string& key) {
    return normalizeKeyName(key);
}

} // namespace nlm

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
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
            NLM_LOG_WARNING("Invalid configuration line " + std::to_string(lineNumber) + 
                           ": no '=' found: " + line);
        }
    }
    
    return true;
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

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
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
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
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

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    // Numeric values convert between int/int64/double/float/etc, so a
    // value stored as double can be read as float and vice versa.
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
        } else {
            return std::nullopt;
        }
    }, it->value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

}

// ConfigValidator implementation
struct ConfigValidator::Impl {
    std::vector<ConfigParameter> parameters;
    std::unordered_map<std::string, std::string> errors;
};

ConfigValidator::ConfigValidator() : pImpl(std::make_unique<Impl>()) {}
ConfigValidator::~ConfigValidator() = default;

void ConfigValidator::addParameter(const ConfigParameter& param) {
    pImpl->parameters.push_back(param);
}

bool ConfigValidator::validate(const std::string& key, const ConfigValue& value) const {
    auto it = std::find_if(pImpl->parameters.begin(), pImpl->parameters.end(),
        [&key](const ConfigParameter& p) { return p.key == key; });
    
    if (it == pImpl->parameters.end()) {
        // Unknown parameter
        return false;
    }
    
    // Validate parameter type
    bool typeValid = false;
    if (std::holds_alternative<int>(value) && (it->type == "int" || it->type == "int64" || it->type == "number")) {
        typeValid = true;
    } else if (std::holds_alternative<int64_t>(value) && (it->type == "int" || it->type == "int64" || it->type == "number")) {
        typeValid = true;
    } else if (std::holds_alternative<double>(value) && (it->type == "float" || it->type == "double" || it->type == "number")) {
        typeValid = true;
    } else if (std::holds_alternative<bool>(value) && it->type == "bool") {
        typeValid = true;
    } else if (std::holds_alternative<std::string>(value) && it->type == "string") {
        typeValid = true;
    } else if (std::holds_alternative<std::vector<int>>(value) && (it->type == "int[]" || it->type == "array")) {
        typeValid = true;
    } else if (std::holds_alternative<std::vector<double>>(value) && (it->type == "float[]" || it->type == "array")) {
        typeValid = true;
    } else if (std::holds_alternative<std::vector<std::string>>(value) && (it->type == "string[]" || it->type == "array")) {
        typeValid = true;
    }
    
    return typeValid;
}

bool ConfigValidator::validate(const std::vector<ConfigEntry>& entries) const {
    std::unordered_set<std::string> seenKeys;
    
    for (const auto& entry : entries) {
        // Check for duplicate keys
        if (seenKeys.find(entry.key) != seenKeys.end()) {
            pImpl->errors[entry.key] = "Duplicate configuration key: " + entry.key;
            return false;
        }
        seenKeys.insert(entry.key);
        
        // Validate parameter
        if (!validate(entry.key, entry.value)) {
            pImpl->errors[entry.key] = "Invalid configuration value for key: " + entry.key;
            return false;
        }
    }
    
    // Check for required parameters
    for (const auto& param : pImpl->parameters) {
        if (param.required && seenKeys.find(param.key) == seenKeys.end()) {
            pImpl->errors[param.key] = "Required configuration parameter missing: " + param.key;
            return false;
        }
    }
    
    return true;
}

std::vector<ConfigParameter> ConfigValidator::getParameters() const {
    return pImpl->parameters;
}

std::optional<ConfigParameter> ConfigValidator::getParameter(const std::string& key) const {
    auto it = std::find_if(pImpl->parameters.begin(), pImpl->parameters.end(),
        [&key](const ConfigParameter& p) { return p.key == key; });
    
    if (it != pImpl->parameters.end()) {
        return *it;
    }
    return std::nullopt;
}

bool ConfigValidator::hasParameter(const std::string& key) const {
    return std::any_of(pImpl->parameters.begin(), pImpl->parameters.end(),
        [&key](const ConfigParameter& p) { return p.key == key; });
}

std::string ConfigValidator::getError(const std::string& key, const ConfigValue& value) const {
    auto it = std::find_if(pImpl->parameters.begin(), pImpl->parameters.end(),
        [&key](const ConfigParameter& p) { return p.key == key; });
    
    if (it == pImpl->parameters.end()) {
        return "Unknown configuration parameter: " + key;
    }
    
    if (!validate(key, value)) {
        return "Invalid configuration value for key " + key + ": expected type " + it->type;
    }
    
    return "";
}

void ConfigValidator::clearParameters() {
    pImpl->parameters.clear();
    pImpl->errors.clear();
}

std::vector<ConfigEntry> ConfigValidator::getDefaultConfiguration() const {
    std::vector<ConfigEntry> entries;
    
    for (const auto& param : pImpl->parameters) {
        if (!param.defaultValue.empty()) {
            // Convert default value to ConfigValue based on type
            ConfigValue configValue;
            if (param.type == "int" || param.type == "int64" || param.type == "number") {
                try {
                    configValue = std::stod(param.defaultValue);
                } catch (...) {
                    continue;
                }
            } else if (param.type == "bool") {
                configValue = (param.defaultValue == "true" || param.defaultValue == "1");
            } else if (param.type == "string") {
                configValue = param.defaultValue;
            }
            
            if (!configValue.valueless_by_exception()) {
                entries.emplace_back(param.key, configValue, ConfigSource::Default, "Default value");
            }
        }
    }
    
    return entries;
}

} // namespace nlm
