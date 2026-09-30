#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include <unordered_set>

namespace nlm {

class Config;

class ConfigGroups {
public:
    struct GroupInfo {
        std::string name;
        std::string description;
        int priority;
        bool isGlobal;
        std::vector<std::string> parentGroups;
        std::unordered_set<std::string> members;
        
        GroupInfo() : priority(100), isGlobal(false) {}
        GroupInfo(const std::string& n, const std::string& desc = "", 
                 int prio = 100, bool global = false)
            : name(n), description(desc), priority(prio), isGlobal(global) {}
    };
    
    struct InheritanceInfo {
        std::string sourceGroup;
        std::string targetGroup;
        std::vector<std::string> excludeKeys;
        bool deepCopy;
        std::chrono::system_clock::time_point lastMerged;
        
        InheritanceInfo() : deepCopy(false), lastMerged(std::chrono::system_clock::now()) {}
        InheritanceInfo(const std::string& src, const std::string& tgt,
                       const std::vector<std::string>& exclude = {},
                       bool deep = false)
            : sourceGroup(src), targetGroup(tgt), excludeKeys(exclude), 
              deepCopy(deep), lastMerged(std::chrono::system_clock::now()) {}
    };
    
    struct MergeResult {
        bool success;
        std::vector<std::string> mergedKeys;
        std::vector<std::string> skippedKeys;
        std::vector<std::string> errorMessages;
        
        MergeResult() : success(true) {}
        MergeResult(bool s, const std::vector<std::string>& merged,
                   const std::vector<std::string>& skipped,
                   const std::vector<std::string>& errors)
            : success(s), mergedKeys(merged), skippedKeys(skipped), 
              errorMessages(errors) {}
    };
    
    struct GroupFilter {
        enum class MatchType {
            Exact,
            Prefix,
            Contains,
            Regex,
            All
        };
        
        MatchType type;
        std::string pattern;
        std::vector<std::string> tags;
        std::map<std::string, std::string> attributes;
        
        GroupFilter() : type(MatchType::Exact), pattern("") {}
        GroupFilter(MatchType t, const std::string& p) : type(t), pattern(p) {}
    };
    
    struct GroupComparator {
        enum class SortOrder {
            Name,
            Priority,
            MemberCount,
            LastModified
        };
        
        SortOrder order;
        bool ascending;
        
        GroupComparator() : order(SortOrder::Name), ascending(true) {}
        GroupComparator(SortOrder o, bool asc = true) : order(o), ascending(asc) {}
    };
    
    ConfigGroups() = default;
    ~ConfigGroups() = default;
    
    // Group management
    void addGroup(const GroupInfo& group);
    void removeGroup(const std::string& groupName);
    bool hasGroup(const std::string& groupName) const;
    GroupInfo* getGroup(const std::string& groupName);
    const GroupInfo* getGroup(const std::string& groupName) const;
    
    // Group membership
    void addToGroup(const std::string& groupName, const std::string& key);
    void removeFromGroup(const std::string& groupName, const std::string& key);
    bool isInGroup(const std::string& groupName, const std::string& key) const;
    std::vector<std::string> getGroupMembers(const std::string& groupName) const;
    std::map<std::string, std::vector<std::string>> getAllGroups() const;
    
    // Group filtering and querying
    std::vector<std::string> findGroupsByFilter(const GroupFilter& filter) const;
    std::vector<std::string> findGroupsByNamePattern(const std::string& pattern) const;
    std::vector<std::string> findGroupsByTag(const std::string& tag) const;
    
    // Group sorting and ordering
    std::vector<std::string> getSortedGroups(const GroupComparator& comparator) const;
    void sortGroups(const GroupComparator& comparator);
    
    // Inheritance management
    void addInheritanceRule(const InheritanceInfo& rule);
    void removeInheritanceRule(const std::string& sourceGroup, const std::string& targetGroup);
    std::vector<InheritanceInfo> getInheritanceRules() const;
    bool hasInheritance(const std::string& sourceGroup, const std::string& targetGroup) const;
    
