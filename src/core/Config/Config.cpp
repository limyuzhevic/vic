#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cmath>
#include <ctime>
#include <iomanip>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Detect file format and delegate to appropriate parser
    if (isJSONFile(filepath)) {
        return loadFromJSON(filepath);
    } else if (isINIFile(filepath)) {
        return loadFromINI(filepath);
    } else if (isYAMLLFile(filepath)) {
        return loadFromYAML(filepath);
    } else {
        // Fallback to simple key=value format
        return parseKeyValueLine("", std::string(), std::string());
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    return parseJSONFromStream(file);
}

bool Config::loadFromINI(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    return parseINIFromStream(file);
}

bool Config::loadFromYAML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    return parseYAMLFromStream(file);
}

bool Config::loadFromArgs(int argc, char** argv) {
    return parseCommandLineArgs(argc, argv);
}

bool Config::saveToFile(const std::string& filepath, const std::string& format) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string fmt = format;
    if (format == "auto") {
        if (isJSONFile(filepath)) {
            fmt = "json";
        } else if (isINIFile(filepath)) {
            fmt = "ini";
        } else if (isYAMLLFile(filepath)) {
            fmt = "yaml";
        } else {
            fmt = "keyvalue";
        }
    }
    
    if (fmt == "json") {
        file << toJSON(*this);
    } else if (fmt == "ini") {
        file << toINI(*this);
    } else if (fmt == "yaml") {
        file << toYAML(*this);
    } else {
        // Default key=value format
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << configValueToString(entry.value) << "\n";
        }
    }
    
    return true;
}

std::optional<ConfigValue> Config::stringToConfigValue(const std::string& str) {
    if (str.empty()) {
        return std::nullopt;
    }
    
    // Try to parse as boolean
    if (str == "true" || str == "1" || str == "yes" || str == "on") {
        return true;
    } else if (str == "false" || str == "0" || str == "no" || str == "off") {
        return false;
    }
    
    // Try to parse as integer
    try {
        size_t pos;
        int intVal = std::stoi(str, &pos);
        if (pos == str.size()) {
            return intVal;
        }
    } catch (...) {}
    
    // Try to parse as int64
    try {
        size_t pos;
        int64_t int64Val = std::stoll(str, &pos);
        if (pos == str.size()) {
            return int64Val;
        }
    } catch (...) {}
    
    // Try to parse as double
    try {
        size_t pos;
        double doubleVal = std::stod(str, &pos);
        if (pos == str.size()) {
            return doubleVal;
        }
    } catch (...) {}
    
    // Try to parse as vector (comma-separated)
    if (str.find(',') != std::string::npos) {
        std::vector<std::string> tokens;
        std::stringstream ss(str);
        std::string token;
        while (std::getline(ss, token, ',')) {
            token = trim(token);
            if (token.empty()) continue;
            auto valueOpt = stringToConfigValue(token);
            if (valueOpt) {
                tokens.push_back(token);
            }
        }
        
        if (!tokens.empty()) {
            // Try to create int vector
            std::vector<int> intVec;
            bool allInt = true;
            for (const auto& token : tokens) {
                try {
                    int val = std::stoi(token);
                    intVec.push_back(val);
                } catch (...) {
                    allInt = false;
                    break;
                }
            }
            if (allInt && !intVec.empty()) {
                return intVec;
            }
            
            // Try to create double vector
            std::vector<double> doubleVec;
            bool allDouble = true;
            for (const auto& token : tokens) {
                try {
                    double val = std::stod(token);
                    doubleVec.push_back(val);
                } catch (...) {
                    allDouble = false;
                    break;
                }
            }
            if (allDouble && !doubleVec.empty()) {
                return doubleVec;
            }
            
            // Return as string vector
            std::vector<std::string> strVec(tokens.begin(), tokens.end());
            return strVec;
        }
    }
    
    // Default to string
    return str;
}

std::string Config::configValueToString(const ConfigValue& value) {
    std::stringstream ss;
    std::visit([&ss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            ss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            ss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i) ss << ", ";
                ss << arg[i];
            }
            ss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            ss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i) ss << ", ";
                ss << arg[i];
            }
            ss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            ss << "[\"";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i) ss << "\", \"";
                ss << arg[i];
            }
            ss << "\"]";
        } else {
            ss << arg;
        }
    }, value);
    return ss.str();
}

bool Config::validate(const ConfigSchemaEntry& schema) const {
    if (!has(schema.key)) {
        if (schema.required) {
            return false;
        }
        return true; // Optional key that's missing is OK
    }
    
    auto valueOpt = get(schema.key);
    if (!valueOpt) {
        return false;
    }
    
    ConfigValue value = *valueOpt;
    
    // Type validation
    ConfigDataType actualType = getDataType(value);
    if (actualType != schema.type) {
        // Try to convert
        if (!tryConvertValue(value, schema.type)) {
            return false;
        }
    }
    
    // Custom validator
    if (schema.validator && !schema.validator(value)) {
        return false;
    }
    
    // Dependency validation
    for (const auto& dep : schema.dependencies) {
        if (!has(dep)) {
            return false;
        }
    }
    
    return true;
}

bool Config::validateAll(const std::vector<ConfigSchemaEntry>& schema) const {
    for (const auto& entry : schema) {
        if (!validate(entry)) {
            return false;
        }
    }
    return true;
}

void Config::addSchemaEntry(const ConfigSchemaEntry& entry) {
    schema[entry.key] = entry;
}

void Config::removeSchemaEntry(const std::string& key) {
    schema.erase(key);
}

std::optional<ConfigSchemaEntry> Config::getSchemaEntry(const std::string& key) const {
    auto it = schema.find(key);
    if (it != schema.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::vector<ConfigSchemaEntry> Config::getSchemaEntries() const {
    std::vector<ConfigSchemaEntry> entries;
    entries.reserve(schema.size());
    for (const auto& pair : schema) {
        entries.push_back(pair.second);
    }
    return entries;
}

std::vector<std::string> Config::parseCommandLineArgs(int argc, char** argv) {
    std::vector<std::string> parsedArgs;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle boolean flags (--enable-feature, --disable-feature)
        if (arg.substr(0, 2) == "--") {
            if (arg.find('=') == std::string::npos) {
                // Boolean flag, default to true
                parsedArgs.push_back(arg + "=true");
            } else {
                // Handle --key=value format
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    parsedArgs.push_back("--" + key + "=" + value);
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc && argv[i + 1][0] != '-') {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            parsedArgs.push_back("--" + key + "=" + value);
        }
    }
    
    return parsedArgs;
}

std::string Config::generateHelp(const std::vector<ConfigSchemaEntry>& schemaEntries) {
    std::stringstream ss;
    ss << "Configuration Options:\n\n";
    
    for (const auto& entry : schemaEntries) {
        ss << "--" << entry.key;
        
        switch (entry.type) {
            case ConfigDataType::Boolean:
                ss << " (bool)";
                break;
            case ConfigDataType::Integer:
                ss << " (int)";
                break;
            case ConfigDataType::Float:
                ss << " (float)";
                break;
            case ConfigDataType::String:
                ss << " (string)";
                break;
            case ConfigDataType::IntVector:
                ss << " (int[])";
                break;
            case ConfigDataType::FloatVector:
                ss << " (float[])";
                break;
            case ConfigDataType::StringVector:
                ss << " (string[])";
                break;
            default:
                break;
        }
        
        ss << ": " << entry.description;
        
        if (entry.required) {
            ss << " [REQUIRED]";
        }
        
        if (entry.defaultValue.has_value()) {
            ss << " [default: " << configValueToString(entry.defaultValue.value()) << "]";
        }
        
        ss << "\n";
    }
    
    return ss.str();
}

std::string Config::getVersionInfo() {
    return "NLM Config System v2.0";
}

bool Config::parseBoolFlag(const std::string& arg) {
    if (arg == "true" || arg == "1" || arg == "yes" || arg == "on") {
        return true;
    } else if (arg == "false" || arg == "0" || arg == "no" || arg == "off") {
        return false;
    }
    return false;
}

std::optional<int64_t> Config::parseIntArg(const std::string& arg) {
    try {
        return std::stoll(arg);
    } catch (...) {
        return std::nullopt;
    }
}

std::optional<double> Config::parseDoubleArg(const std::string& arg) {
    try {
        return std::stod(arg);
    } catch (...) {
        return std::nullopt;
    }
}

std::vector<std::string> Config::parseListArg(const std::string& arg) {
    std::vector<std::string> result;
    std::stringstream ss(arg);
    std::string item;
    while (std::getline(ss, item, ',')) {
        item = trim(item);
        if (!item.empty()) {
            result.push_back(item);
        }
    }
    return result;
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

std::string Config::toJSON(const Config& config) {
    std::stringstream ss;
    ss << "{\n";
    
    for (size_t i = 0; i < config.pImpl->entries.size(); ++i) {
        const auto& entry = config.pImpl->entries[i];
        ss << "  \"" << entry.key << "\": " << configValueToJSON(entry.value);
        if (i < config.pImpl->entries.size() - 1) {
            ss << ",\n";
        } else {
            ss << "\n";
        }
    }
    
    ss << "}\n";
    return ss.str();
}

std::string Config::toINI(const Config& config) {
    std::stringstream ss;
    
    for (const auto& entry : config.pImpl->entries) {
        ss << "# " << entry.description << "\n";
        ss << entry.key << " = " << configValueToString(entry.value) << "\n\n";
    }
    
    return ss.str();
}

std::string Config::toYAML(const Config& config) {
    std::stringstream ss;
    ss << "# Configuration in YAML format\n";
    
    for (const auto& entry : config.pImpl->entries) {
        ss << entry.key << ": " << configValueToString(entry.value) << "\n";
    }
    
    return ss.str();
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

bool Config::startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

bool Config::endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::string Config::trimQuotes(const std::string& str) {
    std::string result = str;
    if (result.size() >= 2 && ((result.front() == '"' && result.back() == '"') ||
        (result.front() == '\'' && result.back() == '\''))) {
        result = result.substr(1, result.size() - 2);
    }
    return result;
}

ConfigSource Config::detectSourceFromFile(const std::string& filepath) {
    if (isJSONFile(filepath)) {
        return ConfigSource::File;
    } else if (isINIFile(filepath)) {
        return ConfigSource::File;
    } else if (isYAMLLFile(filepath)) {
        return ConfigSource::File;
    } else {
        return ConfigSource::File; // Fallback
    }
}

bool Config::isJSONFile(const std::string& filepath) {
    return endsWith(filepath, ".json") || endsWith(filepath, ".JSON");
}

bool Config::isINIFile(const std::string& filepath) {
    return endsWith(filepath, ".ini") || endsWith(filepath, ".INI") ||
        endsWith(filepath, ".cfg") || endsWith(filepath, ".CFG");
}

bool Config::isYAMLLFile(const std::string& filepath) {
    return endsWith(filepath, ".yaml") || endsWith(filepath, ".YAML") ||
        endsWith(filepath, ".yml") || endsWith(filepath, ".YML");
}

bool Config::parseKeyValueLine(const std::string& line, std::string& key, std::string& value) {
    size_t pos = line.find('=');
    if (pos != std::string::npos) {
        key = trim(line.substr(0, pos));
        value = trim(line.substr(pos + 1));
        value = trimQuotes(value);
        return true;
    }
    return false;
}

bool Config::parseJSONFromStream(std::istream& stream) {
    // Simplified JSON parsing for demonstration
    // In practice, would use a full JSON parser like nlohmann::json
    std::string line;
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = trim(line.substr(0, colonPos));
            std::string value = trim(line.substr(colonPos + 1));
            // Remove trailing comma and whitespace
            if (!value.empty() && value.back() == ',') {
                value.pop_back();
            }
            set(key, value, ConfigSource::File);
        }
    }
    return true;
}

bool Config::parseINIFromStream(std::istream& stream) {
    std::string line;
    std::string currentSection = "";
    
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == ';' || line[0] == '#') {
            continue;
        }
        
        // Check for section
        if (line[0] == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.size() - 2);
            continue;
        }
        
        // Parse key = value
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = currentSection.empty() ? trim(line.substr(0, pos)) :
                currentSection + "." + trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            value = trimQuotes(value);
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::parseYAMLFromStream(std::istream& stream) {
    // Simplified YAML parsing for demonstration
    std::string line;
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = trim(line.substr(0, colonPos));
            std::string value = trim(line.substr(colonPos + 1));
            // Remove list marker and whitespace
            if (!value.empty() && value[0] == '-') {
                value = value.substr(1);
            }
            value = trim(value);
            set(key, value, ConfigSource::File);
        }
    }
    return true;
}

