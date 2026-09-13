#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <memory>
#include <stdexcept>

// Try to include nlohmann::json if available
#ifdef HAVE_NLOHMANN_JSON
#include <nlohmann/json.hpp>
#endif

// Try to include yaml-cpp if available  
#ifdef HAVE_YAML_CPP
#include <yaml-cpp/yaml.h>
#endif

namespace nlm {

// Helper methods for JSON and YAML parsing
namespace {
    
    // Try to detect and parse JSON content
    bool tryParseJSON(const std::string& content, std::vector<ConfigEntry>& entries) {
#ifdef HAVE_NLOHMANN_JSON
        try {
            auto json = nlohmann::json::parse(content);
            
            // Convert JSON object to ConfigEntry vector
            for (auto it = json.begin(); it != json.end(); ++it) {
                std::string key = it.key();
                ConfigValue value;
                
                // Handle different JSON value types
                const auto& val = *it;
                if (val.is_string()) {
                    value = val.get<std::string>();
                } else if (val.is_number_integer()) {
                    value = val.get<int64_t>();
                } else if (val.is_number_float()) {
                    value = val.get<double>();
                } else if (val.is_boolean()) {
                    value = val.get<bool>();
                } else if (val.is_array()) {
                    // Convert array to appropriate vector type
                    if (!val.empty()) {
                        // Check element type
                        const auto& first = val[0];
                        if (first.is_string()) {
                            std::vector<std::string> vec;
                            for (const auto& item : val) {
                                vec.push_back(item.get<std::string>());
                            }
                            value = vec;
                        } else if (first.is_number_integer()) {
                            std::vector<int64_t> vec;
                            for (const auto& item : val) {
                                vec.push_back(item.get<int64_t>());
                            }
                            value = vec;
                        } else if (first.is_number_float()) {
                            std::vector<double> vec;
                            for (const auto& item : val) {
                                vec.push_back(item.get<double>());
                            }
                            value = vec;
                        } else {
                            // Default to int vector for unknown numeric types
                            std::vector<int64_t> vec;
                            for (const auto& item : val) {
                                vec.push_back(item.get<int64_t>());
                            }
                            value = vec;
                        }
                    } else {
                        value = std::vector<std::string>{};
                    }
                }
                
                entries.emplace_back(key, value, ConfigSource::File);
            }
            
            return true;
        } catch (const std::exception& e) {
            // JSON parsing failed, return false
            return false;
        }
#else
        // nlohmann::json not available
        return false;
#endif
    }
    
