// YAMLConfigParser implementation
#include "YAMLConfigParser.hpp"
#include "AdvancedConfig.hpp"
#include <yaml-cpp/yaml.h>

namespace nlm {

bool YAMLConfigParserImpl::parse(const std::string& yamlStr, Config& config) {
    try {
        YAML::Node yaml = YAML::Load(yamlStr);
        
        // Clear existing config
        config.clear();
        
        // Parse groups if present
        if (yaml["groups"]) {
            for (const auto& groupNode : yaml["groups"]) {
                std::string groupName = groupNode["name"].as<std::string>("unnamed");
                ConfigGroup group;
                group.name = groupName;
                
                if (groupNode["description"]) {
                    group.description = groupNode["description"].as<std::string>();
                }
                if (groupNode["priority"]) {
                    group.priority = groupNode["priority"].as<int>();
                }
                if (groupNode["isGlobal"]) {
                    group.isGlobal = groupNode["isGlobal"].as<bool>();
                }
                
                // Parse entries in this group
                if (groupNode["entries"]) {
                    for (const auto& entryNode : groupNode["entries"]) {
                        std::string key = entryNode["key"].as<std::string>("");
                        if (key.empty()) continue;
                        
                        ConfigEntry entry;
                        entry.key = key;
                        entry.source = ConfigSource::File;
                        entry.category = groupName;
                        
                        // Parse metadata
                        if (entryNode["description"]) {
                            entry.description = entryNode["description"].as<std::string>();
                        }
                        if (entryNode["unit"]) {
                            entry.unit = entryNode["unit"].as<std::string>();
                        }
                        if (entryNode["required"]) {
                            entry.required = entryNode["required"].as<bool>();
                        }
                        if (entryNode["readOnly"]) {
                            entry.readOnly = entryNode["readOnly"].as<bool>();
                        }
                        if (entryNode["tags"]) {
                            for (const auto& tagNode : entryNode["tags"]) {
                                entry.tags.push_back(tagNode.as<std::string>());
                            }
                        }
                        
                        // Parse value
                        parseYAMLValue(entryNode["value"], entry.value);
                        
                        group.entries[key] = entry;
                        config.pImpl->entries.push_back(entry);
                        config.pImpl->keyToGroup[key] = groupName;
                    }
                }
                
                config.addGroup(group);
            }
        }
        
        // Parse top-level entries (if no groups)
        if (yaml["entries"]) {
            for (const auto& entryNode : yaml["entries"]) {
                std::string key = entryNode["key"].as<std::string>("");
                if (key.empty()) continue;
                
                ConfigEntry entry;
                entry.key = key;
                entry.source = ConfigSource::File;
                
                parseYAMLValue(entryNode["value"], entry.value);
                config.pImpl->entries.push_back(entry);
            }
        }
        
        return true;
    } catch (const YAML::Exception& e) {
        return false;
    }
}

std::string YAMLConfigParserImpl::serialize(const Config& config, bool pretty) const {
    YAML::Emitter out;
    
    if (pretty) {
        out.SetIndent(2);
    } else {
        out.SetIndent(0);
    }
    
    out << YAML::BeginMap;
    
    // Add entries if present
    if (!config.pImpl->entries.empty()) {
        out << YAML::Key << "entries" << YAML::Value << YAML::BeginSeq;
        for (const auto& entry : config.pImpl->entries) {
            out << YAML::BeginMap;
            out << YAML::Key << "key" << YAML::Value << entry.key;
            out << YAML::Key << "value";
            serializeYAMLValue(out, entry.value);
            
            if (!entry.description.empty()) {
                out << YAML::Key << "description" << YAML::Value << entry.description;
            }
            if (!entry.category.empty()) {
                out << YAML::Key << "category" << YAML::Value << entry.category;
            }
            if (!entry.unit.empty()) {
                out << YAML::Key << "unit" << YAML::Value << entry.unit;
            }
            if (entry.required) {
                out << YAML::Key << "required" << YAML::Value << entry.required;
            }
            if (entry.readOnly) {
                out << YAML::Key << "readOnly" << YAML::Value << entry.readOnly;
            }
            if (!entry.tags.empty()) {
                out << YAML::Key << "tags" << YAML::Value << YAML::BeginSeq;
                for (const auto& tag : entry.tags) {
                    out << tag;
                }
                out << YAML::EndSeq;
            }
            
            out << YAML::EndMap;
        }
        out << YAML::EndSeq;
    }
    
    // Add groups
    if (!config.pImpl->groups.empty()) {
        out << YAML::Key << "groups" << YAML::Value << YAML::BeginMap;
        for (const auto& [groupName, group] : config.pImpl->groups) {
            out << YAML::Key << groupName << YAML::Value << YAML::BeginMap;
            out << YAML::Key << "name" << YAML::Value << groupName;
            out << YAML::Key << "description" << YAML::Value << group.description;
            out << YAML::Key << "priority" << YAML::Value << group.priority;
            out << YAML::Key << "isGlobal" << YAML::Value << group.isGlobal;
            
            if (!group.entries.empty()) {
                out << YAML::Key << "entries" << YAML::Value << YAML::BeginMap;
                for (const auto& [key, entry] : group.entries) {
                    out << YAML::Key << key << YAML::Value << YAML::BeginMap;
                    serializeYAMLValue(out, entry.value);
                    out << YAML::EndMap;
                }
                out << YAML::EndMap;
            }
            
            out << YAML::EndMap;
        }
        out << YAML::EndMap;
    }
    
    out << YAML::EndMap;
    
    return out.c_str();
}

void YAMLConfigParserImpl::parseYAMLValue(const YAML::Node& node, ConfigValue& value) {
    if (!node) {
        value = std::string();
        return;
    }
    
    if (node.IsBoolean()) {
        value = node.as<bool>();
    } else if (node.IsInt()) {
        value = node.as<int>();
    } else if (node.IsDouble()) {
        value = node.as<double>();
    } else if (node.IsScalar()) {
        value = node.as<std::string>();
    } else if (node.IsSequence()) {
        std::vector<std::any> vec;
        for (const auto& item : node) {
            if (item.IsBoolean()) {
                vec.push_back(item.as<bool>());
            } else if (item.IsInt()) {
                vec.push_back(item.as<int>());
            } else if (item.IsDouble()) {
                vec.push_back(item.as<double>());
            } else if (item.IsScalar()) {
                vec.push_back(item.as<std::string>());
            }
        }
        value = vec;
    } else if (node.IsMap()) {
        // For nested objects, store as YAML string
        std::stringstream ss;
        ss << node;
        value = ss.str();
    } else {
        value = std::string();
    }
}

void YAMLConfigParserImpl::serializeYAMLValue(YAML::Emitter& out, const ConfigValue& value) {
    std::visit([&out](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            out << arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            out << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            out << arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            out << arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            out << arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            out << YAML::Flow << arg;
        } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
            out << YAML::Flow << arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            out << YAML::Flow << arg;
        } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
            out << YAML::Flow << arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            out << YAML::Flow << arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::any>>) {
            out << YAML::Flow << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) out << ", ";
                std::visit([&out](auto&& elem) {
                    using ET = std::decay_t<decltype(elem)>;
                    if constexpr (std::is_same_v<ET, int>) {
                        out << elem;
                    } else if constexpr (std::is_same_v<ET, double>) {
                        out << elem;
                    } else if constexpr (std::is_same_v<ET, bool>) {
                        out << elem;
                    } else if constexpr (std::is_same_v<ET, std::string>) {
                        out << elem;
                    }
                }, arg[i]);
            }
            out << "]";
        }
    }, value);
}

} // namespace nlm