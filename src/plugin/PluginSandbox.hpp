#pragma once

#include <string>
#include <vector>
#include <map>
#include <mutex>
#include "PluginBase.hpp"

namespace nlm {

enum class PluginPermission {
    READ,
    WRITE,
    EXECUTE,
    NETWORK,
    FILESYSTEM,
    DEBUG,
    ADMIN
};

class PluginSandbox {
public:
    PluginSandbox();
    ~PluginSandbox();
    
    // Security policies
    bool checkSecurityPolicy(const PluginConfig& config);
    bool validatePluginCode(const std::string& code);
    bool checkResourceLimits(const PluginConfig& config);
    
    // Isolation
    void isolatePlugin(const std::string& pluginName);
    void restorePluginIsolation(const std::string& pluginName);
    
    // Permissions
    bool checkPermission(const std::string& pluginName, PluginPermission permission);
    void grantPermission(const std::string& pluginName, PluginPermission permission);
    void revokePermission(const std::string& pluginName, PluginPermission permission);
    
    // Security policies
    void setSecurityPolicy(const std::string& policyName, const std::string& policyData);
    std::string getSecurityPolicy(const std::string& policyName) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