std::string Config::configValueToJSON(const ConfigValue& value) const {
    std::stringstream ss;
    std::visit([&ss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            ss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            ss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            ss << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            ss << std::setprecision(6) << arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            ss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i) ss << ", ";
                ss << arg[i];
            }
            ss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            ss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i) ss << ", ";
                ss << std::setprecision(6) << arg[i];
            }
            ss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            ss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i) ss << ", ";
                ss << "\"" << arg[i] << "\"";
            }
            ss << "]";
        } else {
            ss << "null";
        }
    }, value);
    return ss.str();
}

ConfigDataType Config::getDataType(const ConfigValue& value) const {
    // This helper function determines the type of a ConfigValue
    return std::visit([](auto&& arg) -> ConfigDataType {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            return ConfigDataType::Integer;
        } else if constexpr (std::is_same_v<T, double>) {
            return ConfigDataType::Float;
        } else if constexpr (std::is_same_v<T, bool>) {
            return ConfigDataType::Boolean;
        } else if constexpr (std::is_same_v<T, std::string>) {
            return ConfigDataType::String;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return ConfigDataType::IntVector;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return ConfigDataType::FloatVector;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return ConfigDataType::StringVector;
        } else {
            return ConfigDataType::String;
        }
    }, value);
}

bool Config::tryConvertValue(ConfigValue& value, ConfigDataType targetType) const {
    // Helper function to try to convert a value to target type
    // Returns true if conversion successful, false otherwise
    
    std::optional<ConfigValue> tryConvert(const ConfigValue& value, ConfigDataType targetType) {
        return std::visit([](auto&& arg) -> std::optional<ConfigValue> {
            using T = std::decay_t<decltype(arg)>;
            
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                if (targetType == ConfigDataType::Float) {
                    return static_cast<double>(arg);
                } else if (targetType == ConfigDataType::String) {
                    return std::to_string(arg);
                }
            } else if constexpr (std::is_same_v<T, double>) {
                if (targetType == ConfigDataType::Integer) {
                    return static_cast<int64_t>(arg);
                } else if (targetType == ConfigDataType::String) {
                    return std::to_string(arg);
                }
            } else if constexpr (std::is_same_v<T, bool>) {
                if (targetType == ConfigDataType::String) {
                    return arg ? "true" : "false";
                }
            } else if constexpr (std::is_same_v<T, std::string>) {
                if (targetType == ConfigDataType::Integer) {
                    try { return std::stoll(arg); } catch (...) { return std::nullopt; }
                } else if (targetType == ConfigDataType::Float) {
                    try { return std::stod(arg); } catch (...) { return std::nullopt; }
                } else if (targetType == ConfigDataType::Boolean) {
                    return parseBoolFlag(arg);
                }
            }
            
            return std::nullopt;
        }, value);
    }
    
    auto converted = tryConvert(value, targetType);
    if (converted) {
        value = *converted;
        return true;
    }
    
    return false;
}

// Template instantiations for get() and getOr()
template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
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
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;
// Configuration namespace implementation
namespace config {
    const std::vector<nlm::ConfigSchemaEntry> defaultSchema = {
        nlm::ConfigSchemaEntry("random_seed", nlm::ConfigDataType::Integer, "Random seed for reproducible experiments"),
        nlm::ConfigSchemaEntry("simulation_timestep", nlm::ConfigDataType::Float, "Simulation timestep in seconds"),
        nlm::ConfigSchemaEntry("neuron_count", nlm::ConfigDataType::Integer, "Number of neurons in the brain"),
        nlm::ConfigSchemaEntry("region_count", nlm::ConfigDataType::Integer, "Number of brain regions"),
        nlm::ConfigSchemaEntry("connection_probability", nlm::ConfigDataType::Float, "Connection probability for random connectivity"),
        nlm::ConfigSchemaEntry("stdp_ltp_weight", nlm::ConfigDataType::Float, "STDP long-term potentiation weight"),
        nlm::ConfigSchemaEntry("stdp_ltd_weight", nlm::ConfigDataType::Float, "STDP long-term depression weight"),
        nlm::ConfigSchemaEntry("stdp_time_constant", nlm::ConfigDataType::Float, "STDP time constant"),
        nlm::ConfigSchemaEntry("synaptogenesis_rate", nlm::ConfigDataType::Float, "Synaptogenesis rate"),
        nlm::ConfigSchemaEntry("pruning_rate", nlm::ConfigDataType::Float, "Pruning rate"),
        nlm::ConfigSchemaEntry("log_level", nlm::ConfigDataType::String, "Log level (DEBUG, INFO, WARN, ERROR)"),
        nlm::ConfigSchemaEntry("max_simulation_steps", nlm::ConfigDataType::Integer, "Maximum simulation steps"),
        nlm::ConfigSchemaEntry("visualization_enabled", nlm::ConfigDataType::Boolean, "Whether visualization is enabled"),
    };
    
    std::string formatConfigValue(const nlm::ConfigValue& value) {
        std::stringstream ss;
        std::visit([&ss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                ss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                ss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                ss << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                ss << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                ss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) ss << ", ";
                    ss << arg[i];
                }
                ss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                ss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) ss << ", ";
                    ss << std::setprecision(6) << arg[i];
                }
                ss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                ss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) ss << ", ";
                    ss << "\"" << arg[i] << "\"";
                }
                ss << "]";
            } else {
                ss << "null";
            }
        }, value);
        return ss.str();
    }
    
    bool compareConfigValues(const nlm::ConfigValue& a, const nlm::ConfigValue& b) {
        return std::visit([](auto&& argA, auto&& argB) -> bool {
            using T1 = std::decay_t<decltype(argA)>;
            using T2 = std::decay_t<decltype(argB)>;
            
            if constexpr (std::is_same_v<T1, T2>) {
                return argA == argB;
            } else if constexpr (std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2>) {
                return static_cast<double>(argA) == static_cast<double>(argB);
            } else {
                return false;
            }
        }, a, b);
    }
    
    std::vector<std::string> getValueTypeNames(nlm::ConfigDataType type) {
        static const std::vector<std::string> typeNames = {
            "int", "float", "bool", "string", "int[]", "float[]", "string[]", "object", "array"
        };
        return typeNames;
    }
    
    bool validateRange(const nlm::ConfigValue& value, double min, double max) {
        return std::visit([min, max](auto&& arg) -> bool {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_arithmetic_v<T>) {
                double val = static_cast<double>(arg);
                return val >= min && val <= max;
            } else {
                return false;
            }
        }, value);
    }
    
    bool validatePattern(const nlm::ConfigValue& value, const std::string& pattern) {
        std::string strVal;
        std::visit([&strVal](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                strVal = arg;
            }
        }, value);
        
        if (strVal.empty()) return true; // Empty string passes all patterns
        
        std::regex regex(pattern);
        return std::regex_match(strVal, regex);
    }
    
    bool validateEnum(const nlm::ConfigValue& value, const std::vector<std::string>& allowed) {
        std::string strVal;
        std::visit([&strVal](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                strVal = arg;
            }
        }, value);
        
        return std::find(allowed.begin(), allowed.end(), strVal) != allowed.end();
    }
}

// Template instantiations for set() overloads
template void nlm::Config::set<int>(const std::string& key, int value, nlm::ConfigSource source);
template void nlm::Config::set<int64_t>(const std::string& key, int64_t value, nlm::ConfigSource source);
template void nlm::Config::set<double>(const std::string& key, double value, nlm::ConfigSource source);
template void nlm::Config::set<std::string>(const std::string& key, const std::string& value, nlm::ConfigSource source);
template void nlm::Config::set<std::vector<int>>(const std::string& key, const std::vector<int>& value, nlm::ConfigSource source);
template void nlm::Config::set<std::vector<double>>(const std::string& key, const std::vector<double>& value, nlm::ConfigSource source);
template void nlm::Config::set<std::vector<std::string>>(const std::string& key, const std::vector<std::string>& value, nlm::ConfigSource source);
template void nlm::Config::set<bool>(const std::string& key, bool value, nlm::ConfigSource source);
