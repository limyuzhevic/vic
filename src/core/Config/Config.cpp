#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <exception>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <set>

// Forward declarations for external libraries
#ifdef HAS_NLOHMANN_JSON
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#endif

#ifdef HAS_YAML_CPP
#include <yaml-cpp/yaml.h>
#endif

namespace nlm {

// Configuration format enumeration
enum class ConfigFormat {
    Unknown,
    SimpleKeyValue,
    JSON,
    YAML,
    INI
};

// Configuration implementation
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::function<bool(const ConfigValue&)>> validators;
    std::unordered_map<std::string, std::string> typeHints;
    bool validateOnSet;
    bool autoBackup;

    Impl() : validateOnSet(false), autoBackup(true) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;
Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    try {
        if (!fileExists(filepath)) {
            throw ConfigFileException("File does not exist: " + filepath);
        }

        // Detect format based on file extension first
        std::string extension = getFileExtension(filepath);
        ConfigFormat format = ConfigFormat::Unknown;

        if (extension == "json") {
            format = ConfigFormat::JSON;
        } else if (extension == "yaml" || extension == "yml") {
            format = ConfigFormat::YAML;
        } else if (extension == "ini" || extension == "cfg") {
            format = ConfigFormat::INI;
        } else {
            // Try to detect format by content
            format = detectFormat(filepath);
        }

        // Create backup if enabled
        if (pImpl->autoBackup) {
            createBackup(filepath);
        }

        // Read file content
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw ConfigFileException("Cannot open file for reading: " + filepath);
        }

        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        pImpl->entries.clear();

        switch (format) {
            case ConfigFormat::JSON:
#ifdef HAS_NLOHMANN_JSON
                parseJSON(content, pImpl->entries);
#else
                throw ConfigException("JSON parser not available (nlohmann/json not found)");
#endif
                break;

            case ConfigFormat::YAML:
#ifdef HAS_YAML_CPP
                parseYAML(content, pImpl->entries);
#else
                throw ConfigException("YAML parser not available (yaml-cpp not found)");
#endif
                break;

            case ConfigFormat::INI:
                parseINI(content, pImpl->entries);
                break;

            case ConfigFormat::SimpleKeyValue:
                parseSimpleKeyValue(content, pImpl->entries);
                break;

            default:
                throw ConfigParseException("Unsupported file format");
        }

        return true;

    } catch (const ConfigException& e) {
        std::cerr << "Error loading config from file: " << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error loading config from file: " << e.what() << std::endl;
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

                try {
                    set(key, value, ConfigSource::CommandLine);
                } catch (const ConfigValidationException& e) {
                    std::cerr << "Validation error for argument --" << key << ": " << e.what() << std::endl;
                    return false;
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];

            try {
                set(key, value, ConfigSource::CommandLine);
            } catch (const ConfigValidationException& e) {
                std::cerr << "Validation error for argument -" << key << ": " << e.what() << std::endl;
                return false;
            }
        }
        // Handle boolean flags
        else if (arg.substr(0, 2) == "--" && (i + 1 >= argc || argv[i + 1][0] == '-')) {
            std::string key = arg.substr(2);
            set(key, true, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        if (pImpl->entries.empty()) {
            std::cerr << "Warning: Config is empty, nothing to save" << std::endl;
            return false;
        }

        // Determine output format based on file extension
        std::string extension = getFileExtension(filepath);

        if (extension == "json") {
#ifdef HAS_NLOHMANN_JSON
            return saveToJSON(filepath);
#else
            throw ConfigException("JSON serializer not available (nlohmann/json not found)");
#endif
        } else if (extension == "yaml" || extension == "yml") {
#ifdef HAS_YAML_CPP
            return saveToYAML(filepath);
#else
            throw ConfigException("YAML serializer not available (yaml-cpp not found)");
#endif
        } else if (extension == "ini" || extension == "cfg") {
            return saveToINI(filepath);
        } else {
            return saveToSimpleKeyValue(filepath);
        }
    } catch (const ConfigException& e) {
        std::cerr << "Error saving config to file: " << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error saving config to file: " << e.what() << std::endl;
        return false;
    }
}

// Helper methods for value conversion
std::string Config::valueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << std::setprecision(6) << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << arg[i] << "\"";
            }
            oss << "]";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

