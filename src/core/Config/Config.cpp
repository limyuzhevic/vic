#pragma once

#include "nlohmann/json.hpp"
#include "nlohmann/yaml.hpp"
#include <fstream>
#include <sstream>

namespace nlm {

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON first
    try {
        std::ifstream file(filepath);
        if (file.is_open()) {
            nlohmann::json jsonData = nlohmann::json::parse(file);
            for (const auto& [key, value] : jsonData.items()) {
                // Convert JSON value to ConfigValue
                if (value.is_string()) {
                    set(key, value.get<std::string>(), ConfigSource::File);
                } else if (value.is_number_integer()) {
                    set(key, value.get<int>(), ConfigSource::File);
                } else if (value.is_number()) {
                    set(key, value.get<double>(), ConfigSource::File);
                } else if (value.is_boolean()) {
                    set(key, value.get<bool>(), ConfigSource::File);
                } else if (value.is_array()) {
                    if (!value.empty() && value[0].is_number_integer()) {
                        std::vector<int> intArray;
                        for (const auto& item : value) {
                            intArray.push_back(item.get<int>());
                        }
                        set(key, intArray, ConfigSource::File);
                    } else if (!value.empty() && value[0].is_number()) {
                        std::vector<double> doubleArray;
                        for (const auto& item : value) {
                            doubleArray.push_back(item.get<double>());
                        }
                        set(key, doubleArray, ConfigSource::File);
                    } else {
                        std::vector<std::string> stringArray;
                        for (const auto& item : value) {
                            stringArray.push_back(item.get<std::string>());
                        }
                        set(key, stringArray, ConfigSource::File);
                    }
                }
            }
            return true;
        }
    } catch (const std::exception&) {}
    
    // Try YAML as fallback
    try {
        std::ifstream file(filepath);
        if (file.is_open()) {
            nlohmann::yaml::Node yamlData = nlohmann::yaml::Load(file);
            for (const auto& node : yamlData.items()) {
                std::string key = node.key();
                const auto& value = node.value();
                
                if (value.IsScalar()) {
                    std::string strValue = value.as<std::string>();
                    if (strValue == "true" || strValue == "false") {
                        set(key, strValue == "true", ConfigSource::File);
                    } else if (strValue.find('.') != std::string::npos || strValue.find('e') != std::string::npos) {
                        set(key, std::stod(strValue), ConfigSource::File);
                    } else {
                        try {
                            set(key, std::stoi(strValue), ConfigSource::File);
                        } catch (...) {
                            set(key, strValue, ConfigSource::File);
                        }
                    }
                } else if (value.IsSequence()) {
                    std::vector<double> array;
                    for (const auto& item : value) {
                        array.push_back(item.as<double>());
                    }
                    set(key, array, ConfigSource::File);
                }
            }
            return true;
        }
    } catch (const std::exception&) {}
    
    return false;
}

void Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return;
    }
    
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::string>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                jsonData[entry.key] = arg;
            }
        }, entry.value);
    }
    
    file << std::setw(2) << jsonData << std::endl;
}
