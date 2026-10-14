#include "PlasticityRuleFactory.hpp"
#include <stdexcept>

namespace nlm {

struct BuiltInPlasticityRuleFactory::Impl {
    // Registry of rule creation functions
    std::unordered_map<std::string, std::function<std::unique_ptr<PlasticityRule>(const std::unordered_map<std::string, std::any>&)>> ruleCreators;
    
    Impl() {
        // Register built-in rule creation functions
        ruleCreators["STDP"] = [this](const auto& config) {
            float ltpWeight = config.count("ltpWeight") ? 
                std::any_cast<float>(config.at("ltpWeight")) : 0.01f;
            float ltdWeight = config.count("ltdWeight") ? 
                std::any_cast<float>(config.at("ltdWeight")) : 0.012f;
            float timeConstant = config.count("timeConstant") ? 
                std::any_cast<float>(config.at("timeConstant")) : 20.0f;
            return createSTDP(ltpWeight, ltdWeight, timeConstant);
        };
        
        ruleCreators["Hebbian"] = [this](const auto& config) {
            float learningRate = config.count("learningRate") ? 
                std::any_cast<float>(config.at("learningRate")) : 0.01f;
            float maxWeight = config.count("maxWeight") ? 
                std::any_cast<float>(config.at("maxWeight")) : 1.0f;
            float minWeight = config.count("minWeight") ? 
                std::any_cast<float>(config.at("minWeight")) : -1.0f;
            return createHebbian(learningRate, maxWeight, minWeight);
        };
        
        ruleCreators["AntiHebbian"] = [this](const auto& config) {
            return createAntiHebbian();
        };
        
        ruleCreators["BCM"] = [this](const auto& config) {
            float bcmThreshold = config.count("bcmThreshold") ? 
                std::any_cast<float>(config.at("bcmThreshold")) : 0.0f;
            return createBCM(bcmThreshold);
        };
    }
};

BuiltInPlasticityRuleFactory::BuiltInPlasticityRuleFactory() : pImpl(std::make_unique<Impl>()) {}
BuiltInPlasticityRuleFactory::~BuiltInPlasticityRuleFactory() = default;

std::unique_ptr<PlasticityRule> BuiltInPlasticityRuleFactory::createFromConfig(
    const std::string& ruleType,
    const std::unordered_map<std::string, std::any>& config) {
    
    auto it = pImpl->ruleCreators.find(ruleType);
    if (it == pImpl->ruleCreators.end()) {
        throw std::invalid_argument("Unknown rule type: " + ruleType);
    }
    
    return it->second(config);
}

const char* BuiltInPlasticityRuleFactory::getName() const {
    return "BuiltInPlasticityRuleFactory";
}

bool BuiltInPlasticityRuleFactory::canCreate(const std::string& ruleType) const {
    return pImpl->ruleCreators.find(ruleType) != pImpl->ruleCreators.end();
}

std::unique_ptr<STDP> BuiltInPlasticityRuleFactory::createSTDP(float ltpWeight, float ltdWeight,
                                                                float timeConstant) {
    auto stdp = std::make_unique<STDP>();
    stdp->configure(ltpWeight, ltdWeight, timeConstant);
    return stdp;
}

std::unique_ptr<Hebbian> BuiltInPlasticityRuleFactory::createHebbian(float learningRate,
                                                                    float maxWeight,
                                                                    float minWeight) {
    auto hebbian = std::make_unique<Hebbian>();
    hebbian->setLearningRate(learningRate);
    hebbian->setMaxWeight(maxWeight);
    // Note: The Hebbian class doesn't expose a setMinWeight method,
    // but minWeight is used internally in the implementation
    return hebbian;
}

std::unique_ptr<AntiHebbianRule> BuiltInPlasticityRuleFactory::createAntiHebbian() {
    return std::make_unique<AntiHebbianRule>();
}

std::unique_ptr<BCMRule> BuiltInPlasticityRuleFactory::createBCM(float bcmThreshold) {
    auto bcm = std::make_unique<BCMRule>();
    // Note: BCM rule doesn't have a threshold setter - this is a limitation
    return bcm;
}

struct PlasticityRuleRegistry::Impl {
    std::unordered_map<std::string, std::unique_ptr<IPlasticityRuleFactory>> factories;
    std::unordered_map<std::string, std::unique_ptr<BuiltInPlasticityRuleFactory>> builtInFactories;
};

PlasticityRuleRegistry& PlasticityRuleRegistry::getInstance() {
    static PlasticityRuleRegistry instance;
    return instance;
}

void PlasticityRuleRegistry::registerRuleType(const std::string& ruleType,
                                             std::unique_ptr<IPlasticityRuleFactory> factory) {
    pImpl->factories[ruleType] = std::move(factory);
}

bool PlasticityRuleRegistry::isRegistered(const std::string& ruleType) const {
    return pImpl->factories.find(ruleType) != pImpl->factories.end();
}

IPlasticityRuleFactory* PlasticityRuleRegistry::getFactory(const std::string& ruleType) const {
    auto it = pImpl->factories.find(ruleType);
    return it != pImpl->factories.end() ? it->second.get() : nullptr;
}

std::vector<std::string> PlasticityRuleRegistry::getRegisteredRuleTypes() const {
    std::vector<std::string> types;
    types.reserve(pImpl->factories.size());
    for (const auto& pair : pImpl->factories) {
        types.push_back(pair.first);
    }
    return types;
}

std::unique_ptr<PlasticityRule> PlasticityRuleRegistry::createRule(
    const std::string& ruleType,
    const RuleConfiguration<PlasticityRule>& config) {
    
    IPlasticityRuleFactory* factory = getFactory(ruleType);
    if (!factory) {
        throw std::invalid_argument("Unknown rule type: " + ruleType);
    }
    
    // Convert RuleConfiguration to map
    std::unordered_map<std::string, std::any> configMap;
    for (const auto& pair : config.getAll()) {
        configMap[pair.first] = pair.second;
    }
    
    // Validate configuration
    std::unique_ptr<PlasticityRule> rule = factory->createFromConfig(ruleType, configMap);
    
    return rule;
}

std::unique_ptr<PlasticityRule> PlasticityRuleRegistry::createRule(
    const std::string& ruleType,
    const std::unordered_map<std::string, std::string>& config) {
    
    RuleConfiguration<PlasticityRule> ruleConfig;
    for (const auto& pair : config) {
        // Try to convert string to appropriate type based on value
        const std::string& value = pair.second;
        if (value.find('.') != std::string::npos) {
            // Try double
            try {
                double doubleValue = std::stod(value);
                ruleConfig.add<double>(pair.first, doubleValue);
            } catch (...) {
                // Fall back to string
                ruleConfig.add<std::string>(pair.first, value);
            }
        } else {
            // Try integer
            try {
                int intValue = std::stoi(value);
                ruleConfig.add<int>(pair.first, intValue);
            } catch (...) {
                // Fall back to string
                ruleConfig.add<std::string>(pair.first, value);
            }
        }
    }
    
    return createRule(ruleType, ruleConfig);
}

void PlasticityRuleRegistry::clear() {
    pImpl->factories.clear();
    pImpl->builtInFactories.clear();
}

} // namespace nlm
