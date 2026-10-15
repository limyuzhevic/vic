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

// Sharing permission level
enum class SharingPermission {
    Read,
    Write,
    Execute,
    Admin,
    Share,
    Collaborate,
    Comment,
    Download,
    Upload,
    Delete
};

// Sharing status
enum class SharingStatus {
    Active,
    Suspended,
    Expired,
    Revoked,
    Pending,
    Rejected
};

// Shared experiment metadata
struct SharedExperimentMetadata {
    std::string experimentName;
    std::string sharedBy;
    std::string sharedWith;
    SharingPermission permission;
    SharingStatus status;
    std::chrono::system_clock::time_point sharedTime;
    std::chrono::system_clock::time_point expiresTime;
    std::string accessToken;
    std::map<std::string, std::string> metadata;
    
    SharedExperimentMetadata() : status(SharingStatus::Active), 
                                sharedTime(std::chrono::system_clock::now()),
                                expiresTime(std::chrono::system_clock::now() + std::chrono::days(30)) {}
    SharedExperimentMetadata(const std::string& expName, const std::string& sharedBy, 
                            const std::string& sharedWith, SharingPermission perm)
        : experimentName(expName), sharedBy(sharedBy), sharedWith(sharedWith), 
          permission(perm), status(SharingStatus::Active),
          sharedTime(std::chrono::system_clock::now()),
          expiresTime(std::chrono::system_clock::now() + std::chrono::days(30)) {}
};

// Shared experiment access log
struct SharedExperimentAccessLog {
    std::string experimentName;
    std::string userId;
    std::chrono::system_clock::time_point accessTime;
    std::string action; // "read", "write", "execute", "download", "upload"
    std::map<std::string, std::string> accessData;
    
    SharedExperimentAccessLog() : accessTime(std::chrono::system_clock::now()) {}
    SharedExperimentAccessLog(const std::string& expName, const std::string& userId, 
                             const std::string& action)
        : experimentName(expName), userId(userId), action(action),
          accessTime(std::chrono::system_clock::now()) {}
};

class ExperimentSharing {
public:
    ExperimentSharing();
    ~ExperimentSharing();
    
    // Disable copying, enable moving
    ExperimentSharing(const ExperimentSharing&) = delete;
    ExperimentSharing& operator=(const ExperimentSharing&) = delete;
    ExperimentSharing(ExperimentSharing&&) noexcept;
    ExperimentSharing& operator=(ExperimentSharing&&) noexcept;
    
    // Sharing management
    bool shareExperiment(const std::string& experimentName, const std::string& sharedWith, 
                        SharingPermission permission, 
                        const std::chrono::system_clock::time_point& expiresTime = std::chrono::system_clock::now() + std::chrono::days(30));
    bool unshareExperiment(const std::string& experimentName, const std::string& sharedWith);
    bool updateSharingPermission(const std::string& experimentName, const std::string& sharedWith, 
                                SharingPermission permission);
    bool suspendSharing(const std::string& experimentName, const std::string& sharedWith, 
                       const std::string& reason);
    bool restoreSharing(const std::string& experimentName, const std::string& sharedWith);
    
    // Access control
    bool grantAccess(const std::string& experimentName, const std::string& userId, 
                    SharingPermission permission);
    bool revokeAccess(const std::string& experimentName, const std::string& userId);
    bool checkAccess(const std::string& experimentName, const std::string& userId, 
                    SharingPermission requiredPermission) const;
    SharingPermission getUserExperimentPermission(const std::string& experimentName, 
                                                  const std::string& userId) const;
    
    // Access logging
    bool logAccess(const SharedExperimentAccessLog& accessLog);
    std::vector<SharedExperimentAccessLog> getAccessLogs(const std::string& experimentName) const;
    std::vector<SharedExperimentAccessLog> getAccessLogs(const std::string& experimentName, 
                                                        const std::string& userId) const;
    
    // Sharing management
    SharedExperimentMetadata getSharingMetadata(const std::string& experimentName, 
                                                const std::string& sharedWith) const;
    std::vector<SharedExperimentMetadata> getSharingMetadata(const std::string& experimentName) const;
    std::vector<SharedExperimentMetadata> getSharedWithMe(const std::string& userId) const;
    
    // Sharing analytics
    size_t getAccessCount(const std::string& experimentName, const std::string& userId) const;
    std::chrono::system_clock::time_point getLastAccessTime(const std::string& experimentName, 
                                                          const std::string& userId) const;
    std::map<std::string, size_t> getSharingStatistics(const std::string& experimentName) const;
    
    // Sharing backup and recovery
    bool backupSharingData(const std::string& backupPath) const;
    bool restoreSharingData(const std::string& backupPath);
    bool exportSharingToJSON(const std::string& filepath) const;
    bool importSharingFromJSON(const std::string& filepath);
    
    // Sharing cleanup
    bool cleanupExpiredSharing(const std::chrono::system_clock::time_point& cutoffTime);
    bool cleanupSuspendedSharing(const std::chrono::system_clock::time_point& cutoffTime);
    bool resetExperimentSharing(const std::string& experimentName);
    bool clearAllSharingData();
    
    // Sharing system management
    bool initializeSharingSystem();
    bool shutdownSharingSystem();
    bool isSharingSystemInitialized() const;
    
    // Sharing utilities
    static std::string generateAccessToken() const;
    static SharingPermission stringToPermission(const std::string& permissionStr) const;
    static std::string permissionToString(SharingPermission permission) const;
    static SharingStatus stringToStatus(const std::string& statusStr) const;
    static std::string statusToString(SharingStatus status) const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm