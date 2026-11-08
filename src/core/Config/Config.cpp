#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <cctype>
#include <regex>
#include <variant>
#include <string>
#include <vector>
#include <iostream>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::function<bool(const ConfigValue&)>> validators;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // First, try to determine file type from extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        return loadFromJSON(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        return loadFromYAML(filepath);
    } else {
        // Fall back to simple key=value format
        return loadFromSimpleFile(filepath);
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
#ifdef HAVE_JSON
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open JSON file: " + filepath);
            return false;
        }
        
        nlohmann::json j;
        file >> j;
        
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            std::string valueStr = it.value().dump();
            
            // Convert JSON value to appropriate ConfigValue
            ConfigValue value = jsonToConfigValue(it.value());
            set(key, value, ConfigSource::File);
        }
        
        NLM_LOG_INFO("Successfully loaded configuration from JSON file: " + filepath);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse JSON file " + filepath + ": " + std::string(e.what()));
        return false;
    }
#else
    NLM_LOG_ERROR("JSON support not available - compile with nlohmann_json or equivalent");
    return false;
#endif
}

bool Config::loadFromYAML(const std::string& filepath) {
#ifdef HAVE_YAML
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open YAML file: " + filepath);
            return false;
        }
        
        YAML::Node doc = YAML::LoadFile(filepath);
        
        for (auto it = doc.begin(); it != doc.end(); ++it) {
            std::string key = it->first.as<std::string>();
            YAML::Node value = it->second;
            
            ConfigValue configValue = yamlToConfigValue(value);
            set(key, configValue, ConfigSource::File);
        }
        
        NLM_LOG_INFO("Successfully loaded configuration from YAML file: " + filepath);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse YAML file " + filepath + ": " + std::string(e.what()));
        return false;
    }
#else
    NLM_LOG_ERROR("YAML support not available - compile with yaml-cpp or equivalent");
    return false;
#endif
}

bool Config::loadFromSimpleFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs with enhanced error handling
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING("Invalid format in config file " + filepath + 
                          " line " + std::to_string(lineNumber) + ": " + line);
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        try {
            set(key, value, ConfigSource::File);
        } catch (const ConfigException& e) {
            NLM_LOG_ERROR("Error setting config key " + key + " in file " + filepath + 
                        " line " + std::to_string(lineNumber) + ": " + std::string(e.what()));
        }
    }
    
    NLM_LOG_INFO("Successfully loaded configuration from simple file: " + filepath);
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (argc < 1) {
        return false;
    }
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            } else {
                NLM_LOG_WARNING("Invalid command line argument format: " + arg);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-') {
            if (i + 1 >= argc) {
                NLM_LOG_ERROR("Missing value for command line argument: " + arg);
                return false;
            }
            
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        } else {
            NLM_LOG_WARNING("Ignoring unrecognized command line argument: " + arg);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    // Determine output format from extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file for writing: " + filepath);
        return false;
    }
    
    // Write in the appropriate format
    if (ext == ".json") {
        saveToJSON(file);
    } else if (ext == ".yaml" || ext == ".yml") {
        saveToYAML(file);
    } else {
        // Default to simple key=value format
        saveToSimpleFormat(file);
    }
    
    NLM_LOG_INFO("Successfully saved configuration to: " + filepath);
    return true;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (key.empty()) {
        throw ConfigException("Configuration key cannot be empty");
    }
    
    // Validate the value using registered validators
    auto it = pImpl->validators.find(key);
    if (it != pImpl->validators.end() && !it->second(value)) {
        throw ConfigException("Validation failed for configuration key: " + key);
    }
    
    auto entryIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (entryIt != pImpl->entries.end()) {
        entryIt->value = value;
        entryIt->source = source;
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

void Config::validate(const std::vector<std::string>& requiredKeys,
                      const std::unordered_map<std::string, std::function<bool(const ConfigValue&)>>& valueValidators) const {
    // Check required keys
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            throw ConfigException("Missing required configuration key: " + key);
        }
    }
    
    // Apply value validators
    for (const auto& [key, validator] : valueValidators) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it != pImpl->entries.end() && !validator(it->value)) {
            throw ConfigException("Validation failed for configuration key: " + key);
        }
    }
}

void Config::merge(const Config& other) {
    for (const auto& entry : other.pImpl->entries) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it != pImpl->entries.end()) {
            // Update existing entry, but don't change its source priority
            it->value = entry.value;
        } else {
            pImpl->entries.push_back(entry);
        }
    }
}

std::string Config::getStats() const {
    std::ostringstream oss;
    oss << "Configuration Statistics:\n";
    oss << "  Total entries: " << pImpl->entries.size() << "\n";
    
    std::unordered_map<ConfigSource, int> sourceCounts;
    for (const auto& entry : pImpl->entries) {
        sourceCounts[entry.source]++;
    }
    
    oss << "  Entries by source:\n";
    for (const auto& [source, count] : sourceCounts) {
        oss << "    " << static_cast<int>(source) << ": " << count << "\n";
    }
    
    oss << "  Largest key: ";
    if (!pImpl->entries.empty()) {
        auto it = std::max_element(pImpl->entries.begin(), pImpl->entries.end(),
            [](const ConfigEntry& a, const ConfigEntry& b) { return a.key.size() < b.key.size(); });
        oss << it->key << " (" << it->key.size() << " chars)\n";
    }
    
    return oss.str();
}

ConfigValue Config::jsonToConfigValue(const nlohmann::json& j) {
    if (j.is_null()) {
        return std::string();
    } else if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_number_integer()) {
        return j.get<int64_t>();
    } else if (j.is_number_unsigned()) {
        return j.get<uint64_t>();
    } else if (j.is_number()) {
        return j.get<double>();
    } else if (j.is_string()) {
        return j.get<std::string>();
    } else if (j.is_array()) {
        std::vector<ConfigValue> result;
        for (const auto& item : j) {
            result.push_back(jsonToConfigValue(item));
        }
        
        // Determine appropriate vector type
        if (!result.empty()) {
            if (std::all_of(result.begin(), result.end(),
                [](const ConfigValue& v) { return std::visit([](auto&& arg) { return std::is_same_v<decltype(arg), int>; }, v); })) {
                std::vector<int> intVec;
                for (const auto& v : result) {
                    intVec.push_back(std::get<int>(v));
                }
                return ConfigValue(intVec);
            } else if (std::all_of(result.begin(), result.end(),
                [](const ConfigValue& v) { return std::visit([](auto&& arg) { return std::is_same_v<decltype(arg), double>; }, v); })) {
                std::vector<double> doubleVec;
                for (const auto& v : result) {
                    doubleVec.push_back(std::get<double>(v));
                }
                return ConfigValue(doubleVec);
            } else {
                // Mixed types or strings - use generic string representation
                std::string jsonStr = j.dump();
                return jsonStr;
            }
        }
        
        return std::vector<ConfigValue>();
    } else {
        return j.get<std::string>();
    }
}

ConfigValue Config::yamlToConfigValue(const YAML::Node& node) {
    if (!node.IsDefined()) {
        return std::string();
    } else if (node.IsScalar()) {
        std::string value = node.as<std::string>();
        // Try to determine type
        if (value == "true" || value == "false") {
            return value == "true";
        } else if (std::regex_match(value, std::regex("^-?\d+$"))) {
            return std::stoll(value);
        } else if (std::regex_match(value, std::regex("^-?\d+\.\d*$") ||
                                std::regex_match(value, std::regex("^-?\d*\.\d+$")))) {
            return std::stod(value);
        } else {
            return value;
        }
    } else if (node.IsSequence()) {
        std::vector<ConfigValue> result;
        for (const auto& item : node) {
            result.push_back(yamlToConfigValue(item));
        }
        
        // Same type detection as JSON conversion
        if (!result.empty()) {
            if (std::all_of(result.begin(), result.end(),
                [](const ConfigValue& v) { return std::visit([](auto&& arg) { return std::is_same_v<decltype(arg), int>; }, v); })) {
                std::vector<int> intVec;
                for (const auto& v : result) {
                    intVec.push_back(std::get<int>(v));
                }
                return ConfigValue(intVec);
            } else if (std::all_of(result.begin(), result.end(),
                [](const ConfigValue& v) { return std::visit([](auto&& arg) { return std::is_same_v<decltype(arg), double>; }, v); })) {
                std::vector<double> doubleVec;
                for (const auto& v : result) {
                    doubleVec.push_back(std::get<double>(v));
                }
                return ConfigValue(doubleVec);
            }
        }
        
        return result;
    } else if (node.IsMap()) {
        // YAML maps are converted to string representation
        return node.as<std::string>();
    } else {
        return node.as<std::string>();
    }
}

void Config::saveToJSON(std::ostream& output) const {
    nlohmann::json j;
    
    for (const auto& entry : pImpl->entries) {
        j[entry.key] = configValueToJson(entry.value);
    }
    
    output << j.dump(2); // Pretty print with 2-space indentation
}

void Config::saveToYAML(std::ostream& output) const {
#ifdef HAVE_YAML
    YAML::Emitter emitter;
    emitter.SetIndent(2);
    
    emitter << YAML::BeginMap;
    for (const auto& entry : pImpl->entries) {
        emitter << YAML::Key << entry.key;
        emitter << YAML::Value << configValueToYAML(entry.value);
    }
    emitter << YAML::EndMap;
    
    output << emitter.c_str();
#endif
}

void Config::saveToSimpleFormat(std::ostream& output) const {
    for (const auto& entry : pImpl->entries) {
        output << "# " << entry.description << "\n";
        output << entry.key << " = " << configValueToString(entry.value) << "\n";
        output << "\n";
    }
}

nlohmann::json Config::configValueToJson(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> nlohmann::json {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return arg;
        } else {
            return arg;
        }
    }, value);
}

YAML::Node Config::configValueToYAML(const ConfigValue& value) {
#ifdef HAVE_YAML
    return std::visit([](auto&& arg) -> YAML::Node {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return YAML::Node(arg);
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            return YAML::Node(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            return YAML::Node(arg);
        } else if constexpr (std::is_same_v<T, bool>) {
            return YAML::Node(arg);
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            YAML::Node node(YAML::NodeType::Sequence);
            for (const auto& item : arg) {
                node.push_back(item);
            }
            return node;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            YAML::Node node(YAML::NodeType::Sequence);
            for (const auto& item : arg) {
                node.push_back(item);
            }
            return node;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            YAML::Node node(YAML::NodeType::Sequence);
            for (const auto& item : arg) {
                node.push_back(item);
            }
            return node;
        } else {
            return YAML::Node();
        }
    }, value);
#else
    return YAML::Node();
#endif
}

std::string Config::configValueToString(const ConfigValue& value) {
    return std::visit([](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return "\"" + arg + "\"";
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            // Format with enough precision but not excessive
            std::ostringstream oss;
            oss << std::setprecision(6) << arg;
            return oss.str();
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            std::ostringstream oss;
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
            return oss.str();
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            std::ostringstream oss;
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << std::setprecision(6) << arg[i];
            }
            oss << "]";
            return oss.str();
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            std::ostringstream oss;
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << arg[i] << "\"";
            }
            oss << "]";
            return oss.str();
        } else {
            return "";
        }
    }, value);
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
