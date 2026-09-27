#ifndef NLM_PLUGIN_ABSTRACTPROCESSINGPLUGIN_HPP
#define NLM_PLUGIN_ABSTRACTPROCESSINGPLUGIN_HPP

#include "PluginBase.hpp"
#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace nlm {
namespace plugin {

struct ProcessingContext {
    std::string pluginName;
    std::chrono::system_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> inputData;
    std::unordered_map<std::string, std::string> outputData;
    double processingTime;
    std::string errorMessage;
    bool success;
};

class AbstractProcessingPlugin : public virtual IPlugin {
public:
    virtual ~AbstractProcessingPlugin() = default;
    
    virtual std::string getProcessingType() const = 0;
    virtual std::vector<std::string> getInputTypes() const = 0;
    virtual std::vector<std::string> getOutputTypes() const = 0;
    
    // Processing configuration
    virtual bool configureProcessing(const std::unordered_map<std::string, std::string>& config) = 0;
    virtual std::unordered_map<std::string, std::string> getProcessingConfig() const = 0;
    
    // Processing lifecycle
    virtual bool preProcess() = 0;
    virtual bool postProcess() = 0;
    virtual bool resetProcessing() = 0;
    
    // Processing execution
    virtual bool process(const ProcessingContext& context) = 0;
    virtual std::vector<ProcessingContext> batchProcess(const std::vector<ProcessingContext>& contexts) = 0;
    virtual bool validateInput(const ProcessingContext& context) const = 0;
    virtual bool validateOutput(const ProcessingContext& context) const = 0;
    
    // Processing hooks
    virtual bool addProcessingHook(const std::string& hookName, const std::function<bool(ProcessingContext&)>& hook) = 0;
    virtual bool removeProcessingHook(const std::string& hookName) = 0;
    virtual std::vector<std::string> getProcessingHooks() const = 0;
    
    // Processing statistics
    virtual uint64_t getProcessCount() const = 0;
    virtual double getAverageProcessingTime() const = 0;
    virtual uint64_t getErrorCount() const = 0;
    virtual std::string getLastError() const = 0;
    
    // Processing performance
    virtual bool enablePerformanceMonitoring(bool enabled) = 0;
    virtual bool isPerformanceMonitoringEnabled() const = 0;
    virtual void resetPerformanceStats() = 0;
    
    // Processing extension points
    virtual void* getProcessingExtension(const std::string& extensionName) = 0;
    virtual bool registerProcessingExtension(const std::string& extensionName, void* extension) = 0;
    
    // Processing optimization
    virtual bool optimizeParameters() = 0;
    virtual bool tuneParameters(const std::unordered_map<std::string, double>& parameters) = 0;
    
protected:
    std::unordered_map<std::string, void*> processingExtensions_;
    std::unordered_map<std::string, std::function<bool(ProcessingContext&)>> hooks_;
    std::unordered_map<std::string, std::string> config_;
    uint64_t processCount_ = 0;
    uint64_t errorCount_ = 0;
    std::chrono::duration<double> totalProcessingTime_;
};

} // namespace plugin
} // namespace nlm

#endif // NLM_PLUGIN_ABSTRACTPROCESSINGPLUGIN_HPP