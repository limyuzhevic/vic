#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <ctime>
#include <unordered_map>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, ConfigEntry> entryMap;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    clear();
    lastError = ConfigError::None;
    lastErrorMsg.clear();
    
    if (filepath.empty()) {
        lastError = ConfigError::FileNotFound;
        lastErrorMsg = "Filepath is empty";
        return false;
    }
    
    // Detect file format
    std::string format = detectFormat(filepath);
    
    if (format == "json" && std::filesystem::exists(filepath)) {
#ifdef NLM_WITH_JSON
        try {
            auto json = parseJsonFile(filepath);
            // Convert JSON to ConfigEntry vector
            for (auto it = json.begin(); it != json.end(); ++it) {
                std::string key = it.key();
                ConfigValue value;
                
                if (it->is_boolean()) {
                    value = it->get<bool>();
                } else if (it->is_number_integer()) {
                    value = it->get<int64_t>();
                } else if (it->is_number_float()) {
                    value = it->get<double>();
                } else if (it->is_string()) {
                    value = it->get<std::string>();
                } else if (it->is_array()) {
                    // Try to infer array type
                    const auto& arr = *it;
                    if (!arr.empty()) {
                        if (arr[0].is_boolean()) {
                            std::vector<bool> boolVec;
                            for (const auto& item : arr) boolVec.push_back(item.get<bool>());
                            value = boolVec;
                        } else if (arr[0].is_number_integer()) {
                            std::vector<int64_t> intVec;
                            for (const auto& item : arr) intVec.push_back(item.get<int64_t>());
                            value = intVec;
                        } else if (arr[0].is_number_float()) {
                            std::vector<double> doubleVec;
                            for (const auto& item : arr) doubleVec.push_back(item.get<double>());
                            value = doubleVec;
                        } else if (arr[0].is_string()) {
                            std::vector<std::string> stringVec;
                            for (const auto& item : arr) stringVec.push_back(item.get<std::string>());
                            value = stringVec;
                        } else {
                            value = std::vector<std::string>();
                        }
                    } else {
                        value = std::vector<std::string>();
                    }
                } else {
                    value = "N/A: Unsupported JSON type";
                }
                
                pImpl->entries.emplace_back(key, value, ConfigSource::File, "Loaded from JSON");
                pImpl->entryMap[key] = pImpl->entries.back();
            }
            return true;
        } catch (const std::exception& e) {
            lastError = ConfigError::ParseError;
            lastErrorMsg = "JSON parse error: " + std::string(e.what());
            return false;
        }
#else
        lastError = ConfigError::UnsupportedFileFormat;
        lastErrorMsg = "JSON support not enabled";
        return false;
#endif
    } else if (format == "yaml" && std::filesystem::exists(filepath)) {
#ifdef NLM_WITH_YAML
        try {
            auto node = parseYamlFile(filepath);
            // Convert YAML to ConfigEntry vector
            for (const auto& pair : node) {
                std::string key = pair.first.as<std::string>();
                ConfigValue value;
                
                if (pair.second.IsScalar()) {
                    if (pair.second.IsBoolean()) {
                        value = pair.second.as<bool>();
                    } else if (pair.second.IsInteger()) {
                        value = pair.second.as<int64_t>();
                    } else if (pair.second.IsDouble()) {
                        value = pair.second.as<double>();
                    } else {
                        value = pair.second.as<std::string>();
                    }
                } else if (pair.second.IsSequence()) {
                    // Determine array type by first element
                    if (pair.second.size() > 0) {
                        if (pair.second[0].IsBoolean()) {
                            std::vector<bool> boolVec;
                            boolVec.reserve(pair.second.size());
                            for (const auto& item : pair.second) boolVec.push_back(item.as<bool>());
                            value = boolVec;
                        } else if (pair.second[0].IsInteger()) {
                            std::vector<int64_t> intVec;
                            intVec.reserve(pair.second.size());
                            for (const auto& item : pair.second) intVec.push_back(item.as<int64_t>());
                            value = intVec;
                        } else if (pair.second[0].IsDouble()) {
                            std::vector<double> doubleVec;
                            doubleVec.reserve(pair.second.size());
                            for (const auto& item : pair.second) doubleVec.push_back(item.as<double>());
                            value = doubleVec;
                        } else {
                            std::vector<std::string> stringVec;
                            stringVec.reserve(pair.second.size());
                            for (const auto& item : pair.second) stringVec.push_back(item.as<std::string>());
                            value = stringVec;
                        }
                    } else {
                        value = std::vector<std::string>();
                    }
                } else {
                    value = "N/A: Unsupported YAML node type";
                }
                
                pImpl->entries.emplace_back(key, value, ConfigSource::File, "Loaded from YAML");
                pImpl->entryMap[key] = pImpl->entries.back();
            }
            return true;
        } catch (const std::exception& e) {
            lastError = ConfigError::ParseError;
            lastErrorMsg = "YAML parse error: " + std::string(e.what());
            return false;
        }
#else
        lastError = ConfigError::UnsupportedFileFormat;
        lastErrorMsg = "YAML support not enabled";
        return false;
#endif
    } else {
        // Fall back to simple key=value format (backward compatibility)
        try {
            std::ifstream file(filepath);
            if (!file.is_open()) {
                lastError = ConfigError::FileNotFound;
                lastErrorMsg = "Could not open file: " + filepath;
                return false;
            }
            
            std::string line;
            int lineNumber = 0;
            while (std::getline(file, line)) {
                lineNumber++;
                line = trim(line);
                
                if (line.empty() || line[0] == '#' || line[0] == '/') {
                    continue;
                }
                
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = trim(line.substr(0, pos));
                    std::string value = trim(line.substr(pos + 1));
                    
                    if (value.size() >= 2 && 
                        ((value.front() == '"' && value.back() == '"') ||
                         (value.front() == '\'' && value.back() == '\''))) {
                        value = value.substr(1, value.size() - 2);
                    }
                    
                    set(key, value, ConfigSource::File);
                }
            }
            return true;
        } catch (const std::exception& e) {
            lastError = ConfigError::ParseError;
            lastErrorMsg = "Key=value parse error: " + std::string(e.what());
            return false;
        }
    }
}

