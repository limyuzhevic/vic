#include "Config.hpp"
#include "Types.hpp"  // For error handling
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <yaml-cpp/yaml.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON first (recommended format)
    if (loadFromJSON(filepath)) {
        NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath);
        return true;
    }
    
    // Try YAML (if available)
    if (loadFromYAML(filepath)) {
        NLM_LOG_INFO("Loaded configuration from YAML file: " + filepath);
        return true;
    }
    
    // Fallback to simple key=value format for backward compatibility
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open configuration file: " + filepath);
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
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    NLM_LOG_INFO("Loaded configuration from simple format file: " + filepath);
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
    // Save in JSON format (recommended)
    if (saveToJSON(filepath)) {
        NLM_LOG_INFO("Saved configuration to JSON file: " + filepath);
        return true;
    }
    
    return false;
}

// JSON parsing implementation
bool Config::loadFromJSON(const std::string& filepath) {
#ifdef HAVE_RAPIDJSON
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    rapidjson::IStreamWrapper isw(file);
    rapidjson::Document doc;
    
    if (doc.ParseStream(isw).HasParseError()) {
        return false;
    }
    
    if (!doc.IsObject()) {
        return false;
    }
    
    // Clear existing entries
    clear();
    
    // Parse JSON object
    for (rapidjson::Document::ConstMemberIterator itr = doc.MemberBegin(); 
         itr != doc.MemberEnd(); ++itr) {
        const std::string key = itr->name.GetString();
        const rapidjson::Value& value = itr->value;
        
        std::string valueStr;
        
        if (value.IsString()) {
            valueStr = value.GetString();
            set(key, valueStr, ConfigSource::File);
        } else if (value.IsInt()) {
            int intVal = value.GetInt();
            set(key, intVal, ConfigSource::File);
        } else if (value.IsInt64()) {
            int64_t int64Val = value.GetInt64();
            set(key, int64Val, ConfigSource::File);
        } else if (value.IsUint()) {
            uint64_t uintVal = value.GetUint();
            set(key, static_cast<int64_t>(uintVal), ConfigSource::File);
        } else if (value.IsDouble()) {
            double doubleVal = value.GetDouble();
            set(key, doubleVal, ConfigSource::File);
        } else if (value.IsBool()) {
            bool boolVal = value.GetBool();
            set(key, boolVal, ConfigSource::File);
        } else if (value.IsArray()) {
            // Convert array to string representation
            valueStr = "[";
            for (rapidjson::SizeType i = 0; i < value.Size(); ++i) {
                if (i > 0) valueStr += ",";
                if (value[i].IsString()) {
                    valueStr += "\"" + std::string(value[i].GetString()) + "\"";
                } else {
                    valueStr += std::to_string(value[i].GetDouble());
                }
            }
            valueStr += "]";
            set(key, valueStr, ConfigSource::File);
        }
    }
    
    return true;
#else
    return false; // rapidjson not available
#endif
}

// YAML parsing implementation
bool Config::loadFromYAML(const std::string& filepath) {
#ifdef HAVE_YAML_CPP
    try {
        YAML::Node doc = YAML::LoadFile(filepath);
        
        // Clear existing entries
        clear();
        
        // Parse YAML node
        for (const auto& node : doc) {
            const std::string key = node.first.as<std::string>();
            const YAML::Node& value = node.second;
            
            std::string valueStr;
            
            if (value.IsScalar()) {
                if (value.IsSequence()) {
                    // Convert sequence to string representation
                    valueStr = "[";
                    bool first = true;
                    for (const auto& item : value) {
                        if (!first) valueStr += ",";
                        valueStr += item.as<std::string>();
                        first = false;
                    }
                    valueStr += "]";
                    set(key, valueStr, ConfigSource::File);
                } else {
                    // Try to determine type
                    try {
                        int intVal = value.as<int>();
                        set(key, intVal, ConfigSource::File);
                    } catch (...) {
                        try {
                            double doubleVal = value.as<double>();
                            set(key, doubleVal, ConfigSource::File);
                        } catch (...) {
                            try {
                                bool boolVal = value.as<bool>();
                                set(key, boolVal, ConfigSource::File);
                            } catch (...) {
                                valueStr = value.as<std::string>();
                                set(key, valueStr, ConfigSource::File);
                            }
                        }
                    }
                }
            }
        }
        
        return true;
    } catch (const YAML::Exception&) {
        return false;
    }
#else
    return false; // yaml-cpp not available
#endif
}

// JSON saving implementation
bool Config::saveToJSON(const std::string& filepath) const {
#ifdef HAVE_RAPIDJSON
    rapidjson::Document doc(rapidjson::kObjectType);
    doc.SetObject();
    
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
    
    // Add configuration entries to JSON
    for (const auto& entry : pImpl->entries) {
        rapidjson::Value keyStr(keyStr.data(), keyStr.size(), allocator);
        
        rapidjson::Value valueStr;
        if (std::holds_alternative<std::string>(entry.value)) {
            valueStr.SetString(std::get<std::string>(entry.value).c_str(), allocator);
        } else if (std::holds_alternative<int>(entry.value)) {
            valueStr.SetInt(std::get<int>(entry.value));
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            valueStr.SetInt64(std::get<int64_t>(entry.value));
        } else if (std::holds_alternative<double>(entry.value)) {
            valueStr.SetDouble(std::get<double>(entry.value));
        } else if (std::holds_alternative<bool>(entry.value)) {
            valueStr.SetBool(std::get<bool>(entry.value));
        }
        
        doc.AddMember(keyStr, valueStr, allocator);
    }
    
    // Write to file
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    rapidjson::OStreamWrapper osw(file);
    rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
    doc.Accept(writer);
    
    return true;
#else
    return false; // rapidjson not available
#endif
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
