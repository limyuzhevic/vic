#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Determine file format based on extension
    std::string extension = getFileExtension(filepath);
    if (extension == "json") {
        return loadFromJsonStringFromFile(filepath);
    } else if (extension == "yaml" || extension == "yml") {
        return loadFromYamlStringFromFile(filepath);
    } else {
        // Try to auto-detect format
        return tryAutoDetectFormat(filepath);
    }
}

bool Config::loadFromJson(const std::string& jsonString) {
    try {
        auto json = nlohmann::json::parse(jsonString);
        return configEntriesFromJson(json);
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadFromYaml(const std::string& yamlString) {
    try {
        auto yaml = YAML::Load(yamlString);
        return configEntriesFromYaml(YAML::Dump(yaml));
    } catch (const std::exception& e) {
        return false;
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

bool Config::saveToFile(const std::string& filepath, bool useYaml) const {
    std::string extension = getFileExtension(filepath);
    if (useYaml || extension == "yaml" || extension == "yml") {
        return saveToYamlFile(filepath);
    } else {
        return saveToJsonFile(filepath);
    }
}

std::string Config::saveToJson() const {
    auto json = configEntriesToJson();
    return json.dump(2); // Pretty print with 2-space indentation
}

std::string Config::saveToYaml() const {
    // Convert to string representation that YAML library can parse
    std::string jsonStr = saveToJson();
    try {
        auto json = nlohmann::json::parse(jsonStr);
        auto yaml = nlohmann::json::to_yaml(json);
        return YAML::Dump(yaml);
    } catch (const std::exception& e) {
        return "{}";
    }
}

std::vector<std::string> Config::validate() const {
    std::vector<std::string> errors;
    
    // Check for required keys (can be extended based on needs)
    const std::vector<std::string> requiredKeys = {
        "neuron_count",
        "region_count",
        "connection_probability"
    };
    
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            errors.push_back("Missing required configuration key: " + key);
        }
    }
    
    return errors;
}

std::string Config::getFileExtension(const std::string& filepath) {
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos == std::string::npos) {
        return "";
    }
    
    std::string extension = filepath.substr(dotPos + 1);
    // Convert to lowercase for comparison
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    return extension;
}

bool Config::isJsonFile(const std::string& filepath) {
    std::string extension = getFileExtension(filepath);
    return extension == "json";
}

bool Config::isYamlFile(const std::string& filepath) {
    std::string extension = getFileExtension(filepath);
    return extension == "yaml" || extension == "yml";
}

nlohmann::json Config::configEntriesToJson() const {
    nlohmann::json json = nlohmann::json::object();
    
    for (const auto& entry : pImpl->entries) {
        nlohmann::json valueJson;
        
        std::visit([&valueJson](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                valueJson = val;
            } else if constexpr (std::is_same_v<T, bool>) {
                valueJson = val;
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                valueJson = val;
            } else if constexpr (std::is_same_v<T, double>) {
                valueJson = val;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                valueJson = val;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                valueJson = val;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                valueJson = val;
            } else {
                valueJson = val;
            }
        }, entry.value);
        
        json[entry.key] = valueJson;
    }
    
    return json;
}

bool Config::configEntriesFromJson(const nlohmann::json& json) {
    clear();
    
    for (const auto& [key, value] : json.items()) {
        ConfigEntry entry;
        entry.key = key;
        entry.source = ConfigSource::File;
        entry.description = "";
        
        // Try to determine value type and set accordingly
        if (value.is_string()) {
            entry.value = value.get<std::string>();
        } else if (value.is_boolean()) {
            entry.value = value.get<bool>();
        } else if (value.is_number_integer()) {
            // Try to fit into int64_t first
            if (value.get<int64_t>() >= INT_MIN && value.get<int64_t>() <= INT_MAX) {
                entry.value = value.get<int>();
            } else {
                entry.value = value.get<int64_t>();
            }
        } else if (value.is_number_float()) {
            entry.value = value.get<double>();
        } else if (value.is_array()) {
            // Try to determine array type
            if (!value.empty()) {
                if (value[0].is_number_integer()) {
                    std::vector<int> intArray;
                    for (const auto& item : value) {
                        intArray.push_back(item.get<int>());
                    }
                    entry.value = intArray;
                } else if (value[0].is_number_float()) {
                    std::vector<double> doubleArray;
                    for (const auto& item : value) {
                        doubleArray.push_back(item.get<double>());
                    }
                    entry.value = doubleArray;
                } else if (value[0].is_string()) {
                    std::vector<std::string> stringArray;
                    for (const auto& item : value) {
                        stringArray.push_back(item.get<std::string>());
                    }
                    entry.value = stringArray;
                } else {
                    // Fallback to string representation
                    entry.value = value.dump();
                }
            } else {
                // Empty array - default to int vector
                entry.value = std::vector<int>();
            }
        } else {
            // Fallback to string representation
            entry.value = value.dump();
        }
        
        pImpl->entries.push_back(entry);
    }
    
    return true;
}