bool Config::loadFromJson(const std::string& jsonString) {
#ifdef NLM_WITH_JSON
    try {
        auto json = nlohmann::json::parse(jsonString);
        clear();
        
        for (auto it = json.begin(); it != json.end(); ++it) {
            std::string key = it.key();
            ConfigValue value;
            
            if (it->is_boolean()) {
                value = it->get<bool>();
            } else if (it->is_number_integer()) {
                value = it->get<int64_t>();
            } else if (it->is_number_float()) {
                value = it->get<double>();
            } else if (it->is_string()) {
                value = it->get<std::string>();
            } else if (it->is_array()) {
                const auto& arr = *it;
                if (!arr.empty()) {
                    if (arr[0].is_boolean()) {
                        std::vector<bool> boolVec;
                        for (const auto& item : arr) boolVec.push_back(item.get<bool>());
                        value = boolVec;
                    } else if (arr[0].is_number_integer()) {
                        std::vector<int64_t> intVec;
                        for (const auto& item : arr) intVec.push_back(item.get<int64_t>());
                        value = intVec;
                    } else if (arr[0].is_number_float()) {
                        std::vector<double> doubleVec;
                        for (const auto& item : arr) doubleVec.push_back(item.get<double>());
                        value = doubleVec;
                    } else if (arr[0].is_string()) {
                        std::vector<std::string> stringVec;
                        for (const auto& item : arr) stringVec.push_back(item.get<std::string>());
                        value = stringVec;
                    } else {
                        value = std::vector<std::string>();
                    }
                } else {
                    value = std::vector<std::string>();
                }
            } else {
                value = "N/A: Unsupported JSON type";
            }
            
            pImpl->entries.emplace_back(key, value, ConfigSource::File, "Loaded from JSON string");
            pImpl->entryMap[key] = pImpl->entries.back();
        }
        return true;
    } catch (const std::exception& e) {
        lastError = ConfigError::ParseError;
        lastErrorMsg = "JSON parse error: " + std::string(e.what());
        return false;
    }
#else
    lastError = ConfigError::UnsupportedFileFormat;
    lastErrorMsg = "JSON support not enabled";
    return false;
#endif
}

