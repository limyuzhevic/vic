#include "Config.hpp"
#include "core/Error/Error.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <system_error>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Helper functions for JSON/YAML parsing
namespace {

std::string getFileExtension(const std::string& filepath) {
    std::filesystem::path path(filepath);
    std::string extension = path.extension().string();
    if (!extension.empty()) {
        extension = toLower(extension);
    }
    return extension;
}

void flattenJson(const nlohmann::json& json, std::vector<ConfigEntry>& entries, const std::string& prefix = "", bool isNested = false) {
    for (auto it = json.begin(); it != json.end(); ++it) {
        std::string key = it.key();
        if (!prefix.empty()) {
            key = prefix + "." + key;
        }
        
        std::string description = "";
        
        if (it->is_object()) {
            flattenJson(*it, entries, key, true);
        } else if (it->is_array()) {
            // Convert array to vector<string> format
            std::vector<std::string> stringArray;
            for (const auto& item : *it) {
                if (item.is_string()) {
                    stringArray.push_back(item.get<std::string>());
                } else {
                    stringArray.push_back(item.dump());
                }
            }
            entries.emplace_back(key, stringArray, ConfigSource::Default, description);
        } else {
            // Convert JSON value to ConfigValue
            ConfigValue configValue;
            if (it->is_number_integer()) {
                configValue = it->get<int>();
            } else if (it->is_number_unsigned()) {
                configValue = static_cast<int64_t>(it->get<unsigned long long>());
            } else if (it->is_number_float()) {
                configValue = it->get<double>();
            } else if (it->is_boolean()) {
                configValue = it->get<bool>();
            } else if (it->is_string()) {
                configValue = it->get<std::string>();
            }
            
            entries.emplace_back(key, configValue, ConfigSource::Default, description);
        }
    }
}

void flattenYaml(const YAML::Node& node, std::vector<ConfigEntry>& entries, const std::string& prefix = "") {
    if (node.IsScalar()) {
        std::string key = prefix;
        std::string value;
        
        if (node.Tag().empty()) {
            if (node.IsInt()) {
                entries.emplace_back(key, node.as<int>(), ConfigSource::Default, "");
            } else if (node.IsFloat()) {
                entries.emplace_back(key, node.as<double>(), ConfigSource::Default, "");
            } else if (node.IsBool()) {
                entries.emplace_back(key, node.as<bool>(), ConfigSource::Default, "");
            } else {
                entries.emplace_back(key, node.as<std::string>(), ConfigSource::Default, "");
            }
        } else {
            value = node.as<std::string>();
            entries.emplace_back(key, value, ConfigSource::Default, "");
        }
    } else if (node.IsMap()) {
        for (const auto& element : node) {
            std::string newPrefix = prefix.empty() ? element.first.as<std::string>() : prefix + "." + element.first.as<std::string>();
            flattenYaml(element.second, entries, newPrefix);
        }
    } else if (node.IsSequence()) {
        std::vector<std::string> stringArray;
        for (const auto& item : node) {
            if (item.IsScalar()) {
                stringArray.push_back(item.as<std::string>());
            } else {
                stringArray.push_back(item.As<std::string>());
            }
        }
        entries.emplace_back(prefix, stringArray, ConfigSource::Default, "");
    }
}

nlohmann::json configValueToJson(const ConfigValue& value) {
    nlohmann::json json;
    
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            json = arg;
        }
    }, value);
    
    return json;
}

YAML::Node configValueToYaml(const ConfigValue& value) {
    YAML::Node node;
    
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            for (const auto& item : arg) {
                node.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            for (const auto& item : arg) {
                node.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            for (const auto& item : arg) {
                node.push_back(item);
            }
        }
    }, value);
    
    return node;
}

} // anonymous namespace

bool Config::loadFromFile(const std::string& filepath) {
    // Validate input parameter
    if (!ErrorUtils::validateNotNull(filepath.c_str(), "filepath")) {
        throw ConfigurationError("Configuration file path cannot be null");
    }
    
    if (filepath.empty()) {
        throw ConfigurationError("Configuration file path cannot be empty");
    }
    
    nlm::Logger::getGlobal().info("Loading configuration from file: " + filepath);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw nlm::FileError("Failed to open configuration file", filepath, nlm::FileOperation::Read,
                            "File does not exist or cannot be opened");
    }
    
    // Detect file format
    std::string extension = getFileExtension(filepath);
    
    // Clear existing entries
    clear();
    
    try {
        if (extension == ".json") {
            // Parse JSON file
            nlohmann::json jsonData;
            file >> jsonData;
            
            // Flatten JSON structure to key-value pairs
            flattenJson(jsonData, pImpl->entries);
            
        } else if (extension == ".yaml" || extension == ".yml") {
            // Parse YAML file
            YAML::Node yamlData = YAML::Load(file);
            
            // Flatten YAML structure to key-value pairs
            flattenYaml(yamlData, pImpl->entries);
            
        } else {
            // Parse as plain text key=value format (backward compatibility)
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
                        ((value.front() == '\"' && value.back() == '\"') ||
                         (value.front() == \'\' && value.back() == \'\'))) {
                        value = value.substr(1, value.size() - 2);
                    }
                    
                    set(key, value, ConfigSource::File);
                }
            }
        }
    } catch (const std::exception& e) {
        // Convert to nlm::FileError with meaningful message
        throw nlm::FileError(std::string("Error parsing configuration file: ") + e.what() + " (file: " + filepath + ")",
                            filepath, nlm::FileOperation::Read, "Configuration parsing failed");
    }
    
    nlm::Logger::getGlobal().info("Configuration loaded successfully from file: " + filepath + " (" + 
                                std::to_string(pImpl->entries.size()) + " entries)");
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
    // Validate input parameter
    if (!ErrorUtils::validateNotNull(filepath.c_str(), "filepath")) {
        throw ConfigurationError("Configuration file path cannot be null");
    }
    
    if (filepath.empty()) {
        throw ConfigurationError("Configuration file path cannot be empty");
    }
    
    nlm::Logger::getGlobal().info("Saving configuration to file: " + filepath);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw nlm::FileError("Failed to open configuration file for writing", filepath, nlm::FileOperation::Write,
                            "Cannot create or write to file");
    }
    
    // Detect output format based on file extension
    std::string extension = getFileExtension(filepath);
    
    try {
        if (extension == ".json") {
            // Save as JSON
            nlohmann::json jsonData;
            
            for (const auto& entry : pImpl->entries) {
                // Convert ConfigValue to nlohmann::json
                jsonData[entry.key] = configValueToJson(entry.value);
                
                // Add description as a special field if present
                if (!entry.description.empty()) {
                    jsonData["__descriptions"][entry.key] = entry.description;
                }
            }
            
            file << jsonData.dump(4); // Pretty print with 4 spaces
            
        } else if (extension == ".yaml" || extension == ".yml") {
            // Save as YAML
            YAML::Emitter yamlData;
            
            for (const auto& entry : pImpl->entries) {
                // Convert ConfigValue to YAML format
                yamlData << YAML::Key << entry.key;
                
                // Handle different ConfigValue types
                std::visit([&yamlData](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                        yamlData << arg;
                    } else if constexpr (std::is_same_v<T, double>) {
                        yamlData << arg;
                    } else if constexpr (std::is_same_v<T, bool>) {
                        yamlData << arg;
                    } else if constexpr (std::is_same_v<T, std::string>) {
                        yamlData << arg;
                    } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                        yamlData << YAML::BeginSeq;
                        for (const auto& item : arg) {
                            yamlData << item;
                        }
                        yamlData << YAML::EndSeq;
                    } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                        yamlData << YAML::BeginSeq;
                        for (const auto& item : arg) {
                            yamlData << item;
                        }
                        yamlData << YAML::EndSeq;
                    } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        yamlData << YAML::BeginSeq;
                        for (const auto& item : arg) {
                            yamlData << item;
                        }
                        yamlData << YAML::EndSeq;
                    }
                }, entry.value);
                
                // Add description as a comment (YAML doesn't have inline descriptions)
                if (!entry.description.empty()) {
                    yamlData << YAML::Comment(entry.description.c_str());
                }
            }
            
            std::stringstream output;
            output << yamlData;
            file << output.str();
            
        } else {
            // Default to plain text key=value format (backward compatibility)
            for (const auto& entry : pImpl->entries) {
                file << "# " << entry.description << "\n";
                file << entry.key << " = ";
                
                std::visit([&file](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, std::string>) {
                        file << "\"" << arg << "\"";
                    } else {
                        file << arg;
                    }
                }, entry.value);
                
                file << "\n\n";
            }
        }
        
        // Ensure file is flushed and closed properly
        file.flush();
        
    } catch (const std::exception& e) {
        // Close file and rethrow with context
        file.close();
        throw nlm::FileError(std::string("Error saving configuration file: ") + e.what() + " (file: " + filepath + ")",
                            filepath, nlm::FileOperation::Write, "Configuration save failed");
    }
    
    nlm::Logger::getGlobal().info("Configuration saved successfully to file: " + filepath);
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
