#pragma once

#include <string>
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <mutex>
#include <functional>
#include <vector>
#include "PluginAPI.hpp"

namespace nlm {

class PluginSecurity {
public:
    // Security levels
    enum class SecurityLevel {
        None,
        Low,
        Medium,
        High,
        Critical
    };
    
    // Permission types
    enum class Permission {
        FileRead,
        FileWrite,
        FileExecute,
        NetworkAccess,
        ProcessAccess,
        MemoryAccess,
        SystemCall,
        DebugAccess,
        PluginManagement,
        ConfigurationAccess
    };
    
    // Security policy
    struct SecurityPolicy {
        std::string name;
        SecurityLevel level;
        std::unordered_set<Permission> allowedPermissions;
        std::vector<std::string> allowedPaths;
        std::vector<std::string> blockedPaths;
        bool allowPluginToPluginCommunication = true;
        bool allowExternalCommunication = false;
        float timeoutSeconds = 30.0f;
        size_t maxMemoryUsage = 64 * 1024 * 1024;  // 64MB
        bool enableLogging = true;
        bool enableMonitoring = true;
    };
    
    PluginSecurity();
    ~PluginSecurity();
    
    // Initialize security system
    bool initialize();
    
    // Check if a path is safe to access
    bool isPathSafe(const std::string& path, const std::string& pluginName = "") const;
    
    // Check if plugin has permission
    bool hasPermission(const std::string& pluginName, Permission permission) const;
    
    // Set plugin security level
    void setPluginSecurityLevel(const std::string& pluginName, SecurityLevel level);
    
    // Set plugin permissions
    void setPluginPermissions(const std::string& pluginName, const std::unordered_set<Permission>& permissions);
    
    // Get plugin security level
    SecurityLevel getPluginSecurityLevel(const std::string& pluginName) const;
    
    // Get plugin permissions
    std::unordered_set<Permission> getPluginPermissions(const std::string& pluginName) const;
    
    // Validate plugin against security policy
    bool validatePlugin(const std::shared_ptr<class Plugin>& plugin) const;
    
    // Execute plugin with security sandboxing
    bool executeWithSecurity(std::function<void()> pluginFunction);
    
    // Monitor plugin activity
    struct PluginActivity {
        std::string pluginName;
        std::string activityType;
        std::chrono::steady_clock::time_point timestamp;
        std::string details;
        bool allowed;
    };
    
    void logActivity(const PluginActivity& activity);
    std::vector<PluginActivity> getRecentActivity(const std::string& pluginName = "", size_t limit = 100) const;
    
    // Security monitoring
    bool isMonitoringEnabled() const { return monitoringEnabled_; }
    void setMonitoringEnabled(bool enable) { monitoringEnabled_ = enable; }
    
    // Resource limits
    bool checkResourceUsage(const std::string& pluginName, size_t memoryBytes, float cpuTime) const;
    
    // Sandbox management
    void createSandbox(const std::string& pluginName);
    void destroySandbox(const std::string& pluginName);
    bool isSandboxed(const std::string& pluginName) const;
    
    // Configure security policy
    void setSecurityPolicy(const SecurityPolicy& policy);
    SecurityPolicy getSecurityPolicy() const;
    
    // Global security settings
    void enableSecurity(bool enable) { securityEnabled_ = enable; }
    bool isSecurityEnabled() const { return securityEnabled_; }
    
    // Exception handling
    struct SecurityViolation {
        std::string pluginName;
        Permission violatedPermission;
        std::string operation;
        std::string details;
        std::chrono::steady_clock::time_point timestamp;
    };
    
    std::vector<SecurityViolation> getRecentViolations(size_t limit = 100) const;
    void clearViolations();
    
    // Cleanup
    void cleanup();
    void shutdown();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    bool securityEnabled_ = false;
    bool monitoringEnabled_ = true;
    SecurityPolicy defaultPolicy_;
    
    // Helper functions
    bool checkPathInAllowedList(const std::string& path, const std::vector<std::string>& allowedPaths) const;
    bool checkPathInBlockedList(const std::string& path, const std::vector<std::string>& blockedPaths) const;
    Permission stringToPermission(const std::string& permissionStr) const;
    std::string permissionToString(Permission permission) const;
};

} // namespace nlm