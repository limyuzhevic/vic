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

// Fork type
enum class ForkType {
    Branch,
    Clone,
    Fork,
    Split,
    Merge,
    Patch,
    Template
};

// Fork status
enum class ForkStatus {
    Pending,
    Creating,
    Created,
    Failed,
    Merged,
    Discarded,
    Archived
};

// Fork configuration
struct ForkConfig {
    std::string forkName;
    ForkType forkType;
    std::string sourceExperimentName;
    std::string sourceVersionId;
    std::string targetExperimentName;
    std::string targetVersionId;
    std::map<std::string, std::string> forkParameters;
    std::chrono::system_clock::time_point created;
    std::chrono::system_clock::time_point lastModified;
    std::string createdBy;
    ForkStatus status;
    std::map<std::string, std::string> forkMetadata;
    
    ForkConfig() : created(std::chrono::system_clock::now()), 
                  lastModified(std::chrono::system_clock::now()),
                  status(ForkStatus::Pending) {}
    ForkConfig(const std::string& name, ForkType type, const std::string& sourceExperiment, 
              const std::string& sourceVersion)
        : forkName(name), forkType(type), sourceExperimentName(sourceExperiment), 
          sourceVersionId(sourceVersion), created(std::chrono::system_clock::now()), 
          lastModified(std::chrono::system_clock::now()),
          status(ForkStatus::Pending) {}
};

// Fork merge conflict
struct ForkMergeConflict {
    std::string conflictId;
    std::string fileName;
    std::string sourceChange;
    std::string targetChange;
    std::string conflictResolution;
    std::string conflictStatus; // "unresolved", "resolved", "ignored"
    std::chrono::system_clock::time_point detectedTime;
    std::string detectedBy;
    
    ForkMergeConflict() : detectedTime(std::chrono::system_clock::now()) {}
    ForkMergeConflict(const std::string& id, const std::string& fileName, 
                     const std::string& sourceChange, const std::string& targetChange)
        : conflictId(id), fileName(fileName), sourceChange(sourceChange), 
          targetChange(targetChange), detectedTime(std::chrono::system_clock::now()),
          conflictStatus("unresolved") {}
};

// Fork branch
struct ForkBranch {
    std::string branchId;
    std::string experimentName;
    std::string branchName;
    std::string baseVersionId;
    std::vector<std::string> commitIds;
    std::chrono::system_clock::time_point created;
    std::chrono::system_clock::time_point lastUpdated;
    std::string createdBy;
    std::map<std::string, std::string> branchMetadata;
    
    ForkBranch() : created(std::chrono::system_clock::now()), 
                  lastUpdated(std::chrono::system_clock::now()) {}
    ForkBranch(const std::string& id, const std::string& experimentName, 
              const std::string& branchName, const std::string& baseVersionId)
        : branchId(id), experimentName(experimentName), branchName(branchName), 
          baseVersionId(baseVersionId), created(std::chrono::system_clock::now()), 
          lastUpdated(std::chrono::system_clock::now()) {}
};

class ExperimentForking {
public:
    ExperimentForking();
    ~ExperimentForking();
    
    // Disable copying, enable moving
    ExperimentForking(const ExperimentForking&) = delete;
    ExperimentForking& operator=(const ExperimentForking&) = delete;
    ExperimentForking(ExperimentForking&&) noexcept;
    ExperimentForking& operator=(ExperimentForking&&) noexcept;
    
    // Fork management
    bool createFork(const std::string& forkName, ForkType type, 
                   const std::string& sourceExperimentName,
                   const std::string& sourceVersionId = "");
    bool createFork(const ForkConfig& config);
    bool removeFork(const std::string& forkId);
    bool updateFork(const std::string& forkId, const ForkConfig& config);
    ForkConfig getFork(const std::string& forkId) const;
    std::vector<ForkConfig> getAllForks() const;
    std::vector<ForkConfig> getForksByExperiment(const std::string& experimentName) const;
    
    // Fork execution
    bool createBranch(const std::string& forkId);
    bool cloneExperiment(const std::string& sourceExperimentName, 
                       const std::string& targetExperimentName,
                       const std::string& targetVersionId = "");
    bool forkExperiment(const std::string& sourceExperimentName, 
                       const std::string& targetExperimentName,
                       const std::map<std::string, std::string>& forkParameters = {});
    bool splitExperiment(const std::string& sourceExperimentName,
                        const std::vector<std::string>& splitNames,
                        const std::vector<std::string>& splitConfigs = {});
    bool mergeFork(const std::string& sourceForkId, const std::string& targetForkId);
    bool resolveMergeConflict(const std::string& conflictId, 
                            const std::string& resolution);
    
    // Branch management
    bool createBranch(const std::string& branchName, const std::string& experimentName, 
                     const std::string& baseVersionId = "");
    bool removeBranch(const std::string& branchId);
    bool updateBranch(const std::string& branchId, const ForkBranch& branch);
    ForkBranch getBranch(const std::string& branchId) const;
    std::vector<ForkBranch> getAllBranches() const;
    std::vector<ForkBranch> getBranchesByExperiment(const std::string& experimentName) const;
    bool checkoutBranch(const std::string& branchId);
    bool mergeBranch(const std::string& sourceBranchId, const std::string& targetBranchId);
    
    // Fork monitoring
    ForkStatus getForkStatus(const std::string& forkId) const;
    std::chrono::system_clock::time_point getForkCreatedTime(const std::string& forkId) const;
    std::chrono::system_clock::time_point getForkModifiedTime(const std::string& forkId) const;
    std::string getForkCreatedBy(const std::string& forkId) const;
    
    // Fork results
    bool storeForkResults(const std::string& forkId, const std::string& experimentName,
                         const Config& config, const std::string& versionId = "");
    Config getForkResults(const std::string& forkId, const std::string& experimentName) const;
    std::string getForkResultsVersionId(const std::string& forkId, const std::string& experimentName) const;
    
    // Fork analysis
    std::vector<ForkMergeConflict> getMergeConflicts(const std::string& forkId) const;
    bool resolveMergeConflict(const std::string& conflictId, const std::string& resolution);
    bool ignoreMergeConflict(const std::string& conflictId);
    std::map<std::string, std::string> getForkDifferences(const std::string& forkId1, 
                                                         const std::string& forkId2) const;
    
    // Fork backup and recovery
    bool backupForkData(const std::string& backupPath) const;
    bool restoreForkData(const std::string& backupPath);
    bool exportForkToJSON(const std::string& filepath) const;
    bool importForkFromJSON(const std::string& filepath);
    
    // Fork cleanup
    bool cleanupFailedForks(const std::chrono::system_clock::time_point& cutoffTime);
    bool cleanupDiscardedForks(const std::chrono::system_clock::time_point& cutoffTime);
    bool cleanupArchivedForks(const std::chrono::system_clock::time_point& cutoffTime);
    bool resetFork(const std::string& forkId);
    bool clearAllForks();
    
    // Fork system management
    bool initializeForkingSystem();
    bool shutdownForkingSystem();
    bool isForkingSystemInitialized() const;
    
    // Fork utilities
    static std::string generateForkId(const std::string& experimentName) const;
    static std::string generateBranchId(const std::string& experimentName) const;
    static ForkType stringToForkType(const std::string& typeStr) const;
    static std::string forkTypeToString(ForkType type) const;
    static ForkStatus stringToForkStatus(const std::string& statusStr) const;
    static std::string forkStatusToString(ForkStatus status) const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm