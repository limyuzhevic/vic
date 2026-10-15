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

// Security levels
enum class SecurityLevel {
    Public,
    Internal,
    Confidential,
    Restricted,
    TopSecret
};

// Access control permissions
enum class AccessPermission {
    Read,
    Write,
    Execute,
    Create,
    Delete,
    Share,
    Admin,
    Backup,
    Restore
};

// Security event types
enum class SecurityEventType {
    AccessDenied,
    AuthenticationFailure,
    AuthorizationFailure,
    DataBreach,
    SecurityPolicyViolation,
    AuditLogUpdate,
    SecurityAlert
};

// Security audit entry
struct SecurityAuditEntry {
    std::string entryId;
    std::string experimentName;
    std::string userId;
    SecurityEventType eventType;
    std::chrono::system_clock::time_point timestamp;
    std::string sourceIp;
    std::string userAgent;
    std::string action;
    std::string resource;
    std::string result; // "success", "failure", "error"
    std::map<std::string, std::string> metadata;
    std::string severity; // "low", "medium", "high", "critical"
    
    SecurityAuditEntry() : timestamp(std::chrono::system_clock::now()) {}
    SecurityAuditEntry(const std::string& entryId, const std::string& experimentName,
                      const std::string& userId, SecurityEventType eventType,
                      const std::string& action, const std::string& resource,
                      const std::string& result)
        : entryId(entryId), experimentName(experimentName), userId(userId),
          eventType(eventType), timestamp(std::chrono::system_clock::now()),
          action(action), resource(resource), result(result), severity("medium") {}
};

// Security policy
struct SecurityPolicy {
    std::string policyId;
    std::string name;
    std::string description;
    SecurityLevel requiredSecurityLevel;
    std::vector<AccessPermission> allowedPermissions;
    std::vector<AccessPermission> deniedPermissions;
    std::map<std::string, std::string> conditions;
    bool isActive;
    std::chrono::system_clock::time_point created;
    std::chrono::system_clock::time_point lastModified;
    std::string createdBy;
    
    SecurityPolicy() : isActive(true), created(std::chrono::system_clock::now()), 
                      lastModified(std::chrono::system_clock::now()) {}
    SecurityPolicy(const std::string& id, const std::string& name, const std::string& desc)
        : policyId(id), name(name), description(desc),
          requiredSecurityLevel(SecurityLevel::Internal), isActive(true),
          created(std::chrono::system_clock::now()), lastModified(std::chrono::system_clock::now()) {}
};

// Security alert
struct SecurityAlert {
    std::string alertId;
    std::string experimentName;
    SecurityEventType alertType;
    std::string severity; // "low", "medium", "high", "critical"
    std::chrono::system_clock::time_point timestamp;
    std::string message;
    std::map<std::string, std::string> alertData;
    bool isAcknowledged;
    bool isResolved;
    std::string source;
    
    SecurityAlert() : timestamp(std::chrono::system_clock::now()), isAcknowledged(false), isResolved(false) {}
    SecurityAlert(const std::string& id, const std::string& experimentName,
                 SecurityEventType type, const std::string& msg, const std::string& severity)
        : alertId(id), experimentName(experimentName), alertType(type), severity(severity),
          timestamp(std::chrono::system_clock::now()), message(msg),
          isAcknowledged(false), isResolved(false) {}
};

class ExperimentSecurity {
public:
    ExperimentSecurity();
    ~ExperimentSecurity();
    
    // Disable copying, enable moving
    ExperimentSecurity(const ExperimentSecurity&) = delete;
    ExperimentSecurity& operator=(const ExperimentSecurity&) = delete;
    ExperimentSecurity(ExperimentSecurity&&) noexcept;
    ExperimentSecurity& operator=(ExperimentSecurity&&) noexcept;
    
    // Security management
    bool initializeSecurity(const std::string& securityConfigPath = "");
    bool shutdownSecurity();
    bool isSecurityInitialized() const;
    
    // Policy management
    bool addSecurityPolicy(const SecurityPolicy& policy);
    bool removeSecurityPolicy(const std::string& policyId);
    bool updateSecurityPolicy(const std::string& policyId, const SecurityPolicy& policy);
    SecurityPolicy getSecurityPolicy(const std::string& policyId) const;
    std::vector<SecurityPolicy> getAllSecurityPolicies() const;
    std::vector<SecurityPolicy> getActiveSecurityPolicies() const;
    
    // Access control
    bool enforceAccessControl(const std::string& experimentName, const std::string& userId,
                             AccessPermission permission) const;
    bool grantAccess(const std::string& experimentName, const std::string& userId,
                   AccessPermission permission, const std::string& grantedBy);
    bool revokeAccess(const std::string& experimentName, const std::string& userId,
                     AccessPermission permission);
    bool setSecurityLevel(const std::string& experimentName, SecurityLevel level);
    SecurityLevel getSecurityLevel(const std::string& experimentName) const;
    
    // Security auditing
    bool logSecurityEvent(const SecurityAuditEntry& auditEntry);
    std::vector<SecurityAuditEntry> getSecurityAuditLogs(const std::string& experimentName) const;
    std::vector<SecurityAuditEntry> getSecurityAuditLogs(const std::string& experimentName,
                                                        const std::string& userId) const;
    bool clearSecurityAuditLogs(const std::string& experimentName);
    
    // Security alerts
    bool triggerSecurityAlert(const SecurityAlert& alert);
    bool acknowledgeSecurityAlert(const std::string& alertId);
    bool resolveSecurityAlert(const std::string& alertId);
    SecurityAlert getSecurityAlert(const std::string& alertId) const;
    std::vector<SecurityAlert> getActiveSecurityAlerts(const std::string& experimentName) const;
    std::vector<SecurityAlert> getAllActiveSecurityAlerts() const;
    
    // Security compliance
    bool validateSecurityCompliance(const std::string& experimentName) const;
    bool checkSecurityPolicyViolations(const std::string& experimentName) const;
    std::vector<std::string> getSecurityPolicyViolations(const std::string& experimentName) const;
    
    // Security backup and recovery
    bool backupSecurityData(const std::string& backupPath) const;
    bool restoreSecurityData(const std::string& backupPath);
    bool exportSecurityToJSON(const std::string& filepath) const;
    bool importSecurityFromJSON(const std::string& filepath);
    
    // Security cleanup
    bool cleanupSecurityLogs(const std::chrono::system_clock::time_point& cutoffTime);
    bool cleanupExpiredSecurityAlerts(const std::chrono::system_clock::time_point& cutoffTime);
    bool resetSecurity(const std::string& experimentName);
    bool clearAllSecurityData();
    
    // Security system management
    bool enableSecurityMonitoring(const std::string& experimentName, bool enable);
    bool isSecurityMonitoringEnabled(const std::string& experimentName) const;
    bool enableRealTimeSecurityAlerts(bool enable);
    bool isRealTimeSecurityAlertsEnabled() const;
    
    // Security utilities
    static std::string generateSecurityToken() const;
    static bool validateSecurityToken(const std::string& token) const;
    static SecurityLevel stringToSecurityLevel(const std::string& levelStr) const;
    static std::string securityLevelToString(SecurityLevel level) const;
    static AccessPermission stringToAccessPermission(const std::string& permissionStr) const;
    static std::string accessPermissionToString(AccessPermission permission) const;
    static SecurityEventType stringToSecurityEventType(const std::string& eventTypeStr) const;
    static std::string securityEventTypeToString(SecurityEventType eventType) const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm