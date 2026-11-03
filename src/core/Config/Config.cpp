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
    // Detect file format by extension
    std::string extension = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        extension = filepath.substr(dotPos + 1);
    }
    
    if (extension == "json") {
        return loadFromJSONFile(filepath);
    } else if (extension == "yaml" || extension == "yml") {
        return loadFromYAMLFile(filepath);
    } else {
        // Default to key=value format
        return loadFromPlainFile(filepath);
    }
}

bool Config::loadFromJSONFile(const std::string& filepath) {
    // TODO: Implement JSON parsing using nlohmann/json library
    // For now, provide a stub implementation
    return loadFromPlainFile(filepath);
}

bool Config::loadFromYAMLFile(const std::string& filepath) {
    // TODO: Implement YAML parsing using yaml-cpp library
    // For now, provide a stub implementation
    return loadFromPlainFile(filepath);
}

bool Config::loadFromPlainFile(const std::string& filepath) {
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
        file << entry.key << " = ";
        
        // Serialize value based on type
        std::visit([&file](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, std::string>) {
                // Quote strings
                file << "\"" << value << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                // Format vector<int> as [a,b,c]
                file << "[";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i) file << ", ";
                    file << value[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                // Format vector<double> as [a,b,c]
                file << "[";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i) file << ", ";
                    file << value[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                // Format vector<string> as ["a","b","c"]
                file << "[";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i) file << ", ";
                    file << "\"" << value[i] << "\"";
                }
                file << "]";
            } else {
                // For int, int64_t, double, bool - just output the value
                file << value;
            }
        }, entry.value);
        
        file << "\n\n"; // Add blank line for readability
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

void Config::configureFromFile(const std::string& filepath) {
    loadFromFile(filepath);
}

void Config::configureFromJSONFile(const std::string& filepath) {
    loadFromJSONFile(filepath);
}

void Config::configureFromYAMLFile(const std::string& filepath) {
    loadFromYAMLFile(filepath);
}

void Config::configureFromCommandLine(int argc, char** argv) {
    loadFromArgs(argc, argv);
}

bool Config::validate() const {
    // Validate configuration values based on known keys and expected types
    for (const auto& entry : pImpl->entries) {
        bool isValid = validateEntry(entry);
        if (!isValid) {
            return false;
        }
    }
    return true;
}

bool Config::validateEntry(const ConfigEntry& entry) const {
    // Basic validation based on key patterns
    if (entry.key.empty()) {
        return false;
    }
    
    // Validate specific keys
    if (entry.key == "simulation_timestep") {
        if (!std::holds_alternative<double>(entry.value)) {
            return false;
        }
        double timestep = std::get<double>(entry.value);
        if (timestep <= 0.0 || timestep > 1.0) {
            return false;
        }
    }
    else if (entry.key == "neuron_count") {
        if (!std::holds_alternative<int>(entry.value)) {
            return false;
        }
        int count = std::get<int>(entry.value);
        if (count <= 0 || count > 100000) {
            return false;
        }
    }
    else if (entry.key == "region_count") {
        if (!std::holds_alternative<int>(entry.value)) {
            return false;
        }
        int count = std::get<int>(entry.value);
        if (count <= 0 || count > 100) {
            return false;
        }
    }
    else if (entry.key == "random_seed") {
        if (!std::holds_alternative<int>(entry.value)) {
            return false;
        }
        int seed = std::get<int>(entry.value);
        if (seed < 0 || seed > 2147483647) {
            return false;
        }
    }
    else if (entry.key.find("weight") != std::string::npos || 
             entry.key.find("rate") != std::string::npos) {
        // Weight and rate parameters
        if (!std::holds_alternative<double>(entry.value)) {
            return false;
        }
        double value = std::get<double>(entry.value);
        if (value < -10.0 || value > 10.0) {
            return false;
        }
    }
    
    return true;
}

std::string Config::getConfigurationReport() const {
    std::ostringstream oss;
    oss << "=== NLM Configuration Report ===\n";
    oss << "Total entries: " << pImpl->entries.size() << "\n\n";
    
    // Validate configuration
    bool isValid = validate();
    oss << "Validation: " << (isValid ? "PASSED" : "FAILED") << "\n\n";
    
    // Show configuration by source
    oss << "Configuration by source:\n";
    int defaultCount = 0, fileCount = 0, cmdlineCount = 0, runtimeCount = 0;
    
    for (const auto& entry : pImpl->entries) {
        switch (entry.source) {
            case ConfigSource::Default:
                defaultCount++;
                break;
            case ConfigSource::File:
                fileCount++;
                break;
            case ConfigSource::CommandLine:
                cmdlineCount++;
                break;
            case ConfigSource::Runtime:
                runtimeCount++;
                break;
        }
    }
    
    oss << "  Default:     " << defaultCount << "\n";
    oss << "  File:        " << fileCount << "\n";
    oss << "  CommandLine: " << cmdlineCount << "\n";
    oss << "  Runtime:     " << runtimeCount << "\n\n";
    
    // Show critical parameters
    oss << "Critical Parameters:\n";
    std::vector<std::string> criticalKeys = {
        "simulation_timestep", "neuron_count", "region_count", "random_seed",
        "plasticity_learning_rate", "stdp_ltp_weight", "stdp_ltd_weight",
        "development_synaptogenesis_rate", "development_pruning_rate"
    };
    
    for (const auto& key : criticalKeys) {
        if (has(key)) {
            oss << "  " << key << " = ";
            
            auto val = get<std::string>(key);
            if (val.has_value()) {
                oss << val.value() << "\n";
            } else {
                oss << "<unavailable>\n";
            }
        }
    }
    
    return oss.str();
}

} // namespace nlm