bool Config::loadFromYaml(const std::string& yamlString) {
#ifdef NLM_WITH_YAML
    try {
        auto node = YAML::Load(yamlString);
        clear();
        
        for (const auto& pair : node) {
            std::string key = pair.first.as<std::string>();
            ConfigValue value;
            
            if (pair.second.IsScalar()) {
                if (pair.second.IsBoolean()) {
                    value = pair.second.as<bool>();
                } else if (pair.second.IsInteger()) {
                    value = pair.second.as<int64_t>();
                } else if (pair.second.IsDouble()) {
                    value = pair.second.as<double>();
                } else {
                    value = pair.second.as<std::string>();
                }
            } else if (pair.second.IsSequence()) {
                if (pair.second.size() > 0) {
                    if (pair.second[0].IsBoolean()) {
                        std::vector<bool> boolVec;
                        boolVec.reserve(pair.second.size());
                        for (const auto& item : pair.second) boolVec.push_back(item.as<bool>());
                        value = boolVec;
                    } else if (pair.second[0].IsInteger()) {
                        std::vector<int64_t> intVec;
                        intVec.reserve(pair.second.size());
                        for (const auto& item : pair.second) intVec.push_back(item.as<int64_t>());
                        value = intVec;
                    } else if (pair.second[0].IsDouble()) {
                        std::vector<double> doubleVec;
                        doubleVec.reserve(pair.second.size());
                        for (const auto& item : pair.second) doubleVec.push_back(item.as<double>());
                        value = doubleVec;
                    } else {
                        std::vector<std::string> stringVec;
                        stringVec.reserve(pair.second.size());
                        for (const auto& item : pair.second) stringVec.push_back(item.as<std::string>());
                        value = stringVec;
                    }
                } else {
                    value = std::vector<std::string>();
                }
            } else {
                value = "N/A: Unsupported YAML node type";
            }
            
            pImpl->entries.emplace_back(key, value, ConfigSource::File, "Loaded from YAML string");
            pImpl->entryMap[key] = pImpl->entries.back();
        }
        return true;
    } catch (const std::exception& e) {
        lastError = ConfigError::ParseError;
        lastErrorMsg = "YAML parse error: " + std::string(e.what());
        return false;
    }
#else
    lastError = ConfigError::UnsupportedFileFormat;
    lastErrorMsg = "YAML support not enabled";
    return false;
#endif
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        } else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::string format = detectFormat(filepath);
    
    if (format == "json") {
#ifdef NLM_WITH_JSON
        try {
            nlohmann::json json;
            for (const auto& entry : pImpl->entries) {
                json[entry.key] = entry.toJson();
            }
            
            std::ofstream file(filepath);
            if (!file.is_open()) {
                lastError = ConfigError::FileNotFound;
                lastErrorMsg = "Could not open file for writing: " + filepath;
                return false;
            }
            
            file << json.dump(2);
            return true;
        } catch (const std::exception& e) {
            lastError = ConfigError::ParseError;
            lastErrorMsg = "JSON save error: " + std::string(e.what());
            return false;
        }
#else
        lastError = ConfigError::UnsupportedFileFormat;
        lastErrorMsg = "JSON support not enabled";
        return false;
#endif
    } else if (format == "yaml") {
#ifdef NLM_WITH_YAML
        try {
            YAML::Emitter emitter;
            emitter << YAML::BeginMap;
            
            for (const auto& entry : pImpl->entries) {
                emitter << YAML::Key << entry.key;
                emitter << YAML::Value;
                
                const auto& node = entry.toYaml();
                if (node.IsScalar()) {
                    if (node.IsBoolean()) {
                        emitter << node.as<bool>();
                    } else if (node.IsInteger()) {
                        emitter << node.as<int64_t>();
                    } else if (node.IsDouble()) {
                        emitter << node.as<double>();
                    } else {
                        emitter << node.as<std::string>();
                    }
                } else if (node.IsSequence()) {
                    emitter << YAML::FlowSeq;
                    for (const auto& item : node) {
                        if (item.IsScalar()) {
                            if (item.IsBoolean()) {
                                emitter << item.as<bool>();
                            } else if (item.IsInteger()) {
                                emitter << item.as<int64_t>();
                            } else if (item.IsDouble()) {
                                emitter << item.as<double>();
                            } else {
                                emitter << item.as<std::string>();
                            }
                        }
                    }
                }
            }
            
            emitter << YAML::EndMap;
            
            std::ofstream file(filepath);
            if (!file.is_open()) {
                lastError = ConfigError::FileNotFound;
                lastErrorMsg = "Could not open file for writing: " + filepath;
                return false;
            }
            
            file << emitter.c_str();
            return true;
        } catch (const std::exception& e) {
            lastError = ConfigError::ParseError;
            lastErrorMsg = "YAML save error: " + std::string(e.what());
            return false;
        }
#else
        lastError = ConfigError::UnsupportedFileFormat;
        lastErrorMsg = "YAML support not enabled";
        return false;
#endif
            for (const auto& entry : pImpl->entries) {
                file << "# " << entry.description << "\n";
            std::visit([&file, entry](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << entry.key << " = \"" << arg << "\"\n";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << entry.key << " = " << (arg ? "true" : "false") << "\n";
                } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    file << entry.key << " = " << arg << "\n";
                } else if constexpr (std::is_same_v<T, double>) {
                    file << entry.key << " = " << arg << "\n";
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    file << entry.key << " = ["; bool first = true;
                    for (const auto& item : arg) {
                        if (!first) file << ", ";
                        file << item;
                        first = false;
                    }
                    file << "\n";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    file << entry.key << " = ["; bool first = true;
                    for (const auto& item : arg) {
                        if (!first) file << ", ";
                        file << item;
                        first = false;
                    }
                    file << "\n";
                } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
                    file << entry.key << " = ["; bool first = true;
                    for (const auto& item : arg) {
                        if (!first) file << ", ";
                        file << (item ? "true" : "false");
                        first = false;
                    }
                    file << "\n";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    file << entry.key << " = ["; bool first = true;
                    for (const auto& item : arg) {
                        if (!first) file << ", ";
                        file << "\"" << item << "\"";
                        first = false;
                    }
                    file << "\n";
                } else {
                    file << entry.key << " = " << arg << "\n";
                }
            }, entry.value);
            file << "\n";
        }
        return true;
    } else {
        // Fall back to simple key=value format (backward compatibility)
        std::ofstream file(filepath);
        if (!file.is_open()) {
            this->lastError = ConfigError::FileNotFound;
            this->lastErrorMsg = "Could not open file for writing: " + filepath;
            return false;
        }
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            std::visit([&file, entry](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << entry.key << " = \"" << arg << "\"\n";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << entry.key << " = " << (arg ? "true" : "false") << "\n";
                } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    file << entry.key << " = " << arg << "\n";
                } else if constexpr (std::is_same_v<T, double>) {
                    file << entry.key << " = " << arg << "\n";
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    file << entry.key << " = ["; bool first = true;
                    for (const auto& item : arg) {
                        if (!first) file << ", ";
                        file << item;
                        first = false;
                    }
                    file << "\n";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    file << entry.key << " = ["; bool first = true;
                    for (const auto& item : arg) {
                        if (!first) file << ", ";
                        file << item;
                        first = false;
                    }
                    file << "\n";
                } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
                    file << entry.key << " = ["; bool first = true;
                    for (const auto& item : arg) {
                        if (!first) file << ", ";
                        file << (item ? "true" : "false");
                        first = false;
                    }
                    file << "\n";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    file << entry.key << " = ["; bool first = true;
                    for (const auto& item : arg) {
                        if (!first) file << ", ";
                        file << "\"" << item << "\"";
                        first = false;
                    }
                    file << "\n";
                } else {
                    file << entry.key << " = " << arg << "\n";
                }
            }, entry.value);
            file << "\n";
        }
        return true;
    }
}

std::string Config::saveToJson() const {
#ifdef NLM_WITH_JSON
    try {
        nlohmann::json json;
        for (const auto& entry : pImpl->entries) {
            json[entry.key] = entry.toJson();
        }
        return json.dump(2);
    } catch (const std::exception& e) {
        lastError = ConfigError::ParseError;
        lastErrorMsg = "JSON save error: " + std::string(e.what());
        return "";
    }
#else
    lastError = ConfigError::UnsupportedFileFormat;
    lastErrorMsg = "JSON support not enabled";
    return "";
#endif
}

std::string Config::saveToYaml() const {
#ifdef NLM_WITH_YAML
    try {
        YAML::Emitter emitter;
        emitter << YAML::BeginMap;
        
        for (const auto& entry : pImpl->entries) {
            emitter << YAML::Key << entry.key;
            emitter << YAML::Value;
            
            const auto& node = entry.toYaml();
            if (node.IsScalar()) {
                if (node.IsBoolean()) {
                    emitter << node.as<bool>();
                } else if (node.IsInteger()) {
                    emitter << node.as<int64_t>();
                } else if (node.IsDouble()) {
                    emitter << node.as<double>();
                } else {
                    emitter << node.as<std::string>();
                }
            } else if (node.IsSequence()) {
                emitter << YAML::FlowSeq;
                for (const auto& item : node) {
                    if (item.IsScalar()) {
                        if (item.IsBoolean()) {
                            emitter << item.as<bool>();
                        } else if (item.IsInteger()) {
                            emitter << item.as<int64_t>();
                        } else if (item.IsDouble()) {
                            emitter << item.as<double>();
                        } else {
                            emitter << item.as<std::string>();
                        }
                    }
                }
            }
        }
        
        emitter << YAML::EndMap;
        return emitter.c_str();
    } catch (const std::exception& e) {
        lastError = ConfigError::ParseError;
        lastErrorMsg = "YAML save error: " + std::string(e.what());
        return "";
    }
#else
    lastError = ConfigError::UnsupportedFileFormat;
    lastErrorMsg = "YAML support not enabled";
    return "";
#endif
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = pImpl->entryMap.find(key);
    if (it == pImpl->entryMap.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->second.value);
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
    auto it = pImpl->entryMap.find(key);
    if (it != pImpl->entryMap.end()) {
        it->second.value = value;
        it->second.source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
        pImpl->entryMap[key] = pImpl->entries.back();
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
    return pImpl->entryMap.find(key) != pImpl->entryMap.end();
}

void Config::remove(const std::string& key) {
    auto it = pImpl->entryMap.find(key);
    if (it != pImpl->entryMap.end()) {
        pImpl->entries.erase(
            std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
                [&key](const ConfigEntry& e) { return e.key == key; }),
            pImpl->entries.end()
        );
        pImpl->entryMap.erase(it);
    }
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
    pImpl->entryMap.clear();
}

    std::string Config::summary() const {
        std::ostringstream oss;
        oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
        for (const auto& entry : pImpl->entries) {
            oss << "  " << entry.key << " = [";
            std::visit([&oss, entry](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    oss << (arg ? "true" : "false");
                } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    oss << arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    oss << arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    oss << "["; bool first = true;
                    for (const auto& item : arg) {
                        if (!first) oss << ", ";
                        oss << item;
                        first = false;
                    }
                    oss << "]";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    oss << "["; bool first = true;
                    for (const auto& item : arg) {
                        if (!first) oss << ", ";
                        oss << item;
                        first = false;
                    }
                    oss << "]";
                } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
                    oss << "["; bool first = true;
                    for (const auto& item : arg) {
                        if (!first) oss << ", ";
                        oss << (item ? "true" : "false");
                        first = false;
                    }
                    oss << "]";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    oss << "["; bool first = true;
                    for (const auto& item : arg) {
                        if (!first) oss << ", ";
                        oss << "\"" << item << "\"";
                        first = false;
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
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "["; bool first = true;
                for (const auto& item : arg) {
                    if (!first) oss << ", ";
                    oss << item;
                    first = false;
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
                oss << "["; bool first = true;
                for (const auto& item : arg) {
                    if (!first) oss << ", ";
                    oss << (item ? "true" : "false");
                    first = false;
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "["; bool first = true;
                for (const auto& item : arg) {
                    if (!first) oss << ", ";
                    oss << "\"" << item << "\"";
                    first = false;
                }
                oss << "]";
            } else {
                // Fallback for other types
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

bool Config::validate() const {
    for (const auto& entry : pImpl->entries) {
        if (!validateEntry(entry)) {
            return false;
        }
    }
    return true;
}

void Config::applyBrainConfig(const BrainConfig& brainCfg) {
    set("neuron_count", static_cast<int64_t>(brainCfg.neuron_count), ConfigSource::Runtime);
    set("region_count", static_cast<int64_t>(brainCfg.region_count), ConfigSource::Runtime);
    set("connection_probability", brainCfg.connection_probability, ConfigSource::Runtime);
    set("brain_structure_type", brainCfg.structure_type, ConfigSource::Runtime);
}

void Config::applyPlasticityConfig(const PlasticityConfig& plasticityCfg) {
    set("learning_rate", plasticityCfg.learning_rate, ConfigSource::Runtime);
    set("stdp_ltp_weight", plasticityCfg.stdp_ltp_weight, ConfigSource::Runtime);
    set("stdp_ltd_weight", plasticityCfg.stdp_ltd_weight, ConfigSource::Runtime);
    set("stdp_time_constant", plasticityCfg.stdp_time_constant, ConfigSource::Runtime);
    set("plasticity_rule_type", plasticityCfg.rule_type, ConfigSource::Runtime);
}

void Config::applyNeuromodulationConfig(const NeuromodulationConfig& neuromodCfg) {
    set("dopamine_baseline", neuromodCfg.dopamine_baseline, ConfigSource::Runtime);
    set("dopamine_decay_rate", neuromodCfg.dopamine_decay_rate, ConfigSource::Runtime);
    set("reward_scale", neuromodCfg.reward_scale, ConfigSource::Runtime);
    set("prediction_error_scale", neuromodCfg.prediction_error_scale, ConfigSource::Runtime);
    set("novelty_threshold_type", neuromodCfg.novelty_threshold_type, ConfigSource::Runtime);
}

void Config::applyMemoryConfig(const MemoryConfig& memoryCfg) {
    set("working_memory_size", static_cast<int64_t>(memoryCfg.working_memory_size), ConfigSource::Runtime);
    set("episodic_memory_capacity", static_cast<int64_t>(memoryCfg.episodic_memory_capacity), ConfigSource::Runtime);
    set("semantic_memory_capacity", static_cast<int64_t>(memoryCfg.semantic_memory_capacity), ConfigSource::Runtime);
    set("consolidation_rate", memoryCfg.consolidation_rate, ConfigSource::Runtime);
    set("consolidation_rule", memoryCfg.consolidation_rule, ConfigSource::Runtime);
}

void Config::applyVisualizationConfig(const VisualizationConfig& visCfg) {
    set("visualization_enabled", visCfg.enabled, ConfigSource::Runtime);
    set("visualization_update_rate", visCfg.update_rate, ConfigSource::Runtime);
    set("visualization_backend", visCfg.backend, ConfigSource::Runtime);
    set("visualization_output_format", visCfg.output_format, ConfigSource::Runtime);
}

void Config::applyEnvironmentConfig(const EnvironmentConfig& envCfg) {
    set("environment_name", envCfg.name, ConfigSource::Runtime);
    set("environment_width", static_cast<int64_t>(envCfg.width), ConfigSource::Runtime);
    set("environment_height", static_cast<int64_t>(envCfg.height), ConfigSource::Runtime);
    set("environment_type", envCfg.type, ConfigSource::Runtime);
}

BrainConfig Config::getBrainConfig() const {
    BrainConfig config;
    config.neuron_count = getOr<int64_t>("neuron_count", 1000);
    config.region_count = getOr<int64_t>("region_count", 2);
    config.connection_probability = getOr<double>("connection_probability", 0.1);
    config.structure_type = getOr<std::string>("brain_structure_type", "random");
    return config;
}

PlasticityConfig Config::getPlasticityConfig() const {
    PlasticityConfig config;
    config.learning_rate = getOr<double>("learning_rate", 0.01);
    config.stdp_ltp_weight = getOr<double>("stdp_ltp_weight", 0.01);
    config.stdp_ltd_weight = getOr<double>("stdp_ltd_weight", 0.012);
    config.stdp_time_constant = getOr<double>("stdp_time_constant", 20.0);
    config.rule_type = getOr<std::string>("plasticity_rule_type", "stdp");
    return config;
}

NeuromodulationConfig Config::getNeuromodulationConfig() const {
    NeuromodulationConfig config;
    config.dopamine_baseline = getOr<double>("dopamine_baseline", 0.0);
    config.dopamine_decay_rate = getOr<double>("dopamine_decay_rate", 0.95);
    config.reward_scale = getOr<double>("reward_scale", 1.0);
    config.prediction_error_scale = getOr<double>("prediction_error_scale", 1.0);
    config.novelty_threshold_type = getOr<std::string>("novelty_threshold_type", "adaptive");
    return config;
}

MemoryConfig Config::getMemoryConfig() const {
    MemoryConfig config;
    config.working_memory_size = getOr<int64_t>("working_memory_size", 100);
    config.episodic_memory_capacity = getOr<int64_t>("episodic_memory_capacity", 1000);
    config.semantic_memory_capacity = getOr<int64_t>("semantic_memory_capacity", 5000);
    config.consolidation_rate = getOr<double>("consolidation_rate", 0.01);
    config.consolidation_rule = getOr<std::string>("consolidation_rule", "lstm");
    return config;
}

VisualizationConfig Config::getVisualizationConfig() const {
    VisualizationConfig config;
    config.enabled = getOr<bool>("visualization_enabled", false);
    config.update_rate = getOr<double>("visualization_update_rate", 30.0);
    config.backend = getOr<std::string>("visualization_backend", "none");
    config.output_format = getOr<std::string>("visualization_output_format", "text");
    return config;
}

EnvironmentConfig Config::getEnvironmentConfig() const {
    EnvironmentConfig config;
    config.name = getOr<std::string>("environment_name", "GridWorld");
    config.width = getOr<int64_t>("environment_width", 10);
    config.height = getOr<int64_t>("environment_height", 10);
    config.type = getOr<std::string>("environment_type", "deterministic");
    return config;
}

std::string Config::getErrorMessage() const {
    switch (lastError) {
        case ConfigError::None:
            return "No error";
        case ConfigError::FileNotFound:
            return "File not found";
        case ConfigError::ParseError:
            return "Parse error: " + lastErrorMsg;
        case ConfigError::InvalidFormat:
            return "Invalid configuration format";
        case ConfigError::ValidationError:
            return "Configuration validation failed";
        case ConfigError::UnsupportedFileFormat:
            return "Unsupported file format";
        default:
            return "Unknown error";
    }
}

std::string Config::detectFormat(const std::string& filepath) {
    if (filepath.empty()) return "";
    
    size_t pos = filepath.rfind('.');
    if (pos == std::string::npos) return "";
    
    std::string extension = filepath.substr(pos + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == "json") return "json";
    if (extension == "yaml" || extension == "yml") return "yaml";
    
    return "unknown";
}

bool Config::validateEntry(const ConfigEntry& entry) const {
    if (entry.key.empty()) {
        lastError = ConfigError::ValidationError;
        lastErrorMsg = "Empty key not allowed";
        return false;
    }
    
    if (entry.key == "neuron_count" && std::holds_alternative<int64_t>(entry.value)) {
        return validateNeuronCount(std::get<int64_t>(entry.value));
    }
    
    if (entry.key == "connection_probability" && std::holds_alternative<double>(entry.value)) {
        return validateConnectionProbability(std::get<double>(entry.value));
    }
    
    if (entry.key == "simulation_timestep" && std::holds_alternative<double>(entry.value)) {
        return validateTimestep(std::get<double>(entry.value));
    }
    
    return true;
}

// Validation methods (defined in Config.hpp)
bool Config::validateNeuronCount(int64_t count) const {
    if (count < 1 || count > 100000) {
        lastError = ConfigError::ValidationError;
        lastErrorMsg = "Neuron count must be between 1 and 100000";
        return false;
    }
    return true;
}

bool Config::validateConnectionProbability(double prob) const {
    if (prob < 0.0 || prob > 1.0) {
        lastError = ConfigError::ValidationError;
        lastErrorMsg = "Connection probability must be between 0.0 and 1.0";
        return false;
    }
    return true;
}

bool Config::validateTimestep(double timestep) const {
    if (timestep <= 0.0 || timestep > 1.0) {
        lastError = ConfigError::ValidationError;
        lastErrorMsg = "Timestep must be between 0.0 and 1.0";
        return false;
    }
    return true;
}

} // namespace nlm