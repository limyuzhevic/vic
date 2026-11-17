#include <map>
#include <sstream>

namespace nlm {

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

// Explicit template instantiations
// (uint64_t and size_t are the same type on this platform, so only one is listed)
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

// Explicit template instantiations
// (uint64_t and size_t are the same type on this platform, so only one is listed)
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

// Configuration schema for validation
std::map<std::string, std::string> Config::getSchema() {
    return {
        {"random_seed", "int:Random seed for reproducible experiments"},
        {"simulation_timestep", "double:Simulation timestep in seconds"},
        {"neuron_count", "int:Number of neurons in the brain"},
        {"region_count", "int:Number of brain regions"},
        {"connection_probability", "double:Connection probability for random connectivity"},
        {"plasticity_learning_rate", "double:Plasticity learning rate"},
        {"stdp_ltp_weight", "double:STDP LTP weight"},
        {"stdp_ltd_weight", "double:STDP LTD weight"},
        {"stdp_time_constant", "double:STDP time constant"},
        {"development_synaptogenesis_rate", "double:Developmental synaptogenesis rate"},
        {"development_pruning_rate", "double:Developmental pruning rate"},
        {"dopamine_baseline", "double:Neuromodulation baseline dopamine level"},
        {"reward_discount_factor", "double:Reward discount factor"},
        {"environment_name", "string:Environment name"},
        {"environment_width", "int:Environment width"},
        {"environment_height", "int:Environment height"},
        {"log_level", "string:Logging level (Debug, Info, Warning, Error, Critical)"},
        {"log_to_file", "bool:Whether to log to file"},
        {"log_filename", "string:Log filename"},
        {"max_simulation_steps", "int:Maximum simulation steps"},
        {"simulation_time_limit", "double:Simulation time limit"},
        {"visualization_enabled", "bool:Whether visualization is enabled"},
        {"visualization_update_rate", "double:Visualization update rate"}
    };
}

// Validate a specific key
bool Config::validateKey(const std::string& key) const {
    // Check if the key exists
    if (!has(key)) {
        return false;
    }
    
    // Get the schema for this key
    auto schema = getSchema();
    auto schemaIt = schema.find(key);
    
    if (schemaIt == schema.end()) {
        // No schema entry for this key - allow it
        return true;
    }
    
    // Get the configuration value
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false;
    }
    
    // Get the schema info
    std::string schemaInfo = schemaIt->second;
    size_t colonPos = schemaInfo.find(':');
    if (colonPos == std::string::npos) {
        return true; // No type info - allow it
    }
    
    std::string expectedType = schemaInfo.substr(0, colonPos);
    
    // Validate based on type
    return std::visit([&](auto&& v) -> bool {
        using T = std::decay_t<decltype(v)>;
        
        if (expectedType == "int" || expectedType == "int64") {
            return std::is_same_v<T, int> || std::is_same_v<T, int64_t> || 
                   std::is_same_v<T, double> || std::is_same_v<T, float>;
        } else if (expectedType == "double" || expectedType == "float") {
            return std::is_same_v<T, double> || std::is_same_v<T, float> ||
                   std::is_same_v<T, int> || std::is_same_v<T, int64_t>;
        } else if (expectedType == "bool") {
            return std::is_same_v<T, bool>;
        } else if (expectedType == "string") {
            return std::is_same_v<T, std::string>;
        } else if (expectedType == "vector") {
            return std::is_same_v<T, std::vector<int>> ||
                   std::is_same_v<T, std::vector<double>> ||
                   std::is_same_v<T, std::vector<std::string>>;
        }
        return true; // Unknown type - allow it
    }, it->value);
}

