#ifndef NLM_PLUGIN_ABSTRACTANALYSISPLUGIN_HPP
#define NLM_PLUGIN_ABSTRACTANALYSISPLUGIN_HPP

#include "PluginBase.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace nlm {
namespace plugin {

struct AnalysisResult {
    std::string pluginName;
    std::string analysisType;
    std::chrono::system_clock::time_point timestamp;
    std::unordered_map<std::string, double> metrics;
    std::unordered_map<std::string, std::string> metadata;
    bool success;
    std::string errorMessage;
};

class AbstractAnalysisPlugin : public virtual IPlugin {
public:
    virtual ~AbstractAnalysisPlugin() = default;
    
    virtual std::string getAnalysisType() const = 0;
    virtual std::vector<std::string> getInputTypes() const = 0;
    virtual std::vector<std::string> getOutputTypes() const = 0;
    
    // Analysis configuration
    virtual bool configureAnalysis(const std::unordered_map<std::string, std::string>& config) = 0;
    virtual std::unordered_map<std::string, std::string> getAnalysisConfig() const = 0;
    
    // Analysis lifecycle
    virtual bool preAnalysis() = 0;
    virtual bool postAnalysis() = 0;
    virtual bool resetAnalysis() = 0;
    
    // Analysis execution
    virtual AnalysisResult analyze(const std::unordered_map<std::string, std::string>& input) = 0;
    virtual std::vector<AnalysisResult> batchAnalyze(const std::vector<std::unordered_map<std::string, std::string>>& inputs) = 0;
    virtual bool validateInput(const std::unordered_map<std::string, std::string>& input) const = 0;
    virtual bool validateOutput(const AnalysisResult& result) const = 0;
    
    // Analysis metrics
    virtual double getAnalysisScore(const std::string& metric) const = 0;
    virtual std::vector<std::string> getAvailableMetrics() const = 0;
    virtual std::unordered_map<std::string, double> getAllMetrics() const = 0;
    
    // Analysis statistics
    virtual uint64_t getAnalysisCount() const = 0;
    virtual double getAverageAnalysisTime() const = 0;
    virtual uint64_t getAnalysisErrors() const = 0;
    virtual std::string getLastAnalysisError() const = 0;
    
    // Analysis extension points
    virtual void* getAnalysisExtension(const std::string& extensionName) = 0;
    virtual bool registerAnalysisExtension(const std::string& extensionName, void* extension) = 0;
    
    // Analysis optimization
    virtual bool optimizeAnalysisParameters() = 0;
    virtual bool tuneAnalysisParameters(const std::unordered_map<std::string, double>& parameters) = 0;
    
    // Analysis visualization
    virtual std::string generateVisualization(const std::string& format, const std::string& metricName) const = 0;
    virtual bool saveResults(const std::string& filename, const AnalysisResult& result) const = 0;
    
protected:
    std::unordered_map<std::string, void*> analysisExtensions_;
    std::unordered_map<std::string, std::string> config_;
    uint64_t analysisCount_ = 0;
    uint64_t analysisErrors_ = 0;
    std::chrono::duration<double> totalAnalysisTime_;
    std::unordered_map<std::string, double> metrics_;
};

} // namespace plugin
} // namespace nlm

#endif // NLM_PLUGIN_ABSTRACTANALYSISPLUGIN_HPP