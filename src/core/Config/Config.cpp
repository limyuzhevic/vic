#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <cmath>

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
    
    // Add header with timestamp and version
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream header;
    header << "# NLM Configuration File\\n";
    header << "# Generated on " << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << "\\n";
    header << "# Version: 1.0\\n\\n";
    file << header.str();
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\\n";
        file << entry.key << " = " << getValueAsString(entry.value) << "\\n\\n";
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

bool Config::validate() const {
    std::vector<std::string> errors = getValidationErrors();
    return errors.empty();
}

std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    
    for (const auto& entry : pImpl->entries) {
        if (entry.key.empty()) {
            errors.push_back("Empty key found");
            continue;
        }
        
        // Type-specific validation
        if (entry.key == "neuron_count") {
            auto val = get<int>(entry.key);
            if (val && (*val <= 0)) {
                errors.push_back("neuron_count must be positive");
            }
        }
        else if (entry.key == "region_count") {
            auto val = get<int>(entry.key);
            if (val && (*val <= 0)) {
                errors.push_back("region_count must be positive");
            }
        }
        else if (entry.key == "connection_probability") {
            auto val = get<float>(entry.key);
            if (val && (*val < 0.0f || *val > 1.0f)) {
                errors.push_back("connection_probability must be between 0.0 and 1.0");
            }
        }
        else if (entry.key == "plasticity_learning_rate") {
            auto val = get<float>(entry.key);
            if (val && (*val < 0.0f || *val > 1.0f)) {
                errors.push_back("plasticity_learning_rate must be between 0.0 and 1.0");
            }
        }
        else if (entry.key == "stdp_ltp_weight" || entry.key == "stdp_ltd_weight" || 
                 entry.key == "stdp_time_constant") {
            auto val = get<float>(entry.key);
            if (val && (*val < 0.0f)) {
                errors.push_back(entry.key + " must be non-negative");
            }
        }
        else if (entry.key == "environment_width" || entry.key == "environment_height") {
            auto val = get<int>(entry.key);
            if (val && (*val <= 0)) {
                errors.push_back(entry.key + " must be positive");
            }
        }
        else if (entry.key == "simulation_timestep") {
            auto val = get<double>(entry.key);
            if (val && (*val <= 0.0)) {
                errors.push_back("simulation_timestep must be positive");
            }
        }
    }
    
    return errors;
}

// Helper function to convert ConfigValue to string
std::string Config::getValueAsString(const ConfigValue& value) const {
    std::stringstream ss;
    std::visit([&ss](auto&& arg) {
        if constexpr (std::is_same_v<decltype(arg), std::string>) {
            ss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<decltype(arg), std::vector<int>>) {
            ss << "["; for (size_t i = 0; i < arg.size(); ++i) {
                if (i) ss << ", "; ss << arg[i];
            } ss << "]";
        } else if constexpr (std::is_same_v<decltype(arg), std::vector<double>>) {
            ss << "["; for (size_t i = 0; i < arg.size(); ++i) {
                if (i) ss << ", "; ss << arg[i];
            } ss << "]";
        } else if constexpr (std::is_same_v<decltype(arg), std::vector<std::string>>) {
            ss << "["; for (size_t i = 0; i < arg.size(); ++i) {
                if (i) ss << ", "; ss << "\"" << arg[i] << "\"";
            } ss << "]";
        } else {
            ss << arg;  // Handles int, int64_t, double, bool
        }
    }, value);
    return ss.str();
}

// Helper function to get current timestamp
std::string Config::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Helper function to validate values
bool Config::validateValue(const std::string& key, const std::string& value) const {
    // Key validation
    if (key.empty() || key.find_first_of(" \t\r\n") != std::string::npos) {
        return false;
    }
    
    // Value validation based on key
    if (key == "neuron_count" || key == "region_count") {
        try {
            int val = std::stoi(value);
            return val > 0;
        } catch (...) { return false; }
    }
    
    if (key == "plasticity_learning_rate" || key == "connection_probability") {
        try {
            double val = std::stod(value);
            return val >= 0.0 && val <= 1.0;
        } catch (...) { return false; }
    }
    
    if (key == "stdp_ltp_weight" || key == "stdp_ltd_weight" || key == "stdp_time_constant") {
        try {
            double val = std::stod(value);
            return val >= 0.0;
        } catch (...) { return false; }
    }
    
    if (key == "environment_width" || key == "environment_height") {
        try {
            int val = std::stoi(value);
            return val > 0;
        } catch (...) { return false; }
    }
    
    if (key == "simulation_timestep") {
        try {
            double val = std::stod(value);
            return val > 0.0;
        } catch (...) { return false; }
    }
    
    return true; // Default: accept any value
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
