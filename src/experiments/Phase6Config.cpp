#include "Phase6Config.hpp"

namespace nlm {

Phase6Config::Phase6Config() {
    // Set default Phase 6 values
    setDefaultPhase6Values();
}

void Phase6Config::setDefaultPhase6Values() {
    // Set all default values for Phase 6
    set("max_steps", 10000, ConfigSource::Default);
    set("neuron_count", 1000, ConfigSource::Default);
    set("region_count", 1, ConfigSource::Default);
    set("connection_probability", 0.1f, ConfigSource::Default);
    set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    set("stdp_tau", 20.0f, ConfigSource::Default);
    set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    set("pruning_rate", 0.00001f, ConfigSource::Default);
    set("enable_checkpointing", true, ConfigSource::Default);
    set("enable_replay", true, ConfigSource::Default);
    set("enable_development", true, ConfigSource::Default);
    set("checkpoint_path", "./checkpoint_test.bin", ConfigSource::Default);
    set("checkpoint_interval", 10000, ConfigSource::Default);
    set("max_checkpoints", 5, ConfigSource::Default);
    set("enable_auto_cleanup", true, ConfigSource::Default);
    set("memory_capacity_multiplier", 0.1f, ConfigSource::Default);
    set("max_episodes", 1000, ConfigSource::Default);
    set("dopamine_baseline", 0.5f, ConfigSource::Default);
    set("curiosity_baseline", 0.5f, ConfigSource::Default);
    set("development_speed", 1.0f, ConfigSource::Default);
    set("maturation_rate", 0.01f, ConfigSource::Default);
    set("planning_depth", 5, ConfigSource::Default);
    set("attention_inhibition_strength", 0.5f, ConfigSource::Default);
    set("attention_excitation_strength", 1.5f, ConfigSource::Default);
    set("replay_interval", 100, ConfigSource::Default);
    set("consolidation_interval", 1000, ConfigSource::Default);
}

} // namespace nlm