std::string Config::configEntriesToYaml() const {
    auto json = configEntriesToJson();
    return json.dump(); // Compact JSON for YAML conversion
}

bool Config::configEntriesFromYaml(const std::string& yamlString) {
    try {
        auto yaml = YAML::Load(yamlString);
        // Convert YAML to JSON string and then parse as JSON
        std::string jsonString = YAML::Dump(yaml);
        return configEntriesFromJson(nlohmann::json::parse(jsonString));
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::tryAutoDetectFormat(const std::string& filepath) {
    // Read first few lines to try to detect format
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line1, line2;
    std::getline(file, line1);
    std::getline(file, line2);
    
    // Check for JSON characteristics (starts with { or [
    if (line1.find('{') != std::string::npos) {
        return loadFromJsonStringFromFile(filepath);
    } else if (line1.find('[') != std::string::npos) {
        return loadFromJsonStringFromFile(filepath);
    } else if (line1.find("#") != std::string::npos && line1.find(" = ") != std::string::npos) {
        return loadFromFile(filepath); // Original simple format
    }
    
    // Default to simple format
    return loadFromFile(filepath);
}

bool Config::loadFromJsonStringFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    return loadFromJson(content);
}

bool Config::loadFromYamlStringFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    return loadFromYaml(content);
}

bool Config::saveToJsonFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    auto json = configEntriesToJson();
    file << json.dump(2); // Pretty print with 2-space indentation
    
    return true;
}

bool Config::saveToYamlFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string yamlString = saveToYaml();
    file << yamlString;
    
    return true;
}

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

bool Config::setValidated(const std::string& key, const std::string& value, ConfigSource source) {
    // Simple validation - can be extended based on needs
    if (key.empty()) {
        return false;
    }
    
    set(key, value, source);
    return true;
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
        std::visit([&oss](const auto& arg) {
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
namespace {
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
}

} // namespace nlm

// ConfigEntry to JSON conversion
nlohmann::json ConfigEntry::toJson() const {
    nlohmann::json json = nlohmann::json::object();
    json["key"] = key;
    json["source"] = static_cast<int>(source);
    json["description"] = description;
    
    // Convert value to JSON based on type
    std::visit([&json](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::string>) {
            json["value"] = "string:" + val;
        } else if constexpr (std::is_same_v<T, bool>) {
            json["value"] = "bool:" + std::string(val ? "true" : "false");
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            json["value"] = "int:" + std::to_string(val);
        } else if constexpr (std::is_same_v<T, double>) {
            json["value"] = "double:" + std::to_string(val);
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            json["value"] = "vector_int:";
            for (const auto& item : val) {
                json["value"].push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            json["value"] = "vector_double:";
            for (const auto& item : val) {
                json["value"].push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            json["value"] = "vector_string:";
            for (const auto& item : val) {
                json["value"].push_back(item);
            }
        } else {
            json["value"] = "unknown:" + std::to_string(val);
        }
    }, value);
    
    return json;
}

// ConfigEntry from JSON conversion
std::optional<ConfigEntry> ConfigEntry::fromJson(const nlohmann::json& json) {
    if (!json.contains("key") || !json.contains("value")) {
        return std::nullopt;
    }
    
    ConfigEntry entry;
    entry.key = json["key"].get<std::string>();
    
    if (json.contains("source")) {
        entry.source = static_cast<ConfigSource>(json["source"].get<int>());
    } else {
        entry.source = ConfigSource::Default;
    }
    
    if (json.contains("description")) {
        entry.description = json["description"].get<std::string>();
    }
    
    // Parse value based on type prefix
    if (json["value"].is_string()) {
        std::string valueStr = json["value"].get<std::string>();
        // Try to parse type prefix
        if (valueStr.substr(0, 6) == "string:") {
            entry.value = valueStr.substr(7);
        } else if (valueStr.substr(0, 5) == "bool:") {
            entry.value = (valueStr.substr(5) == "true");
        } else if (valueStr.substr(0, 4) == "int:") {
            entry.value = std::stoi(valueStr.substr(4));
        } else if (valueStr.substr(0, 8) == "double:") {
            entry.value = std::stod(valueStr.substr(8));
        } else if (valueStr.substr(0, 10) == "vector_int:") {
            // Parse as array
            std::vector<int> intArray;
            for (const auto& item : json["value"].items()) {
                intArray.push_back(item.value().get<int>());
            }
            entry.value = intArray;
        } else if (valueStr.substr(0, 13) == "vector_double:") {
            // Parse as array
            std::vector<double> doubleArray;
            for (const auto& item : json["value"].items()) {
                doubleArray.push_back(item.value().get<double>());
            }
            entry.value = doubleArray;
        } else if (valueStr.substr(0, 13) == "vector_string:") {
            // Parse as array
            std::vector<std::string> stringArray;
            for (const auto& item : json["value"].items()) {
                stringArray.push_back(item.value().get<std::string>());
            }
            entry.value = stringArray;
        } else {
            // Try to parse as number
            try {
                if (valueStr.find('.') != std::string::npos) {
                    entry.value = std::stod(valueStr);
                } else {
                    entry.value = std::stoi(valueStr);
                }
            } catch (...) {
                entry.value = valueStr;
            }
        }
    } else if (json["value"].is_boolean()) {
        entry.value = json["value"].get<bool>();
    } else if (json["value"].is_number_integer()) {
        entry.value = json["value"].get<int>();
    } else if (json["value"].is_number_float()) {
        entry.value = json["value"].get<double>();
    } else if (json["value"].is_array()) {
        // Check array type
        if (!json["value"].empty()) {
            if (json["value"][0].is_number_integer()) {
                std::vector<int> intArray;
                for (const auto& item : json["value"]) {
                    intArray.push_back(item.get<int>());
                }
                entry.value = intArray;
            } else if (json["value"][0].is_number_float()) {
                std::vector<double> doubleArray;
                for (const auto& item : json["value"]) {
                    doubleArray.push_back(item.get<double>());
                }
                entry.value = doubleArray;
            } else if (json["value"][0].is_string()) {
                std::vector<std::string> stringArray;
                for (const auto& item : json["value"]) {
                    stringArray.push_back(item.get<std::string>());
                }
                entry.value = stringArray;
            } else {
                entry.value = json["value"].dump();
            }
        } else {
            entry.value = std::vector<int>();
        }
    } else {
        entry.value = json["value"].dump();
    }
    
    return entry;
}

} // namespace nlm

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
        
        // Format value based on type
        std::visit([&file](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << val << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (val ? "true" : "false") << "\n";
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                file << entry.key << " = " << val << "\n";
            } else if constexpr (std::is_same_v<T, double>) {
                file << entry.key << " = " << std::to_string(val) << "\n";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < val.size(); ++i) {
                    file << val[i];
                    if (i < val.size() - 1) file << ", ";
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < val.size(); ++i) {
                    file << std::to_string(val[i]);
                    if (i < val.size() - 1) file << ", ";
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << entry.key << " = [\"";
                for (size_t i = 0; i < val.size(); ++i) {
                    file << val[i];
                    if (i < val.size() - 1) file << "\", \"";
                }
                file << "\"]\n";
            } else {
                file << entry.key << " = " << val << "\n";
            }
        }, entry.value);
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
