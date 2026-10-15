#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include "../core/Config/Config.hpp"
#include "../core/ExtensibleBrain.hpp"
#include "../plugins/PluginInterface.hpp"
#include "../arch/ArchitectureCompiler.hpp"

namespace nlm {

class NeuralArchitecture {
public:
    NeuralArchitecture();
    ~NeuralArchitecture();
    
    // Disable copying, enable moving
    NeuralArchitecture(const NeuralArchitecture&) = delete;
    NeuralArchitecture& operator=(const NeuralArchitecture&) = delete;
    NeuralArchitecture(NeuralArchitecture&&) noexcept;
    NeuralArchitecture& operator=(NeuralArchitecture&&) noexcept;
    
    // Architecture lifecycle
    bool initialize(const Config& config);
    void update(double timestep);
    void shutdown();
    
    // Architecture management
    void addRegion(const RegionSpec& region);
    void removeRegion(const std::string& regionName);
    RegionSpec* getRegion(const std::string& regionName);
    const RegionSpec* getRegion(const std::string& regionName) const;
    std::vector<RegionSpec>& getRegions();
    const std::vector<RegionSpec>& getRegions() const;
    
    void addPopulation(const PopulationSpec& population);
    void removePopulation(const std::string& populationName);
    PopulationSpec* getPopulation(const std::string& populationName);
    const PopulationSpec* getPopulation(const std::string& populationName) const;
    std::vector<PopulationSpec>& getPopulations();
    const std::vector<PopulationSpec>& getPopulations() const;
    
    void addConnection(const ConnectionSpec& connection);
    void removeConnection(const std::string& sourceRegion, const std::string& targetRegion);
    ConnectionSpec* getConnection(const std::string& sourceRegion, const std::string& targetRegion);
    const ConnectionSpec* getConnection(const std::string& sourceRegion, const std::string& targetRegion) const;
    std::vector<ConnectionSpec>& getConnections();
    const std::vector<ConnectionSpec>& getConnections() const;
    
    // Architecture compilation
    bool compile(const std::string& outputPath = "");
    bool compileToCode(const std::string& outputPath = "");
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    std::vector<std::string> getValidationWarnings() const;
    
    // Architecture execution
    bool build(const std::string& brainName = "GeneratedBrain");
    bool execute(double duration = 0.0);
    bool stop();
    bool reset();
    
    // Architecture analysis
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getRegionCount() const;
    size_t getPopulationCount() const;
    size_t getConnectionCount() const;
    std::string getArchitectureStats() const;
    
    // Configuration management
    void setConfig(const Config& config);
    Config getConfig() const;
    
    // Plugin integration
    bool addNeuronModelPlugin(std::shared_ptr<INeuronModelPlugin> plugin);
    bool addPlasticityRulePlugin(std::shared_ptr<IPlasticityRulePlugin> plugin);
    bool addSynapticDynamicsPlugin(std::shared_ptr<ISynapticDynamicsPlugin> plugin);
    
    // Custom optimization
    void enableOptimization(bool enable);
    bool isOptimizationEnabled() const;
    void setOptimizationLevel(int level);
    int getOptimizationLevel() const;
    
    // Performance monitoring
    void enablePerformanceMonitoring(bool enable);
    bool isPerformanceMonitoringEnabled() const;
    std::string getPerformanceProfile() const;
    
    // Architecture templates
    bool saveAsTemplate(const std::string& templatePath) const;
    bool loadFromTemplate(const std::string& templatePath);
    std::vector<std::string> getAvailableTemplates() const;
    
    // Scripted network construction
    void loadConnectionPatterns(const std::string& patternsFile);
    void applyTopologicalPatterns(const std::string& patternType);
    void generateStructuralPlasticityRules(const std::string& ruleFile);
    
    // Neural architecture builder
    class NeuralArchitectureBuilder* getBuilder() const;
    bool setBuilder(std::unique_ptr<NeuralArchitectureBuilder> builder);
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Neural architecture builder interface
class NeuralArchitectureBuilder {
public:
    virtual ~NeuralArchitectureBuilder() = default;
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual bool initialize(const Config& config) = 0;
    virtual void update(double timestep) = 0;
    virtual void shutdown() = 0;
    
    // Building methods
    virtual void addRegion(const RegionSpec& region) = 0;
    virtual void addPopulation(const PopulationSpec& population) = 0;
    virtual void addConnection(const ConnectionSpec& connection) = 0;
    virtual void clear() = 0;
    
    // Topological pattern generation
    virtual void generateRandomTopology(size_t regionCount, size_t populationSize) = 0;
    virtual void generateFeedforwardTopology(size_t layerCount, size_t neuronsPerLayer) = 0;
    virtual void generateRecurrentTopology(size_t regionCount, float connectionProbability) = 0;
    virtual void generateModularTopology(const std::vector<std::vector<size_t>>& moduleSpecs) = 0;
    
    // Connection pattern specifications
    virtual void addFeedforwardConnections(const std::string& fromRegion, const std::string& toRegion) = 0;
    virtual void addFeedbackConnections(const std::string& fromRegion, const std::string& toRegion) = 0;
    virtual void addRecurrentConnections(const std::string& region, float probability) = 0;
    virtual void addSkipConnections(const std::vector<std::string>& layers) = 0;
    
    // Structural plasticity rules as code
    virtual void addStructuralPlasticityRule(const std::string& ruleName,
                                           std::function<bool(const SynapseState&, const SynapseState&)> rule) = 0;
    virtual void removeStructuralPlasticityRule(const std::string& ruleName) = 0;
    virtual std::vector<std::string> getStructuralPlasticityRules() const = 0;
};

// Default neural architecture builder
class DefaultNeuralArchitectureBuilder : public NeuralArchitectureBuilder {
public:
    DefaultNeuralArchitectureBuilder() = default;
    ~DefaultNeuralArchitectureBuilder() override = default;
    
    std::string getName() const override { return "DefaultBuilder"; }
    std::string getVersion() const override { return "1.0.0"; }
    
    bool initialize(const Config& config) override;
    void update(double timestep) override;
    void shutdown() override;
    
    void addRegion(const RegionSpec& region) override;
    void addPopulation(const PopulationSpec& population) override;
    void addConnection(const ConnectionSpec& connection) override;
    void clear() override;
    
    void generateRandomTopology(size_t regionCount, size_t populationSize) override;
    void generateFeedforwardTopology(size_t layerCount, size_t neuronsPerLayer) override;
    void generateRecurrentTopology(size_t regionCount, float connectionProbability) override;
    void generateModularTopology(const std::vector<std::vector<size_t>>& moduleSpecs) override;
    
    void addFeedforwardConnections(const std::string& fromRegion, const std::string& toRegion) override;
    void addFeedbackConnections(const std::string& fromRegion, const std::string& toRegion) override;
    void addRecurrentConnections(const std::string& region, float probability) override;
    void addSkipConnections(const std::vector<std::string>& layers) override;
    
    void addStructuralPlasticityRule(const std::string& ruleName,
                                   std::function<bool(const SynapseState&, const SynapseState&)> rule) override;
    void removeStructuralPlasticityRule(const std::string& ruleName) override;
    std::vector<std::string> getStructuralPlasticityRules() const override;
    
private:
    std::vector<RegionSpec> regions;
    std::vector<PopulationSpec> populations;
    std::vector<ConnectionSpec> connections;
    std::map<std::string, std::function<bool(const SynapseState&, const SynapseState&)>> plasticityRules;
};

// Connection specification
struct ConnectionSpec {
    std::string sourceRegion;
    std::string targetRegion;
    std::string connectionType; // "feedforward", "feedback", "recurrent"
    double connectionProbability;
    std::map<std::string, double> weightParameters;
    std::vector<std::string> plasticityRules;
    std::map<std::string, std::string> metadata;
    
    ConnectionSpec()
        : connectionProbability(0.5), connectionType("feedforward") {}
    
    ConnectionSpec(const std::string& src, const std::string& tgt)
        : sourceRegion(src), targetRegion(tgt), connectionProbability(0.5), connectionType("feedforward") {}
};

} // namespace nlm
