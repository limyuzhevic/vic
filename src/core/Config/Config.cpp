#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <json.hpp>
#include <yaml-cpp/yaml.h>

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

bool Config::loadFromFile(const std::string& filepath, ConfigFormat format) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    switch (format) {
        case ConfigFormat::JSON:
            return loadFromJSONFile(filepath);
        case ConfigFormat::YAML:
            return loadFromYAMLFile(filepath);
        case ConfigFormat::Auto:
            return loadFromAutoFormatFile(filepath);
        default:
            return false;
    }
}

bool Config::loadFromJSONFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json j;
        file >> j;
        
        // Parse JSON into configuration entries
        parseJSONToConfig(j, ConfigSource::File);
        
        return true;
    } catch (const std::exception& e) {
        // Fallback to simple key=value format if JSON parsing fails
        return loadFromFile(filepath);
    }
}

bool Config::loadFromYAMLFile(const std::string& filepath) {
    try {
        YAML::Loader loader;
        YAML::Node doc;
        
        if (filepath.size() > 5 && filepath.substr(filepath.size() - 5) == ".yaml") {
            loader.LoadFile(filepath, doc);
        } else {
            loader.Load(filepath, doc);
        }
        
        // Parse YAML into configuration entries
        parseYAMLToConfig(doc, ConfigSource::File);
        
        return true;
    } catch (const std::exception& e) {
        // Fallback to simple key=value format if YAML parsing fails
        return loadFromFile(filepath);
    }
}

bool Config::loadFromAutoFormatFile(const std::string& filepath) {
    // Determine file format by extension and content
    std::string extension = "." + std::filesystem::path(filepath).extension().string();
    
    if (extension == ".json" || extension == ".JSON") {
        return loadFromJSONFile(filepath);
    } else if (extension == ".yaml" || extension == ".yml" || extension == ".YAML" || extension == ".YML") {
        return loadFromYAMLFile(filepath);
    } else {
        // Try JSON first, then YAML, then fallback to simple format
        if (loadFromJSONFile(filepath)) {
            return true;
        } else if (loadFromYAMLFile(filepath)) {
            return true;
        } else {
            return loadFromFile(filepath);
        }
    }
}

void Config::parseJSONToConfig(const nlohmann::json& j, ConfigSource source) {
    if (j.is_object()) {
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            ConfigValue value;
            std::string description = "";
            
            // Add description if present in JSON
            if (it->contains("__description")) {
                description = it->value("__description", "");
            }
            
            // Convert JSON value to ConfigValue
            if (it->is_string()) {
                value = it->get<std::string>();
            } else if (it->is_number_integer()) {
                value = it->get<int64_t>();
            } else if (it->is_number_float()) {
                value = it->get<double>();
            } else if (it->is_boolean()) {
                value = it->get<bool>();
            } else if (it->is_array()) {
                // Handle array types
                const auto& arr = *it;
                if (!arr.empty()) {
                    // Determine array type from first element
                    if (arr[0].is_string()) {
                        std::vector<std::string> vec;
                        for (const auto& item : arr) {
                            vec.push_back(item.get<std::string>());
                        }
                        value = vec;
                    } else if (arr[0].is_number_integer()) {
                        std::vector<int> vec;
                        for (const auto& item : arr) {
                            vec.push_back(item.get<int>());
                        }
                        value = vec;
                    } else if (arr[0].is_number_float()) {
                        std::vector<double> vec;
                        for (const auto& item : arr) {
                            vec.push_back(item.get<double>());
                        }
                        value = vec;
                    }
                }
            }
            
            set(key, value, source);
            
            if (!description.empty()) {
                // Update the description
                auto it_entry = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                    [&key](const ConfigEntry& e) { return e.key == key; });
                if (it_entry != pImpl->entries.end()) {
                    it_entry->description = description;
                }
            }
        }
    }
}

void Config::parseYAMLToConfig(const YAML::Node& node, ConfigSource source) {
    if (node.IsMap()) {
        for (auto it = node.begin(); it != node.end(); ++it) {
            std::string key = it->first.as<std::string>();
            ConfigValue value;
            std::string description = "";
            
            // Add description if present in YAML (using special key)
            if (it->second.IsMap() && it->second["__description"]) {
                description = it->second["__description"].as<std::string>();
            }
            
            // Convert YAML value to ConfigValue
            if (it->second.IsScalar()) {
                const std::string& str_val = it->second.as<std::string>();
                // Try to determine type
                if (str_val == "true" || str_val == "false") {
                    value = str_val == "true";
                } else {
                    try {
                        size_t pos;
                        int int_val = std::stoi(str_val, &pos);
                        if (pos == str_val.size()) {
                            value = int_val;
                        } else {
                            double double_val = std::stod(str_val, &pos);
                            if (pos == str_val.size()) {
                                value = double_val;
                            } else {
                                value = str_val;
                            }
                        }
                    } catch (...) {
                        value = str_val;
                    }
                }
            } else if (it->second.IsSequence()) {
                // Handle sequence types
                const YAML::Node& seq = *it;
                if (!seq.empty()) {
                    // Determine sequence type from first element
                    if (seq[0].IsScalar()) {
                        const std::string& first_str = seq[0].as<std::string>();
                        if (first_str == "true" || first_str == "false") {
                            // Boolean array
                            std::vector<bool> vec;
                            for (const auto& item : seq) {
                                vec.push_back(item.as<bool>());
                            }
                            value = vec;
                        } else {
                            try {
                                size_t pos;
                                int int_val = std::stoi(first_str, &pos);
                                if (pos == first_str.size()) {
                                    std::vector<int> vec;
                                    for (const auto& item : seq) {
                                        vec.push_back(item.as<int>());
                                    }
                                    value = vec;
                                } else {
                                    double double_val = std::stod(first_str, &pos);
                                    if (pos == first_str.size()) {
                                        std::vector<double> vec;
                                        for (const auto& item : seq) {
                                            vec.push_back(item.as<double>());
                                        }
                                        value = vec;
                                    } else {
                                        std::vector<std::string> vec;
                                        for (const auto& item : seq) {
                                            vec.push_back(item.as<std::string>());
                                        }
                                        value = vec;
                                    }
                                }
                            } catch (...) {
                                std::vector<std::string> vec;
                                for (const auto& item : seq) {
                                    vec.push_back(item.as<std::string>());
                                }
                                value = vec;
                            }
                        }
                    }
                }
            }
            
            set(key, value, source);
            
            if (!description.empty()) {
                // Update the description
                auto it_entry = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                    [&key](const ConfigEntry& e) { return e.key == key; });
                if (it_entry != pImpl->entries.end()) {
                    it_entry->description = description;
                }
            }
        }
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
