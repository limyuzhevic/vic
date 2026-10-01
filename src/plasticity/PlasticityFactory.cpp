#include "PlasticityFactory.hpp"
#include "PlasticityRule.hpp"
#include "Hebbian.hpp"
#include "STDP.hpp"
#include "RewardModulatedSTDP.hpp"
#include "TripletSTDP.hpp"
#include "BCMRule.hpp"
#include "../../core/Logger/Logger.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace nlm {

struct PlasticityFactory::Impl {
    // Registry of plasticity rule factories
    std::unordered_map<std::string, std::function<std::unique_ptr<PlasticityRule>()>> ruleFactories;
    
    Impl() {
        // Register default rule factories
        ruleFactories["Hebbian"] = []() { return std::make_unique<Hebbian>(); };
        ruleFactories["STDP"] = []() { return std::make_unique<STDP>(); };
        ruleFactories["R-STDP"] = []() { return std::make_unique<RewardModulatedSTDP>(); };
        ruleFactories["TripletSTDP"] = []() { return std::make_unique<TripletSTDP>(); };
        ruleFactories["BCM"] = []() { return std::make_unique<BCMRule>(); };
        
        // Register placeholder rules
        ruleFactories["AntiHebbian"] = []() { return std::make_unique<AntiHebbianRule>(); };
    }
};

PlasticityFactory::PlasticityFactory() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("PlasticityFactory created");
}

PlasticityFactory::~PlasticityFactory() = default;

bool PlasticityFactory::registerRule(const std::string& name, 
                                     std::function<std::unique_ptr<PlasticityRule>()> factory) {
    if (name.empty() || !factory) {
        return false;
    }
    
    pImpl->ruleFactories[name] = std::move(factory);
    NLM_LOG_INFO("Registered plasticity rule: " + name);
    return true;
}

std::unique_ptr<PlasticityRule> PlasticityFactory::createRule(const std::string& name) {
    auto it = pImpl->ruleFactories.find(name);
    if (it == pImpl->ruleFactories.end()) {
        NLM_LOG_WARNING("Unknown plasticity rule: " + name);
        return nullptr;
    }
    
    try {
        auto rule = it->second();
        NLM_LOG_INFO("Created plasticity rule: " + name);
        return rule;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to create plasticity rule " + name + ": " + e.what());
        return nullptr;
    }
}

bool PlasticityFactory::hasRule(const std::string& name) const {
    return pImpl->ruleFactories.find(name) != pImpl->ruleFactories.end();
}

std::vector<std::string> PlasticityFactory::getAvailableRules() const {
    std::vector<std::string> rules;
    rules.reserve(pImpl->ruleFactories.size());
    for (const auto& pair : pImpl->ruleFactories) {
        rules.push_back(pair.first);
    }
    return rules;
}

void PlasticityFactory::clear() {
    pImpl->ruleFactories.clear();
    NLM_LOG_INFO("PlasticityFactory cleared");
}

std::string PlasticityFactory::getRuleDescription(const std::string& name) const {
    if (name == "Hebbian") {
        return "Hebbian plasticity rule: \"neurons that fire together, wire together\" based on spike correlation.";
    } else if (name == "STDP") {
        return "Spike-Timing-Dependent Plasticity: weight changes based on precise timing of pre- and post-synaptic spikes.";
    } else if (name == "R-STDP") {
        return "Reward-Modulated STDP: STDP plasticity gated by reward prediction error signals.";
    } else if (name == "TripletSTDP") {
        return "Triplet STDP: extended STDP considering recent spike triplets for more nuanced learning.";
    } else if (name == "BCM") {
        return "Bienenstock-Cooper-Munro rule: sliding threshold learning providing stability and flexibility.";
    } else if (name == "AntiHebbian") {
        return "Anti-Hebbian rule: decreases weight when neurons fire together (opponent learning).";
    }
    return "Unknown plasticity rule.";
}

} // namespace nlm