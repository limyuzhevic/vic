#include "PluginSecuritySystem.hpp"
#include "PluginCore.hpp"
#include "../core/Logger/Logger.hpp"
#include <thread>
#include <algorithm>
#include <sstream>

namespace nlm {

struct PluginSecuritySystem::Impl {
    bool active{false};
    PluginSecurityPolicy policy;
    
    std::mutex securityMutex;
    
    std::map<std::string, PluginSecurityContext> securityContexts;
    std::map<std::string, std::vector<PluginSecurityViolation>> securityViolations;
    
    std::map<std::string, std::unique_ptr<PluginSecurityAuditor>> securityAuditors;
    
    std::map<std::string, PluginPermissionSet> pluginPermissions;
    
    std::vector<std::string> systemMessages;
    std::vector<std::string> systemWarnings;
    std::vector<std::string> systemErrors;
    
    std::atomic<size_t> totalViolations{0};
    std::atomic<double> overallSecurityScore{100.0};
    
    Impl() {
        // Initialize default policy
        policy.name = "Default Security Policy";
        policy.description = "Default security policy for NLM plugins";
        policy.level = PluginSecurityLevel::Medium;
        policy.enableLogging = true;
        policy.enableAuditing = true;
        policy.enableIsolation = true;
        policy.enableSandboxing = true;
        
        // Add default permissions
        policy.requiredPermissions.insert(PluginPermission::Read);
        policy.requiredPermissions.insert(PluginPermission::Safe);
    }
    
    void logMessage(const std::string& message, const std::string& level = "info") {
        if (level == "error") {
            systemErrors.push_back(message);
            NLM_LOG_ERROR(message);
        } else if (level == "warning") {
            systemWarnings.push_back(message);
            NLM_LOG_WARNING(message);
        } else {
            systemMessages.push_back(message);
            NLM_LOG_INFO(message);
        }
        
        // Keep only recent messages
        if (systemMessages.size() > 1000) {
            systemMessages.erase(systemMessages.begin());
        }
        if (systemWarnings.size() > 500) {
            systemWarnings.erase(systemWarnings.begin());
        }
        if (systemErrors.size() > 500) {
            systemErrors.erase(systemErrors.begin());
        }
    }
    
    bool hasPermission(const PluginPermissionSet& permissions, PluginPermission permission) const {
        return permissions.find(permission) != permissions.end();
    }
    
    std::string getSecurityLevelStringInternal(PluginSecurityLevel level) const {
        switch (level) {
            case PluginSecurityLevel::Low:
                return "Low";
            case PluginSecurityLevel::Medium:
                return "Medium";
            case PluginSecurityLevel::High:
                return "High";
            case PluginSecurityLevel::Critical:
                return "Critical";
            default:
                return "Unknown";
        }
    }
    
    PluginSecurityLevel getSecurityLevelFromStringInternal(const std::string& level) const {
        if (level == "Low") return PluginSecurityLevel::Low;
        if (level == "Medium") return PluginSecurityLevel::Medium;
        if (level == "High") return PluginSecurityLevel::High;
        if (level == "Critical") return PluginSecurityLevel::Critical;
        return PluginSecurityLevel::Medium;
    }
    
    std::string getPermissionStringInternal(PluginPermission permission) const {
        switch (permission) {
            case PluginPermission::Read:
                return "Read";
            case PluginPermission::Write:
                return "Write";
            case PluginPermission::Execute:
                return "Execute";
            case PluginPermission::Network:
                return "Network";
            case PluginPermission::File:
                return "File";
            case PluginPermission::System:
                return "System";
            case PluginPermission::Debug:
                return "Debug";
            case PluginPermission::Admin:
                return "Admin";
            case PluginPermission::Safe:
                return "Safe";
            case PluginPermission::Restricted:
                return "Restricted";
            case PluginPermission::None:
                return "None";
            default:
                return "Unknown";
        }
    }
    
    PluginPermission getPermissionFromStringInternal(const std::string& permission) const {
        if (permission == "Read") return PluginPermission::Read;
        if (permission == "Write") return PluginPermission::Write;
        if (permission == "Execute") return PluginPermission::Execute;
        if (permission == "Network") return PluginPermission::Network;
        if (permission == "File") return PluginPermission::File;
        if (permission == "System") return PluginPermission::System;
        if (permission == "Debug") return PluginPermission::Debug;
        if (permission == "Admin") return PluginPermission::Admin;
        if (permission == "Safe") return PluginPermission::Safe;
        if (permission == "Restricted") return PluginPermission::Restricted;
        if (permission == "None") return PluginPermission::None;
        return PluginPermission::None;
    }
};

PluginSecuritySystem::PluginSecuritySystem() : pImpl(std::make_unique<Impl>()) {}

PluginSecuritySystem::~PluginSecuritySystem() {
    shutdown();
}

PluginSecuritySystem::PluginSecuritySystem(PluginSecuritySystem&& other) noexcept : pImpl(std::move(other.pImpl)) {}

PluginSecuritySystem& PluginSecuritySystem::operator=(PluginSecuritySystem&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool PluginSecuritySystem::initialize() {
    if (pImpl->active) {
        logMessage("PluginSecuritySystem: Already initialized", "warning");
        return true;
    }
    
    pImpl->active = true;
    
    logMessage("PluginSecuritySystem: Initialized successfully");
    
    onSystemInitialized();
    
    return true;
}

void PluginSecuritySystem::shutdown() {
    if (!pImpl->active) {
        return;
    }
    
    logMessage("PluginSecuritySystem: Shutting down...");
    
    // Clear all security contexts
    pImpl->securityContexts.clear();
    
    // Clear all violations
    pImpl->securityViolations.clear();
    pImpl->totalViolations = 0;
    
    // Clear all auditors
    pImpl->securityAuditors.clear();
    
    // Clear all permissions
    pImpl->pluginPermissions.clear();
    
    pImpl->active = false;
    
    logMessage("PluginSecuritySystem: Shutdown successfully");
    
    onSystemShutdown();
}

bool PluginSecuritySystem::isActive() const {
    return pImpl->active;
}

bool PluginSecuritySystem::addSecurityPolicy(const PluginSecurityPolicy& policy) {
    std::lock_guard<std::mutex> lock(pImpl->securityMutex);
    
    pImpl->policy = policy;
    
    logMessage("PluginSecuritySystem: Added security policy: " + policy.name);
    
    return true;
}

bool PluginSecuritySystem::removeSecurityPolicy(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->securityMutex);
    
    // For now, we only have one policy
    if (pImpl->policy.name == name) {
        pImpl->policy.name = "Default Security Policy";
        pImpl->policy.description = "Default security policy for NLM plugins";
        pImpl->policy.level = PluginSecurityLevel::Medium;
        pImpl->policy.requiredPermissions.clear();
        pImpl->policy.requiredPermissions.insert(PluginPermission::Read);
        pImpl->policy.requiredPermissions.insert(PluginPermission::Safe);
        pImpl->policy.enableLogging = true;
        pImpl->policy.enableAuditing = true;
        pImpl->policy.enableIsolation = true;
        pImpl->policy.enableSandboxing = true;
        
        logMessage("PluginSecuritySystem: Removed security policy: " + name);
        return true;
    }
    
    return false;
}

PluginSecurityPolicy PluginSecuritySystem::getSecurityPolicy(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->securityMutex);
    
    if (pImpl->policy.name == name) {
        return pImpl->policy;
    }
    
    return PluginSecurityPolicy();
}

std::vector<PluginSecurityPolicy> PluginSecuritySystem::getAllSecurityPolicies() const {
    std::lock_guard<std::mutex> lock(pImpl->securityMutex);
    
    return {pImpl->policy};
}

bool PluginSecuritySystem::updateSecurityPolicy(const std::string& name, const PluginSecurityPolicy& policy) {
    if (pImpl->policy.name == name) {
        pImpl->policy = policy;
        return true;
    }
    
    return false;
}

bool PluginSecuritySystem::validatePlugin(const Plugin& plugin) const {
    return validatePluginInternal(plugin);
}

bool PluginSecuritySystem::validatePluginSecurity(const std::string& pluginName) const {
    const Plugin* plugin = nullptr; // TODO: Get plugin from registry
    if (!plugin) {
        logMessage("PluginSecuritySystem: Plugin not found: " + pluginName, "error");
        return false;
    }
    
    return validatePluginInternal(*plugin);
}

bool PluginSecuritySystem::checkPluginPermissions(const std::string& pluginName, const PluginPermissionSet& permissions) const {
    return checkPluginPermissionsInternal(pluginName, permissions);
}

bool PluginSecuritySystem::grantPluginPermissions(const std::string& pluginName, const PluginPermissionSet& permissions) {
    return grantPluginPermissionsInternal(pluginName, permissions);
}

bool PluginSecuritySystem::revokePluginPermissions(const std::string& pluginName, const PluginPermissionSet& permissions) {
    return revokePluginPermissionsInternal(pluginName, permissions);
}

PluginPermissionSet PluginSecuritySystem::getPluginPermissions(const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(pImpl->securityMutex);
    
    auto it = pImpl->pluginPermissions.find(pluginName);
    if (it != pImpl->pluginPermissions.end()) {
        return it->second;
    }
    
    return PluginPermissionSet();
}

bool PluginSecuritySystem::createSecurityContext(const std::string& pluginName, std::shared_ptr<Plugin> plugin) {
    return createSecurityContextInternal(pluginName, plugin);
}

bool PluginSecuritySystem::removeSecurityContext(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(pImpl->securityMutex);
    
    auto it = pImpl->securityContexts.find(pluginName);
    if (it != pImpl->securityContexts.end()) {
        pImpl->securityContexts.erase(it);
        return true;
    }
    
    return false;
}

PluginSecurityContext PluginSecuritySystem::getSecurityContext(const std::string& pluginName) const {
    return getSecurityContextInternal(pluginName);
}

bool PluginSecuritySystem::updateSecurityContext(const std::string& pluginName, const PluginSecurityContext& context) {
    std::lock_guard<std::mutex> lock(pImpl->securityMutex);
    
    pImpl->securityContexts[pluginName] = context;
    return true;
}

bool PluginSecuritySystem::reportSecurityViolation(const PluginSecurityViolation& violation) {
    return reportSecurityViolationInternal(violation);
}

std::vector<PluginSecurityViolation> PluginSecuritySystem::getSecurityViolations(const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(pImpl->securityMutex);
    
    auto it = pImpl->securityViolations.find(pluginName);
    if (it != pImpl->securityViolations.end()) {
        return it->second;
    }
    
    return std::vector<PluginSecurityViolation>();
}

std::vector<PluginSecurityViolation> PluginSecuritySystem::getAllSecurityViolations() const {
    std::lock_guard<std::mutex> lock(pImpl->securityMutex);
    
    std::vector<PluginSecurityViolation> allViolations;
    
    for (const auto& pair : pImpl->securityViolations) {
        allViolations.insert(allViolations.end(), pair.second.begin(), pair.second.end());
    }
    
    return allViolations;
}

bool PluginSecuritySystem::clearSecurityViolations(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(pImpl->securityMutex);
    
    pImpl->securityViolations.erase(pluginName);
    return true;
}

bool PluginSecuritySystem::clearAllSecurityViolations() {
    std::lock_guard<std::mutex> lock(pImpl->securityMutex);
    
    pImpl->securityViolations.clear();
    pImpl->totalViolations = 0;
    
    return true;
}

bool PluginSecuritySystem::isolatePlugin(const std::string& pluginName) {
    return isolatePluginInternal(pluginName);
}

bool PluginSecuritySystem::sandboxPlugin(const std::string& pluginName) {
    return sandboxPluginInternal(pluginName);
}

bool PluginSecuritySystem::removeIsolation(const std::string& pluginName) {
    return removeIsolationInternal(pluginName);
}

bool PluginSecuritySystem::removeSandboxing(const std::string& pluginName) {
    return removeSandboxingInternal(pluginName);
}

bool PluginSecuritySystem::isIsolated(const std::string& pluginName) const {
    PluginSecurityContext context = getSecurityContextInternal(pluginName);
    return context.isolated;
}

bool PluginSecuritySystem::isSandboxed(const std::string& pluginName) const {
    PluginSecurityContext context = getSecurityContextInternal(pluginName);
    return context.sandboxed;
}

bool PluginSecuritySystem::addSecurityAuditor(const std::string& name, std::unique_ptr<PluginSecurityAuditor> auditor) {
    return addSecurityAuditorInternal(name, std::move(auditor));
}

bool PluginSecuritySystem::removeSecurityAuditor(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->securityMutex);
    
    pImpl->securityAuditors.erase(name);
    return true;
}

PluginSecurityAuditor* PluginSecuritySystem::getSecurityAuditor(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->securityMutex);
    
    auto it = pImpl->securityAuditors.find(name);
    if (it != pImpl->securityAuditors.end()) {
        return it->second.get();
    }
    
    return nullptr;
}

std::vector<std::string> PluginSecuritySystem::getSecurityAuditorNames() const {
    std::lock_guard<std::mutex> lock(pImpl->securityMutex);
    
    std::vector<std::string> names;
    for (const auto& pair : pImpl->securityAuditors) {
        names.push_back(pair.first);
    }
    
    return names;
}

bool PluginSecuritySystem::auditAllPlugins() {
    return auditAllPluginsInternal();
}

bool PluginSecuritySystem::auditPlugin(const std::string& pluginName) {
    return auditPluginInternal(pluginName);
}

size_t PluginSecuritySystem::getSecurityViolationCount(const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(pImpl->securityMutex);
    
    auto it = pImpl->securityViolations.find(pluginName);
    if (it != pImpl->securityViolations.end()) {
        return it->second.size();
    }
    
    return 0;
}

size_t PluginSecuritySystem::getSecurityViolationCount() const {
    return pImpl->totalViolations;
}

double PluginSecuritySystem::getSecurityScore(const std::string& pluginName) const {
    // TODO: Implement security score calculation
    return 100.0;
}

double PluginSecuritySystem::getOverallSecurityScore() const {
    return pImpl->overallSecurityScore;
}

bool PluginSecuritySystem::setSecurityPolicy(const PluginSecurityPolicy& policy) {
    return addSecurityPolicy(policy);
}

PluginSecurityPolicy PluginSecuritySystem::getSecurityPolicy() const {
    return pImpl->policy;
}

bool PluginSecuritySystem::enableSecurity(const std::string& pluginName) {
    return grantPluginPermissionsInternal(pluginName, pImpl->policy.requiredPermissions);
}

bool PluginSecuritySystem::disableSecurity(const std::string& pluginName) {
    return revokePluginPermissionsInternal(pluginName, pImpl->policy.requiredPermissions);
}

bool PluginSecuritySystem::quarantinePlugin(const std::string& pluginName) {
    // Isolate and sandbox the plugin
    if (!isolatePluginInternal(pluginName)) {
        return false;
    }
    
    if (!sandboxPluginInternal(pluginName)) {
        return false;
    }
    
    // Add a violation to track quarantine
    PluginSecurityViolation violation;
    violation.pluginName = pluginName;
    violation.type = PluginSecurityViolationType::SecurityBreach;
    violation.description = "Plugin quarantined for security reasons";
    violation.severity = "error";
    
    reportSecurityViolationInternal(violation);
    
    logMessage("PluginSecuritySystem: Plugin quarantined: " + pluginName);
    
    return true;
}

bool PluginSecuritySystem::restorePlugin(const std::string& pluginName) {
    // Remove isolation and sandboxing
    if (!removeIsolationInternal(pluginName)) {
        return false;
    }
    
    if (!removeSandboxingInternal(pluginName)) {
        return false;
    }
    
    logMessage("PluginSecuritySystem: Plugin restored: " + pluginName);
    
    return true;
}

bool PluginSecuritySystem::executeSecurityCommand(const std::string& command,
                                                   const std::map<std::string, std::string>& args) {
    // TODO: Implement security command execution
    return true;
}

bool PluginSecuritySystem::cleanSecurityContexts() {
    std::lock_guard<std::mutex> lock(pImpl->securityMutex);
    
    pImpl->securityContexts.clear();
    return true;
}

bool PluginSecuritySystem::validateSecuritySystem() {
    bool success = true;
    
    for (const auto& pair : pImpl->securityContexts) {
        if (!validatePluginInternal(*pair.second.plugin)) {
            logMessage("PluginSecuritySystem: Security validation failed for plugin " + pair.first, "warning");
            success = false;
        }
    }
    
    return success;
}

bool PluginSecuritySystem::repairSecuritySystem() {
    // TODO: Implement security system repair
    return true;
}

bool PluginSecuritySystem::optimizeSecuritySystem() {
    // TODO: Implement security system optimization
    return true;
}

std::string PluginSecuritySystem::getSecurityStatus() const {
    std::stringstream ss;
    ss << "PluginSecuritySystem Status:" << std::endl;
    ss << "  Active: " << (isActive() ? "true" : "false") << std::endl;
    ss << "  Policy: " << pImpl->policy.name << std::endl;
    ss << "  Policy Level: " << getSecurityLevelStringInternal(pImpl->policy.level) << std::endl;
    ss << "  Total Violations: " << getSecurityViolationCount() << std::endl;
    ss << "  Overall Security Score: " << getOverallSecurityScore() << std::endl;
    
    return ss.str();
}

std::string PluginSecuritySystem::getSecurityInfo() const {
    std::stringstream ss;
    ss << "PluginSecuritySystem Information:" << std::endl;
    ss << "  Policy Description: " << pImpl->policy.description << std::endl;
    ss << "  Required Permissions Count: " << pImpl->policy.requiredPermissions.size() << std::endl;
    ss << "  Forbidden Permissions Count: " << pImpl->policy.forbiddenPermissions.size() << std::endl;
    ss << "  Allowed Plugins Count: " << pImpl->policy.allowedPlugins.size() << std::endl;
    ss << "  Forbidden Plugins Count: " << pImpl->policy.forbiddenPlugins.size() << std::endl;
    ss << "  Security Logging: " << (pImpl->policy.enableLogging ? "enabled" : "disabled") << std::endl;
    ss << "  Security Auditing: " << (pImpl->policy.enableAuditing ? "enabled" : "disabled") << std::endl;
    ss << "  Isolation Enabled: " << (pImpl->policy.enableIsolation ? "enabled" : "disabled") << std::endl;
    ss << "  Sandboxing Enabled: " << (pImpl->policy.enableSandboxing ? "enabled" : "disabled") << std::endl;
    
    return ss.str();
}

std::vector<std::string> PluginSecuritySystem::getSecurityMessages() const {
    return pImpl->systemMessages;
}

std::vector<std::string> PluginSecuritySystem::getSecurityWarnings() const {
    return pImpl->systemWarnings;
}

std::vector<std::string> PluginSecuritySystem::getSecurityErrors() const {
    return pImpl->systemErrors;
}

bool PluginSecuritySystem::exportSecurityData(const std::string& filename, const std::string& format) const {
    // TODO: Implement security data export
    return true;
}

bool PluginSecuritySystem::importSecurityData(const std::string& filename, const std::string& format) {
    // TODO: Implement security data import
    return true;
}

std::string PluginSecuritySystem::getAbout() const {
    return "PluginSecuritySystem - Plugin Security Management System for NLM";
}

std::string PluginSecuritySystem::getVersion() const {
    return "1.0.0";
}

std::string PluginSecuritySystem::getBuildInfo() const {
    return "PluginSecuritySystem Build Information";
}

void PluginSecuritySystem::onSecurityViolation(const PluginSecurityViolation& violation) {
    // Default implementation - can be overridden
    logMessage("PluginSecuritySystem: Security violation: " + violation.pluginName + ", type: " + 
              std::to_string(static_cast<int>(violation.type)) + ", severity: " + violation.severity);
}

