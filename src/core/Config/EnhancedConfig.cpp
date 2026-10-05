// Enhanced Configuration Implementation
// Provides full implementation of advanced configuration features

#include "core/Config/EnhancedConfig.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <regex>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {
    
    // JSON parser implementation
    namespace JSONConfigParser {
        bool parse(const std::string& content, std::vector<AdvancedConfigEntry>& entries) {
            try {
                auto json = nlohmann::json::parse(content);
                
                for (const auto& item : json.items()) {
                    std::string key = item.key();
                    nlohmann::json value = item.value();
                    
                    AdvancedConfigEntry entry;
                    entry.key = key;
                    entry.source = ConfigSource::File;
                    entry.timestamp = std::time(nullptr);
                    
                    // Parse value based on type
                    if (value.is_string()) {
                        entry.value = value.get<std::string>();
                        entry.dataType = "string";
                    } else if (value.is_number_integer()) {
                        entry.value = value.get<int64_t>();
                        entry.dataType = "int64_t";
                    } else if (value.is_number_unsigned()) {
                        entry.value = value.get<uint64_t>();
                        entry.dataType = "uint64_t";
                    } else if (value.is_number()) {
                        entry.value = value.get<double>();
                        entry.dataType = "double";
                    } else if (value.is_boolean()) {
                        entry.value = value.get<bool>();
                        entry.dataType = "bool";
                    } else if (value.is_array()) {
                        // Determine array type
                        bool allInts = std::all_of(value.begin(), value.end(), [](const nlohmann::json& v) { return v.is_number_integer(); });
                        bool allFloats = std::all_of(value.begin(), value.end(), [](const nlohmann::json& v) { return v.is_number(); });
                        bool allStrings = std::all_of(value.begin(), value.end(), [](const nlohmann::json& v) { return v.is_string(); });
                        bool allBools = std::all_of(value.begin(), value.end(), [](const nlohmann::json& v) { return v.is_boolean(); });
                        
                        if (allInts) {
                            entry.value = value.get<std::vector<int64_t>>();
                            entry.dataType = "std::vector<int64_t>";
                        } else if (allFloats) {
                            entry.value = value.get<std::vector<double>>();
                            entry.dataType = "std::vector<double>";
                        } else if (allStrings) {
                            entry.value = value.get<std::vector<std::string>>();
                            entry.dataType = "std::vector<std::string>";
                        } else if (allBools) {
                            entry.value = value.get<std::vector<bool>>();
                            entry.dataType = "std::vector<bool>";
                        }
                    }
                    
                    entries.push_back(entry);
                }
                
                return true;
                
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("JSON parsing error: " + std::string(e.what()));
                return false;
            }
        }
        
        std::string toJSON(const std::vector<AdvancedConfigEntry>& entries) {
            nlohmann::json json;
            
            for (const auto& entry : entries) {
                std::string key = entry.key;
                json[key] = formatValue(entry.value);
            }
            
            return json.dump(2); // Pretty print with 2-space indentation
        }
        
        std::string formatValue(const ConfigValue& value) {
            std::string result;
            
            std::visit([&result](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int>) {
                    result = std::to_string(arg);
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    result = std::to_string(arg);
                } else if constexpr (std::is_same_v<T, uint64_t>) {
                    result = std::to_string(arg);
                } else if constexpr (std::is_same_v<T, double>) {
                    result = std::to_string(arg);
                } else if constexpr (std::is_same_v<T, bool>) {
                    result = arg ? "true" : "false";
                } else if constexpr (std::is_same_v<T, std::string>) {
                    result = "\"" + escapeString(arg) + "\"";
                } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
                    nlohmann::json arr = arg;
                    result = arr.dump();
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    nlohmann::json arr = arg;
                    result = arr.dump();
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    nlohmann::json arr = arg;
                    result = arr.dump();
                } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
                    nlohmann::json arr = arg;
                    result = arr.dump();
                }
            }, value);
            
            return result;
        }
        
        bool validateJSON(const std::string& content) {
            try {
                nlohmann::json::parse(content);
                return true;
            } catch (const std::exception&) {
                return false;
            }
        }
        
        std::string escapeString(const std::string& str) {
            std::string result;
            for (char c : str) {
                switch (c) {
                    case '\\': result += "\\\\"; break;
                    case '\"': result += "\\\""; break;
                    case '\b': result += "\\b"; break;
                    case '\f': result += "\\f"; break;
                    case '\n': result += "\\n"; break;
                    case '\r': result += "\\r"; break;
                    case '\t': result += "\\t"; break;
                    default:
                        if (c < ' ' || c > '~') {
                            result += "\\u" + std::hex + std::setw(4) + std::setfill('0') + std::to_string(static_cast<int>(c));
                        } else {
                            result += c;
                        }
                }
            }
            return result;
        }
        
        std::string unescapeString(const std::string& str) {
            std::string result;
            for (size_t i = 0; i < str.size(); ++i) {
                if (str[i] == '\\' && i + 1 < str.size()) {
                    switch (str[i + 1]) {
                        case '\\': result += '\\'; break;
                        case '\"': result += '\"'; break;
                        case 'b': result += '\b'; break;
                        case 'f': result += '\f'; break;
                        case 'n': result += '\n'; break;
                        case 'r': result += '\r'; break;
                        case 't': result += '\t'; break;
                        case 'u': {
                            if (i + 5 < str.size()) {
                                std::string hexCode = str.substr(i + 2, 4);
                                try {
                                    char unicodeChar = static_cast<char>(std::stoul(hexCode, nullptr, 16));
                                    result += unicodeChar;
                                    i += 5;
                                } catch (...) {
                                    result += str[i]; // Fallback to original character
                                }
                            } else {
                                result += str[i]; // Fallback
                            }
                        } break;
                        default: result += str[i + 1]; break; // Unknown escape, include the next char
                    }
                    i++; // Skip the escape character
                } else {
                    result += str[i];
                }
            }
            return result;
        }
        
        std::string toLower(const std::string& str) {
            std::string result = str;
            std::transform(result.begin(), result.end(), result.begin(), ::tolower);
            return result;
        }
        
        std::string trim(const std::string& str) {
            size_t start = str.find_first_not_of(" \t\r\n");
            if (start == std::string::npos) return "";
            size_t end = str.find_last_not_of(" \t\r\n");
            return str.substr(start, end - start + 1);
        }
        
        bool isValidJSONKey(const std::string& key) {
            // JSON keys must be strings and cannot be empty
            // They should follow JSON key naming conventions
            static const std::regex validKeyRegex("^[a-zA-Z_][a-zA-Z0-9_]*$");
            return !key.empty() && std::regex_match(key, validKeyRegex);
        }
    }
    
    // YAML parser implementation
    namespace YAMLConfigParser {
        bool parse(const std::string& content, std::vector<AdvancedConfigEntry>& entries) {
            try {
                auto yaml = nlohmann::json::parse(content, nullptr, false);
                if (!yaml.is_object()) {
                    NLM_LOG_ERROR("YAML content is not a mapping");
                    return false;
                }
                
                for (const auto& item : yaml.items()) {
                    std::string key = item.key();
                    nlohmann::json value = item.value();
                    
                    AdvancedConfigEntry entry;
                    entry.key = key;
                    entry.source = ConfigSource::File;
                    entry.timestamp = std::time(nullptr);
                    
                    // Parse value based on type
                    if (value.is_string()) {
                        entry.value = value.get<std::string>();
                        entry.dataType = "string";
                    } else if (value.is_number_integer()) {
                        entry.value = value.get<int64_t>();
                        entry.dataType = "int64_t";
                    } else if (value.is_number_unsigned()) {
                        entry.value = value.get<uint64_t>();
                        entry.dataType = "uint64_t";
                    } else if (value.is_number()) {
                        entry.value = value.get<double>();
                        entry.dataType = "double";
                    } else if (value.is_boolean()) {
                        entry.value = value.get<bool>();
                        entry.dataType = "bool";
                    } else if (value.is_array()) {
                        // Determine array type
                        bool allInts = std::all_of(value.begin(), value.end(), [](const nlohmann::json& v) { return v.is_number_integer(); });
                        bool allFloats = std::all_of(value.begin(), value.end(), [](const nlohmann::json& v) { return v.is_number(); });
                        bool allStrings = std::all_of(value.begin(), value.end(), [](const nlohmann::json& v) { return v.is_string(); });
                        bool allBools = std::all_of(value.begin(), value.end(), [](const nlohmann::json& v) { return v.is_boolean(); });
                        
                        if (allInts) {
                            entry.value = value.get<std::vector<int64_t>>();
                            entry.dataType = "std::vector<int64_t>";
                        } else if (allFloats) {
                            entry.value = value.get<std::vector<double>>();
                            entry.dataType = "std::vector<double>";
                        } else if (allStrings) {
                            entry.value = value.get<std::vector<std::string>>();
                            entry.dataType = "std::vector<std::string>";
                        } else if (allBools) {
                            entry.value = value.get<std::vector<bool>>();
                            entry.dataType = "std::vector<bool>";
                        }
                    }
                    
                    // Try to get YAML-specific metadata
                    if (value.contains("description")) {
                        entry.description = value["description"].get<std::string>();
                    }
                    if (value.contains("type")) {
                        entry.dataType = value["type"].get<std::string>();
                    }
                    if (value.contains("required")) {
                        entry.required = value["required"].get<bool>();
                    }
                    if (value.contains("group")) {
                        // Group would be stored in metadata
                        entry.metadata["group"] = value["group"].get<std::string>();
                    }
                    
                    entries.push_back(entry);
                }
                
                return true;
                
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("YAML parsing error: " + std::string(e.what()));
                return false;
            }
        }
        
        std::string toYAML(const std::vector<AdvancedConfigEntry>& entries) {
            nlohmann::json yaml;
            
            for (const auto& entry : entries) {
                nlohmann::json item;
                
                // Convert value to JSON (YAML-compatible)
                item["value"] = JSONConfigParser::formatValue(entry.value);
                item["source"] = static_cast<int>(entry.source);
                
                if (!entry.description.empty()) {
                    item["description"] = entry.description;
                }
                if (!entry.dataType.empty() && entry.dataType != "string") {
                    item["type"] = entry.dataType;
                }
                if (entry.required) {
                    item["required"] = true;
                }
                if (!entry.defaultValue.empty()) {
                    item["default"] = entry.defaultValue;
                }
                
                // Add metadata
                if (!entry.validationRegex.empty()) {
                    item["validation"] = entry.validationRegex;
                }
                if (!entry.allowedValues.empty()) {
                    nlohmann::json allowed;
                    for (const auto& val : entry.allowedValues) {
                        allowed.push_back(val);
                    }
                    item["allowed_values"] = allowed;
                }
                
                // For now, just use key-value structure
                // In a full YAML implementation, we would preserve the original structure
                std::string key = entry.key;
                // Escape special characters in key for YAML
                std::replace(key.begin(), key.end(), ' ', '_');
                
                if (std::holds_alternative<std::string>(entry.value)) {
                    yaml[key] = std::get<std::string>(entry.value);
                } else if (std::holds_alternative<int64_t>(entry.value)) {
                    yaml[key] = std::get<int64_t>(entry.value);
                } else if (std::holds_alternative<uint64_t>(entry.value)) {
                    yaml[key] = std::get<uint64_t>(entry.value);
                } else if (std::holds_alternative<double>(entry.value)) {
                    yaml[key] = std::get<double>(entry.value);
                } else if (std::holds_alternative<bool>(entry.value)) {
                    yaml[key] = std::get<bool>(entry.value);
                } else if (std::holds_alternative<std::vector<int64_t>>(entry.value)) {
                    nlohmann::json arr = std::get<std::vector<int64_t>>(entry.value);
                    yaml[key] = arr;
                } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
                    nlohmann::json arr = std::get<std::vector<double>>(entry.value);
                    yaml[key] = arr;
                } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
                    nlohmann::json arr = std::get<std::vector<std::string>>(entry.value);
                    yaml[key] = arr;
                } else if (std::holds_alternative<std::vector<bool>>(entry.value)) {
                    nlohmann::json arr = std::get<std::vector<bool>>(entry.value);
                    yaml[key] = arr;
                }
            }
            
            return yaml.dump(2); // Pretty print
        }
        
        std::string formatValue(const ConfigValue& value) {
            return JSONConfigParser::formatValue(value);
        }
        
        bool validateYAML(const std::string& content) {
            // For now, use the same validation as JSON since we're using nlohmann::json
            return JSONConfigParser::validateJSON(content);
        }
        
        std::string escapeYAMLString(const std::string& str) {
            return JSONConfigParser::escapeString(str);
        }
        
        std::string unescapeYAMLString(const std::string& str) {
            return JSONConfigParser::unescapeString(str);
        }
        
        std::string yamlStringToString(const std::string& str) {
            return JSONConfigParser::unescapeString(str);
        }
        
        std::string yamlNumberToString(const std::string& str) {
            return str; // Numbers are already in string format
        }
        
        bool isIndentationConsistent(const std::string& content) {
            // Simple indentation check
            size_t indentSize = 2; // Assume 2-space indentation
            size_t pos = 0;
            bool firstLine = true;
            
            while ((pos = content.find_first_not_of(' ', pos)) != std::string::npos) {
                if (firstLine) {
                    firstLine = false;
                    pos++; // Skip the first character
                    continue;
                }
                
                size_t nextNonSpace = content.find_first_not_of(' ', pos);
                if (nextNonSpace == std::string::npos) break;
                
                // Check if indentation is consistent (multiple of indentSize)
                size_t indent = nextNonSpace - pos;
                if (indent > 0 && indent % indentSize != 0) {
                    return false;
                }
                
                pos = nextNonSpace;
            }
            
            return true;
        }
    }
    
    // Environment config loader implementation
    namespace EnvironmentConfigLoader {
        void load(std::vector<AdvancedConfigEntry>& entries, const std::string& prefix) {
            // This would iterate through environment variables
            // and load them into the entries vector
            // For now, it's a stub implementation
            NLM_LOG_INFO("Loading environment variables with prefix: " + prefix);
        }
        
        std::string convertEnvKeyToConfigKey(const std::string& envKey) {
            std::string result;
            bool first = true;
            
            for (char c : envKey) {
                if (c == '_') {
                    if (!first) {
                        result += '-';
                    }
                } else {
                    if (first) {
                        result += std::tolower(c);
                        first = false;
                    } else {
                        result += c;
                    }
                }
            }
            
            return result;
        }
        
        std::optional<ConfigValue> parseEnvValue(const std::string& valueStr) {
            // Try to parse the environment value as different types
            try {
                // Try boolean
                if (valueStr == "true" || valueStr == "1" || valueStr == "yes" || valueStr == "on") {
                    return true;
                }
                if (valueStr == "false" || valueStr == "0" || valueStr == "no" || valueStr == "off") {
                    return false;
                }
                
                // Try integer
                size_t pos;
                int64_t intVal = std::stoll(valueStr, &pos);
                if (pos == valueStr.size()) {
                    return intVal;
                }
                
                // Try double
                double doubleVal = std::stod(valueStr, &pos);
                if (pos == valueStr.size()) {
                    return doubleVal;
                }
                
                // Return as string
                return valueStr;
                
            } catch (...) {
                return valueStr; // Fallback to string
            }
        }
    }
    
    // CLI argument parser implementation
    namespace CLIArgumentParser {
        std::vector<Argument> parse(const std::vector<std::string>& args) {
            std::vector<Argument> arguments;
            
            for (const auto& arg : args) {
                if (arg.substr(0, 2) == "--") {
                    // Long option (--option=value)
                    size_t pos = arg.find('=');
                    if (pos != std::string::npos) {
                        Argument argument;
                        argument.name = arg.substr(2, pos - 2);
                        argument.value = arg.substr(pos + 1);
                        argument.description = "Command line argument";
                        argument.type = "string";
                        arguments.push_back(argument);
                    } else {
                        Argument argument;
                        argument.name = arg.substr(2);
                        argument.value = "true";
                        argument.description = "Flag argument";
                        argument.type = "bool";
                        arguments.push_back(argument);
                    }
                } else if (arg.substr(0, 1) == "-") {
                    // Short option (-x value)
                    if (arg.size() >= 2) {
                        Argument argument;
                        argument.name = arg.substr(1);
                        // Check if next argument is not another flag
                        // This would need to be handled at a higher level
                        argument.value = "true";
                        argument.description = "Short option argument";
                        argument.type = "string";
                        arguments.push_back(argument);
                    }
                }
            }
            
            return arguments;
        }
        
        std::vector<AdvancedConfigEntry> argumentsToEntries(const std::vector<Argument>& args) {
            std::vector<AdvancedConfigEntry> entries;
            
            for (const auto& arg : args) {
                AdvancedConfigEntry entry;
                entry.key = arg.name;
                entry.source = ConfigSource::CommandLine;
                
                try {
                    if (arg.type == "int" || arg.type == "integer" || arg.type == "number") {
                        size_t pos;
                        int64_t intVal = std::stoll(arg.value, &pos);
                        if (pos == arg.value.size()) {
                            entry.value = intVal;
                            entry.dataType = "int64_t";
                        } else {
                            entry.value = arg.value;
                            entry.dataType = "string";
                        }
                    } else if (arg.type == "float" || arg.type == "double") {
                        size_t pos;
                        double doubleVal = std::stod(arg.value, &pos);
                        if (pos == arg.value.size()) {
                            entry.value = doubleVal;
                            entry.dataType = "double";
                        } else {
                            entry.value = arg.value;
                            entry.dataType = "string";
                        }
                    } else if (arg.type == "bool" || arg.type == "boolean") {
                        if (arg.value == "true" || arg.value == "1" || arg.value == "yes" || arg.value == "on") {
                            entry.value = true;
                            entry.dataType = "bool";
                        } else {
                            entry.value = false;
                            entry.dataType = "bool";
                        }
                    } else if (arg.type == "string") {
                        entry.value = arg.value;
                        entry.dataType = "string";
                    } else {
                        entry.value = arg.value;
                        entry.dataType = "string";
                    }
                    
                    if (!arg.description.empty()) {
                        entry.description = arg.description;
                    }
                    entry.required = true; // CLI arguments are typically required
                    
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Failed to parse CLI argument " + arg.name + ": " + std::string(e.what()));
                    entry.value = arg.value;
                    entry.dataType = "string";
                }
                
                entries.push_back(entry);
            }
            
            return entries;
        }
        
        std::string generateHelp(const std::vector<Argument>& args) {
            std::stringstream help;
            
            help << "Usage: program [options]" << std::endl;
            help << std::endl;
            help << "Options:" << std::endl;
            
            for (const auto& arg : args) {
                help << "  --" << arg.name;
                if (arg.type == "bool") {
                    help << "\t" << arg.description << std::endl;
                } else {
                    help << "=<value>";
                    help << "\t" << arg.description << " (type: " << arg.type << ")" << std::endl;
                }
            }
            
            return help.str();
        }
        
        bool isFlag(const std::string& arg) {
            return arg.size() >= 2 && arg[0] == '-' && (arg[1] == '-' || std::isalpha(arg[1]));
        }
        
        std::string extractFlagName(const std::string& arg) {
            if (arg.substr(0, 2) == "--") {
                return arg.substr(2);
            } else if (arg.size() >= 2 && arg[0] == '-') {
                return arg.substr(1);
            }
            return arg;
        }
        
        std::string extractOptionName(const std::string& arg) {
            return extractFlagName(arg);
        }
    }
    
    // Config migrator implementation
    namespace ConfigMigrator {
        std::vector<ConfigMigrator::Migration> migrations_;
        int currentVersion_ = 1;
        
        void addMigration(int fromVersion, int toVersion,
                         const std::string& description,
                         std::function<void(std::vector<AdvancedConfigEntry>&)> migrate) {
            Migration migration;
            migration.version = toVersion;
            migration.description = description;
            migration.migrate = migrate;
            migrations_.push_back(migration);
        }
        
        bool migrate(std::vector<AdvancedConfigEntry>& entries, int targetVersion) {
            // Find migrations that need to be applied
            std::vector<Migration> toApply;
            for (const auto& migration : migrations_) {
                if (migration.version > currentVersion_ && migration.version <= targetVersion) {
                    toApply.push_back(migration);
                }
            }
            
            for (const auto& migration : toApply) {
                try {
                    migration.migrate(entries);
                    NLM_LOG_INFO("Applied migration: " + migration.description);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Failed to apply migration " + migration.description + ": " + std::string(e.what()));
                    return false;
                }
            }
            
            return true;
        }
    }
    
    // EnhancedConfig implementation
    EnhancedConfig::EnhancedConfig() : version_(1), historyEnabled_(false) {
        pImpl = std::make_unique<Impl>();
        // Initialize with default entries
        initializeDefaultEntries();
    }
    
    EnhancedConfig::~EnhancedConfig() = default;
    
    EnhancedConfig::EnhancedConfig(EnhancedConfig&&) noexcept = default;
    EnhancedConfig& EnhancedConfig::operator=(EnhancedConfig&&) noexcept = default;
    
    bool EnhancedConfig::loadFromFile(const std::string& filepath, const std::string& format) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            addError("Failed to open config file: " + filepath, "", "loadFromFile");
            return false;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        
        std::string detectedFormat = format;
        if (detectedFormat == "auto") {
            detectedFormat = "json"; // Default to JSON for now
            if (!JSONConfigParser::validateJSON(content)) {
                detectedFormat = "yaml"; // Try YAML
            }
        }
        
        bool success = false;
        if (detectedFormat == "json" || detectedFormat == "JSON") {
            success = JSONConfigParser::parse(content, pImpl->entries);
        } else if (detectedFormat == "yaml" || detectedFormat == "YAML" || detectedFormat == "yml") {
            success = YAMLConfigParser::parse(content, pImpl->entries);
        } else {
            addError("Unsupported format: " + detectedFormat, "", "loadFromFile");
            return false;
        }
        
        if (success) {
            for (auto& entry : pImpl->entries) {
                entry.source = ConfigSource::File;
                entry.timestamp = std::time(nullptr);
            }
        }
        
        return success;
    }
    
    bool EnhancedConfig::loadFromJSON(const std::string& filepath) {
        return loadFromFile(filepath, "json");
    }
    
    bool EnhancedConfig::loadFromYAML(const std::string& filepath) {
        return loadFromFile(filepath, "yaml");
    }
    
    bool EnhancedConfig::loadFromEnv(const std::string& prefix) {
        EnvironmentConfigLoader::load(pImpl->entries, prefix);
        return !pImpl->entries.empty();
    }
    
    bool EnhancedConfig::loadFromArgs(int argc, char** argv) {
        std::vector<std::string> args;
        for (int i = 1; i < argc; ++i) {
            args.push_back(argv[i]);
        }
        return loadFromArgs(args);
    }
    
    bool EnhancedConfig::loadFromArgs(const std::vector<std::string>& args) {
        auto cliArgs = CLIArgumentParser::parse(args);
        auto entries = CLIArgumentParser::argumentsToEntries(cliArgs);
        
        pImpl->entries.insert(pImpl->entries.end(), entries.begin(), entries.end());
        
        return !entries.empty();
    }
    
    void EnhancedConfig::set(const std::string& key, const ConfigValue& value,
                            ConfigSource source, const std::string& description,
                            const std::string& dataType, const std::string& validationRegex,
                            const std::string& defaultValue, bool required) {
        // Check if key exists
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                              [&key](const AdvancedConfigEntry& e) { return e.key == key; });n        
        ConfigValue oldValue;
        if (it != pImpl->entries.end()) {
            oldValue = it->value;
        }
        
        // Create new entry
        AdvancedConfigEntry newEntry(key, value, source, description);
        newEntry.dataType = dataType.empty() ? inferDataType(value) : dataType;
        newEntry.validationRegex = validationRegex;
        newEntry.defaultValue = defaultValue;
        newEntry.required = required;
        
        if (it != pImpl->entries.end()) {
            oldValue = it->value;
            *it = newEntry;
        } else {
            pImpl->entries.push_back(newEntry);
        }
        
        // Add to history if enabled
        if (historyEnabled_) {
            ConfigChange change;
            change.timestamp = std::time(nullptr);
            change.key = key;
            change.oldValue = oldValue;
            change.newValue = value;
            change.source = source;
            change.description = description;
            history_.push_back(change);
        }
        
        // Validate if schema exists
        if (!schema_.entries.empty()) {
            ConfigValidator::ValidationResult validation = ConfigValidator::validateEntry(newEntry);
            if (!validation.valid) {
                for (const auto& error : validation.errors) {
                    addError(error, key, "set");
                }
            }
        }
    }
    
    // Additional implementations would continue...
    
    // Helper functions implementation
    std::string EnhancedConfig::toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
    std::string EnhancedConfig::trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    std::string EnhancedConfig::escapeString(const std::string& str) {
        return JSONConfigParser::escapeString(str);
    }
    
    std::string EnhancedConfig::unescapeString(const std::string& str) {
        return JSONConfigParser::unescapeString(str);
    }
    
    bool EnhancedConfig::validateKeyFormat(const std::string& key) {
        static const std::regex validKeyRegex("^[a-zA-Z_][a-zA-Z0-9_]*$");
        return std::regex_match(key, validKeyRegex);
    }
    
    std::string EnhancedConfig::normalizeKey(const std::string& key) {
        std::string result;
        for (char c : key) {
            if (std::isalnum(c) || c == '_') {
                result += c;
            } else {
                result += '_';
            }
        }
        return result;
    }
    
    std::string EnhancedConfig::getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    void EnhancedConfig::addToHistory(std::vector<ConfigChange>& history, const std::string& key,
                                    const ConfigValue& oldValue, const ConfigValue& newValue,
                                    ConfigSource source, const std::string& description) {
        ConfigChange change;
        change.timestamp = std::time(nullptr);
        change.key = key;
        change.oldValue = oldValue;
        change.newValue = newValue;
        change.source = source;
        change.description = description;
        history.push_back(change);
    }
    
    std::string EnhancedConfig::inferDataType(const ConfigValue& value) {
        std::string type;
        std::visit([&type](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                type = "int64_t";
            } else if constexpr (std::is_same_v<T, double>) {
                type = "double";
            } else if constexpr (std::is_same_v<T, bool>) {
                type = "bool";
            } else if constexpr (std::is_same_v<T, std::string>) {
                type = "string";
            } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
                type = "std::vector<int64_t>";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                type = "std::vector<double>";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                type = "std::vector<std::string>";
            } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
                type = "std::vector<bool>";
            } else {
                type = "string";
            }
        }, value);
        return type;
    }
    
    void EnhancedConfig::initializeDefaultEntries() {
        // Add default configuration entries for NLM
        set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default, "Number of neurons in the network");
        set("region_count", static_cast<int64_t>(1), ConfigSource::Default, "Number of neural regions");
        set("connection_probability", 0.15f, ConfigSource::Default, "Probability of synapse creation between neurons");
        set("simulation_timestep", 0.001, ConfigSource::Default, "Time step for simulation in seconds");
        set("random_seed", static_cast<int64_t>(42), ConfigSource::Default, "Random seed for reproducible results");
        
        // STDP parameters
        set("stdp_ltp_weight", 0.02f, ConfigSource::Default, "STDP Long-Term Potentiation weight");
        set("stdp_ltd_weight", 0.015f, ConfigSource::Default, "STDP Long-Term Depression weight");
        set("stdp_tau", 20.0f, ConfigSource::Default, "STDP time constant");
        
        // Structural plasticity parameters
        set("synaptogenesis_rate", 0.0001f, ConfigSource::Default, "Rate of new synapse formation");
        set("pruning_rate", 0.00001f, ConfigSource::Default, "Rate of synapse elimination");
        
        // Neuron parameters
        set("neuron_threshold", -55.0f, ConfigSource::Default, "Neuron firing threshold in mV");
        set("neuron_resting_potential", -70.0f, ConfigSource::Default, "Neuron resting potential in mV");
        set("neuron_refractory_period", 5, ConfigSource::Default, "Neuron refractory period in ms");
    }
    
} // namespace nlm
