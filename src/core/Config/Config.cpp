#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cstring>
#include <cmath>
#include <cctype>
#include <unordered_map>
#include <nlohmann/json.hpp>

namespace nlm {

using json = nlohmann::json;

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::string> descriptions; // Additional descriptions
    std::vector<std::shared_ptr<ConfigValidator>> validators;
    std::unordered_map<std::string, std::vector<std::string>> validationPatterns; // For future validation
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Check file extension to determine format
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    try {
        if (ext == ".json" || ext == ".yaml" || ext == ".yml") {
            return loadFromJSON(filepath);
        } else if (ext == ".cfg" || ext == ".txt" || ext == ".ini" || ext.empty()) {
            // Maintain backward compatibility with simple key=value format
            return loadFromSimpleFormat(filepath);
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to load config from " + filepath + ": " + e.what()));
        return false;
    }
    
    return loadFromSimpleFormat(filepath);
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        json j;
        file >> j;
        
        clear(); // Clear existing entries
        
        // Recursively process JSON into config entries
        processJSON(j, "", ConfigSource::File);
        
        NLM_LOG_INFO("Loaded JSON configuration from: " + filepath);
        return true;
        
    } catch (const json::parse_error& e) {
        NLM_LOG_ERROR(std::string("JSON parse error in " + filepath + ": " + e.what()));
        return false;
    } catch (const json::type_error& e) {
        NLM_LOG_ERROR(std::string("JSON type error in " + filepath + ": " + e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error loading JSON config from " + filepath + ": " + e.what()));
        return false;
    }
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
    // Enhanced version with better error handling
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    clear(); // Clear existing entries
    
    std::string line;
    size_t lineNum = 0;
    while (std::getline(file, line)) {
        lineNum++;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/' || line[0] == ';') {
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
            
            // Store description if available from comments
            std::string description = "";
            auto descIt = pImpl->descriptions.find(key);
            if (descIt != pImpl->descriptions.end()) {
                description = descIt->second;
                pImpl->descriptions.erase(descIt);
            }
            
            set(key, value, ConfigSource::File, description);
        } else {
            NLM_LOG_WARNING(std::string("Invalid line " + std::to_string(lineNum) + " in config file: " + line));
        }
    }
    
    if (lineNum > 0) {
        NLM_LOG_INFO("Loaded configuration from: " + filepath);
    }
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    clear(); // Clear existing entries
    
    bool success = true;
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            } else {
                NLM_LOG_WARNING(std::string("Invalid command line argument format: " + arg));
                success = false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        } else if (arg[0] == '-') {
            NLM_LOG_WARNING(std::string("Missing value for argument: " + arg));
            success = false;
        }
    }
    
    if (!success) {
        NLM_LOG_WARNING("Some command line arguments were invalid");
    }
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR(std::string("Failed to open config file for writing: " + filepath));
        return false;
    }
    
    try {
        // Determine format based on file extension
        std::string ext = "." + std::filesystem::path(filepath).extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        if (ext == ".json") {
            saveToJSON(file);
        } else {
            saveToSimpleFormat(file);
        }
        
        NLM_LOG_INFO("Configuration saved to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error saving config to " + filepath + ": " + e.what()));
        return false;
    }
}

void Config::processJSON(const json& j, const std::string& prefix, ConfigSource source) {
    if (j.is_object()) {
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = prefix.empty() ? it.key() : prefix + "." + it.key();
            processJSON(it.value(), key, source);
        }
    } else {
        // Leaf value - convert to ConfigValue
        ConfigValue value;
        
        if (j.is_string()) {
            value = j.get<std::string>();
        } else if (j.is_number_integer()) {
            value = j.get<int64_t>();
        } else if (j.is_number_float()) {
            value = j.get<double>();
        } else if (j.is_boolean()) {
            value = j.get<bool>();
        } else if (j.is_array()) {
            // Convert array to vector based on content
            std::vector<json> arr = j.get<std::vector<json>>();
            if (!arr.empty()) {
                if (std::all_of(arr.begin(), arr.end(), [](const json& v) { return v.is_number_integer(); })) {
                    std::vector<int> intVec;
                    for (const auto& v : arr) intVec.push_back(v.get<int64_t>());
                    value = intVec;
                } else if (std::all_of(arr.begin(), arr.end(), [](const json& v) { return v.is_number_float(); })) {
                    std::vector<double> doubleVec;
                    for (const auto& v : arr) doubleVec.push_back(v.get<double>());
                    value = doubleVec;
                } else if (std::all_of(arr.begin(), arr.end(), [](const json& v) { return v.is_string(); })) {
                    std::vector<std::string> strVec;
                    for (const auto& v : arr) strVec.push_back(v.get<std::string>());
                    value = strVec;
                } else {
                    // Mixed type array - store as JSON string representation
                    value = j.dump();
                }
            } else {
                value = std::vector<int>();
            }
        } else {
            // Other types
            value = j.dump(); // Fallback to string representation
        }
        
        set(key, value, source);
    }
}

