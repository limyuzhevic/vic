#include "ConfigGroup.hpp"
#include <sstream>
#include <algorithm>

namespace nlm {

void ConfigGroup::addGroup(const GroupInfo& group) {
    // Check if group already exists
    if (!hasGroup(group.name)) {
        groups.push_back(group);
    }
}

void ConfigGroup::addField(const std::string& fieldName, const std::any& value,
                           const std::string& description, bool editable) {
    // Find or create group based on field name prefix
    std::string groupName = "default";
    size_t dotPos = fieldName.find('.');
    if (dotPos != std::string::npos) {
        groupName = fieldName.substr(0, dotPos);
    }
    
    // Ensure group exists
    if (!hasGroup(groupName)) {
        GroupInfo group(groupName, "Automatically created group");
        addGroup(group);
    }
    
    fields[groupName][fieldName] = value;
}

const ConfigGroup::GroupInfo* ConfigGroup::getGroupInfo(const std::string& groupName) const {
    auto it = std::find_if(groups.begin(), groups.end(),
                          [&groupName](const GroupInfo& g) { return g.name == groupName; });
    if (it != groups.end()) {
        return &(*it);
    }
    return nullptr;
}

ConfigGroup::GroupInfo* ConfigGroup::getGroupInfo(const std::string& groupName) {
    auto it = std::find_if(groups.begin(), groups.end(),
                          [&groupName](const GroupInfo& g) { return g.name == groupName; });
    if (it != groups.end()) {
        return &(*it);
    }
    return nullptr;
}

const std::any* ConfigGroup::getField(const std::string& fieldName) const {
    // Find group by field name
    for (const auto& groupEntry : fields) {
        const auto& fieldMap = groupEntry.second;
        auto it = fieldMap.find(fieldName);
        if (it != fieldMap.end()) {
            return &it->second;
        }
    }
    return nullptr;
}

std::any* ConfigGroup::getField(const std::string& fieldName) {
    // Find group by field name
    for (auto& groupEntry : fields) {
        auto& fieldMap = groupEntry.second;
        auto it = fieldMap.find(fieldName);
        if (it != fieldMap.end()) {
            return &it->second;
        }
    }
    return nullptr;
}

std::vector<std::string> ConfigGroup::getFieldNames() const {
    std::vector<std::string> allFieldNames;
    for (const auto& groupEntry : fields) {
        const auto& fieldMap = groupEntry.second;
        for (const auto& fieldEntry : fieldMap) {
            allFieldNames.push_back(fieldEntry.first);
        }
    }
    return allFieldNames;
}

std::vector<std::string> ConfigGroup::getFieldNamesInGroup(const std::string& groupName) const {
    std::vector<std::string> fieldNames;
    auto it = fields.find(groupName);
    if (it != fields.end()) {
        const auto& fieldMap = it->second;
        for (const auto& fieldEntry : fieldMap) {
            fieldNames.push_back(fieldEntry.first);
        }
    }
    return fieldNames;
}

std::vector<std::pair<std::string, std::any>> ConfigGroup::getFieldsInGroup(const std::string& groupName) const {
    std::vector<std::pair<std::string, std::any>> fieldValues;
    auto it = fields.find(groupName);
    if (it != fields.end()) {
        const auto& fieldMap = it->second;
        for (const auto& fieldEntry : fieldMap) {
            fieldValues.emplace_back(fieldEntry.first, fieldEntry.second);
        }
    }
    return fieldValues;
}

bool ConfigGroup::hasGroup(const std::string& groupName) const {
    return std::any_of(groups.begin(), groups.end(),
                      [&groupName](const GroupInfo& g) { return g.name == groupName; });
}

bool ConfigGroup::hasField(const std::string& fieldName) const {
    for (const auto& groupEntry : fields) {
        const auto& fieldMap = groupEntry.second;
        if (fieldMap.find(fieldName) != fieldMap.end()) {
            return true;
        }
    }
    return false;
}

void ConfigGroup::removeField(const std::string& fieldName) {
    for (auto& groupEntry : fields) {
        auto& fieldMap = groupEntry.second;
        fieldMap.erase(fieldName);
    }
}

std::string ConfigGroup::summary() const {
    std::ostringstream oss;
    oss << "Configuration Groups (" << groups.size() << " groups):\n";
    
    for (const auto& group : groups) {
        oss << "  Group: " << group.name << " (priority: " << group.priority << ")\n";
        
        bool expanded = groupExpanded.find(group.name) != groupExpanded.end() ? groupExpanded.at(group.name) : true;
        if (expanded) {
            oss << "    Fields:\n";
            auto fieldNames = getFieldNamesInGroup(group.name);
            for (const auto& fieldName : fieldNames) {
                oss << "      - " << fieldName << "\n";
            }
        }
    }
    
    return oss.str();
}

void ConfigGroup::setGroupExpanded(const std::string& groupName, bool expanded) {
    groupExpanded[groupName] = expanded;
}

bool ConfigGroup::isGroupExpanded(const std::string& groupName) const {
    auto it = groupExpanded.find(groupName);
    return it != groupExpanded.end() ? it->second : true;
}

} // namespace nlm