// Get validation error message for a specific key
std::string Config::getKeyValidationError(const std::string& key) const {
    // Check if the key exists
    if (!has(key)) {
        return "Missing required configuration key: '" + key + "'";
    }
    
    // Get the schema for this key
    auto schema = getSchema();
    auto schemaIt = schema.find(key);
    
    if (schemaIt == schema.end()) {
        return "Unknown configuration key: '" + key + "'";
    }
    
    // Get the configuration value
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return "Configuration key not found: '" + key + "'";
    }
    
    // Get the schema info
    std::string schemaInfo = schemaIt->second;
    size_t colonPos = schemaInfo.find(':');
    if (colonPos == std::string::npos) {
        return ""; // No type info - assume valid
    }
    
    std::string expectedType = schemaInfo.substr(0, colonPos);
    
    // Validate based on type
    std::string error;
    std::visit([&](auto&& v) {
        using T = std::decay_t<decltype(v)>;
        
        if (expectedType == "int" || expectedType == "int64") {
            if (!std::is_same_v<T, int> && !std::is_same_v<T, int64_t> && 
                !std::is_same_v<T, double> && !std::is_same_v<T, float>) {
                error = "Key '" + key + "' expects type int, got " + getTypeName<T>();
            }
        } else if (expectedType == "double" || expectedType == "float") {
            if (!std::is_same_v<T, double> && !std::is_same_v<T, float> &&
                !std::is_same_v<T, int> && !std::is_same_v<T, int64_t>) {
                error = "Key '" + key + "' expects type double, got " + getTypeName<T>();
            }
        } else if (expectedType == "bool") {
            if (!std::is_same_v<T, bool>) {
                error = "Key '" + key + "' expects type bool, got " + getTypeName<T>();
            }
        } else if (expectedType == "string") {
            if (!std::is_same_v<T, std::string>) {
                error = "Key '" + key + "' expects type string, got " + getTypeName<T>();
            }
        } else if (expectedType == "vector") {
            if (!std::is_same_v<T, std::vector<int>> &&
                !std::is_same_v<T, std::vector<double>> &&
                !std::is_same_v<T, std::vector<std::string>>) {
                error = "Key '" + key + "' expects type vector, got " + getTypeName<T>();
            }
        }
    }, it->value);
    
    return error;
}

// Validate all configuration
bool Config::validate() const {
    return validateAndGetErrors().empty();
}

// Get all validation errors
std::vector<std::string> Config::validateAndGetErrors() const {
    std::vector<std::string> errors;
    
    // Get the schema
    auto schema = getSchema();
    
    // Check all schema keys for existence
    for (const auto& [key, schemaInfo] : schema) {
        size_t colonPos = schemaInfo.find(':');
        if (colonPos != std::string::npos) {
            std::string expectedType = schemaInfo.substr(0, colonPos);
            // Only check type for keys that have explicit type requirements
            if (expectedType == "int" || expectedType == "int64" ||
                expectedType == "double" || expectedType == "float" ||
                expectedType == "bool" || expectedType == "string" ||
                expectedType == "vector") {
                
                if (!has(key)) {
                    errors.push_back("Missing required configuration key: '" + key + "'");
                } else {
                    std::string error = getKeyValidationError(key);
                    if (!error.empty()) {
                        errors.push_back(error);
                    }
                }
            }
        }
    }
    
    return errors;
}

// Helper function to get type name for error messages
template<typename T>
std::string Config::getTypeName() {
    if constexpr (std::is_same_v<T, int>) return "int";
    if constexpr (std::is_same_v<T, int64_t>) return "int64_t";
    if constexpr (std::is_same_v<T, double>) return "double";
    if constexpr (std::is_same_v<T, float>) return "float";
    if constexpr (std::is_same_v<T, bool>) return "bool";
    if constexpr (std::is_same_v<T, std::string>) return "string";
    if constexpr (std::is_same_v<T, std::vector<int>>) return "vector<int>";
    if constexpr (std::is_same_v<T, std::vector<double>>) return "vector<double>";
    if constexpr (std::is_same_v<T, std::vector<std::string>>) return "vector<string>";
    return "unknown";
}
