#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <map>
#include <set>
#include <unordered_map>
#include <chrono>
#include <memory>
#include <variant>
#include <sstream>
#include <iomanip>

namespace nlm {

// Simple JSON-like value types using variant
using JSONValue = std::variant<
    std::nullptr_t,
    bool,
    int,
    int64_t,
    double,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>
>;

class Config::Impl {
public:
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, JSONValue> cachedJSON;
    bool jsonLoaded;
    std::string configPath;
    std::chrono::system_clock::time_point lastModified;

    Impl() : jsonLoaded(false), lastModified(std::chrono::system_clock::now()) {}

    bool isFileModified(const std::string& filepath) {
        try {
            auto ftime = std::filesystem::last_write_time(filepath);
            auto ftime_duration = std::chrono::duration_cast<std::chrono::seconds>(
                ftime.time_since_epoch()
            ).count();
            auto current_duration = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()
            ).count();
            return ftime_duration != lastModified.time_since_epoch().count();
        } catch (const std::filesystem::filesystem_error&) {
            return true;
        }
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromJSON(const std::string& filepath) {
    auto errorContext = NLM_CURRENT_ERROR_CONTEXT;
    
    if (!std::filesystem::exists(filepath)) {
        NLM_THROW_CONFIG_ERROR(ErrorCode::ConfigFileNotFound, "Config file not found: " + filepath);
        return false;
    }

    auto fileTime = std::filesystem::last_write_time(filepath);
    if (!pImpl->isFileModified(filepath) && pImpl->cachedJSON.find(filepath) != pImpl->cachedJSON.end()) {
        return true;
    }

    nlohmann::json jsonData;
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_THROW_CONFIG_ERROR(ErrorCode::FileIOError, "Failed to open config file: " + filepath);
            return false;
        }
        file >> jsonData;
    } catch (const std::exception& e) {
        NLM_THROW_CONFIG_ERROR(ErrorCode::ConfigParseError, "Failed to parse JSON config file " + filepath + ": " + e.what());
        return false;
    }

    if (!jsonData.is_object()) {
        NLM_THROW_CONFIG_ERROR(ErrorCode::InvalidArgument, "Config file is not a valid JSON object: " + filepath);
        return false;
    }

    pImpl->entries.clear();
    pImpl->cachedJSON[filepath] = jsonData;

    for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
        std::string key = it.key();
        ConfigValue configValue;

        if (it->is_null()) {
            configValue = std::string("null");
        } else if (it->is_boolean()) {
            configValue = it->get<bool>();
        } else if (it->is_number_integer()) {
            configValue = it->get<int>();
        } else if (it->is_number_unsigned()) {
            configValue = static_cast<int64_t>(it->get<uint64_t>());
        } else if (it->is_number_float()) {
            configValue = it->get<double>();
        } else if (it->is_string()) {
            configValue = substituteEnvironmentVariables(it->get<std::string>());
        } else if (it->is_array()) {
            nlohmann::json array = *it;
            if (array.empty()) {
                configValue = std::vector<std::string>{};
            } else {
                bool allInt = std::all_of(array.begin(), array.end(), 
                    [](const nlohmann::json& val) { return val.is_number_integer(); });
                bool allString = std::all_of(array.begin(), array.end(), 
                    [](const nlohmann::json& val) { return val.is_string(); });

                if (allInt) {
                    std::vector<int> result;
                    for (const auto& val : array) {
                        result.push_back(val.get<int>());
                    }
                    configValue = result;
                } else if (allString) {
                    std::vector<std::string> result;
                    for (const auto& val : array) {
                        result.push_back(val.get<std::string>());
                    }
                    configValue = result;
                } else {
                    configValue = array.dump();
                }
            }
        } else {
            configValue = it->dump();
        }

        pImpl->entries.emplace_back(key, configValue, ConfigSource::File);
    }

    pImpl->configPath = filepath;
    pImpl->lastModified = fileTime;
    pImpl->jsonLoaded = true;

    return true;
}

bool Config::mergeFromFile(const std::string& filepath) {
    if (!std::filesystem::exists(filepath)) {
        return false;
    }

    nlohmann::json jsonData;
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        file >> jsonData;
    } catch (...) {
        return false;
    }

    if (!jsonData.is_object()) {
        return false;
    }

    for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
        std::string key = it.key();
        ConfigValue configValue;

        if (it->is_null()) {
            configValue = std::string("null");
        } else if (it->is_boolean()) {
            configValue = it->get<bool>();
        } else if (it->is_number_integer()) {
            configValue = it->get<int>();
        } else if (it->is_number_unsigned()) {
            configValue = static_cast<int64_t>(it->get<uint64_t>());
        } else if (it->is_number_float()) {
            configValue = it->get<double>();
        } else if (it->is_string()) {
            configValue = substituteEnvironmentVariables(it->get<std::string>());
        } else if (it->is_array()) {
            nlohmann::json array = *it;
            if (array.empty()) {
                configValue = std::vector<std::string>{};
            } else {
                bool allInt = std::all_of(array.begin(), array.end(), 
                    [](const nlohmann::json& val) { return val.is_number_integer(); });
                bool allString = std::all_of(array.begin(), array.end(), 
                    [](const nlohmann::json& val) { return val.is_string(); });

                if (allInt) {
                    std::vector<int> result;
                    for (const auto& val : array) {
                        result.push_back(val.get<int>());
                    }
                    configValue = result;
                } else if (allString) {
                    std::vector<std::string> result;
                    for (const auto& val : array) {
                        result.push_back(val.get<std::string>());
                    }
                    configValue = result;
                } else {
                    configValue = array.dump();
                }
            }
        } else {
            configValue = it->dump();
        }

        auto entryIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });

        if (entryIt != pImpl->entries.end()) {
            entryIt->value = configValue;
            entryIt->source = ConfigSource::File;
        } else {
            pImpl->entries.emplace_back(key, configValue, ConfigSource::File);
        }
    }

    if (std::filesystem::exists(filepath)) {
        auto fileTime = std::filesystem::last_write_time(filepath);
        pImpl->lastModified = fileTime;
    }

    return true;
}

bool Config::loadFromFile(const std::string& filepath) {
    try {
        if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".json") {
            return loadFromJSON(filepath);
        }

        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_THROW_CONFIG_ERROR(ErrorCode::FileIOError, "Failed to open config file: " + filepath);
            return false;
        }

        pImpl->entries.clear();
        pImpl->jsonLoaded = false;
        pImpl->cachedJSON.clear();

        std::string line;
        int lineNum = 0;
        while (std::getline(file, line)) {
            lineNum++;
            line = trim(line);

            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }

            size_t pos = line.find('=');
            if (pos == std::string::npos) {
                std::string trimmed = trim(line);
                if (trimmed.empty() || trimmed[0] == '{' || trimmed[0] == '[') {
                    try {
                        nlohmann::json jsonValue;
                        std::istringstream iss(trimmed);
                        iss >> jsonValue;
                        if (jsonValue.is_object()) {
                            for (auto it = jsonValue.begin(); it != jsonValue.end(); ++it) {
                                std::string key = it.key();
                                ConfigValue configValue;

                                if (it->is_null()) {
                                    configValue = std::string("null");
                                } else if (it->is_boolean()) {
                                    configValue = it->get<bool>();
                                } else if (it->is_number_integer()) {
                                    configValue = it->get<int>();
                                } else if (it->is_number_unsigned()) {
                                    configValue = static_cast<int64_t>(it->get<uint64_t>());
                                } else if (it->is_number_float()) {
                                    configValue = it->get<double>();
                                } else if (it->is_string()) {
                                    configValue = substituteEnvironmentVariables(it->get<std::string>());
                                } else if (it->is_array()) {
                                    nlohmann::json array = *it;
                                    if (array.empty()) {
                                        configValue = std::vector<std::string>{};
                                    } else {
                                        bool allInt = std::all_of(array.begin(), array.end(), 
                                            [](const nlohmann::json& val) { return val.is_number_integer(); });
                                        bool allString = std::all_of(array.begin(), array.end(), 
                                            [](const nlohmann::json& val) { return val.is_string(); });

                                        if (allInt) {
                                            std::vector<int> result;
                                            for (const auto& val : array) {
                                                result.push_back(val.get<int>());
                                            }
                                            configValue = result;
                                        } else if (allString) {
                                            std::vector<std::string> result;
                                            for (const auto& val : array) {
                                                result.push_back(val.get<std::string>());
                                            }
                                            configValue = result;
                                        } else {
                                            configValue = array.dump();
                                        }
                                    }
                                } else {
                                    configValue = it->dump();
                                }

                                set(key, configValue, ConfigSource::File);
                            }
                        }
                    } catch (...) {
                        pos = line.find('=');
                        if (pos != std::string::npos) {
                            std::string key = trim(line.substr(0, pos));
                            std::string value = trim(line.substr(pos + 1));

                            if (value.size() >= 2 && 
                                ((value.front() == '"' && value.back() == '"') ||
                                 (value.front() == '\'' && value.back() == '\''))) {
                                value = value.substr(1, value.size() - 2);
                            }

                            value = substituteEnvironmentVariables(value);
                            set(key, value, ConfigSource::File);
                        }
                        continue;
                    }
                    continue;
                }
            }

            if (pos == std::string::npos) {
                throw std::runtime_error("Invalid config line " + std::to_string(lineNum) + " in " + filepath + ": no '=' found");
            }

            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));

            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }

            value = substituteEnvironmentVariables(value);
            set(key, value, ConfigSource::File);
        }

        pImpl->configPath = filepath;
        return true;
    } catch (const NLMException& e) {
        nlm::ErrorHandling::handleException(e);
        return false;
    } catch (const std::exception& e) {
        ErrorContext context(__FILE__, __func__, __LINE__);
        NLMException genericEx(ErrorCategory::Configuration, ErrorCode::ConfigParseError, 
                               ErrorSeverity::Error, std::string("Failed to load config file: ") + e.what(), context);
        nlm::ErrorHandling::handleException(genericEx);
        return false;
    }
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
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }

        nlohmann::json jsonData = exportJSON();
        file << jsonData.dump(2);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

void Config::validate(const std::string& schemaFile) {
    auto errorContext = NLM_CURRENT_ERROR_CONTEXT;
    
    if (!std::filesystem::exists(schemaFile)) {
        NLM_THROW_CONFIG_ERROR(ErrorCode::ConfigFileNotFound, "Schema file not found: " + schemaFile);
    }

    std::ifstream schemaFileStream(schemaFile);
    if (!schemaFileStream.is_open()) {
        NLM_THROW_CONFIG_ERROR(ErrorCode::FileIOError, "Failed to open schema file: " + schemaFile);
    }

    nlohmann::json schema;
    try {
        schemaFileStream >> schema;
    } catch (const nlohmann::json::parse_error& e) {
        NLM_THROW_CONFIG_ERROR(ErrorCode::ConfigParseError, "Invalid JSON in schema file: " + schemaFile + ": " + e.what());
    }

    if (!schema.is_object()) {
        NLM_THROW_CONFIG_ERROR(ErrorCode::InvalidArgument, "Schema must be a JSON object");
    }

    if (schema.contains("required") && schema["required"].is_array()) {
        for (const auto& field : schema["required"]) {
            if (!field.is_string()) {
                NLM_THROW_CONFIG_ERROR(ErrorCode::InvalidArgument, "Required field must be a string");
            }
            std::string fieldName = field.get<std::string>();
            if (!has(fieldName)) {
                NLM_THROW_CONFIG_ERROR(ErrorCode::RequiredFieldMissing, "Missing required configuration field: " + fieldName);
            }
        }
    }

    if (schema.contains("properties") && schema["properties"].is_object()) {
        for (auto it = schema["properties"].begin(); it != schema["properties"].end(); ++it) {
            std::string fieldName = it.key();
            if (!has(fieldName)) {
                continue;
            }

            nlohmann::json fieldSchema = it.value();
            if (fieldSchema.is_object() && fieldSchema.contains("type")) {
                std::string expectedType = fieldSchema["type"].get<std::string>();
                
                auto entryIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                    [&fieldName](const ConfigEntry& e) { return e.key == fieldName; });
                if (entryIt == pImpl->entries.end()) {
                    continue;
                }

                bool typeMatches = false;
                
                std::visit([&](const auto& value) {
                    using T = std::decay_t<decltype(value)>;
                    if (expectedType == "string" && std::is_same_v<T, std::string>) {
                        typeMatches = true;
                    } else if (expectedType == "integer" && std::is_same_v<T, int>) {
                        typeMatches = true;
                    } else if (expectedType == "number" && (std::is_same_v<T, double> || std::is_same_v<T, int64_t>)) {
                        typeMatches = true;
                    } else if (expectedType == "boolean" && std::is_same_v<T, bool>) {
                        typeMatches = true;
                    } else if (expectedType == "array" && (std::is_same_v<T, std::vector<int>> || 
                                                         std::is_same_v<T, std::vector<double>> || 
                                                         std::is_same_v<T, std::vector<std::string>>)) {
                        typeMatches = true;
                    }
                }, entryIt->value);

                if (!typeMatches && !fieldSchema.value("optional", false)) {
                    NLM_THROW_CONFIG_ERROR(ErrorCode::ConfigValidationFailed, "Type mismatch for field: " + fieldName);
                }
            }
        }
    }
}

std::string Config::exportJSON() const {
    nlohmann::json jsonData = nlohmann::json::object();

    for (const auto& entry : pImpl->entries) {
        std::visit([&jsonData, &entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
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
            } else {
                jsonData[entry.key] = entry.key + "=" + std::to_string(arg);
            }
        }, entry.value);
    }

    return jsonData.dump(2);
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
// Note: Template instantiations moved to Config.cpp implementation

} // namespace nlm
