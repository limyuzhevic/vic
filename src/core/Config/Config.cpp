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

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to parse as JSON first
    try {
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        
        // Check if it looks like JSON (starts with { and ends with })
        if (content.find('{') != std::string::npos && content.find('}') != std::string::npos) {
            // TODO: Implement JSON parser using a lightweight JSON library
            // For now, fall back to simple format
        }
    } catch (...) {
        // If JSON parsing fails, try simple format
    }
    
    // Parse simple key=value pairs (fallback)
    file.clear();
    file.seekg(0);
    
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
    oss << "Configuration Summary:" << std::endl;
    oss << "  Total entries: " << pImpl->entries.size() << std::endl;
    
    // Show configured values
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
        oss << "] (Source: " << static_cast<int>(entry.source) << ")" << std::endl;
    }
    
    // Show validation status
    auto errors = validate();
    if (!errors.empty()) {
        oss << std::endl << "  Validation Errors:" << std::endl;
        for (const auto& error : errors) {
            oss << "    " << error.key << ": " << error.message << std::endl;
        }
    } else {
        oss << std::endl << "  ✓ Configuration is valid" << std::endl;
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

// Validation implementation
std::vector<ConfigValidationError> Config::validate() const {
    std::vector<ConfigValidationError> errors;
    
    // Check for required configuration keys
    std::vector<std::string> requiredKeys = getRequiredKeys();
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            ConfigValidationError error;
            error.key = key;
            error.message = "Missing required configuration key: " + key;
            error.source = ConfigSource::Default;
            errors.push_back(error);
        }
    }
    
    // Validate specific configuration values
    if (has("neuron_count")) {
        auto count = get<size_t>("neuron_count");
        if (count.has_value()) {
            if (count.value() < 1) {
                ConfigValidationError error;
                error.key = "neuron_count";
                error.message = "neuron_count must be >= 1, got " + std::to_string(count.value());
                error.source = ConfigSource::File;
                errors.push_back(error);
            }
        }
    }
    
    if (has("simulation_timestep")) {
        auto timestep = get<double>("simulation_timestep");
        if (timestep.has_value()) {
            if (timestep.value() <= 0.0) {
                ConfigValidationError error;
                error.key = "simulation_timestep";
                error.message = "simulation_timestep must be > 0, got " + std::to_string(timestep.value());
                error.source = ConfigSource::File;
                errors.push_back(error);
            }
        }
    }
    
    if (has("connection_probability")) {
        auto prob = get<float>("connection_probability");
        if (prob.has_value()) {
            if (prob.value() < 0.0f || prob.value() > 1.0f) {
                ConfigValidationError error;
                error.key = "connection_probability";
                error.message = "connection_probability must be in [0.0, 1.0], got " + std::to_string(prob.value());
                error.source = ConfigSource::File;
                errors.push_back(error);
            }
        }
    }
    
    return errors;
}

bool Config::validateAndFix() const {
    auto errors = validate();
    
    // If there are errors that can be automatically fixed, fix them
    bool wasFixed = false;
    for (const auto& error : errors) {
        if (error.key == "neuron_count" && !has("neuron_count")) {
            // Fix missing neuron_count by setting a default
            set("neuron_count", 1000LL, ConfigSource::Default);
            wasFixed = true;
        } else if (error.key == "simulation_timestep" && !has("simulation_timestep")) {
            // Fix missing simulation_timestep by setting a default
            set("simulation_timestep", 0.001, ConfigSource::Default);
            wasFixed = true;
        } else if (error.key == "connection_probability" && !has("connection_probability")) {
            // Fix missing connection_probability by setting a default
            set("connection_probability", 0.1f, ConfigSource::Default);
            wasFixed = true;
        }
    }
    
    return wasFixed || errors.empty();
}

// Configuration defaults
std::string Config::getDefaultConfigFile() {
    return "configs/default.cfg";
}

std::vector<std::string> Config::getRequiredKeys() {
    return {
        "neuron_count",
        "simulation_timestep",
        "connection_probability"
    };
}