void PluginSecuritySystem::onSecurityPolicyChanged(const PluginSecurityPolicy& policy) {
    // Default implementation - can be overridden
    logMessage("PluginSecuritySystem: Security policy changed: " + policy.name);
}

void PluginSecuritySystem::onPluginIsolated(const std::string& pluginName) {
    // Default implementation - can be overridden
    logMessage("PluginSecuritySystem: Plugin isolated: " + pluginName);
}

void PluginSecuritySystem::onPluginSandboxed(const std::string& pluginName) {
    // Default implementation - can be overridden
    logMessage("PluginSecuritySystem: Plugin sandboxed: " + pluginName);
}

void PluginSecuritySystem::onPluginQuarantined(const std::string& pluginName) {
    // Default implementation - can be overridden
    logMessage("PluginSecuritySystem: Plugin quarantined: " + pluginName);
}

void PluginSecuritySystem::onPluginRestored(const std::string& pluginName) {
    // Default implementation - can be overridden
    logMessage("PluginSecuritySystem: Plugin restored: " + pluginName);
}

void PluginSecuritySystem::onSystemInitialized() {
    // Default implementation - can be overridden
    logMessage("PluginSecuritySystem: System initialized");
}

void PluginSecuritySystem::onSystemShutdown() {
    // Default implementation - can be overridden
    logMessage("PluginSecuritySystem: System shutting down");
}

void PluginSecuritySystem::onSystemError(const std::string& error) {
    // Default implementation - can be overridden
    logMessage("PluginSecuritySystem: System error: " + error, "error");
}

void PluginSecuritySystem::onSystemWarning(const std::string& warning) {
    // Default implementation - can be overridden
    logMessage("PluginSecuritySystem: System warning: " + warning, "warning");
}

// Private helper methods implementation
bool PluginSecuritySystem::validatePluginInternal(const Plugin& plugin) const {
    // Basic validation
    if (plugin.getName().empty() || plugin.getVersion().empty() || 
        plugin.getDescription().empty() || plugin.getAuthor().empty()) {
        return false;
    }
    
    // Check if plugin is allowed
    const PluginMetadata& metadata = plugin.getMetadata();
    if (pImpl->policy.isPluginForbidden(plugin.getName())) {
        return false;
    }
    
    // Check if plugin meets security requirements
    if (!validatePluginMetadataInternal(metadata)) {
        return false;
    }
    
    return true;
}

bool PluginSecuritySystem::checkPluginPermissionsInternal(const std::string& pluginName, const PluginPermissionSet& permissions) const {
    PluginPermissionSet pluginPermissions = getPluginPermissions(pluginName);
    
    // Check if plugin has all required permissions
    for (const auto& permission : pImpl->policy.requiredPermissions) {
        if (pluginPermissions.find(permission) == pluginPermissions.end()) {
            logMessage("PluginSecuritySystem: Plugin " + pluginName + " missing required permission: " + 
                      getPermissionStringInternal(permission), "warning");
            return false;
        }
    }
    
    // Check if plugin has forbidden permissions
    for (const auto& permission : pImpl->policy.forbiddenPermissions) {
        if (pluginPermissions.find(permission) != pluginPermissions.end()) {
            logMessage("PluginSecuritySystem: Plugin " + pluginName + " has forbidden permission: " + 
                      getPermissionStringInternal(permission), "warning");
            return false;
        }
    }
    
    // Check if user permissions are subset of plugin permissions
    for (const auto& permission : permissions) {
        if (pluginPermissions.find(permission) == pluginPermissions.end()) {
            logMessage("PluginSecuritySystem: User request for permission not granted to plugin " + pluginName + ": " + 
                      getPermissionStringInternal(permission), "warning");
            return false;
        }
    }
    
    return true;
}

bool PluginSecuritySystem::grantPluginPermissionsInternal(const std::string& pluginName, const PluginPermissionSet& permissions) {
    std::lock_guard<std::mutex> lock(pImpl->securityMutex);
    
    pImpl->pluginPermissions[pluginName] = permissions;
    
    logMessage("PluginSecuritySystem: Granted permissions to plugin " + pluginName);
    
    return true;
}

