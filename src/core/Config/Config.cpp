#include "Logger/Logger.hpp"

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

struct ConfigSchema::Impl {
    std::vector<ConfigSchema> schemas;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

ConfigSchema::ConfigSchema() : pImpl(std::make_unique<Impl>()) {}

ConfigSchema::~ConfigSchema() = default;

ConfigSchema::ConfigSchema(ConfigSchema&&) noexcept = default;
ConfigSchema& ConfigSchema::operator=(ConfigSchema&&) noexcept = default;

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
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
    // First check for special flags
    bool showHelp = false;
    bool validateConfig = false;
    bool showVersion = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            showHelp = true;
        } else if (arg == "--validate") {
            validateConfig = true;
        } else if (arg == "--version") {
            showVersion = true;
        }
    }
    
    // If help, version, or validate requested, handle them
    if (showHelp) {
        std::cout << help() << std::endl;
        return true;
    }
    
    if (showVersion) {
        std::cout << "NLM Phase 2 Real Neural Computation" << std::endl;
        std::cout << "Version: 2.0" << std::endl;
        std::cout << "Build: Release" << std::endl;
        return true;
    }
    
    if (validateConfig) {
        if (validate()) {
            std::cout << "Configuration validation passed." << std::endl;
        } else {
            std::cout << "Configuration validation failed." << std::endl;
        }
        return true;
    }
    
    // Continue with normal argument parsing
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

bool Config::loadFromArgsWithValidation(int argc, char** argv) {
    // Get default schema for validation
    auto schema = ConfigSchema::getDefaultSchema();
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Find the schema entry for this key
                auto schemaEntry = std::find_if(schema.begin(), schema.end(),
                    [&key](const ConfigSchema& s) { return s.key == key; });
                
                if (schemaEntry != schema.end()) {
                    // Validate the value against the schema
                    ConfigValue parsedValue;
                    if (ConfigSchemaValidator::stringToValue(schemaEntry->type, value, parsedValue)) {
                        set(key, parsedValue, ConfigSource::CommandLine);
                    } else {
                        NLM_LOG_WARNING("Invalid value for " + key + ": " + value + ". Expected type: " + schemaEntry->type);
                    }
                } else {
                    NLM_LOG_WARNING("Unknown configuration key: " + key);
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Find the schema entry for this key
            auto schemaEntry = std::find_if(schema.begin(), schema.end(),
                [&key](const ConfigSchema& s) { return s.key == key; });
            
            if (schemaEntry != schema.end()) {
                // Validate the value against the schema
                ConfigValue parsedValue;
                if (ConfigSchemaValidator::stringToValue(schemaEntry->type, value, parsedValue)) {
                    set(key, parsedValue, ConfigSource::CommandLine);
                } else {
                    NLM_LOG_WARNING("Invalid value for " + key + ": " + value + ". Expected type: " + schemaEntry->type);
                }
            } else {
                NLM_LOG_WARNING("Unknown configuration key: " + key);
            }
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
        file << entry.key << " = ";
        
        // Helper to convert ConfigValue to string
        auto valueToString = [&file](const ConfigValue& val) {
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else if constexpr (std::is_same_v<T, int> || 
                                   std::is_same_v<T, int64_t> || 
                                   std::is_same_v<T, double>) {
                    file << arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    file << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << arg[i];
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    file << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << arg[i];
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    file << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << "\"" << arg[i] << "\"";
                    }
                    file << "]";
                }
            }, val);
        };
        
        valueToString(entry.value);
        file << "\n\n";
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

std::string Config::help() const {
    std::ostringstream oss;
    oss << "NLM Configuration Options:\n\n";
    
    auto schema = ConfigSchema::getDefaultSchema();
    oss << "Available configuration parameters:\n\n";
    
    // Group by category
    std::map<std::string, std::vector<ConfigSchema>> categories;
    
    for (const auto& param : schema) {
        std::string category;
        if (param.key.find("random_") == 0) {
            category = "Random Number Generation";
        } else if (param.key.find("simulation_") == 0) {
            category = "Simulation";
        } else if (param.key.find("neuron_") == 0 || 
                  param.key.find("region_") == 0 || 
                  param.key.find("connection_") == 0) {
            category = "Brain Architecture";
        } else if (param.key.find("plasticity_") == 0 || 
                  param.key.find("stdp_") == 0) {
            category = "Plasticity";
        } else if (param.key.find("development_") == 0) {
            category = "Development";
        } else if (param.key.find("dopamine_") == 0) {
            category = "Neuromodulation";
        } else if (param.key.find("reward_") == 0) {
            category = "Reward Processing";
        } else if (param.key.find("environment_") == 0) {
            category = "Environment";
        } else if (param.key.find("log_") == 0) {
            category = "Logging";
        } else if (param.key.find("simulation_") == 0) {
            category = "Simulation";
        } else if (param.key.find("visualization_") == 0) {
            category = "Visualization";
        } else {
            category = "Other";
        }
        
        categories[category].push_back(param);
    }
    
    // Print by category
    for (const auto& [category, params] : categories) {
        oss << category << ":\n";
        for (const auto& param : params) {
            oss << "  " << param.key << " <" << param.type << ">"; 
            if (!param.description.empty()) {
                oss << " - " << param.description;
            }
            oss << "\n";
            
            // Show default value
            oss << "    Default: " << ConfigSchemaValidator::valueToString(param.defaultValue) << "\n";
        }
        oss << "\n";
    }
    
    oss << "Command-line usage:\n";
    oss << "  nlm [options]\n";
    oss << "  Options:\n";
    oss << "    --config <file>     Load configuration from file\n";
    oss << "    --key <value>       Set configuration value (e.g., --random_seed 42)\n";
    oss << "    --help               Show this help message\n";
    oss << "    --version            Show version information\n";
    oss << "    --validate           Validate current configuration\n";
    oss << "\n";
    oss << "File format (simple key=value or JSON):\n";
    oss << "  key = value\n";
    oss << "  key = \"value with spaces\"\n";
    oss << "  key = 123\n";
    oss << "  key = 3.14\n";
    oss << "  key = true\n";
    oss << "\n";
    
    return oss.str();
}

bool Config::validate() const {
    auto schema = ConfigSchema::getDefaultSchema();
    return validateAgainstSchema(schema);
}

std::string Config::getUsage() const {
    return help();
}

bool Config::validateAgainstSchema(const ConfigSchema& schema) const {
    // Check if all values in the schema have corresponding entries in the config
    for (const auto& schemaEntry : schema) {
        if (!has(schemaEntry.key)) {
            NLM_LOG_WARN("Missing configuration: " + schemaEntry.key);
            return false;
        }
    }
    return true;
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
// Schema validation implementation
namespace nlm {

struct ConfigSchema::Impl {
    std::vector<ConfigSchema> schemas;
};

std::vector<ConfigSchema> ConfigSchema::getDefaultSchema() {
    std::vector<ConfigSchema> schemas;
    
    // Add all standard configuration parameters with their defaults and descriptions
    schemas.push_back(ConfigSchema("random_seed", "int64_t", int64_t(42),
        "Random seed for reproducible experiments"));
    
    schemas.push_back(ConfigSchema("simulation_timestep", "double", 0.001,
        "Simulation timestep in seconds"));
    
    schemas.push_back(ConfigSchema("neuron_count", "int64_t", int64_t(1000),
        "Number of neurons in the brain"));
    
    schemas.push_back(ConfigSchema("region_count", "int64_t", int64_t(2),
        "Number of brain regions"));
    
    schemas.push_back(ConfigSchema("connection_probability", "double", 0.1,
        "Connection probability for random connectivity"));
    
    // Plasticity settings
    schemas.push_back(ConfigSchema("plasticity_learning_rate", "double", 0.01,
        "General plasticity learning rate"));
    
    schemas.push_back(ConfigSchema("stdp_ltp_weight", "double", 0.01,
        "STDP long-term potentiation weight"));
    
    schemas.push_back(ConfigSchema("stdp_ltd_weight", "double", 0.012,
        "STDP long-term depression weight"));
    
    schemas.push_back(ConfigSchema("stdp_time_constant", "double", 20.0,
        "STDP time constant in steps"));
    
    // Development settings
    schemas.push_back(ConfigSchema("development_synaptogenesis_rate", "double", 0.001,
        "Synaptogenesis rate during development"));
    
    schemas.push_back(ConfigSchema("development_pruning_rate", "double", 0.0001,
        "Pruning rate during development"));
    
    // Neuromodulation settings
    schemas.push_back(ConfigSchema("dopamine_baseline", "double", 0.0,
        "Baseline dopamine level"));
    
    schemas.push_back(ConfigSchema("dopamine_curiosity", "double", 0.1,
        "Curiosity parameter for dopamine"));
    
    // Reward settings
    schemas.push_back(ConfigSchema("reward_discount_factor", "double", 0.99,
        "Discount factor for reward prediction"));
    
    // Environment settings
    schemas.push_back(ConfigSchema("environment_name", "string", std::string("GridWorld"),
        "Environment name"));
    
    schemas.push_back(ConfigSchema("environment_width", "int64_t", int64_t(10),
        "Environment width in grid cells"));
    
    schemas.push_back(ConfigSchema("environment_height", "int64_t", int64_t(10),
        "Environment height in grid cells"));
    
    // Logging settings
    schemas.push_back(ConfigSchema("log_level", "string", std::string("INFO"),
        "Log level (DEBUG, INFO, WARN, ERROR, FATAL)"));
    
    schemas.push_back(ConfigSchema("log_to_file", "bool", bool(false),
        "Whether to log to file"));
    
    schemas.push_back(ConfigSchema("log_filename", "string", std::string("nlm.log"),
        "Log filename when log_to_file is true"));
    
    // Simulation settings
    schemas.push_back(ConfigSchema("max_simulation_steps", "int64_t", int64_t(10000),
        "Maximum number of simulation steps"));
    
    schemas.push_back(ConfigSchema("simulation_time_limit", "double", 0.0,
        "Time limit in seconds (0 = unlimited)"));
    
    // Visualization settings
    schemas.push_back(ConfigSchema("visualization_enabled", "bool", bool(false),
        "Whether visualization is enabled"));
    
    schemas.push_back(ConfigSchema("visualization_update_rate", "double", 30.0,
        "Visualization update rate in Hz"));
    
    return schemas;
}

bool ConfigSchemaValidator::validate(const ConfigSchema& schema, const std::string& key, const ConfigValue& value) {
    // For now, basic validation: just check that the key matches
    return schema.key == key;
}

std::string ConfigSchemaValidator::valueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, int> || 
                           std::is_same_v<T, int64_t> || 
                           std::is_same_v<T, double>) {
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
        }
    }, value);
    return oss.str();
}

bool ConfigSchemaValidator::stringToValue(const std::string& type, const std::string& str, ConfigValue& outValue) {
    // Convert string to ConfigValue based on type
    if (type == "int64_t") {
        try {
            size_t pos;
            int64_t val = std::stoll(str, &pos);
            if (pos == str.size()) {
                outValue = val;
                return true;
            }
        } catch (...) {
        }
    } else if (type == "double") {
        try {
            size_t pos;
            double val = std::stod(str, &pos);
            if (pos == str.size()) {
                outValue = val;
                return true;
            }
        } catch (...) {
        }
    } else if (type == "string") {
        // Remove surrounding quotes if present
        std::string val = str;
        if (val.size() >= 2 && ((val.front() == '\"' && val.back() == '\"') ||
                               (val.front() == '\'' && val.back() == '\''))) {
            val = val.substr(1, val.size() - 2);
        }
        outValue = val;
        return true;
    } else if (type == "bool") {
        std::string lower = str;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        if (lower == "true" || lower == "1") {
            outValue = true;
            return true;
        } else if (lower == "false" || lower == "0") {
            outValue = false;
            return true;
        }
    }
    return false;
}

// Helper functions for argument parsing
namespace {
    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
}

// ConfigSchema implementation

std::vector<ConfigSchema> ConfigSchema::getDefaultSchema() {
    std::vector<ConfigSchema> schemas;
    
    // Add all standard configuration parameters with their defaults and descriptions
    schemas.push_back(ConfigSchema("random_seed", "int64_t", int64_t(42),
        "Random seed for reproducible experiments"));
    
    schemas.push_back(ConfigSchema("simulation_timestep", "double", 0.001,
        "Simulation timestep in seconds"));
    
    schemas.push_back(ConfigSchema("neuron_count", "int64_t", int64_t(1000),
        "Number of neurons in the brain"));
    
    schemas.push_back(ConfigSchema("region_count", "int64_t", int64_t(2),
        "Number of brain regions"));
    
    schemas.push_back(ConfigSchema("connection_probability", "double", 0.1,
        "Connection probability for random connectivity"));
    
    // Plasticity settings
    schemas.push_back(ConfigSchema("plasticity_learning_rate", "double", 0.01,
        "General plasticity learning rate"));
    
    schemas.push_back(ConfigSchema("stdp_ltp_weight", "double", 0.01,
        "STDP long-term potentiation weight"));
    
    schemas.push_back(ConfigSchema("stdp_ltd_weight", "double", 0.012,
        "STDP long-term depression weight"));
    
    schemas.push_back(ConfigSchema("stdp_time_constant", "double", 20.0,
        "STDP time constant in steps"));
    
    // Development settings
    schemas.push_back(ConfigSchema("development_synaptogenesis_rate", "double", 0.001,
        "Synaptogenesis rate during development"));
    
    schemas.push_back(ConfigSchema("development_pruning_rate", "double", 0.0001,
        "Pruning rate during development"));
    
    // Neuromodulation settings
    schemas.push_back(ConfigSchema("dopamine_baseline", "double", 0.0,
        "Baseline dopamine level"));
    
    schemas.push_back(ConfigSchema("dopamine_curiosity", "double", 0.1,
        "Curiosity parameter for dopamine"));
    
    // Reward settings
    schemas.push_back(ConfigSchema("reward_discount_factor", "double", 0.99,
        "Discount factor for reward prediction"));
    
    // Environment settings
    schemas.push_back(ConfigSchema("environment_name", "string", std::string("GridWorld"),
        "Environment name"));
    
    schemas.push_back(ConfigSchema("environment_width", "int64_t", int64_t(10),
        "Environment width in grid cells"));
    
    schemas.push_back(ConfigSchema("environment_height", "int64_t", int64_t(10),
        "Environment height in grid cells"));
    
    // Logging settings
    schemas.push_back(ConfigSchema("log_level", "string", std::string("INFO"),
        "Log level (DEBUG, INFO, WARN, ERROR, FATAL)"));
    
    schemas.push_back(ConfigSchema("log_to_file", "bool", bool(false),
        "Whether to log to file"));
    
    schemas.push_back(ConfigSchema("log_filename", "string", std::string("nlm.log"),
        "Log filename when log_to_file is true"));
    
    // Simulation settings
    schemas.push_back(ConfigSchema("max_simulation_steps", "int64_t", int64_t(10000),
        "Maximum number of simulation steps"));
    
    schemas.push_back(ConfigSchema("simulation_time_limit", "double", 0.0,
        "Time limit in seconds (0 = unlimited)"));
    
    // Visualization settings
    schemas.push_back(ConfigSchema("visualization_enabled", "bool", bool(false),
        "Whether visualization is enabled"));
    
    schemas.push_back(ConfigSchema("visualization_update_rate", "double", 30.0,
        "Visualization update rate in Hz"));
    
    return schemas;
}

