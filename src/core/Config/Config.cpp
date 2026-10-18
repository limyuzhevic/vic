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

// Set a value from string with automatic type detection
void Config::setTypedValue(const std::string& key, const std::string& value, ConfigSource source) {
    if (value.empty()) {
        set(key, std::string(), source);
        return;
    }
    
    // Try to parse as boolean
    if (value == "true" || value == "false") {
        bool boolVal = (value == "true");
        set(key, boolVal, source);
        return;
    }
    
    // Try to parse as integer
    try {
        size_t pos;
        int intVal = std::stoi(value, &pos);
        if (pos == value.size()) {
            set(key, intVal, source);
            return;
        }
    } catch (...) {}
    
    // Try to parse as 64-bit integer
    try {
        size_t pos;
        int64_t int64Val = std::stoll(value, &pos);
        if (pos == value.size()) {
            set(key, int64Val, source);
            return;
        }
    } catch (...) {}
    
    // Try to parse as double
    try {
        size_t pos;
        double doubleVal = std::stod(value, &pos);
        if (pos == value.size()) {
            set(key, doubleVal, source);
            return;
        }
    } catch (...) {}
    
    // Default to string
    set(key, value, source);
}

// Set a value with validation
bool Config::setValidated(const std::string& key, const ConfigValue& value, 
                         ConfigSource source, std::string& error) {
    // Validate based on key and value type
    // Add specific validation rules for known configuration keys
    
    if (key == "neuron_count") {
        if (std::holds_alternative<int64_t>(value) || std::holds_alternative<int>(value)) {
            int64_t count = std::holds_alternative<int64_t>(value) ? 
                std::get<int64_t>(value) : std::get<int>(value);
            if (count < 1 || count > 10000) {
                error = "neuron_count must be between 1 and 10000";
                return false;
            }
        } else {
            error = "neuron_count must be an integer";
            return false;
        }
    } else if (key == "simulation_timestep") {
        if (std::holds_alternative<double>(value)) {
            double timestep = std::get<double>(value);
            if (timestep <= 0.0 || timestep > 1.0) {
                error = "simulation_timestep must be between 0.000001 and 1.0";
                return false;
            }
        } else if (std::holds_alternative<float>(value)) {
            float timestep = std::get<float>(value);
            if (timestep <= 0.0f || timestep > 1.0f) {
                error = "simulation_timestep must be between 0.000001 and 1.0";
                return false;
            }
        } else {
            error = "simulation_timestep must be a floating point number";
            return false;
        }
    } else if (key == "connection_probability") {
        if (std::holds_alternative<double>(value) || std::holds_alternative<float>(value)) {
            double prob = std::holds_alternative<double>(value) ? 
                std::get<double>(value) : std::get<float>(value);
            if (prob < 0.0 || prob > 1.0) {
                error = "connection_probability must be between 0.0 and 1.0";
                return false;
            }
        } else {
            error = "connection_probability must be a floating point number";
            return false;
        }
    }
    
    // If validation passes, set the value
    set(key, value, source);
    return true;
}

// Load from file with error handling
bool Config::loadFromFile(const std::string& filepath, std::string& error) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        error = "Cannot open config file: " + filepath;
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        ++lineNum;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            error = "Invalid format in config file " + filepath + ": line " + 
                    std::to_string(lineNum) + " - expected key=value";
            return false;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Set the value with validation
        std::string valError;
        if (!setTypedValue(key, value, ConfigSource::File)) {
            error = "Error setting value for key " + key + " in " + filepath + ": " + valError;
            return false;
        }
    }
    
    return true;
}

// Load from command line with error handling
bool Config::loadFromArgs(int argc, char** argv, std::string& error) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                std::string valError;
                if (!setTypedValue(key, value, ConfigSource::CommandLine)) {
                    error = "Error processing command line argument --" + key + "=" + value + ": " + valError;
                    return false;
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            std::string valError;
            if (!setTypedValue(key, value, ConfigSource::CommandLine)) {
                error = "Error processing command line argument -" + key + " " + value + ": " + valError;
                return false;
            }
        }
        // Handle --key value format (when value doesn't contain =)
        else if (arg[0] == '-' && i + 1 < argc && arg.find('=') == std::string::npos) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            std::string valError;
            if (!setTypedValue(key, value, ConfigSource::CommandLine)) {
                error = "Error processing command line argument -" + key + " " + value + ": " + valError;
                return false;
            }
        }
    }
    return true;
}

// Save to file with error handling
bool Config::saveToFile(const std::string& filepath, std::string& error) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        error = "Cannot create config file: " + filepath;
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Serialize the config value based on its type
        std::visit([&file](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            
            if constexpr (std::is_same_v<T, int>) {
                file << value;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << value;
            } else if constexpr (std::is_same_v<T, double>) {
                file << value;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (value ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << value << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << value[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << value[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[\"\"";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i > 0) file << "\", \"\"";
                    file << value[i];
                }
                file << "\"]";
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    return true;
}

// Set default values for a specific configuration profile
void Config::setDefaults(const std::string& profile, ConfigSource source) {
    if (profile == "phase2") {
        set("random_seed", static_cast<int64_t>(42), source);
        set("simulation_timestep", 0.001, source);
        set("neuron_count", static_cast<int64_t>(500), source);
        set("region_count", static_cast<int64_t>(1), source);
        set("connection_probability", 0.15f, source);
        set("stdp_ltp_weight", 0.02f, source);
        set("stdp_ltd_weight", 0.015f, source);
        set("stdp_tau", 20.0f, source);
        set("synaptogenesis_rate", 0.0001f, source);
        set("pruning_rate", 0.00001f, source);
    } else if (profile == "phase3") {
        set("random_seed", static_cast<int64_t>(42), source);
        set("simulation_timestep", 0.001, source);
        set("neuron_count", static_cast<int64_t>(2000), source);
        set("region_count", static_cast<int64_t>(2), source);
        set("connection_probability", 0.1f, source);
        set("stdp_ltp_weight", 0.01f, source);
        set("stdp_ltd_weight", 0.012f, source);
        set("stdp_tau", 20.0f, source);
        set("synaptogenesis_rate", 0.00005f, source);
        set("pruning_rate", 0.000005f, source);
    } else if (profile == "phase4") {
        set("random_seed", static_cast<int64_t>(42), source);
        set("simulation_timestep", 0.001, source);
        set("neuron_count", static_cast<int64_t>(5000), source);
        set("region_count", static_cast<int64_t>(3), source);
        set("connection_probability", 0.05f, source);
        set("stdp_ltp_weight", 0.005f, source);
        set("stdp_ltd_weight", 0.006f, source);
        set("stdp_tau", 20.0f, source);
        set("synaptogenesis_rate", 0.00001f, source);
        set("pruning_rate", 0.000001f, source);
    }
    // Add more profiles as needed
}

// Merge configuration from another Config object
void Config::merge(const Config& other) {
    for (const auto& entry : other.getKeys()) {
        auto value = other.get<std::string>(entry);
        if (value.has_value()) {
            set(entry, value.value(), ConfigSource::Runtime);
        }
    }
}

// Get configuration profile names
std::vector<std::string> Config::getAvailableProfiles() {
    return {"phase2", "phase3", "phase4"};
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
                setTypedValue(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            setTypedValue(key, value, ConfigSource::CommandLine);
        }
        // Handle --key value format (when value doesn't contain =)
        else if (arg[0] == '-' && i + 1 < argc && arg.find('=') == std::string::npos) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            setTypedValue(key, value, ConfigSource::CommandLine);
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
        
        // Serialize the config value based on its type
        std::visit([&file](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            
            if constexpr (std::is_same_v<T, int>) {
                file << value;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << value;
            } else if constexpr (std::is_same_v<T, double>) {
                file << value;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (value ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << value << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << value[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << value[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[\"\"";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i > 0) file << "\", \"\"";
                    file << value[i];
                }
                file << "\"]";
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

// Set a value from string with automatic type detection
void Config::setTypedValue(const std::string& key, const std::string& value, ConfigSource source) {
    if (value.empty()) {
        set(key, std::string(), source);
        return;
    }
    
    // Try to parse as boolean
    if (value == "true" || value == "false") {
        bool boolVal = (value == "true");
        set(key, boolVal, source);
        return;
    }
    
    // Try to parse as integer
    try {
        size_t pos;
        int intVal = std::stoi(value, &pos);
        if (pos == value.size()) {
            set(key, intVal, source);
            return;
        }
    } catch (...) {}
    
    // Try to parse as 64-bit integer
    try {
        size_t pos;
        int64_t int64Val = std::stoll(value, &pos);
        if (pos == value.size()) {
            set(key, int64Val, source);
            return;
        }
    } catch (...) {}
    
    // Try to parse as double
    try {
        size_t pos;
        double doubleVal = std::stod(value, &pos);
        if (pos == value.size()) {
            set(key, doubleVal, source);
            return;
        }
    } catch (...) {}
    
    // Default to string
    set(key, value, source);
}
bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Serialize the config value based on its type
        std::visit([&file](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            
            if constexpr (std::is_same_v<T, int>) {
                file << value;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << value;
            } else if constexpr (std::is_same_v<T, double>) {
                file << value;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (value ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << value << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << value[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << value[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[\"\"";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i > 0) file << "\", \"\"";
                    file << value[i];
                }
                file << "\"]";
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    return true;
}

// Get configuration value as string
std::string Config::getString(const std::string& key, const std::string& defaultValue) const {
    auto value = get<std::string>(key);
    return value.has_value() ? value.value() : defaultValue;
}

// Get numeric values with error checking
int Config::getInt(const std::string& key, int defaultValue) const {
    auto value = get<int>(key);
    return value.has_value() ? value.value() : defaultValue;
}

double Config::getDouble(const std::string& key, double defaultValue) const {
    auto value = get<double>(key);
    return value.has_value() ? value.value() : defaultValue;
}

bool Config::getBool(const std::string& key, bool defaultValue) const {
    auto value = get<bool>(key);
    return value.has_value() ? value.value() : defaultValue;
}

// Configuration validation and error handling
bool Config::validate(const std::string& key, std::string& error) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        error = "Key not found: " + key;
        return false;
    }
    
    // Additional validation can be added here based on key
    // For example, validate ranges, format, etc.
    
    return true;
}

// Helper for getting value as string
std::string Config::getValueAsString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << val << "\"";
        } else {
            oss << val;
        }
    }, value);
    return oss.str();
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
