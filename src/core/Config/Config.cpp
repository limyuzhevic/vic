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

bool Config::loadFromFile(const std::string& filepath) {
// Auto-detect file format based on extension
    std::string ext = getFileExtension(filepath);
    if (isJSONFileExtension(ext)) {
        return loadFromJSON(filepath);
    } else if (isYAMLFileExtension(ext)) {
        return loadFromYAML(filepath);
    } else {
        // Fallback to simple key=value format for backward compatibility
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
                     (value.front() == \'\' && value.back() == \'\'))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
            }
        }
        return true;
    }
}
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

}

bool Config::validateConfig() const {
    bool valid = true;
    for (const auto& entry : pImpl->entries) {
        std::visit([&valid](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                if (arg < INT_MIN || arg > INT_MAX) valid = false;
            } else if constexpr (std::is_same_v<T, std::string>) {
                if (arg.empty()) valid = false;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                for (const auto& val : arg) {
                    if (val < INT_MIN || val > INT_MAX) valid = false;
                }
            }
        }, entry.value);
    }
    return valid;
}

void Config::updatePlaceholders() {
    for (auto& entry : pImpl->entries) {
        if (entry.description.find("PLACEHOLDER") != std::string::npos) {
            entry.description = "Updated placeholder value";
        }
    }
}

std::string Config::getFileExtension(const std::string& filepath) {
    size_t pos = filepath.find_last_of('.');
    if (pos == std::string::npos) {
        return "";
    }
    return toLower(filepath.substr(pos + 1));
}

bool Config::isJSONFileExtension(const std::string& ext) {
    return ext == "json";
}

bool Config::isYAMLFileExtension(const std::string& ext) {
    return ext == "yaml" || ext == "yml";
}

bool Config::isJSONFile(const std::string& filepath) {
    return isJSONFileExtension(getFileExtension(filepath));
}

bool Config::isYAMLFile(const std::string& filepath) {
    return isYAMLFileExtension(getFileExtension(filepath));
}

