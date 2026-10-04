#pragma once

#include "IPlugin.hpp"
#include "../plasticity/PlasticityRule.hpp"
#include "../brain/Synapse.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

// Plugin wrapper for PlasticityRule
class PlasticityRulePlugin : public IPlasticityRule {
public:
    PlasticityRulePlugin(std::unique_ptr<IPlasticityRule> rule);
    ~PlasticityRulePlugin() override;
    
    const char* getName() const override;
    const char* getVersion() const override { return NLM_PLUGIN_VERSION; }
    const char* getDescription() const override;
    PluginCategory getCategory() const override;
    
    bool initialize(std::map<std::string, std::string> config) override;
    void shutdown() override;
    PluginStatus getStatus() const override;
    
    bool setParameter(const std::string& name, const std::string& value) override;
    std::string getParameter(const std::string& name) const override;
    std::map<std::string, std::string> getAllParameters() const override;
    
    bool isEnabled() const override;
    void setEnabled(bool enabled) override;
    
    // IPlasticityRule interface
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    const char* getName() const override;
    
    float getLearningRate() const override;
    void setLearningRate(float rate) override;
    
    bool isApplicable(SimulationStep step, Timestamp time) const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Plugin wrapper for Neuromodulator
class NeuromodulatorPlugin : public INeuromodulator {
public:
    NeuromodulatorPlugin(std::unique_ptr<INeuromodulator> neuromodulator);
    ~NeuromodulatorPlugin() override;
    
    const char* getName() const override;
    const char* getVersion() const override { return NLM_PLUGIN_VERSION; }
    const char* getDescription() const override;
    PluginCategory getCategory() const override;
    
    bool initialize(std::map<std::string, std::string> config) override;
    void shutdown() override;
    PluginStatus getStatus() const override;
    
    bool setParameter(const std::string& name, const std::string& value) override;
    std::string getParameter(const std::string& name) const override;
    std::map<std::string, std::string> getAllParameters() const override;
    
    bool isEnabled() const override;
    void setEnabled(bool enabled) override;
    
    // INeuromodulator interface
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    float applyToPlasticity(float currentFactor, SimulationStep step) const override;
    
    const char* getTypeName() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Plugin wrapper for RewardFunction
class RewardFunctionPlugin : public IRewardFunction {
public:
    RewardFunctionPlugin(std::unique_ptr<IRewardFunction> rewardFunction);
    ~RewardFunctionPlugin() override;
    
    const char* getName() const override;
    const char* getVersion() const override { return NLM_PLUGIN_VERSION; }
    const char* getDescription() const override;
    PluginCategory getCategory() const override;
    
    bool initialize(std::map<std::string, std::string> config) override;
    void shutdown() override;
    PluginStatus getStatus() const override;
    
    bool setParameter(const std::string& name, const std::string& value) override;
    std::string getParameter(const std::string& name) const override;
    std::map<std::string, std::string> getAllParameters() const override;
    
    bool isEnabled() const override;
    void setEnabled(bool enabled) override;
    
    // IRewardFunction interface
    float computeReward(const Observation& observation, SimulationStep step) const override;
    
    float getBaselineReward() const override;
    void setBaselineReward(float baseline) override;
    
    bool isValid(const Observation& observation, SimulationStep step) const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Plugin wrapper for DevelopmentModel
class DevelopmentModelPlugin : public IDevelopmentModel {
public:
    DevelopmentModelPlugin(std::unique_ptr<IDevelopmentModel> model);
    ~DevelopmentModelPlugin() override;
    
    const char* getName() const override;
    const char* getVersion() const override { return NLM_PLUGIN_VERSION; }
    const char* getDescription() const override;
    PluginCategory getCategory() const override;
    
    bool initialize(std::map<std::string, std::string> config) override;
    void shutdown() override;
    PluginStatus getStatus() const override;
    
    bool setParameter(const std::string& name, const std::string& value) override;
    std::string getParameter(const std::string& name) const override;
    std::map<std::string, std::string> getAllParameters() const override;
    
    bool isEnabled() const override;
    void setEnabled(bool enabled) override;
    
    // IDevelopmentModel interface
    DevelopmentalStage getCurrentStage() const override;
    void advanceStage() override;
    float getDevelopmentProgress() const override;
    void update(class Brain* brain, TimestepDuration dt) override;
    
    const char* getStageName(DevelopmentalStage stage) const override;
    float getStageDuration(DevelopmentalStage stage) const override;
    
    bool isActive(SimulationStep step, Timestamp time) const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Factory functions for plugin creation
inline namespace PluginFactories {
    
    // Plasticity rule plugins
    std::unique_ptr<IPlugin> createSTDPPlugin();
    std::unique_ptr<IPlugin> createHebbianPlugin();
    std::unique_ptr<IPlugin> createStructuralPlasticityPlugin();
    std::unique_ptr<IPlugin> createAntiHebbianPlugin();
    std::unique_ptr<IPlugin> createBCMPlugin();
    std::unique_ptr<IPlugin> createTripletSTDPPlugin();
    
    // Neuromodulator plugins
    std::unique_ptr<IPlugin> createDopaminePlugin();
    std::unique_ptr<IPlugin> createCuriosityPlugin();
    std::unique_ptr<IPlugin> createNoveltyPlugin();
    std::unique_ptr<IPlugin> createAcetylcholinePlugin();
    std::unique_ptr<IPlugin> createNorepinephrinePlugin();
    std::unique_ptr<IPlugin> createSerotoninPlugin();
    
    // Reward function plugins
    std::unique_ptr<IPlugin> createPredictionErrorPlugin();
    std::unique_ptr<IPlugin> createSparseCodingPlugin();
    std::unique_ptr<IPlugin> createTemporalDifferencePlugin();
    
    // Development model plugins
    std::unique_ptr<IPlugin> createDevelopmentSystemPlugin();
    std::unique_ptr<IPlugin> createSynaptogenesisPlugin();
    std::unique_ptr<IPlugin> createPruningPlugin();
    std::unique_ptr<IPlugin> createMaturationPlugin();
}

} // namespace nlm
