#include "Config.hpp"
#include "ConfigSchema.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <memory>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
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
        // Check file extension to determine format
        std::string ext = "." + std::string(std::filesystem::path(filepath).extension().string());
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        // Try JSON first
        if (ext == ".json" || ext == ".cfg") {
            return loadFromJSON(filepath);
        }
        // Try YAML
        else if (ext == ".yaml" || ext == ".yml") {
            return loadFromYAML(filepath);
        }
        // Default to simple key=value format
        else {
            return loadFromSimpleFormat(filepath);
        }
    }

    bool Config::loadFromJSON(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string jsonContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        
        rapidjson::Document document;
        document.Parse(jsonContent.c_str());
        
        if (document.HasParseError()) {
            NLM_LOG_WARNING("Failed to parse JSON file: " + filepath);
            return false;
        }
        
        if (!document.IsObject()) {
            NLM_LOG_WARNING("JSON file is not an object: " + filepath);
            return false;
        }
        
        // Clear existing entries
        clear();
        
        // Parse JSON object
        for (rapidjson::Value::ConstMemberIterator itr = document.MemberBegin(); 
             itr != document.MemberEnd(); ++itr) {
            std::string key = itr->name.GetString();
            
            if (itr->value.IsString()) {
                set(key, itr->value.GetString(), ConfigSource::File);
            } else if (itr->value.IsInt()) {
                set(key, itr->value.GetInt(), ConfigSource::File);
            } else if (itr->value.IsInt64()) {
                set(key, itr->value.GetInt64(), ConfigSource::File);
            } else if (itr->value.IsUint()) {
                set(key, static_cast<int64_t>(itr->value.GetUint()), ConfigSource::File);
            } else if (itr->value.IsDouble()) {
                set(key, itr->value.GetDouble(), ConfigSource::File);
            } else if (itr->value.IsBool()) {
                set(key, itr->value.GetBool(), ConfigSource::File);
            } else if (itr->value.IsArray()) {
                // Parse array - for now as vector<string>
                std::vector<std::string> vec;
                for (rapidjson::SizeType i = 0; i < itr->value.Size(); ++i) {
                    if (itr->value[i].IsString()) {
                        vec.push_back(itr->value[i].GetString());
                    } else if (itr->value[i].IsInt()) {
                        vec.push_back(std::to_string(itr->value[i].GetInt()));
                    } else if (itr->value[i].IsDouble()) {
                        vec.push_back(std::to_string(itr->value[i].GetDouble()));
                    } else {
                        vec.push_back("");
                    }
                }
                // Convert to appropriate type based on content
                bool allInts = true;
                bool allDoubles = true;
                bool allStrings = true;
                
                for (const auto& v : vec) {
                    try {
                        std::stoi(v);
                    } catch (...) {
                        allInts = false;
                    }
                    try {
                        std::stod(v);
                    } catch (...) {
                        allDoubles = false;
                    }
                    allStrings = allStrings && (v.empty() || std::isalpha(v[0]));
                }
                
                if (allInts) {
                    std::vector<int> intVec;
                    for (const auto& v : vec) {
                        intVec.push_back(std::stoi(v));
                    }
                    set(key, intVec, ConfigSource::File);
                } else if (allDoubles) {
                    std::vector<double> doubleVec;
                    for (const auto& v : vec) {
                        doubleVec.push_back(std::stod(v));
                    }
                    set(key, doubleVec, ConfigSource::File);
                } else {
                    set(key, vec, ConfigSource::File);
                }
            } else {
                // Unknown type, skip
                NLM_LOG_WARNING("Skipping JSON field " + key + " with unsupported type");
            }
        }
        
        return true;
    }

    bool Config::loadFromYAML(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        try {
            YAML::Node doc = YAML::LoadFile(filepath);
            
            // Clear existing entries
            clear();
            
            // Parse YAML document
            if (doc.IsMap()) {
                for (const auto& it : doc) {
                    std::string key = it.first.as<std::string>();
                    
                    // Determine type and parse accordingly
                    if (it.second.IsScalar()) {
                        std::string value = it.second.as<std::string>();
                        set(key, value, ConfigSource::File);
                    } else if (it.second.IsSequence()) {
                        // Parse sequence - determine element type
                        std::vector<std::string> vec;
                        for (const auto& item : it.second) {
                            if (item.IsScalar()) {
                                vec.push_back(item.as<std::string>());
                            } else {
                                // Try to get as other types
                                try {
                                    if (item.IsInt()) {
                                        vec.push_back(std::to_string(item.as<int>()));
                                    } else if (item.IsDouble()) {
                                        vec.push_back(std::to_string(item.as<double>()));
                                    } else {
                                        vec.push_back("?");
                                    }
                                } catch (...) {
                                    vec.push_back("?");
                                }
                            }
                        }
                        
                        // Convert to appropriate type
                        bool allInts = true;
                        bool allDoubles = true;
                        
                        for (const auto& v : vec) {
                            if (v == "?") {
                                allInts = false;
                                allDoubles = false;
                                continue;
                            }
                            try {
                                std::stoi(v);
                            } catch (...) {
                                allInts = false;
                            }
                            try {
                                std::stod(v);
                            } catch (...) {
                                allDoubles = false;
                            }
                        }
                        
                        if (allInts) {
                            std::vector<int> intVec;
                            for (const auto& v : vec) {
                                if (v != "?") {
                                    intVec.push_back(std::stoi(v));
                                } else {
                                    intVec.push_back(0);
                                }
                            }
                            set(key, intVec, ConfigSource::File);
                        } else if (allDoubles) {
                            std::vector<double> doubleVec;
                            for (const auto& v : vec) {
                                if (v != "?") {
                                    doubleVec.push_back(std::stod(v));
                                } else {
                                    doubleVec.push_back(0.0);
                                }
                            }
                            set(key, doubleVec, ConfigSource::File);
                        } else {
                            set(key, vec, ConfigSource::File);
                        }
                    } else if (it.second.IsMap()) {
                        // Nested object - for now skip
                        NLM_LOG_WARNING("Skipping nested YAML object for key: " + key);
                    } else {
                        NLM_LOG_WARNING("Skipping YAML field " + key + " with unsupported type");
                    }
                }
            } else {
                NLM_LOG_WARNING("YAML file is not a map: " + filepath);
                return false;
            }
        } catch (const YAML::Exception& e) {
            NLM_LOG_ERROR("YAML parsing error in " + filepath + ": " + std::string(e.what()));
            return false;
        }
        
        return true;
    }

    bool Config::loadFromSimpleFormat(const std::string& filepath) {
        // Original simple key=value format implementation
        std::ifstream file(filepath);
        if (!file.is_open()) {
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
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
