//! Plugin Security Manager for NLM Neural Simulation Project
//! 
//! This module implements the PluginSecurityManager class which provides
//! security management for plugins. The PluginSecurityManager:
//! - Manages plugin security policies and permissions
//! - Provides plugin sandboxing and isolation
//! - Validates plugin security certificates
//! - Manages plugin access controls
//! - Monitors plugin security violations
//! - Provides security logging and auditing
//! - Integrates with the NLM security system
//! - Supports security templates and defaults
//! - Manages plugin security lifecycle
//! 
//! The PluginSecurityManager ensures that plugins operate within
//! defined security boundaries while maintaining system integrity.

#pragma once

#include "PluginBase.hpp"
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <mutex>
#include <any>
#include <unordered_set>
#include <chrono>
#include <algorithm>
#include <set>
#include <bitset>

namespace nlm {

class PluginSecurityManager {
public:
    PluginSecurityManager();
    ~PluginSecurityManager();
    
    // Disable copying
    PluginSecurityManager(const PluginSecurityManager&) = delete;
    PluginSecurityManager& operator=(const PluginSecurityManager&) = delete;
    
    // Enable moving
    PluginSecurityManager(PluginSecurityManager&&) noexcept = default;
    PluginSecurityManager& operator=(PluginSecurityManager&&) noexcept = default;
    
    // Security policy management
    bool setSecurityPolicy(const std::string& pluginName, const std::string& policy);
    bool removeSecurityPolicy(const std::string& pluginName);
    std::string getSecurityPolicy(const std::string& pluginName) const;
    std::vector<std::string> getSecurityPolicies() const;
    bool clearAllSecurityPolicies();
    
    // Permission management
    bool grantPermission(const std::string& pluginName, const std::string& permission);
    bool revokePermission(const std::string& pluginName, const std::string& permission);
    bool hasPermission(const std::string& pluginName, const std::string& permission) const;
    bool hasAnyPermission(const std::string& pluginName, const std::vector<std::string>& permissions) const;
    std::vector<std::string> getPluginPermissions(const std::string& pluginName) const;
    std::unordered_map<std::string, std::vector<std::string>> getAllPluginPermissions() const;
    
    // Security sandboxing
    bool createSandbox(const std::string& pluginName, const SecuritySandboxConfig& config);
    bool removeSandbox(const std::string& pluginName);
    bool isSandboxed(const std::string& pluginName) const;
    SecuritySandboxConfig getSandboxConfig(const std::string& pluginName) const;
    bool updateSandboxConfig(const std::string& pluginName, const SecuritySandboxConfig& config);
    
    // Security certificate management
    bool issueSecurityCertificate(const std::string& pluginName, const SecurityCertificate& certificate);
    bool revokeSecurityCertificate(const std::string& pluginName);
    bool validateSecurityCertificate(const std::string& pluginName) const;
    SecurityCertificate getSecurityCertificate(const std::string& pluginName) const;
    std::vector<SecurityCertificate> getSecurityCertificates() const;
    
    // Security violation monitoring
    bool reportSecurityViolation(const SecurityViolation& violation);
    std::vector<SecurityViolation> getSecurityViolations() const;
    std::vector<SecurityViolation> getSecurityViolations(const std::string& pluginName) const;
    bool clearSecurityViolations(const std::string& pluginName);
    bool clearSecurityViolations();
    
    // Security access control
    bool setAccessControl(const std::string& pluginName, const AccessControl& accessControl);
    bool removeAccessControl(const std::string& pluginName);
    AccessControl getAccessControl(const std::string& pluginName) const;
    bool checkAccessControl(const std::string& pluginName, const AccessRequest& request) const;
    
    // Security validation
    bool validatePluginSecurity(const PluginMetadata& metadata) const;
    std::vector<std::string> getPluginSecurityValidationErrors(const PluginMetadata& metadata) const;
    bool validatePluginSecurity(const std::string& pluginName) const;
    std::vector<std::string> getPluginSecurityErrors(const std::string& pluginName) const;
    
    // Security auditing
    bool enableSecurityLogging(bool enable);
    bool isSecurityLoggingEnabled() const;
    void logSecurityEvent(const SecurityEvent& event);
    std::vector<SecurityEvent> getSecurityEvents() const;
    std::vector<SecurityEvent> getSecurityEvents(const std::string& pluginName) const;
    std::vector<SecurityEvent> getSecurityEvents(const std::string& pluginName, SecurityEventType eventType) const;
    
    // Security system integration
    void setNLMConfig(std::shared_ptr<Config> nlmConfig);
    std::shared_ptr<Config> getNLMConfig() const;
    bool syncWithNLMConfig(const std::string& securityPrefix = "");
    bool applyNLMConfigToSecurity(const std::string& securityPrefix = "");
    
    // Security templates and defaults
    bool registerSecurityTemplate(const std::string& templateName, const SecurityPolicy& templatePolicy);
    bool unregisterSecurityTemplate(const std::string& templateName);
    std::unique_ptr<PluginSecurityManager> createFromSecurityTemplate(const std::string& templateName,
                                                                   const std::unordered_map<std::string, std::any>& parameters = {});
    std::vector<std::string> getRegisteredSecurityTemplateNames() const;
    
    // Security statistics
    struct SecurityStats {
        size_t totalPlugins;
        size_t sandboxedPlugins;
        size_t certificateValidatedPlugins;
        size_t securityViolations;
        size_t securityEvents;
        double securityScore;
        
        SecurityStats() : totalPlugins(0), sandboxedPlugins(0), certificateValidatedPlugins(0),
            securityViolations(0), securityEvents(0), securityScore(0.0) {}
    };
    
    SecurityStats getSecurityStats() const;
    
    // Security lifecycle
    void pauseSecurityMonitoring();
    void resumeSecurityMonitoring();
    void shutdownSecurityManager();
    bool isSecurityManagerActive() const;
    
    // Security system integration
    void setPluginManager(std::shared_ptr<PluginManager> pluginManager);
    void setRegistry(std::shared_ptr<PluginRegistry> registry);
    void setConfigurationManager(std::shared_ptr<PluginConfiguration> configurationManager);
    void setUpdateManager(std::shared_ptr<PluginUpdateManager> updateManager);
    void setHookSystem(std::shared_ptr<HookSystem> hookSystem);
    void setEventSystem(std::shared_ptr<EventSystem> eventSystem);
    void setFactory(std::shared_ptr<ComponentFactory> factory);
    
    // Security debugging and monitoring
    void enableDebugLogging(bool enable);
    bool isDebugLoggingEnabled() const;
    void logSecurityPolicyChange(const std::string& pluginName, const std::string& oldPolicy, const std::string& newPolicy) const;
    void logSecurityPermissionChange(const std::string& pluginName, const std::string& permission, bool granted) const;
    void logSecurityViolation(const SecurityViolation& violation) const;
    void logSecuritySystemError(const std::string& error) const;
    
    // Security system optimization
    void optimizeSecurityPolicies();
    void consolidateSecuritySandboxes();
    void cleanInvalidSecurityCertificates();
    void balanceSecurityMonitoring();
    
    // Security system export/import
    bool exportSecurityConfiguration(const std::string& filePath) const;
    bool importSecurityConfiguration(const std::string& filePath);
    std::string exportSecurityConfigurationAsJSON() const;
    bool importSecurityConfigurationFromJSON(const std::string& jsonData);
    
    // Security system testing
    bool testSecuritySystem() const;
    std::vector<std::string> getSecurityTestResults() const;
    bool testSecurityPolicy(const std::string& pluginName) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void initializeImpl();
    void cleanupImpl();
    
    // Thread-safe operations
    template<typename T>
    T withSecurityLock(T func) {
        std::unique_lock<std::mutex> lock(securityMutex_);
        return func();
    }
    
    // Security validation
    bool validateSecurityPolicy(const std::string& policy) const;
    bool validateSecurityCertificate(const SecurityCertificate& certificate) const;
    bool validateSandboxConfig(const SecuritySandboxConfig& config) const;
    
    // Security violation detection
    bool detectSecurityViolation(const std::string& pluginName, const SecurityViolation& violation) const;
    
    // Security logging
    void logSecurityAuditEvent(const SecurityEvent& event);
    
    // Security statistics
    void updateSecurityStatistics();
    double calculateSecurityScore() const;
    
    // Security cleanup
    void cleanupExpiredSecurityCertificates();
    void cleanupInvalidSecurityPolicies();
    
    // Security policy optimization
    void optimizeSecurityPolicyOrder();
    void consolidateDuplicateSecurityPolicies();
}; // namespace nlm

// Security-related data structures

struct SecurityPolicy {
    std::string name;
    std::string description;
    std::bitset<64> permissions; // Bitmask of permissions
    std::unordered_map<std::string, std::any> constraints;
    bool allowNetworkAccess;
    bool allowFileSystemAccess;
    bool allowMemoryAccess;
    bool allowCodeExecution;
    bool enableSandboxing;
    bool requireAuthentication;
    std::string authenticationMethod;
    std::chrono::system_clock::time_point created;
    std::chrono::system_clock::time_point expires;
    bool isTemporary;
    