bool PluginSecuritySystem::revokePluginPermissionsInternal(const std::string& pluginName, const PluginPermissionSet& permissions) {
    std::lock_guard<std::mutex> lock(pImpl->securityMutex);
    
    auto it = pImpl->pluginPermissions.find(pluginName);
    if (it == pImpl->pluginPermissions.end()) {
        return false;
    }
    
    // Remove granted permissions
    PluginPermissionSet& pluginPermissions = it->second;
    for (const auto& permission : permissions) {
        pluginPermissions.erase(permission);
    }
    
    logMessage("PluginSecuritySystem: Revoked permissions from plugin " + pluginName);
    
    return true;
}

bool PluginSecuritySystem::createSecurityContextInternal(const std::string& pluginName, std::shared_ptr<Plugin> plugin) {
    std::lock_guard<std::mutex> lock(pImpl->securityMutex);
    
    // Check if plugin is allowed
    if (!pImpl->policy.isPluginAllowed(pluginName)) {
        logMessage("PluginSecuritySystem: Plugin not allowed: " + pluginName, "error");
        return false;
    }
    
    // Check if plugin is forbidden
    if (pImpl->policy.isPluginForbidden(pluginName)) {
        logMessage("PluginSecuritySystem: Plugin forbidden: " + pluginName, "error");
        return false;
    }
    
    // Create security context
    PluginSecurityContext context;
    context.pluginName = pluginName;
    context.plugin = plugin;
    context.level = pImpl->policy.level;
    context.activePermissions = pImpl->pluginPermissions[pluginName];
    
    pImpl->securityContexts[pluginName] = context;
    
    logMessage("PluginSecuritySystem: Created security context for plugin " + pluginName);
    
    return true;
}

bool PluginSecuritySystem::removeSecurityContextInternal(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(pImpl->securityMutex);
    
    pImpl->securityContexts.erase(pluginName);
    pImpl->pluginPermissions.erase(pluginName);
    
    logMessage("PluginSecuritySystem: Removed security context for plugin " + pluginName);
    
    return true;
}

PluginSecurityContext PluginSecuritySystem::getSecurityContextInternal(const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(pImpl->securityMutex);
    
    auto it = pImpl->securityContexts.find(pluginName);
    if (it != pImpl->securityContexts.end()) {
        return it->second;
    }
    
    return PluginSecurityContext();
}

bool PluginSecuritySystem::reportSecurityViolationInternal(const PluginSecurityViolation& violation) {
    std::lock_guard<std::mutex> lock(pImpl->securityMutex);
    
    pImpl->securityViolations[violation.pluginName].push_back(violation);
    pImpl->totalViolations++;
    
    onSecurityViolation(violation);
    
    logMessage("PluginSecuritySystem: Security violation reported for plugin " + violation.pluginName + ", type: " + 
              std::to_string(static_cast<int>(violation.type)) + ", severity: " + violation.severity);
    
    return true;
}

bool PluginSecuritySystem::isolatePluginInternal(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(pImpl->securityMutex);
    
    auto it = pImpl->securityContexts.find(pluginName);
    if (it != pImpl->securityContexts.end()) {
        it->second.isolated = true;
        onPluginIsolated(pluginName);
        logMessage("PluginSecuritySystem: Plugin isolated: " + pluginName);
        return true;
    }
    
    return false;
}

bool PluginSecuritySystem::sandboxPluginInternal(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(pImpl->securityMutex);
    
    auto it = pImpl->securityContexts.find(pluginName);
    if (it != pImpl->securityContexts.end()) {
        it->second.sandboxed = true;
        onPluginSandboxed(pluginName);
        logMessage("PluginSecuritySystem: Plugin sandboxed: " + pluginName);
        return true;
    }
    
    return false;
}

bool PluginSecuritySystem::removeIsolationInternal(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(pImpl->securityMutex);
    
    auto it = pImpl->securityContexts.find(pluginName);
    if (it != pImpl->securityContexts.end()) {
        it->second.isolated = false;
        logMessage("PluginSecuritySystem: Isolation removed for plugin " + pluginName);
        return true;
    }
    
    return false;
}

