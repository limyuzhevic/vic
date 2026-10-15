#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../plugins/PluginInterface.hpp"

namespace nlm {

class ExtensibleBrain : public Brain {
public:
    // Create extensible brain with configuration
    explicit ExtensibleBrain(std::shared_ptr<Config> config);
    ~ExtensibleBrain() override;
    
    // Disable copying, enable moving
    ExtensibleBrain(const ExtensibleBrain&) = delete;
    ExtensibleBrain& operator=(const ExtensibleBrain&) = delete;
    ExtensibleBrain(ExtensibleBrain&&) noexcept;
    ExtensibleBrain& operator=(ExtensibleBrain&&) noexcept;
    
    // Plugin management
    bool addNeuronModelPlugin(std::shared_ptr<INeuronModelPlugin> plugin);
    bool addPlasticityRulePlugin(std::shared_ptr<IPlasticityRulePlugin> plugin);
    bool addSynapticDynamicsPlugin(std::shared_ptr<ISynapticDynamicsPlugin> plugin);
    
    // Custom memory system
    class CustomMemorySystem* getCustomMemorySystem() const;
    bool setCustomMemorySystem(std::unique_ptr<CustomMemorySystem> memorySystem);
    
    // Advanced neuromodulator
    class AdvancedNeuromodulator* getAdvancedNeuromodulator() const;
    bool setAdvancedNeuromodulator(std::unique_ptr<AdvancedNeuromodulator> neuromodulator);
    
    // Neural architecture
    class NeuralArchitecture* getNeuralArchitecture() const;
    bool setNeuralArchitecture(std::unique_ptr<NeuralArchitecture> architecture);
    
    // Custom optimization hooks
    void setOptimizationHooks(std::unique_ptr<class OptimizationHooks> hooks);
    class OptimizationHooks* getOptimizationHooks() const;
    
    // Experiment framework
    void setExperimentFramework(std::unique_ptr<class AdvancedExperiment> experiment);
    class AdvancedExperiment* getExperimentFramework() const;
    
    // Architecture definition language support
    bool loadArchitectureFromFile(const std::string& filepath);
    bool loadArchitectureFromString(const std::string& yamlString);
    bool compileArchitecture(const class NeuralArchitecture& arch, const std::string& outputPath = "");
    
    // Runtime extensibility
    void enablePlugin(const std::string& pluginName);
    void disablePlugin(const std::string& pluginName);
    std::vector<std::string> getLoadedPlugins() const;
    
    // Configuration inheritance and validation
    void inheritConfiguration(const std::string& baseConfigPath);
    bool validateCurrentConfiguration() const;
    
    // Hyperparameter optimization
    bool runHyperparameterOptimization(const class HyperparameterOptimizer& optimizer);
    std::map<std::string, double> getOptimalParameters() const;
    
    // Performance profiling
    void enableProfiling(bool enable);
    bool isProfilingEnabled() const;
    std::string getPerformanceProfile() const;
    
    // Event system for extensibility
    using EventCallback = std::function<void(const std::string& eventName, const class EventData& data)>;
    void registerEventCallback(const std::string& eventType, EventCallback callback);
    void triggerEvent(const std::string& eventName, const class EventData& data);
    
    // Custom protocol support
    bool addCustomProtocol(std::unique_ptr<class CustomProtocol> protocol);
    class CustomProtocol* getCustomProtocol(const std::string& name) const;
    
    // Scripted network construction
    void loadConnectionPatterns(const std::string& patternsFile);
    void applyTopologicalPatterns(const std::string& patternType);
    void generateStructuralPlasticityRules(const std::string& ruleFile);
    
protected:
    // Initialize with extensibility features
    bool initialize() override;
    
    // Update with plugin integration
    void step(SimulationStep currentStep) override;
    void step(SimulationStep currentStep, Timestamp currentTime) override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
