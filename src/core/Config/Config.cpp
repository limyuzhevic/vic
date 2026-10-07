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

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
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
        NLM_LOG_WARNING("Configuration file not found: " + filepath);
        return false;
    }
    
    NLM_LOG_INFO("Loading configuration from: " + filepath);

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
        } else {
            NLM_LOG_WARNING("Invalid configuration line (no '='): " + line);
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (argc == 0 || argv == nullptr) {
        NLM_LOG_WARNING("No command line arguments to parse");
        return false;
    }
    
    NLM_LOG_INFO("Loading configuration from command line arguments");

    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
                NLM_LOG_INFO("Command line config: " + key + " = " + value);
            } else {
                NLM_LOG_WARNING("Invalid command line argument format: " + arg);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
            NLM_LOG_INFO("Command line config: " + key + " = " + value);
        }
        else {
            NLM_LOG_WARNING("Unrecognized command line argument: " + arg);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot save configuration to file: " + filepath);
        return false;
    }
    
    NLM_LOG_INFO("Saving configuration to: " + filepath);
    file << "# NLM Configuration File (Saved)" << std::endl;
    file << "# Generated automatically by NLM system" << std::endl << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Format based on type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << std::setprecision(6) << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << "\"" << arg[i] << "\"";
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]\n";
            } else {
                file << entry.key << " = " << arg << "\n";
            }
        }, entry.value);
        file << std::endl;
    }
    
    NLM_LOG_INFO("Configuration saved successfully");
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        NLM_LOG_DEBUG("Configuration key not found: " + key);
        return std::nullopt;
    }
    
    try {
        T value = std::get<T>(it->value);
        NLM_LOG_DEBUG("Retrieved config " + key + " = " + std::to_string(value));
        return value;
    } catch (const std::bad_variant_access&) {
        NLM_LOG_WARNING("Type mismatch for configuration key: " + key);
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    if (val.has_value()) {
        NLM_LOG_DEBUG("Using config " + key + " = " + std::to_string(val.value()));
        return val.value();
    } else {
        NLM_LOG_DEBUG("Using default config " + key + " = " + std::to_string(defaultValue));
        return defaultValue;
    }
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate configuration key-value pairs
    validateKeyValue(key, value);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        NLM_LOG_INFO("Updated configuration: " + key);
    } else {
        pImpl->entries.emplace_back(key, value, source);
        NLM_LOG_INFO("Set configuration: " + key + " = " + formatValue(value));
    }
}

void Config::validateKeyValue(const std::string& key, const ConfigValue& value) const {
    // Validate specific configuration keys
    
    if (key == "simulation_timestep") {
        try {
            double timestep = std::get<double>(value);
            if (timestep <= 0.0) {
                throw std::invalid_argument("simulation_timestep must be positive");
            }
            if (timestep > 1.0) {
                NLM_LOG_WARNING("Large simulation timestep (" + std::to_string(timestep) + ") may cause numerical instability");
            }
        } catch (const std::bad_variant_access&) {
            throw std::invalid_argument("simulation_timestep must be a numeric value");
        }
    }
    
    if (key == "neuron_count") {
        try {
            int64_t count = std::get<int64_t>(value);
            if (count <= 0) {
                throw std::invalid_argument("neuron_count must be positive");
            }
            if (count > 100000) {
                NLM_LOG_WARNING("Large neuron_count (" + std::to_string(count) + ") may impact performance");
            }
        } catch (const std::bad_variant_access&) {
            throw std::invalid_argument("neuron_count must be an integer");
        }
    }
    
    if (key == "connection_probability") {
        try {
            double prob = std::get<double>(value);
            if (prob < 0.0 || prob > 1.0) {
                throw std::invalid_argument("connection_probability must be between 0.0 and 1.0");
            }
        } catch (const std::bad_variant_access&) {
            throw std::invalid_argument("connection_probability must be a numeric value");
        }
    }
    
    if (key == "random_seed") {
        try {
            int64_t seed = std::get<int64_t>(value);
            if (seed < 0) {
                throw std::invalid_argument("random_seed must be non-negative");
            }
        } catch (const std::bad_variant_access&) {
            throw std::invalid_argument("random_seed must be an integer");
        }
    }
}

std::string Config::formatValue(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                oss << arg[i];
                if (i < arg.size() - 1) oss << ", ";
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                oss << std::setprecision(6) << arg[i];
                if (i < arg.size() - 1) oss << ", ";
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                oss << "\"" << arg[i] << "\"";
                if (i < arg.size() - 1) oss << ", ";
            }
            oss << "]";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

bool Config::has(const std::string& key) const {
    bool exists = std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (!exists) {
        NLM_LOG_DEBUG("Configuration key does not exist: " + key);
    }
    
    return exists;
}

void Config::remove(const std::string& key) {
    if (!has(key)) {
        NLM_LOG_WARNING("Attempted to remove non-existent configuration key: " + key);
        return;
    }
    
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    
    NLM_LOG_INFO("Removed configuration: " + key);
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    
    NLM_LOG_DEBUG("Retrieved " + std::to_string(keys.size()) + " configuration keys");
    return keys;
}

void Config::clear() {
    if (!pImpl->entries.empty()) {
        NLM_LOG_INFO("Clearing all " + std::to_string(pImpl->entries.size()) + " configuration entries");
        pImpl->entries.clear();
    } else {
        NLM_LOG_DEBUG("Configuration already empty");
    }
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
    
    NLM_LOG_INFO("Configuration summary generated");
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
