#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

#ifdef NLM_USE_NLOHMANN_JSON
#include <nlohmann/json.hpp>
#endif

#ifdef NLM_USE_YAML_CPP
#include <yaml-cpp/yaml.h>
#endif

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // First, try to parse as JSON
#ifdef NLM_USE_NLOHMANN_JSON
    try {
        std::ifstream jsonFile(filepath);
        if (jsonFile.is_open()) {
            nlohmann::json jsonData = nlohmann::json::parse(jsonFile);
            clear();
            
            for (const auto& [key, value] : jsonData.items()) {
                ConfigValue configValue;
                setValueFromJSON(value, configValue);
                set(key, configValue, ConfigSource::File);
            }
            return true;
        }
    } catch (const std::exception&) {
        // Fall through to YAML or legacy format
    }
#endif

    // Try to parse as YAML
#ifdef NLM_USE_YAML_CPP
    try {
        YAML::Node yamlData = YAML::LoadFile(filepath);
        clear();
        
        for (const auto& node : yamlData) {
            std::string key = node.first.as<std::string>();
            ConfigValue configValue;
            setValueFromYAML(node.second, configValue);
            set(key, configValue, ConfigSource::File);
        }
        return true;
    } catch (const std::exception&) {
        // Fall through to legacy format
    }
#endif

    // Fall back to legacy key=value format
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

bool Config::loadFromJSON(const std::string& jsonString) {
#ifdef NLM_USE_NLOHMANN_JSON
    try {
        nlohmann::json jsonData = nlohmann::json::parse(jsonString);
        clear();
        
        for (const auto& [key, value] : jsonData.items()) {
            ConfigValue configValue;
            setValueFromJSON(value, configValue);
            set(key, configValue, ConfigSource::File);
        }
        return true;
    } catch (const std::exception&) {
        return false;
    }
#else
    (void)jsonString;
    return false;
#endif
}

bool Config::loadFromYAML(const std::string& yamlString) {
#ifdef NLM_USE_YAML_CPP
    try {
        YAML::Node yamlData = YAML::Load(yamlString);
        clear();
        
        for (const auto& node : yamlData) {
            std::string key = node.first.as<std::string>();
            ConfigValue configValue;
            setValueFromYAML(node.second, configValue);
            set(key, configValue, ConfigSource::File);
        }
        return true;
    } catch (const std::exception&) {
        return false;
    }
#else
    (void)yamlString;
    return false;
#endif
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

bool Config::saveToJSON(const std::string& filepath) const {
#ifdef NLM_USE_NLOHMANN_JSON
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        jsonData[entry.key] = getJSONValue(entry.value);
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << jsonData.dump(2);
    return true;
#else
    (void)filepath;
    return false;
#endif
}

bool Config::saveToYAML(const std::string& filepath) const {
#ifdef NLM_USE_YAML_CPP
    YAML::Emitter emitter;
    emitter << YAML::BeginMap;
    
    for (const auto& entry : pImpl->entries) {
        emitter << YAML::Key << entry.key;
        emitter << YAML::Value << getYAMLValue(entry.value);
    }
    
    emitter << YAML::EndMap;
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << emitter.c_str();
    return true;
#else
    (void)filepath;
    return false;
#endif
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << getStringValue(entry.value) << "\n";
    }
    
    return true;
}

// Helper functions for JSON/YAML conversion
void Config::setValueFromJSON(const nlohmann::json& json, ConfigValue& configValue) {
#ifdef NLM_USE_NLOHMANN_JSON
    if (json.is_string()) {
        configValue = json.get<std::string>();
    } else if (json.is_number_integer()) {
        configValue = json.get<int64_t>();
    } else if (json.is_number_unsigned()) {
        configValue = json.get<uint64_t>();
    } else if (json.is_number_float()) {
        configValue = json.get<double>();
    } else if (json.is_boolean()) {
        configValue = json.get<bool>();
    } else if (json.is_array()) {
        // Handle arrays
        if (json.empty()) {
            configValue = std::vector<std::string>();
        } else if (json[0].is_string()) {
            std::vector<std::string> strArray;
            for (const auto& item : json) {
                strArray.push_back(item.get<std::string>());
            }
            configValue = strArray;
        } else if (json[0].is_number_integer()) {
            std::vector<int> intArray;
            for (const auto& item : json) {
                intArray.push_back(item.get<int>());
            }
            configValue = intArray;
        } else if (json[0].is_number_float()) {
            std::vector<double> doubleArray;
            for (const auto& item : json) {
                doubleArray.push_back(item.get<double>());
            }
            configValue = doubleArray;
        }
    }
#endif
}

