#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cstring>
#include <cctype>

// nlohmann_json
#include <nlohmann/json.hpp>

// yaml-cpp
#include <yaml-cpp/yaml.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Load from file (JSON/YAML format detected by extension)
bool Config::loadFromFile(const std::string& filepath) {
    std::string format = getFormat(filepath);
    std::string content = readFile(filepath);
    return loadFromString(content, format);
}

// Load from string (JSON/YAML format)
bool Config::loadFromString(const std::string& content, const std::string& format) {
    if (format == "json") {
        return parseJSON(nlohmann::json::parse(content), ConfigSource::File);
    } else if (format == "yaml" || format == "yml") {
        return parseYAML(YAML::Load(content), ConfigSource::File);
    } else {
        parseSimpleFormat(content, ConfigSource::File);
        return true;
    }
}

// Load from command line arguments
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

// Save to file (JSON format by default, YAML if .yaml extension)
bool Config::saveToFile(const std::string& filepath, const std::string& format) const {
    std::string content;
    if (format == "json") {
        nlohmann::json j = toJSON();
        content = toString(j);
    } else if (format == "yaml" || format == "yml") {
        YAML::Node node = toYAML();
        content = toString(node);
    } else {
        // Fallback to simple format
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    file << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        file << arg[i];
                        if (i < arg.size() - 1) file << ", ";
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    file << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        file << arg[i];
                        if (i < arg.size() - 1) file << ", ";
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    file << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        file << "\"" << arg[i] << "\"";
                        if (i < arg.size() - 1) file << ", ";
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
                    file << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        file << (arg[i] ? "true" : "false");
                        if (i < arg.size() - 1) file << ", ";
                    }
                    file << "]";
                } else {
                    file << arg;
                }
            }, entry.value);
            file << "\n";
        }
        return true;
    }
    
    // Write to file
    return writeFile(filepath, content);
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

// Complex type setters
void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<bool>& value, ConfigSource source) {
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
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    oss << arg[i];
                    if (i < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    oss << arg[i];
                    if (i < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    oss << "\"" << arg[i] << "\"";
                    if (i < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    oss << (arg[i] ? "true" : "false");
                    if (i < arg.size() - 1) oss << ", ";
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

// Type conversion helpers
template<typename T>
bool Config::convertFromJSON(const nlohmann::json& j, T& out) {
    try {
        out = j.get<T>();
        return true;
    } catch (...) {
        return false;
    }
}

template<typename T>
bool Config::convertFromYAML(const YAML::Node& node, T& out) {
    try {
        out = node.as<T>();
        return true;
    } catch (...) {
        return false;
    }
}

template<typename T>
nlohmann::json Config::convertToJSON(const T& value) {
    return value;
}

template<typename T>
YAML::Node Config::convertToYAML(const T& value) {
    return value;
}

// Internal helpers
static std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

static std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

static std::string getFormat(const std::string& filepath) {
    std::string ext = "." + std::string(std::filesystem::path(filepath).extension().string());
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") return "json";
    if (ext == ".yaml" || ext == ".yml") return "yaml";
    return "simple";
}

static bool detectFormat(const std::string& content, std::string& format) {
    if (content.empty()) return false;
    
    format.clear();
    if (content.find('{') != std::string::npos || content.find('[') != std::string::npos) {
        format = "json";
    } else if (content.find(':') != std::string::npos) {
        format = "yaml";
    } else {
        format = "simple";
    }
    return true;
}

// JSON/YAML parsing helpers
void Config::parseJSON(const nlohmann::json& j, ConfigSource source) {
    if (j.is_object()) {
        for (const auto& item : j.items()) {
            std::string key = item.key();
            ConfigValue value;
            
            if (item.value().is_string()) {
                value = item.value().get<std::string>();
            } else if (item.value().is_number_integer()) {
                value = item.value().get<int64_t>();
            } else if (item.value().is_number_float()) {
                value = item.value().get<double>();
            } else if (item.value().is_boolean()) {
                value = item.value().get<bool>();
            } else if (item.value().is_array()) {
                // Determine array type
                const auto& arr = item.value();
                if (!arr.empty()) {
                    if (arr[0].is_number_integer()) {
                        std::vector<int> vec;
                        vec.reserve(arr.size());
                        for (const auto& elem : arr) {
                            vec.push_back(elem.get<int>());
                        }
                        value = vec;
                    } else if (arr[0].is_number_float()) {
                        std::vector<double> vec;
                        vec.reserve(arr.size());
                        for (const auto& elem : arr) {
                            vec.push_back(elem.get<double>());
                        }
                        value = vec;
                    } else if (arr[0].is_string()) {
                        std::vector<std::string> vec;
                        vec.reserve(arr.size());
                        for (const auto& elem : arr) {
                            vec.push_back(elem.get<std::string>());
                        }
                        value = vec;
                    } else if (arr[0].is_boolean()) {
                        std::vector<bool> vec;
                        vec.reserve(arr.size());
                        for (const auto& elem : arr) {
                            vec.push_back(elem.get<bool>());
                        }
                        value = vec;
                    }
                }
            }
            
            set(key, value, source);
        }
    }
}

void Config::parseYAML(const YAML::Node& node, ConfigSource source) {
    if (node.IsMap()) {
        for (const auto& item : node) {
            std::string key = item.first.as<std::string>();
            ConfigValue value;
            
            if (item.second.IsScalar()) {
                const std::string& val = item.second.as<std::string>();
                if (val == "true" || val == "false") {
                    value = item.second.as<bool>();
                } else if (val.find('.') != std::string::npos) {
                    value = item.second.as<double>();
                } else {
                    // Try to parse as integer
                    try {
                        value = item.second.as<int64_t>();
                    } catch (...) {
                        value = val;
                    }
                }
            } else if (item.second.IsSequence()) {
                // Determine array type
                const YAML::Node& seq = item.second;
                if (!seq.empty()) {
                    if (seq[0].IsScalar()) {
                        const std::string& val = seq[0].as<std::string>();
                        if (val == "true" || val == "false") {
                            std::vector<bool> vec;
                            vec.reserve(seq.size());
                            for (const auto& elem : seq) {
                                vec.push_back(elem.as<bool>());
                            }
                            value = vec;
                        } else if (val.find('.') != std::string::npos) {
                            std::vector<double> vec;
                            vec.reserve(seq.size());
                            for (const auto& elem : seq) {
                                vec.push_back(elem.as<double>());
                            }
                            value = vec;
                        } else {
                            std::vector<std::string> vec;
                            vec.reserve(seq.size());
                            for (const auto& elem : seq) {
                                vec.push_back(elem.as<std::string>());
                            }
                            value = vec;
                        }
                    } else if (seq[0].IsInteger()) {
                        std::vector<int> vec;
                        vec.reserve(seq.size());
                        for (const auto& elem : seq) {
                            vec.push_back(elem.as<int>());
                        }
                        value = vec;
                    } else if (seq[0].IsFloat()) {
                        std::vector<double> vec;
                        vec.reserve(seq.size());
                        for (const auto& elem : seq) {
                            vec.push_back(elem.as<double>());
                        }
                        value = vec;
                    }
                }
            }
            
            set(key, value, source);
        }
    }
}

void Config::parseSimpleFormat(const std::string& content, ConfigSource source) {
    std::istringstream stream(content);
    std::string line;
    while (std::getline(stream, line)) {
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
            
            set(key, value, source);
        }
    }
}

// File I/O helpers
std::string Config::readFile(const std::string& filepath) const {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return "";
    }
    
    std::ostringstream oss;
    oss << file.rdbuf();
    return oss.str();
}

bool Config::writeFile(const std::string& filepath, const std::string& content) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    file << content;
    return true;
}

// Serialization helpers
nlohmann::json Config::toJSON() const {
    nlohmann::json j = nlohmann::json::object();
    for (const auto& entry : pImpl->entries) {
        std::visit([&j, &entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::string>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
                j[entry.key] = arg;
            }
        }, entry.value);
    }
    return j;
}

YAML::Node Config::toYAML() const {
    YAML::Node node = YAML::Node::Map();
    for (const auto& entry : pImpl->entries) {
        std::visit([&node, &entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                node[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                node[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                node[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                node[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::string>) {
                node[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                node[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                node[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                node[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
                node[entry.key] = arg;
            }
        }, entry.value);
    }
    return node;
}

std::string Config::toString(const nlohmann::json& j) const {
    return j.dump(2);
}

std::string Config::toString(const YAML::Node& node) const {
    std::stringstream stream;
    stream << node;
    return stream.str();
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

template bool Config::convertFromJSON<int>(const nlohmann::json&, int&) const;
template bool Config::convertFromJSON<double>(const nlohmann::json&, double&) const;
template bool Config::convertFromJSON<std::string>(const nlohmann::json&, std::string&) const;
template bool Config::convertFromJSON<bool>(const nlohmann::json&, bool&) const;

template nlohmann::json Config::convertToJSON<int>(const int&);
template nlohmann::json Config::convertToJSON<double>(const double&);
template nlohmann::json Config::convertToJSON<std::string>(const std::string&);
template nlohmann::json Config::convertToJSON<bool>(const bool&);

// Complex type template instantiations
template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string&) const;
template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;
template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;
template std::optional<std::vector<bool>> Config::get<std::vector<bool>>(const std::string&) const;

template std::vector<int> Config::getOr<std::vector<int>>(const std::string&, const std::vector<int>&) const;
template std::vector<double> Config::getOr<std::vector<double>>(const std::string&, const std::vector<double>&) const;
template std::vector<std::string> Config::getOr<std::vector<std::string>>(const std::string&, const std::vector<std::string>&) const;
template std::vector<bool> Config::getOr<std::vector<bool>>(const std::string&, const std::vector<bool>&) const;