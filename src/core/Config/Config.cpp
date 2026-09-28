#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::string ext = ".";
    size_t dotPos = filepath.rfind('.');
    if (dotPos != std::string::npos && dotPos < filepath.length() - 1) {
        ext = "." + filepath.substr(dotPos + 1);
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    clear();
    
    if (ext == ".json") {
        try {
            nlohmann::json j;
            file >> j;
            
            if (j.is_object()) {
                for (auto& [key, value] : j.items()) {
                    std::string description = "";
                    if (value.is_object() && value.contains("_description")) {
                        description = value["_description"].get<std::string>();
                        nlohmann::json valueWithoutDesc = value;
                        valueWithoutDesc.erase("_description");
                        value = valueWithoutDesc;
                    }
                    
                    set(key, JsonUtils::JsonToConfigValue(value), ConfigSource::File);
                    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                        [&key](const ConfigEntry& e) { return e.key == key; });
                    if (it != pImpl->entries.end()) {
                        it->description = description;
                    }
                }
            } else {
                return false;
            }
        } catch (const std::exception& e) {
            return false;
        }
    } else if (ext == ".yaml" || ext == ".yml") {
        std::string line;
        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '%') {
                continue;
            }
            
            std::string key, value;
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                key = trim(line.substr(0, colonPos));
                value = trim(line.substr(colonPos + 1));
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                set(key, value, ConfigSource::File);
            }
        }
    } else {
        std::string line;
        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                set(key, value, ConfigSource::File);
            }
        }
    }
    
    return true;
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
    std::string ext = ".";
    size_t dotPos = filepath.rfind('.');
    if (dotPos != std::string::npos && dotPos < filepath.length() - 1) {
        ext = "." + filepath.substr(dotPos + 1);
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (ext == ".json") {
        nlohmann::json j = nlohmann::json::object();
        
        for (const auto& entry : pImpl->entries) {
            nlohmann::json valueJson;
            
            if (std::holds_alternative<std::string>(entry.value)) {
                valueJson = std::get<std::string>(entry.value);
            } else if (std::holds_alternative<int>(entry.value)) {
                valueJson = std::get<int>(entry.value);
            } else if (std::holds_alternative<int64_t>(entry.value)) {
                valueJson = std::get<int64_t>(entry.value);
            } else if (std::holds_alternative<double>(entry.value)) {
                valueJson = std::get<double>(entry.value);
            } else if (std::holds_alternative<bool>(entry.value)) {
                valueJson = std::get<bool>(entry.value);
            } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
                valueJson = std::get<std::vector<int>>(entry.value);
            } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
                valueJson = std::get<std::vector<double>>(entry.value);
            } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
                valueJson = std::get<std::vector<std::string>>(entry.value);
            }
            
            nlohmann::json entryJson = valueJson;
            if (!entry.description.empty()) {
                entryJson["_description"] = entry.description;
            }
            
            j[entry.key] = entryJson;
        }
        
        file << j.dump(2);
    } else {
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << JsonUtils::ConfigValueToString(entry.value) << "\n\n";
        }
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
        if constexpr (std::is_same_v<T, std::string>) {
            if (std::holds_alternative<std::string>(it->value)) {
                return std::get<std::string>(it->value);
            }
        } else if constexpr (std::is_same_v<T, int>) {
            if (std::holds_alternative<int>(it->value)) {
                return std::get<int>(it->value);
            } else if (std::holds_alternative<int64_t>(it->value)) {
                int64_t val = std::get<int64_t>(it->value);
                if (val >= std::numeric_limits<int>::min() && val <= std::numeric_limits<int>::max()) {
                    return static_cast<int>(val);
                }
            }
        } else if constexpr (std::is_same_v<T, int64_t>) {
            if (std::holds_alternative<int>(it->value)) {
                return static_cast<int64_t>(std::get<int>(it->value));
            } else if (std::holds_alternative<int64_t>(it->value)) {
                return std::get<int64_t>(it->value);
            }
        } else if constexpr (std::is_same_v<T, double>) {
            if (std::holds_alternative<double>(it->value)) {
                return std::get<double>(it->value);
            } else if (std::holds_alternative<int>(it->value)) {
                return static_cast<double>(std::get<int>(it->value));
            } else if (std::holds_alternative<int64_t>(it->value)) {
                return static_cast<double>(std::get<int64_t>(it->value));
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            if (std::holds_alternative<bool>(it->value)) {
                return std::get<bool>(it->value);
            }
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            if (std::holds_alternative<std::vector<int>>(it->value)) {
                return std::get<std::vector<int>>(it->value);
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            if (std::holds_alternative<std::vector<double>>(it->value)) {
                return std::get<std::vector<double>>(it->value);
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            if (std::holds_alternative<std::vector<std::string>>(it->value)) {
                return std::get<std::vector<std::string>>(it->value);
            }
        }
        
        return std::nullopt;
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
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[\"";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", \"";
                    oss << arg[i];
                }
                oss << "\"]";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << std::fixed << std::setprecision(6) << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, int> || 
                                 std::is_same_v<T, int64_t> ||
                                 std::is_same_v<T, double> ||
                                 std::is_same_v<T, bool>) {
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