// Template implementations
#include <sstream>

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
    // Validate if enabled
    if (pImpl->validateOnSet) {
        auto typeIt = pImpl->typeHints.find(key);
        if (typeIt != pImpl->typeHints.end()) {
            if (!validateType(value, typeIt->second)) {
                throw ConfigValidationException("Invalid type for key '" + key + "'. Expected: " + typeIt->second +
                    ", Got: " + configValueToString(value));
            }

            auto validatorIt = pImpl->validators.find(key);
            if (validatorIt != pImpl->validators.end()) {
                if (!validatorIt->second(value)) {
                    throw ConfigValidationException("Validation failed for key '" + key + "'");
                }
            }
        }
    }

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
    try {
        set(key, ConfigValue(value), source);
    } catch (const ConfigValidationException& e) {
        std::cerr << "Validation error for key '" << key << "': " << e.what() << std::endl;
        throw;
    }
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
    pImpl->validators.erase(key);
    pImpl->typeHints.erase(key);
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
    pImpl->validators.clear();
    pImpl->typeHints.clear();
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
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << std::setprecision(6) << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << arg[i] << "\"";
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

// Helper functions definitions
inline bool fileExists(const std::string& filepath) {
    return std::filesystem::exists(filepath);
}

inline std::string getFileExtension(const std::string& filepath) {
    size_t pos = filepath.rfind('.');
    if (pos == std::string::npos) return "";
    return toLower(filepath.substr(pos + 1));
}

inline std::string getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y%m%d_%H%M%S");
    return ss.str();
}

inline void createBackup(const std::string& filepath) {
    if (fileExists(filepath)) {
        std::filesystem::path srcPath(filepath);
        std::filesystem::path backupPath = filepath + ".backup_" + getTimestamp();
        std::filesystem::copy_file(srcPath, backupPath, std::filesystem::copy_options::overwrite_existing);
    }
}

inline std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

inline std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

inline std::string configValueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << std::setprecision(6) << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << arg[i] << "\"";
            }
            oss << "]";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

// Type validation function templates
inline bool validateType(const ConfigValue& value, const std::string& expectedType) {
    using T = std::decay_t<decltype(value)>;
    if (expectedType == "int" && std::is_same_v<T, int>) return true;
    if (expectedType == "int64_t" && std::is_same_v<T, int64_t>) return true;
    if (expectedType == "double" && std::is_same_v<T, double>) return true;
    if (expectedType == "bool" && std::is_same_v<T, bool>) return true;
    if (expectedType == "string" && std::is_same_v<T, std::string>) return true;
    if (expectedType == "vector<int>" && std::is_same_v<T, std::vector<int>>) return true;
    if (expectedType == "vector<double>" && std::is_same_v<T, std::vector<double>>) return true;
    if (expectedType == "vector<string>" && std::is_same_v<T, std::vector<std::string>>) return true;
    return false;
}

// Configuration format detection
inline ConfigFormat detectFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ConfigFileException("Cannot open file: " + filepath);
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    if (content.empty()) {
        return ConfigFormat::SimpleKeyValue;
    }

    std::string ext = getFileExtension(filepath);
    
    // Detection based on file extension
    if (ext == "json") {
        try {
#ifdef HAS_NLOHMANN_JSON
            auto json = json::parse(content);
            if (json.is_object()) {
                return ConfigFormat::JSON;
            }
#endif
        } catch (...) {}
    } else if (ext == "yaml" || ext == "yml") {
        try {
#ifdef HAS_YAML_CPP
            auto yaml = YAML::Load(content);
            if (!yaml.IsNull() && yaml.IsMap()) {
                return ConfigFormat::YAML;
            }
#endif
        } catch (...) {}
    } else if (ext == "ini" || ext == "cfg") {
        return ConfigFormat::INI;
    }

    // Detection based on content
    try {
#ifdef HAS_NLOHMANN_JSON
        auto json = json::parse(content);
        if (json.is_object()) {
            return ConfigFormat::JSON;
        }
#endif
    } catch (...) {}

    try {
#ifdef HAS_YAML_CPP
        auto yaml = YAML::Load(content);
        if (!yaml.IsNull() && yaml.IsMap()) {
            return ConfigFormat::YAML;
        }
#endif
    } catch (...) {}

    // Check for INI format
    bool isINI = true;
    std::istringstream stream(content);
    std::string line;
    size_t sectionStart = std::string::npos;
    bool hasSection = false;
    
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        
        if (line.find('[') != std::string::npos && line.find(']') != std::string::npos) {
            hasSection = true;
        } else if (line.find('=') == std::string::npos) {
            isINI = false;
            break;
        }
    }
    
    if (isINI && hasSection) {
        return ConfigFormat::INI;
    }

    return ConfigFormat::SimpleKeyValue;
}

// Simple key=value parser
inline void parseSimpleKeyValue(const std::string& content, std::vector<ConfigEntry>& entries) {
    std::istringstream stream(content);
    std::string line;
    size_t lineNumber = 0;

    while (std::getline(stream, line)) {
        ++lineNumber;
        line = trim(line);

        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }

        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            throw ConfigParseException("Missing '=' in line " + std::to_string(lineNumber));
        }

        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));

        if (key.empty()) {
            throw ConfigParseException("Empty key in line " + std::to_string(lineNumber));
        }

        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }

        // Parse value to appropriate type
        ConfigValue configValue;
        bool parsed = false;

        // Try to parse as int
        try {
            size_t pos;
            int intVal = std::stoi(value, &pos);
            if (pos == value.size()) {
                configValue = intVal;
                parsed = true;
            }
        } catch (...) {}

        // Try to parse as double
        if (!parsed) {
            try {
                size_t pos;
                double doubleVal = std::stod(value, &pos);
                if (pos == value.size()) {
                    configValue = doubleVal;
                    parsed = true;
                }
            } catch (...) {}
        }

        // Try to parse as bool
        if (!parsed) {
            if (toLower(value) == "true") {
                configValue = true;
                parsed = true;
            } else if (toLower(value) == "false") {
                configValue = false;
                parsed = true;
            }
        }

        // Try to parse as vector
        if (!parsed) {
            if (value.find('[') == 0 && value.find(']') == value.size() - 1) {
                std::string inner = value.substr(1, value.size() - 2);
                std::istringstream innerStream(inner);
                std::string item;
                std::vector<std::string> stringItems;

                while (std::getline(innerStream, item, ',')) {
                    item = trim(item);
                    if (item.empty()) continue;
                    stringItems.push_back(item);
                }

                if (!stringItems.empty()) {
                    bool allInt = true;
                    bool allDouble = true;

                    for (const auto& item : stringItems) {
                        try {
                            size_t pos;
                            std::stoi(item, &pos);
                            if (pos != item.size()) allInt = false;
                        } catch (...) { allInt = false; }

                        try {
                            size_t pos;
                            std::stod(item, &pos);
                            if (pos != item.size()) allDouble = false;
                        } catch (...) { allDouble = false; }
                    }

                    if (allInt) {
                        std::vector<int> intVector;
                        for (const auto& item : stringItems) {
                            intVector.push_back(std::stoi(item));
                        }
                        configValue = intVector;
                        parsed = true;
                    } else if (allDouble) {
                        std::vector<double> doubleVector;
                        for (const auto& item : stringItems) {
                            doubleVector.push_back(std::stod(item));
                        }
                        configValue = doubleVector;
                        parsed = true;
                    } else {
                        std::vector<std::string> stringVector;
                        for (const auto& item : stringItems) {
                            if (!item.empty() && item.front() == '\"' && item.back() == '\"') {
                                stringVector.push_back(item.substr(1, item.size() - 2));
                            } else {
                                stringVector.push_back(item);
                            }
                        }
                        configValue = stringVector;
                        parsed = true;
                    }
                }
            }
        }

        // Default to string
        if (!parsed) {
            configValue = value;
        }

        entries.emplace_back(key, configValue, ConfigSource::File);
    }
}

// JSON parser
#ifdef HAS_NLOHMANN_JSON
inline void parseJSON(const std::string& content, std::vector<ConfigEntry>& entries) {
    auto json = json::parse(content);

    for (auto it = json.begin(); it != json.end(); ++it) {
        const std::string& key = it.key();
        const json& value = it.value();

        ConfigValue configValue;

        if (value.is_boolean()) {
            configValue = value.get<bool>();
        } else if (value.is_number_integer()) {
            configValue = value.get<int>();
        } else if (value.is_number_unsigned()) {
            configValue = value.get<int64_t>();
        } else if (value.is_number_float()) {
            configValue = value.get<double>();
        } else if (value.is_string()) {
            configValue = value.get<std::string>();
        } else if (value.is_array()) {
            bool allInt = true;
            bool allDouble = true;
            bool allString = true;

            for (const auto& item : value) {
                if (!item.is_number_integer()) allInt = false;
                if (!item.is_number_float() && !item.is_number_integer()) allDouble = false;
                if (!item.is_string()) allString = false;
            }

            if (allInt) {
                std::vector<int> intVector;
                for (const auto& item : value) {
                    intVector.push_back(item.get<int>());
                }
                configValue = intVector;
            } else if (allDouble) {
                std::vector<double> doubleVector;
                for (const auto& item : value) {
                    doubleVector.push_back(item.get<double>());
                }
                configValue = doubleVector;
            } else if (allString) {
                std::vector<std::string> stringVector;
                for (const auto& item : value) {
                    stringVector.push_back(item.get<std::string>());
                }
                configValue = stringVector;
            } else {
                std::ostringstream oss;
                oss << "[";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << value[i];
                }
                oss << "]";
                configValue = oss.str();
            }
        } else {
            configValue = value.dump();
        }

        entries.emplace_back(key, configValue, ConfigSource::File);
    }
}
#endif

// YAML parser
#ifdef HAS_YAML_CPP
inline void parseYAML(const std::string& content, std::vector<ConfigEntry>& entries) {
    auto yaml = YAML::Load(content);

    if (!yaml.IsMap()) {
        throw ConfigParseException("YAML content is not a mapping (object)");
    }

    for (const auto& node : yaml) {
        const std::string& key = node.first.as<std::string>();
        const YAML::Node& value = node.second;

        ConfigValue configValue;

        if (value.IsBoolean()) {
            configValue = value.as<bool>();
        } else if (value.IsInteger()) {
            configValue = value.as<int>();
        } else if (value.IsScalar()) {
            if (value.IsString()) {
                configValue = value.as<std::string>();
            } else {
                std::string strValue = value.as<std::string>();
                try {
                    size_t pos;
                    int intVal = std::stoi(strValue, &pos);
                    if (pos == strValue.size()) {
                        configValue = intVal;
                    } else {
                        configValue = strValue;
                    }
                } catch (...) {
                    try {
                        size_t pos;
                        double doubleVal = std::stod(strValue, &pos);
                        if (pos == strValue.size()) {
                            configValue = doubleVal;
                        } else {
                            configValue = strValue;
                        }
                    } catch (...) {
                        configValue = strValue;
                    }
                }
            }
        } else if (value.IsSequence()) {
            bool allInt = true;
            bool allDouble = true;
            bool allString = true;

            for (const auto& item : value) {
                if (!item.IsInteger()) allInt = false;
                if (!item.IsScalar() || item.IsNull()) allDouble = false;
                if (!item.IsString()) allString = false;
            }

            if (allInt) {
                std::vector<int> intVector;
                for (const auto& item : value) {
                    intVector.push_back(item.as<int>());
                }
                configValue = intVector;
            } else if (allDouble) {
                std::vector<double> doubleVector;
                for (const auto& item : value) {
                    if (item.IsScalar() && !item.IsNull()) {
                        std::string strVal = item.as<std::string>();
                        try {
                            doubleVector.push_back(std::stod(strVal));
                        } catch (...) {
                            doubleVector.push_back(0.0);
                        }
                    } else {
                        doubleVector.push_back(0.0);
                    }
                }
                configValue = doubleVector;
            } else if (allString) {
                std::vector<std::string> stringVector;
                for (const auto& item : value) {
                    if (item.IsString()) {
                        stringVector.push_back(item.as<std::string>());
                    } else {
                        stringVector.push_back(item.as<std::string>());
                    }
                }
                configValue = stringVector;
            } else {
                std::ostringstream oss;
                oss << "[";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << value[i].as<std::string>();
                }
                oss << "]";
                configValue = oss.str();
            }
        } else {
            std::string strValue = value.IsNull() ? "null" : value.as<std::string>();
            configValue = strValue;
        }

        entries.emplace_back(key, configValue, ConfigSource::File);
    }
}
#endif