void Config::setValueFromYAML(const YAML::Node& yaml, ConfigValue& configValue) {
#ifdef NLM_USE_YAML_CPP
    if (yaml.IsScalar()) {
        if (yaml.IsString()) {
            configValue = yaml.as<std::string>();
        } else if (yaml.IsInteger()) {
            configValue = yaml.as<int64_t>();
        } else if (yaml.IsDouble()) {
            configValue = yaml.as<double>();
        } else if (yaml.IsBoolean()) {
            configValue = yaml.as<bool>();
        }
    } else if (yaml.IsSequence()) {
        // Handle sequences
        if (yaml.empty()) {
            configValue = std::vector<std::string>();
        } else if (yaml[0].IsString()) {
            std::vector<std::string> strArray;
            for (const auto& item : yaml) {
                strArray.push_back(item.as<std::string>());
            }
            configValue = strArray;
        } else if (yaml[0].IsInteger()) {
            std::vector<int> intArray;
            for (const auto& item : yaml) {
                intArray.push_back(item.as<int>());
            }
            configValue = intArray;
        } else if (yaml[0].IsDouble()) {
            std::vector<double> doubleArray;
            for (const auto& item : yaml) {
                doubleArray.push_back(item.as<double>());
            }
            configValue = doubleArray;
        }
    }
#endif
}

nlohmann::json Config::getJSONValue(const ConfigValue& configValue) const {
#ifdef NLM_USE_NLOHMANN_JSON
    return std::visit([](const auto& v) -> nlohmann::json {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return v;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return v;
        } else if constexpr (std::is_same_v<T, uint64_t>) {
            return v;
        } else if constexpr (std::is_same_v<T, double>) {
            return v;
        } else if constexpr (std::is_same_v<T, bool>) {
            return v;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            nlohmann::json array = nlohmann::json::array();
            for (const auto& item : v) {
                array.push_back(item);
            }
            return array;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            nlohmann::json array = nlohmann::json::array();
            for (const auto& item : v) {
                array.push_back(item);
            }
            return array;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            nlohmann::json array = nlohmann::json::array();
            for (const auto& item : v) {
                array.push_back(item);
            }
            return array;
        } else {
            return "";
        }
    }, configValue);
#else
    return nlohmann::json();
#endif
}

YAML::Node Config::getYAMLValue(const ConfigValue& configValue) const {
#ifdef NLM_USE_YAML_CPP
    YAML::Node node;
    std::visit([&](const auto& v) {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, std::string>) {
            node = v;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            node = v;
        } else if constexpr (std::is_same_v<T, uint64_t>) {
            node = v;
        } else if constexpr (std::is_same_v<T, double>) {
            node = v;
        } else if constexpr (std::is_same_v<T, bool>) {
            node = v;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            YAML::Node array = YAML::Node(YAML::NodeType::Sequence);
            for (const auto& item : v) {
                array.push_back(item);
            }
            node = array;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            YAML::Node array = YAML::Node(YAML::NodeType::Sequence);
            for (const auto& item : v) {
                array.push_back(item);
            }
            node = array;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            YAML::Node array = YAML::Node(YAML::NodeType::Sequence);
            for (const auto& item : v) {
                array.push_back(item);
            }
            node = array;
        }
    }, configValue);
    return node;
#else
    return YAML::Node();
#endif
}

std::string Config::getStringValue(const ConfigValue& configValue) const {
    return std::visit([](const auto& v) -> std::string {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return v;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return std::to_string(v);
        } else if constexpr (std::is_same_v<T, uint64_t>) {
            return std::to_string(v);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::to_string(v);
        } else if constexpr (std::is_same_v<T, bool>) {
            return v ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            std::string result = "[";
            for (size_t i = 0; i < v.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(v[i]);
            }
            result += "]";
            return result;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            std::string result = "[";
            for (size_t i = 0; i < v.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(v[i]);
            }
            result += "]";
            return result;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            std::string result = "[";
            for (size_t i = 0; i < v.size(); ++i) {
                if (i > 0) result += ", ";
                result += "\"" + v[i] + "\"";
            }
            result += "]";
            return result;
        } else {
            return "";
        }
    }, configValue);
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

// Helper functions for JSON/YAML conversion
void Config::setValueFromJSON(const nlohmann::json& json, ConfigValue& configValue) {
#ifdef NLM_USE_NLOHMANN_JSON
    if (json.is_string()) {
        configValue = json.get<std::string>();
    } else if (json.is_number_integer()) {
        configValue = json.get<int64_t>();
    } else if (json.is_number_unsigned()) {
        configValue = json.get<uint64_t>();
    } else if (json.is_number_float()) {
        configValue = json.get<double>();
    } else if (json.is_boolean()) {
        configValue = json.get<bool>();
    } else if (json.is_array()) {
        // Handle arrays
        if (json.empty()) {
            configValue = std::vector<std::string>();
        } else if (json[0].is_string()) {
            std::vector<std::string> strArray;
            for (const auto& item : json) {
                strArray.push_back(item.get<std::string>());
            }
            configValue = strArray;
        } else if (json[0].is_number_integer()) {
            std::vector<int> intArray;
            for (const auto& item : json) {
                intArray.push_back(item.get<int>());
            }
            configValue = intArray;
        } else if (json[0].is_number_float()) {
            std::vector<double> doubleArray;
            for (const auto& item : json) {
                doubleArray.push_back(item.get<double>());
            }
            configValue = doubleArray;
        }
    }
#endif
}

