#include "factories/PlasticityRuleFactory.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct PlasticityRuleFactory::Impl {
    size_t creationCount;
    std::string lastCreatedType;
};

PlasticityRuleFactory::PlasticityRuleFactory() : creationCount_(0) {
    pImpl_ = std::make_unique<Impl>();
    pImpl_->creationCount = 0;
    pImpl_->lastCreatedType = "None";
}

PlasticityRuleFactory::~PlasticityRuleFactory() = default;

std::unique_ptr<AbstractPlasticityRule> PlasticityRuleFactory::createPlasticityRule(
    PlasticityRuleType type, 
    std::shared_ptr<Config> config
) {
    auto rule = createFromConfig(type, config);
    ++pImpl_->creationCount;
    ++creationCount_;
    return rule;
}

std::unique_ptr<AbstractPlasticityRule> PlasticityRuleFactory::createPlasticityRule(
    const std::string& ruleName,
    std::shared_ptr<Config> config
) {
    PlasticityRuleType type;
    
    // Map string names to types
    if (ruleName == "STDP") type = PlasticityRuleType::STDP;
    else if (ruleName == "Hebbian") type = PlasticityRuleType::Hebbian;
    else if (ruleName == "RewardModulatedSTDP") type = PlasticityRuleType::RewardModulatedSTDP;
    else if (ruleName == "StructuralPlasticity") type = PlasticityRuleType::StructuralPlasticity;
    else type = PlasticityRuleType::Custom;
    
    return createPlasticityRule(type, config);
}

std::unique_ptr<AbstractPlasticityRule> PlasticityRuleFactory::createFromConfig(
    const Config& config
) {
    // Get rule type from config
    std::string ruleType = config.get_or<std::string>("plasticity_rule_type", "STDP");
    
    // Map type name to enum
    PlasticityRuleType type;
    if (ruleType == "STDP") type = PlasticityRuleType::STDP;
    else if (ruleType == "Hebbian") type = PlasticityRuleType::Hebbian;
    else if (ruleType == "RewardModulatedSTDP") type = PlasticityRuleType::RewardModulatedSTDP;
    else if (ruleType == "StructuralPlasticity") type = PlasticityRuleType::StructuralPlasticity;
    else type = PlasticityRuleType::STDP; // Default to STDP
    
    return createPlasticityRule(type, std::make_shared<Config>(config));
}

std::unique_ptr<AbstractPlasticityRule> PlasticityRuleFactory::createFromPlugin(
    const std::string& pluginName,
    std::shared_ptr<Config> config
) {
    auto it = plugins_.find(pluginName);
    if (it != plugins_.end()) {
        auto rule = it->second(config);
        ++pImpl_->creationCount;
        ++creationCount_;
        return rule;
    }
    
    NLM_LOG_ERROR("Plugin not found: " + pluginName);
    return nullptr;
}

void PlasticityRuleFactory::configureFromConfig(AbstractPlasticityRule* rule, const Config& config) {
    if (!rule) return;
    
    // Configure based on rule type (this would need RTTI or visitor pattern in real implementation)
    // For now, we'll just log the configuration
    NLM_LOG_INFO("Configuring plasticity rule from config");
}

void PlasticityRuleFactory::configureFromConfig(STDP* stdp, const Config& config) {
    if (!stdp) return;
    
    float ltpWeight = config.get_or<float>("stdp_ltp_weight", 0.01f);
    float ltdWeight = config.get_or<float>("stdp_ltd_weight", 0.012f);
    float tau = config.get_or<float>("stdp_tau", 20.0f);
    
    stdp->configure(ltpWeight, ltdWeight, tau);
}

void PlasticityRuleFactory::configureFromConfig(Hebbian* hebbian, const Config& config) {
    if (!hebbian) return;
    
    float learningRate = config.get_or<float>("hebbian_learning_rate", 0.01f);
    hebbian->setLearningRate(learningRate);
}

void PlasticityRuleFactory::configureFromConfig(RewardModulatedSTDP* rewardStdp, const Config& config) {
    if (!rewardStdp) return;
    
    // Configure reward-modulated STDP parameters
    float rewardGain = config.get_or<float>("reward_gain", 1.0f);
    float noveltyThreshold = config.get_or<float>("novelty_threshold", 0.5f);
    
    // These would need to be exposed in the RewardModulatedSTDP interface
    // rewardStdp->setRewardGain(rewardGain);
    // rewardStdp->setNoveltyThreshold(noveltyThreshold);
}

void PlasticityRuleFactory::configureFromConfig(StructuralPlasticity* structural, const Config& config) {
    if (!structural) return;
    
    float synaptogenesisRate = config.get_or<float>("synaptogenesis_rate", 0.0001f);
    float pruningRate = config.get_or<float>("pruning_rate", 0.00001f);
    
    structural->setSynaptogenesisRate(synaptogenesisRate);
    structural->setPruningRate(pruningRate);
}

bool PlasticityRuleFactory::registerPlugin(
    const std::string& name, 
    std::function<std::unique_ptr<AbstractPlasticityRule>(std::shared_ptr<Config>)> factory
) {
    plugins_[name] = factory;
    NLM_LOG_INFO("Registered plasticity rule plugin: " + name);
    return true;
}

std::vector<std::string> PlasticityRuleFactory::getRegisteredPlugins() const {
    std::vector<std::string> pluginNames;
    for (const auto& pair : plugins_) {
        pluginNames.push_back(pair.first);
    }
    return pluginNames;
}

} // namespace nlm