// INI parser
inline void parseINI(const std::string& content, std::vector<ConfigEntry>& entries) {
    std::istringstream stream(content);
    std::string line;
    std::string currentSection;
    size_t lineNumber = 0;

    while (std::getline(stream, line)) {
        ++lineNumber;
        line = trim(line);

        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }

        // Check for section
        if (line.find('[') != std::string::npos && line.find(']') != std::string::npos) {
            size_t bracketStart = line.find('[');
            size_t bracketEnd = line.find(']');
            currentSection = line.substr(bracketStart + 1, bracketEnd - bracketStart - 1);
            continue;
        }

        // Check for key=value pair
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            continue;
        }

        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));

        if (key.empty()) {
            continue;
        }

        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }

        std::string fullKey = currentSection.empty() ? key : currentSection + "." + key;

        ConfigValue configValue;
        bool parsed = false;

        try {
            size_t pos;
            int intVal = std::stoi(value, &pos);
            if (pos == value.size()) {
                configValue = intVal;
                parsed = true;
            }
        } catch (...) {}

        if (!parsed) {
            try {
                size_t pos;
                double doubleVal = std::stod(value, &pos);
                if (pos == value.size()) {
                    configValue = doubleVal;
                    parsed = true;
                }
            } catch (...) {}
        }

        if (!parsed) {
            if (toLower(value) == "true") {
                configValue = true;
                parsed = true;
            } else if (toLower(value) == "false") {
                configValue = false;
                parsed = true;
            }
        }

        if (!parsed) {
            if (value.find('[') == 0 && value.find(']') == value.size() - 1) {
                std::string inner = value.substr(1, value.size() - 2);
                std::istringstream innerStream(inner);
                std::string item;
                std::vector<std::string> stringItems;

                while (std::getline(innerStream, item, ',')) {
                    item = trim(item);
                    if (item.empty()) continue;
                    stringItems.push_back(item);
                }

                if (!stringItems.empty()) {
                    bool allInt = true;
                    bool allDouble = true;

                    for (const auto& item : stringItems) {
                        try {
                            size_t pos;
                            std::stoi(item, &pos);
                            if (pos != item.size()) allInt = false;
                        } catch (...) { allInt = false; }

                        try {
                            size_t pos;
                            std::stod(item, &pos);
                            if (pos != item.size()) allDouble = false;
                        } catch (...) { allDouble = false; }
                    }

                    if (allInt) {
                        std::vector<int> intVector;
                        for (const auto& item : stringItems) {
                            intVector.push_back(std::stoi(item));
                        }
                        configValue = intVector;
                        parsed = true;
                    } else if (allDouble) {
                        std::vector<double> doubleVector;
                        for (const auto& item : stringItems) {
                            doubleVector.push_back(std::stod(item));
                        }
                        configValue = doubleVector;
                        parsed = true;
                    } else {
                        std::vector<std::string> stringVector;
                        for (const auto& item : stringItems) {
                            if (!item.empty() && item.front() == '\"' && item.back() == '\"') {
                                stringVector.push_back(item.substr(1, item.size() - 2));
                            } else {
                                stringVector.push_back(item);
                            }
                        }
                        configValue = stringVector;
                        parsed = true;
                    }
                }
            }
        }

        if (!parsed) {
            configValue = value;
        }

        entries.emplace_back(fullKey, configValue, ConfigSource::File);
    }
}

// Config validation and configuration methods
void Config::setValidation(const std::string& key, const std::function<bool(const ConfigValue&)>& validator) {
    pImpl->validators[key] = validator;
}

void Config::setTypeHint(const std::string& key, const std::string& typeHint) {
    pImpl->typeHints[key] = typeHint;
}

void Config::enableValidation(bool enable) {
    pImpl->validateOnSet = enable;
}

void Config::enableAutoBackup(bool enable) {
    pImpl->autoBackup = enable;
}

