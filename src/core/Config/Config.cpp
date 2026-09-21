#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::string> descriptions;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Detect file format
    std::string format = detectFileFormat(filepath);
    
    clear();
    
    if (format == "json") {
        return loadFromJSON(filepath);
    } else if (format == "yaml" || format == "yml") {
        return loadFromYAML(filepath);
    } else {
        // Fall back to simple key=value format for .cfg files
        return loadFromSimpleFormat(filepath);
    }
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
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

std::string Config::detectFileFormat(const std::string& filepath) {
    if (filepath.empty()) {
        return "unknown";
    }
    
    std::string extension = toLower(filepath);
    
    // Check for JSON files
    if (extension.find(".json") != std::string::npos) {
        return "json";
    } else if (extension.find(".yaml") != std::string::npos) {
        return "yaml";
    } else if (extension.find(".yml") != std::string::npos) {
        return "yaml";
    } else if (extension.find(".cfg") != std::string::npos || 
               extension.find(".conf") != std::string::npos) {
        return "simple";
    }
    
    // Try to detect by content if extension unknown
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return "unknown";
    }
    
    // Peek at first few characters
    std::string firstChars(1000, '\0');
    file.read(&firstChars[0], 1000);
    file.close();
    
    // Check for JSON
    if (firstChars.find("{") != std::string::npos && firstChars.find("}") != std::string::npos) {
        return "json";
    }
    
    // Check for YAML (common YAML indicators)
    if (firstChars.find(":") != std::string::npos && 
        (firstChars.find("-") != std::string::npos || firstChars.find("*") != std::string::npos)) {
        return "yaml";
    }
    
    return "simple";
}

bool Config::loadFromJSON(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Clear existing entries
        clear();
        
        // Parse JSON object
        if (jsonData.is_object()) {
            for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
                std::string key = it.key();
                const auto& value = it.value();
                
                // Convert JSON value to ConfigValue
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
                    // Handle arrays
                    std::vector<ConfigValue> arrayValue;
                    for (const auto& item : value) {
                        if (item.is_string()) {
                            arrayValue.push_back(item.get<std::string>());
                        } else if (item.is_number_integer()) {
                            arrayValue.push_back(item.get<int64_t>());
                        } else if (item.is_number_float()) {
                            arrayValue.push_back(item.get<double>());
                        } else if (item.is_boolean()) {
                            arrayValue.push_back(item.get<bool>());
                        }
                    }
                    configValue = arrayValue;
                } else {
                    // Default to string for unknown types
                    configValue = value.dump();
                }
                
                set(key, configValue, ConfigSource::File);
                
                // Store description if present
                if (value.contains("_comment") && value["_comment"].is_string()) {
                    pImpl->descriptions[key] = value["_comment"].get<std::string>();
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        // Silently fail for now - TODO: Add proper error handling
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    try {
        YAML::Node yamlData = YAML::LoadFile(filepath);
        
        // Clear existing entries
        clear();
        
        // Parse YAML mapping
        if (yamlData.IsMap()) {
            for (const auto& it : yamlData) {
                std::string key = it.first.as<std::string>();
                const YAML::Node& value = it.second;
                
                // Convert YAML node to ConfigValue
                ConfigValue configValue;
                
                if (value.IsScalar()) {
                    std::string strValue = value.as<std::string>();
                    // Try to infer type
                    if (strValue == "true" || strValue == "false") {
                        configValue = strValue == "true";
                    } else {
                        try {
                            size_t pos;
                            int intVal = std::stoi(strValue, &pos);
                            if (pos == strValue.size()) {
                                configValue = intVal;
                            } else {
                                double doubleVal = std::stod(strValue, &pos);
                                if (pos == strValue.size()) {
                                    configValue = doubleVal;
                                } else {
                                    configValue = strValue;
                                }
                            }
                        } catch (...) {
                            configValue = strValue;
                        }
                    }
                } else if (value.IsSequence()) {
                    // Handle sequences
                    std::vector<ConfigValue> arrayValue;
                    for (const auto& item : value) {
                        if (item.IsScalar()) {
                            std::string strItem = item.as<std::string>();
                            if (strItem == "true" || strItem == "false") {
                                arrayValue.push_back(strItem == "true");
                            } else {
                                try {
                                    size_t pos;
                                    int intItem = std::stoi(strItem, &pos);
                                    if (pos == strItem.size()) {
                                        arrayValue.push_back(intItem);
                                    } else {
                                        double doubleItem = std::stod(strItem, &pos);
                                        if (pos == strItem.size()) {
                                            arrayValue.push_back(doubleItem);
                                        } else {
                                            arrayValue.push_back(strItem);
                                        }
                                    }
                                } catch (...) {
                                    arrayValue.push_back(strItem);
                                }
                            }
                        }
                    }
                    configValue = arrayValue;
                } else {
                    configValue = value.as<std::string>();
                }
                
                set(key, configValue, ConfigSource::File);
                
                // Store description if present
                if (value["_comment"] && value["_comment"].IsScalar()) {
                    pImpl->descriptions[key] = value["_comment"].as<std::string>();
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        // Silently fail for now - TODO: Add proper error handling
        return false;
    }
}

// Add default config loading and improved argument parsing
void Config::loadFromArgs(int argc, char** argv) {
    // Parse command-line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle help flag
        if (arg == "--help" || arg == "-h") {
            printHelp();
            std::exit(0);
        }
        
        // Handle version flag
        if (arg == "--version") {
            std::cout << "NLM Config System v1.0" << std::endl;
            std::exit(0);
        }
        
        // Handle config file argument
        if (arg == "--config" && i + 1 < argc) {
            std::string configFile = argv[++i];
            if (!loadFromFile(configFile)) {
                std::cerr << "Error: Failed to load config file: " << configFile << std::endl;
                return;
            }
            continue;
        }
        
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
        // Handle boolean flags
        else if (arg[0] == '-' && arg[1] != '-') {
            // Simple boolean flag - key is the flag name
            std::string key = arg.substr(1);
            set(key, true, ConfigSource::CommandLine);
        }
    }
}

void Config::addDefaultConfig(const std::string& key, const ConfigValue& value) {
    // Add a default configuration value
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        pImpl->entries.emplace_back(key, value, ConfigSource::Default);
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Determine file format based on extension
    std::string extension = toLower(filepath);
    bool isJSON = extension.find(".json") != std::string::npos;
    bool isYAML = extension.find(".yaml") != std::string::npos || extension.find(".yml") != std::string::npos;
    
    if (isJSON) {
        nlohmann::json jsonData;
        
        for (const auto& entry : pImpl->entries) {
            // Add description as comment
            if (!entry.description.empty()) {
                jsonData[entry.key]["_comment"] = entry.description;
            }
            
            // Add the value
            std::visit([&](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    jsonData[entry.key] = val;
                } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
                    jsonData[entry.key] = val;
                } else if constexpr (std::is_same_v<T, double>) {
                    jsonData[entry.key] = val;
                } else if constexpr (std::is_same_v<T, bool>) {
                    jsonData[entry.key] = val;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    jsonData[entry.key] = val;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    jsonData[entry.key] = val;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    jsonData[entry.key] = val;
                }
            }, entry.value);
        }
        
        file << jsonData.dump(4);
    } else if (isYAML) {
        YAML::Emitter out;
        out << YAML::BeginMap;
        
        for (const auto& entry : pImpl->entries) {
            out << YAML::Key << entry.key;
            
            std::visit([&](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    out << val;
                } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
                    out << static_cast<int>(val);
                } else if constexpr (std::is_same_v<T, double>) {
                    out << val;
                } else if constexpr (std::is_same_v<T, bool>) {
                    out << val;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    out << YAML::FlowSeq;
                    for (const auto& item : val) {
                        out << item;
                    }
                    out << YAML::EndSeq;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    out << YAML::FlowSeq;
                    for (const auto& item : val) {
                        out << item;
                    }
                    out << YAML::EndSeq;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    out << YAML::FlowSeq;
                    for (const auto& item : val) {
                        out << item;
                    }
                    out << YAML::EndSeq;
                }
            }, entry.value);
            
            out << YAML::Key << "_comment" << entry.description;
            out << YAML::Key << "_source" << static_cast<int>(entry.source);
        }
        
        out << YAML::EndMap;
        file << out.c_str();
    } else {
        // Default to simple key=value format
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
            
            std::visit([&](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << val << "\"";
                } else {
                    file << val;
                }
            }, entry.value);
            file << "\n\n";
        }
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

// Add validation and type checking methods
bool Config::validateKey(const std::string& key) const {
    if (key.empty()) {
        return false;
    }
    // Allow only alphanumeric characters and underscores
    return std::all_of(key.begin(), key.end(),
        [](char c) { return std::isalnum(c) || c == '_'; });
}

