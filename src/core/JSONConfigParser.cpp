// JSONConfigParser implementation
#include "JSONConfigParser.hpp"
#include "AdvancedConfig.hpp"
#include <nlohmann/json.hpp>

namespace nlm {

bool JSONConfigParserImpl::parse(const std::string& jsonStr, Config& config) {
    try {
        auto json = nlohmann::json::parse(jsonStr);
        
        // Clear existing config
        config.clear();
        
        // Parse groups if present
        if (json.contains("groups")) {
            for (const auto& [groupName, groupData] : json["groups"].items()) {
                ConfigGroup group;
                group.name = groupName;
                
                if (groupData.contains("description")) {
                    group.description = groupData["description"].get<std::string>();
                }
                if (groupData.contains("priority")) {
                    group.priority = groupData["priority"].get<int>();
                }
                if (groupData.contains("isGlobal")) {
                    group.isGlobal = groupData["isGlobal"].get<bool>();
                }
                
                // Parse entries in this group
                if (groupData.contains("entries")) {
                    for (const auto& [key, value] : groupData["entries"].items()) {
                        ConfigEntry entry;
                        entry.key = key;
                        entry.source = ConfigSource::File;
                        entry.category = groupName;
                        
                        // Parse metadata
                        if (value.contains("description")) {
                            entry.description = value["description"].get<std::string>();
                        }
                        if (value.contains("unit")) {
                            entry.unit = value["unit"].get<std::string>();
                        }
                        if (value.contains("required")) {
                            entry.required = value["required"].get<bool>();
                        }
                        if (value.contains("readOnly")) {
                            entry.readOnly = value["readOnly"].get<bool>();
                        }
                        if (value.contains("tags")) {
                            for (const auto& tag : value["tags"]) {
                                entry.tags.push_back(tag.get<std::string>());
                            }
                        }
                        
                        // Parse value based on type
                        parseJSONValue(value, entry.value);
                        
                        config.addGroup(group);
                        config.pImpl->entries.push_back(entry);
                        config.pImpl->keyToGroup[key] = groupName;
                    }
                }
            }
        }
        
        // Parse top-level entries (if no groups)
        if (json.contains("entries")) {
            for (const auto& [key, value] : json["entries"].items()) {
                ConfigEntry entry;
                entry.key = key;
                entry.source = ConfigSource::File;
                
                parseJSONValue(value, entry.value);
                config.pImpl->entries.push_back(entry);
            }
        }
        
        return true;
    } catch (const nlohmann::json::exception& e) {
        return false;
    }
}

std::string JSONConfigParserImpl::serialize(const Config& config, bool pretty) const {
    nlohmann::json json;
    
    // Add entries
    nlohmann::json entriesJson;
    for (const auto& entry : config.pImpl->entries) {
        nlohmann::json entryJson;
        
        serializeJSONValue(entryJson, entry.value);
        
        if (!entry.description.empty()) {
            entryJson["description"] = entry.description;
        }
        if (!entry.category.empty()) {
            entryJson["category"] = entry.category;
        }
        if (!entry.unit.empty()) {
            entryJson["unit"] = entry.unit;
        }
        if (entry.required) {
            entryJson["required"] = entry.required;
        }
        if (entry.readOnly) {
            entryJson["readOnly"] = entry.readOnly;
        }
        if (!entry.tags.empty()) {
            nlohmann::json tagsJson;
            for (const auto& tag : entry.tags) {
                tagsJson.push_back(tag);
            }
            entryJson["tags"] = tagsJson;
        }
        
        entriesJson[entry.key] = entryJson;
    }
    
    if (!config.pImpl->entries.empty()) {
        json["entries"] = entriesJson;
    }
    
    // Add groups
    if (!config.pImpl->groups.empty()) {
        nlohmann::json groupsJson;
        for (const auto& [groupName, group] : config.pImpl->groups) {
            nlohmann::json groupJson;
            groupJson["description"] = group.description;
            groupJson["priority"] = group.priority;
            groupJson["isGlobal"] = group.isGlobal;
            
            nlohmann::json entriesJson;
            for (const auto& [key, entry] : group.entries) {
                nlohmann::json entryJson;
                serializeJSONValue(entryJson, entry.value);
                entriesJson[key] = entryJson;
            }
            groupJson["entries"] = entriesJson;
            
            groupsJson[groupName] = groupJson;
        }
        json["groups"] = groupsJson;
    }
    
    if (pretty) {
        return json.dump(4);
    } else {
        return json.dump();
    }
}

void JSONConfigParserImpl::parseJSONValue(const nlohmann::json& json, ConfigValue& value) {
    if (json.is_boolean()) {
        value = json.get<bool>();
    } else if (json.is_number_integer()) {
        value = json.get<int>();
    } else if (json.is_number()) {
        // Try double first, then int if it's actually an integer
        double doubleVal = json.get<double>();
        if (std::floor(doubleVal) == doubleVal) {
            value = static_cast<int>(doubleVal);
        } else {
            value = doubleVal;
        }
    } else if (json.is_string()) {
        value = json.get<std::string>();
    } else if (json.is_array()) {
        std::vector<std::any> vec;
        for (const auto& item : json) {
            if (item.is_boolean()) {
                vec.push_back(item.get<bool>());
            } else if (item.is_number_integer()) {
                vec.push_back(item.get<int>());
            } else if (item.is_number()) {
                vec.push_back(item.get<double>());
            } else if (item.is_string()) {
                vec.push_back(item.get<std::string>());
            } else {
                vec.push_back(item.get<std::string>()); // Fallback
            }
        }
        value = vec;
    } else if (json.is_object()) {
        // For nested objects, store as string representation
        value = json.dump();
    } else {
        value = "";
    }
}

void JSONConfigParserImpl::serializeJSONValue(nlohmann::json& json, const ConfigValue& value) {
    std::visit([&json](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            nlohmann::json arrayJson;
            for (const auto& item : arg) {
                arrayJson.push_back(item);
            }
            json = arrayJson;
        } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
            nlohmann::json arrayJson;
            for (const auto& item : arg) {
                arrayJson.push_back(item);
            }
            json = arrayJson;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            nlohmann::json arrayJson;
            for (const auto& item : arg) {
                arrayJson.push_back(item);
            }
            json = arrayJson;
        } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
            nlohmann::json arrayJson;
            for (const auto& item : arg) {
                arrayJson.push_back(item);
            }
            json = arrayJson;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            nlohmann::json arrayJson;
            for (const auto& item : arg) {
                arrayJson.push_back(item);
            }
            json = arrayJson;
        } else if constexpr (std::is_same_v<T, std::vector<std::any>>) {
            nlohmann::json arrayJson;
            for (const auto& item : arg) {
                std::visit([&arrayJson](auto&& elem) {
                    using ET = std::decay_t<decltype(elem)>;
                    if constexpr (std::is_same_v<ET, int>) {
                        arrayJson.push_back(elem);
                    } else if constexpr (std::is_same_v<ET, double>) {
                        arrayJson.push_back(elem);
                    } else if constexpr (std::is_same_v<ET, bool>) {
                        arrayJson.push_back(elem);
                    } else if constexpr (std::is_same_v<ET, std::string>) {
                        arrayJson.push_back(elem);
                    }
                }, item);
            }
            json = arrayJson;
        }
    }, value);
}

} // namespace nlm