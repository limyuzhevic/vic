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

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Empty configuration for production use
    // All production code should explicitly set values as needed
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Read and parse JSON file using nlohmann::json
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        // Parse JSON file
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        
        // Try to parse as JSON
        auto json = nlohmann::json::parse(content);
        
        // Convert JSON to config entries
        for (auto& [key, value] : json.items()) {
            ConfigSource source = ConfigSource::File;
            
            // Check if key is commented (starts with #)
            bool isCommented = false;
            std::string actualKey = key;
            
            // Handle optional comments in the key
            if (key.find("##") != std::string::npos) {
                // Remove comment marker
                actualKey = key.substr(0, key.find("##") - 1);
                isCommented = true;
            }
            
            // Skip commented keys
            if (isCommented) {
                continue;
            }
            
            // Convert JSON value to ConfigValue based on type
            ConfigValue configValue;
            
            if (value.is_string()) {
                configValue = value.get<std::string>();
            } else if (value.is_number_integer()) {
                configValue = value.get<int64_t>();
            } else if (value.is_number_float()) {
                configValue = value.get<double>();
            } else if (value.is_boolean()) {
                configValue = value.get<bool>();
            } else if (value.is_array()) {
                // Check if it's an array of strings or numbers
                if (!value.empty() && value[0].is_string()) {
                    std::vector<std::string> strArray;
                    for (const auto& item : value) {
                        strArray.push_back(item.get<std::string>());
                    }
                    configValue = strArray;
                } else if (!value.empty() && value[0].is_number()) {
                    std::vector<double> numArray;
                    for (const auto& item : value) {
                        numArray.push_back(item.get<double>());
                    }
                    configValue = numArray;
                }
            }
            
            set(actualKey, configValue, source);
        }
        
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        // JSON parsing failed, fall back to simple parser
        file.clear();
        file.seekg(0);
        
        std::string line;
        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Parse key=value pairs
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
                bool success = false;
                
                // Try int64_t
                try {
                    size_t posEnd;
                    int64_t intVal = std::stoll(value, &posEnd);
                    if (posEnd == value.size()) {
                        set(key, intVal, ConfigSource::File);
                        success = true;
                    }
                } catch (...) {}
                
                // Try double
                if (!success) {
                    try {
                        size_t posEnd;
                        double doubleVal = std::stod(value, &posEnd);
                        if (posEnd == value.size()) {
                            set(key, doubleVal, ConfigSource::File);
                            success = true;
                        }
                    } catch (...) {}
                }
                
                // Try boolean
                if (!success) {
                    if (toLower(value) == "true") {
                        set(key, true, ConfigSource::File);
                        success = true;
                    } else if (toLower(value) == "false") {
                        set(key, false, ConfigSource::File);
                        success = true;
                    }
                }
                
                // Try vector
                if (!success) {
                    std::string vectorContent = value;
                    if (vectorContent.front() == '[' && vectorContent.back() == ']') {
                        vectorContent = vectorContent.substr(1, vectorContent.size() - 2);
                        std::vector<std::string> parts;
                        std::string part;
                        for (char c : vectorContent) {
                            if (c == ',' || c == ' ') {
                                if (!part.empty()) {
                                    parts.push_back(part);
                                    part.clear();
                                }
                            } else {
                                part += c;
                            }
                        }
                        if (!part.empty()) {
                            parts.push_back(part);
                        }
                        
                        if (parts.size() > 1) {
                            // Try to parse as numbers
                            std::vector<double> numVector;
                            bool isNumberVector = true;
                            for (const auto& part : parts) {
                                try {
                                    numVector.push_back(std::stod(part));
                                } catch (...) {
                                    isNumberVector = false;
                                    break;
                                }
                            }
                            if (isNumberVector) {
                                set(key, numVector, ConfigSource::File);
                                success = true;
                            }
                        }
                    }
                }
                
                // Fall back to string
                if (!success) {
                    set(key, value, ConfigSource::File);
                }
            }
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
        
        // Convert ConfigValue to string based on type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, int64_t> || 
                                 std::is_same_v<T, int> ||
                                 std::is_same_v<T, double> ||
                                 std::is_same_v<T, bool>) {
                file << entry.key << " = " << arg << "\n";
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
                file << entry.key << " = \"PLACEHOLDER_VALUE\"\n";
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

// Advanced configuration features

bool Config::setBatch(const std::vector<std::pair<std::string, ConfigValue>>& updates,
                     ConfigSource source) {
    bool success = true;
    for (const auto& update : updates) {
        set(update.first, update.second, source);
    }
    return success;
}

std::vector<std::pair<std::string, bool>> Config::setMultipleFromJson(
    const std::string& filepath) {
    std::vector<std::pair<std::string, bool>> results;
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        results.push_back({filepath, false});
        return results;
    }
    
    try {
        auto json = nlohmann::json::parse(file);
        
        for (auto& [key, value] : json.items()) {
            ConfigValue configValue;
            
            if (value.is_string()) {
                configValue = value.get<std::string>();
            } else if (value.is_number_integer()) {
                configValue = value.get<int64_t>();
            } else if (value.is_number_float()) {
                configValue = value.get<double>();
            } else if (value.is_boolean()) {
                configValue = value.get<bool>();
            } else if (value.is_array()) {
                if (!value.empty() && value[0].is_string()) {
                    std::vector<std::string> strArray;
                    for (const auto& item : value) {
                        strArray.push_back(item.get<std::string>());
                    }
                    configValue = strArray;
                } else if (!value.empty() && value[0].is_number()) {
                    std::vector<double> numArray;
                    for (const auto& item : value) {
                        numArray.push_back(item.get<double>());
                    }
                    configValue = numArray;
                }
            }
            
            set(key, configValue, ConfigSource::File);
            results.push_back({key, true});
        }
        
        return results;
    } catch (const nlohmann::json::parse_error& e) {
        results.push_back({filepath, false});
        return results;
    }
}

