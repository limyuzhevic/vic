#include "PluginWrappers.hpp"
#include <algorithm>

namespace nlm {

// Plugin wrapper for PlasticityRule implementation
struct PlasticityRulePlugin::Impl {
    std::unique_ptr<IPlasticityRule> rule;
    std::map<std::string, std::string> parameters;
    PluginStatus status;
    std::string name;
    std::string description;
    
    Impl(std::unique_ptr<IPlasticityRule> r) : rule(std::move(r)) {
        name = rule->getName();
        description = rule->getDescription();
        status = PluginStatus::UNLOADED;
    }
};

PlasticityRulePlugin::PlasticityRulePlugin(std::unique_ptr<IPlasticityRule> rule) 
    : pImpl(std::make_unique<Impl>(std::move(rule))) {}

PlasticityRulePlugin::~PlasticityRulePlugin() = default;

const char* PlasticityRulePlugin::getName() const {
    return pImpl->name.c_str();
}

const char* PlasticityRulePlugin::getDescription() const {
    return pImpl->description.c_str();
}

PluginCategory PlasticityRulePlugin::getCategory() const {
    return PluginCategory::PLASTICITY;
}

bool PlasticityRulePlugin::initialize(std::map<std::string, std::string> config) {
    pImpl->status = PluginStatus::LOADING;
    
    for (const auto& param : config) {
        pImpl->parameters[param.first] = param.second;
    }
    
    // Apply parameters to the underlying rule
    for (const auto& param : pImpl->parameters) {
        setParameter(param.first, param.second);
    }
    
    pImpl->status = PluginStatus::LOADED;
    return true;
}

void PlasticityRulePlugin::shutdown() {
    pImpl->parameters.clear();
    pImpl->status = PluginStatus::UNLOADED;
}

PluginStatus PlasticityRulePlugin::getStatus() const {
    return pImpl->status;
}

bool PlasticityRulePlugin::setParameter(const std::string& name, const std::string& value) {
    pImpl->parameters[name] = value;
    
    // Apply specific parameters to the underlying rule
    if (name == "learning_rate") {
        float rate = 0.0f;
        PluginUtils::stringToFloat(value, rate);
        setLearningRate(rate);
    } else if (name == "enabled") {
        bool enabled = (value == "true" || value == "1" || value == "yes");
        setEnabled(enabled);
    }
    
    return true;
}

std::string PlasticityRulePlugin::getParameter(const std::string& name) const {
    auto it = pImpl->parameters.find(name);
    if (it != pImpl->parameters.end()) {
        return it->second;
    }
    return "";
}

std::map<std::string, std::string> PlasticityRulePlugin::getAllParameters() const {
    return pImpl->parameters;
}

bool PlasticityRulePlugin::isEnabled() const {
    return pImpl->rule->isEnabled();
}

void PlasticityRulePlugin::setEnabled(bool enabled) {
    pImpl->rule->setEnabled(enabled);
}

void PlasticityRulePlugin::update(Synapse* synapse,
                                   const std::vector<Timestamp>& preSpikes,
                                   const std::vector<Timestamp>& postSpikes,
                                   TimestepDuration dt) {
    if (pImpl->rule->isEnabled()) {
        pImpl->rule->update(synapse, preSpikes, postSpikes, dt);
    }
}

void PlasticityRulePlugin::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    pImpl->rule->applyWeightChange(synapse, delta);
}

const char* PlasticityRulePlugin::getName() const {
    return pImpl->rule->getName();
}

float PlasticityRulePlugin::getLearningRate() const {
    return pImpl->rule->getLearningRate();
}

void PlasticityRulePlugin::setLearningRate(float rate) {
    pImpl->rule->setLearningRate(rate);
}

bool PlasticityRulePlugin::isApplicable(SimulationStep step, Timestamp time) const {
    return pImpl->rule->isApplicable(step, time);
}

// Plugin wrapper for Neuromodulator implementation
struct NeuromodulatorPlugin::Impl {
    std::unique_ptr<INeuromodulator> neuromodulator;
    std::map<std::string, std::string> parameters;
    PluginStatus status;
    std::string name;
    std::string description;
    std::string typeName;
    
    Impl(std::unique_ptr<INeuromodulator> nm) : neuromodulator(std::move(nm)) {
        name = neuromodulator->getName();
        description = neuromodulator->getDescription();
        typeName = neuromodulator->getTypeName();
        status = PluginStatus::UNLOADED;
    }
};

NeuromodulatorPlugin::NeuromodulatorPlugin(std::unique_ptr<INeuromodulator> neuromodulator)
    : pImpl(std::make_unique<Impl>(std::move(neuromulator))) {}

NeuromodulatorPlugin::~NeuromodulatorPlugin() = default;

const char* NeuromodulatorPlugin::getName() const {
    return pImpl->name.c_str();
}

