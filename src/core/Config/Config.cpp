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
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
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

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

// Implementation of missing functions
bool Config::validateFilePath(const std::string& filepath) const {
    if (filepath.empty()) {
        return false;
    }
    
    // Check if file exists and is readable
    if (!std::filesystem::exists(filepath)) {
        return false;
    }
    
    if (!std::filesystem::is_regular_file(filepath)) {
        return false;
    }
    
    // Check if file is readable
    std::ifstream test_file(filepath);
    return test_file.good();
}

bool Config::validateConfigKey(const std::string& key) const {
    if (key.empty()) {
        return false;
    }
    
    // Key must start with a letter or underscore
    if (!std::isalpha(static_cast<unsigned char>(key[0])) && key[0] != '_') {
        return false;
    }
    
    // Key can contain letters, digits, underscores, and hyphens
    for (char c : key) {
        if (!std::isalnum(static_cast<unsigned char>(c)) && c != '_' && c != '-') {
            return false;
        }
    }
    
    // Key length should be reasonable (1-100 characters)
    return key.length() >= 1 && key.length() <= 100;
}

bool Config::validateConfigValue(const ConfigValue& value, const std::string& expectedType) const {
    // Enhanced validation based on expectedType
    if (expectedType.empty()) {
        return true; // No validation specified
    }
    
    std::string typeName = expectedType;
    std::transform(typeName.begin(), typeName.end(), typeName.begin(), ::tolower);
    
    if (typeName == "string") {
        if (std::holds_alternative<std::string>(value)) {
            const auto& str = std::get<std::string>(value);
            // Check string length (reasonable limits)
            return str.length() <= 10000; // 10KB max string
        }
        return false;
    } else if (typeName == "int" || typeName == "integer") {
        return std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value);
    } else if (typeName == "double" || typeName == "float") {
        return std::holds_alternative<double>(value);
    } else if (typeName == "bool" || typeName == "boolean") {
        return std::holds_alternative<bool>(value);
    } else if (typeName == "int_array" || typeName == "vector<int>") {
        return std::holds_alternative<std::vector<int>>(value);
    } else if (typeName == "double_array" || typeName == "vector<double>") {
        return std::holds_alternative<std::vector<double>>(value);
    } else if (typeName == "string_array" || typeName == "vector<string>") {
        return std::holds_alternative<std::vector<std::string>>(value);
    }
    
    return true; // Unknown type, assume valid
}

// JSON/YAML parsing implementations
#if defined(HAS_NLOHMANN_JSON)
bool Config::tryParseJSON(const std::string& jsonStr) {
    try {
        auto jsonData = nlohmann::json::parse(jsonStr);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::jsonToConfigValue(const nlohmann::json& json, ConfigValue& outValue) {
    try {
        if (json.is_string()) {
            outValue = json.get<std::string>();
        } else if (json.is_number_integer()) {
            outValue = json.get<int>();
        } else if (json.is_number_unsigned()) {
            outValue = static_cast<int64_t>(json.get<uint64_t>());
        } else if (json.is_number_float()) {
            outValue = json.get<double>();
        } else if (json.is_boolean()) {
            outValue = json.get<bool>();
        } else if (json.is_array()) {
            std::vector<std::string> arrayStr;
            for (size_t i = 0; i < json.size(); ++i) {
                if (json[i].is_string()) {
                    arrayStr.push_back(json[i].get<std::string>());
                } else {
                    arrayStr.push_back(std::to_string(json[i].get<double>()));
                }
            }
            outValue = arrayStr;
        } else if (json.is_null()) {
            outValue = std::string("null");
        } else {
            outValue = json.dump();
        }
        return true;
    } catch (const std::exception&) {
        outValue = std::string();
        return false;
    }
}
#endif

#if defined(HAS_YAML_CPP)
bool Config::tryParseYAML(const std::string& yamlStr) {
    try {
        auto yamlData = YAML::Load(yamlStr);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::yamlToConfigValue(const YAML::Node& yaml, ConfigValue& outValue) {
    try {
        if (yaml.IsScalar()) {
            std::string scalar = yaml.as<std::string>();
            
            if (scalar == "true" || scalar == "false") {
                outValue = (scalar == "true");
            } else {
                try {
                    outValue = std::stoi(scalar);
                } catch (...) {
                    try {
                        outValue = std::stod(scalar);
                    } catch (...) {
                        outValue = scalar;
                    }
                }
            }
        } else if (yaml.IsSequence()) {
            std::vector<std::string> arrayStr;
            for (size_t i = 0; i < yaml.size(); ++i) {
                std::string item = yaml[i].as<std::string>();
                
                if (item == "true" || item == "false") {
                    arrayStr.push_back(item);
                } else {
                    try {
                        int intVal = std::stoi(item);
                        arrayStr.push_back(std::to_string(intVal));
                    } catch (...) {
                        try {
                            double doubleVal = std::stod(item);
                            arrayStr.push_back(std::to_string(doubleVal));
                        } catch (...) {
                            arrayStr.push_back(item);
                        }
                    }
                }
            }
            outValue = arrayStr;
        } else {
            outValue = yaml.as<std::string>();
        }
        return true;
    } catch (const std::exception&) {
        outValue = std::string();
        return false;
    }
}
#endif

std::string Config::configValueToString(const ConfigValue& value) {
    std::string result;
    std::visit([&result](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            result = std::to_string(arg);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            result = std::to_string(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            std::ostringstream oss;
            oss.precision(10);
            oss << std::fixed << arg;
            result = oss.str();
            if (result.find('.') != std::string::npos) {
                result.erase(result.find_last_not_of('0') + 1, std::string::npos);
                if (result.back() == '.') {
                    result.pop_back();
                }
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            result = arg ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::string>) {
            result = "\"" + arg + "\"";
        }
    }, value);
    return result;
}

} // namespace nlm
