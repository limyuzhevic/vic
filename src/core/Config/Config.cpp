} // namespace

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    initializeSchemaRegistry();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    try {
        std::string format = detectFileFormat(filepath);
        
        if (format == "json") {
            std::ifstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            nlohmann::json j;
            file >> j;
            
            if (j.is_object()) {
                for (auto it = j.begin(); it != j.end(); ++it) {
                    ConfigValue value = jsonToConfigValue(it.value());
                    
                    // Validate before setting
                    ValidationResult validationResult = validateValue(it.key(), value);
                    if (!validationResult.valid) {
                        // For backward compatibility, warn but continue
                        for (const auto& error : validationResult.errors) {
                            // Log warning but continue loading
                        }
                    }
                    
                    set(it.key(), value, ConfigSource::File);
                }
            }
        } else if (format == "yaml") {
            YAML::Node doc = YAML::LoadFile(filepath);
            
            for (const auto& node : doc) {
                if (node.IsMap()) {
                    for (const auto& pair : node) {
                        std::string key = pair.first.as<std::string>();
                        ConfigValue value = yamlToConfigValue(pair.second);
                        
                        // Validate before setting
                        ValidationResult validationResult = validateValue(key, value);
                        if (!validationResult.valid) {
                            // For backward compatibility, warn but continue
                            for (const auto& error : validationResult.errors) {
                                // Log warning but continue loading
                            }
                        }
                        
                        set(key, value, ConfigSource::File);
                    }
                } else if (node.IsScalar()) {
                    ConfigValue value = yamlToConfigValue(node);
                    std::string key = std::to_string(node.as<int>());
                    
                    // Validate before setting
                    ValidationResult validationResult = validateValue(key, value);
                    if (!validationResult.valid) {
                        // For backward compatibility, warn but continue
                        for (const auto& error : validationResult.errors) {
                            // Log warning but continue loading
                        }
                    }
                    
                    set(key, value, ConfigSource::File);
                }
            }
        } else {
            // Fallback to simple key=value format for backward compatibility
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
                    
                    // Convert value based on key
                    ConfigValue configValue;
                    if (key == "log_to_file") {
                        configValue = isValidBoolean(value) ? (toLower(value) == "true") : ConfigValue(std::string(""));
                    } else {
                        // Try to determine type based on content
                        bool isInt = true;
                        for (char c : value) {
                            if (!std::isdigit(c) && c != '-') {
                                isInt = false;
                                break;
                            }
                        }
                        if (isInt) {
                            configValue = std::stoi(value);
                        } else {
                            bool isDouble = false;
                            try {
                                size_t pos2;
                                double d = std::stod(value, &pos2);
                                if (pos2 == value.size()) {
                                    isDouble = true;
                                }
                            } catch (...) {}
                            if (isDouble) {
                                configValue = std::stod(value);
                            } else {
                                configValue = value;
                            }
                        }
                    }
                    
                    // Validate before setting
                    ValidationResult validationResult = validateValue(key, configValue);
                    if (!validationResult.valid) {
                        // For backward compatibility, warn but continue
                        for (const auto& error : validationResult.errors) {
                            // Log warning but continue loading
                        }
                    }
                    
                    set(key, configValue, ConfigSource::File);
                }
            }
        }
        
        // Check for duplicate keys and validate integrity
        ValidationResult integrityCheck = checkIntegrity(pImpl->entries);
        if (!integrityCheck.valid) {
            // For backward compatibility, log warnings but continue
            for (const auto& warning : integrityCheck.warnings) {
                // Log warning but continue
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        // Handle JSON/YAML parsing errors gracefully
        return false;
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
    try {
        // First, save to a JSON file for proper type support
        std::string jsonPath = filepath;
        if (jsonPath.substr(jsonPath.find_last_of(".") + 1) != "json") {
            jsonPath += ".json";
        }
        
        nlohmann::json j = nlohmann::json::object();
        for (const auto& entry : pImpl->entries) {
            j[entry.key] = configValueToJson(entry.value);
        }
        
        std::ofstream jsonFile(jsonPath);
        if (!jsonFile.is_open()) {
            return false;
        }
        
        jsonFile << j.dump(2);
        jsonFile.close();
        
        // Also save a simple key=value version for backward compatibility
        std::string simplePath = filepath;
        if (simplePath.find('.') != std::string::npos) {
            simplePath.erase(simplePath.find_last_of('.'));
        }
        simplePath += "_simple.txt";
        
        std::ofstream simpleFile(simplePath);
        if (simpleFile.is_open()) {
            for (const auto& entry : pImpl->entries) {
                simpleFile << "# " << entry.description << "\n";
                
                std::visit([&simpleFile](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, std::string>) {
                        simpleFile << entry.key << " = \"" << arg << "\"\n";
                    } else if constexpr (std::is_same_v<T, bool>) {
                        simpleFile << entry.key << " = " << (arg ? "true" : "false") << "\n";
                    } else if constexpr (std::is_same_v<T, std::vector<int64_t>> ||
                                        std::is_same_v<T, std::vector<int>> ||
                                        std::is_same_v<T, std::vector<double>> ||
                                        std::is_same_v<T, std::vector<std::string>>) {
                        simpleFile << entry.key << " = [";
                        bool first = true;
                        std::visit([&simpleFile, &first](auto&& arr) {
                            for (const auto& item : arr) {
                                if (!first) simpleFile << ", ";
                                if constexpr (std::is_same_v<decltype(item), std::string>) {
                                    simpleFile << "\"" << item << "\"";
                                } else {
                                    simpleFile << item;
                                }
                                first = false;
                            }
                        }, entry.value);
                        simpleFile << "]\n";
                    } else {
                        simpleFile << entry.key << " = " << arg << "\n";
                    }
                }, entry.value);
            }
            simpleFile.close();
        }
        
        return true;
    } catch (const std::exception& e) {
        // Handle file I/O errors gracefully
        return false;
    }
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

// Validation function implementations
ValidationResult Config::validate() const {
    ValidationResult result;
    
    // Check for duplicate keys
    ValidationResult duplicateCheck = checkKeyUniqueness(pImpl->entries);
    if (!duplicateCheck.valid) {
        result.errors.insert(result.errors.end(), 
                           duplicateCheck.errors.begin(), 
                           duplicateCheck.errors.end());
    }
    
    // Validate each key-value pair
    for (const auto& entry : pImpl->entries) {
        ValidationResult validationResult = validateKey(entry.key);
        if (!validationResult.valid) {
            for (const auto& error : validationResult.errors) {
                result.addError("Key \"" + entry.key + "\": " + error);
            }
        }
    }
    
    // Check integrity
    ValidationResult integrityCheck = checkIntegrity(pImpl->entries);
    if (!integrityCheck.valid) {
        result.errors.insert(result.errors.end(),
                           integrityCheck.errors.begin(),
                           integrityCheck.errors.end());
    }
    
    return result;
}

ValidationResult Config::validateKey(const std::string& key) const {
    auto configValue = get<ConfigValue>(key);
    return validateConfigValue(key, configValue.value_or(ConfigValue(std::string("")));
}

ValidationResult Config::validateValue(const std::string& key, const ConfigValue& value) {
    return validateConfigValue(key, value);
}

ValidationResult Config::validateAgainstSchema(const ConfigSchema& schema, const ConfigValue& value) {
    ValidationResult result;
    
    if (!schema.type.empty()) {
        if (schema.type == "int" && !(std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value))) {
            result.addError("Expected integer value");
        } else if (schema.type == "double" && !std::holds_alternative<double>(value)) {
            result.addError("Expected double value");
        } else if (schema.type == "bool" && !std::holds_alternative<bool>(value)) {
            result.addError("Expected boolean value");
        } else if (schema.type == "string" && !std::holds_alternative<std::string>(value)) {
            result.addError("Expected string value");
        } else if (schema.type == "vector<int>" && 
                  !(std::holds_alternative<std::vector<int>>(value) || 
                    std::holds_alternative<std::vector<int64_t>>(value))) {
            result.addError("Expected integer vector");
        } else if (schema.type == "vector<double>" && !std::holds_alternative<std::vector<double>>(value)) {
            result.addError("Expected double vector");
        } else if (schema.type == "vector<string>" && !std::holds_alternative<std::vector<std::string>>(value)) {
            result.addError("Expected string vector");
        }
    }
    
    return result;
}

bool Config::checkIntegrity(const std::vector<ConfigEntry>& entries) {
    return checkKeyUniqueness(entries).valid;
}

ValidationResult Config::checkKeyUniqueness(const std::vector<ConfigEntry>& entries) {
    ValidationResult result;
    std::unordered_map<std::string, int> keyCounts;
    
    for (const auto& entry : entries) {
        keyCounts[entry.key]++;
    }
    
    for (const auto& pair : keyCounts) {
        if (pair.second > 1) {
            result.addError("Duplicate key: \"" + pair.first + "\" (appears " + 
                         std::to_string(pair.second) + " times)");
        }
    }
    
    return result;
}

std::string Config::getLogLevelValue(const std::string& level) {
    std::string lowerLevel = toLower(level);
    if (lowerLevel == "debug" || lowerLevel == "info" || lowerLevel == "warning" || 
        lowerLevel == "error" || lowerLevel == "critical") {
        return lowerLevel;
    }
    return "info"; // Default
}

bool Config::isValidBoolean(const std::string& value) {
    std::string lowerValue = toLower(value);
    return lowerValue == "true" || lowerValue == "false";
}

// Private validation implementation
ValidationResult Config::validateConfigValue(const std::string& key, const ConfigValue& value) {
    ValidationResult result;
    
    // First check type using variant type_info
    if (std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value)) {
        ValidationResult intResult = validateInt(key, value);
        result.errors.insert(result.errors.end(), intResult.errors.begin(), intResult.errors.end());
        result.warnings.insert(result.warnings.end(), intResult.warnings.begin(), intResult.warnings.end());
    } else if (std::holds_alternative<double>(value)) {
        ValidationResult doubleResult = validateDouble(key, value);
        result.errors.insert(result.errors.end(), doubleResult.errors.begin(), doubleResult.errors.end());
        result.warnings.insert(result.warnings.end(), doubleResult.warnings.begin(), doubleResult.warnings.end());
    } else if (std::holds_alternative<bool>(value)) {
        ValidationResult boolResult = validateBool(key, value);
        result.errors.insert(result.errors.end(), boolResult.errors.begin(), boolResult.errors.end());
        result.warnings.insert(result.warnings.end(), boolResult.warnings.begin(), boolResult.warnings.end());
    } else if (std::holds_alternative<std::string>(value)) {
        ValidationResult stringResult = validateString(key, value);
        result.errors.insert(result.errors.end(), stringResult.errors.begin(), stringResult.errors.end());
        result.warnings.insert(result.warnings.end(), stringResult.warnings.begin(), stringResult.warnings.end());
    } else if (std::holds_alternative<std::vector<int>>(value) || 
               std::holds_alternative<std::vector<int64_t>>(value)) {
        ValidationResult intArrayResult = validateIntArray(key, value);
        result.errors.insert(result.errors.end(), intArrayResult.errors.begin(), intArrayResult.errors.end());
        result.warnings.insert(result.warnings.end(), intArrayResult.warnings.begin(), intArrayResult.warnings.end());
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        ValidationResult doubleArrayResult = validateDoubleArray(key, value);
        result.errors.insert(result.errors.end(), doubleArrayResult.errors.begin(), doubleArrayResult.errors.end());
        result.warnings.insert(result.warnings.end(), doubleArrayResult.warnings.begin(), doubleArrayResult.warnings.end());
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        ValidationResult stringArrayResult = validateStringArray(key, value);
        result.errors.insert(result.errors.end(), stringArrayResult.errors.begin(), stringArrayResult.errors.end());
        result.warnings.insert(result.warnings.end(), stringArrayResult.warnings.begin(), stringArrayResult.warnings.end());
    } else {
        result.addError("Unsupported value type for key: " + key);
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

void Config::initializeSchemaRegistry() {
    // Initialize default schema registry with known configuration keys and their validation rules
    
    // Neuron count validation
    {
        ConfigSchema schema;
        schema.key = "neuron_count";
        schema.type = "int";
        schema.description = "Number of neurons in the brain";
        schema.required = true;
        schema.min_int = 1;
        schema.max_int = 10000000;
        ConfigSchemaRegistry::getInstance().registerSchema(schema);
    }
    
    // Simulation timestep validation
    {
        ConfigSchema schema;
        schema.key = "simulation_timestep";
        schema.type = "double";
        schema.description = "Simulation timestep in seconds";
        schema.required = true;
        schema.min_double = 0.0;
        schema.max_double = 1.0;
        ConfigSchemaRegistry::getInstance().registerSchema(schema);
    }
    
    // Connection probability validation
    {
        ConfigSchema schema;
        schema.key = "connection_probability";
        schema.type = "double";
        schema.description = "Connection probability for random connectivity";
        schema.required = true;
        schema.min_double = 0.0;
        schema.max_double = 1.0;
        ConfigSchemaRegistry::getInstance().registerSchema(schema);
    }
    
    // Log level validation
    {
        ConfigSchema schema;
        schema.key = "log_level";
        schema.type = "string";
        schema.description = "Log level for logging";
        schema.required = true;
        schema.allowed_values = {"debug", "info", "warning", "error", "critical"};
        ConfigSchemaRegistry::getInstance().registerSchema(schema);
    }
    
    // Boolean values validation
    {
        ConfigSchema schema;
        schema.key = "log_to_file";
        schema.type = "bool";
        schema.description = "Whether to log to file";
        schema.required = false;
        ConfigSchemaRegistry::getInstance().registerSchema(schema);
    }
    
    // Additional validation for specific keys
    {
        ConfigSchema schema;
        schema.key = "region_count";
        schema.type = "int";
        schema.description = "Number of brain regions";
        schema.required = true;
        schema.min_int = 1;
        schema.max_int = 1000;
        ConfigSchemaRegistry::getInstance().registerSchema(schema);
    }
    
    {
        ConfigSchema schema;
        schema.key = "plasticity_learning_rate";
        schema.type = "double";
        schema.description = "Plasticity learning rate";
        schema.required = false;
        schema.min_double = 0.0;
        schema.max_double = 1.0;
        ConfigSchemaRegistry::getInstance().registerSchema(schema);
    }
    
    {
        ConfigSchema schema;
        schema.key = "stdp_ltp_weight";
        schema.type = "double";
        schema.description = "STDP LTP weight";
        schema.required = false;
        schema.min_double = 0.0;
        schema.max_double = 0.1;
        ConfigSchemaRegistry::getInstance().registerSchema(schema);
    }
    
    {
        ConfigSchema schema;
        schema.key = "stdp_ltd_weight";
        schema.type = "double";
        schema.description = "STDP LTD weight";
        schema.required = false;
        schema.min_double = 0.0;
        schema.max_double = 0.12;
        ConfigSchemaRegistry::getInstance().registerSchema(schema);
    }
    
    {
        ConfigSchema schema;
        schema.key = "stdp_time_constant";
        schema.type = "double";
        schema.description = "STDP time constant";
        schema.required = false;
        schema.min_double = 1.0;
        schema.max_double = 100.0;
        ConfigSchemaRegistry::getInstance().registerSchema(schema);
    }
    
    {
        ConfigSchema schema;
        schema.key = "development_synaptogenesis_rate";
        schema.type = "double";
        schema.description = "Development synaptogenesis rate";
        schema.required = false;
        schema.min_double = 0.0;
        schema.max_double = 0.001;
        ConfigSchemaRegistry::getInstance().registerSchema(schema);
    }
    
    {
        ConfigSchema schema;
        schema.key = "development_pruning_rate";
        schema.type = "double";
        schema.description = "Development pruning rate";
        schema.required = false;
        schema.min_double = 0.0;
        schema.max_double = 0.0001;
        ConfigSchemaRegistry::getInstance().registerSchema(schema);
    }
    
    {
        ConfigSchema schema;
        schema.key = "reward_discount_factor";
        schema.type = "double";
        schema.description = "Reward discount factor";
        schema.required = false;
        schema.min_double = 0.0;
        schema.max_double = 1.0;
        ConfigSchemaRegistry::getInstance().registerSchema(schema);
    }
    
    {
        ConfigSchema schema;
        schema.key = "environment_width";
        schema.type = "double";
        schema.description = "Environment width";
        schema.required = false;
        schema.min_double = 1.0;
        schema.max_double = 1000.0;
        ConfigSchemaRegistry::getInstance().registerSchema(schema);
    }
    
    {
        ConfigSchema schema;
        schema.key = "environment_height";
        schema.type = "double";
        schema.description = "Environment height";
        schema.required = false;
        schema.min_double = 1.0;
        schema.max_double = 1000.0;
        ConfigSchemaRegistry::getInstance().registerSchema(schema);
    }
    
    {
        ConfigSchema schema;
        schema.key = "visualization_update_rate";
        schema.type = "double";
        schema.description = "Visualization update rate";
        schema.required = false;
        schema.min_double = 1.0;
        schema.max_double = 60.0;
        ConfigSchemaRegistry::getInstance().registerSchema(schema);
    }
    
    {
        ConfigSchema schema;
        schema.key = "max_simulation_steps";
        schema.type = "int";
        schema.description = "Maximum simulation steps";
        schema.required = false;
        schema.min_int = 1;
        schema.max_int = 1000000;
        ConfigSchemaRegistry::getInstance().registerSchema(schema);
    }
    
    {
        ConfigSchema schema;
        schema.key = "environment_name";
        schema.type = "string";
        schema.description = "Environment name";
        schema.required = false;
        ConfigSchemaRegistry::getInstance().registerSchema(schema);
    }
    
    {
        ConfigSchema schema;
        schema.key = "log_filename";
        schema.type = "string";
        schema.description = "Log filename";
        schema.required = false;
        ConfigSchemaRegistry::getInstance().registerSchema(schema);
    }
}

} // namespace nlm