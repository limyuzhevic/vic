#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cstring>

// Optional JSON/YAML parser support
#ifdef HAVE_NLOHMANN_JSON
#include "nlohmann/json.hpp"
#endif

#ifdef HAVE_YAML_CPP
#include "yaml-cpp/yaml.h"
#endif

namespace nlm {

// Helper functions for configuration parsing
namespace {

// Detect file format based on extension and content
enum class FileFormat {
    Unknown,
    Simple,
    JSON,
    YAML
};

FileFormat detectFileFormat(const std::string& filepath) {
    std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "json") return FileFormat::JSON;
    if (ext == "yaml" || ext == "yml") return FileFormat::YAML;
    
    // Try to detect by content
    std::ifstream file(filepath);
    if (!file.is_open()) return FileFormat::Unknown;
    
    char firstChar;
    file.get(firstChar);
    file.clear();
    file.seekg(0);
    
    // Check first character to guess format
    if (firstChar == '{') return FileFormat::JSON;
    if (firstChar == '[') return FileFormat::JSON; // Array format
    if (firstChar == '#') return FileFormat::Simple; // Commented format
    if (firstChar == ' ') return FileFormat::Simple;
    
    // Default to simple format
    return FileFormat::Simple;
}

#ifdef HAVE_NLOHMANN_JSON
bool parseJSONFile(const std::string& filepath, Config& config) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json j;
        file >> j;
        
        for (const auto& item : j.items()) {
            std::string key = item.key();
            config.set(key, item.value().dump());
        }
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}
#endif

#ifdef HAVE_YAML_CPP
bool parseYAMLFile(const std::string& filepath, Config& config) {
    try {
        YAML::Node doc = YAML::LoadFile(filepath);
        
        for (const auto& item : doc) {
            if (item.first.IsScalar()) {
                std::string key = item.first.as<std::string>();
                
                // Handle different YAML value types
                if (item.second.IsScalar()) {
                    std::string value = item.second.as<std::string>();
                    config.set(key, value);
                } else if (item.second.IsSequence()) {
                    // Convert YAML sequence to string representation
                    std::stringstream ss;
                    ss << "["; 
                    bool first = true;
                    for (const auto& element : item.second) {
                        if (!first) ss << ", ";
                        if (element.IsScalar()) {
                            ss << "\"" << element.as<std::string>() << "\"";
                        } else {
                            ss << element.as<std::string>();
                        }
                        first = false;
                    }
                    ss << "]";
                    config.set(key, ss.str());
                } else if (item.second.IsMap()) {
                    // Convert YAML map to string representation
                    std::stringstream ss;
                    ss << "{";
                    bool first = true;
                    for (const auto& element : item.second) {
                        if (!first) ss << ", ";
                        if (element.first.IsScalar()) {
                            ss << "\"" << element.first.as<std::string>() << "\": \"";
                        }
                        if (element.second.IsScalar()) {
                            ss << element.second.as<std::string>() << "\"";
                        }
                        first = false;
                    }
                    ss << "}";
                    config.set(key, ss.str());
                }
            }
        }
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}
#endif

}} // namespace

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Enhanced loadFromFile with support for JSON, YAML, and simple formats
bool Config::loadFromFile(const std::string& filepath) {
    FileFormat format = detectFileFormat(filepath);
    
    bool success = false;
    
    switch (format) {
        case FileFormat::Simple:
            // Original simple key=value parser
            {
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
                        success = true;
                    }
                }
            }
            break;
            
#ifdef HAVE_NLOHMANN_JSON
        case FileFormat::JSON:
            return parseJSONFile(filepath, *this);
            
#endif
            
#ifdef HAVE_YAML_CPP
        case FileFormat integrator:
            return parseYAMLFile(filepath, *this);
            
#endif
            
        default:
            // Fall back to simple format
            return loadFromFile(filepath);
    }
    
    return success;
}

// Enhance argument parsing to handle more formats and provide better error messages
bool Config::loadFromArgs(int argc, char** argv) {
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Validate key format (allow alphanumeric, underscores, dots)
                if (!std::all_of(key.begin(), key.end(), [](char c) {
                    return std::isalnum(c) || c == '_' || c == '.' || c == '-';
                })) {
                    continue; // Skip invalid key
                }
                
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Validate key format
            if (!std::all_of(key.begin(), key.end(), [](char c) {
                return std::isalnum(c) || c == '_' || c == '.' || c == '-';
            })) {
                continue; // Skip invalid key
            }
            
            set(key, value, ConfigSource::CommandLine);
        }
        else if (arg.substr(0, 2) == "--help" || arg == "--help") {
            printUsage();
        }
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Save configuration in simple key=value format
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Convert value to string representation
        std::string valueStr;
        std::visit([&valueStr](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                valueStr = "\"" + arg + "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                valueStr = "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) valueStr += ", ";
                    if constexpr (std::is_same_v<T, std::vector<std::string>) {
                        valueStr += "\"" + arg[i] + "\"";
                    } else {
                        valueStr += std::to_string(arg[i]);
                    }
                }
                valueStr += "]";
            } else {
                valueStr = std::to_string(arg);
            }
        }, entry.value);
        
        file << entry.key << " = " << valueStr << "\n\n";
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

template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

// Configuration validation and helper function implementations
namespace nlm {

void Config::printUsage() {
    std::cout << "NLM Configuration Usage:" << std::endl;
    std::cout << "  --key=value        Set configuration parameter" << std::endl;
    std::cout << "  --help             Show this help message" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Common configuration options:" << std::endl;
    std::cout << "  --brain.neuron_count=N        Number of neurons (e.g., 1000)" << std::endl;
    std::cout << "  --brain.connection_probability=P  Connection probability (e.g., 0.1)" << std::endl;
    std::cout << "  --plasticity.stdp.learning_rate=R  STDP learning rate" << std::endl;
    std::cout << "  --neuromod.dopamine.scale=S    Dopamine modulation scale" << std::endl;
}

bool Config::validate() const {
    // Basic validation: check that all keys are valid
    for (const auto& entry : pImpl->entries) {
        if (!validateKey(entry.key)) {
            return false;
        }
    }
    return true;
}

template<typename T>
std::optional<T> Config::getValidated(const std::string& key, const T& defaultValue,
                                      std::function<bool(const T&)> validator) const {
    auto val = get<T>(key);
    if (!val.has_value()) {
        return defaultValue;
    }
    
    if (validator && !validator(val.value())) {
        return defaultValue;
    }
    
    return val.value();
}

template<typename T>
bool Config::setValidated(const std::string& key, const T& value,
                          ConfigSource source,
                          std::function<bool(const T&)> validator) {
    if (!validateKey(key)) {
        return false;
    }
    
    if (validator && !validator(value)) {
        return false;
    }
    
    set(key, value, source);
    return true;
}

std::string Config::valueToString(const ConfigValue& value) const {
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return "[int_array]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return "[double_array]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return "[string_array]";
        }
    }, value);
    return "";
}

} // namespace nlm