bool ConfigSchemaValidator::validate(const ConfigSchema& schema, const std::string& key, const ConfigValue& value) {
    // For now, basic validation: just check that the key matches
    return schema.key == key;
}

std::string ConfigSchemaValidator::valueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, int> || 
                           std::is_same_v<T, int64_t> || 
                           std::is_same_v<T, double>) {
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
        }
    }, value);
    return oss.str();
}

bool ConfigSchemaValidator::stringToValue(const std::string& type, const std::string& str, ConfigValue& outValue) {
    // Convert string to ConfigValue based on type
    if (type == "int64_t") {
        try {
            size_t pos;
            int64_t val = std::stoll(str, &pos);
            if (pos == str.size()) {
                outValue = val;
                return true;
            }
        } catch (...) {
        }
    } else if (type == "double") {
        try {
            size_t pos;
            double val = std::stod(str, &pos);
            if (pos == str.size()) {
                outValue = val;
                return true;
            }
        } catch (...) {
        }
    } else if (type == "string") {
        // Remove surrounding quotes if present
        std::string val = str;
        if (val.size() >= 2 && ((val.front() == '\"' && val.back() == '\"') ||
                               (val.front() == '\'' && val.back() == '\''))) {
            val = val.substr(1, val.size() - 2);
        }
        outValue = val;
        return true;
    } else if (type == "bool") {
        std::string lower = str;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        if (lower == "true" || lower == "1") {
            outValue = true;
            return true;
        } else if (lower == "false" || lower == "0") {
            outValue = false;
            return true;
        }
    }
    return false;
}

} // namespace nlm
