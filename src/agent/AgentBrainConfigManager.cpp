#include "AgentBrainConfigManager.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

void AgentBrainConfigManager::setDefaultConfig(std::shared_ptr<Config> config) {
    if (!config) {
        NLM_LOG_ERROR("Cannot set config on null config pointer");
        return;
    }
    
    // Set default values for Phase 2
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Phase 6 defaults
    config->set("sensory_novelty_decay", DEFAULT_SENSORY_NOVELTY_DECAY, ConfigSource::Default);
    
    NLM_LOG_INFO("Default AgentBrain configuration applied");
}

bool AgentBrainConfigManager::validateAgentBrainConfig(const Config& config) {
    bool isValid = true;
    
    // Validate neuron count
    int64_t neuronCount = config.getOr<int64_t>("neuron_count", -1);
    if (neuronCount <= 0) {
        NLM_LOG_ERROR("Invalid neuron_count in AgentBrain config: " + std::to_string(neuronCount));
        isValid = false;
    }
    
    // Validate region count
    int64_t regionCount = config.getOr<int64_t>("region_count", -1);
    if (regionCount <= 0) {
        NLM_LOG_ERROR("Invalid region_count in AgentBrain config: " + std::to_string(regionCount));
        isValid = false;
    }
    
    // Validate connection probability
    float connectionProb = config.getOr<float>("connection_probability", -1.0f);
    if (connectionProb < 0.0f || connectionProb > 1.0f) {
        NLM_LOG_ERROR("Invalid connection_probability in AgentBrain config: " + std::to_string(connectionProb));
        isValid = false;
    }
    
    // Validate simulation timestep
    double timestep = config.getOr<double>("simulation_timestep", -1.0);
    if (timestep <= 0.0) {
        NLM_LOG_ERROR("Invalid simulation_timestep in AgentBrain config: " + std::to_string(timestep));
        isValid = false;
    }
    
    return isValid;
}

void AgentBrainConfigManager::applyConfigOverrides(std::shared_ptr<Config> config) {
    if (!config) {
        NLM_LOG_ERROR("Cannot apply overrides to null config pointer");
        return;
    }
    
    // Set AgentBrain-specific configuration
    config->set("sensory_novelty_decay", DEFAULT_SENSORY_NOVELTY_DECAY, ConfigSource::Runtime);
    
    NLM_LOG_INFO("AgentBrain configuration overrides applied");
}

std::string AgentBrainConfigManager::getConfigSummary(const Config& config) {
    std::ostringstream oss;
    oss << "AgentBrain Configuration:" << std::endl;
    
    oss << "  random_seed: " << config.getOr<int64_t>("random_seed", 42) << std::endl;
    oss << "  simulation_timestep: " << config.getOr<double>("simulation_timestep", 0.001) << "s" << std::endl;
    oss << "  neuron_count: " << config.getOr<int64_t>("neuron_count", 500) << std::endl;
    oss << "  region_count: " << config.getOr<int64_t>("region_count", 1) << std::endl;
    oss << "  connection_probability: " << config.getOr<float>("connection_probability", 0.15f) << std::endl;
    oss << "  stdp_ltp_weight: " << config.getOr<float>("stdp_ltp_weight", 0.02f) << std::endl;
    oss << "  stdp_ltd_weight: " << config.getOr<float>("stdp_ltd_weight", 0.015f) << std::endl;
    oss << "  stdp_tau: " << config.getOr<float>("stdp_tau", 20.0f) << std::endl;
    oss << "  synaptogenesis_rate: " << config.getOr<float>("synaptogenesis_rate", 0.0001f) << std::endl;
    oss << "  pruning_rate: " << config.getOr<float>("pruning_rate", 0.00001f) << std::endl;
    
    return oss.str();
}

} // namespace nlm