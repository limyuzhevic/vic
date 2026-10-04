#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

// Include actual JSON/YAML parser if available
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <chrono>

// Forward declaration for JSON/YAML parsing helpers
namespace nlm {
    bool parseJSONFile(const std::string& filepath, std::map<std::string, ConfigValue>& entries);
    bool parseYAMLFile(const std::string& filepath, std::map<std::string, ConfigValue>& entries);
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    
    // JSON/YAML parsing helpers
    static bool loadFromJSON(const std::string& filepath, std::vector<ConfigEntry>& entries);
    static bool loadFromYAML(const std::string& filepath, std::vector<ConfigEntry>& entries);
    static bool loadFromSimpleFormat(const std::string& filepath, std::vector<ConfigEntry>& entries);
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Determine file format based on extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    bool success = false;
    
    if (ext == ".json" || ext == ".jsonc" || ext == ".json5") {
        // Try to parse as JSON
        success = Impl::loadFromJSON(filepath, pImpl->entries);
    } else if (ext == ".yaml" || ext == ".yml") {
        // Try to parse as YAML
        success = Impl::loadFromYAML(filepath, pImpl->entries);
    } else {
        // Default to simple key=value format for backward compatibility
        success = Impl::loadFromSimpleFormat(filepath, pImpl->entries);
    }
    
    if (!success) {
        // Try simple format as fallback (for backward compatibility)
        return Impl::loadFromSimpleFormat(filepath, pImpl->entries);
    }
    
    return true;
}

// JSON/YAML parsing implementation
namespace nlm {
    
    bool parseJSONFile(const std::string& filepath, std::map<std::string, ConfigValue>& entries) {
        try {
            std::ifstream file(filepath);
            if (!file.is_open()) return false;
            
            nlohmann::json j;
            file >> j;
            
            for (auto it = j.begin(); it != j.end(); ++it) {
                std::string key = it.key();
                const auto& value = it.value();
                
                ConfigValue configValue;
                if (value.is_string()) {
                    configValue = value.get<std::string>();
                } else if (value.is_number_integer()) {
                    configValue = value.get<int64_t>();
                } else if (value.is_number_unsigned()) {
                    configValue = value.get<uint64_t>();
                } else if (value.is_number_float()) {
                    configValue = value.get<double>();
                } else if (value.is_boolean()) {
                    configValue = value.get<bool>();
                } else if (value.is_array()) {
                    // Parse array
                    std::vector<double> array;
                    for (const auto& item : value) {
                        array.push_back(item.get<double>());
                    }
                    configValue = array;
                }
                
                entries[key] = configValue;
            }
            
            return true;
        } catch (const std::exception&) {
            return false;
        }
    }
    
    bool parseYAMLFile(const std::string& filepath, std::map<std::string, ConfigValue>& entries) {
        try {
            YAML::Node doc = YAML::LoadFile(filepath);
            
            for (const auto& node : doc) {
                std::string key = node.first.as<std::string>();
                const YAML::Node& value = node.second;
                
                ConfigValue configValue;
                if (value.IsScalar()) {
                    if (value.IsSequence()) {
                        // This is an array
                        std::vector<double> array;
                        for (const auto& item : value) {
                            array.push_back(item.as<double>());
                        }
                        configValue = array;
                    } else if (value.IsDouble()) {
                        configValue = value.as<double>();
                    } else if (value.IsInt()) {
                        configValue = value.as<int64_t>();
                    } else if (value.IsBool()) {
                        configValue = value.as<bool>();
                    } else {
                        configValue = value.as<std::string>();
                    }
                }
                
                entries[key] = configValue;
            }
            
            return true;
        } catch (const std::exception&) {
            return false;
        }
    }
    
} // namespace nlm

// Implementation of JSON/YAML loading helpers
namespace nlm {
    
    bool Config::Impl::loadFromJSON(const std::string& filepath, std::vector<ConfigEntry>& entries) {
        std::map<std::string, ConfigValue> parsedEntries;
        if (parseJSONFile(filepath, parsedEntries)) {
            for (const auto& pair : parsedEntries) {
                entries.emplace_back(pair.first, pair.second, ConfigSource::File);
            }
            return true;
        }
        return false;
    }
    
    bool Config::Impl::loadFromYAML(const std::string& filepath, std::vector<ConfigEntry>& entries) {
        std::map<std::string, ConfigValue> parsedEntries;
        if (parseYAMLFile(filepath, parsedEntries)) {
            for (const auto& pair : parsedEntries) {
                entries.emplace_back(pair.first, pair.second, ConfigSource::File);
            }
            return true;
        }
        return false;
    }
    
    bool Config::Impl::loadFromSimpleFormat(const std::string& filepath, std::vector<ConfigEntry>& entries) {
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
                    ((value.front() == '\"' && value.back() == '\"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
            }
        }
        
        return true;
    }
    
} // namespace nlm

void Config::printHelp() {
    std::cout << "NLM - Neural Learning Machine" << std::endl;
    std::cout << "=" << std::string(50, '=') << std::endl;
    std::cout << "\nUsage: nlm [--config <file>] [--key <value>]... [options]" << std::endl;
    std::cout << "\nOptions:" << std::endl;
    std::cout << "  --help, -h           Show this help message" << std::endl;
    std::cout << "  --version            Show version information" << std::endl;
    std::cout << "  --config <file>      Load configuration from file (JSON, YAML, or key=value)" << std::endl;
    std::cout << "\nKey-value pairs (key=value or --key <value>):" << std::endl;
    std::cout << "  --random_seed <num>      Random seed for reproducibility" << std::endl;
    std::cout << "  --neuron_count <num>     Number of neurons in the brain" << std::endl;
    std::cout << "  --region_count <num>     Number of brain regions" << std::endl;
    std::cout << "  --connection_probability <float>  Probability of connections" << std::endl;
    std::cout << "  --simulation_timestep <float>  Time step for simulation (seconds)" << std::endl;
    std::cout << "\nConfiguration files support JSON, YAML, and key=value formats." << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm --config config.json --neuron_count 2000" << std::endl;
    std::cout << "  nlm --random_seed 42 --connection_probability 0.2" << std::endl;
}

void Config::printVersion() {
    std::cout << "NLM Phase 2: Real Neural Computation" << std::endl;
    std::cout << "Version: 2.0.0 (Phase 6 - Integrated Artificial Brain)" << std::endl;
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << "Description: An experimental artificial developmental brain with real spiking neural computation." << std::endl;
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