bool Config::validateAll() const {
    for (const auto& entry : pImpl->entries) {
        auto typeIt = pImpl->typeHints.find(entry.key);
        if (typeIt != pImpl->typeHints.end()) {
            if (!validateType(entry.value, typeIt->second)) {
                std::cerr << "Validation failed for key '" << entry.key << "': invalid type" << std::endl;
                return false;
            }

            auto validatorIt = pImpl->validators.find(entry.key);
            if (validatorIt != pImpl->validators.end()) {
                if (!validatorIt->second(entry.value)) {
                    std::cerr << "Validation failed for key '" << entry.key << "'" << std::endl;
                    return false;
                }
            }
        }
    }
    return true;
}

// Getters for configuration
bool Config::getValidateOnSet() const {
    return pImpl->validateOnSet;
}

bool Config::getAutoBackup() const {
    return pImpl->autoBackup;
}

std::unordered_map<std::string, std::string> Config::getTypeHints() const {
    return pImpl->typeHints;
}

std::unordered_map<std::string, std::function<bool(const ConfigValue&)>> Config::getValidators() const {
    return pImpl->validators;
}

// JSON/YAML conversion methods
std::string Config::toJSON() const {
#ifdef HAS_NLOHMANN_JSON
    json j;
    for (const auto& entry : pImpl->entries) {
        j[entry.key] = valueToJson(entry.value);
    }
    return j.dump(2);
#else
    throw ConfigException("JSON serializer not available");
#endif
}

std::string Config::toYAML() const {
#ifdef HAS_YAML_CPP
    YAML::Emitter emitter;
    emitter << YAML::BeginMap;
    for (const auto& entry : pImpl->entries) {
        emitter << YAML::Key << entry.key;
        emitter << YAML::Value << valueToYamlString(entry.value);
    }
    emitter << YAML::EndMap;
    return emitter.c_str();
#else
    throw ConfigException("YAML serializer not available");
#endif
}

bool Config::fromJSON(const std::string& jsonStr) {
#ifdef HAS_NLOHMANN_JSON
    try {
        auto json = json::parse(jsonStr);
        pImpl->entries.clear();
        
        for (auto it = json.begin(); it != json.end(); ++it) {
            const std::string& key = it.key();
            const json& value = it.value();

            ConfigValue configValue;

            if (value.is_boolean()) {
                configValue = value.get<bool>();
            } else if (value.is_number_integer()) {
                configValue = value.get<int>();
            } else if (value.is_number_unsigned()) {
                configValue = value.get<int64_t>();
            } else if (value.is_number_float()) {
                configValue = value.get<double>();
            } else if (value.is_string()) {
                configValue = value.get<std::string>();
            } else if (value.is_array()) {
                bool allInt = true;
                bool allDouble = true;
                bool allString = true;

                for (const auto& item : value) {
                    if (!item.is_number_integer()) allInt = false;
                    if (!item.is_number_float() && !item.is_number_integer()) allDouble = false;
                    if (!item.is_string()) allString = false;
                }

                if (allInt) {
                    std::vector<int> intVector;
                    for (const auto& item : value) {
                        intVector.push_back(item.get<int>());
                    }
                    configValue = intVector;
                } else if (allDouble) {
                    std::vector<double> doubleVector;
                    for (const auto& item : value) {
                        doubleVector.push_back(item.get<double>());
                    }
                    configValue = doubleVector;
                } else if (allString) {
                    std::vector<std::string> stringVector;
                    for (const auto& item : value) {
                        stringVector.push_back(item.get<std::string>());
                    }
                    configValue = stringVector;
                } else {
                    std::ostringstream oss;
                    oss << "[";
                    for (size_t i = 0; i < value.size(); ++i) {
                        if (i > 0) oss << ", ";
                        oss << value[i];
                    }
                    oss << "]";
                    configValue = oss.str();
                }
            } else {
                configValue = value.dump();
            }

            pImpl->entries.emplace_back(key, configValue, ConfigSource::File);
        }

        return true;
    } catch (const json::exception& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return false;
    }
#else
    std::cerr << "JSON serializer not available" << std::endl;
    return false;
#endif
}

