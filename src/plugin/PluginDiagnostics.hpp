#pragma once

#include <string>
#include <vector>
#include <map>
#include <mutex>
#include "PluginBase.hpp"

namespace nlm {

class PluginDiagnostics {
public:
    PluginDiagnostics();
    ~PluginDiagnostics();
    
    // Unit testing
    bool runUnitTests(const std::string& pluginName);
    bool runIntegrationTests(const std::string& pluginName);
    bool runPerformanceTests(const std::string& pluginName);
    bool runSecurityTests(const std::string& pluginName);
    
    // Validation
    bool validatePlugin(const std::string& pluginPath);
    bool validatePluginFunctionality(const std::string& pluginName);
    bool validatePluginSecurity(const std::string& pluginPath);
    
    // Benchmarking
    double benchmarkPlugin(const std::string& pluginName, double deltaTime);
    size_t benchmarkPluginMemory(const std::string& pluginName);
    double benchmarkPluginCPU(const std::string& pluginName, double deltaTime);
    
    // Stress testing
    bool stressTestPlugin(const std::string& pluginName, int duration);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