bool PluginSecuritySystem::removeSandboxingInternal(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(pImpl->securityMutex);
    
    auto it = pImpl->securityContexts.find(pluginName);
    if (it != pImpl->securityContexts.end()) {
        it->second.sandboxed = false;
        onPluginRestored(pluginName);
        logMessage("PluginSecuritySystem: Sandboxing removed for plugin " + pluginName);
        return true;
    }
    
    return false;
}

bool PluginSecuritySystem::addSecurityAuditorInternal(const std::string& name, std::unique_ptr<PluginSecurityAuditor> auditor) {
    std::lock_guard<std::mutex> lock(pImpl->securityMutex);
    
    pImpl->securityAuditors[name] = std::move(auditor);
    
    logMessage("PluginSecuritySystem: Added security auditor: " + name);
    
    return true;
}

bool PluginSecuritySystem::auditAllPluginsInternal() {
    bool success = true;
    
    for (const auto& pair : pImpl->securityContexts) {
        if (!auditPluginInternal(pair.first)) {
            logMessage("PluginSecuritySystem: Security audit failed for plugin " + pair.first, "warning");
            success = false;
        }
    }
    
    return success;
}

bool PluginSecuritySystem::auditPluginInternal(const std::string& pluginName) {
    auto it = pImpl->securityAuditors.find(pluginName);
    if (it == pImpl->securityAuditors.end()) {
        // Use default auditor if available
        for (const auto& pair : pImpl->securityAuditors) {
            if (pair.first == "default") {
                it = pair;
                break;
            }
        }
        
        if (it == pImpl->securityAuditors.end()) {
            logMessage("PluginSecuritySystem: No security auditor available for plugin " + pluginName, "warning");
            return true; // Skip audit
        }
    }
    
    // Perform audit
    auto contextIt = pImpl->securityContexts.find(pluginName);
    if (contextIt != pImpl->securityContexts.end()) {
        return it->second->auditPlugin(*contextIt->second.plugin);
    }
    
    return true;
}

bool PluginSecuritySystem::validatePluginMetadataInternal(const PluginMetadata& metadata) const {
    if (metadata.name.empty() || metadata.version.empty() || 
        metadata.description.empty() || metadata.author.empty()) {
        return false;
    }
    
    if (metadata.securityLevel > pImpl->policy.level) {
        return false;
    }
    
    return true;
}

bool PluginSecuritySystem::validatePluginPermissionsInternal(const PluginPermissionSet& permissions) const {
    for (const auto& permission : permissions) {
        if (pImpl->policy.forbiddenPermissions.find(permission) != pImpl->policy.forbiddenPermissions.end()) {
            return false;
        }
    }
    
    return true;
}

bool PluginSecuritySystem::checkPluginSecurityPolicyInternal(const Plugin& plugin, const PluginSecurityPolicy& policy) const {
    return policy.hasPermission(PluginPermission::Safe);
}

bool PluginSecuritySystem::checkComplianceInternal(const Plugin& plugin, const PluginSecurityPolicy& policy) {
    // TODO: Implement compliance checking
    return true;
}

std::vector<std::string> PluginSecuritySystem::getComplianceIssuesInternal(const Plugin& plugin, const PluginSecurityPolicy& policy) {
    return std::vector<std::string>();
}

double PluginSecuritySystem::assessPluginRiskInternal(const Plugin& plugin, const PluginSecurityPolicy& policy) {
    // TODO: Implement risk assessment
    return 0.0;
}

std::string PluginSecuritySystem::getRiskLevelInternal(double riskScore) {
    if (riskScore < 0.3) {
        return "Low";
    } else if (riskScore < 0.7) {
        return "Medium";
    } else {
        return "High";
    }
}

bool PluginSecuritySystem::monitorPluginSecurityInternal(const std::string& pluginName, double intervalSeconds) {
    // TODO: Implement security monitoring
    return true;
}

bool PluginSecuritySystem::stopMonitoringPluginSecurityInternal(const std::string& pluginName) {
    // TODO: Implement security monitoring stop
    return true;
}

} // namespace nlm