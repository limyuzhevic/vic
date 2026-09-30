#include <stdexcept>
#include <fstream>
#include <sstream>
#include <json/json.h>

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        Json::Value root;
        Json::CharReaderBuilder readerBuilder;
        std::string errors;
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        
        bool parseSuccess = Json::parseFromStream(readerBuilder, buffer, root, &errors);
        if (!parseSuccess) {
            return false;
        }
        
        // Clear existing config
        clear();
        
        // Parse JSON and load into config
        parseJson(root);
        
    } catch (const std::exception& e) {
        return false;
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (!pImpl) return false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.find("=" ) != std::string::npos) {
            size_t eqPos = arg.find("=");
            std::string key = arg.substr(0, eqPos);
            std::string value = arg.substr(eqPos + 1);
            
            // Try to parse value as different types
            bool success = false;
            
            try {
                // Try int
                int intVal = std::stoi(value);
                set(key, intVal, ConfigSource::CommandLine);
                success = true;
            } catch (...) {}
            
            try {
                // Try double
                double doubleVal = std::stod(value);
                set(key, doubleVal, ConfigSource::CommandLine);
                success = true;
            } catch (...) {}
            
            try {
                // Try bool
                if (value == "true" || value == "1") {
                    set(key, true, ConfigSource::CommandLine);
                    success = true;
                } else if (value == "false" || value == "0") {
                    set(key, false, ConfigSource::CommandLine);
                    success = true;
                }
            } catch (...) {}
            
            if (!success) {
                // Treat as string
                set(key, value, ConfigSource::CommandLine);
            }
        }
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    if (!pImpl) return false;
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        Json::Value root;
        serializeJson(root);
        
        Json::StreamWriterBuilder writerBuilder;
        writerBuilder["indentation"] = "    ";
        std::unique_ptr<Json::StreamWriter> writer(writerBuilder.newStreamWriter());
        writer->write(root, &file);
        
    } catch (const std::exception& e) {
        return false;
    }
    
    return true;
}

void Config::parseJson(const Json::Value& root) {
    if (!pImpl) return;
    
    if (root.isObject()) {
        for (auto it = root.begin(); it != root.end(); ++it) {
            std::string key = it.name();
            const Json::Value& value = *it;
            
            setValue(key, value, ConfigSource::File);
        }
    }
}

void Config::serializeJson(Json::Value& root) const {
    if (!pImpl) return;
    
    // Serialize all config entries
    for (const auto& entry : pImpl->entries) {
        setJsonValue(root[entry.key], entry.value);
    }
}

void Config::setValue(const std::string& key, const Json::Value& value, ConfigSource source) {
    ConfigValue configValue;
    
    if (value.isInt()) {
        configValue = value.asInt();
    } else if (value.isUInt()) {
        configValue = static_cast<int64_t>(value.asUInt64());
    } else if (value.isReal()) {
        configValue = value.asDouble();
    } else if (value.isBool()) {
        configValue = value.asBool();
    } else if (value.isString()) {
        configValue = value.asString();
    } else if (value.isArray()) {
        if (value.size() > 0 && value[0].isInt()) {
            std::vector<int> intArray;
            for (const auto& item : value) {
                intArray.push_back(item.asInt());
            }
            configValue = intArray;
        } else if (value.size() > 0 && value[0].isReal()) {
            std::vector<double> doubleArray;
            for (const auto& item : value) {
                doubleArray.push_back(item.asDouble());
            }
            configValue = doubleArray;
        } else {
            std::vector<std::string> stringArray;
            for (const auto& item : value) {
                stringArray.push_back(item.asString());
            }
            configValue = stringArray;
        }
    }
    
    set(key, configValue, source);
}

void Config::setJsonValue(Json::Value& jsonValue, const ConfigValue& configValue) {
    std::visit([&](auto&& value) {
        using T = std::decay_t<decltype(value)>;
        if constexpr (std::is_same_v<T, int>) {
            jsonValue = value;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            jsonValue = static_cast<Json::Int64>(value);
        } else if constexpr (std::is_same_v<T, double>) {
            jsonValue = value;
        } else if constexpr (std::is_same_v<T, bool>) {
            jsonValue = value;
        } else if constexpr (std::is_same_v<T, std::string>) {
            jsonValue = value;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            for (int item : value) {
                jsonValue.append(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            for (double item : value) {
                jsonValue.append(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            for (const std::string& item : value) {
                jsonValue.append(item);
            }
        }
    }, configValue);
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    size_t end = str.find_last_not_of(" \t\n\r");
    
    if (start == std::string::npos) return "";
    if (end == std::string::npos) return "";
    
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    for (char& c : result) {
        c = std::tolower(static_cast<unsigned char>(c));
    }
    return result;
}
