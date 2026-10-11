#include "DefaultConfig.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iomanip>
#include <sstream>

namespace nlm {

void DefaultConfig::applyPhase2Defaults(Config& config) {
    // Set default values for Phase 2
    config.set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config.set("simulation_timestep", 0.001, ConfigSource::Default);
    config.set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);  // Smaller for faster test
    config.set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config.set("connection_probability", 0.15f, ConfigSource::Default);

    // STDP parameters
    config.set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config.set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config.set("stdp_tau", 20.0f, ConfigSource::Default);

    // Structural plasticity parameters
    config.set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config.set("pruning_rate", 0.00001f, ConfigSource::Default);
}

void DefaultConfig::applyPhase6Defaults(Config& config) {
    // Enhanced defaults for Phase 6 - Final Integration
    config.set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config.set("simulation_timestep", 0.0001, ConfigSource::Default);  // Higher precision
    config.set("neuron_count", static_cast<int64_t>(10000), ConfigSource::Default);
    config.set("region_count", static_cast<int64_t>(4), ConfigSource::Default);
    config.set("connection_probability", 0.05f, ConfigSource::Default);

    // Enhanced memory system defaults
    config.set("memory.working_memory.capacity", static_cast<int64_t>(1000), ConfigSource::Default);
    config.set("memory.episodic.capacity", static_cast<int64_t>(10000), ConfigSource::Default);
    config.set("memory.associative.max_patterns", static_cast<int64_t>(5000), ConfigSource::Default);

    // Enhanced neuromodulation defaults
    config.set("neuromod.dopamine.scale", 1.0f, ConfigSource::Default);
    config.set("neuromod.curiosity.threshold", 0.1f, ConfigSource::Default);
    config.set("neuromod.novelty.sensitivity", 0.5f, ConfigSource::Default);

    // Enhanced prediction system defaults
    config.set("prediction.horizon", 10, ConfigSource::Default);
    config.set("prediction.confidence_threshold", 0.7f, ConfigSource::Default);

    // Enhanced development defaults
    config.set("development.critical_period", 1000, ConfigSource::Default);
    config.set("development.maturation_rate", 0.01f, ConfigSource::Default);

    // Enhanced plasticity defaults
    config.set("plasticity.stdp.learning_rate", 0.001f, ConfigSource::Default);
    config.set("plasticity.stdp.tau_plus", 20.0f, ConfigSource::Default);
    config.set("plasticity.stdp.tau_minus", 20.0f, ConfigSource::Default);
    config.set("plasticity.hebbian.learning_rate", 0.001f, ConfigSource::Default);
    config.set("plasticity.structural.enable", true, ConfigSource::Default);
    config.set("plasticity.structural.growth_rate", 0.00005f, ConfigSource::Default);
    config.set("plasticity.structural.pruning_rate", 0.00001f, ConfigSource::Default);

    // Enhanced environment defaults
    config.set("environment.ticks_per_step", 10, ConfigSource::Default);
    config.set("environment.reward_scale", 1.0f, ConfigSource::Default);
    config.set("environment.episode_length", 1000, ConfigSource::Default);
}

std::string DefaultConfig::getConfigSummary(const Config& config) {
    std::stringstream ss;
    ss << "=== Configuration Summary ===" << std::endl;

    // Core parameters
    ss << "Core:" << std::endl;
    ss << "  random_seed: " << config.getOr<int64_t>("random_seed", 42) << std::endl;
    ss << "  simulation_timestep: " << config.getOr<double>("simulation_timestep", 0.001) << "s" << std::endl;
    ss << "  neuron_count: " << config.getOr<int64_t>("neuron_count", 500) << std::endl;
    ss << "  region_count: " << config.getOr<int64_t>("region_count", 1) << std::endl;
    ss << "  connection_probability: " << config.getOr<float>("connection_probability", 0.15f) << std::endl;

    // Memory systems
    ss << "Memory:" << std::endl;
    ss << "  working_memory.capacity: " << config.getOr<int64_t>("memory.working_memory.capacity", 1000) << std::endl;
    ss << "  episodic.capacity: " << config.getOr<int64_t>("memory.episodic.capacity", 10000) << std::endl;

    // Neuromodulation
    ss << "Neuromodulation:" << std::endl;
    ss << "  dopamine.scale: " << config.getOr<float>("neuromod.dopamine.scale", 1.0f) << std::endl;
    ss << "  curiosity.threshold: " << config.getOr<float>("neuromod.curiosity.threshold", 0.1f) << std::endl;
    ss << "  novelty.sensitivity: " << config.getOr<float>("neuromod.novelty.sensitivity", 0.5f) << std::endl;

    // Development
    ss << "Development:" << std::endl;
    ss << "  critical_period: " << config.getOr<int64_t>("development.critical_period", 1000) << std::endl;

    // Plasticity
    ss << "Plasticity:" << std::endl;
    ss << "  stdp.learning_rate: " << config.getOr<float>("plasticity.stdp.learning_rate", 0.001f) << std::endl;
    ss << "  hebbian.learning_rate: " << config.getOr<float>("plasticity.hebbian.learning_rate", 0.001f) << std::endl;

    ss << "=========================" << std::endl;
    return ss.str();
}

} // namespace nlm
