#pragma once

#include "ExperimentProtocol.hpp"

namespace nlm {

class CustomExperimentProtocol : public ExperimentProtocol {
public:
    virtual ~CustomExperimentProtocol() = default;
    
    // Custom protocol configuration
    virtual bool setCustomParameters(const std::map<std::string, std::string>& parameters) = 0;
    virtual std::map<std::string, std::string> getCustomParameters() const = 0;
    virtual bool validateCustomParameters() const = 0;
    
    // Plugin support for custom protocols
    virtual bool addPlugin(std::shared_ptr<ExperimentProtocol> plugin) = 0;
    virtual bool removePlugin(const std::string& pluginName) = 0;
    virtual std::vector<std::string> getAvailablePlugins() const = 0;
    
    // Performance optimization
    virtual bool optimizePerformance() = 0;
    virtual bool enableCaching(bool enable) = 0;
    virtual bool isCachingEnabled() const = 0;
    
    // Advanced features
    virtual bool enableParallelExecution(bool enable) = 0;
    virtual bool enableDynamicReconfiguration(bool enable) = 0;
};

} // namespace nlm