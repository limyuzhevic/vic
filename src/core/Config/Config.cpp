#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <rapidjson/document.h>
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
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to parse as JSON first
    if (tryParseJSON(file, filepath)) {
        return true;
    }
    
    // Try to parse as YAML if JSON failed
    file.clear();
    file.seekg(0);
    if (tryParseYAML(file, filepath)) {
        return true;
    }
    
    // Fall back to simple key=value format
    file.clear();
    file.seekg(0);
    return loadFromSimpleFormat(file, filepath);
}

bool Config::tryParseJSON(std::ifstream& file, const std::string& filepath) {
    try {
        rapidjson::IStreamWrapper wrapper(file);
        rapidjson::Document doc;
        doc.ParseStream(wrapper);
        
        if (doc.HasParseError() || !doc.IsObject()) {
            return false;
        }
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Recursively parse JSON into config entries
        parseJSONObject(doc.GetObject(), ConfigSource::File);
        return true;
        
    } catch (...) {
        return false;
    }
}

bool Config::tryParseYAML(std::ifstream& file, const std::string& filepath) {
    try {
        // TODO: Implement YAML parsing with a YAML library
        // For now, just return false to fall back to simple format
        return false;
    } catch (...) {
        return false;
    }
}

void Config::parseJSONObject(const rapidjson::Value& obj, ConfigSource source) {
    for (auto it = obj.MemberBegin(); it != obj.MemberEnd(); ++it) {
        const std::string& key = it->name.GetString();
        
        if (it->value.IsString()) {
            set(key, it->value.GetString(), source);
        } else if (it->value.IsInt()) {
            set(key, it->value.GetInt64(), source);
        } else if (it->value.IsUint()) {
            set(key, it->value.GetUint64(), source);
        } else if (it->value.IsInt64()) {
            set(key, it->value.GetInt64(), source);
        } else if (it->value.IsUint64()) {
            set(key, it->value.GetUint64(), source);
        } else if (it->value.IsDouble()) {
            set(key, it->value.GetDouble(), source);
        } else if (it->value.IsFloat()) {
            set(key, it->value.GetFloat(), source);
        } else if (it->value.IsBool()) {
            set(key, it->value.GetBool(), source);
        } else if (it->value.IsArray()) {
            // Handle arrays as JSON format
            std::ostringstream oss;
            oss << "[";
            bool first = true;
            for (const auto& element : it->value.GetArray()) {
                if (!first) oss << ",";
                if (element.IsString()) {
                    oss << "\"" << element.GetString() << "\"";
                } else {
                    oss << element;
                }
                first = false;
            }
            oss << "]";
            set(key, oss.str(), source);
        } else if (it->value.IsObject()) {
            // Nested objects - convert to string representation
            std::ostringstream oss;
            oss << "{";
            bool first = true;
            for (auto it2 = it->value.GetObject().MemberBegin(); 
                 it2 != it->value.GetObject().MemberEnd(); ++it2) {
                if (!first) oss << ", ";
                oss << "\"" << it2->name.GetString() << "\": ";
                if (it2->value.IsString()) {
                    oss << "\"" << it2->value.GetString() << "\"";
                } else {
                    oss << it2->value;
                }
                first = false;
            }
            oss << "}";
            set(key, oss.str(), source);
        }
    }
}

bool Config::loadFromSimpleFormat(std::ifstream& file, const std::string& filepath) {
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
template int64_t Config::getOr<int64_t>(const std::string&, const int6464_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm