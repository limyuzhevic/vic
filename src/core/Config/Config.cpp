#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <unordered_map>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, ParserPtr> parsers;
    std::vector<std::unique_ptr<Schema>> schemas;
    ValidationCallback validationCallback;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Register default parsers
    pImpl->parsers[".json"] = std::make_unique<JsonParser>();
    pImpl->parsers[".yaml"] = std::make_unique<YamlParser>();
    pImpl->parsers[".yml"] = std::make_unique<YamlParser>();
    pImpl->parsers[".ini"] = std::make_unique<IniParser>();
    pImpl->parsers[".cfg"] = std::make_unique<IniParser>();
    pImpl->parsers[".conf"] = std::make_unique<IniParser>();
}

Config::~Config() = default;

Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool Config::loadFromFile(const std::string& filepath) {
    std::string extension = getFileExtension(filepath);
    auto it = pImpl->parsers.find(extension);
    if (it == pImpl->parsers.end()) {
        NLM_LOG_ERROR("Unsupported file format: " + extension + " for file: " + filepath);
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    return it->second->parse(buffer.str(), *this);
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
    std::string extension = getFileExtension(filepath);
    auto it = pImpl->parsers.find(extension);
    if (it == pImpl->parsers.end()) {
        NLM_LOG_ERROR("Unsupported file format for saving: " + extension + " for file: " + filepath);
        return false;
    }
    
    // Validate configuration before saving
    if (!validate()) {
        NLM_LOG_ERROR("Configuration validation failed - cannot save to file");
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file for writing: " + filepath);
        return false;
    }
    
    std::string content;
    if (it->second->serialize(*this, content)) {
        file << content;
        NLM_LOG_INFO("Configuration saved to: " + filepath);
        return true;
    } else {
        NLM_LOG_ERROR("Failed to serialize configuration to: " + filepath);
        return false;
    }
}

template<typename T>
void Config::set(const std::string& key, const T& value, ConfigSource source) {
    ConfigEntry entry;
    entry.key = key;
    entry.value = value;
    entry.source = source;
    entry.description = "";
    
    if (pImpl->validationCallback && !pImpl->validationCallback(entry)) {
        NLM_LOG_WARNING("Configuration validation failed for key: " + key);
    }
    
    pImpl->entries.push_back(std::move(entry));
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set<std::string>(key, value, source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set<int>(key, value, source);
}

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
    set<int64_t>(key, value, source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set<double>(key, value, source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set<bool>(key, value, source);
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

bool Config::validate() const {
    // Basic validation: check for null values
    for (const auto& entry : pImpl->entries) {
        if (entry.value.index() == 0 && std::get<std::string>(entry.value).empty()) {
            NLM_LOG_WARNING("Configuration value is empty for key: " + entry.key);
        }
    }
    
    // Run validation callback if set
    if (pImpl->validationCallback) {
        for (const auto& entry : pImpl->entries) {
            if (!pImpl->validationCallback(entry)) {
                return false;
            }
        }
    }
    
    return true;
}

void Config::setValidationCallback(const ValidationCallback& callback) {
    pImpl->validationCallback = callback;
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        ConfigEntry newEntry = entry;
        newEntry.source = source;
        
        // Check if key already exists
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });n        
        if (it != pImpl->entries.end()) {
            *it = std::move(newEntry);
        } else {
            pImpl->entries.push_back(std::move(newEntry));
        }
    }
}

size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

const ConfigEntry& Config::getEntry(size_t index) const {
    return pImpl->entries[index];
}

void Config::registerParser(const std::string& extension, ParserPtr parser) {
    pImpl->parsers[extension] = std::move(parser);
}

bool Config::loadFromFileAuto(const std::string& filepath) {
    std::string extension = getFileExtension(filepath);
    auto it = pImpl->parsers.find(extension);
    if (it == pImpl->parsers.end()) {
        NLM_LOG_ERROR("No parser available for file extension: " + extension);
        return false;
    }
    return it->second->parse(filepath, *this);
}

void Config::registerSchema(std::unique_ptr<Schema> schema) {
    pImpl->schemas.push_back(std::move(schema));
}

bool Config::validateSchema() const {
    for (const auto& schema : pImpl->schemas) {
        auto result = schema->validate(*this);
        if (!result.empty()) {
            NLM_LOG_ERROR("Schema validation failed for " + schema->getName() + ":");
            for (const auto& error : result) {
                NLM_LOG_ERROR("  - " + error);
            }
            return false;
        }
    }
    return true;
}

std::string Config::trim(const std::string& str) const {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) const {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string Config::getFileExtension(const std::string& filepath) const {
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos == std::string::npos) return "";
    return "." + filepath.substr(dotPos + 1);
}

// JsonParser implementation
bool Config::JsonParser::parse(const std::string& content, Config& config) {
    try {
        auto json = nlohmann::json::parse(content);
        
        for (auto& [key, value] : json.items()) {
            if (value.is_string()) {
                config.set<std::string>(key, value.get<std::string>(), ConfigSource::File);
            } else if (value.is_number_integer()) {
                config.set<int64_t>(key, value.get<int64_t>(), ConfigSource::File);
            } else if (value.is_number_float()) {
                config.set<double>(key, value.get<double>(), ConfigSource::File);
            } else if (value.is_boolean()) {
                config.set<bool>(key, value.get<bool>(), ConfigSource::File);
            } else if (value.is_object()) {
                // Convert object to string representation for complex values
                config.set<std::string>(key, value.dump(), ConfigSource::File);
            } else if (value.is_array()) {
                config.set<std::string>(key, value.dump(), ConfigSource::File);
            } else {
                NLM_LOG_WARNING("Skipping JSON value with unsupported type for key: " + key);
            }
        }
        
        NLM_LOG_INFO("Successfully parsed JSON configuration");
        return true;
        
    } catch (const nlohmann::json::parse_error& e) {
        NLM_LOG_ERROR("JSON parse error: " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing JSON configuration: " + std::string(e.what()));
        return false;
    }
}

bool Config::JsonParser::serialize(const Config& config, std::string& output) const {
    try {
        nlohmann::json json;
        
        for (const auto& entry : config.pImpl->entries) {
            std::visit([&json, &entry](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    json[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, int>) {
                    json[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    json[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    json[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    json[entry.key] = arg;
                }
            }, entry.value);
        }
        
        output = json.dump(2); // Pretty print with 2-space indentation
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error serializing JSON configuration: " + std::string(e.what()));
        return false;
    }
}

// YamlParser implementation
bool Config::YamlParser::parse(const std::string& content, Config& config) {
    try {
        auto yaml = YAML::Load(content);
        
        for (const auto& it : yaml) {
            std::string key = it.first.as<std::string>();
            const YAML::Node& value = it.second;
            
            if (value.IsScalar()) {
                std::string scalar = value.as<std::string>();
                if (value.IsBoolean()) {
                    config.set<bool>(key, value.as<bool>(), ConfigSource::File);
                } else if (value.IsInteger()) {
                    config.set<int64_t>(key, value.as<int64_t>(), ConfigSource::File);
                } else if (value.IsFloat()) {
                    config.set<double>(key, value.as<double>(), ConfigSource::File);
                } else {
                    config.set<std::string>(key, scalar, ConfigSource::File);
                }
            } else if (value.IsSequence()) {
                // Convert array to string representation
                std::stringstream ss;
                ss << value;
                config.set<std::string>(key, ss.str(), ConfigSource::File);
            } else if (value.IsMap()) {
                // Convert map to string representation
                std::stringstream ss;
                ss << value;
                config.set<std::string>(key, ss.str(), ConfigSource::File);
            }
        }
        
        NLM_LOG_INFO("Successfully parsed YAML configuration");
        return true;
        
    } catch (const YAML::Exception& e) {
        NLM_LOG_ERROR("YAML parse error: " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing YAML configuration: " + std::string(e.what()));
        return false;
    }
}

bool Config::YamlParser::serialize(const Config& config, std::string& output) const {
    try {
        YAML::Emitter emitter;
        emitter << YAML::BeginDoc;
        
        for (const auto& entry : config.pImpl->entries) {
            emitter << YAML::Key << entry.key;
            std::visit([&emitter](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    emitter << YAML::Value << arg;
                } else if constexpr (std::is_same_v<T, int>) {
                    emitter << YAML::Value << arg;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    emitter << YAML::Value << arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    emitter << YAML::Value << arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    emitter << YAML::Value << arg;
                }
            }, entry.value);
        }
        
        emitter << YAML::EndDoc;
        output = emitter.c_str();
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error serializing YAML configuration: " + std::string(e.what()));
        return false;
    }
}

// IniParser implementation
bool Config::IniParser::parse(const std::string& content, Config& config) {
    std::istringstream stream(content);
    std::string line;
    std::string currentSection;
    
    while (std::getline(stream, line)) {
        line = config.trim(line);
        
        if (line.empty() || line[0] == ';' || line[0] == '#') {
            continue;
        }
        
        if (line[0] == '[' && line[line.size() - 1] == ']') {
            currentSection = line.substr(1, line.size() - 2);
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string& key = line.substr(0, pos);
            size_t valuePos = key.find_last_not_of(" \t");
            key = key.substr(0, valuePos + 1);
            
            std::string value = line.substr(pos + 1);
            size_t valueStart = value.find_first_not_of(" \t");
            if (valueStart != std::string::npos) {
                value = value.substr(valueStart);
            }
            
            if (!currentSection.empty()) {
                key = currentSection + "." + key;
            }
            
            config.set<std::string>(key, value, ConfigSource::File);
        }
    }
    
    NLM_LOG_INFO("Successfully parsed INI configuration");
    return true;
}

bool Config::IniParser::serialize(const Config& config, std::string& output) const {
    std::ostringstream oss;
    
    std::string currentSection;
    for (const auto& entry : config.pImpl->entries) {
        size_t dotPos = entry.key.find('.');
        if (dotPos != std::string::npos) {
            std::string section = entry.key.substr(0, dotPos);
            std::string key = entry.key.substr(dotPos + 1);
            
            if (section != currentSection) {
                if (!currentSection.empty()) {
                    oss << "\n";
                }
                oss << "[" << section << "]";
                currentSection = section;
            }
            oss << "\n  " << key << " = " << entry.key;
        } else {
            if (!currentSection.empty()) {
                oss << "\n";
            }
            oss << entry.key << " = " << entry.key;
            currentSection = "";
        }
    }
    
    output = oss.str();
    return true;
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
