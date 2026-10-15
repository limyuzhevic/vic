#include "AdvancedPlasticityPlugin.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

AdvancedPlasticityPlugin::AdvancedPlasticityPlugin()
    : brain_(nullptr)
    , config_(nullptr)
    , stdpLTPWeight_(0.01f)
    , stdpLTDWeight_(0.012f)
    , hebbianEnabled_(true)
    , learningRate_(0.001f)
{
}

AdvancedPlasticityPlugin::~AdvancedPlasticityPlugin() = default;

std::string AdvancedPlasticityPlugin::getName() const {
    return "AdvancedPlasticityPlugin";
}

std::string AdvancedPlasticityPlugin::getVersion() const {
    return "1.0.0";
}

std::string AdvancedPlasticityPlugin::getDescription() const {
    return "Advanced plasticity rules plugin for NLM brain";
}

std::string AdvancedPlasticityPlugin::getType() const {
    return "plasticity";
}

bool AdvancedPlasticityPlugin::initialize(Brain* brain, std::shared_ptr<Config> config) {
    if (!brain || !config) {
        return false;
    }
    
    brain_ = brain;
    config_ = config;
    
    // Get advanced plasticity parameters from config
    stdpLTPWeight_ = config_->getOr<float>("plugin.plasticity.stdp_ltp_weight", 0.01f);
    stdpLTDWeight_ = config_->getOr<float>("plugin.plasticity.stdp_ltd_weight", 0.012f);
    hebbianEnabled_ = config_->getOr<bool>("plugin.plasticity.hebbian_enabled", true);
    learningRate_ = config_->getOr<float>("plugin.plasticity.learning_rate", 0.001f);
    
    // Apply advanced plasticity rules
    if (auto* stdp = brain_->getSTDP()) {
        stdp->setLTPWeight(stdpLTPWeight_);
        stdp->setLTDWeight(stdpLTDWeight_);
        NLM_LOG_INFO("AdvancedPlasticityPlugin configured STDP weights: LTP=" + std::to_string(stdpLTPWeight_) + 
                     ", LTD=" + std::to_string(stdpLTDWeight_));
    }
    
    NLM_LOG_INFO("AdvancedPlasticityPlugin initialized successfully");
    NLM_LOG_INFO("  STDP weights: LTP=" + std::to_string(stdpLTPWeight_) + 
                 ", LTD=" + std::to_string(stdpLTDWeight_));
    NLM_LOG_INFO("  Hebbian enabled: " + std::to_string(hebbianEnabled_));
    NLM_LOG_INFO("  Learning rate: " + std::to_string(learningRate_));
    
    return true;
}

void AdvancedPlasticityPlugin::update(double timestep) {
    if (!brain_ || !config_) {
        return;
    }
    
    // Adaptive learning rate based on neural activity
    if (auto* stdp = brain_->getSTDP()) {
        size_t totalSpikes = brain_->getTotalSpikeCount();
        float activityFactor = std::min(1.0f, static_cast<float>(totalSpikes) / 1000.0f);
        
        // Adjust learning rate based on activity level
        float adaptiveLearningRate = learningRate_ * (1.0f + activityFactor * 0.5f);
        
        NLM_LOG_INFO("AdvancedPlasticityPlugin adaptive learning rate: " + std::to_string(adaptiveLearningRate));
    }
}

void AdvancedPlasticityPlugin::shutdown() {
    NLM_LOG_INFO("AdvancedPlasticityPlugin shutting down");
    brain_ = nullptr;
    config_ = nullptr;
}

void AdvancedPlasticityPlugin::setSTDPWeight(float ltp, float ltd) {
    if (ltp > 0.0f && ltd > 0.0f) {
        stdpLTPWeight_ = ltp;
        stdpLTDWeight_ = ltd;
        
        if (auto* stdp = brain_->getSTDP()) {
            stdp->setLTPWeight(ltp);
            stdp->setLTDWeight(ltd);
            NLM_LOG_INFO("AdvancedPlasticityPlugin STDP weights updated: LTP=" + std::to_string(ltp) + 
                         ", LTD=" + std::to_string(ltd));
        }
    }
}

void AdvancedPlasticityPlugin::enableHebbian(bool enable) {
    hebbianEnabled_ = enable;
    NLM_LOG_INFO("AdvancedPlasticityPlugin Hebbian " + std::string(enable ? "enabled" : "disabled"));
}

void AdvancedPlasticityPlugin::setLearningRate(float rate) {
    if (rate > 0.0f) {
        learningRate_ = rate;
        NLM_LOG_INFO("AdvancedPlasticityPlugin learning rate set to: " + std::to_string(rate));
    }
}

}} // namespace nlm