void Config::setValueFromYAML(const YAML::Node& yaml, ConfigValue& configValue) {
#ifdef NLM_USE_YAML_CPP
    if (yaml.IsScalar()) {
        if (yaml.IsString()) {
            configValue = yaml.as<std::string>();
        } else if (yaml.IsInteger()) {
            configValue = yaml.as<int64_t>();
        } else if (yaml.IsDouble()) {
            configValue = yaml.as<double>();
        } else if (yaml.IsBoolean()) {
            configValue = yaml.as<bool>();
        }
    } else if (yaml.IsSequence()) {
        // Handle sequences
        if (yaml.empty()) {
            configValue = std::vector<std::string>();
        } else if (yaml[0].IsString()) {
            std::vector<std::string> strArray;
            for (const auto& item : yaml) {
                strArray.push_back(item.as<std::string>());
            }
            configValue = strArray;
        } else if (yaml[0].IsInteger()) {
            std::vector<int> intArray;
            for (const auto& item : yaml) {
                intArray.push_back(item.as<int>());
            }
            configValue = intArray;
        } else if (yaml[0].IsDouble()) {
            std::vector<double> doubleArray;
            for (const auto& item : yaml) {
                doubleArray.push_back(item.as<double>());
            }
            configValue = doubleArray;
        }
    }
#endif
}

nlohmann::json Config::getJSONValue(const ConfigValue& configValue) const {
#ifdef NLM_USE_NLOHMANN_JSON
    return std::visit([](const auto& v) -> nlohmann::json {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return v;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return v;
        } else if constexpr (std::is_same_v<T, uint64_t>) {
            return v;
        } else if constexpr (std::is_same_v<T, double>) {
            return v;
        } else if constexpr (std::is_same_v<T, bool>) {
            return v;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            nlohmann::json array = nlohmann::json::array();
            for (const auto& item : v) {
                array.push_back(item);
            }
            return array;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            nlohmann::json array = nlohmann::json::array();
            for (const auto& item : v) {
                array.push_back(item);
            }
            return array;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            nlohmann::json array = nlohmann::json::array();
            for (const auto& item : v) {
                array.push_back(item);
            }
            return array;
        } else {
            return "";
        }
    }, configValue);
#else
    return nlohmann::json();
#endif
}

YAML::Node Config::getYAMLValue(const ConfigValue& configValue) const {
#ifdef NLM_USE_YAML_CPP
    YAML::Node node;
    std::visit([&](const auto& v) {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, std::string>) {
            node = v;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            node = v;
        } else if constexpr (std::is_same_v<T, uint64_t>) {
            node = v;
        } else if constexpr (std::is_same_v<T, double>) {
            node = v;
        } else if constexpr (std::is_same_v<T, bool>) {
            node = v;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            YAML::Node array = YAML::Node(YAML::NodeType::Sequence);
            for (const auto& item : v) {
                array.push_back(item);
            }
            node = array;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            YAML::Node array = YAML::Node(YAML::NodeType::Sequence);
            for (const auto& item : v) {
                array.push_back(item);
            }
            node = array;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            YAML::Node array = YAML::Node(YAML::NodeType::Sequence);
            for (const auto& item : v) {
                array.push_back(item);
            }
            node = array;
        }
    }, configValue);
    return node;
#else
    return YAML::Node();
#endif
}

std::string Config::getStringValue(const ConfigValue& configValue) const {
    return std::visit([](const auto& v) -> std::string {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return v;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return std::to_string(v);
        } else if constexpr (std::is_same_v<T, uint64_t>) {
            return std::to_string(v);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::to_string(v);
        } else if constexpr (std::is_same_v<T, bool>) {
            return v ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            std::string result = "[";
            for (size_t i = 0; i < v.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(v[i]);
            }
            result += "]";
            return result;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            std::string result = "[";
            for (size_t i = 0; i < v.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(v[i]);
            }
            result += "]";
            return result;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            std::string result = "[";
            for (size_t i = 0; i < v.size(); ++i) {
                if (i > 0) result += ", ";
                result += "\"" + v[i] + "\"";
            }
            result += "]";
            return result;
        } else {
            return "";
        }
    }, configValue);
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

} // namespace nlm
