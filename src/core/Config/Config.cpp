#include "Config.hpp"
#include "constants.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <unordered_map>
#include <vector>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::string> typeHints;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with basic type validation
    initializeTypeHints();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::initializeTypeHints() {
    // Register common configuration keys with their types for validation
    pImpl->typeHints = {
        {"neuron_count", "int64"},
        {"region_count", "int64"},
        {"connection_probability", "float"},
        {"random_seed", "int64"},
        {"simulation_timestep", "double"},
        {"stdp_ltp_weight", "float"},
        {"stdp_ltd_weight", "float"},
        {"stdp_tau", "float"},
        {"synaptogenesis_rate", "float"},
        {"pruning_rate", "float"},
        {"environment_width", "float"},
        {"environment_height", "float"},
        {"vision_width", "int"},
        {"vision_height", "int"},
        {"turn_speed", "float"},
        {"move_speed", "float"},
        {"agent_energy_max", "float"},
        {"agent_energy_decay_rate", "float"},
        {"working_memory_size", "int"},
        {"episodic_memory_capacity", "int"},
        {"planning_horizon", "int"},
        {"attention_selectivity", "float"},
        {"enable_plasticity", "bool"},
        {"enable_curiosity", "bool"},
        {"enable_reward_modulation", "bool"},
        {"enable_development", "bool"},
    };
}

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    size_t lineNum = 0;
    std::string line;
    size_t fileSize = std::filesystem::file_size(filepath);
    
    if (fileSize > constants::MAX_FILE_SIZE) {
        NLM_LOG_ERROR("Config file too large: " + std::to_string(fileSize) + " bytes (max " + 
                     std::to_string(constants::MAX_FILE_SIZE) + " bytes)");
        return false;
    }
    
    while (std::getline(file, line)) {
        ++lineNum;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Check for syntax errors
        if (line.find('=') == std::string::npos) {
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNum) + ": expected 'key = value' format, got: " + line);
            continue;
        }
        
        // Parse key=value pairs
        size_t pos = line.find('=');
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        if (key.empty() || value.empty()) {
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNum) + ": empty key or value");
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Validate value format before setting
        if (!validateValue(key, value)) {
            NLM_LOG_WARNING("Invalid config value for key '" + key + "' on line " + std::to_string(lineNum));
            continue;
        }
        
        try {
            set(key, value, ConfigSource::File);
            NLM_LOG_DEBUG("Loaded config: " + key + " = " + value);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error setting config key '" + key + "': " + e.what());
        }
    }
    
    size_t validEntries = 0;
    for (const auto& entry : pImpl->entries) {
        if (!entry.key.empty()) validEntries++;
    }
    
    if (validEntries == 0) {
        NLM_LOG_WARNING("No valid configuration entries found in file: " + filepath);
        return false;
    }
    
    NLM_LOG_INFO("Loaded " + std::to_string(validEntries) + " configuration entries from: " + filepath);
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    size_t validArgs = 0;
    size_t totalArgs = 0;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        totalArgs++;
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Validate key format
                if (!validateKeyFormat(key)) {
                    NLM_LOG_WARNING("Invalid command line key format: " + key);
                    continue;
                }
                
                if (!validateValue(key, value)) {
                    NLM_LOG_WARNING("Invalid command line value for key '" + key + "': " + value);
                    continue;
                }
                
                set(key, value, ConfigSource::CommandLine);
                validArgs++;
                NLM_LOG_DEBUG("Command line arg: " + key + " = " + value);
            } else {
                NLM_LOG_WARNING("Missing '=' in command line argument: " + arg);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Validate key format
            if (!validateKeyFormat(key)) {
                NLM_LOG_WARNING("Invalid command line key format: " + key);
                continue;
            }
            
            if (!validateValue(key, value)) {
                NLM_LOG_WARNING("Invalid command line value for key '" + key + "': " + value);
                continue;
            }
            
            set(key, value, ConfigSource::CommandLine);
            validArgs++;
            NLM_LOG_DEBUG("Command line arg: " + key + " = " + value);
        } else {
            NLM_LOG_WARNING("Unknown command line argument: " + arg);
        }
    }
    
    if (validArgs < totalArgs) {
        NLM_LOG_INFO(std::to_string(totalArgs - validArgs) + " invalid command line arguments ignored");
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to save config file: " + filepath);
        return false;
    }
    
    // Write header comment
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM on " + getCurrentTimestamp() + "\n";
    file << "# This is a simple key=value format for Phase 1 compatibility.\n\n";
    
    // Write configuration entries
    size_t entryCount = 0;
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Format the value appropriately
        std::string formattedValue;
        if (std::holds_alternative<std::string>(entry.value)) {
            formattedValue = "\"" + std::get<std::string>(entry.value) + "\"";
        } else if (std::holds_alternative<int>(entry.value)) {
            formattedValue = std::to_string(std::get<int>(entry.value));
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            formattedValue = std::to_string(std::get<int64_t>(entry.value));
        } else if (std::holds_alternative<float>(entry.value)) {
            formattedValue = std::to_string(std::get<float>(entry.value));
        } else if (std::holds_alternative<double>(entry.value)) {
            formattedValue = std::to_string(std::get<double>(entry.value));
        } else if (std::holds_alternative<bool>(entry.value)) {
            formattedValue = std::get<bool>(entry.value) ? "true" : "false";
        } else {
            formattedValue = "UNKNOWN";
        }
        
        file << entry.key << " = " << formattedValue << "\n\n";
        entryCount++;
    }
    
    file << "# End of configuration file\n";
    
    NLM_LOG_INFO("Saved " + std::to_string(entryCount) + " configuration entries to: " + filepath);
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

// Helper function implementations

bool validateKeyFormat(const std::string& key) {
    // Keys should match pattern: [a-z][a-z_]*
    if (key.empty()) return false;
    if (!std::isalpha(key[0]) && key[0] != '_') return false;
    
    for (char c : key) {
        if (!std::isalnum(c) && c != '_') {
            return false;
        }
    }
    
    if (key.size() > 100) {
        return false;  // Prevent excessively long keys
    }
    
    return true;
}

bool validateValue(const std::string& key, const std::string& value) {
    // Basic format validation
    if (value.empty()) return false;
    if (value.size() > 1000) return false;  // Prevent excessively long values
    
    // Type-specific validation based on key hints
    auto it = pImpl->typeHints.find(key);
    if (it != pImpl->typeHints.end()) {
        const std::string& typeHint = it->second;
        
        if (typeHint == "int" || typeHint == "int64") {
            // Check if value can be converted to int
            try {
                if (typeHint == "int64") {
                    size_t pos;
                    int64_t val = std::stoll(value, &pos);
                    if (pos != value.size()) return false;
                    // Check range for typical neuron counts
                    if (key == "neuron_count" && (val < constants::MIN_NEURON_COUNT || val > constants::MAX_NEURON_COUNT)) {
                        NLM_LOG_WARNING("neuron_count out of range: " + value + " (valid: " + 
                                     std::to_string(constants::MIN_NEURON_COUNT) + "-" + 
                                     std::to_string(constants::MAX_NEURON_COUNT) + ")");
                    }
                } else {
                    size_t pos;
                    int val = std::stoi(value, &pos);
                    if (pos != value.size()) return false;
                }
            } catch (...) {
                return false;
            }
        } else if (typeHint == "float") {
            try {
                size_t pos;
                float val = std::stof(value, &pos);
                if (pos != value.size()) return false;
                
                // Range validation based on key
                if (key == "connection_probability" && (val < constants::MIN_CONNECTION_PROBABILITY || val > constants::MAX_CONNECTION_PROBABILITY)) {
                    NLM_LOG_WARNING("connection_probability out of range: " + value + 
                                 " (valid: " + std::to_string(constants::MIN_CONNECTION_PROBABILITY) + "-" + 
                                 std::to_string(constants::MAX_CONNECTION_PROBABILITY) + ")");
                } else if (key == "input_injection" && (val < constants::MIN_INPUT_INJECTION || val > constants::MAX_INPUT_INJECTION)) {
                    NLM_LOG_WARNING("input_injection out of range: " + value + 
                                 " (valid: " + std::to_string(constants::MIN_INPUT_INJECTION) + "-" + 
                                 std::to_string(constants::MAX_INPUT_INJECTION) + ")");
                }
            } catch (...) {
                return false;
            }
        } else if (typeHint == "double") {
            try {
                size_t pos;
                double val = std::stod(value, &pos);
                if (pos != value.size()) return false;
            } catch (...) {
                return false;
            }
        } else if (typeHint == "bool") {
            std::string lowerValue = toLower(value);
            if (lowerValue != "true" && lowerValue != "false" && lowerValue != "1" && lowerValue != "0") {
                return false;
            }
        }
    }
    
    return true;
}

std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}