    SecurityPolicy() : allowNetworkAccess(false), allowFileSystemAccess(false),
        allowMemoryAccess(false), allowCodeExecution(false), enableSandboxing(true),
        requireAuthentication(false), isTemporary(false),
        created(std::chrono::system_clock::now()), expires(std::chrono::system_clock::now()) {}
};

struct SecurityCertificate {
    std::string pluginName;
    std::string issuer;
    PluginVersion pluginVersion;
    std::chrono::system_clock::time_point issued;
    std::chrono::system_clock::time_point expires;
    std::string signature;
    std::unordered_map<std::string, std::any> metadata;
    bool isValid;
    
    SecurityCertificate() : pluginVersion(1, 0, 0), isValid(false),
        issued(std::chrono::system_clock::now()),
        expires(std::chrono::system_clock::now()) {}
};

struct SecuritySandboxConfig {
    std::string pluginName;
    std::string sandboxPath;
    size_t memoryLimit;
    size_t fileSystemLimit;
    std::vector<std::string> allowedFiles;
    std::vector<std::string> blockedFiles;
    std::vector<std::string> allowedNetworkHosts;
    std::vector<std::string> blockedNetworkHosts;
    std::unordered_map<std::string, std::any> environmentVariables;
    bool enableSystemCalls;
    bool enableExternalInterfaces;
    
    SecuritySandboxConfig() : memoryLimit(1024 * 1024 * 1024), // 1GB default
        fileSystemLimit(1024 * 1024 * 1024), enableSystemCalls(false),
        enableExternalInterfaces(false) {}
};

struct SecurityViolation {
    std::string violationId;
    std::string pluginName;
    SecurityViolationType violationType;
    std::string description;
    std::chrono::system_clock::time_point timestamp;
    std::string severity;
    std::unordered_map<std::string, std::any> context;
    bool isResolved;
    
    SecurityViolation() : violationType(SecurityViolationType::Unknown), isResolved(false),
        timestamp(std::chrono::system_clock::now()) {}
};

struct AccessControl {
    std::string pluginName;
    std::vector<AccessRule> accessRules;
    bool requireExplicitGrant;
    bool denyByDefault;
    std::unordered_map<std::string, std::any> defaultPermissions;
    
    AccessControl() : requireExplicitGrant(true), denyByDefault(false) {}
};

struct AccessRequest {
    std::string pluginName;
    std::string resource;
    AccessType accessType;
    std::unordered_map<std::string, std::any> parameters;
    std::string requestId;
    std::chrono::system_clock::time_point timestamp;
    std::string requester;
    
    AccessRequest() : accessType(AccessType::Read), timestamp(std::chrono::system_clock::now()) {}
};

struct SecurityEvent {
    std::string eventId;
    std::string pluginName;
    SecurityEventType eventType;
    std::chrono::system_clock::time_point timestamp;
    std::string severity;
    std::string description;
    std::unordered_map<std::string, std::any> context;
    std::string errorMessage;
    
    SecurityEvent() : eventType(SecurityEventType::Unknown),
        timestamp(std::chrono::system_clock::now()) {}
};

// Enum definitions for security-related types
enum class SecurityViolationType {
    Unknown,
    UnauthorizedAccess,
    PermissionEscalation,
    CodeInjection,
    ResourceExhaustion,
    NetworkPolicyViolation,
    FileSystemPolicyViolation,
    MemoryAccessViolation,
    CertificateValidationFailure,
    SandboxEscape,
    SecurityPolicyViolation,
    ConfigurationTampering
};

enum class SecurityEventType {
    Unknown,
    PolicyChange,
    PermissionGrant,
    PermissionRevoke,
    CertificateIssued,
    CertificateRevoked,
    SecurityViolationReported,
    SecurityViolationResolved,
    SandboxCreated,
    SandboxRemoved,
    AccessControlUpdated,
    SecurityPolicyUpdated
};

enum class AccessType {
    Read,
    Write,
    Execute,
    Create,
    Delete,
    Modify,
    Configure
};

// Constants for security
namespace SecurityConstants {
    const size_t DEFAULT_MEMORY_LIMIT = 1024 * 1024 * 1024; // 1GB
    const size_t DEFAULT_FILE_SYSTEM_LIMIT = 1024 * 1024 * 1024; // 1GB
    const int DEFAULT_CERTIFICATE_VALIDITY_DAYS = 365;
    const int DEFAULT_POLICY_VALIDITY_DAYS = 180;
    const double DEFAULT_SECURITY_SCORE_THRESHOLD = 0.8;
    const int DEFAULT_MAX_SECURITY_VIOLATIONS = 10;
    const std::string DEFAULT_SECURITY_POLICY = "default";
    const std::string DEFAULT_AUTHENTICATION_METHOD = "certificate";
}