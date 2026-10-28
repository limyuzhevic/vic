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
    // Try JSON first (files ending with .json or containing JSON syntax)
    if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".json") {
        return loadFromJSON(filepath);
    }
    
    // Try YAML (files ending with .yaml or .yml)
    std::string ext = "";
    if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".yaml") {
        ext = ".yaml";
    } else if (filepath.size() >= 4 && filepath.substr(filepath.size() - 4) == ".yml") {
        ext = ".yml";
    }
    
    if (!ext.empty()) {
        return loadFromYAML(filepath);
    }
    
    // Fallback to simple key=value format for backward compatibility
    NLM_LOG_INFO("Loading configuration from simple key=value format: " + filepath);
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARN("Could not open configuration file: " + filepath);
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        ++lineNum;
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
            
            // Try to convert to appropriate type
            bool converted = false;
            // Check for boolean
            if (value == "true" || value == "false") {
                set(key, value == "true", ConfigSource::File);
                converted = true;
            }
            // Check for integer
            else {
                try {
                    size_t endpos;
                    int intVal = std::stoi(value, &endpos);
                    if (endpos == value.size()) {
                        set(key, intVal, ConfigSource::File);
                        converted = true;
                    }
                } catch (...) {}
                
                if (!converted) {
                    try {
                        size_t endpos;
                        double doubleVal = std::stod(value, &endpos);
                        if (endpos == value.size()) {
                            set(key, doubleVal, ConfigSource::File);
                            converted = true;
                        }
                    } catch (...) {}
                }
            }
            
            if (!converted) {
                // Try to parse as vector (comma-separated)
                if (value.find(',') != std::string::npos) {
                    std::vector<double> vec;
                    std::stringstream ss(value);
                    std::string item;
                    while (std::getline(ss, item, ',')) {
                        item = trim(item);
                        try {
                            double val = std::stod(item);
                            vec.push_back(val);
                        } catch (...) {
                            // Skip invalid items
                        }
                    }
                    if (!vec.empty()) {
                        set(key, vec, ConfigSource::File);
                        converted = true;
                    }
                }
            }
            
            if (!converted) {
                // Use as string
                set(key, value, ConfigSource::File);
            }
        } else {
            NLM_LOG_WARN("Invalid config line in " + filepath + ":" + std::to_string(lineNum) + ": " + line);
        }
    }
    
    NLM_LOG_INFO("Loaded " + std::to_string(pImpl->entries.size()) + " configuration entries");
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
        NLM_LOG_WARN("Could not open configuration file for writing: " + filepath);
        return false;
    }
    
    // Determine format based on file extension
    std::string ext = "";
    if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".json") {
        ext = ".json";
    } else if (filepath.size() >= 4 && filepath.substr(filepath.size() - 4) == ".yml") {
        ext = ".yml";
    } else if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".yaml") {
        ext = ".yaml";
    }
    
    bool saved = false;
    if (!ext.empty()) {
        // Use format-specific saver
        if (ext == ".json") {
            saved = saveToJSON(filepath);
        } else if (ext == ".yml" || ext == ".yaml") {
            saved = saveToYAML(filepath);
        }
    }
    
    if (!saved) {
        // Fallback to simple key=value format
        NLM_LOG_INFO("Saving configuration in simple key=value format: " + filepath);
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
            
            // Format value based on type
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else if constexpr (std::is_same_v<T, double>) {
                    file << arg;
                } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    file << arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    file << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << arg[i];
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    file << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << arg[i];
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    file << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << "\"" << arg[i] << "\"";
                    }
                    file << "]";
                }
            }, entry.value);
            file << "\n\n";
        }
    }
    
    return true;
}

// Load from JSON file (full implementation)
    bool loadFromJSON(const std::string& filepath) {
        NLM_LOG_INFO("Loading configuration from JSON: " + filepath);
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_WARN("Could not open JSON configuration file: " + filepath);
            return false;
        }
        
        // Parse JSON with a simple approach
        nlohmann::json jsonData;
        try {
            file >> jsonData;
        } catch (const nlohmann::json::parse_error& e) {
            NLM_LOG_WARN("Failed to parse JSON config file " + filepath + ": " + e.what());
            return false;
        }
        
        // Recursively process JSON object
        processJSONConfig(jsonData, ConfigSource::File);
        
        NLM_LOG_INFO("Loaded " + std::to_string(pImpl->entries.size()) + " configuration entries from JSON");
        return true;
    }
    
    // Load from YAML file (full implementation)
    bool loadFromYAML(const std::string& filepath) {
        NLM_LOG_INFO("Loading configuration from YAML: " + filepath);
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_WARN("Could not open YAML configuration file: " + filepath);
            return false;
        }
        
        // Parse YAML
        YAML::Node yamlData;
        try {
            yamlData = YAML::LoadFile(filepath);
        } catch (const YAML::Exception& e) {
            NLM_LOG_WARN("Failed to parse YAML config file " + filepath + ": " + e.what());
            return false;
        }
        
        // Recursively process YAML node
        processYAMLConfig(yamlData, ConfigSource::File);
        
        NLM_LOG_INFO("Loaded " + std::to_string(pImpl->entries.size()) + " configuration entries from YAML");
        return true;
    }
    
    // Save to JSON file (full implementation)
    bool saveToJSON(const std::string& filepath) const {
        NLM_LOG_INFO("Saving configuration to JSON: " + filepath);
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_WARN("Could not open JSON configuration file for writing: " + filepath);
            return false;
        }
        
        nlohmann::json jsonData;
        
        // Convert config entries to JSON
        for (const auto& entry : pImpl->entries) {
            jsonData[entry.key] = getJSONValue(entry.value);
            jsonData[entry.key]."_source" = static_cast<int>(entry.source);
            if (!entry.description.empty()) {
                jsonData[entry.key]."_description" = entry.description;
            }
        }
        
        // Write JSON to file with pretty formatting
        file << std::setw(2) << jsonData << std::endl;
        
        return true;
    }
    
    // Save to YAML file (full implementation)
    bool saveToYAML(const std::string& filepath) const {
        NLM_LOG_INFO("Saving configuration to YAML: " + filepath);
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_WARN("Could not open YAML configuration file for writing: " + filepath);
            return false;
        }
        
        YAML::Emitter emitter;
        emitter.SetIndent(2);
        
        emitter << YAML::BeginMap;
        for (const auto& entry : pImpl->entries) {
            emitter << YAML::Key << entry.key;
            emitter << YAML::Value << getYAMLValue(entry.value);
            
            // Add source as a comment
            emitter << YAML::Comment("Source: " + std::to_string(static_cast<int>(entry.source)));
            if (!entry.description.empty()) {
                emitter << YAML::Comment("Description: " + entry.description);
            }
        }
        emitter << YAML::EndMap;
        
        file << emitter.c_str();
        
        return true;
    }
    
    // Process JSON object and add to config
    void processJSONConfig(const nlohmann::json& json, ConfigSource source) {
        if (json.is_object()) {
            for (auto it = json.begin(); it != json.end(); ++it) {
                std::string key = it.key();
                if (key == "_source" || key == "_description") {
                    continue;  // Skip metadata keys
                }
                
                const nlohmann::json& value = it.value();
                ConfigValue configValue;
                
                if (value.is_boolean()) {
                    configValue = value.get<bool>();
                } else if (value.is_number_integer()) {
                    configValue = value.get<int64_t>();
                } else if (value.is_number_unsigned()) {
                    configValue = value.get<uint64_t>();
                } else if (value.is_number_float()) {
                    configValue = value.get<double>();
                } else if (value.is_string()) {
                    configValue = value.get<std::string>();
                } else if (value.is_array()) {
                    std::vector<double> vec;
                    for (const auto& item : value) {
                        if (item.is_number()) {
                            vec.push_back(item.get<double>());
                        }
                    }
                    configValue = vec;
                } else if (value.is_null()) {
                    configValue = std::string("");
                }
                
                set(key, configValue, source);
                
                // Get description if present
                std::string description = "";
                if (value.is_object() && value.contains("_description")) {
                    description = value["_description"].get<std::string>();
                    setDescription(key, description);
                }
            }
        }
    }
    
    // Process YAML node and add to config
    void processYAMLConfig(const YAML::Node& node, ConfigSource source) {
        if (!node.IsMap()) {
            return;
        }
        
        for (const auto& it : node) {
            std::string key = it.first.as<std::string>();
            
            YAML::Node value = it.second;
            ConfigValue configValue;
            
            if (value.IsScalar()) {
                // Try to determine type from scalar
                std::string strVal = value.as<std::string>();
                
                // Check for boolean
                if (strVal == "true" || strVal == "false") {
                    configValue = (strVal == "true");
                }
                // Check for number
                else {
                    try {
                        size_t endpos;
                        int intVal = std::stoi(strVal, &endpos);
                        if (endpos == strVal.size()) {
                            configValue = intVal;
                        } else {
                            double doubleVal = std::stod(strVal, &endpos);
                            if (endpos == strVal.size()) {
                                configValue = doubleVal;
                            } else {
                                configValue = strVal;
                            }
                        }
                    } catch (...) {
                        configValue = strVal;
                    }
                }
            } else if (value.IsSequence()) {
                std::vector<double> vec;
                for (size_t i = 0; i < value.size(); ++i) {
                    try {
                        vec.push_back(value[i].as<double>());
                    } catch (...) {}
                }
                configValue = vec;
            } else {
                // Complex YAML structure, store as string representation
                configValue = value.as<std::string>();
            }
            
            set(key, configValue, source);
            
            // Get description if present in comment
            if (value.GetComment()) {
                std::string comment = value.GetComment();
                if (comment.find("Description: ") == 0) {
                    std::string description = comment.substr(13);
                    setDescription(key, description);
                }
            }
        }
    }
    
    // Get JSON representation of config value
    nlohmann::json getJSONValue(const ConfigValue& value) const {
        return std::visit([](auto&& arg) -> nlohmann::json {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, bool>) {
                return arg;
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>) {
                return arg;
            } else if constexpr (std::is_same_v<T, double>) {
                return arg;
            } else if constexpr (std::is_same_v<T, std::string>) {
                return arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                nlohmann::json arr = nlohmann::json::array();
                for (const auto& item : arg) {
                    arr.push_back(item);
                }
                return arr;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                nlohmann::json arr = nlohmann::json::array();
                for (const auto& item : arg) {
                    arr.push_back(item);
                }
                return arr;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                nlohmann::json arr = nlohmann::json::array();
                for (const auto& item : arg) {
                    arr.push_back(item);
                }
                return arr;
            } else {
                return "UNKNOWN_TYPE";
            }
        }, value);
    }
    
    // Get YAML representation of config value
    YAML::Node getYAMLValue(const ConfigValue& value) const {
        YAML::Node node;
        
        node.SetTag("!");  // Generic type
        
        bool set = false;
        set = std::visit([&node](auto&& arg) -> bool {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, bool>) {
                node = arg;
                return true;
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>) {
                node = static_cast<int>(arg);
                return true;
            } else if constexpr (std::is_same_v<T, double>) {
                node = arg;
                return true;
            } else if constexpr (std::is_same_v<T, std::string>) {
                node = arg;
                return true;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                YAML::Node arr = YAML::Node(YAML::NodeType::Sequence);
                for (const auto& item : arg) {
                    arr.push_back(item);
                }
                node = arr;
                return true;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                YAML::Node arr = YAML::Node(YAML::NodeType::Sequence);
                for (const auto& item : arg) {
                    arr.push_back(item);
                }
                node = arr;
                return true;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                YAML::Node arr = YAML::Node(YAML::NodeType::Sequence);
                for (const auto& item : arg) {
                    arr.push_back(item);
                }
                node = arr;
                return true;
            } else {
                return false;
            }
        }, value);
        
        return node;
    }
    
    // Set description for a configuration key
    void setDescription(const std::string& key, const std::string& description) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it != pImpl->entries.end()) {
            it->description = description;
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
