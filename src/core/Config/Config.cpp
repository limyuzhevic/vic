#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <unordered_map>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/istreamwrapper.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Check file extension to determine format
    std::string ext = "";
    size_t dot_pos = filepath.find_last_of('.');
    if (dot_pos != std::string::npos) {
        ext = filepath.substr(dot_pos);
    }
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    // Try JSON/YAML format first, fallback to simple format
    if (ext == ".json") {
        return loadFromJson(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        return loadFromYaml(filepath);
    } else {
        return loadFromSimpleFormat(filepath);
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
        else if (arg[0] == '-') {
            std::string key = arg.substr(1);
            if (i + 1 < argc) {
                std::string value = argv[++i];
                set(key, value, ConfigSource::CommandLine);
            }
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
        file << entry.key << " = ";
        
        // Serialize based on value type
        std::visit([&file](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << val << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (val ? "true" : "false");
            } else {
                file << val;
            }
        }, entry.value);
        
        file << "\n\n";
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
    
    // Handle case where we want T but have a string value
    if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
        if (std::holds_alternative<std::string>(it->value)) {
            try {
                size_t pos;
                T result;
                if constexpr (std::is_same_v<T, int>) {
                    result = std::stoi(std::get<std::string>(it->value), pos);
                } else {
                    result = std::stoll(std::get<std::string>(it->value), pos);
                }
                // Check if entire string was consumed (no extra characters)
                if (pos == std::get<std::string>(it->value).size()) {
                    return result;
                }
            } catch (const std::exception&) {}
        }
    } else if constexpr (std::is_same_v<T, double>) {
        if (std::holds_alternative<std::string>(it->value)) {
            try {
                size_t pos;
                double result = std::stod(std::get<std::string>(it->value), pos);
                if (pos == std::get<std::string>(it->value).size()) {
                    return result;
                }
            } catch (const std::exception&) {}
        }
    } else if constexpr (std::is_same_v<T, bool>) {
        if (std::holds_alternative<std::string>(it->value)) {
            std::string val = std::get<std::string>(it->value);
            std::transform(val.begin(), val.end(), val.begin(), ::tolower);
            if (val == "true" || val == "1" || val == "yes" || val == "on") {
                return true;
            } else if (val == "false" || val == "0" || val == "no" || val == "off") {
                return false;
            }
        }
    }
    
    // Normal type matching
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
// Helper functions for string-to-value conversion
    std::string trim(const std::string& str);
    std::string toLower(const std::string& str);

// Load from various file formats
bool Config::loadFromSimpleFormat(const std::string& filepath);
bool Config::loadFromJson(const std::string& filepath);
bool Config::loadFromYaml(const std::string& filepath);

}} // namespace nlm

namespace nlm {

bool Config::loadFromSimpleFormat(const std::string& filepath) {
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
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::loadFromJson(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        rapidjson::IStreamWrapper isw(file);
        rapidjson::Document doc;
        doc.ParseStream(isw);
        
        if (!doc.IsObject()) {
            return false;
        }
        
        for (auto it = doc.MemberBegin(); it != doc.MemberEnd(); ++it) {
            std::string key = it->name.GetString();
            
            // Parse based on JSON type
            if (it->value.IsInt()) {
                set(key, it->value.GetInt(), ConfigSource::File);
            } else if (it->value.IsInt64()) {
                set(key, it->value.GetInt64(), ConfigSource::File);
            } else if (it->value.IsUint64()) {
                set(key, static_cast<int64_t>(it->value.GetUint64()), ConfigSource::File);
            } else if (it->value.IsDouble()) {
                set(key, it->value.GetDouble(), ConfigSource::File);
            } else if (it->value.IsBool()) {
                set(key, it->value.GetBool(), ConfigSource::File);
            } else if (it->value.IsString()) {
                set(key, it->value.GetString(), ConfigSource::File);
            } else if (it->value.IsArray()) {
                // Convert array to vector
                std::vector<double> vec;
                for (const auto& item : it->value.GetArray()) {
                    if (item.IsNumber()) {
                        vec.push_back(item.GetDouble());
                    }
                }
                // For now, store as vector<double> - could add vector<int> support
                set(key, ConfigValue(vec), ConfigSource::File);
            }
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::loadFromYaml(const std::string& filepath) {
    // Placeholder for YAML support - could use a YAML library
    // For now, treat as simple format
    return loadFromSimpleFormat(filepath);
}

}} // namespace nlm
