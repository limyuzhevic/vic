#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "ExperimentProtocol.hpp"
#include "ExperimentStateManager.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

// Version status
enum class VersionStatus {
    Draft,
    Published,
    Deprecated,
    Archived,
    Deleted
};

// Version comparison result
struct VersionComparison {
    std::string versionId1;
    std::string versionId2;
    double similarityScore;
    std::map<std::string, std::string> differences;
    std::vector<std::string> commonElements;
    std::chrono::system_clock::time_point comparisonTime;
    
    VersionComparison() : similarityScore(0.0), comparisonTime(std::chrono::system_clock::now()) {}
    VersionComparison(const std::string& id1, const std::string& id2)
        : versionId1(id1), versionId2(id2), similarityScore(0.0),
          comparisonTime(std::chrono::system_clock::now()) {}
};

// Experiment version
struct ExperimentVersion {
    std::string versionId;
    std::string experimentName;
    std::string versionNumber;
    std::string description;
    std::chrono::system_clock::time_point created;
    std::chrono::system_clock::time_point lastModified;
    std::string createdBy;
    VersionStatus status;
    std::map<std::string, std::string> versionMetadata;
    
    ExperimentVersion() : created(std::chrono::system_clock::now()), 
                         lastModified(std::chrono::system_clock::now()),
                         status(VersionStatus::Draft) {}
    ExperimentVersion(const std::string& versionId, const std::string& experimentName, 
                     const std::string& versionNumber)
        : versionId(versionId), experimentName(experimentName), versionNumber(versionNumber),
          created(std::chrono::system_clock::now()), 
          lastModified(std::chrono::system_clock::now()),
          status(VersionStatus::Draft) {}
};

// Version history entry
struct VersionHistoryEntry {
    std::string entryId;
    std::string versionId;
    std::string experimentName;
    std::string action; // "create", "update", "publish", "deprecate", "archive", "delete"
    std::string performedBy;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, std::string> actionData;
    std::string comment;
    
    VersionHistoryEntry() : timestamp(std::chrono::system_clock::now()) {}
    VersionHistoryEntry(const std::string& entryId, const std::string& versionId, 
                       const std::string& experimentName, const std::string& action,
                       const std::string& performedBy)
        : entryId(entryId), versionId(versionId), experimentName(experimentName), action(action),
          performedBy(performedBy), timestamp(std::chrono::system_clock::now()) {}
};

class ExperimentVersioning {
public:
    ExperimentVersioning();
    ~ExperimentVersioning();
    
    // Disable copying, enable moving
    ExperimentVersioning(const ExperimentVersioning&) = delete;
    ExperimentVersioning& operator=(const ExperimentVersioning&) = delete;
    ExperimentVersioning(ExperimentVersioning&&) noexcept;
    ExperimentVersioning& operator=(ExperimentVersioning&&) noexcept;
    
    // Version management
    bool createVersion(const std::string& experimentName, const Config& config, 
                      const std::string& description = "", 
                      const std::string& versionNumber = "");
    bool createVersion(const std::string& versionId, const std::string& experimentName, 
                      const Config& config, const std::string& versionNumber = "");
    bool updateVersion(const std::string& versionId, const Config& config, 
                      const std::string& description = "");
    bool publishVersion(const std::string& versionId, const std::string& description = "");
    bool deprecateVersion(const std::string& versionId, const std::string& reason = "");
    bool archiveVersion(const std::string& versionId, const std::string& reason = "");
    bool deleteVersion(const std::string& versionId);
    
    // Version retrieval
    ExperimentVersion getVersion(const std::string& versionId) const;
    std::vector<ExperimentVersion> getVersions(const std::string& experimentName) const;
    std::vector<ExperimentVersion> getAllVersions() const;
    ExperimentVersion getLatestVersion(const std::string& experimentName) const;
    std::vector<ExperimentVersion> getVersionsByStatus(const std::string& experimentName, 
                                                      VersionStatus status) const;
    
    // Version comparison
    VersionComparison compareVersions(const std::string& versionId1, const std::string& versionId2) const;
    std::map<std::string, std::string> getVersionDifferences(const std::string& versionId1, 
                                                           const std::string& versionId2) const;
    std::vector<std::string> getCommonElements(const std::string& versionId1, 
                                              const std::string& versionId2) const;
    
    // Version history
    bool addVersionHistoryEntry(const VersionHistoryEntry& entry);
    bool removeVersionHistoryEntry(const std::string& entryId);
    VersionHistoryEntry getVersionHistoryEntry(const std::string& entryId) const;
    std::vector<VersionHistoryEntry> getVersionHistory(const std::string& experimentName) const;
    std::vector<VersionHistoryEntry> getVersionHistory(const std::string& versionId) const;
    std::vector<VersionHistoryEntry> getAllVersionHistory() const;
    
    // Version rollback
    bool rollbackVersion(const std::string& experimentName, const std::string& versionNumber);
    bool restoreVersion(const std::string& versionId, const std::string& experimentName);
    bool checkoutVersion(const std::string& versionId);
    bool branchVersion(const std::string& sourceVersionId, const std::string& branchName, 
                      const std::string& description = "");
    
    // Version analytics
    size_t getVersionCount(const std::string& experimentName) const;
    std::chrono::system_clock::time_point getVersionCreatedTime(const std::string& versionId) const;
    std::chrono::system_clock::time_point getVersionModifiedTime(const std::string& versionId) const;
    std::string getVersionCreatedBy(const std::string& versionId) const;
    
    // Version backup and recovery
    bool backupVersionData(const std::string& backupPath) const;
    bool restoreVersionData(const std::string& backupPath);
    bool exportVersionToJSON(const std::string& filepath) const;
    bool importVersionFromJSON(const std::string& filepath);
    
    // Version cleanup
    bool cleanupDeprecatedVersions(const std::chrono::system_clock::time_point& cutoffTime);
    bool cleanupArchivedVersions(const std::chrono::system_clock::time_point& cutoffTime);
    bool cleanupDeletedVersions(const std::chrono::system_clock::time_point& cutoffTime);
    bool resetVersionHistory(const std::string& experimentName);
    bool clearAllVersionHistory();
    
    // Version system management
    bool initializeVersioningSystem();
    bool shutdownVersioningSystem();
    bool isVersioningSystemInitialized() const;
    
    // Version utilities
    static std::string generateVersionId(const std::string& experimentName) const;
    static std::string generateVersionNumber(const std::string& experimentName) const;
    static VersionStatus stringToVersionStatus(const std::string& statusStr) const;
    static std::string versionStatusToString(VersionStatus status) const;
    static bool isValidVersionNumber(const std::string& versionNumber) const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm