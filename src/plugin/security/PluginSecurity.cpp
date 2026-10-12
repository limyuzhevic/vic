// PluginSecurity.cpp implementation
#include "PluginSecurity.hpp"
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>

namespace nlm {

struct PluginSecurity::Impl {
    SecurityPolicy policy;
    std::unordered_map<std::string, SecurityLevel> pluginSecurityLevels;
    std::unordered_map<std::string, std::unordered_set<Permission>> pluginPermissions;
    std::unordered_map<std::string, bool> sandboxes;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> lastActivity;
    std::vector<SecurityViolation> violations;
    
    std::mutex mutex;
    bool monitoringEnabled = true;
    std::unordered_map<std::string, PluginActivity> activityLog;
};

PluginSecurity::PluginSecurity() : pImpl(std::make_unique<Impl>()) {
    pImpl->policy = SecurityPolicy();
    pImpl->policy.name = "Default Security Policy";
    pImpl->policy.level = SecurityLevel::Medium;
    pImpl->policy.allowedPermissions = {Permission::FileRead, Permission::MemoryAccess, Permission::SystemCall};
    pImpl->policy.enableLogging = true;
    pImpl->policy.enableMonitoring = true;
}

PluginSecurity::~PluginSecurity() = default;

bool PluginSecurity::initialize() {
    return true; // Initialization is complete
}

bool PluginSecurity::isPathSafe(const std::string& path, const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check if path is in allowed list
    if (!checkPathInAllowedList(path, pImpl->policy.allowedPaths)) {
        logSecurityViolation(pluginName, Permission::FileRead, "Access to path not allowed", path);
        return false;
    }
    
    // Check if path is in blocked list
    if (checkPathInBlockedList(path, pImpl->policy.blockedPaths)) {
        logSecurityViolation(pluginName, Permission::FileRead, "Path is blocked", path);
        return false;
    }
    
    return true;
}

bool PluginSecurity::hasPermission(const std::string& pluginName, Permission permission) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check if plugin has explicit permissions
    auto pluginPermsIt = pImpl->pluginPermissions.find(pluginName);
    if (pluginPermsIt != pImpl->pluginPermissions.end()) {
        return pluginPermsIt->second.find(permission) != pluginPermsIt->second.end();
    }
    
    // Check if plugin has default permissions based on security level
    SecurityLevel level = pImpl->pluginSecurityLevels.count(pluginName) ? 
        pImpl->pluginSecurityLevels.at(pluginName) : SecurityLevel::Medium;
    
    // Determine allowed permissions based on security level
    switch (level) {
        case SecurityLevel::None:
            return false;
        case SecurityLevel::Low:
            return permission == Permission::MemoryAccess;
        case SecurityLevel::Medium:
            return permission == Permission::MemoryAccess || 
                   permission == Permission::FileRead ||
                   permission == Permission::SystemCall;
        case SecurityLevel::High:
            return permission == Permission::MemoryAccess ||
                   permission == Permission::FileRead ||
                   permission == Permission::FileWrite ||
                   permission == Permission::NetworkAccess ||
                   permission == Permission::SystemCall;
        case SecurityLevel::Critical:
            // All permissions allowed
            return true;
    }
    
    return false;
}

void PluginSecurity::setPluginSecurityLevel(const std::string& pluginName, SecurityLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->pluginSecurityLevels[pluginName] = level;
}

void PluginSecurity::setPluginPermissions(const std::string& pluginName, const std::unordered_set<Permission>& permissions) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->pluginPermissions[pluginName] = permissions;
}

PluginSecurity::SecurityLevel PluginSecurity::getPluginSecurityLevel(const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->pluginSecurityLevels.find(pluginName);
    if (it != pImpl->pluginSecurityLevels.end()) {
        return it->second;
    }
    
    return SecurityLevel::Medium; // Default
}

std::unordered_set<PluginSecurity::Permission> PluginSecurity::getPluginPermissions(const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->pluginPermissions.find(pluginName);
    if (it != pImpl->pluginPermissions.end()) {
        return it->second;
    }
    
    return {}; // No explicit permissions
}

bool PluginSecurity::validatePlugin(const std::shared_ptr<class Plugin>& plugin) const {
    if (!plugin) {
        return false;
    }
    
    const auto& metadata = plugin->getMetadata();
    
    // Check if plugin requires sandboxing
    if (metadata.capabilities.requiresSandboxing) {
        if (!pImpl->policy.sandboxPlugins) {
            return false;
        }
        
        // Create sandbox if needed
        if (!isSandboxed(metadata.name)) {
            createSandbox(metadata.name);
        }
    }
    
    return true;
}

bool PluginSecurity::executeWithSecurity(std::function<void()> pluginFunction) {
    try {
        // Execute plugin function
        pluginFunction();
        return true;
    } catch (const std::exception& e) {
        // Log exception
        logSecurityViolation("system", Permission::SystemCall, "Plugin execution exception", e.what());
        return false;
    }
}

void PluginSecurity::logActivity(const PluginActivity& activity) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->activityLog[activity.pluginName] = activity;
    
    // Check if activity is allowed
    if (pImpl->policy.enableLogging) {
        // Log activity based on security level
        if (!activity.allowed && pImpl->policy.enableMonitoring) {
            logSecurityViolation(activity.pluginName, Permission::SystemCall, "Security violation", activity.details);
        }
    }
}

std::vector<PluginSecurity::PluginActivity> PluginSecurity::getRecentActivity(const std::string& pluginName, size_t limit) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<PluginActivity> activities;
    
    if (pluginName.empty()) {
        // Return all activities
        for (const auto& pair : pImpl->activityLog) {
            activities.push_back(pair.second);
        }
    } else {
        // Return activities for specific plugin
        auto it = pImpl->activityLog.find(pluginName);
        if (it != pImpl->activityLog.end()) {
            activities.push_back(it->second);
        }
    }
    
    // Sort by timestamp (newest first)
    std::sort(activities.begin(), activities.end(),
        [](const PluginActivity& a, const PluginActivity& b) {
            return a.timestamp > b.timestamp;
        });
    
    // Limit the number of activities
    if (activities.size() > limit) {
        activities.resize(limit);
    }
    
    return activities;
}

bool PluginSecurity::checkResourceUsage(const std::string& pluginName, size_t memoryBytes, float cpuTime) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    SecurityLevel level = getPluginSecurityLevel(pluginName);
    
    // Check memory usage
    if (memoryBytes > pImpl->policy.maxMemoryUsage) {
        logSecurityViolation(pluginName, Permission::MemoryAccess, "Memory usage exceeded", 
                           std::to_string(memoryBytes) + " bytes > " + std::to_string(pImpl->policy.maxMemoryUsage) + " bytes");
        return false;
    }
    
    // Check CPU time (placeholder)
    if (cpuTime > pImpl->policy.timeoutSeconds) {
        logSecurityViolation(pluginName, Permission::SystemCall, "CPU time exceeded",
                           std::to_string(cpuTime) + "s > " + std::to_string(pImpl->policy.timeoutSeconds) + "s");
        return false;
    }
    
    return true;
}

void PluginSecurity::createSandbox(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->sandboxes[pluginName] = true;
}

void PluginSecurity::destroySandbox(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->sandboxes.erase(pluginName);
}

bool PluginSecurity::isSandboxed(const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->sandboxes.find(pluginName) != pImpl->sandboxes.end();
}

void PluginSecurity::setSecurityPolicy(const SecurityPolicy& policy) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->policy = policy;
}

PluginSecurity::SecurityPolicy PluginSecurity::getSecurityPolicy() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->policy;
}

std::vector<PluginSecurity::SecurityViolation> PluginSecurity::getRecentViolations(size_t limit) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<SecurityViolation> violations(pImpl->violations.begin(), pImpl->violations.end());
    
    // Sort by timestamp (newest first)
    std::sort(violations.begin(), violations.end(),
        [](const SecurityViolation& a, const SecurityViolation& b) {
            return a.timestamp > b.timestamp;
        });
    
    // Limit the number of violations
    if (violations.size() > limit) {
        violations.resize(limit);
    }
    
    return violations;
}

void PluginSecurity::clearViolations() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->violations.clear();
}

void PluginSecurity::cleanup() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Destroy all sandboxes
    pImpl->sandboxes.clear();
    
    // Clear activity log
    pImpl->activityLog.clear();
    
    // Clear violations
    pImpl->violations.clear();
}

void PluginSecurity::shutdown() {
    cleanup();
    securityEnabled_ = false;
}

void PluginSecurity::logSecurityViolation(const std::string& pluginName, Permission permission, 
                                         const std::string& operation, const std::string& details) {
    SecurityViolation violation;
    violation.pluginName = pluginName;
    violation.violatedPermission = permission;
    violation.operation = operation;
    violation.details = details;
    violation.timestamp = std::chrono::steady_clock::now();
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->violations.push_back(violation);
}

bool PluginSecurity::checkPathInAllowedList(const std::string& path, const std::vector<std::string>& allowedPaths) const {
    if (allowedPaths.empty()) {
        return true; // Empty list means all paths allowed
    }
    
    for (const auto& allowed : allowedPaths) {
        if (path.find(allowed) == 0) {
            return true;
        }
    }
    
    return false;
}

bool PluginSecurity::checkPathInBlockedList(const std::string& path, const std::vector<std::string>& blockedPaths) const {
    for (const auto& blocked : blockedPaths) {
        if (path.find(blocked) == 0) {
            return true;
        }
    }
    
    return false;
}

PluginSecurity::Permission PluginSecurity::stringToPermission(const std::string& permissionStr) const {
    static const std::unordered_map<std::string, Permission> permissionMap = {
        {"file_read", Permission::FileRead},
        {"file_write", Permission::FileWrite},
        {"file_execute", Permission::FileExecute},
        {"network_access", Permission::NetworkAccess},
        {"process_access", Permission::ProcessAccess},
        {"memory_access", Permission::MemoryAccess},
        {"system_call", Permission::SystemCall},
        {"debug_access", Permission::DebugAccess},
        {"plugin_management", Permission::PluginManagement},
        {"configuration_access", Permission::ConfigurationAccess}
    };
    
    auto it = permissionMap.find(permissionStr);
    if (it != permissionMap.end()) {
        return it->second;
    }
    
    return Permission::FileRead; // Default
}

std::string PluginSecurity::permissionToString(Permission permission) const {
    static const std::unordered_map<Permission, std::string> permissionStrMap = {
        {Permission::FileRead, "file_read"},
        {Permission::FileWrite, "file_write"},
        {Permission::FileExecute, "file_execute"},
        {Permission::NetworkAccess, "network_access"},
        {Permission::ProcessAccess, "process_access"},
        {Permission::MemoryAccess, "memory_access"},
        {Permission::SystemCall, "system_call"},
        {Permission::DebugAccess, "debug_access"},
        {Permission::PluginManagement, "plugin_management"},
        {Permission::ConfigurationAccess, "configuration_access"}
    };
    
    auto it = permissionStrMap.find(permission);
    if (it != permissionStrMap.end()) {
        return it->second;
    }
    
    return "unknown";
}

} // namespace nlm