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

// Helper function to convert string to lowercase
std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Helper function to check if file has JSON extension
bool Config::isJSONFile(const std::string& filepath) {
    std::string ext = ".json";
    if (filepath.size() < ext.size()) return false;
    std::string lowerPath = toLower(filepath);
    std::string lowerExt = toLower(ext);
    return lowerPath.substr(lowerPath.size() - ext.size()) == lowerExt;
}

// Helper function to check if file has YAML extension
bool Config::isYAMLFile(const std::string& filepath) {
    std::string ext = ".yaml";
    if (filepath.size() < ext.size()) return false;
    std::string lowerPath = toLower(filepath);
    std::string lowerExt = toLower(ext);
    return lowerPath.substr(lowerPath.size() - ext.size()) == lowerExt;
}

// Helper function to parse JSON value to ConfigValue
ConfigValue Config::parseJSONValue(const nlohmann::json& j) {
    if (j.is_string()) {
        return j.get<std::string>();
    } else if (j.is_number_integer()) {
        return j.get<int64_t>();
    } else if (j.is_number_float()) {
        return j.get<double>();
    } else if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_array()) {
        // Try to detect array type
        bool allInts = true;
        bool allDoubles = true;
        bool allStrings = true;
        
        for (const auto& item : j) {
            if (!item.is_number_integer()) allInts = false;
            if (!item.is_number_float()) allDoubles = false;
            if (!item.is_string()) allStrings = false;
        }
        
        if (allInts) {
            std::vector<int64_t> arr;
            arr.reserve(j.size());
            for (const auto& item : j) {
                arr.push_back(item.get<int64_t>());
            }
            return arr;
        } else if (allDoubles) {
            std::vector<double> arr;
            arr.reserve(j.size());
            for (const auto& item : j) {
                arr.push_back(item.get<double>());
            }
            return arr;
        } else if (allStrings) {
            std::vector<std::string> arr;
            arr.reserve(j.size());
            for (const auto& item : j) {
                arr.push_back(item.get<std::string>());
            }
            return arr;
        }
        // Fallback to vector of strings for mixed arrays
        std::vector<std::string> arr;
        arr.reserve(j.size());
        for (const auto& item : j) {
            arr.push_back(item.dump());
        }
        return arr;
    } else if (j.is_null()) {
        return std::string("null");
    } else {
        // For any other type, convert to string
        return j.dump();
    }
}

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Determine file format by extension
    std::string lowerPath = toLower(filepath);
    bool isJSON = isJSONFile(filepath);
    bool isYAML = isYAMLFile(filepath);
    
    if (isJSON) {
        // Parse JSON file
        try {
            nlohmann::json j;
            file >> j;
            
            // Clear existing entries
            clear();
            
            // Process JSON object
            if (j.is_object()) {
                for (auto it = j.begin(); it != j.end(); ++it) {
                    // Get description from comment if available (simplified approach)
                    std::string description = "";
                    
                    set(it.key(), parseJSONValue(it.value()), ConfigSource::File);
                }
            } else {
                return false;  // JSON root should be an object
            }
            
        } catch (const nlohmann::json::parse_error& e) {
            return false;
        } catch (const nlohmann::json::type_error& e) {
            return false;
        }
    } else if (isYAML) {
        // TODO: Implement YAML parsing if needed
        // For now, fall back to simple key=value format
        file.clear();
        file.seekg(0);
        return loadFromSimpleFormat(file);
    } else {
        // Try to auto-detect format by reading first few lines
        file.clear();
        file.seekg(0);
        
        std::string line;
        bool looksLikeJSON = false;
        bool looksLikeYAML = false;
        
        for (int i = 0; i < 10 && std::getline(file, line); ++i) {
            line = trim(line);
            if (line.empty() || line[0] == '#') continue;
            
            if (line[0] == '{' || line.find("\"key\"") != std::string::npos || line.find("'key'") != std::string::npos) {
                looksLikeJSON = true;
            }
            if (line.find(":") != std::string::npos && (line.find("- ") != std::string::npos || line.find(":") != std::string::npos)) {
                looksLikeYAML = true;
            }
        }
        
        file.clear();
        file.seekg(0);
        
        if (looksLikeJSON) {
            return loadFromFile(filepath);  // Recursive call for JSON
        } else if (looksLikeYAML) {
            // TODO: Implement YAML parsing
            return loadFromSimpleFormat(file);
        } else {
            return loadFromSimpleFormat(file);
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    // Validate input
    if (!argv || argc <= 1) {
        return false;
    }
    
    for (int i = 1; i < argc; ++i) {
        if (!argv[i]) {
            continue; // Skip null pointers
        }
        
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.size() >= 3 && arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos && pos + 1 < arg.size()) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            } else if (pos != std::string::npos) {
                // Key with empty value
                std::string key = arg.substr(2, pos - 2);
                set(key, "", ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg.size() > 1 && arg[0] == '-' && i + 1 < argc && argv[i + 1] != nullptr) {
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
        
        // Write the actual value based on its type
        std::visit([&file, &entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]\n";
            } else {
                file << entry.key << " = " << arg << "\n";
            }
        }, entry.value);
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

// Function to convert string to lowercase
std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Helper function to check if file has JSON extension
bool Config::isJSONFile(const std::string& filepath) {
    std::string ext = ".json";
    if (filepath.size() < ext.size()) return false;
    std::string lowerPath = toLower(filepath);
    std::string lowerExt = toLower(ext);
    return lowerPath.substr(lowerPath.size() - ext.size()) == lowerExt;
}

// Helper function to check if file has YAML extension
bool Config::isYAMLFile(const std::string& filepath) {
    std::string ext = ".yaml";
    if (filepath.size() < ext.size()) return false;
    std::string lowerPath = toLower(filepath);
    std::string lowerExt = toLower(ext);
    return lowerPath.substr(lowerPath.size() - ext.size()) == lowerExt;
}

// Helper method to load from simple key=value format
bool Config::loadFromSimpleFormat(std::ifstream& file) {
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

// Function to parse JSON value to ConfigValue
ConfigValue Config::parseJSONValue(const nlohmann::json& j) {
    if (j.is_string()) {
        return j.get<std::string>();
    } else if (j.is_number_integer()) {
        return j.get<int64_t>();
    } else if (j.is_number_float()) {
        return j.get<double>();
    } else if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_array()) {
        // Try to detect array type
        bool allInts = true;
        bool allDoubles = true;
        bool allStrings = true;
        
        for (const auto& item : j) {
            if (!item.is_number_integer()) allInts = false;
            if (!item.is_number_float()) allDoubles = false;
            if (!item.is_string()) allStrings = false;
        }
        
        if (allInts) {
            std::vector<int64_t> arr;
            arr.reserve(j.size());
            for (const auto& item : j) {
                arr.push_back(item.get<int64_t>());
            }
            return arr;
        } else if (allDoubles) {
            std::vector<double> arr;
            arr.reserve(j.size());
            for (const auto& item : j) {
                arr.push_back(item.get<double>());
            }
            return arr;
        } else if (allStrings) {
            std::vector<std::string> arr;
            arr.reserve(j.size());
            for (const auto& item : j) {
                arr.push_back(item.get<std::string>());
            }
            return arr;
        }
        // Fallback to vector of strings for mixed arrays
        std::vector<std::string> arr;
        arr.reserve(j.size());
        for (const auto& item : j) {
            arr.push_back(item.dump());
        }
        return arr;
    } else if (j.is_null()) {
        return std::string("null");
    } else {
        // For any other type, convert to string
        return j.dump();
    }
}

// Function to convert string to lowercase
std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

} // namespace nlm
