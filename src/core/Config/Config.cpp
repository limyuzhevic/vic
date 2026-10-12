#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

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
    json j;
    try {
        file >> j;
        clear();
        parseJson(j);
        return true;
    } catch (const json::parse_error&) {
        // Reset file stream for YAML parsing
        file.clear();
        file.seekg(0);
    }
    
    // Try to parse as YAML if JSON parsing fails
    // For now, we'll assume YAML is similar enough to JSON for basic parsing
    // In a full implementation, we'd use a YAML parser like yaml-cpp
    return loadFromFileSimpleFormat(filepath);
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

// Add new helper methods to Config class before the namespace closing

// Parse JSON into config entries
void Config::parseJson(const json& j) {
    if (j.is_object()) {
        for (const auto& [key, value] : j.items()) {
            ConfigValue configValue = jsonToConfigValue(value);
            set(key, configValue, ConfigSource::File);
        }
    }
}

// Convert JSON value to ConfigValue
ConfigValue Config::jsonToConfigValue(const json& j) {
    if (j.is_string()) {
        return j.get<std::string>();
    } else if (j.is_number_integer()) {
        return j.get<int64_t>();
    } else if (j.is_number_unsigned()) {
        return j.get<uint64_t>();
    } else if (j.is_number_float()) {
        return j.get<double>();
    } else if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_array()) {
        // Handle arrays - check element types
        const auto& arr = j.get<std::vector<json>>();
        if (!arr.empty()) {
            // Try to infer array type
            if (std::all_of(arr.begin(), arr.end(), [](const json& item) { return item.is_number_integer(); })) {
                return arr.get<std::vector<int64_t>>();
            } else if (std::all_of(arr.begin(), arr.end(), [](const json& item) { return item.is_number_float(); })) {
                return arr.get<std::vector<double>>();
            } else if (std::all_of(arr.begin(), arr.end(), [](const json& item) { return item.is_string(); })) {
                return arr.get<std::vector<std::string>>();
            } else if (std::all_of(arr.begin(), arr.end(), [](const json& item) { return item.is_boolean(); })) {
                return arr.get<std::vector<bool>>();
            }
        }
        // Default to vector of strings if type cannot be determined
        std::vector<std::string> strArray;
        for (const auto& item : arr) {
            strArray.push_back(item.get<std::string>());
        }
        return strArray;
    } else if (j.is_null()) {
        return std::string("");
    }
    // Fallback to string representation
    return j.dump();
}

// Save config to JSON file
bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Create JSON object
    json j;
    
    for (const auto& entry : pImpl->entries) {
        j[entry.key] = configValueToJson(entry.value);
        // Note: description is not saved in JSON format for simplicity
    }
    
    // Write JSON with indentation for readability
    file << j.dump(2);
    
    return true;
}

// Convert ConfigValue to JSON value
json Config::configValueToJson(const ConfigValue& value) const {
    return std::visit([](const auto& val) -> json {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return val;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return val;
        } else if constexpr (std::is_same_v<T, uint64_t>) {
            return val;
        } else if constexpr (std::is_same_v<T, double>) {
            return val;
        } else if constexpr (std::is_same_v<T, bool>) {
            return val;
        } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
            return val;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return val;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return val;
        } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
            return val;
        } else {
            // For unknown types, convert to string
            return std::to_string(val);
        }
    }, value);
}

// Legacy simple format loader (kept for compatibility)
bool Config::loadFromFileSimpleFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    clear();
    
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
            
            // Try to convert value to appropriate type
            if (value == "true" || value == "false") {
                bool boolVal = (value == "true");
                set(key, boolVal, ConfigSource::File);
            } else {
                try {
                    // Try integer conversion
                    size_t posEnd;
                    int64_t intVal = std::stoll(value, &posEnd);
                    if (posEnd == value.size()) {
                        set(key, intVal, ConfigSource::File);
                        continue;
                    }
                } catch (...) {}
                
                try {
                    // Try floating point conversion
                    size_t posEnd;
                    double doubleVal = std::stod(value, &posEnd);
                    if (posEnd == value.size()) {
                        set(key, doubleVal, ConfigSource::File);
                        continue;
                    }
                } catch (...) {}
                
                // Default to string
                set(key, value, ConfigSource::File);
            }
        }
    }
    
    return true;
}

} // namespace nlm