std::vector<std::string> Config::validate() const {
    std::vector<std::string> errors;
    
    for (const auto& entry : pImpl->entries) {
        // Basic validation rules
        if (entry.key.empty()) {
            errors.push_back("Empty key found");
            continue;
        }
        
        // Type-specific validation
        if (std::holds_alternative<int64_t>(entry.value)) {
            int64_t val = std::get<int64_t>(entry.value);
            if (val < -1000000 || val > 1000000) {
                errors.push_back("Value for key '" + entry.key + "' out of range");
            }
        } else if (std::holds_alternative<double>(entry.value)) {
            double val = std::get<double>(entry.value);
            if (val < -1e6 || val > 1e6) {
                errors.push_back("Value for key '" + entry.key + "' out of range");
            }
        } else if (std::holds_alternative<std::string>(entry.value)) {
            const std::string& str = std::get<std::string>(entry.value);
            if (str.empty() || str.length() > 1000) {
                errors.push_back("Value for key '" + entry.key + "' invalid length");
            }
        } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
            const auto& vec = std::get<std::vector<std::string>>(entry.value);
            if (vec.size() > 1000) {
                errors.push_back("Value for key '" + entry.key + "' too large");
            }
        }
    }
    
    return errors;
}

bool Config::isValid() const {
    return validate().empty();
}

bool Config::exportToJson(const std::string& filepath, bool pretty) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    nlohmann::json json;
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&json, &entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int64_t> || 
                         std::is_same_v<T, int> ||
                         std::is_same_v<T, double> ||
                         std::is_same_v<T, bool>) {
                json[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::string>) {
                json[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                json[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                json[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                json[entry.key] = arg;
            } else {
                json[entry.key] = "PLACEHOLDER_VALUE";
            }
        }, entry.value);
    }
    
    if (pretty) {
        file << json.dump(2);
    } else {
        file << json.dump();
    }
    
    return true;
}

bool Config::importFromJson(const std::string& filepath) {
    return loadFromFile(filepath);
}

bool Config::saveTemplate(const std::string& name) const {
    std::string filepath = "templates/" + name + ".json";
    
    // Ensure templates directory exists
    std::filesystem::create_directories("templates");
    
    return exportToJson(filepath);
}

static bool Config::loadTemplate(const std::string& name,
                                 const std::string& filepath) {
    return Config().loadFromFile(filepath);
}

bool Config::hasChanges(const Config& other) const {
    if (pImpl->entries.size() != other.pImpl->entries.size()) {
        return true;
    }
    
    for (const auto& entry : pImpl->entries) {
        auto it = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it == other.pImpl->entries.end() || it->value != entry.value) {
            return true;
        }
    }
    
    return false;
}

double Config::calculateSimilarity(const Config& other) const {
    if (pImpl->entries.empty() && other.pImpl->entries.empty()) {
        return 1.0;
    }
    
    int matches = 0;
    int total = 0;
    
    for (const auto& entry : pImpl->entries) {
        auto it = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it != other.pImpl->entries.end()) {
            ++matches;
        }
        ++total;
    }
    
    for (const auto& entry : other.pImpl->entries) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it == pImpl->entries.end()) {
            ++total;
        }
    }
    
    return total > 0 ? static_cast<double>(matches) / total : 0.0;
}

size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

std::string Config::getStats() const {
    std::ostringstream oss;
    oss << "Config statistics:\n";
    oss << "  Total entries: " << pImpl->entries.size() << "\n";
    
    size_t intCount = 0, doubleCount = 0, stringCount = 0, boolCount = 0,
           intVecCount = 0, doubleVecCount = 0, stringVecCount = 0;
    
    for (const auto& entry : pImpl->entries) {
        if (std::holds_alternative<int64_t>(entry.value) ||
            std::holds_alternative<int>(entry.value)) {
            ++intCount;
        } else if (std::holds_alternative<double>(entry.value)) {
            ++doubleCount;
        } else if (std::holds_alternative<std::string>(entry.value)) {
            ++stringCount;
        } else if (std::holds_alternative<bool>(entry.value)) {
            ++boolCount;
        } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
            ++intVecCount;
        } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
            ++doubleVecCount;
        } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
            ++stringVecCount;
        }
    }
    
    oss << "  Integer values: " << intCount << "\n";
    oss << "  Double values: " << doubleCount << "\n";
    oss << "  String values: " << stringCount << "\n";
    oss << "  Boolean values: " << boolCount << "\n";
    oss << "  Integer vectors: " << intVecCount << "\n";
    oss << "  Double vectors: " << doubleVecCount << "\n";
    oss << "  String vectors: " << stringVecCount << "\n";
    
    return oss.str();
}

