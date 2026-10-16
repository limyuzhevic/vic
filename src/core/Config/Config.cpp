// Parameter validation helper implementations
ValidationFunction config_validation::createFloatRangeValidator(float min, float max) {
    // Create a validation function that checks if a float value is within the specified range
    // Args:
    //   min: Minimum allowed value (inclusive)
    //   max: Maximum allowed value (inclusive)
    // Returns: Validation function that checks range
    return [min, max](const ConfigValue& value) -> bool {
        try {
            float val = std::get<float>(value);
            return val >= min && val <= max;
        } catch (const std::bad_variant_access&) {
            return false;
        }
    };
}

ValidationFunction config_validation::createIntRangeValidator(int min, int max) {
    // Create a validation function that checks if an integer value is within the specified range
    // Args:
    //   min: Minimum allowed value (inclusive)
    //   max: Maximum allowed value (inclusive)
    // Returns: Validation function that checks range
    return [min, max](const ConfigValue& value) -> bool {
        try {
            int val = std::get<int>(value);
            return val >= min && val <= max;
        } catch (const std::bad_variant_access&) {
            try {
                int64_t val = std::get<int64_t>(value);
                return val >= min && val <= max;
            } catch (const std::bad_variant_access&) {
                return false;
            }
        }
    };
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // Parameter metadata and validation
    std::unordered_map<std::string, ParameterMeta> parameters;
    std::unordered_map<std::string, std::string> sectionMap;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize parameter metadata with default values and validators
    // This establishes the complete set of configurable parameters for the simulation
    // Neuron parameters
    NEURON_MEMBRANE_CAPACITANCE = ParameterMeta(
        "neuron.membrane_capacitance", 
        "Membrane capacitance (nF)", 
        1.0f,
        config_validation::createFloatRangeValidator(0.1f, 10.0f)
    );
    
    NEURON_TIME_CONSTANT = ParameterMeta(
        "neuron.time_constant",
        "Membrane time constant (ms)",
        20.0f,
        config_validation::createFloatRangeValidator(1.0f, 100.0f)
    );
    
    NEURON_MAX_SPIKE_HISTORY = ParameterMeta(
        "neuron.max_spike_history",
        "Maximum spike history entries",
        100,
        config_validation::createIntRangeValidator(10, 1000)
    );
    
    NEURON_RESTING_POTENTIAL_DEFAULT = ParameterMeta(
        "neuron.resting_potential_default",
        "Default resting potential (mV)",
        -70.0f,
        config_validation::createFloatRangeValidator(-90.0f, -50.0f)
    );
    
    NEURON_THRESHOLD_DEFAULT = ParameterMeta(
        "neuron.threshold_default",
        "Default firing threshold (mV)",
        -55.0f,
        config_validation::createFloatRangeValidator(-80.0f, -40.0f)
    );
    
    NEURON_RESET_POTENTIAL_DEFAULT = ParameterMeta(
        "neuron.reset_potential_default",
        "Default reset potential after spike (mV)",
        -70.0f,
        config_validation::createFloatRangeValidator(-90.0f, -50.0f)
    );
    
    NEURON_LEAK_CONDUCTANCE_DEFAULT = ParameterMeta(
        "neuron.leak_conductance_default",
        "Default leak conductance (nS)",
        10.0f,
        config_validation::createFloatRangeValidator(1.0f, 100.0f)
    );
    
    NEURON_ADAPTATION_RATE = ParameterMeta(
        "neuron.adaptation_rate",
        "Spike-frequency adaptation rate",
        0.01f,
        config_validation::createFloatRangeValidator(0.001f, 0.1f)
    );
    
    NEURON_REFRACTORY_PERIOD_DEFAULT = ParameterMeta(
        "neuron.refractory_period_default",
        "Default refractory period (steps)",
        5,
        config_validation::createIntRangeValidator(1, 50)
    );
    
    // Synapse parameters
    SYNAPSE_WEIGHT_MIN = ParameterMeta(
        "synapse.weight_min",
        "Minimum synaptic weight",
        -1.0f,
        config_validation::createFloatRangeValidator(-10.0f, 0.0f)
    );
    
    SYNAPSE_WEIGHT_MAX = ParameterMeta(
        "synapse.weight_max",
        "Maximum synaptic weight",
        1.0f,
        config_validation::createFloatRangeValidator(0.0f, 10.0f)
    );
    
    SYNAPSE_STP_FACILITATION_TAU = ParameterMeta(
        "synapse.stp_facilitation_tau",
        "Short-term plasticity facilitation time constant (ms)",
        100.0f,
        config_validation::createFloatRangeValidator(10.0f, 500.0f)
    );
    
    SYNAPSE_STP_DEPRESSION_TAU = ParameterMeta(
        "synapse.stp_depression_tau",
        "Short-term plasticity depression time constant (ms)",
        200.0f,
        config_validation::createFloatRangeValidator(50.0f, 500.0f)
    );
    
    SYNAPSE_STP_U_MAX = ParameterMeta(
        "synapse.stp_u_max",
        "Short-term plasticity maximum utilization",
        1.0f,
        config_validation::createFloatRangeValidator(0.1f, 5.0f)
    );
    
    SYNAPSE_MAX_SPIKE_HISTORY = ParameterMeta(
        "synapse.max_spike_history",
        "Maximum spike history entries",
        100,
        config_validation::createIntRangeValidator(10, 1000)
    );
    
    SYNAPSE_EFFICIENCY_RANGE_MIN = ParameterMeta(
        "synapse.efficiency_range_min",
        "Synaptic efficacy minimum range",
        0.5f,
        config_validation::createFloatRangeValidator(0.1f, 1.0f)
    );
    
    SYNAPSE_EFFICIENCY_RANGE_MAX = ParameterMeta(
        "synapse.efficiency_range_max",
        "Synaptic efficacy maximum range",
        2.0f,
        config_validation::createFloatRangeValidator(1.0f, 5.0f)
    );
    
    // Register all parameters in the parameters map
    pImpl->parameters[NEURON_MEMBRANE_CAPACITANCE.name] = NEURON_MEMBRANE_CAPACITANCE;
    pImpl->parameters[NEURON_TIME_CONSTANT.name] = NEURON_TIME_CONSTANT;
    pImpl->parameters[NEURON_MAX_SPIKE_HISTORY.name] = NEURON_MAX_SPIKE_HISTORY;
    pImpl->parameters[NEURON_RESTING_POTENTIAL_DEFAULT.name] = NEURON_RESTING_POTENTIAL_DEFAULT;
    pImpl->parameters[NEURON_THRESHOLD_DEFAULT.name] = NEURON_THRESHOLD_DEFAULT;
    pImpl->parameters[NEURON_RESET_POTENTIAL_DEFAULT.name] = NEURON_RESET_POTENTIAL_DEFAULT;
    pImpl->parameters[NEURON_LEAK_CONDUCTANCE_DEFAULT.name] = NEURON_LEAK_CONDUCTANCE_DEFAULT;
    pImpl->parameters[NEURON_ADAPTATION_RATE.name] = NEURON_ADAPTATION_RATE;
    pImpl->parameters[NEURON_REFRACTORY_PERIOD_DEFAULT.name] = NEURON_REFRACTORY_PERIOD_DEFAULT;
    
    pImpl->parameters[SYNAPSE_WEIGHT_MIN.name] = SYNAPSE_WEIGHT_MIN;
    pImpl->parameters[SYNAPSE_WEIGHT_MAX.name] = SYNAPSE_WEIGHT_MAX;
    pImpl->parameters[SYNAPSE_STP_FACILITATION_TAU.name] = SYNAPSE_STP_FACILITATION_TAU;
    pImpl->parameters[SYNAPSE_STP_DEPRESSION_TAU.name] = SYNAPSE_STP_DEPRESSION_TAU;
    pImpl->parameters[SYNAPSE_STP_U_MAX.name] = SYNAPSE_STP_U_MAX;
    pImpl->parameters[SYNAPSE_MAX_SPIKE_HISTORY.name] = SYNAPSE_MAX_SPIKE_HISTORY;
    pImpl->parameters[SYNAPSE_EFFICIENCY_RANGE_MIN.name] = SYNAPSE_EFFICIENCY_RANGE_MIN;
    pImpl->parameters[SYNAPSE_EFFICIENCY_RANGE_MAX.name] = SYNAPSE_EFFICIENCY_RANGE_MAX;
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Load configuration from a file (simple key=value format)
    // Args:
    //   filepath: Path to configuration file
    // Returns: true if file was loaded successfully, false otherwise
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
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
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    // Load configuration from command line arguments
    // Args:
    //   argc: Number of command line arguments
    //   argv: Array of command line argument strings
    // Returns: true if arguments were parsed successfully, false otherwise
    
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
    // Save configuration to a file
    // Args:
    //   filepath: Path to output configuration file
    // Returns: true if file was saved successfully, false otherwise
    
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
    // Get a configuration value by key, with type specification
    // Args:
    //   key: Configuration parameter key (e.g., "brain.neuron_count")
    // Returns: Optional containing the value if found, empty otherwise
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
    // Get a configuration value by key, or return a default if not found
    // Args:
    //   key: Configuration parameter key
    //   defaultValue: Value to return if key is not found
    // Returns: Value from configuration or default
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Set a configuration value
    // Args:
    //   key: Configuration parameter key
    //   value: Value to set (can be string, int, double, bool, or vector)
    //   source: Source of the configuration value (File, CommandLine, Runtime)
    // Throws: std::runtime_error if parameter validation fails
    
    // Check if this is a parameter with validation
    auto paramIt = pImpl->parameters.find(key);
    if (paramIt != pImpl->parameters.end()) {
        const ParameterMeta& meta = paramIt->second;
        if (!validateParameterInternal(key, value, meta)) {
            throw std::runtime_error("Configuration validation failed for key: " + key);
        }
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        // Try to find parameter description
        std::string description = "";
        if (paramIt != pImpl->parameters.end()) {
            description = paramIt->second.description;
        }
        pImpl->entries.emplace_back(key, value, source, description);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    // Set a string configuration value
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    // Set an int configuration value
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    // Set a double configuration value
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    // Set a bool configuration value
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    // Check if a configuration parameter exists
    // Args:
    //   key: Configuration parameter key to check
    // Returns: true if parameter exists, false otherwise
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    // Remove a configuration parameter
    // Args:
    //   key: Configuration parameter key to remove
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

std::vector<std::string> Config::getKeys() const {
    // Get all configuration parameter keys
    // Returns: Vector of all configuration keys
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    // Clear all configuration parameters
    // Removes all entries from the configuration
    pImpl->entries.clear();
}

std::string Config::summary() const {
    // Generate a human-readable summary of all configuration parameters
    // Returns: String containing configuration summary
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

void Config::registerParameter(const std::string& section, const ParameterMeta& param) {
    // Register a new configuration parameter
    // Args:
    //   section: Configuration section name
    //   param: Parameter metadata with name, description, default value, and validator
    std::string fullKey = section + "." + param.name;
    pImpl->parameters[fullKey] = param;
    pImpl->sectionMap[param.name] = section;
}

void Config::setParameter(const std::string& section, const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Set a configuration parameter within a specific section
    // Args:
    //   section: Configuration section name
    //   key: Parameter key within the section
    //   value: Parameter value
    //   source: Source of the configuration value
    std::string fullKey = section + "." + key;
    
    // Check if this is a registered parameter with validation
    auto paramIt = pImpl->parameters.find(fullKey);
    if (paramIt != pImpl->parameters.end()) {
        const ParameterMeta& meta = paramIt->second;
        if (!validateParameterInternal(fullKey, value, meta)) {
            throw std::runtime_error("Configuration validation failed for key: " + fullKey);
        }
    }
    
    // Set the value using the existing set method
    set(fullKey, value, source);
}

template<typename T>
std::optional<T> Config::getParameter(const std::string& section, const std::string& key) const {
    // Get a configuration parameter from a specific section
    // Args:
    //   section: Configuration section name
    //   key: Parameter key within the section
    // Returns: Optional containing the value if found, empty otherwise
    std::string fullKey = section + "." + key;
    return get<T>(fullKey);
}

template<typename T>
T Config::getParameterOr(const std::string& section, const std::string& key, const T& defaultValue) const {
    // Get a configuration parameter from a specific section, or return default if not found
    // Args:
    //   section: Configuration section name
    //   key: Parameter key within the section
    //   defaultValue: Value to return if parameter not found
    // Returns: Value from configuration or default
    std::string fullKey = section + "." + key;
    return getOr<T>(fullKey, defaultValue);
}

bool Config::validateParameter(const std::string& section, const std::string& key, const ConfigValue& value) const {
    // Validate a configuration parameter within a specific section
    // Args:
    //   section: Configuration section name
    //   key: Parameter key within the section
    //   value: Value to validate
    // Returns: true if parameter is valid, false otherwise
    std::string fullKey = section + "." + key;
    return validateParameterInternal(fullKey, value, pImpl->parameters.at(fullKey));
}

bool Config::validateParameterInternal(const std::string& key, const ConfigValue& value, const ParameterMeta& meta) const {
    // Internal parameter validation function
    // Args:
    //   key: Parameter key
    //   value: Value to validate
    //   meta: Parameter metadata containing validator
    // Returns: true if parameter is valid, false otherwise
    // Check if value is within bounds
    if (meta.validator) {
        return meta.validator(value);
    }
    return true;
}

void Config::ensureRequiredParameters(const std::string& section) const {
    // Ensure all required parameters in a section are present
    // Args:
    //   section: Configuration section name
    // Throws: std::runtime_error if required parameter is missing
    for (const auto& paramPair : pImpl->parameters) {
        if (paramPair.second.required) {
            std::string sectionPart = pImpl->sectionMap[paramPair.first];
            if (sectionPart == section) {
                std::string key = paramPair.first.substr(paramPair.first.find('.') + 1);
                if (!has(paramPair.first)) {
                    throw std::runtime_error("Required configuration parameter missing: " + paramPair.first);
                }
            }
        }
    }
}

bool Config::isInRange(const ConfigValue& value, const ConfigValue& min, const ConfigValue& max) {
    // Check if a configuration value is within specified range
    // Args:
    //   value: Value to check
    //   min: Minimum value (inclusive)
    //   max: Maximum value (inclusive)
    // Returns: true if value is within range, false otherwise
    try {
        double val = std::get<double>(value);
        double minVal = std::get<double>(min);
        double maxVal = std::get<double>(max);
        return val >= minVal && val <= maxVal;
    } catch (const std::bad_variant_access&) {
        return false;
    }
}

// Static parameter definitions
ParameterMeta Config::NEURON_MEMBRANE_CAPACITANCE;
ParameterMeta Config::NEURON_TIME_CONSTANT;
ParameterMeta Config::NEURON_MAX_SPIKE_HISTORY;
ParameterMeta Config::NEURON_RESTING_POTENTIAL_DEFAULT;
ParameterMeta Config::NEURON_THRESHOLD_DEFAULT;
ParameterMeta Config::NEURON_RESET_POTENTIAL_DEFAULT;
ParameterMeta Config::NEURON_LEAK_CONDUCTANCE_DEFAULT;
ParameterMeta Config::NEURON_ADAPTATION_RATE;
ParameterMeta Config::NEURON_REFRACTORY_PERIOD_DEFAULT;

ParameterMeta Config::SYNAPSE_WEIGHT_MIN;
ParameterMeta Config::SYNAPSE_WEIGHT_MAX;
ParameterMeta Config::SYNAPSE_STP_FACILITATION_TAU;
ParameterMeta Config::SYNAPSE_STP_DEPRESSION_TAU;
ParameterMeta Config::SYNAPSE_STP_U_MAX;
ParameterMeta Config::SYNAPSE_MAX_SPIKE_HISTORY;
ParameterMeta Config::SYNAPSE_EFFICIENCY_RANGE_MIN;
ParameterMeta Config::SYNAPSE_EFFICIENCY_RANGE_MAX;

} // namespace nlm