    // Inheritance execution
    MergeResult applyInheritances(Config& config) const;
    MergeResult applyInheritance(const std::string& sourceGroup, const std::string& targetGroup, Config& config) const;
    
    // Configuration inheritance
    bool shouldInherit(const std::string& sourceKey, const std::string& targetGroup, 
                      const std::vector<std::string>& excludeKeys) const;
    
    // Validation
    bool validateGroup(const GroupInfo& group) const;
    bool validateAllGroups() const;
    
    // Batch operations
    void addMultipleToGroup(const std::vector<std::string>& keys, const std::string& groupName);
    void removeMultipleFromGroup(const std::vector<std::string>& keys, const std::string& groupName);
    void clearGroup(const std::string& groupName);
    void clearAllGroups();
    
    // Group merging
    MergeResult mergeGroups(const std::string& sourceGroup, const std::string& targetGroup,
                           bool overwrite = true);
    MergeResult mergeGroups(const std::vector<std::string>& sourceGroups, 
                           const std::string& targetGroup, bool overwrite = true);
    
    // Group analysis
    size_t getGroupMemberCount(const std::string& groupName) const;
    std::chrono::system_clock::timePoint getGroupCreationTime(const std::string& groupName) const;
    std::chrono::system_clock::timePoint getGroupLastModified(const std::string& groupName) const;
    
    // Export/import
    std::string exportGroup(const std::string& groupName) const;
    bool importGroup(const std::string& groupName, const std::string& content);
    
    // Group copying
    bool copyGroup(const std::string& sourceGroup, const std::string& targetGroup,
                  bool overwrite = false);
    bool duplicateGroup(const std::string& sourceGroup, const std::string& newGroupName);
    
    // Advanced grouping
    void createNestedGroups(const std::string& parentGroup, const std::vector<std::string>& childGroups);
    bool isNestedGroup(const std::string& groupName, const std::string& parentGroup) const;
    std::vector<std::string> getNestedGroups(const std::string& parentGroup) const;
    std::vector<std::string> getParentGroups(const std::string& groupName) const;
    
    // Group validation
    struct GroupValidationError {
        std::string errorCode;
        std::string message;
        std::string key;
        std::string groupName;
        
        GroupValidationError() : errorCode("UNKNOWN"), message("Unknown error") {}
        GroupValidationError(const std::string& code, const std::string& msg, 
                           const std::string& key = "", const std::string& group = "")
            : errorCode(code), message(msg), key(key), groupName(group) {}
    };
    
    std::vector<GroupValidationError> validateGroup(const std::string& groupName) const;
    std::vector<GroupValidationError> validateAllGroups() const;
    
    // Group statistics
    struct GroupStats {
        size_t memberCount;
        size_t totalMembers;
        double averageMembers;
        std::chrono::system_clock::time_point oldestMember;
        std::chrono::system_clock::time_point newestMember;
        
        GroupStats() : memberCount(0), totalMembers(0), averageMembers(0.0) {}
    };
    
    GroupStats getGroupStats(const std::string& groupName) const;
    std::map<std::string, GroupStats> getAllGroupStats() const;
    
    // Performance optimization
    void optimizeGroupLookups();
    void rebuildGroupIndices();
    
    // Serialization
    std::string toJSON() const;
    bool fromJSON(const std::string& jsonStr);
    
    std::string toYAML() const;
    bool fromYAML(const std::string& yamlStr);
    
private:
    std::map<std::string, GroupInfo> groups;
    std::vector<InheritanceInfo> inheritanceRules;
    mutable std::map<std::string, std::unordered_set<std::string>> groupMembersCache;
    mutable std::chrono::system_clock::time_point lastCacheUpdate;
    
    void updateCache() const;
    bool matchesFilter(const GroupInfo& group, const GroupFilter& filter) const;
    bool matchesComparator(const GroupInfo& group1, const GroupInfo& group2, 
                          const GroupComparator& comparator) const;
};

} // namespace nlm