std::vector<std::string> Config::getIntegerKeys() const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (std::holds_alternative<int64_t>(entry.value) ||
            std::holds_alternative<int>(entry.value)) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

std::vector<std::string> Config::getDoubleKeys() const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (std::holds_alternative<double>(entry.value)) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

std::vector<std::string> Config::getStringKeys() const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (std::holds_alternative<std::string>(entry.value)) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

std::vector<std::string> Config::getBoolKeys() const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (std::holds_alternative<bool>(entry.value)) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

std::vector<std::string> Config::getVectorKeys() const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (std::holds_alternative<std::vector<int>>(entry.value) ||
            std::holds_alternative<std::vector<double>>(entry.value) ||
            std::holds_alternative<std::vector<std::string>>(entry.value)) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

bool Config::validateValue(const std::string& key, const ConfigValue& value) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return true;  // No validation for new keys
    }
    
    // Type-based validation
    if (std::holds_alternative<int64_t>(value) && std::holds_alternative<int64_t>(it->value)) {
        int64_t val = std::get<int64_t>(value);
        if (val < -1000000 || val > 1000000) return false;  // Reasonable range
        return true;
    }
    
    if (std::holds_alternative<double>(value) && std::holds_alternative<double>(it->value)) {
        double val = std::get<double>(value);
        if (val < -1e6 || val > 1e6) return false;
        return true;
    }
    
    if (std::holds_alternative<bool>(value) && std::holds_alternative<bool>(it->value)) {
        return true;  // Any bool is valid
    }
    
    if (std::holds_alternative<std::string>(value) && std::holds_alternative<std::string>(it->value)) {
        const std::string& str = std::get<std::string>(value);
        if (str.empty() || str.length() > 1000) return false;
        return true;
    }
    
    if (std::holds_alternative<std::vector<int>>(value) && std::holds_alternative<std::vector<int>>(it->value)) {
        const auto& vec = std::get<std::vector<int>>(value);
        if (vec.size() > 1000) return false;
        return true;
    }
    
    if (std::holds_alternative<std::vector<double>>(value) && std::holds_alternative<std::vector<double>>(it->value)) {
        const auto& vec = std::get<std::vector<double>>(value);
        if (vec.size() > 1000) return false;
        return true;
    }
    
    if (std::holds_alternative<std::vector<std::string>>(value) && std::holds_alternative<std::vector<std::string>>(it->value)) {
        const auto& vec = std::get<std::vector<std::string>>(value);
        if (vec.size() > 1000) return false;
        for (const auto& str : vec) {
            if (str.empty() || str.length() > 100) return false;
        }
        return true;
    }
    
    return true;  // For now, allow any type changes
}

ConfigValue Config::convertToTargetType(const ConfigValue& value,
                                        const std::string& targetType) const {
    // Type conversion logic
    std::string lowerType = toLower(targetType);
    
    if (lowerType == "int" || lowerType == "integer") {
        if (std::holds_alternative<int64_t>(value)) {
            return value;
        } else if (std::holds_alternative<double>(value)) {
            double val = std::get<double>(value);
            return static_cast<int64_t>(val);
        } else if (std::holds_alternative<bool>(value)) {
            bool val = std::get<bool>(value);
            return val ? int64_t(1) : int64_t(0);
        }
    }
    
    if (lowerType == "double" || lowerType == "float") {
        if (std::holds_alternative<int64_t>(value)) {
            int64_t val = std::get<int64_t>(value);
            return static_cast<double>(val);
        } else if (std::holds_alternative<double>(value)) {
            return value;
        } else if (std::holds_alternative<bool>(value)) {
            bool val = std::get<bool>(value);
            return val ? 1.0 : 0.0;
        }
    }
    
    if (lowerType == "bool" || lowerType == "boolean") {
        if (std::holds_alternative<bool>(value)) {
            return value;
        } else if (std::holds_alternative<int64_t>(value)) {
            int64_t val = std::get<int64_t>(value);
            return val != 0;
        }
    }
    
    if (lowerType == "string") {
        if (std::holds_alternative<std::string>(value)) {
            return value;
        } else if (std::holds_alternative<int64_t>(value)) {
            int64_t val = std::get<int64_t>(value);
            return std::to_string(val);
        } else if (std::holds_alternative<double>(value)) {
            double val = std::get<double>(value);
            return std::to_string(val);
        }
    }
    
    return value;  // Return original value if no conversion possible
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
