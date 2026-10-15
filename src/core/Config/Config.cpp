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
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to parse as JSON first
    try {
        nlohmann::json jsonConfig;
        file >> jsonConfig;
        
        // Parse JSON and convert to internal format
        for (auto it = jsonConfig.begin(); it != jsonConfig.end(); ++it) {
            std::string key = it.key();
            ConfigValue value;
            
            // Convert JSON value to appropriate ConfigValue type
            if (it->is_boolean()) {
                value = it->get<bool>();
            } else if (it->is_number_integer()) {
                value = it->get<int64_t>();
            } else if (it->is_number_float()) {
                value = it->get<double>();
            } else if (it->is_string()) {
                value = it->get<std::string>();
            } else if (it->is_array()) {
                // Handle arrays - determine element type
                const auto& arr = *it;
                if (arr.empty()) {
                    continue;  // Skip empty arrays
                }
                
                // Check if all elements are same type
                bool allInts = std::all_of(arr.begin(), arr.end(), [](const auto& el) { return el.is_number_integer(); });
                bool allDoubles = std::all_of(arr.begin(), arr.end(), [](const auto& el) { return el.is_number_float(); });
                bool allStrings = std::all_of(arr.begin(), arr.end(), [](const auto& el) { return el.is_string(); });
                
                if (allInts) {
                    std::vector<int64_t> intArray;
                    intArray.reserve(arr.size());
                    for (const auto& el : arr) {
                        intArray.push_back(el.get<int64_t>());
                    }
                    value = intArray;
                } else if (allDoubles) {
                    std::vector<double> doubleArray;
                    doubleArray.reserve(arr.size());
                    for (const auto& el : arr) {
                        doubleArray.push_back(el.get<double>());
                    }
                    value = doubleArray;
                } else if (allStrings) {
                    std::vector<std::string> stringArray;
                    stringArray.reserve(arr.size());
                    for (const auto& el : arr) {
                        stringArray.push_back(el.get<std::string>());
                    }
                    value = stringArray;
                } else {
                    // Mixed types - store as string representation
                    std::string arrayStr = "[";
                    for (size_t i = 0; i < arr.size(); ++i) {
                        if (i > 0) arrayStr += ", ";
                        arrayStr += arr[i].dump();
                    }
                    arrayStr += "]";
                    value = arrayStr;
                }
            } else {
                // For other types (objects, null), store as string representation
                value = it->dump();
            }
            
            set(key, value, ConfigSource::File);
        }
        
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        // Not valid JSON, fall back to simple key=value format
        file.clear();
        file.seekg(0);
    } catch (...) {
        // Any other error, fall back to simple format
        file.clear();
        file.seekg(0);
    }
    
    // Fallback to simple key=value format
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
    
    // Create JSON object
    nlohmann::json jsonConfig;
    
    for (const auto& entry : pImpl->entries) {
        // Convert ConfigValue to JSON based on its type
        std::visit([&](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            
            if constexpr (std::is_same_v<T, int64_t>) {
                jsonConfig[entry.key] = val;
            } else if constexpr (std::is_same_v<T, double>) {
                jsonConfig[entry.key] = val;
            } else if constexpr (std::is_same_v<T, bool>) {
                jsonConfig[entry.key] = val;
            } else if constexpr (std::is_same_v<T, std::string>) {
                jsonConfig[entry.key] = val;
            } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
                // Convert int64_t vector to array
                jsonConfig[entry.key] = val;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                // Convert double vector to array
                jsonConfig[entry.key] = val;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                // Convert string vector to array
                jsonConfig[entry.key] = val;
            } else {
                // For any other type, use string representation
                jsonConfig[entry.key] = "[UNSUPPORTED_TYPE]";
            }
        }, entry.value);
    }
    
    // Write JSON to file with indentation for readability
    file << jsonConfig.dump(2);
    
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
