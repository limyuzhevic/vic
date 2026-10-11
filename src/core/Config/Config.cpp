#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Determine file format by extension
    std::string ext = "." + std::string(
        std::filesystem::path(filepath).extension().begin(),
        std::filesystem::path(filepath).extension().end());
    
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        return loadFromJSON(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        return loadFromYAML(filepath);
    } else {
        // Default to key=value format for backward compatibility
        return loadFromKeyValue(filepath);
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
#ifdef HAS_NLOHMANN_JSON
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json jsonData;
        file >> jsonData;
        
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            
            // Handle different JSON value types
            if (it->is_string()) {
                set(key, it->get<std::string>(), ConfigSource::File);
            } else if (it->is_number_integer()) {
                set(key, it->get<int>(), ConfigSource::File);
            } else if (it->is_number_unsigned()) {
                set(key, static_cast<int64_t>(it->get<uint64_t>()), ConfigSource::File);
            } else if (it->is_number_float()) {
                set(key, it->get<double>(), ConfigSource::File);
            } else if (it->is_boolean()) {
                set(key, it->get<bool>(), ConfigSource::File);
            } else if (it->is_array()) {
                // Try to infer array type
                if (!it->empty()) {
                    if (it->front().is_number_integer()) {
                        std::vector<int> intArray;
                        for (const auto& item : *it) {
                            intArray.push_back(item.get<int>());
                        }
                        set(key, intArray, ConfigSource::File);
                    } else if (it->front().is_number_float()) {
                        std::vector<double> doubleArray;
                        for (const auto& item : *it) {
                            doubleArray.push_back(item.get<double>());
                        }
                        set(key, doubleArray, ConfigSource::File);
                    } else if (it->front().is_string()) {
                        std::vector<std::string> stringArray;
                        for (const auto& item : *it) {
                            stringArray.push_back(item.get<std::string>());
                        }
                        set(key, stringArray, ConfigSource::File);
                    }
                }
            }
        }
        
        NLM_LOG_INFO("Loaded JSON configuration from: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading JSON configuration from " + filepath + ": " + std::string(e.what()));
        return false;
    }
#else
    NLM_LOG_WARNING("JSON parsing not available (nlohmann/json not found), falling back to key=value format");
    return loadFromKeyValue(filepath);
#endif
}

bool Config::loadFromYAML(const std::string& filepath) {
#ifdef HAS_YAML_CPP
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        YAML::Node yamlData = YAML::LoadFile(filepath);
        
        for (const auto& it : yamlData) {
            std::string key = it.first.as<std::string>();
            
            // Handle different YAML value types
            if (it.second.IsScalar()) {
                // Try to determine scalar type
                std::string value = it.second.as<std::string>();
                
                // Try to parse as boolean
                if (value == "true" || value == "false") {
                    set(key, value == "true", ConfigSource::File);
                } else {
                    // Try to parse as integer
                    try {
                        size_t pos;
                        int intVal = std::stoi(value, &pos);
                        if (pos == value.length()) {
                            set(key, intVal, ConfigSource::File);
                            continue;
                        }
                    } catch (...) {}
                    
                    // Try to parse as double
                    try {
                        size_t pos;
                        double doubleVal = std::stod(value, &pos);
                        if (pos == value.length()) {
                            set(key, doubleVal, ConfigSource::File);
                            continue;
                        }
                    } catch (...) {}
                    
                    // Treat as string
                    set(key, value, ConfigSource::File);
                }
            } else if (it.second.IsSequence()) {
                // Handle YAML sequences (arrays)
                if (!it.second.IsNull()) {
                    std::vector<std::string> stringArray;
                    for (const auto& item : it.second) {
                        stringArray.push_back(item.as<std::string>());
                    }
                    set(key, stringArray, ConfigSource::File);
                }
            }
        }
        
        NLM_LOG_INFO("Loaded YAML configuration from: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading YAML configuration from " + filepath + ": " + std::string(e.what()));
        return false;
    }
