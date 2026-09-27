#ifndef NLM_PLUGIN_EXAMPLES_PLASTICITYPLUGIN_HPP
#define NLM_PLUGIN_EXAMPLES_PLASTICITYPLUGIN_HPP

#include "AbstractProcessingPlugin.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>

namespace nlm {
namespace plugin {

class PlasticityPlugin : public AbstractProcessingPlugin {
public:
    PlasticityPlugin();
    ~PlasticityPlugin() override;
    
    // IPlugin interface implementation
    std::string getName() const override;
    std::string getVersion() const override;
    std::string getDescription() const override;
    PluginType getType() const override;
    PluginMetadata getMetadata() const override;
    
    bool initialize(const PluginMetadata& metadata) override;
    void shutdown() override;
    bool activate() override;
    bool deactivate() override;
    bool loadDependencies() override;
    bool checkConflicts() override;
    
    // AbstractProcessingPlugin interface implementation
    std::string getProcessingType() const override;
    std::vector<std::string> getInputTypes() const override;
    std::vector<std::string> getOutputTypes() const override;
    
    bool configureProcessing(const std::unordered_map<std::string, std::string>& config) override;
    std::unordered_map<std::string, std::string> getProcessingConfig() const override;
    
    bool preProcess() override;
    bool postProcess() override;
    bool resetProcessing() override;
    
    bool process(const ProcessingContext& context) override;
    std::vector<ProcessingContext> batchProcess(const std::vector<ProcessingContext>& contexts) override;
    bool validateInput(const ProcessingContext& context) const override;
    bool validateOutput(const ProcessingContext& context) const override;
    
    bool addProcessingHook(const std::string& hookName, const std::function<bool(ProcessingContext&)>& hook) override;
    bool removeProcessingHook(const std::string& hookName) override;
    std::vector<std::string> getProcessingHooks() const override;
    
    uint64_t getProcessCount() const override;
    double getAverageProcessingTime() const override;
    uint64_t getErrorCount() const override;
    std::string getLastError() const override;
    
    bool enablePerformanceMonitoring(bool enabled) override;
    bool isPerformanceMonitoringEnabled() const override;
    void resetPerformanceStats() override;
    
    void* getProcessingExtension(const std::string& extensionName) override;
    bool registerProcessingExtension(const std::string& extensionName, void* extension) override;
    
    bool optimizeParameters() override;
    bool tuneParameters(const std::unordered_map<std::string, double>& parameters) override;
    
    // Plasticity-specific functionality
    class PlasticityRule {
    public:
        PlasticityRule(const std::string& name, PlasticityPlugin* plugin);
        ~PlasticityRule();
        
        std::string getName() const { return name_; }
        std::string getType() const { return type_; }
        double getStrength() const { return strength_; }
        double getLearningRate() const { return learningRate_; }
        
        void setType(const std::string& type) { type_ = type; }
        void setStrength(double strength) { strength_ = strength; }
        void setLearningRate(double rate) { learningRate_ = rate; }
        
        virtual bool apply(ProcessingContext& context) = 0;
        virtual bool validate(const ProcessingContext& context) const = 0;
        
    protected:
        std::string name_;
        std::string type_;
        double strength_;
        double learningRate_;
        PlasticityPlugin* plugin_;
    };
    
    class STDPPlasticityRule : public PlasticityRule {
    public:
        STDPPlasticityRule(const std::string& name, PlasticityPlugin* plugin);
        ~STDPPlasticityRule() override;
        
        bool apply(ProcessingContext& context) override;
        bool validate(const ProcessingContext& context) const override;
        
    private:
        std::unordered_map<std::string, double> preSynapticTrace_;
        std::unordered_map<std::string, double> postSynapticTrace_;
        double timeConstant_;
    };
    
    class HebbianPlasticityRule : public PlasticityRule {
    public:
        HebbianPlasticityRule(const std::string& name, PlasticityPlugin* plugin);
        ~HebbianPlasticityRule() override;
        
        bool apply(ProcessingContext& context) override;
        bool validate(const ProcessingContext& context) const override;
        
    private:
        std::unordered_map<std::string, double> correlationMatrix_;
        double threshold_;
    };
    
    PlasticityRule* createPlasticityRule(const std::string& name, const std::string& type);
    bool destroyPlasticityRule(PlasticityRule* rule);
    std::vector<PlasticityRule*> getPlasticityRules() const;
    PlasticityRule* getPlasticityRule(const std::string& name) const;
    
    // Plasticity management
    bool addPlasticityRule(PlasticityRule* rule);
    bool removePlasticityRule(const std::string& name);
    bool enablePlasticityRule(const std::string& name);
    bool disablePlasticityRule(const std::string& name);
    std::vector<PlasticityRule*> getEnabledPlasticityRules() const;
    
    // Plasticity plugin configuration
    struct PlasticityConfig {
        double stdpTimeConstant = 20.0;
        double stdpLearningRate = 0.01;
        double hebbianThreshold = 0.5;
        double hebbianLearningRate = 0.001;
        bool enableSTDP = true;
        bool enableHebbian = true;
        bool enableStructuralPlasticity = false;
        double structuralPlasticityRate = 0.001;
        bool enableHomeostaticPlasticity = false;
        double homeostaticTarget = 1.0;
        double homeostaticLearningRate = 0.0001;
    };
    
    PlasticityConfig getPlasticityConfig() const { return config_; }
    void setPlasticityConfig(const PlasticityConfig& config) { config_ = config; }
    
private:
    PluginMetadata metadata_;
    PluginState state_;
    std::string error_;
    PlasticityConfig config_;
    std::vector<PlasticityRule*> plasticityRules_;
    std::unordered_map<std::string, void*> processingExtensions_;
    std::unordered_map<std::string, std::function<bool(ProcessingContext&)>> hooks_;
    std::unordered_map<std::string, std::string> config_;
    uint64_t processCount_ = 0;
    uint64_t errorCount_ = 0;
    std::chrono::duration<double> totalProcessingTime_;
};

} // namespace plugin
} // namespace nlm

#endif // NLM_PLUGIN_EXAMPLES_PLASTICITYPLUGIN_HPP