const char* NeuromodulatorPlugin::getDescription() const {
    return pImpl->description.c_str();
}

PluginCategory NeuromodulatorPlugin::getCategory() const {
    return PluginCategory::NEUROMODULATION;
}

bool NeuromodulatorPlugin::initialize(std::map<std::string, std::string> config) {
    pImpl->status = PluginStatus::LOADING;
    
    for (const auto& param : config) {
        pImpl->parameters[param.first] = param.second;
    }
    
    // Apply parameters to the underlying neuromodulator
    for (const auto& param : pImpl->parameters) {
        setParameter(param.first, param.second);
    }
    
    pImpl->status = PluginStatus::LOADED;
    return true;
}

void NeuromodulatorPlugin::shutdown() {
    pImpl->parameters.clear();
    pImpl->status = PluginStatus::UNLOADED;
}

PluginStatus NeuromodulatorPlugin::getStatus() const {
    return pImpl->status;
}

bool NeuromodulatorPlugin::setParameter(const std::string& name, const std::string& value) {
    pImpl->parameters[name] = value;
    
    // Apply specific parameters to the underlying neuromodulator
    if (name == "level") {
        float level = 0.0f;
        PluginUtils::stringToFloat(value, level);
        setLevel(level);
    } else if (name == "enabled") {
        bool enabled = (value == "true" || value == "1" || value == "yes");
        setEnabled(enabled);
    }
    
    return true;
}

std::string NeuromodulatorPlugin::getParameter(const std::string& name) const {
    auto it = pImpl->parameters.find(name);
    if (it != pImpl->parameters.end()) {
        return it->second;
    }
    return "";
}

std::map<std::string, std::string> NeuromodulatorPlugin::getAllParameters() const {
    return pImpl->parameters;
}

bool NeuromodulatorPlugin::isEnabled() const {
    return pImpl->neuromodulator->isEnabled();
}

void NeuromodulatorPlugin::setEnabled(bool enabled) {
    pImpl->neuromodulator->setEnabled(enabled);
}

float NeuromodulatorPlugin::getLevel() const {
    return pImpl->neuromodulator->getLevel();
}

void NeuromodulatorPlugin::setLevel(float level) {
    pImpl->neuromodulator->setLevel(level);
}

float NeuromodulatorPlugin::getPlasticityFactor() const {
    return pImpl->neuromodulator->getPlasticityFactor();
}

void NeuromodulatorPlugin::update(TimestepDuration dt) {
    pImpl->neuromodulator->update(dt);
}

float NeuromodulatorPlugin::applyToPlasticity(float currentFactor, SimulationStep step) const {
    return pImpl->neuromodulator->applyToPlasticity(currentFactor, step);
}

const char* NeuromodulatorPlugin::getTypeName() const {
    return pImpl->typeName.c_str();
}

// Plugin wrapper for RewardFunction implementation
struct RewardFunctionPlugin::Impl {
    std::unique_ptr<IRewardFunction> rewardFunction;
    std::map<std::string, std::string> parameters;
    PluginStatus status;
    std::string name;
    std::string description;
    
    Impl(std::unique_ptr<IRewardFunction> rf) : rewardFunction(std::move(rf)) {
        name = rewardFunction->getName();
        description = rewardFunction->getDescription();
        status = PluginStatus::UNLOADED;
    }
};

RewardFunctionPlugin::RewardFunctionPlugin(std::unique_ptr<IRewardFunction> rewardFunction)
    : pImpl(std::make_unique<Impl>(std::move(rewardFunction))) {}

RewardFunctionPlugin::~RewardFunctionPlugin() = default;

const char* RewardFunctionPlugin::getName() const {
    return pImpl->name.c_str();
}

const char* RewardFunctionPlugin::getDescription() const {
    return pImpl->description.c_str();
}

PluginCategory RewardFunctionPlugin::getCategory() const {
    return PluginCategory::REWARD;
}

bool RewardFunctionPlugin::initialize(std::map<std::string, std::string> config) {
    pImpl->status = PluginStatus::LOADING;
    
    for (const auto& param : config) {
        pImpl->parameters[param.first] = param.second;
    }
    
    // Apply parameters to the underlying reward function
    for (const auto& param : pImpl->parameters) {
        setParameter(param.first, param.second);
    }
    
    pImpl->status = PluginStatus::LOADED;
    return true;
}

void RewardFunctionPlugin::shutdown() {
    pImpl->parameters.clear();
    pImpl->status = PluginStatus::UNLOADED;
}

PluginStatus RewardFunctionPlugin::getStatus() const {
    return pImpl->status;
}

bool RewardFunctionPlugin::setParameter(const std::string& name, const std::string& value) {
    pImpl->parameters[name] = value;
    
    // Apply specific parameters to the underlying reward function
    if (name == "baseline_reward") {
        float baseline = 0.0f;
        PluginUtils::stringToFloat(value, baseline);
        setBaselineReward(baseline);
    } else if (name == "enabled") {
        bool enabled = (value == "true" || value == "1" || value == "yes");
        setEnabled(enabled);
    }
    
    return true;
}

std::string RewardFunctionPlugin::getParameter(const std::string& name) const {
    auto it = pImpl->parameters.find(name);
    if (it != pImpl->parameters.end()) {
        return it->second;
    }
    return "";
}

std::map<std::string, std::string> RewardFunctionPlugin::getAllParameters() const {
    return pImpl->parameters;
}

bool RewardFunctionPlugin::isEnabled() const {
    return pImpl->rewardFunction->isEnabled();
}

void RewardFunctionPlugin::setEnabled(bool enabled) {
    pImpl->rewardFunction->setEnabled(enabled);
}

float RewardFunctionPlugin::computeReward(const Observation& observation, SimulationStep step) const {
    if (pImpl->rewardFunction->isEnabled()) {
        return pImpl->rewardFunction->computeReward(observation, step);
    }
    return pImpl->rewardFunction->getBaselineReward();
}

float RewardFunctionPlugin::getBaselineReward() const {
    return pImpl->rewardFunction->getBaselineReward();
}

void RewardFunctionPlugin::setBaselineReward(float baseline) {
    pImpl->rewardFunction->setBaselineReward(baseline);
}

bool RewardFunctionPlugin::isValid(const Observation& observation, SimulationStep step) const {
    return pImpl->rewardFunction->isValid(observation, step);
}

// Plugin wrapper for DevelopmentModel implementation
struct DevelopmentModelPlugin::Impl {
    std::unique_ptr<IDevelopmentModel> model;
    std::map<std::string, std::string> parameters;
    PluginStatus status;
    std::string name;
    std::string description;
    
    Impl(std::unique_ptr<IDevelopmentModel> m) : model(std::move(m)) {
        name = model->getName();
        description = model->getDescription();
        status = PluginStatus::UNLOADED;
    }
};

DevelopmentModelPlugin::DevelopmentModelPlugin(std::unique_ptr<IDevelopmentModel> model)
    : pImpl(std::make_unique<Impl>(std::move(model))) {}

DevelopmentModelPlugin::~DevelopmentModelPlugin() = default;

const char* DevelopmentModelPlugin::getName() const {
    return pImpl->name.c_str();
}

const char* DevelopmentModelPlugin::getDescription() const {
    return pImpl->description.c_str();
}

PluginCategory DevelopmentModelPlugin::getCategory() const {
    return PluginCategory::DEVELOPMENT;
}

bool DevelopmentModelPlugin::initialize(std::map<std::string, std::string> config) {
    pImpl->status = PluginStatus::LOADING;
    
    for (const auto& param : config) {
        pImpl->parameters[param.first] = param.second;
    }
    
    // Apply parameters to the underlying development model
    for (const auto& param : pImpl->parameters) {
        setParameter(param.first, param.second);
    }
    
    pImpl->status = PluginStatus::LOADED;
    return true;
}

void DevelopmentModelPlugin::shutdown() {
    pImpl->parameters.clear();
    pImpl->status = PluginStatus::UNLOADED;
}

PluginStatus DevelopmentModelPlugin::getStatus() const {
    return pImpl->status;
}

bool DevelopmentModelPlugin::setParameter(const std::string& name, const std::string& value) {
    pImpl->parameters[name] = value;
    
    // Apply specific parameters to the underlying development model
    if (name == "enabled") {
        bool enabled = (value == "true" || value == "1" || value == "yes");
        setEnabled(enabled);
    }
    
    return true;
}

std::string DevelopmentModelPlugin::getParameter(const std::string& name) const {
    auto it = pImpl->parameters.find(name);
    if (it != pImpl->parameters.end()) {
        return it->second;
    }
    return "";
}

std::map<std::string, std::string> DevelopmentModelPlugin::getAllParameters() const {
    return pImpl->parameters;
}

bool DevelopmentModelPlugin::isEnabled() const {
    return pImpl->model->isEnabled();
}

void DevelopmentModelPlugin::setEnabled(bool enabled) {
    pImpl->model->setEnabled(enabled);
}

DevelopmentalStage DevelopmentModelPlugin::getCurrentStage() const {
    return pImpl->model->getCurrentStage();
}

void DevelopmentModelPlugin::advanceStage() {
    pImpl->model->advanceStage();
}

float DevelopmentModelPlugin::getDevelopmentProgress() const {
    return pImpl->model->getDevelopmentProgress();
}

void DevelopmentModelPlugin::update(class Brain* brain, TimestepDuration dt) {
    if (pImpl->model->isActive(brain->getStep(), 0.0f)) {
        pImpl->model->update(brain, dt);
    }
}

const char* DevelopmentModelPlugin::getStageName(DevelopmentalStage stage) const {
    return pImpl->model->getStageName(stage);
}

float DevelopmentModelPlugin::getStageDuration(DevelopmentalStage stage) const {
    return pImpl->model->getStageDuration(stage);
}

bool DevelopmentModelPlugin::isActive(SimulationStep step, Timestamp time) const {
    return pImpl->model->isActive(step, time);
}

// Factory function implementations
namespace PluginFactories {
    
    std::unique_ptr<IPlugin> createSTDPPlugin() {
        auto rule = std::make_unique<STDP>();
        return std::make_unique<PlasticityRulePlugin>(std::move(rule));
    }
    
    std::unique_ptr<IPlugin> createHebbianPlugin() {
        auto rule = std::make_unique<HebbianRule>();
        return std::make_unique<PlasticityRulePlugin>(std::move(rule));
    }
    
    std::unique_ptr<IPlugin> createStructuralPlasticityPlugin() {
        auto rule = std::make_unique<StructuralPlasticity>();
        return std::make_unique<PlasticityRulePlugin>(std::move(rule));
    }
    
    std::unique_ptr<IPlugin> createAntiHebbianPlugin() {
        auto rule = std::make_unique<AntiHebbianRule>();
        return std::make_unique<PlasticityRulePlugin>(std::move(rule));
    }
    
    std::unique_ptr<IPlugin> createBCMPlugin() {
        auto rule = std::make_unique<BCMRule>();
        return std::make_unique<PlasticityRulePlugin>(std::move(rule));
    }
    
    std::unique_ptr<IPlugin> createTripletSTDPPlugin() {
        auto rule = std::make_unique<TripletSTDP>();
        return std::make_unique<PlasticityRulePlugin>(std::move(rule));
    }
    
    std::unique_ptr<IPlugin> createDopaminePlugin() {
        auto neuromodulator = std::make_unique<Dopamine>();
        return std::make_unique<NeuromodulatorPlugin>(std::move(neuromodulator));
    }
    
    std::unique_ptr<IPlugin> createCuriosityPlugin() {
        auto neuromodulator = std::make_unique<Curiosity>();
        return std::make_unique<NeuromodulatorPlugin>(std::move(neuromodulator));
    }
    
    std::unique_ptr<IPlugin> createNoveltyPlugin() {
        auto neuromodulator = std::make_unique<Novelty>();
        return std::make_unique<NeuromodulatorPlugin>(std::move(neuromodulator));
    }
    
    std::unique_ptr<IPlugin> createAcetylcholinePlugin() {
        auto neuromodulator = std::make_unique<Acetylcholine>();
        return std::make_unique<NeuromodulatorPlugin>(std::move(neuromodulator));
    }
    
    std::unique_ptr<IPlugin> createNorepinephrinePlugin() {
        auto neuromodulator = std::make_unique<Norepinephrine>();
        return std::make_unique<NeuromodulatorPlugin>(std::move(neuromodulator));
    }
    
    std::unique_ptr<IPlugin> createSerotoninPlugin() {
        auto neuromodulator = std::make_unique<Serotonin>();
        return std::make_unique<NeuromodulatorPlugin>(std::move(neuromodulator));
    }
    
    std::unique_ptr<IPlugin> createPredictionErrorPlugin() {
        auto rewardFunction = std::make_unique<PredictionError>();
        return std::make_unique<RewardFunctionPlugin>(std::move(rewardFunction));
    }
    
    std::unique_ptr<IPlugin> createSparseCodingPlugin() {
        auto rewardFunction = std::make_unique<Reward>();
        return std::make_unique<RewardFunctionPlugin>(std::move(rewardFunction));
    }
    
    std::unique_ptr<IPlugin> createTemporalDifferencePlugin() {
        auto rewardFunction = std::make_unique<Reward>();
        return std::make_unique<RewardFunctionPlugin>(std::move(rewardFunction));
    }
    
    std::unique_ptr<IPlugin> createDevelopmentSystemPlugin() {
        auto model = std::make_unique<DevelopmentSystem>();
        return std::make_unique<DevelopmentModelPlugin>(std::move(model));
    }
    
    std::unique_ptr<IPlugin> createSynaptogenesisPlugin() {
        auto model = std::make_unique<Synaptogenesis>();
        return std::make_unique<DevelopmentModelPlugin>(std::move(model));
    }
    
    std::unique_ptr<IPlugin> createPruningPlugin() {
        auto model = std::make_unique<Pruning>();
        return std::make_unique<DevelopmentModelPlugin>(std::move(model));
    }
    
    std::unique_ptr<IPlugin> createMaturationPlugin() {
        auto model = std::make_unique<Maturation>();
        return std::make_unique<DevelopmentModelPlugin>(std::move(model));
    }
}

} // namespace nlm
