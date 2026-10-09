#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <map>
#include <regex>
#include <ctime>
#include <iomanip>
#include <cctype>
#include <cmath>
#include <unordered_map>
#include <set>

namespace nlm {

// Implementation of ConfigSchema methods
void ConfigSchema::addRule(const ValidationRule& rule) {
    rules_.push_back(rule);
    errors_.clear();
}

bool ConfigSchema::validate(const ConfigEntry& entry) const {
    bool valid = true;
    for (const auto& rule : rules_) {
        if (rule.fieldName == entry.key) {
            if (!rule.validator(entry.value)) {
                errors_.push_back("Validation failed for '" + entry.key + "': " + rule.errorMessage);
                valid = false;
            }
        }
    }
    return valid;
}

bool ConfigSchema::validateAll(const std::vector<ConfigEntry>& entries) const {
    errors_.clear();
    bool valid = true;
    for (const auto& entry : entries) {
        if (!validate(entry)) {
            valid = false;
        }
    }
    
    std::set<std::string> presentKeys;
    for (const auto& entry : entries) {
        presentKeys.insert(entry.key);
    }
    
    for (const auto& rule : rules_) {
        if (rule.required && presentKeys.find(rule.fieldName) == presentKeys.end()) {
            errors_.push_back("Missing required field: '" + rule.fieldName + "'");
            valid = false;
        }
    }
    
    return valid;
}

std::vector<std::string> ConfigSchema::getErrors() const {
    return errors_;
}

// Implementation of ConfigValidator methods
ConfigValidator::ConfigValidator(const ConfigSchema& schema) : schema_(schema) {}

bool ConfigValidator::validate(const ConfigEntry& entry) const {
    return schema_.validate(entry);
}

bool ConfigValidator::validateAll(const std::vector<ConfigEntry>& entries) const {
    return schema_.validateAll(entries);
}

std::vector<std::string> ConfigValidator::getErrors() const {
    return schema_.getErrors();
}

// Implementation of ConfigFormatDetector
ConfigFormat ConfigFormatDetector::detectFromExtension(const std::string& filepath) {
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json" || ext == ".jsonc" || ext == ".json5") {
        return ConfigFormat::JSON;
    } else if (ext == ".yaml" || ext == ".yml") {
        return ConfigFormat::YAML;
    } else if (ext == ".cfg" || ext == ".ini" || ext == ".properties") {
        return ConfigFormat::SimpleKeyValue;
    }
    
    return ConfigFormat::Unknown;
}

ConfigFormat ConfigFormatDetector::detectFromContent(const std::string& content) {
    std::string trimmed = trim(content);
    if (trimmed.empty()) return ConfigFormat::Unknown;
    
    if (trimmed.front() == '{' && trimmed.back() == '}') {
        return ConfigFormat::JSON;
    } else if (trimmed.find("---") != std::string::npos || 
               trimmed.find("key: value") != std::string::npos ||
               trimmed.find(": ") != std::string::npos) {
        return ConfigFormat::YAML;
    } else if (trimmed.find("=") != std::string::npos) {
        return ConfigFormat::SimpleKeyValue;
    }
    
    return ConfigFormat::Unknown;
}

std::string ConfigFormatDetector::formatToString(ConfigFormat format) {
    switch (format) {
        case ConfigFormat::Unknown: return "unknown";
        case ConfigFormat::SimpleKeyValue: return "key=value";
        case ConfigFormat::JSON: return "JSON";
        case ConfigFormat::YAML: return "YAML";
        default: return "unknown";
    }
}

// Implementation of Config class methods
struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    ConfigFormat format = detectFormat(filepath);
    return loadFromFile(filepath, format);
}

bool Config::loadFromFile(const std::string& filepath, ConfigFormat format) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    
    return loadFromString(content, format);
}

bool Config::loadFromString(const std::string& content, ConfigFormat format) {
    clear();
    validationErrors_.clear();
    
    std::unique_ptr<ConfigParser> parser = createParser(format);
    if (!parser) {
        return false;
    }
    
    return parser->parse(content, pImpl->entries);
}