void Config::saveToJSON(std::ostream& stream) const {
    json j;
    
    // Recursively build JSON from config entries
    for (const auto& entry : pImpl->entries) {
        std::string key = entry.key;
        
        // Handle nested keys (e.g., "neural.stdp_ltp_weight")
        size_t dotPos = key.find('.');
        if (dotPos != std::string::npos) {
            std::string prefix = key.substr(0, dotPos);
            std::string suffix = key.substr(dotPos + 1);
            
            if (!j.contains(prefix)) {
                j[prefix] = json::object();
            }
            
            // Recursively set nested values
            setJSONValue(j[prefix], suffix, entry.value);
        } else {
            setJSONValue(j, entry.key, entry.value);
        }
    }
    
    // Add descriptions as comments (not standard JSON, but useful)
    stream << j.dump(2) << std::endl;
    
    // Write descriptions as comments
    for (const auto& entry : pImpl->entries) {
        if (!entry.description.empty()) {
            stream << "# " << entry.description << std::endl;
        }
    }
}

void Config::setJSONValue(json& j, const std::string& key, const ConfigValue& value) const {
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            j[key] = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            j[key] = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            j[key] = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            j[key] = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            j[key] = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            j[key] = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            j[key] = arg;
        }
    }, value);
}

void Config::saveToSimpleFormat(std::ostream& stream) const {
    for (const auto& entry : pImpl->entries) {
        if (!entry.description.empty()) {
            stream << "# " << entry.description << "\n";
        }
        
        stream << entry.key << " = ";
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                stream << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                stream << (arg ? "true" : "false");
            } else {
                stream << arg;
            }
        }, entry.value);
        stream << "\n\n";
    }
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
        NLM_LOG_ERROR(std::string("Type mismatch for config key: " + key));
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

std::shared_ptr<TypedConfigAccessor> Config::createAccessor(
    const std::vector<std::shared_ptr<ConfigValidator>>& validators) const {
    return std::make_shared<TypedConfigAccessor>(*this, validators);
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    set(key, value, source, "");
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

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source, const std::string& description) {
    // Validate against registered validators if any
    for (const auto& validator : pImpl->validators) {
        if (!validator->validate(key, value)) {
            NLM_LOG_ERROR(std::string("Config validation failed for key '" + key + "': " + 
                validator->getErrorMessage(key, value)));
            return; // Reject invalid configuration
        }
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        if (!description.empty()) {
            it->description = description;
        }
    } else {
        ConfigEntry newEntry(key, value, source, description);
        pImpl->entries.emplace_back(std::move(newEntry));
    }
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
    pImpl->descriptions.erase(key);
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
    pImpl->descriptions.clear();
    pImpl->validationPatterns.clear();
    pImpl->validators.clear();
}

bool Config::validate() const {
    for (const auto& entry : pImpl->entries) {
        if (!validateEntry(entry)) {
            NLM_LOG_ERROR(std::string("Configuration validation failed for key: " + entry.key));
            return false;
        }
    }
    return true;
}

bool Config::validateEntry(const ConfigEntry& entry) const {
    // Check against registered validators
    for (const auto& validator : pImpl->validators) {
        if (!validator->validate(entry.key, entry.value)) {
            return false;
        }
    }
    
    // Check against validation patterns
    auto it = pImpl->validationPatterns.find(entry.key);
    if (it != pImpl->validationPatterns.end()) {
        const auto& patterns = it->second;
        // Future validation logic can be implemented here
    }
    
    return true;
}

void Config::addValidator(const std::shared_ptr<ConfigValidator>& validator) {
    if (validator) {
        pImpl->validators.push_back(validator);
    }
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
        oss << "] (Source: " << static_cast<int>(entry.source) << ")";
        if (!entry.description.empty()) {
            oss << " - " << entry.description;
        }
        oss << "\n";
    }
    return oss.str();
}

// Advanced configuration methods
bool Config::loadFromJSON(const std::string& jsonString) {
    try {
        auto j = json::parse(jsonString);
        clear(); // Clear existing entries
        processJSON(j, "", ConfigSource::Runtime);
        return true;
    } catch (const json::parse_error& e) {
        NLM_LOG_ERROR(std::string("JSON parse error: " + e.what()));
        return false;
    }
}

bool Config::importFromJSON(const std::string& jsonString) {
    return loadFromJSON(jsonString);
}

std::string Config::exportToJSON() const {
    json j;
    for (const auto& entry : pImpl->entries) {
        setJSONValue(j, entry.key, entry.value);
    }
    return j.dump(2);
}

std::string Config::getValueAsString(const std::string& key) const {
    auto entry = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (entry == pImpl->entries.end()) {
        return "";
    }
    
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else {
            oss << arg;
        }
    }, entry->value);
    
    return oss.str();
}

ConfigValue Config::getValue(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->value;
    }
    
    throw std::runtime_error("Config key not found: " + key);
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

// Explicit template instantiations for common types
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