#else
    NLM_LOG_WARNING("YAML parsing not available (yaml-cpp not found), falling back to key=value format");
    return loadFromKeyValue(filepath);
#endif
}

bool Config::loadFromKeyValue(const std::string& filepath) {
    // Original key=value format loading (Phase 1 implementation)
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
    
    NLM_LOG_INFO("Loaded key=value configuration from: " + filepath);
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
        NLM_LOG_ERROR("Failed to open configuration file for writing: " + filepath);
        return false;
    }
    
    // Determine file format by extension
    std::string ext = "." + std::string(
        std::filesystem::path(filepath).extension().begin(),
        std::filesystem::path(filepath).extension().end());
    
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    try {
        if (ext == ".json") {
            saveToJSON(file);
        } else if (ext == ".yaml" || ext == ".yml") {
            saveToYAML(file);
        } else {
            // Default to key=value format for backward compatibility
            saveToKeyValue(file);
        }
        
        NLM_LOG_INFO("Saved configuration to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving configuration to " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

void Config::saveToJSON(std::ostream& os) const {
#ifdef HAS_NLOHMANN_JSON
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        // Add comment if available
        if (!entry.description.empty()) {
            jsonData[entry.key] = entry.description;
        } else {
            // Store actual value
            std::visit([&jsonData, entry](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, int>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    jsonData[entry.key] = arg;
                }
            }, entry.value);
        }
    }
    
    os << std::setw(2) << jsonData;
#endif
}

void Config::saveToYAML(std::ostream& os) const {
#ifdef HAS_YAML_CPP
    YAML::Emitter emitter;
    emitter << YAML::BeginMap;
    
    for (const auto& entry : pImpl->entries) {
        emitter << YAML::Key << entry.key;
        
        // Visit the variant and emit appropriate YAML type
        std::visit([&emitter](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                emitter << YAML::Value << arg;
            } else if constexpr (std::is_same_v<T, int>) {
                emitter << YAML::Value << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                emitter << YAML::Value << static_cast<int>(arg);
            } else if constexpr (std::is_same_v<T, double>) {
                emitter << YAML::Value << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                emitter << YAML::Value << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                emitter << YAML::Value << YAML::BeginSeq;
                for (const auto& item : arg) {
                    emitter << item;
                }
                emitter << YAML::EndSeq;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                emitter << YAML::Value << YAML::BeginSeq;
                for (const auto& item : arg) {
                    emitter << item;
                }
                emitter << YAML::EndSeq;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                emitter << YAML::Value << YAML::BeginSeq;
                for (const auto& item : arg) {
                    emitter << item;
                }
                emitter << YAML::EndSeq;
            }
        }, entry.value);
    }
    
    emitter << YAML::EndMap;
    os << emitter.c_str();
#endif
}

void Config::saveToKeyValue(std::ostream& os) const {
    for (const auto& entry : pImpl->entries) {
        // Add comment if available
        if (!entry.description.empty()) {
            os << "# " << entry.description << "\n";
        }
        
        // Convert value to string representation
        std::visit([&os](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                os << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                os << entry.key << " = " << (arg ? "true" : "false") << "\n";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                os << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) os << ", ";
                    os << arg[i];
                }
                os << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                os << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) os << ", ";
                    os << arg[i];
                }
                os << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                os << entry.key << " = [\"";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) os << "\", \"";
                    os << arg[i];
                }
                os << "\"]\n";
            } else {
                // For scalar types (int, int64_t, double)
                os << entry.key << " = " << arg << "\n";
            }
        }, entry.value);
    }
}

void Config::saveToJSON(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (file.is_open()) {
        saveToJSON(file);
    }
}

void Config::saveToYAML(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (file.is_open()) {
        saveToYAML(file);
    }
}

void Config::saveToKeyValue(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (file.is_open()) {
        saveToKeyValue(file);
    }
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