bool Config::fromYAML(const std::string& yamlStr) {
#ifdef HAS_YAML_CPP
    try {
        auto yaml = YAML::Load(yamlStr);
        pImpl->entries.clear();
        
        if (!yaml.IsMap()) {
            std::cerr << "YAML content is not a mapping (object)" << std::endl;
            return false;
        }

        for (const auto& node : yaml) {
            const std::string& key = node.first.as<std::string>();
            const YAML::Node& value = node.second;

            ConfigValue configValue;

            if (value.IsBoolean()) {
                configValue = value.as<bool>();
            } else if (value.IsInteger()) {
                configValue = value.as<int>();
            } else if (value.IsScalar()) {
                if (value.IsString()) {
                    configValue = value.as<std::string>();
                } else {
                    std::string strValue = value.as<std::string>();
                    try {
                        size_t pos;
                        int intVal = std::stoi(strValue, &pos);
                        if (pos == strValue.size()) {
                            configValue = intVal;
                        } else {
                            configValue = strVal;
                        }
                    } catch (...) {
                        try {
                            size_t pos;
                            double doubleVal = std::stod(strValue, &pos);
                            if (pos == strValue.size()) {
                                configValue = doubleVal;
                            } else {
                                configValue = strValue;
                            }
                        } catch (...) {
                            configValue = strValue;
                        }
                    }
                }
            } else if (value.IsSequence()) {
                bool allInt = true;
                bool allDouble = true;
                bool allString = true;

                for (const auto& item : value) {
                    if (!item.IsInteger()) allInt = false;
                    if (!item.IsScalar() || item.IsNull()) allDouble = false;
                    if (!item.IsString()) allString = false;
                }

                if (allInt) {
                    std::vector<int> intVector;
                    for (const auto& item : value) {
                        intVector.push_back(item.as<int>());
                    }
                    configValue = intVector;
                } else if (allDouble) {
                    std::vector<double> doubleVector;
                    for (const auto& item : value) {
                        if (item.IsScalar() && !item.IsNull()) {
                            std::string strVal = item.as<std::string>();
                            try {
                                doubleVector.push_back(std::stod(strVal));
                            } catch (...) {
                                doubleVector.push_back(0.0);
                            }
                        } else {
                            doubleVector.push_back(0.0);
                        }
                    }
                    configValue = doubleVector;
                } else if (allString) {
                    std::vector<std::string> stringVector;
                    for (const auto& item : value) {
                        if (item.IsString()) {
                            stringVector.push_back(item.as<std::string>());
                        } else {
                            stringVector.push_back(item.as<std::string>());
                        }
                    }
                    configValue = stringVector;
                } else {
                    std::ostringstream oss;
                    oss << "[";
                    for (size_t i = 0; i < value.size(); ++i) {
                        if (i > 0) oss << ", ";
                        oss << value[i].as<std::string>();
                    }
                    oss << "]";
                    configValue = oss.str();
                }
            } else {
                std::string strValue = value.IsNull() ? "null" : value.as<std::string>();
                configValue = strValue;
            }

            pImpl->entries.emplace_back(key, configValue, ConfigSource::File);
        }

        return true;
    } catch (const YAML::Exception& e) {
        std::cerr << "YAML parse error: " << e.what() << std::endl;
        return false;
    }
#else
    std::cerr << "YAML serializer not available" << std::endl;
    return false;
#endif
}

// Restore configuration from backup file
bool Config::restoreFromBackup(const std::string& filepath) {
    std::string backupPath = getBackupPath(filepath);
    if (!fileExists(backupPath)) {
        std::cerr << "No backup file found: " << backupPath << std::endl;
        return false;
    }

    return loadFromFile(backupPath);
}

// Get list of backup files
std::vector<std::string> Config::getBackups(const std::string& filepath) {
    std::vector<std::string> backups;
    std::filesystem::path path(filepath);
    std::string parent = path.parent_path().string();
    std::string filename = path.filename().string();
    
    for (const auto& entry : std::filesystem::directory_iterator(parent)) {
        if (entry.is_regular_file()) {
            std::string name = entry.path().filename().string();
            if (name.find(filename) != std::string::npos && name.find(".backup_") != std::string::npos) {
                backups.push_back(entry.path().string());
            }
        }
    }

    std::sort(backups.begin(), backups.end(),
        [](const std::string& a, const std::string& b) {
            return std::filesystem::last_write_time(std::filesystem::path(a)) > 
                   std::filesystem::last_write_time(std::filesystem::path(b));
        });

    return backups;
}

