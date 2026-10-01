#include "PlasticityRule.hpp"

namespace nlm {

bool PlasticityRule::isEnabled() const {
    return enabled_;
}

void PlasticityRule::setEnabled(bool enabled) {
    enabled_ = enabled;
}

// Factory implementation
std::shared_ptr<PlasticityRule> PlasticityRuleFactory::createSTDP() {
    return std::make_shared<STDP>();
}

std::shared_ptr<PlasticityRule> PlasticityRuleFactory::createHebbian() {
    return std::make_shared<HebbianRule>();
}

std::shared_ptr<PlasticityRule> PlasticityRuleFactory::createBCM() {
    return std::make_shared<BCMRule>();
}

std::shared_ptr<PlasticityRule> PlasticityRuleFactory::createAntiHebbian() {
    return std::make_shared<AntiHebbianRule>();
}

std::shared_ptr<PlasticityRule> PlasticityRuleFactory::createOja() {
    return std::make_shared<OjaRule>();
}

std::shared_ptr<PlasticityRule> PlasticityRuleFactory::createBMU() {
    return std::make_shared<BMURule>();
}

std::vector<std::string> PlasticityRuleFactory::getAvailableRuleNames() {
    return {"STDP", "Hebbian", "BCM", "AntiHebbian", "Oja", "BMU"};
}

std::shared_ptr<PlasticityRule> PlasticityRuleFactory::createFromName(const std::string& name) {
    if (name == "STDP") return createSTDP();
    if (name == "Hebbian") return createHebbian();
    if (name == "BCM") return createBCM();
    if (name == "AntiHebbian") return createAntiHebbian();
    if (name == "Oja") return createOja();
    if (name == "BMU") return createBMU();
    return nullptr;
}

} // namespace nlm