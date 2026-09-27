#pragma once

/**
 * Configuration Versioning
 * 
 * Configuration versioning system for managing configuration versions,
 * supporting version tracking, rollback, branching, and merging for the NLM brain simulation project.
 * 
 * Features:
 * - Configuration versioning
 * - Version creation and management
 * - Version rollback
 * - Version branching
 * - Version comparison
 * - Version merging
 * - Version history
 * - Version tags
 * - Version annotations
 * - Version snapshot management
 */

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <chrono>
#include <algorithm>
#include <unordered_map>
#include <functional>

namespace nlm {
namespace core {

class ConfigurationManager;

/**
 * Version number
 */
struct VersionNumber {
    int major;
    int minor;
    int patch;
    std::string preRelease;
    std::string buildMeta;
    
    VersionNumber(int major = 0, int minor = 0, int patch = 0,
                  const std::string& pre = "", const std::string& meta = "")
        : major(major), minor(minor), patch(patch), preRelease(pre), buildMeta(meta) {}
    
    bool operator<(const VersionNumber& other) const;
    bool operator<=(const VersionNumber& other) const;
    bool operator==(const VersionNumber& other) const;
    bool operator!=(const VersionNumber& other) const;
    bool operator>=(const VersionNumber& other) const;
    bool operator>(const VersionNumber& other) const;
    
    std::string toString() const;
    static VersionNumber fromString(const std::string& versionString);
};

/**
 * Version tag
 */
struct VersionTag {
    std::string name;
    VersionNumber version;
    std::string description;
    std::string createdBy;
    double createdAt;
    
    VersionTag(const std::string& name, const VersionNumber& version,
               const std::string& desc = "", const std::string& creator = "")
        : name(name), version(version), description(desc), createdBy(creator),
          createdAt(0.0) {}
};

/**
 * Configuration version
 */
struct ConfigVersion {
    std::string versionId;
    VersionNumber versionNumber;
    double createdAt;
    std::string createdBy;
    std::string description;
    std::map<std::string, std::string> configurationData;
    std::string checksum;
    
    ConfigVersion(const std::string& id, const VersionNumber& version,
                  const std::string& creator, const std::string& desc = "")
        : versionId(id), versionNumber(version), createdAt(0.0),
          createdBy(creator), description(desc) {}
};

/**
 * Configuration versioning system
 */
class ConfigurationVersioning {
public:
    ConfigurationVersioning();
    ~ConfigurationVersioning();
    
    // Disable copying, enable moving
    ConfigurationVersioning(const ConfigurationVersioning&) = delete;
    ConfigurationVersioning& operator=(const ConfigurationVersioning&) = delete;
    ConfigurationVersioning(ConfigurationVersioning&&) noexcept = default;
    ConfigurationVersioning& operator=(ConfigurationVersioning&&) noexcept = default;
    
    // Version management
    std::string createVersion(ConfigurationManager& configManager, const std::string& description = "");
    bool removeVersion(const std::string& versionId);
    ConfigVersion* getVersion(const std::string& versionId);
    const ConfigVersion* getVersion(const std::string& versionId) const;
    std::vector<std::string> getVersionIds() const;
    
    // Version retrieval
    ConfigVersion* getLatestVersion();
    const ConfigVersion* getLatestVersion() const;
    ConfigVersion* getVersionByNumber(const VersionNumber& versionNumber);
    const ConfigVersion* getVersionByNumber(const VersionNumber& versionNumber) const;
    
    // Version branching
    std::string createBranch(const std::string& sourceVersionId, const std::string& branchName);
    bool mergeBranch(const std::string& targetVersionId, const std::string& sourceBranchId);
    std::vector<std::string> getBranches(const std::string& versionId) const;
    
    // Version rollback
    bool rollbackVersion(const std::string& versionId, ConfigurationManager& configManager);
    bool rollbackToVersion(const std::string& versionId, ConfigurationManager& configManager);
    bool restoreFromVersion(const std::string& versionId, ConfigurationManager& configManager);
    
    // Version tags
    bool addTag(const VersionTag& tag);
    bool removeTag(const std::string& tagName);
    VersionTag* getTag(const std::string& tagName);
    const VersionTag* getTag(const std::string& tagName) const;
    std::vector<VersionTag> getAllTags() const;
    
    // Version comparison
    bool isVersionNewer(const std::string& versionId1, const std::string& versionId2) const;
    bool isVersionOlder(const std::string& versionId1, const std::string& versionId2) const;
    bool areVersionsCompatible(const std::string& versionId1, const std::string& versionId2) const;
    std::vector<std::string> getVersionDifferences(const std::string& versionId1, const std::string& versionId2) const;
    
    // Version history
    std::vector<ConfigVersion> getVersionHistory(size_t limit = 100) const;
    std::vector<ConfigVersion> getVersionHistoryForComponent(const std::string& componentName,
                                                             size_t limit = 100) const;
    void clearVersionHistory();
    
    // Version annotations
    bool addAnnotation(const std::string& versionId, const std::string& annotation);
    std::vector<std::string> getAnnotations(const std::string& versionId) const;
    bool removeAnnotation(const std::string& versionId, const std::string& annotation);
    
    // Version snapshot management
    bool createSnapshot(const std::string& snapshotName, ConfigurationManager& configManager);
    bool restoreSnapshot(const std::string& snapshotName, ConfigurationManager& configManager);
    std::vector<std::string> getAvailableSnapshots() const;
    
    // Version statistics
    size_t getVersionCount() const { return versions_.size(); }
    size_t getTagCount() const { return tags_.size(); }
    size_t getBranchCount() const { return branches_.size(); }
    VersionNumber getLatestVersionNumber() const;
    
    // Version loading/saving
    bool saveToFile(const std::string& filepath) const;
    bool loadFromFile(const std::string& filepath);
    
    // Version cleanup
    bool cleanupOldVersions(double beforeDate);
    bool cleanupUnusedVersions();
    
    // Thread safety
    void lock() const;
    void unlock() const;
    bool tryLock() const;
    
    // Debug and diagnostics
    std::string dump() const;
    std::string getStatisticsString() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    mutable std::mutex mutex_;
    
    std::map<std::string, ConfigVersion> versions_;
    std::map<std::string, VersionTag> tags_;
    std::map<std::string, std::vector<std::string>> branches_;
    
    std::string generateVersionId() const;
    std::string calculateVersionChecksum(const std::map<std::string, std::string>& configData) const;
    VersionNumber calculateNextVersionNumber(const VersionNumber& currentVersion) const;
    
    friend class ConfigurationManager;
};

} // namespace core
} // namespace nlm
