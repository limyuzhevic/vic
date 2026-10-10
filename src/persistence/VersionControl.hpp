// Version control - checkpoint version control system
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <functional>
#include "CheckpointSystem.hpp"

namespace nlm {

// Version control entry
struct VersionControlEntry {
    std::string versionName;
    uint64_t checkpointStep;
    double simulationTime;
    time_t timestamp;
    std::string description;
    std::string author;
    std::string baseVersion;
    std::vector<std::string> changes;
    bool isMerge;
    
    VersionControlEntry() : checkpointStep(0), simulationTime(0.0), timestamp(0), isMerge(false) {}
};

// Version control statistics
struct VersionControlStats {
    uint64_t totalVersions;
    uint64_t branches;
    uint64_t merges;
    uint32_t errorCount;
    
    VersionControlStats() : totalVersions(0), branches(0), merges(0), errorCount(0) {}
};

// Version control operations
enum class VersionControlOperation {
    Create,
    Merge,
    Branch,
    Tag,
    Revert,
    Compare
};

// Main version control class
class VersionControl {
public:
    VersionControl();
    ~VersionControl();
    
    // Configuration
    bool configure(const std::string& versionDir, const std::shared_ptr<Config>& config);
    void shutdown();
    
    // Version control operations
    bool createVersion(const std::string& versionName, const std::string& checkpointName,
                       const std::string& description, const std::string& author);
    bool mergeVersions(const std::string& targetVersion, const std::vector<std::string>& sourceVersions);
    bool createBranch(const std::string& baseVersion, const std::string& branchName);
    bool tagVersion(const std::string& versionName, const std::string& tag);
    bool revertVersion(const std::string& versionName, const std::string& targetCheckpoint);
    
    // Version management
    bool deleteVersion(const std::string& versionName);
    std::vector<std::string> listVersions() const;
    std::optional<VersionControlEntry> getVersion(const std::string& versionName) const;
    std::vector<std::string> getBranches() const;
    std::vector<VersionControlEntry> getVersionHistory(const std::string& versionName, size_t limit = 10) const;
    
    // Version comparison
    bool compareVersions(const std::string& version1, const std::string& version2, std::string& diffOutput);
    std::vector<std::string> findCommonAncestors(const std::string& version1, const std::string& version2);
    
    // Branch management
    bool mergeBranch(const std::string& branchName, const std::string& targetVersion);
    bool switchBranch(const std::string& branchName);
    bool listBranches() const;
    
    // Statistics and monitoring
    VersionControlStats getStats() const { return stats_; }
    
    bool isHealthy() const { return errorCount_ == 0; }
    
    // Event handling
    using VersionControlEventCallback = std::function<void(const std::string&, const VersionControlEntry&)>;
    void setVersionControlEventCallback(VersionControlEventCallback callback) { 
        versionControlEventCallback_ = callback; 
    }
    
    // Thread safety
    void lock() const { mutex_.lock(); }
    void unlock() const { mutex_.unlock(); }
    bool tryLock() const { return mutex_.try_lock(); }

private:
    bool initialize();
    void cleanup();
    
    bool createVersionImpl(const VersionControlEntry& entry);
    bool mergeVersionsImpl(const std::string& targetVersion, const std::vector<std::string>& sourceVersions);
    bool createBranchImpl(const std::string& baseVersion, const std::string& branchName);
    
    void updateStats();
    void logEvent(const std::string& operation, const std::string& versionName,
                  const VersionControlEntry& entry);
    
    std::string getVersionPath(const std::string& versionName) const;
    std::string getBranchPath(const std::string& branchName) const;
    
    // Components
    std::unique_ptr<CheckpointIndexer> checkpointIndexer_;
    
    // Configuration
    std::string versionDir_;
    std::shared_ptr<Config> config_;
    
    // State
    mutable std::mutex mutex_;
    uint32_t errorCount_;
    
    // Statistics
    VersionControlStats stats_;
    
    // Events
    VersionControlEventCallback versionControlEventCallback_;
};

} // namespace nlm