    // Try to detect and parse YAML content  
    bool tryParseYAML(const std::string& content, std::vector<ConfigEntry>& entries) {
#ifdef HAVE_YAML_CPP
        try {
            auto yaml = YAML::Load(content);
            
            // Convert YAML mapping to ConfigEntry vector
            for (const auto& pair : yaml) {
                std::string key = pair.first.as<std::string>();
                ConfigValue value;
                
                // Handle different YAML value types
                const auto& val = pair.second;
                if (val.IsScalar()) {
                    const std::string& str = val.Scalar();
                    
                    // Try to convert to boolean
                    if (str == "true" || str == "yes") {
                        value = true;
                    } else if (str == "false" || str == "no") {
                        value = false;
                    } 
                    // Try to convert to integer
                    else {
                        try {
                            size_t pos;
                            int64_t int_val = std::stoll(str, &pos);
                            if (pos == str.size()) {
                                value = int_val;
                            }
                        } catch (...) {}
                    
                    // Try to convert to double
                    if (!std::holds_alternative<ConfigValue>(value)) {
                        try {
                            size_t pos;
                            double double_val = std::stod(str, &pos);
                            if (pos == str.size()) {
                                value = double_val;
                            }
                        } catch (...) {}
                    }
                    
                    // Try to convert to string
                    if (!std::holds_alternative<ConfigValue>(value)) {
                        value = str;
                    }
                } else if (val.IsSequence()) {
                    // Convert YAML sequence to appropriate vector type
                    if (!val.empty()) {
                        const auto& first = val[0];
                        if (first.IsScalar()) {
                            const std::string& first_str = first.Scalar();
                            bool all_strings = true;
                            bool all_ints = true;
                            bool all_doubles = true;
                            
                            for (const auto& item : val) {
                                if (!item.IsScalar()) {
                                    all_strings = all_ints = all_doubles = false;
                                    break;
                                }
                                const std::string& item_str = item.Scalar();
                                
                                bool is_int = true;
                                try {
                                    size_t pos;
                                    int64_t test_int = std::stoll(item_str, &pos);
                                    if (pos != item_str.size()) is_int = false;
                                } catch (...) { is_int = false; }
                                if (is_int) all_ints = true; else all_ints = false;
                                
                                bool is_double = true;
                                try {
                                    size_t pos;
                                    double test_double = std::stod(item_str, &pos);
                                    if (pos != item_str.size()) is_double = false;
                                } catch (...) { is_double = false; }
                                if (is_double) all_doubles = true; else all_doubles = false;
                            }
                            
                            if (all_strings) {
                                std::vector<std::string> vec;
                                for (const auto& item : val) {
                                    vec.push_back(item.Scalar());
                                }
                                value = vec;
                            } else if (all_ints) {
                                std::vector<int64_t> vec;
                                for (const auto& item : val) {
                                    vec.push_back(std::stoll(item.Scalar()));
                                }
                                value = vec;
                            } else if (all_doubles) {
                                std::vector<double> vec;
                                for (const auto& item : val) {
                                    vec.push_back(std::stod(item.Scalar()));
                                }
                                value = vec;
                            } else {
                                // Default to string vector
                                std::vector<std::string> vec;
                                for (const auto& item : val) {
                                    vec.push_back(item.Scalar());
                                }
                                value = vec;
                            }
                        }
                    } else {
                        value = std::vector<std::string>{};
                    }
                }
                
                entries.emplace_back(key, value, ConfigSource::File);
            }
            
            return true;
        } catch (const std::exception& e) {
            // YAML parsing failed, return false
            return false;
        }
#else
        // yaml-cpp not available
        return false;
#endif
    }
    
    // Convert ConfigEntry vector to JSON string
    std::string entriesToJSON(const std::vector<ConfigEntry>& entries) {
#ifdef HAVE_NLOHMANN_JSON
        nlohmann::json json_obj;
        
        // Convert ConfigEntry vector to JSON
        for (const auto& entry : entries) {
            // Handle different value types
            std::visit([&json_obj, &entry](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    json_obj[entry.key] = val;
                } else if constexpr (std::is_same_v<T, double>) {
                    json_obj[entry.key] = val;
                } else if constexpr (std::is_same_v<T, bool>) {
                    json_obj[entry.key] = val;
                } else if constexpr (std::is_same_v<T, std::string>) {
                    json_obj[entry.key] = val;
                } else if constexpr (std::is_same_v<T, std::vector<int> || std::is_same_v<T, std::vector<int64_t>>) {
                    json_obj[entry.key] = val;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    json_obj[entry.key] = val;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    json_obj[entry.key] = val;
                }
            }, entry.value);
        }
        
        return json_obj.dump(2);
#else
        return "{\"error\": \"nlohmann::json not available\"}";
#endif
    }
    
    // Convert ConfigEntry vector to YAML string
    std::string entriesToYAML(const std::vector<ConfigEntry>& entries) {
#ifdef HAVE_YAML_CPP
        YAML::Emitter emitter;
        emitter << YAML::BeginMap;
        
        for (const auto& entry : entries) {
            emitter << YAML::Key << entry.key;
            
            std::visit([&emitter](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    emitter << (int64_t)val;
                } else if constexpr (std::is_same_v<T, double>) {
                    emitter << val;
                } else if constexpr (std::is_same_v<T, bool>) {
                    emitter << val;
                } else if constexpr (std::is_same_v<T, std::string>) {
                    emitter << val;
                } else if constexpr (std::is_same_v<T, std::vector<int> || std::is_same_v<T, std::vector<int64_t>>) {
                    emitter << YAML::BeginSeq;
                    for (const auto& item : val) {
                        emitter << (int64_t)item;
                    }
                    emitter << YAML::EndSeq;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    emitter << YAML::BeginSeq;
                    for (const auto& item : val) {
                        emitter << item;
                    }
                    emitter << YAML::EndSeq;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    emitter << YAML::BeginSeq;
                    for (const auto& item : val) {
                        emitter << item;
                    }
                    emitter << YAML::EndSeq;
                }
            }, entry.value);
        }
        
        emitter << YAML::EndMap;
        return emitter.c_str();
#else
        return "# error: yaml-cpp not available\n";
#endif
    }
    
    // Convert ConfigEntry vector to key=value format (preserves compatibility)
    std::string entriesToKeyValue(const std::vector<ConfigEntry>& entries) {
        std::ostringstream oss;
        
        for (const auto& entry : entries) {
            oss << "# " << entry.description << "\n";
            
            // Convert value to string based on type
            std::visit([&oss](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    oss << entry.key << " = " << val << "\n";
                } else if constexpr (std::is_same_v<T, double>) {
                    oss << entry.key << " = " << val << "\n";
                } else if constexpr (std::is_same_v<T, bool>) {
                    oss << entry.key << " = " << (val ? "true" : "false") << "\n";
                } else if constexpr (std::is_same_v<T, std::string>) {
                    oss << entry.key << " = \"" << val << "\"\n";
                } else if constexpr (std::is_same_v<T, std::vector<int> || std::is_same_v<T, std::vector<int64_t>>) {
                    oss << entry.key << " = ";
                    for (size_t i = 0; i < val.size(); ++i) {
                        if (i > 0) oss << ", ";
                        oss << val[i];
                    }
                    oss << "\n";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    oss << entry.key << " = ";
                    for (size_t i = 0; i < val.size(); ++i) {
                        if (i > 0) oss << ", ";
                        oss << val[i];
                    }
                    oss << "\n";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    oss << entry.key << " = ";
                    for (size_t i = 0; i < val.size(); ++i) {
                        if (i > 0) oss << ", ";
                        oss << "\"" << val[i] << "\"";
                    }
                    oss << "\n";
                }
            }, entry.value);
        }
        
        return oss.str();
    }
    
}} // anonymous namespace

// Update Config::Impl definition to include parser flags
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    bool has_json_support = false;
    bool has_yaml_support = false;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Detect available parsers
#ifdef HAVE_NLOHMANN_JSON
    pImpl->has_json_support = true;
#endif
#ifdef HAVE_YAML_CPP
    pImpl->has_yaml_support = true;
#endif
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    
    // Clear existing entries
    clear();
    
    // Determine file format by extension
    std::string ext = std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    // Try JSON format first if extension is .json
    if (ext == ".json") {
        if (pImpl->has_json_support) {
            if (tryParseJSON(content, pImpl->entries)) {
                return true;
            }
        }
    }
    
    // Try YAML format if extension is .yaml or .yml
    if (ext == ".yaml" || ext == ".yml") {
        if (pImpl->has_yaml_support) {
            if (tryParseYAML(content, pImpl->entries)) {
                return true;
            }
        }
    }
    
    // Fallback to simple key=value format for .cfg, .config, .properties, or unknown formats
    if (ext == ".cfg" || ext == ".config" || ext == ".properties" || 
        ext.empty() || (ext.size() > 1 && ext[0] == '.' && !std::isalnum(ext[1]))) {
        
        std::istringstream iss(content);
        std::string line;
        while (std::getline(iss, line)) {
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
    
    // Determine file format by extension
    std::string ext = std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    std::string content;
    
    // Save in appropriate format
    if (ext == ".json" && pImpl->has_json_support) {
        content = entriesToJSON(pImpl->entries);
    } else if ((ext == ".yaml" || ext == ".yml") && pImpl->has_yaml_support) {
        content = entriesToYAML(pImpl->entries);
    } else {
        // Default to key=value format for compatibility
        content = entriesToKeyValue(pImpl->entries);
    }
    
    file << content;
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
