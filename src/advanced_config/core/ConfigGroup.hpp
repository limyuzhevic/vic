#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <any>
#include <functional>
#include "Config.hpp"

namespace nlm {

// Configuration group for organizing related settings
class ConfigGroup {
public:
    struct GroupInfo {
        std::string name;
        std::string description;
        int priority;
        std::vector<std::string> parentGroups;
        bool expandable;
        
        GroupInfo() : name(), description(), priority(0), expandable(true) {}
        GroupInfo(const std::string& name, const std::string& description = "")
            : name(name), description(description), priority(0), expandable(true) {}
    };

    ConfigGroup() = default;
    ~ConfigGroup() = default;

    // Add a group
    void addGroup(const GroupInfo& group);

    // Add a configuration field to the group
    void addField(const std::string& fieldName, const std::any& value,
                  const std::string& description = "", bool editable = true);

    // Get group info
    const GroupInfo* getGroupInfo(const std::string& groupName) const;
    GroupInfo* getGroupInfo(const std::string& groupName);

    // Get all groups
    const std::vector<GroupInfo>& getGroups() const { return groups; }
    std::vector<GroupInfo>& getGroups() { return groups; }

    // Get field by name
    const std::any* getField(const std::string& fieldName) const;
    std::any* getField(const std::string& fieldName);

    // Get all field names
    std::vector<std::string> getFieldNames() const;

    // Get field names by group
    std::vector<std::string> getFieldNamesInGroup(const std::string& groupName) const;

    // Get fields by group
    std::vector<std::pair<std::string, std::any>> getFieldsInGroup(const std::string& groupName) const;

    // Check if group exists
    bool hasGroup(const std::string& groupName) const;

    // Check if field exists
    bool hasField(const std::string& fieldName) const;

    // Remove field
    void removeField(const std::string& fieldName);

    // Clear all fields
    void clearFields() { fields.clear(); }

    // Get summary
    std::string summary() const;

    // Expand/collapse group
    void setGroupExpanded(const std::string& groupName, bool expanded);
    bool isGroupExpanded(const std::string& groupName) const;

private:
    std::vector<GroupInfo> groups;
    std::unordered_map<std::string, std::unordered_map<std::string, std::any>> fields;
    std::unordered_map<std::string, bool> groupExpanded;
};

} // namespace nlm