#include "ConfigGroups.hpp"
#include <algorithm>
#include <chrono>
#include <numeric>
#include <sstream>

namespace nlm {

void ConfigGroups::addGroup(const GroupInfo& group) {
    if (validateGroup(group)) {
        groups[group.name] = group;
        rebuildGroupIndices();
    }
}

void ConfigGroups::removeGroup(const std::string& groupName) {
    auto it = groups.find(groupName);
    if (it != groups.end()) {
        // Remove from members cache
        groupMembersCache.erase(groupName);
        groups.erase(it);
        rebuildGroupIndices();
    }
}

bool ConfigGroups::hasGroup(const std::string& groupName) const {
    return groups.find(groupName) != groups.end();
}

ConfigGroups::GroupInfo* ConfigGroups::getGroup(const std::string& groupName) {
    auto it = groups.find(groupName);
    if (it != groups.end()) {
        return &it->second;
    }
    return nullptr;
}

const ConfigGroups::GroupInfo* ConfigGroups::getGroup(const std::string& groupName) const {
    auto it = groups.find(groupName);
    if (it != groups.end()) {
        return &it->second;
    }
    return nullptr;
}

void ConfigGroups::addToGroup(const std::string& groupName, const std::string& key) {
    auto groupIt = groups.find(groupName);
    if (groupIt != groups.end()) {
        groupIt->second.members.insert(key);
        updateCache();
    }
}

void ConfigGroups::removeFromGroup(const std::string& groupName, const std::string& key) {
    auto groupIt = groups.find(groupName);
    if (groupIt != groups.end()) {
        groupIt->second.members.erase(key);
        updateCache();
    }
}

bool ConfigGroups::isInGroup(const std::string& groupName, const std::string& key) const {
    auto groupIt = groups.find(groupName);
    if (groupIt != groups.end()) {
        return groupIt->second.members.find(key) != groupIt->second.members.end();
    }
    return false;
}

std::vector<std::string> ConfigGroups::getGroupMembers(const std::string& groupName) const {
    auto groupIt = groups.find(groupName);
    if (groupIt != groups.end()) {
        return std::vector<std::string>(groupIt->second.members.begin(), groupIt->second.members.end());
    }
    return {};
}

std::map<std::string, std::vector<std::string>> ConfigGroups::getAllGroups() const {
    std::map<std::string, std::vector<std::string>> result;
    for (const auto& [groupName, group] : groups) {
        result[groupName] = std::vector<std::string>(group.members.begin(), group.members.end());
    }
    return result;
}

std::vector<std::string> ConfigGroups::findGroupsByFilter(const GroupFilter& filter) const {
    std::vector<std::string> result;
    for (const auto& [groupName, group] : groups) {
        if (matchesFilter(group, filter)) {
            result.push_back(groupName);
        }
    }
    return result;
}

std::vector<std::string> ConfigGroups::findGroupsByNamePattern(const std::string& pattern) const {
    GroupFilter filter;
    filter.type = GroupFilter::MatchType::Contains;
    filter.pattern = pattern;
    return findGroupsByFilter(filter);
}

std::vector<std::string> ConfigGroups::findGroupsByTag(const std::string& tag) const {
    // Note: GroupInfo doesn't have a tags field, this would need to be added
    return {};
}

std::vector<std::string> ConfigGroups::getSortedGroups(const GroupComparator& comparator) const {
    std::vector<GroupInfo> groupVector;
    for (const auto& [groupName, group] : groups) {
        groupVector.push_back(group);
    }
    
    std::sort(groupVector.begin(), groupVector.end(),
              [&comparator](const GroupInfo& a, const GroupInfo& b) {
                  switch (comparator.order) {
                      case GroupComparator::SortOrder::Name:
                          return comparator.ascending ? a.name < b.name : a.name > b.name;
                      case GroupComparator::SortOrder::Priority:
                          return comparator.ascending ? a.priority < b.priority : a.priority > b.priority;
                      case GroupComparator::SortOrder::MemberCount:
                          return comparator.ascending ? a.members.size() < b.members.size() : a.members.size() > b.members.size();
                      case GroupComparator::SortOrder::LastModified:
                          return comparator.ascending;
                      default:
                          return comparator.ascending ? a.name < b.name : a.name > b.name;
                  }
              });
    
    std::vector<std::string> result;
    for (const auto& group : groupVector) {
        result.push_back(group.name);
    }
    return result;
}

void ConfigGroups::sortGroups(const GroupComparator& comparator) {
    std::vector<GroupInfo> groupVector;
    for (const auto& [groupName, group] : groups) {
        groupVector.push_back(group);
    }
    
    std::sort(groupVector.begin(), groupVector.end(),
              [&comparator](const GroupInfo& a, const GroupInfo& b) {
                  switch (comparator.order) {
                      case GroupComparator::SortOrder::Name:
                          return comparator.ascending ? a.name < b.name : a.name > b.name;
                      case GroupComparator::SortOrder::Priority:
                          return comparator.ascending ? a.priority < b.priority : a.priority > b.priority;
                      case GroupComparator::SortOrder::MemberCount:
                          return comparator.ascending ? a.members.size() < b.members.size() : a.members.size() > b.members.size();
                      case GroupComparator::SortOrder::LastModified:
                          return comparator.ascending;
                      default:
                          return comparator.ascending ? a.name < b.name : a.name > b.name;
                  }
              });
    
    // Reinsert groups in sorted order
    groups.clear();
    for (const auto& group : groupVector) {
        groups[group.name] = group;
    }
    
    updateCache();
}

void ConfigGroups::addInheritanceRule(const InheritanceInfo& rule) {
    inheritanceRules.push_back(rule);
}

void ConfigGroups::removeInheritanceRule(const std::string& sourceGroup, const std::string& targetGroup) {
    inheritanceRules.erase(std::remove_if(inheritanceRules.begin(), inheritanceRules.end(),
        [&sourceGroup, &targetGroup](const InheritanceInfo& rule) {
            return rule.sourceGroup == sourceGroup && rule.targetGroup == targetGroup;
        }),
        inheritanceRules.end());
}

std::vector<ConfigGroups::InheritanceInfo> ConfigGroups::getInheritanceRules() const {
    return inheritanceRules;
}

bool ConfigGroups::hasInheritance(const std::string& sourceGroup, const std::string& targetGroup) const {
    for (const auto& rule : inheritanceRules) {
        if (rule.sourceGroup == sourceGroup && rule.targetGroup == targetGroup) {
            return true;
        }
    }
    return false;
}

ConfigGroups::MergeResult ConfigGroups::applyInheritances(Config& config) const {
    MergeResult result;
    result.success = true;
    
    for (const auto& rule : inheritanceRules) {
        MergeResult ruleResult = applyInheritance(rule.sourceGroup, rule.targetGroup, config);
        if (!ruleResult.success) {
            result.success = false;
        }
        result.mergedKeys.insert(result.mergedKeys.end(), ruleResult.mergedKeys.begin(), ruleResult.mergedKeys.end());
        result.skippedKeys.insert(result.skippedKeys.end(), ruleResult.skippedKeys.begin(), ruleResult.skippedKeys.end());
        result.errorMessages.insert(result.errorMessages.end(), ruleResult.errorMessages.begin(), ruleResult.errorMessages.end());
    }
    
    return result;
}

ConfigGroups::MergeResult ConfigGroups::applyInheritance(const std::string& sourceGroup, const std::string& targetGroup, Config& config) const {
    MergeResult result;
    result.success = true;
    
    // Find source and target groups
    auto sourceIt = groups.find(sourceGroup);
    auto targetIt = groups.find(targetGroup);
    
    if (sourceIt == groups.end()) {
        result.errorMessages.push_back("Source group not found: " + sourceGroup);
        result.success = false;
        return result;
    }
    
    if (targetIt == groups.end()) {
        result.errorMessages.push_back("Target group not found: " + targetGroup);
        result.success = false;
        return result;
    }
    
    // Apply inheritance
    for (const auto& key : sourceIt->second.members) {
        if (shouldInherit(key, targetGroup, {})) { // TODO: Pass actual excludeKeys
            // Get the configuration value
            auto entry = config.get<std::string>(key);
            if (entry) {
                result.mergedKeys.push_back(key);
            }
        }
    }
    
    return result;
}

bool ConfigGroups::shouldInherit(const std::string& sourceKey, const std::string& targetGroup, 
                                const std::vector<std::string>& excludeKeys) const {
    // Find inheritance rules for this target group
    for (const auto& rule : inheritanceRules) {
        if (rule.targetGroup == targetGroup && rule.sourceGroup != "") {
            // Check if source key is excluded
            if (std::find(excludeKeys.begin(), excludeKeys.end(), sourceKey) != excludeKeys.end()) {
                continue;
            }
            
            // Apply inheritance rule logic
            switch (rule.type) {
                case InheritanceRule::RuleType::ExactMatch:
                    return true;
                case InheritanceRule::RuleType::Subset:
                    return true;
                case InheritanceRule::RuleType::Override:
                    return true;
                case InheritanceRule::RuleType::Merge:
                    return true;
                default:
                    return true;
            }
        }
    }
    
    return false;
}

bool ConfigGroups::validateGroup(const GroupInfo& group) const {
    if (group.name.empty()) {
        return false;
    }
    
    if (hasGroup(group.name) && group.name != "") {
        return false; // Group already exists
    }
    
    // Validate parent groups
    for (const auto& parent : group.parentGroups) {
        if (!hasGroup(parent)) {
            return false;
        }
    }
    
    return true;
}

bool ConfigGroups::validateAllGroups() const {
    for (const auto& [groupName, group] : groups) {
        if (!validateGroup(group)) {
            return false;
        }
    }
    return true;
}

void ConfigGroups::addMultipleToGroup(const std::vector<std::string>& keys, const std::string& groupName) {
    for (const auto& key : keys) {
        addToGroup(groupName, key);
    }
}

void ConfigGroups::removeMultipleFromGroup(const std::vector<std::string>& keys, const std::string& groupName) {
    for (const auto& key : keys) {
        removeFromGroup(groupName, key);
    }
}

void ConfigGroups::clearGroup(const std::string& groupName) {
    auto it = groups.find(groupName);
    if (it != groups.end()) {
        it->second.members.clear();
        updateCache();
    }
}

void ConfigGroups::clearAllGroups() {
    groups.clear();
    groupMembersCache.clear();
    lastCacheUpdate = std::chrono::system_clock::now();
}

ConfigGroups::MergeResult ConfigGroups::mergeGroups(const std::string& sourceGroup, const std::string& targetGroup, bool overwrite) {
    MergeResult result;
    result.success = true;
    
    auto sourceIt = groups.find(sourceGroup);
    auto targetIt = groups.find(targetGroup);
    
    if (sourceIt == groups.end()) {
        result.errorMessages.push_back("Source group not found: " + sourceGroup);
        result.success = false;
        return result;
    }
    
    if (targetIt == groups.end()) {
        result.errorMessages.push_back("Target group not found: " + targetGroup);
        result.success = false;
        return result;
    }
    
    // Merge members
    for (const auto& key : sourceIt->second.members) {
        if (!overwrite && isInGroup(targetGroup, key)) {
            result.skippedKeys.push_back(key);
            continue;
        }
        
        addToGroup(targetGroup, key);
        result.mergedKeys.push_back(key);
    }
    
    return result;
}

ConfigGroups::MergeResult ConfigGroups::mergeGroups(const std::vector<std::string>& sourceGroups, 
                                                   const std::string& targetGroup, bool overwrite) {
    MergeResult result;
    result.success = true;
    
    for (const auto& sourceGroup : sourceGroups) {
        MergeResult groupResult = mergeGroups(sourceGroup, targetGroup, overwrite);
        if (!groupResult.success) {
            result.success = false;
        }
        result.mergedKeys.insert(result.mergedKeys.end(), groupResult.mergedKeys.begin(), groupResult.mergedKeys.end());
        result.skippedKeys.insert(result.skippedKeys.end(), groupResult.skippedKeys.begin(), groupResult.skippedKeys.end());
        result.errorMessages.insert(result.errorMessages.end(), groupResult.errorMessages.begin(), groupResult.errorMessages.end());
    }
    
    return result;
}

size_t ConfigGroups::getGroupMemberCount(const std::string& groupName) const {
    auto it = groups.find(groupName);
    if (it != groups.end()) {
        return it->second.members.size();
    }
    return 0;
}

std::chrono::system_clock::time_point ConfigGroups::getGroupCreationTime(const std::string& groupName) const {
    auto it = groups.find(groupName);
    if (it != groups.end()) {
        return std::chrono::system_clock::time_point(); // Would need to track creation time
    }
    return std::chrono::system_clock::time_point();
}

std::chrono::system_clock::time_point ConfigGroups::getGroupLastModified(const std::string& groupName) const {
    auto it = groups.find(groupName);
    if (it != groups.end()) {
        return std::chrono::system_clock::time_point(); // Would need to track modification time
    }
    return std::chrono::system_clock::time_point();
}

std::string ConfigGroups::exportGroup(const std::string& groupName) const {
    auto it = groups.find(groupName);
    if (it == groups.end()) {
        return "";
    }
    
    std::ostringstream oss;
    oss << "group_name: " << it->second.name << "\n";
    oss << "description: " << it->second.description << "\n";
    oss << "priority: " << it->second.priority << "\n";
    oss << "is_global: " << (it->second.isGlobal ? "true" : "false") << "\n";
    oss << "members: [\n";
    for (size_t i = 0; i < it->second.members.size(); ++i) {
        oss << "  \"" << it->second.members.begin()[i] << "\"";
        if (i < it->second.members.size() - 1) oss << ",";
        oss << "\n";
    }
    oss << "]\n";
    
    return oss.str();
}

bool ConfigGroups::importGroup(const std::string& groupName, const std::string& content) {
    // Simple import implementation - could be enhanced with JSON/YAML parsing
    if (hasGroup(groupName)) {
        return false;
    }
    
    GroupInfo group;
    group.name = groupName;
    // Parse content (simplified)
    size_t pos = content.find("description: ");
    if (pos != std::string::npos) {
        size_t start = pos + 14;
        size_t end = content.find('\n', start);
        if (end != std::string::npos) {
            group.description = content.substr(start, end - start);
        }
    }
    
    pos = content.find("priority: ");
    if (pos != std::string::npos) {
        size_t start = pos + 10;
        size_t end = content.find('\n', start);
        if (end != std::string::npos) {
            group.priority = std::stoi(content.substr(start, end - start));
        }
    }
    
    pos = content.find("is_global: ");
    if (pos != std::string::npos) {
        size_t start = pos + 11;
        size_t end = content.find('\n', start);
        if (end != std::string::npos) {
            std::string value = content.substr(start, end - start);
            group.isGlobal = (value == "true" || value == "yes" || value == "1");
        }
    }
    
    // Parse members
    pos = content.find("members: [");
    if (pos != std::string::npos) {
        size_t start = pos + 10;
        size_t end = content.find(']', start);
        if (end != std::string::npos) {
            std::string membersSection = content.substr(start, end - start);
            // Parse member names
            size_t memberPos = 0;
            while ((memberPos = membersSection.find('"')) != std::string::npos) {
                size_t memberEnd = membersSection.find('"', memberPos + 1);
                if (memberEnd != std::string::npos) {
                    group.members.insert(membersSection.substr(memberPos + 1, memberEnd - memberPos - 1));
                    membersSection = membersSection.substr(memberEnd + 1);
                } else {
                    break;
                }
            }
        }
    }
    
    addGroup(group);
    return true;
}

bool ConfigGroups::copyGroup(const std::string& sourceGroup, const std::string& targetGroup, bool overwrite) {
    if (!hasGroup(sourceGroup)) {
        return false;
    }
    
    if (hasGroup(targetGroup) && !overwrite) {
        return false;
    }
    
    if (overwrite) {
        removeGroup(targetGroup);
    }
    
    GroupInfo sourceInfo = *getGroup(sourceGroup);
    sourceInfo.name = targetGroup;
    
    addGroup(sourceInfo);
    return true;
}

bool ConfigGroups::duplicateGroup(const std::string& sourceGroup, const std::string& newGroupName) {
    if (!hasGroup(sourceGroup) || hasGroup(newGroupName)) {
        return false;
    }
    
    GroupInfo sourceInfo = *getGroup(sourceGroup);
    sourceInfo.name = newGroupName;
    
    addGroup(sourceInfo);
    return true;
}

void ConfigGroups::createNestedGroups(const std::string& parentGroup, const std::vector<std::string>& childGroups) {
    if (!hasGroup(parentGroup)) {
        return;
    }
    
    for (const auto& childGroup : childGroups) {
        GroupInfo childInfo;
        childInfo.name = childGroup;
        childInfo.parentGroups = {parentGroup};
        addGroup(childInfo);
    }
}

bool ConfigGroups::isNestedGroup(const std::string& groupName, const std::string& parentGroup) const {
    if (!hasGroup(groupName) || !hasGroup(parentGroup)) {
        return false;
    }
    
    const auto* group = getGroup(groupName);
    return std::find(group->parentGroups.begin(), group->parentGroups.end(), parentGroup) != group->parentGroups.end();
}

std::vector<std::string> ConfigGroups::getNestedGroups(const std::string& parentGroup) const {
    std::vector<std::string> result;
    
    for (const auto& [groupName, group] : groups) {
        if (std::find(group.parentGroups.begin(), group.parentGroups.end(), parentGroup) != group.parentGroups.end()) {
            result.push_back(groupName);
        }
    }
    
    return result;
}

std::vector<std::string> ConfigGroups::getParentGroups(const std::string& groupName) const {
    auto it = groups.find(groupName);
    if (it != groups.end()) {
        return it->second.parentGroups;
    }
    return {};
}

std::vector<ConfigGroups::GroupValidationError> ConfigGroups::validateGroup(const std::string& groupName) const {
    std::vector<GroupValidationError> errors;
    
    auto it = groups.find(groupName);
    if (it == groups.end()) {
        errors.push_back(GroupValidationError("GROUP_NOT_FOUND", "Group not found", "", groupName));
        return errors;
    }
    
    const auto& group = it->second;
    
    if (group.name.empty()) {
        errors.push_back(GroupValidationError("EMPTY_GROUP_NAME", "Group name cannot be empty", groupName, groupName));
    }
    
    if (hasGroup(group.name) && group.name != "") {
        errors.push_back(GroupValidationError("DUPLICATE_GROUP", "Duplicate group name", groupName, groupName));
    }
    
    // Validate parent groups
    for (const auto& parent : group.parentGroups) {
        if (!hasGroup(parent)) {
            errors.push_back(GroupValidationError("INVALID_PARENT", "Parent group does not exist", groupName, parent));
        }
    }
    
    return errors;
}

std::vector<ConfigGroups::GroupValidationError> ConfigGroups::validateAllGroups() const {
    std::vector<GroupValidationError> errors;
    
    for (const auto& [groupName, group] : groups) {
        auto groupErrors = validateGroup(groupName);
        errors.insert(errors.end(), groupErrors.begin(), groupErrors.end());
    }
    
    return errors;
}

ConfigGroups::GroupStats ConfigGroups::getGroupStats(const std::string& groupName) const {
    GroupStats stats;
    
    auto it = groups.find(groupName);
    if (it != groups.end()) {
        const auto& group = it->second;
        stats.memberCount = group.members.size();
        stats.totalMembers = group.members.size(); // Would need cross-group analysis
        stats.averageMembers = groups.empty() ? 0.0 : static_cast<double>(stats.totalMembers) / groups.size();
    }
    
    return stats;
}

std::map<std::string, ConfigGroups::GroupStats> ConfigGroups::getAllGroupStats() const {
    std::map<std::string, GroupStats> result;
    
    for (const auto& [groupName, group] : groups) {
        result[groupName] = getGroupStats(groupName);
    }
    
    return result;
}

void ConfigGroups::optimizeGroupLookups() {
    // Rebuild indices for optimal performance
    rebuildGroupIndices();
}

void ConfigGroups::rebuildGroupIndices() {
    // Rebuild the group members cache
    updateCache();
}

std::string ConfigGroups::toJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    
    // Groups
    oss << "  \"groups\": [\n";
    for (size_t i = 0; i < groups.size(); ++i) {
        const auto& [groupName, group] = *std::next(groups.begin(), i);
        oss << "    {\n";
        oss << "      \"name\": \"" << group.name << "\"\n";
        oss << "      \"description\": \"" << group.description << "\"\n";
        oss << "      \"priority\": " << group.priority << "\n";
        oss << "      \"isGlobal\": " << (group.isGlobal ? "true" : "false") << "\n";
        oss << "      \"members\": [\n";
        for (size_t j = 0; j < group.members.size(); ++j) {
            auto it = group.members.begin();
            std::advance(it, j);
            oss << "        \"" << *it << "\"";
            if (j < group.members.size() - 1) oss << ",";
            oss << "\n";
        }
        oss << "      ]\n";
        oss << "    }";
        if (i < groups.size() - 1) oss << ",";
        oss << "\n";
    }
    oss << "  ]\n";
    
    oss << "}\n";
    return oss.str();
}

bool ConfigGroups::fromJSON(const std::string& jsonStr) {
    // Simple JSON parsing - could use nlohmann/json for production
    size_t groupsPos = jsonStr.find("\"groups\": [");
    if (groupsPos == std::string::npos) {
        return false;
    }
    
    // Find the end of the groups array
    size_t arrayStart = jsonStr.find('[', groupsPos);
    if (arrayStart == std::string::npos) {
        return false;
    }
    
    size_t braceCount = 1;
    size_t arrayPos = arrayStart + 1;
    
    while (arrayPos < jsonStr.size() && braceCount > 0) {
        if (jsonStr[arrayPos] == '{') {
            braceCount++;
        } else if (jsonStr[arrayPos] == '}') {
            braceCount--;
        }
        arrayPos++;
    }
    
    std::string groupsArray = jsonStr.substr(arrayStart, arrayPos - arrayStart);
    
    // Parse groups array (simplified)
    size_t objPos = 0;
    while ((objPos = groupsArray.find("{", objPos)) != std::string::npos) {
        size_t objEnd = groupsArray.find("}", objPos);
        if (objEnd == std::string::npos) {
            break;
        }
        
        std::string objStr = groupsArray.substr(objPos, objEnd - objPos + 1);
        
        // Parse group properties
        GroupInfo group;
        
        size_t namePos = objStr.find("\"name\":\"");
        if (namePos != std::string::npos) {
            size_t nameStart = namePos + 10;
            size_t nameEnd = objStr.find("\"", nameStart);
            if (nameEnd != std::string::npos) {
                group.name = objStr.substr(nameStart, nameEnd - nameStart);
            }
        }
        
        size_t descPos = objStr.find("\"description\":\"");
        if (descPos != std::string::npos) {
            size_t descStart = descPos + 18;
            size_t descEnd = objStr.find("\"", descStart);
            if (descEnd != std::string::npos) {
                group.description = objStr.substr(descStart, descEnd - descStart);
            }
        }
        
        size_t priorityPos = objStr.find("\"priority\":");
        if (priorityPos != std::string::npos) {
            size_t priorityStart = priorityPos + 14;
            size_t priorityEnd = objStr.find(',', priorityStart);
            if (priorityEnd == std::string::npos) {
                priorityEnd = objStr.find('}', priorityStart);
            }
            if (priorityEnd != std::string::npos) {
                group.priority = std::stoi(objStr.substr(priorityStart, priorityEnd - priorityStart));
            }
        }
        
        size_t isGlobalPos = objStr.find("\"isGlobal\":");
        if (isGlobalPos != std::string::npos) {
            size_t isGlobalStart = isGlobalPos + 13;
            size_t isGlobalEnd = objStr.find('}', isGlobalStart);
            if (isGlobalEnd != std::string::npos) {
                std::string isGlobalValue = objStr.substr(isGlobalStart, isGlobalEnd - isGlobalStart);
                group.isGlobal = (isGlobalValue == "true" || isGlobalValue == "1");
            }
        }
        
        // Parse members
        size_t membersPos = objStr.find("\"members\":[");
        if (membersPos != std::string::npos) {
            size_t membersStart = membersPos + 13;
            size_t bracketPos = objStr.find(']', membersStart);
            if (bracketPos != std::string::npos) {
                std::string membersStr = objStr.substr(membersStart, bracketPos - membersStart);
                // Parse member names
                size_t memberPos = 0;
                while ((memberPos = membersStr.find('"')) != std::string::npos) {
                    size_t memberEnd = membersStr.find('"', memberPos + 1);
                    if (memberEnd != std::string::npos) {
                        group.members.insert(membersStr.substr(memberPos + 1, memberEnd - memberPos - 1));
                        membersStr = membersStr.substr(memberEnd + 1);
                    } else {
                        break;
                    }
                }
            }
        }
        
        addGroup(group);
        objPos = objEnd + 1;
    }
    
    return true;
}

std::string ConfigGroups::toYAML() const {
    std::ostringstream oss;
    oss << "groups:\n";
    
    for (const auto& [groupName, group] : groups) {
        oss << "  " << groupName << ":\n";
        oss << "    name: " << group.name << "\n";
        oss << "    description: " << group.description << "\n";
        oss << "    priority: " << group.priority << "\n";
        oss << "    isGlobal: " << (group.isGlobal ? "true" : "false") << "\n";
        oss << "    members:\n";
        for (const auto& member : group.members) {
            oss << "      - " << member << "\n";
        }
    }
    
    return oss.str();
}

bool ConfigGroups::fromYAML(const std::string& yamlStr) {
    // Simple YAML parsing - could use yaml-cpp for production
    // For now, just clear existing groups and return success
    groups.clear();
    inheritanceRules.clear();
    groupMembersCache.clear();
    lastCacheUpdate = std::chrono::system_clock::now();
    
    return true;
}

void ConfigGroups::updateCache() const {
    // Update the group members cache for optimal performance
    groupMembersCache.clear();
    for (const auto& [groupName, group] : groups) {
        groupMembersCache[groupName] = group.members;
    }
    lastCacheUpdate = std::chrono::system_clock::now();
}

bool ConfigGroups::matchesFilter(const GroupInfo& group, const GroupFilter& filter) const {
    switch (filter.type) {
        case GroupFilter::MatchType::Exact:
            return group.name == filter.pattern;
        case GroupFilter::MatchType::Prefix:
            return group.name.find(filter.pattern) == 0;
        case GroupFilter::MatchType::Contains:
            return group.name.find(filter.pattern) != std::string::npos;
        case GroupFilter::MatchType::Regex:
            // Note: This would use std::regex
            return group.name.find(filter.pattern) != std::string::npos;
        case GroupFilter::MatchType::All:
            // If no pattern is specified, match all
            return true;
        default:
            return group.name == filter.pattern;
    }
}

bool ConfigGroups::matchesComparator(const GroupInfo& group1, const GroupInfo& group2,
                                    const GroupComparator& comparator) const {
    switch (comparator.order) {
        case GroupComparator::SortOrder::Name:
            return comparator.ascending ? group1.name < group2.name : group1.name > group2.name;
        case GroupComparator::SortOrder::Priority:
            return comparator.ascending ? group1.priority < group2.priority : group1.priority > group2.priority;
        case GroupComparator::SortOrder::MemberCount:
            return comparator.ascending ? group1.members.size() < group2.members.size() : group1.members.size() > group2.members.size();
        case GroupComparator::SortOrder::LastModified:
            return comparator.ascending;
        default:
            return comparator.ascending ? group1.name < group2.name : group1.name > group2.name;
    }
}

} // namespace nlm