#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

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
    
    // Try to parse as JSON first (modern format)
    std::string firstChar = file.peek() != std::ifstream::traits_type::eof() ? 
        std::string(1, file.get()) : "";
    file.clear();
    file.seekg(0);
    
    if (firstChar == "{") {
        // Parse as JSON
        return loadFromJSON(filepath);
    }
    
    // Fall back to simple key=value format for backward compatibility
    std::string line;
    size_t lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
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
        } else {
            // Log warning about malformed line
            std::cerr << "WARNING: Config file " << filepath << 
                         " line " << lineNumber << 
                         ": malformed line, skipping: " << line << std::endl;
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
    
    // Determine file format based on extension
    std::string extension = "." + filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == ".json") {
        return saveToJSON(filepath);
    } else {
        // Fall back to simple key=value format for backward compatibility
        return saveToSimpleFormat(filepath);
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
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    rapidjson::IStreamWrapper isw(file);
    rapidjson::Document doc;
    
    if (doc.ParseStream(isw).HasParseError()) {
        std::cerr << "ERROR: Failed to parse JSON file: " << filepath << std::endl;
        return false;
    }
    
    if (!doc.IsObject()) {
        std::cerr << "ERROR: JSON document is not an object: " << filepath << std::endl;
        return false;
    }
    
    // Parse JSON object
    for (rapidjson::Value::ConstMemberIterator it = doc.MemberBegin(); it != doc.MemberEnd(); ++it) {
        std::string key = it->name.GetString();
        
        // Convert JSON value to ConfigValue
        ConfigValue configValue;
        bool success = convertJSONValueToConfigValue(it->value, configValue);
        
        if (success) {
            set(key, configValue, ConfigSource::File);
        } else {
            std::cerr << "WARNING: Failed to convert JSON value for key \"" << key << "\"" << std::endl;
        }
    }
    
    return true;
}

bool Config::saveToJSON(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    
    writer.StartObject();
    
    for (const auto& entry : pImpl->entries) {
        writer.Key(entry.key.c_str());
        writeConfigValueToJSON(writer, entry.value);
    }
    
    writer.EndObject();
    
    file << buffer.GetString();
    
    return true;
}

bool Config::convertJSONValueToConfigValue(const rapidjson::Value& jsonValue, ConfigValue& configValue) {
    if (jsonValue.IsInt()) {
        configValue = jsonValue.GetInt();
    } else if (jsonValue.IsUint64()) {
        configValue = static_cast<int64_t>(jsonValue.GetUint64());
    } else if (jsonValue.IsInt64()) {
        configValue = jsonValue.GetInt64();
    } else if (jsonValue.IsDouble()) {
        configValue = jsonValue.GetDouble();
    } else if (jsonValue.IsBool()) {
        configValue = jsonValue.GetBool();
    } else if (jsonValue.IsString()) {
        configValue = std::string(jsonValue.GetString());
    } else if (jsonValue.IsArray()) {
        // Parse array as vector of doubles (most flexible)
        std::vector<double> array;
        for (rapidjson::SizeType i = 0; i < jsonValue.Size(); ++i) {
            if (jsonValue[i].IsNumber()) {
                array.push_back(jsonValue[i].GetDouble());
            } else if (jsonValue[i].IsInt()) {
                array.push_back(static_cast<double>(jsonValue[i].GetInt()));
            }
        }
        configValue = array;
    } else {
        std::cerr << "WARNING: Unsupported JSON value type" << std::endl;
        return false;
    }
    
    return true;
}

void Config::writeConfigValueToJSON(rapidjson::Writer<rapidjson::StringBuffer>& writer, const ConfigValue& configValue) {
    std::visit([&writer](auto&& value) {
        using T = std::decay_t<decltype(value)>;
        
        if constexpr (std::is_same_v<T, int>) {
            writer.Int(value);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            writer.Int64(value);
        } else if constexpr (std::is_same_v<T, double>) {
            writer.Double(value);
        } else if constexpr (std::is_same_v<T, bool>) {
            writer.Bool(value);
        } else if constexpr (std::is_same_v<T, std::string>) {
            writer.String(value.c_str());
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            writer.StartArray();
            for (int item : value) {
                writer.Int(item);
            }
            writer.EndArray();
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            writer.StartArray();
            for (double item : value) {
                writer.Double(item);
            }
            writer.EndArray();
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            writer.StartArray();
            for (const std::string& item : value) {
                writer.String(item.c_str());
            }
            writer.EndArray();
        } else {
            writer.Null();
        }
    }, configValue);
}

bool Config::saveToSimpleFormat(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Write key with appropriate formatting
        file << entry.key << " = ";
        
        // Write value based on its type
        std::visit([&file](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            
            if constexpr (std::is_same_v<T, std::string>) {
                // Quote strings that contain spaces or special characters
                if (value.find(' ') != std::string::npos || value.find('=') != std::string::npos) {
                    file << "\"" << value << "\"";
                } else {
                    file << value;
                }
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (value ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "["; 
                for (size_t i = 0; i < value.size(); ++i) {
                    file << value[i];
                    if (i < value.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "["; 
                for (size_t i = 0; i < value.size(); ++i) {
                    file << value[i];
                    if (i < value.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "["; 
                for (size_t i = 0; i < value.size(); ++i) {
                    if (value[i].find(' ') != std::string::npos || value[i].find('=') != std::string::npos) {
                        file << "\"" << value[i] << "\"";
                    } else {
                        file << value[i];
                    }
                    if (i < value.size() - 1) file << ", ";
                }
                file << "]";
            } else {
                file << value;
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    } // namespace nlm
