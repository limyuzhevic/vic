#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

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
    
    // Detect file format by extension
    std::string ext = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = filepath.substr(dotPos + 1);
    }
    
    try {
        if (ext == "json" || ext.empty()) {
            // Try JSON parsing
            json j;
            file >> j;
            
            // Convert JSON to ConfigEntry vector
            pImpl->entries.clear();
            
            if (j.is_object() && j.contains("entries")) {
                // New format with "entries" array
                const auto& entriesArray = j["entries"];
                for (const auto& entryJson : entriesArray) {
                    ConfigEntry entry;
                    entry.key = entryJson.value("key", "");
                    entry.description = entryJson.value("description", "");
                    
                    // Parse source
                    int sourceInt = entryJson.value("source", 0);
                    entry.source = static_cast<ConfigSource>(sourceInt);
                    
                    // Parse value based on type
                    if (entryJson.contains("value")) {
                        const auto& valueJson = entryJson["value"];
                        
                        if (valueJson.is_number_integer()) {
                            // Check if it fits in int64_t
                            try {
                                entry.value = valueJson.get<int64_t>();
                            } catch (...) {
                                entry.value = valueJson.get<double>();
                            }
                        } else if (valueJson.is_number()) {
                            entry.value = valueJson.get<double>();
                        } else if (valueJson.is_boolean()) {
                            entry.value = valueJson.get<bool>();
                        } else if (valueJson.is_string()) {
                            entry.value = valueJson.get<std::string>();
                        } else if (valueJson.is_array()) {
                            // Handle arrays (simplified)
                            if (entry.key.find("weight") != std::string::npos || 
                                entry.key.find("rate") != std::string::npos) {
                                // Try to parse as double array
                                std::vector<double> doubleArray;
                                for (const auto& item : valueJson) {
                                    doubleArray.push_back(item.get<double>());
                                }
                                entry.value = doubleArray;
                            } else {
                                // Try to parse as string array
                                std::vector<std::string> stringArray;
                                for (const auto& item : valueJson) {
                                    stringArray.push_back(item.get<std::string>());
                                }
                                entry.value = stringArray;
                            }
                        }
                    }
                    
                    pImpl->entries.push_back(entry);
                }
            } else {
                // Old format: simple key=value pairs
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
    } 
    catch (const std::exception& e) {
        // If JSON parsing fails, try the simple format as fallback
        file.clear();
        file.seekg(0);
        
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
        return true; // Fallback succeeded
    }
    
    return false;
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
    
    // Detect file format by extension
    std::string ext = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = filepath.substr(dotPos + 1);
    }
    
    try {
        if (ext == "json" || ext.empty()) {
            json j;
            
            // Create new JSON format with "entries" array
            j["description"] = "NLM Configuration";
            j["version"] = "1.0";
            
            // Add entries array
            json entriesArray = json::array();
            for (const auto& entry : pImpl->entries) {
                json entryJson;
                entryJson["key"] = entry.key;
                entryJson["description"] = entry.description;
                entryJson["source"] = static_cast<int>(entry.source);
                
                // Convert ConfigValue to JSON
                json valueJson;
                std::visit([&valueJson](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, int>) {
                        valueJson = arg;
                    } else if constexpr (std::is_same_v<T, int64_t>) {
                        valueJson = arg;
                    } else if constexpr (std::is_same_v<T, double>) {
                        valueJson = arg;
                    } else if constexpr (std::is_same_v<T, bool>) {
                        valueJson = arg;
                    } else if constexpr (std::is_same_v<T, std::string>) {
                        valueJson = arg;
                    } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                        valueJson = arg;
                    } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                        valueJson = arg;
                    } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        valueJson = arg;
                    }
                }, entry.value);
                
                entryJson["value"] = valueJson;
                entriesArray.push_back(entryJson);
            }
            
            j["entries"] = entriesArray;
            
            file << j.dump(2); // Pretty print with 2-space indent
            return true;
        }
    } 
    catch (const std::exception& e) {
        // Fallback to simple format
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << entry.key << " = \"" << arg << "\"\n";
                } else {
                    file << entry.key << " = " << arg << "\n";
                }
            }, entry.value);
            file << "\n";
        }
        return true;
    }
    
    return false;
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