bool Config::saveToFile(const std::string& filepath, ConfigFormat format) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (format == ConfigFormat::Unknown) {
        format = detectFormat(filepath);
    }
    
    switch (format) {
        case ConfigFormat::SimpleKeyValue:
            for (const auto& entry : pImpl->entries) {
                file << "# " << entry.description << "\n";
                file << entry.key << " = " << configValueToStringHelper(entry.value) << "\n";
                file << "\n";
            }
            break;
            
        case ConfigFormat::JSON:
            file << "{\n";
            for (size_t i = 0; i < pImpl->entries.size(); ++i) {
                const auto& entry = pImpl->entries[i];
                file << "  \"" << entry.key << "\": " << jsonValueToJSON(entry.value);
                if (i < pImpl->entries.size() - 1) file << ",\n";
                else file << "\n";
            }
            file << "}\n";
            break;
            
        case ConfigFormat::YAML:
            file << "# NLM Configuration File\n";
            file << "# Generated from configuration data\n\n";
            for (const auto& entry : pImpl->entries) {
                file << "# " << entry.description << "\n";
                file << entry.key << ": " << yamlValueToYAML(entry.value) << "\n\n";
            }
            break;
            
        default:
            return false;
    }
    
    return true;
}

std::string Config::jsonValueToJSON(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return "\"" + arg + "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            std::ostringstream oss;
            oss << std::setprecision(10) << arg;
            return oss.str();
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
                oss << std::setprecision(10) << arg[i];
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
            return "\"unknown\"";
        }
    }, value);
}

std::string Config::yamlValueToYAML(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return "\"" + arg + "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            std::ostringstream oss;
            oss << std::setprecision(10) << arg;
            return oss.str();
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            std::ostringstream oss;
            oss << "[\n";
            for (size_t i = 0; i < arg.size(); ++i) {
                oss << "  " << arg[i];
                if (i < arg.size() - 1) oss << ",";
                oss << "\n";
            }
            oss << "]";
            return oss.str();
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            std::ostringstream oss;
            oss << "[\n";
            for (size_t i = 0; i < arg.size(); ++i) {
                oss << "  " << std::setprecision(10) << arg[i];
                if (i < arg.size() - 1) oss << ",";
                oss << "\n";
            }
            oss << "]";
            return oss.str();
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            std::ostringstream oss;
            oss << "[\n";
            for (size_t i = 0; i < arg.size(); ++i) {
                oss << "  \"" << arg[i] << "\"";
                if (i < arg.size() - 1) oss << ",";
                oss << "\n";
            }
            oss << "]";
            return oss.str();
        } else {
            return "null";
        }
    }, value);
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
        it->timestamp = std::chrono::system_clock::now();
    } else {
        ConfigEntry entry(key, value, source);
        pImpl->entries.push_back(entry);
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

std::vector<int> Config::getIntList(const std::string& key) const {
    auto val = get<std::vector<int>>(key);
    return val.has_value() ? val.value() : std::vector<int>();
}

std::vector<double> Config::getDoubleList(const std::string& key) const {
    auto val = get<std::vector<double>>(key);
    return val.has_value() ? val.value() : std::vector<double>();
}

std::vector<std::string> Config::getStringList(const std::string& key) const {
    auto val = get<std::vector<std::string>>(key);
    return val.has_value() ? val.value() : std::vector<std::string>();
}

std::vector<std::vector<int>> Config::getIntMatrix(const std::string& key) const {
    auto val = get<std::vector<std::vector<int>>>(key);
    return val.has_value() ? val.value() : std::vector<std::vector<int>>();
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
    validationErrors_.clear();
}

bool Config::setSchema(const ConfigSchema& schema) {
    schema_ = std::make_unique<ConfigSchema>(schema);
    return validate();
}

bool Config::validate() const {
    if (!schema_) {
        return true;
    }
    
    validationErrors_ = schema_->getErrors();
    return validationErrors_.empty();
}

std::vector<std::string> Config::getValidationErrors() const {
    return validationErrors_;
}

ConfigFormat Config::detectFormat(const std::string& filepath) {
    return ConfigFormatDetector::detectFromExtension(filepath);
}

ConfigFormat Config::detectFormatFromContent(const std::string& content) {
    return ConfigFormatDetector::detectFromContent(content);
}

std::unique_ptr<ConfigParser> Config::createParser(ConfigFormat format) {
    switch (format) {
        case ConfigFormat::SimpleKeyValue:
            return nullptr;
            
        case ConfigFormat::JSON:
            return std::make_unique<SimpleJSONParser>();
            
        case ConfigFormat::YAML:
            return std::make_unique<SimpleYAMLParser>();
            
        default:
            return nullptr;
    }
}

std::vector<ConfigFormat> Config::getSupportedFormats() {
    std::vector<ConfigFormat> formats;
    formats.push_back(ConfigFormat::SimpleKeyValue);
    formats.push_back(ConfigFormat::JSON);
    formats.push_back(ConfigFormat::YAML);
    return formats;
}

bool Config::loadFromAdvancedArgs(int argc, char** argv) {
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        } else if (arg.size() >= 2 && arg[0] == '-' && arg[1] == '-') {
            if (i + 1 < argc && argv[i+1][0] != '-') {
                std::string key = arg.substr(2);
                std::string value = argv[++i];
                set(key, value, ConfigSource::CommandLine);
            }
        } else if (arg.substr(0, 7) == "--file=") {
            std::string filepath = arg.substr(7);
            ConfigFormat format = detectFormat(filepath);
            if (!loadFromFile(filepath, format)) {
                success = false;
            }
        } else if (arg == "--json" && i + 1 < argc) {
            std::string jsonContent = argv[++i];
            loadFromString(jsonContent, ConfigFormat::JSON);
        } else if (arg == "--yaml" && i + 1 < argc) {
            std::string yamlContent = argv[++i];
            loadFromString(yamlContent, ConfigFormat::YAML);
        } else if (arg == "--validate") {
            if (schema_) {
                validate();
            }
        } else if (arg == "--help") {
        } else if (arg == "--version") {
        }
    }
    
    return success;
}

std::string Config::getHelpText() const {
    std::ostringstream oss;
    oss << "Configuration Options:\n";
    oss << "  --key=value          Set configuration value\n";
    oss << "  --file=path          Load configuration from file (auto-detects format)\n";
    oss << "  --json               Next argument is JSON configuration content\n";
    oss << "  --yaml               Next argument is YAML configuration content\n";
    oss << "  --validate           Validate configuration against schema\n";
    oss << "  --help               Show this help message\n";
    oss << "  --version            Show version information\n";
    oss << "\nSupported formats: key=value (.cfg, .ini), JSON (.json), YAML (.yaml, .yml)\n";
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

namespace {
    std::string configValueToStringInternal(const ConfigValue& value) {
        return std::visit([](auto&& arg) -> std::string {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return "\"" + arg + "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                return arg ? "true" : "false";
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                return std::to_string(arg);
            } else if constexpr (std::is_same_v<T, double>) {
                std::ostringstream oss;
                oss << arg;
                return oss.str();
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
                    oss << arg[i];
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
            } else if constexpr (std::is_same_v<T, std::vector<std::vector<int>>>) {
                std::ostringstream oss;
                oss << "[[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    for (size_t j = 0; j < arg[i].size(); ++j) {
                        if (i == 0 && j == 0) oss << arg[i][j];
                        else oss << ", " << arg[i][j];
                    }
                }
                oss << "]]";
                return oss.str();
            } else {
                return "";
            }
        }, value);
    }
}

