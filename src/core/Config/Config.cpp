#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <cstring>

namespace nlm {

namespace {
    // Helper function to trim whitespace from strings
    std::string trim(const std::string& str) {
        size_t start = 0;
        size_t end = str.size();
        
        // Trim leading whitespace
        while (start < end && std::isspace(str[start])) {
            ++start;
        }
        
        // Trim trailing whitespace
        while (end > start && std::isspace(str[end - 1])) {
            --end;
        }
        
        return str.substr(start, end - start);
    }
    
    // Helper function to check if a string is a valid key
    bool isValidKey(const std::string& key) {
        return !key.empty() && std::find_if(key.begin(), key.end(),
            [](unsigned char c) { return !std::isalnum(c) && c != '_' && c != '-'; }) == key.end();
    }
    
    // Helper function to convert string to appropriate ConfigValue type
    ConfigValue stringToConfigValue(const std::string& value) {
        // Check if it's a boolean
        if (value == "true" || value == "false") {
            return value == "true";
        }
        
        // Check if it's an integer
        try {
            size_t pos;
            long long intVal = std::stoll(value);
            // Check if it fits in int range
            if (intVal >= std::numeric_limits<int>::min() && intVal <= std::numeric_limits<int>::max()) {
                return static_cast<int>(intVal);
            }
            return intVal;
        } catch (...) {}
        
        // Check if it's a floating point
        try {
            double doubleVal = std::stod(value);
            // Check if it fits in float range
            if (doubleVal >= std::numeric_limits<float>::lowest() && 
                doubleVal <= std::numeric_limits<float>::max()) {
                return static_cast<float>(doubleVal);
            }
            return doubleVal;
        } catch (...) {}
        
        // Return as string (default)
        return value;
    }
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default values
    pImpl->entries = {
        ConfigEntry("neuron_count", 500, ConfigSource::Default, "Number of neurons in the brain"),
        ConfigEntry("region_count", 1, ConfigSource::Default, "Number of neural regions"),
        ConfigEntry("connection_probability", 0.15f, ConfigSource::Default, "Probability of connections between neurons"),
        ConfigEntry("stdp_ltp_weight", 0.02f, ConfigSource::Default, "STDP long-term potentiation weight"),
        ConfigEntry("stdp_ltd_weight", 0.015f, ConfigSource::Default, "STDP long-term depression weight"),
        ConfigEntry("stdp_tau", 20.0f, ConfigSource::Default, "STDP time constant"),
        ConfigEntry("synaptogenesis_rate", 0.0001f, ConfigSource::Default, "Rate of new synapse formation"),
        ConfigEntry("pruning_rate", 0.00001f, ConfigSource::Default, "Rate of synapse elimination"),
        ConfigEntry("simulation_timestep", 0.001, ConfigSource::Default, "Simulation time step in seconds"),
        ConfigEntry("random_seed", static_cast<int64_t>(42), ConfigSource::Default, "Random seed for reproducibility"),
        ConfigEntry("max_energy", 100.0f, ConfigSource::Default, "Maximum energy for agents"),
        ConfigEntry("energy_decay_rate", 0.1f, ConfigSource::Default, "Energy decay rate per step"),
        ConfigEntry("development_stage", static_cast<int>(DevelopmentalStage::Adult), ConfigSource::Default, "Current developmental stage"),
        ConfigEntry("enable_plasticity", true, ConfigSource::Default, "Enable/disable plasticity mechanisms"),
        ConfigEntry("enable_development", true, ConfigSource::Default, "Enable/disable developmental processes"),
        ConfigEntry("enable_checkpointing", true, ConfigSource::Default, "Enable/disable checkpoint saving"),
        ConfigEntry("learning_rate", 0.01f, ConfigSource::Default, "Learning rate for plasticity"),
        ConfigEntry("novelty_threshold", 0.5f, ConfigSource::Default, "Threshold for novelty detection"),
        ConfigEntry("curiosity_factor", 0.1f, ConfigSource::Default, "Curiosity-driven exploration factor")
    };
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Validate file path
    if (filepath.empty()) {
        throw std::invalid_argument("Filepath cannot be empty");
    }
    
    // Check if file exists and is readable
    if (!std::filesystem::exists(filepath)) {
        throw std::runtime_error("Configuration file does not exist: " + filepath);
    }
    
    if (!std::filesystem::is_regular_file(filepath)) {
        throw std::runtime_error("Configuration path is not a file: " + filepath);
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open configuration file for reading: " + filepath);
    }
    
    std::string line;
    size_t lineNumber = 0;
    bool hasValidContent = false;
    
    while (std::getline(file, line)) {
        ++lineNumber;
        
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
            
            // Validate key
            if (!isValidKey(key)) {
                throw std::runtime_error("Invalid configuration key at line " + std::to_string(lineNumber) + ": " + key);
            }
            
            // Parse and set value
            try {
                ConfigValue configValue = stringToConfigValue(value);
                set(key, configValue, ConfigSource::File);
                hasValidContent = true;
            } catch (const std::exception& e) {
                throw std::runtime_error("Error parsing configuration value at line " + std::to_string(lineNumber) + ": " + std::string(e.what()));
            }
        } else {
            throw std::runtime_error("Invalid configuration format at line " + std::to_string(lineNumber) + ": missing '='");
        }
    }
    
    if (!hasValidContent) {
        throw std::runtime_error("Configuration file contains no valid key=value pairs");
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (!argv) {
        throw std::invalid_argument("argv cannot be nullptr");
    }
    
    if (argc < 1) {
        throw std::invalid_argument("argc must be at least 1");
    }
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Validate argument
        if (arg.empty()) {
            throw std::invalid_argument("Argument " + std::to_string(i) + " is empty");
        }
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Validate key
                if (!isValidKey(key)) {
                    throw std::runtime_error("Invalid configuration key in argument: " + key);
                }
                
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Validate key
            if (!isValidKey(key)) {
                throw std::runtime_error("Invalid configuration key in argument: " + key);
            }
            
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    // Validate file path
    if (filepath.empty()) {
        throw std::invalid_argument("Filepath cannot be empty");
    }
    
    // Check if directory exists (file may not exist yet)
    std::filesystem::path filepathObj(filepath);
    if (!filepathObj.has_parent_path() || !std::filesystem::exists(filepathObj.parent_path())) {
        throw std::runtime_error("Cannot write to directory: " + filepathObj.parent_path().to_string());
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open configuration file for writing: " + filepath);
    }
    
    // Write header
    file << "// NLM Configuration File" << std::endl;
    file << "// Generated: " << std::chrono::system_clock::now() << std::endl;
    file << "// Version: 0. aspettare della Configuration File (placeholder for values)" << std::endl;
    file << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        // Write comment
        file << "# " << entry.description << "\n";
        
        // Write key=value pair
        file << entry.key << " = ";
        
        // Convert ConfigValue to string
        std::visit([&file](const auto& val) {
            if constexpr (std::is_same<const std::string&, decltype(val)>::value) {
                file << "\"" << val << "\"\n";
            } else if constexpr (std::is_same<const int, decltype(val)>::value) {
                file << val << "\n";
            } else if constexpr (std::is_same<const int64_t, decltype(val)>::value) {
                file << val << "LL\n";
            } else if constexpr (std::is_same<const double, decltype(val)>::value) {
                file << val << "\n";
            } else if constexpr (std::is_same<const bool, decltype(val)>::value) {
                file << (val ? "true" : "false") << "\n";
            } else if constexpr (std::is_same<const float, decltype(val)>::value) {
                file << val << "f\n";
            } else {
                // Handle vector types (simplified for example)
                file << "// Vector value: " << val << "\n";
            }
        }, entry.value);
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

} // namespace nlm
