#pragma once

// Plugin Security System for NLM (Neural Learning Machine)
// Provides plugin security validation and sandboxing

#include "PluginCore.hpp"
#include <vector>
#include <unordered_set>
#include <chrono>
#include <memory>

namespace nlm {

// Plugin Permission
enum class PluginPermission {
    Read,           // Read-only access to data
    Write,          // Write access to data
    Execute,        // Execute code
    Network,        // Network access
    File,           // File system access
    System,         // System access
    Debug,          // Debug access
    Admin,          // Administrator access
    Safe,           // Safe/sandboxed access
    Restricted,     // Restricted access
    None            // No access
};

// Permission Set
using PluginPermissionSet = std::unordered_set<PluginPermission>;

// Plugin Security Policy
struct PluginSecurityPolicy {
    std::string name;
    std::string description;
    PluginSecurityLevel level;
    PluginPermissionSet requiredPermissions;
    PluginPermissionSet forbiddenPermissions;
    std::vector<std::string> allowedPlugins;
    std::vector<std::string> forbiddenPlugins;
    std::map<std::string, PluginConfigValue> securityRules;
    bool enableLogging;
    bool enableAuditing;
    bool enableIsolation;
    bool enableSandboxing;
    
    bool hasPermission(PluginPermission permission) const {
        return requiredPermissions.find(permission) != requiredPermissions.end() &&
               forbiddenPermissions.find(permission) == forbiddenPermissions.end();
    }
    
    bool isPluginAllowed(const std::string& pluginName) const {
        if (allowedPlugins.empty()) {
            return true; // No restrictions
        }
        return std::find(allowedPlugins.begin(), allowedPlugins.end(), pluginName) != allowedPlugins.end();
    }
    
    bool isPluginForbidden(const std::string& pluginName) const {
        return std::find(forbiddenPlugins.begin(), forbiddenPlugins.end(), pluginName) != forbiddenPlugins.end();
    }
};

// Plugin Security Context
struct PluginSecurityContext {
    std::string pluginName;
    std::shared_ptr<Plugin> plugin;
    PluginSecurityLevel level;
    PluginPermissionSet activePermissions;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point lastAccessTime;
    std::map<std::string, std::string> accessLog;
    bool isolated;
    bool sandboxed;
    
    PluginSecurityContext() : isolated(false), sandboxed(false) {
        startTime = std::chrono::system_clock::now();
        lastAccessTime = startTime;
    }
};

// Plugin Security Violation
struct PluginSecurityViolation {
    std::string pluginName;
    PluginSecurityViolationType type;
    std::string description;
    std::chrono::system_clock::time_point timestamp;
    std::string severity; // "info", "warning", "error", "critical"
    std::map<std::string, std::string> details;
    
    PluginSecurityViolation() {
        timestamp = std::chrono::system_clock::now();
        severity = "info";
    }
};

// Plugin Security Auditor
class PluginSecurityAuditor {
public:
    virtual ~PluginSecurityAuditor() = default;
    virtual bool auditPlugin(const Plugin& plugin) = 0;
    virtual bool auditPluginAccess(const PluginSecurityContext& context, const std::string& resource) = 0;
    virtual bool auditPluginBehavior(const Plugin& plugin, const std::map<std::string, std::string>& behavior) = 0;
    virtual std::vector<PluginSecurityViolation> getViolations(const std::string& pluginName) const = 0;
    virtual void clearViolations(const std::string& pluginName) = 0;
};

// Plugin Sandbox
class PluginSandbox {
public:
    virtual ~PluginSandbox() = default;
    virtual bool isolatePlugin(const std::string& pluginName) = 0;
    virtual bool sandboxPlugin(const std::string& pluginName) = 0;
    virtual bool removeIsolation(const std::string& pluginName) = 0;
    virtual bool removeSandboxing(const std::string& pluginName) = 0;
    virtual bool isIsolated(const std::string& pluginName) const = 0;
    virtual bool isSandboxed(const std::string& pluginName) const = 0;
    virtual std::map<std::string, std::string> getIsolationInfo(const std::string& pluginName) const = 0;
    virtual std::map<std::string, std::string> getSandboxInfo(const std::string& pluginName) const = 0;
};

// Plugin Security Manager
class PluginSecuritySystem {
public:
    PluginSecuritySystem();
    ~PluginSecuritySystem();
    
    // Disable copying, enable moving
    PluginSecuritySystem(const PluginSecuritySystem&) = delete;
    PluginSecuritySystem& operator=(const PluginSecuritySystem&) = delete;
    PluginSecuritySystem(PluginSecuritySystem&&) noexcept;
    PluginSecuritySystem& operator=(PluginSecuritySystem&&) noexcept;
    
    // Initialize security system
    bool initialize();
    
    // Shutdown security system
    void shutdown();
    
    // Check if security system is active
    bool isActive() const;
    
    // Security policy management
    bool addSecurityPolicy(const PluginSecurityPolicy& policy);
    bool removeSecurityPolicy(const std::string& name);
    PluginSecurityPolicy getSecurityPolicy(const std::string& name) const;
    std::vector<PluginSecurityPolicy> getAllSecurityPolicies() const;
    bool updateSecurityPolicy(const std::string& name, const PluginSecurityPolicy& policy);
    
    // Plugin security validation
    bool validatePlugin(const Plugin& plugin) const;
    bool validatePluginSecurity(const std::string& pluginName) const;
    bool checkPluginPermissions(const std::string& pluginName, const PluginPermissionSet& permissions) const;
    bool grantPluginPermissions(const std::string& pluginName, const PluginPermissionSet& permissions);
    bool revokePluginPermissions(const std::string& pluginName, const PluginPermissionSet& permissions);
    PluginPermissionSet getPluginPermissions(const std::string& pluginName) const;
    
    // Plugin security context management
    bool createSecurityContext(const std::string& pluginName, std::shared_ptr<Plugin> plugin);
    bool removeSecurityContext(const std::string& pluginName);
    PluginSecurityContext getSecurityContext(const std::string& pluginName) const;
    bool updateSecurityContext(const std::string& pluginName, const PluginSecurityContext& context);
    
    // Security violation management
    bool reportSecurityViolation(const PluginSecurityViolation& violation);
    std::vector<PluginSecurityViolation> getSecurityViolations(const std::string& pluginName) const;
    std::vector<PluginSecurityViolation> getAllSecurityViolations() const;
    bool clearSecurityViolations(const std::string& pluginName);
    bool clearAllSecurityViolations();
    
    // Isolation and sandboxing
    bool isolatePlugin(const std::string& pluginName);
    bool sandboxPlugin(const std::string& pluginName);
    bool removeIsolation(const std::string& pluginName);
    bool removeSandboxing(const std::string& pluginName);
    bool isIsolated(const std::string& pluginName) const;
    bool isSandboxed(const std::string& pluginName) const;
    
    // Security auditing
    bool addSecurityAuditor(const std::string& name, std::unique_ptr<PluginSecurityAuditor> auditor);
    bool removeSecurityAuditor(const std::string& name);
    PluginSecurityAuditor* getSecurityAuditor(const std::string& name);
    std::vector<std::string> getSecurityAuditorNames() const;
    bool auditAllPlugins();
    bool auditPlugin(const std::string& pluginName);
    
    // Security statistics
    size_t getSecurityViolationCount(const std::string& pluginName) const;
    size_t getSecurityViolationCount() const;
    double getSecurityScore(const std::string& pluginName) const;
    double getOverallSecurityScore() const;
    
    // System configuration
    bool setSecurityPolicy(const PluginSecurityPolicy& policy);
    PluginSecurityPolicy getSecurityPolicy() const;
    
    // Security commands
    bool enableSecurity(const std::string& pluginName);
    bool disableSecurity(const std::string& pluginName);
    bool quarantinePlugin(const std::string& pluginName);
    bool restorePlugin(const std::string& pluginName);
    
    // System commands
    bool executeSecurityCommand(const std::string& command,
                               const std::map<std::string, std::string>& args = {});
    
    // System maintenance
    bool cleanSecurityContexts();
    bool validateSecuritySystem();
    bool repairSecuritySystem();
    bool optimizeSecuritySystem();
    
    // System information
    std::string getSecurityStatus() const;
    std::string getSecurityInfo() const;
    std::vector<std::string> getSecurityMessages() const;
    std::vector<std::string> getSecurityWarnings() const;
    std::vector<std::string> getSecurityErrors() const;
    
    // System commands
    bool exportSecurityData(const std::string& filename, const std::string& format = "json") const;
    bool importSecurityData(const std::string& filename, const std::string& format = "json");
    
    // System about information
    std::string getAbout() const;
    std::string getVersion() const;
    std::string getBuildInfo() const;
    
    // Security validation helpers
    static bool validatePluginMetadata(const PluginMetadata& metadata) const;
    static bool validatePluginPermissions(const PluginPermissionSet& permissions) const;
    static bool checkPluginSecurityPolicy(const Plugin& plugin, const PluginSecurityPolicy& policy) const;
    static std::string getSecurityLevelString(PluginSecurityLevel level);
    static PluginSecurityLevel getSecurityLevelFromString(const std::string& level);
    static std::string getPermissionString(PluginPermission permission);
    static PluginPermission getPermissionFromString(const std::string& permission);
    
    // Security compliance checking
    static bool checkCompliance(const Plugin& plugin, const PluginSecurityPolicy& policy);
    static std::vector<std::string> getComplianceIssues(const Plugin& plugin, const PluginSecurityPolicy& policy);
    
    // Security risk assessment
    static double assessPluginRisk(const Plugin& plugin, const PluginSecurityPolicy& policy);
    static std::string getRiskLevel(double riskScore);
    
    // Security monitoring
    static bool monitorPluginSecurity(const std::string& pluginName, double intervalSeconds = 1.0);
    static bool stopMonitoringPluginSecurity(const std::string& pluginName);
    static bool isPluginSecurityMonitored(const std::string& pluginName);
    
    // Security event handling
    virtual void onSecurityViolation(const PluginSecurityViolation& violation);
    virtual void onSecurityPolicyChanged(const PluginSecurityPolicy& policy);
    virtual void onPluginIsolated(const std::string& pluginName);
    virtual void onPluginSandboxed(const std::string& pluginName);
    virtual void onPluginQuarantined(const std::string& pluginName);
    virtual void onPluginRestored(const std::string& pluginName);
    
    // System event handling
    virtual void onSystemInitialized();
    virtual void onSystemShutdown();
    virtual void onSystemError(const std::string& error);
    virtual void onSystemWarning(const std::string& warning);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper methods
    bool validatePluginInternal(const Plugin& plugin) const;
    bool checkPluginPermissionsInternal(const std::string& pluginName, const PluginPermissionSet& permissions) const;
    bool grantPluginPermissionsInternal(const std::string& pluginName, const PluginPermissionSet& permissions);
    bool revokePluginPermissionsInternal(const std::string& pluginName, const PluginPermissionSet& permissions);
    bool createSecurityContextInternal(const std::string& pluginName, std::shared_ptr<Plugin> plugin);
    bool removeSecurityContextInternal(const std::string& pluginName);
    PluginSecurityContext getSecurityContextInternal(const std::string& pluginName) const;
    bool reportSecurityViolationInternal(const PluginSecurityViolation& violation);
    std::vector<PluginSecurityViolation> getSecurityViolationsInternal(const std::string& pluginName) const;
    bool clearSecurityViolationsInternal(const std::string& pluginName);
    bool isolatePluginInternal(const std::string& pluginName);
    bool sandboxPluginInternal(const std::string& pluginName);
    bool removeIsolationInternal(const std::string& pluginName);
    bool removeSandboxingInternal(const std::string& pluginName);
    
    bool addSecurityAuditorInternal(const std::string& name, std::unique_ptr<PluginSecurityAuditor> auditor);
    bool auditAllPluginsInternal();
    bool auditPluginInternal(const std::string& pluginName);
    
    void logSecurityMessage(const std::string& message, const std::string& level = "info");
    void updateSecurityStatistics();
    
    bool validatePluginMetadataInternal(const PluginMetadata& metadata) const;
    bool validatePluginPermissionsInternal(const PluginPermissionSet& permissions) const;
    bool checkPluginSecurityPolicyInternal(const Plugin& plugin, const PluginSecurityPolicy& policy) const;
    std::string getSecurityLevelStringInternal(PluginSecurityLevel level) const;
    PluginSecurityLevel getSecurityLevelFromStringInternal(const std::string& level) const;
    std::string getPermissionStringInternal(PluginPermission permission) const;
    PluginPermission getPermissionFromStringInternal(const std::string& permission) const;
    
    bool checkComplianceInternal(const Plugin& plugin, const PluginSecurityPolicy& policy);
    std::vector<std::string> getComplianceIssuesInternal(const Plugin& plugin, const PluginSecurityPolicy& policy);
    
    double assessPluginRiskInternal(const Plugin& plugin, const PluginSecurityPolicy& policy);
    std::string getRiskLevelInternal(double riskScore);
    
    bool monitorPluginSecurityInternal(const std::string& pluginName, double intervalSeconds);
    bool stopMonitoringPluginSecurityInternal(const std::string& pluginName);
};

} // namespace nlm