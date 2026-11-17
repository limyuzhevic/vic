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
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Helper to convert JSON value to ConfigValue
static ConfigValue jsonToConfigValue(const nlohmann::json& j) {
    if (j.is_string()) {
        return j.get<std::string>();
    } else if (j.is_number_integer()) {
        return j.get<int64_t>();
    } else if (j.is_number_unsigned()) {
        return j.get<uint64_t>();
    } else if (j.is_number_float()) {
        return j.get<double>();
    } else if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_array()) {
        // Handle arrays - determine element type
        if (!j.empty() && j[0].is_number_integer()) {
            std::vector<int64_t> vec;
            vec.reserve(j.size());
            for (const auto& elem : j) {
                vec.push_back(elem.get<int64_t>());
            }
            return vec;
        } else if (!j.empty() && j[0].is_number_float()) {
            std::vector<double> vec;
            vec.reserve(j.size());
            for (const auto& elem : j) {
                vec.push_back(elem.get<double>());
            }
            return vec;
        } else if (!j.empty() && j[0].is_string()) {
            std::vector<std::string> vec;
            vec.reserve(j.size());
            for (const auto& elem : j) {
                vec.push_back(elem.get<std::string>());
            }
            return vec;
        } else {
            // Empty array
            return std::vector<int64_t>{};
        }
    } else {
        // Fallback to string
        return j.get<std::string>();
    }
}

// Helper to convert YAML node to ConfigValue
static ConfigValue yamlToConfigValue(const YAML::Node& node) {
    if (node.IsScalar()) {
        const std::string& value = node.Scalar();
        // Try to detect numeric types
        try {
            // Try to parse as integer
            size_t pos;
            int64_t intVal = std::stoll(value, &pos);
            if (pos == value.size()) {
                return intVal;
            }
        } catch (...) {}
        
        try {
            // Try to parse as double
            size_t pos;
            double doubleVal = std::stod(value, &pos);
            if (pos == value.size()) {
                return doubleVal;
            }
        } catch (...) {}
        
        if (value == "true" || value == "false") {
            return (value == "true");
        }
        
        return value;
    } else if (node.IsSequence()) {
        // Check element type
        if (node.size() > 0) {
            if (node[0].IsScalar()) {
                // Determine if numeric
                const std::string& firstVal = node[0].Scalar();
                bool allNumeric = true;
                bool allIntegers = true;
                for (const auto& elem : node) {
                    if (!elem.IsScalar()) {
                        allNumeric = false;
                        break;
                    }
                    const std::string& val = elem.Scalar();
                    try {
                        std::stoll(val);
                    } catch (...) {
                        allIntegers = false;
                    }
                    try {
                        std::stod(val);
                    } catch (...) {
                        allNumeric = false;
                    }
                }
                
                if (allIntegers) {
                    std::vector<int64_t> vec;
                    vec.reserve(node.size());
                    for (const auto& elem : node) {
                        vec.push_back(std::stoll(elem.Scalar()));
                    }
                    return vec;
                } else if (allNumeric) {
                    std::vector<double> vec;
                    vec.reserve(node.size());
                    for (const auto& elem : node) {
                        vec.push_back(std::stod(elem.Scalar()));
                    }
                    return vec;
                } else {
                    // Treat as strings
                    std::vector<std::string> vec;
                    vec.reserve(node.size());
                    for (const auto& elem : node) {
                        vec.push_back(elem.Scalar());
                    }
                    return vec;
                }
            }
        }
        // Empty sequence
        return std::vector<int64_t>{};
    } else if (node.IsMap()) {
        // Convert map to string representation for now
        // TODO: Implement proper map support
        std::ostringstream oss;
        oss << "{";
        bool first = true;
        for (const auto& pair : node) {
            if (!first) oss << ", ";
            oss << pair.first.Scalar() << ": ";
            if (pair.second.IsScalar()) {
                oss << pair.second.Scalar();
            } else {
                oss << "[complex]";
            }
            first = false;
        }
        oss << "}";
        return oss.str();
    }
    
    return std::string();
}

bool Config::loadFromFile(const std::string& filepath) {
    // Detect file format
    ConfigFormat format = detectConfigFormat(filepath);
    
    if (format == ConfigFormat::JSON) {
        return loadFromJSON(filepath);
    } else if (format == ConfigFormat::YAML) {
        return loadFromYAML(filepath);
    } else {
        // Fallback to simple key=value format
        return loadFromSimpleFile(filepath);
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open JSON config file: " + filepath);
        return false;
    }
    
    try {
        nlohmann::json j;
        file >> j;
        
        if (!j.is_object()) {
            NLM_LOG_ERROR("JSON config file must contain a JSON object");
            return false;
        }
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Parse JSON object
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            ConfigValue value = jsonToConfigValue(it.value());
            
            // Extract description from comment if present
            std::string description = "";
            
            set(key, value, ConfigSource::File);
        }
        
        NLM_LOG_INFO("Loaded JSON configuration from: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing JSON config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    try {
        YAML::Node doc = YAML::LoadFile(filepath);
        
        if (!doc.IsMap()) {
            NLM_LOG_ERROR("YAML config file must contain a YAML mapping");
            return false;
        }
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Parse YAML mapping
        for (const auto& pair : doc) {
            std::string key = pair.first.as<std::string>();
            ConfigValue value = yamlToConfigValue(pair.second);
            
            set(key, value, ConfigSource::File);
        }
        
        NLM_LOG_INFO("Loaded YAML configuration from: " + filepath);
        return true;
        
    } catch (const YAML::Exception& e) {
        NLM_LOG_ERROR("Error parsing YAML config file " + filepath + ": " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading YAML config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromSimpleFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open config file: " + filepath);
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
    
    NLM_LOG_INFO("Loaded simple configuration from: " + filepath);
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
    // Detect file format based on extension
    std::filesystem::path path(filepath);
    std::string ext = path.extension().string();
    
    if (ext == ".json" || ext == ".JSON") {
        return saveToJSON(filepath);
    } else if (ext == ".yaml" || ext == ".yml" || ext == ".YAML" || ext == ".YML") {
        return saveToYAML(filepath);
    } else {
        // Default to simple format
        return saveToSimpleFile(filepath);
    }
}

bool Config::saveToJSON(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open JSON config file for writing: " + filepath);
        return false;
    }
    
    try {
        nlohmann::json j;
        
        for (const auto& entry : pImpl->entries) {
            // Add description as comment if present
            if (!entry.description.empty()) {
                j[entry.key] = entry.value;
                // Note: nlohmann/json doesn't support comments directly
                // For production, you might want to use a different library
            } else {
                j[entry.key] = entry.value;
            }
        }
        
        file << std::setw(2) << j << std::endl;
        NLM_LOG_INFO("Saved JSON configuration to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error writing JSON config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToYAML(const std::string& filepath) const {
    try {
        YAML::Emitter emitter;
        emitter << YAML::BeginMap;
        
        for (const auto& entry : pImpl->entries) {
            emitter << YAML::Key << entry.key << YAML::Value;
            
            // Convert ConfigValue to YAML node
            const ConfigValue& value = entry.value;
            std::visit([&emitter](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    emitter << arg;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    emitter << static_cast<int>(arg);
                } else if constexpr (std::is_same_v<T, uint64_t>) {
                    emitter << static_cast<uint64_t>(arg);
                } else if constexpr (std::is_same_v<T, double>) {
                    emitter << arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    emitter << arg;
                } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
                    emitter << YAML::BeginSeq;
                    for (const auto& item : arg) {
                        emitter << static_cast<int>(item);
                    }
                    emitter << YAML::EndSeq;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    emitter << YAML::BeginSeq;
                    for (const auto& item : arg) {
                        emitter << item;
                    }
                    emitter << YAML::EndSeq;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    emitter << YAML::BeginSeq;
                    for (const auto& item : arg) {
                        emitter << item;
                    }
                    emitter << YAML::EndSeq;
                } else {
                    emitter << "[unknown]";
                }
            }, value);
        }
        
        emitter << YAML::EndMap;
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open YAML config file for writing: " + filepath);
            return false;
        }
        
        file << emitter.c_str();
        NLM_LOG_INFO("Saved YAML configuration to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error writing YAML config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToSimpleFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open config file for writing: " + filepath);
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << entry.value << "\n\n";
    }
    
    NLM_LOG_INFO("Saved simple configuration to: " + filepath);
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
            } else if constexpr (std::is_same_v<T, std::vector<int64_t>> ||
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

} // namespace nlm

