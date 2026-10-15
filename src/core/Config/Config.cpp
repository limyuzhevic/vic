#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>  // For JSON parsing
#include <ryml.hpp>             // For YAML parsing
#include <ryml_std.hpp>         // For YAML standard types

using json = nlohmann::json;
namespace ryml = ryml;

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Enhanced loadFromFile with JSON/YAML support
bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Determine file format based on extension
    std::string extension = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        extension = filepath.substr(dotPos + 1);
    }
    
    bool success = false;
    
    if (extension == "json" || extension == "JSON") {
        success = loadFromJSON(file);
    } else if (extension == "yaml" || extension == "yml" || extension == "YAML" || extension == "YML") {
        // Read entire file for YAML parsing
        file.clear();
        file.seekg(0, std::ios::beg);
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        success = loadFromYAML(content);
    } else {
        // Default to simple key=value format for backward compatibility
        file.clear();
        file.seekg(0, std::ios::beg);
        success = loadFromSimple(file);
    }
    
    return success;
}

bool Config::loadFromJSON(std::ifstream& file) {
    try {
        json data = json::parse(file);
        pImpl->entries.clear();
        
        // Convert JSON to Config entries
        convertJSONToConfig(data, ConfigSource::File);
        return true;
    } catch (const json::parse_error& e) {
        NLM_LOG_ERROR("JSON parse error: " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromYAML(const std::string& content) {
    try {
        auto tree = ryml::parse_in_place(content);
        pImpl->entries.clear();
        
        // Convert YAML to Config entries
        convertYAMLToConfig(tree, ConfigSource::File);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("YAML parse error: " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromSimple(std::ifstream& file) {
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

void Config::convertJSONToConfig(const json& data, ConfigSource source) {
    if (data.is_object()) {
        for (auto it = data.begin(); it != data.end(); ++it) {
            std::string key = it.key();
            const json& value = it.value();
            setFromJSONValue(key, value, source);
        }
    } else {
        NLM_LOG_WARNING("JSON root is not an object, skipping");
    }
}

void Config::convertYAMLToConfig(ryml::Tree& tree, ConfigSource source) {
    if (tree.rootref().is_map()) {
        auto node = tree.rootref();
        for (auto it = node.begin(); it != node.end(); ++it) {
            std::string key = ryml::to_cstr(it->key());
            setFromYAMLValue(key, *it, source);
        }
    } else {
        NLM_LOG_WARNING("YAML root is not a map, skipping");
    }
}

void Config::setFromJSONValue(const std::string& key, const json& value, ConfigSource source) {
    if (value.is_string()) {
        set(key, value.get<std::string>(), source);
    } else if (value.is_number_integer()) {
        set(key, value.get<int64_t>(), source);
    } else if (value.is_number_float()) {
        set(key, value.get<double>(), source);
    } else if (value.is_boolean()) {
        set(key, value.get<bool>(), source);
    } else if (value.is_array()) {
        // Handle arrays
        std::vector<double> doubleArray;
        for (const auto& item : value) {
            if (item.is_number()) {
                doubleArray.push_back(item.get<double>());
            }
        }
        // For now, store arrays as strings. Could enhance later.
        set(key, "ARRAY_NOT_FULLY_SUPPORTED", source);
    } else {
        // null or other types
        set(key, "NULL_OR_UNSUPPORTED", source);
    }
}

void Config::setFromYAMLValue(const std::string& key, const ryml::ConstNodeRef& node, ConfigSource source) {
    if (ryml::is_scalar(node)) {
        std::string value = ryml::to_cstr(node);
        // Try to determine type
        if (value == "true" || value == "false") {
            set(key, value == "true", source);
        } else {
            try {
                // Try to parse as integer
                size_t pos;
                int64_t intVal = std::stoll(value, &pos);
                if (pos == value.size()) {
                    set(key, intVal, source);
                    return;
                }
            } catch (...) {}
            
            try {
                // Try to parse as float
                size_t pos;
                double floatVal = std::stod(value, &pos);
                if (pos == value.size()) {
                    set(key, floatVal, source);
                    return;
                }
            } catch (...) {}
            
            // Default to string
            set(key, value, source);
        }
    } else if (ryml::is_seq(node)) {
        // Handle sequences (arrays)
        std::vector<double> doubleArray;
        for (auto it = node.begin(); it != node.end(); ++it) {
            if (ryml::is_scalar(*it)) {
                std::string value = ryml::to_cstr(*it);
                try {
                    double val = std::stod(value);
                    doubleArray.push_back(val);
                } catch (...) {}
            }
        }
        // For now, store arrays as strings. Could enhance later.
        set(key, "ARRAY_NOT_FULLY_SUPPORTED", source);
    } else {
        NLM_LOG_WARNING("Unsupported YAML node type for key: " + key);
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