// SimpleJSONParser implementation
class SimpleJSONParser : public ConfigParser {
public:
    bool parse(const std::string& content, std::vector<ConfigEntry>& entries) override {
        std::istringstream stream(content);
        std::string line;
        bool inObject = false;
        bool inString = false;
        std::string currentKey;
        std::string jsonValue;
        char quoteChar = 0;
        
        while (std::getline(stream, line)) {
            for (size_t i = 0; i < line.size(); ++i) {
                char c = line[i];
                
                if (!inString && c == '{') {
                    inObject = true;
                    continue;
                } else if (!inString && c == '}') {
                    inObject = false;
                    continue;
                } else if (!inString && c == '"') {
                    inString = true;
                    quoteChar = c;
                    continue;
                } else if (inString && c == quoteChar && (i == 0 || line[i-1] != '\\')) {
                    inString = false;
                    quoteChar = 0;
                    
                    if (currentKey.empty()) {
                        currentKey = jsonValue;
                    } else {
                        ConfigEntry entry;
                        entry.key = currentKey;
                        entry.value = parseJSONValue(jsonValue);
                        entry.source = ConfigSource::File;
                        entries.push_back(entry);
                        currentKey.clear();
                    }
                    jsonValue.clear();
                    continue;
                } else if (inString) {
                    jsonValue += c;
                }
            }
        }
        return true;
    }
    
    std::string getFormatName() const override { return "JSON"; }
    std::vector<std::string> getSupportedExtensions() const override { 
        return {".json", ".jsonc", ".json5"}; 
    }
    
private:
    ConfigValue parseJSONValue(const std::string& value) {
        std::string val = trim(value);
        
        if (val.empty()) return std::string();
        
        if (val == "true") return true;
        if (val == "false") return false;
        
        if (std::all_of(val.begin(), val.end(), ::isdigit)) {
            return std::stoi(val);
        }
        
        try {
            size_t pos;
            double d = std::stod(val, &pos);
            if (pos == val.size()) return d;
        } catch (...) {}
        
        if (val.front() == '[' && val.back() == ']') {
            return parseJSONArray(val.substr(1, val.size() - 2));
        }
        
        if ((val.front() == '"' && val.back() == '"') || 
            (val.front() == '\'' && val.back() == '\'')) {
            return val.substr(1, val.size() - 2);
        }
        
        return val;
    }
    
    std::vector<int> parseJSONArray(const std::string& content) {
        std::vector<int> result;
        std::istringstream stream(content);
        std::string item;
        
        while (std::getline(stream, item, ',')) {
            item = trim(item);
            if (!item.empty()) {
                try {
                    result.push_back(std::stoi(item));
                } catch (...) {
                    result.push_back(0);
                }
            }
        }
        return result;
    }
};

// SimpleYAMLParser implementation
class SimpleYAMLParser : public ConfigParser {
public:
    bool parse(const std::string& content, std::vector<ConfigEntry>& entries) override {
        std::istringstream stream(content);
        std::string line;
        bool inQuotes = false;
        char quoteChar = 0;
        
        while (std::getline(stream, line)) {
            line = trim(line);
            if (line.empty()) continue;
            
            size_t commentPos = line.find('#');
            if (commentPos != std::string::npos) {
                line = line.substr(0, commentPos);
                line = trim(line);
                if (line.empty()) continue;
            }
            
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string key = trim(line.substr(0, colonPos));
                std::string rest = trim(line.substr(colonPos + 1));
                
                if (!rest.empty()) {
                    std::string value = rest;
                    for (size_t i = 0; i < value.size(); ++i) {
                        char c = value[i];
                        if (!inQuotes && (c == '\"' || c == '\'')) {
                            inQuotes = true;
                            quoteChar = c;
                        } else if (inQuotes && c == quoteChar) {
                            inQuotes = false;
                            quoteChar = 0;
                        }
                        // Handle escape sequences
                        if (c == '\\' && i + 1 < value.size()) {
                            value.erase(i, 1);
                        }
                    }
                    ConfigEntry entry;
                    entry.key = key;
                    entry.value = value;
                    entry.source = ConfigSource::File;
                    entries.push_back(entry);
                }
            }
        }
        
        return true;
    }
    
    std::string getFormatName() const override { return "YAML"; }
    std::vector<std::string> getSupportedExtensions() const override { 
        return {".yaml", ".yml"}; 
    }
};

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