bool Config::validateValue(const std::string& key, const ConfigValue& value) const {
    // Basic validation - can be expanded with more specific rules
    if (key.empty()) {
        return false;
    }
    
    // Type-specific validation
    std::visit([this, &key](auto&& val) {
        using T = std::decay_t<decltype(val)>;
        
        if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
            if (val < -1000000 || val > 1000000) {
                throw std::out_of_range("Integer value out of valid range");
            }
        } else if constexpr (std::is_same_v<T, double>) {
            if (std::isnan(val) || std::isinf(val)) {
                throw std::invalid_argument("Double value is NaN or infinity");
            }
            if (val < -1000000.0 || val > 1000000.0) {
                throw std::out_of_range("Double value out of valid range");
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            // Nothing special to validate for bool
        } else if constexpr (std::is_same_v<T, std::string>) {
            if (val.empty()) {
                throw std::invalid_argument("String value cannot be empty for key: " + key);
            }
            if (val.size() > 1000) {
                throw std::out_of_range("String value too long for key: " + key);
            }
        } else if constexpr (std::is_same_v<T, std::vector<int64_t>> ||
                             std::is_same_v<T, std::vector<int>>) {
            if (val.size() > 100) {
                throw std::out_of_range("Vector size too large for key: " + key);
            }
            for (const auto& item : val) {
                if (item < -1000000 || item > 1000000) {
                    throw std::out_of_range("Vector item out of range for key: " + key);
                }
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            if (val.size() > 100) {
                throw std::out_of_range("Vector size too large for key: " + key);
            }
            for (const auto& item : val) {
                if (std::isnan(item) || std::isinf(item)) {
                    throw std::invalid_argument("NaN or infinity in vector for key: " + key);
                }
                if (item < -1000000.0 || item > 1000000.0) {
                    throw std::out_of_range("Vector item out of range for key: " + key);
                }
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            if (val.size() > 100) {
                throw std::out_of_range("Vector size too large for key: " + key);
            }
            for (const auto& item : val) {
                if (item.empty()) {
                    throw std::invalid_argument("Empty string in vector for key: " + key);
                }
                if (item.size() > 100) {
                    throw std::out_of_range("String in vector too long for key: " + key);
                }
            }
        }
    }, value);
    
    return true;
}

bool Config::validateAll() const {
    for (const auto& entry : pImpl->entries) {
        try {
            if (!validateKey(entry.key)) {
                throw std::invalid_argument("Invalid key format: " + entry.key);
            }
            if (!validateValue(entry.key, entry.value)) {
                throw std::invalid_argument("Invalid value for key: " + entry.key);
            }
        } catch (const std::exception& e) {
            // In production, would log the error
            return false;
        }
    }
    return true;
}

void Config::printHelp() const {
    std::cout << "=== NLM Configuration Help ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Configuration files can be in one of these formats:" << std::endl;
    std::cout << "  - JSON (.json): Key-value pairs with arrays and objects" << std::endl;
    std::cout << "  - YAML (.yaml/.yml): Indentation-based format" << std::endl;
    std::cout << "  - Simple (.cfg/.conf): key = value format" << std::endl;
    std::cout << std::endl;
    std::cout << "File format is automatically detected." << std::endl;
    std::cout << std::endl;
    std::cout << "Command-line arguments:" << std::endl;
    std::cout << "  --key=value or -key value" << std::endl;
    std::cout << std::endl;
    std::cout << "Configuration values supported:" << std::endl;
    std::cout << "  - integers (e.g., 42)" << std::endl;
    std::cout << "  - floating-point numbers (e.g., 3.14)" << std::endl;
    std::cout << "  - booleans (true/false)" << std::endl;
    std::cout << "  - strings (e.g., \"hello\")" << std::endl;
    std::cout << "  - arrays of numbers or strings (e.g., [1, 2, 3] or [a, b, c])" << std::endl;
    std::cout << std::endl;
    std::cout << "Configuration sources (in order of precedence):" << std::endl;
    std::cout << "  1. Command-line arguments (highest priority)" << std::endl;
    std::cout << "  2. Configuration file (middle priority)" << std::endl;
    std::cout << "  3. Default values (lowest priority)" << std::endl;
    std::cout << std::endl;
    std::cout << "Example usage:" << std::endl;
    std::cout << "  ./program --random_seed=42 --neuron_count=1000" << std::endl;
    std::cout << "  ./program --simulation_timestep=0.01 config.json" << std::endl;
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    pImpl->descriptions.erase(key);
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
    pImpl->descriptions.clear();
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
                    if (i > 0) oss << ", ";
                    if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        oss << "\"" << arg[i] << "\"";
                    } else {
                        oss << arg[i];
                    }
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