nlohmann::json Config::configToJSON() const {
    nlohmann::json jsonObj;
    for (const auto& entry : pImpl->entries) {
        std::visit([&jsonObj, &entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                jsonObj[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                jsonObj[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                jsonObj[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::string>) {
                jsonObj[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                jsonObj[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                jsonObj[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                jsonObj[entry.key] = arg;
            }
        }, entry.value);
    }
    return jsonObj;
}

void Config::jsonToConfig(const nlohmann::json& jsonObj) {
    clear();
    for (auto it = jsonObj.begin(); it != jsonObj.end(); ++it) {
        std::string key = it.key();
        const auto& value = it.value();
        
        // Convert JSON value to ConfigValue
        ConfigValue configValue;
        
        if (value.is_number_integer()) {
            configValue = value.get<int>();
        } else if (value.is_number_float()) {
            configValue = value.get<double>();
        } else if (value.is_boolean()) {
            configValue = value.get<bool>();
        } else if (value.is_string()) {
            configValue = value.get<std::string>();
        } else if (value.is_array()) {
            // Check array type
            if (!value.empty()) {
                if (value[0].is_number_integer()) {
                    std::vector<int> intVector;
                    for (const auto& item : value) {
                        intVector.push_back(item.get<int>());
                    }
                    configValue = intVector;
                } else if (value[0].is_number_float()) {
                    std::vector<double> doubleVector;
                    for (const auto& item : value) {
                        doubleVector.push_back(item.get<double>());
                    }
                    configValue = doubleVector;
                } else if (value[0].is_string()) {
                    std::vector<std::string> stringVector;
                    for (const auto& item : value) {
                        stringVector.push_back(item.get<std::string>());
                    }
                    configValue = stringVector;
                }
            }
        }
        
        if (!std::holds_alternative<std::monostate>(configValue)) {
            set(key, configValue, ConfigSource::File);
        }
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonObj = nlohmann::json::parse(file);
        jsonToConfig(jsonObj);
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        YAML::Node yamlNode = YAML::Load(file);
        yamlToConfig(yamlNode);
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

void Config::yamlToConfig(const YAML::Node& yamlNode) {
    clear();
    for (const auto& pair : yamlNode) {
        std::string key = pair.first.as<std::string>();
        const YAML::Node& value = pair.second;
        
        // Convert YAML value to ConfigValue
        ConfigValue configValue;
        
        if (value.IsScalar()) {
            if (value.IsInteger()) {
                configValue = value.as<int>();
            } else if (value.IsFloat()) {
                configValue = value.as<double>();
            } else if (value.IsBoolean()) {
                configValue = value.as<bool>();
            } else {
                configValue = value.as<std::string>();
            }
        } else if (value.IsSequence()) {
            if (!value.empty()) {
                if (value[0].IsInteger()) {
                    std::vector<int> intVector;
                    for (const auto& item : value) {
                        intVector.push_back(item.as<int>());
                    }
                    configValue = intVector;
                } else if (value[0].IsFloat()) {
                    std::vector<double> doubleVector;
                    for (const auto& item : value) {
                        doubleVector.push_back(item.as<double>());
                    }
                    configValue = doubleVector;
                } else if (value[0].IsScalar()) {
                    std::vector<std::string> stringVector;
                    for (const auto& item : value) {
                        stringVector.push_back(item.as<std::string>());
                    }
                    configValue = stringVector;
                }
            }
        }
        
        if (!std::holds_alternative<std::monostate>(configValue)) {
            set(key, configValue, ConfigSource::File);
        }
    }
}

std::string Config::configToYAML() const {
    YAML::Emitter emitter;
    emitter << YAML::BeginMap;
    
    for (const auto& entry : pImpl->entries) {
        emitter << YAML::Key << entry.key;
        
        std::visit([&emitter](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                emitter << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                emitter << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                emitter << arg;
            } else if constexpr (std::is_same_v<T, std::string>) {
                emitter << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                emitter << YAML::FlowSeq << arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                emitter << YAML::FlowSeq << arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                emitter << YAML::FlowSeq << arg;
            }
        }, entry.value);
    }
    
    emitter << YAML::EndMap;
    return emitter.str();
}

bool Config::saveToJSON(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonObj = configToJSON();
        file << jsonObj.dump(4);
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::saveToYAML(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        std::string yamlContent = configToYAML();
        file << yamlContent;
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    std::string ext = getFileExtension(filepath);
    if (isJSONFileExtension(ext)) {
        return saveToJSON(filepath);
    } else if (isYAMLFileExtension(ext)) {
        return saveToYAML(filepath);
    } else {
        // Fallback to simple key=value format for backward compatibility
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << entry.key << " = \"" << arg << "\"\n";
                } else {
                    file << entry.key << " = " << arg << "\n";
                }
            }, entry.value);
        }
        return true;
    }
}

template<typename T>
std::optional<T> Config::getConfigValue(const std::string& key) const {
    return get<T>(key);
}

template<typename T>
T Config::getConfigValue(const std::string& key, const T& defaultValue) const {
    auto val = getConfigValue<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

bool Config::mergeConfig(const nlm::Config& other, bool overrideExisting) {
    bool merged = false;
    for (const auto& entry : other.pImpl->entries) {
        if (!has(entry.key) || overrideExisting) {
            set(entry.key, entry.value, entry.source);
            merged = true;
        }
    }
    return merged;
}

std::string Config::getConfigAsString() const {
    std::ostringstream oss;
    oss << "Configuration:";
    
    for (const auto& entry : pImpl->entries) {
        oss << "\n  " << entry.key << ": ";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << " (source: " << static_cast<int>(entry.source) << ")";
    }
    
    oss << "\nValid: " << (validateConfig() ? "Yes" : "No");
    return oss.str();
}
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