// Delete backup files
void Config::deleteBackups(const std::string& filepath) {
    std::filesystem::path path(filepath);
    std::string parent = path.parent_path().string();
    std::string filename = path.filename().string();
    
    for (const auto& entry : std::filesystem::directory_iterator(parent)) {
        if (entry.is_regular_file()) {
            std::string name = entry.path().filename().string();
            if (name.find(filename) != std::string::npos && name.find(".backup_") != std::string::npos) {
                std::filesystem::remove(entry.path());
            }
        }
    }
}

// Export configuration to CSV format
std::string Config::toCSV() const {
    std::ostringstream oss;
    
    // Write header
    oss << "Key,Value,Source,Description" << std::endl;
    
    // Write data
    for (const auto& entry : pImpl->entries) {
        oss << entry.key << ",";
        oss << "\"" << configValueToString(entry.value) << "\"" << ",";
        oss << static_cast<int>(entry.source) << ",";
        oss << "\"" << entry.description << "\"" << std::endl;
    }
    
    return oss.str();
}

// Import configuration from CSV format
bool Config::fromCSV(const std::string& csvStr) {
    clear();
    std::istringstream stream(csvStr);
    std::string line;
    size_t lineNumber = 0;
    
    while (std::getline(stream, line)) {
        ++lineNumber;
        if (lineNumber == 1) continue; // Skip header
        
        if (line.empty()) continue;
        
        std::string key, valueStr, sourceStr, description;
        size_t pos1 = 0, pos2 = 0, pos3 = 0;
        
        // Parse CSV line
        if ((pos1 = line.find(',')) != std::string::npos) {
            key = line.substr(0, pos1);
            
            if ((pos2 = line.find(',', pos1 + 1)) != std::string::npos) {
                valueStr = line.substr(pos1 + 1, pos2 - pos1 - 1);
                
                if ((pos3 = line.find(',', pos2 + 1)) != std::string::npos) {
                    sourceStr = line.substr(pos2 + 1, pos3 - pos2 - 1);
                    description = line.substr(pos3 + 1);
                } else {
                    sourceStr = line.substr(pos2 + 1);
                    description = "";
                }
            } else {
                sourceStr = line.substr(pos1 + 1);
                description = "";
            }
        }
        
        // Remove quotes from value
        if (!valueStr.empty() && valueStr.front() == '\"' && valueStr.back() == '\"') {
            valueStr = valueStr.substr(1, valueStr.size() - 2);
        }
        
        if (!description.empty() && description.front() == '\"' && description.back() == '\"') {
            description = description.substr(1, description.size() - 2);
        }
        
        // Parse source
        ConfigSource source = static_cast<ConfigSource>(std::stoi(sourceStr));
        
        // Parse value
        ConfigValue configValue;
        bool parsed = false;
        
        // Try to parse as int
        try {
            configValue = std::stoi(valueStr);
            parsed = true;
        } catch (...) {}
        
        // Try to parse as double
        if (!parsed) {
            try {
                configValue = std::stod(valueStr);
                parsed = true;
            } catch (...) {}
        }
        
        // Try to parse as bool
        if (!parsed) {
            if (toLower(valueStr) == "true") {
                configValue = true;
                parsed = true;
            } else if (toLower(valueStr) == "false") {
                configValue = false;
                parsed = true;
            }
        }
        
        // Default to string
        if (!parsed) {
            configValue = valueStr;
        }
        
        pImpl->entries.emplace_back(key, configValue, source, description);
    }
    
    return true;
}

} // namespace nlm

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

// Helper method to convert ConfigValue to JSON
#ifdef HAS_NLOHMANN_JSON
json Config::valueToJson(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << std::setprecision(6) << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << arg[i] << "\"";
            }
            oss << "]";
        } else {
            oss << arg;
        }
    }, value);
    return json::parse(oss.str());
}
#endif

// Helper method to convert ConfigValue to YAML string
#ifdef HAS_YAML_CPP
std::string Config::valueToYamlString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << std::setprecision(6) << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << arg[i] << "\"";
            }
            oss << "]";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}
#endif

// saveToJSON and saveToYAML methods
template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
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

} // namespace nlm
