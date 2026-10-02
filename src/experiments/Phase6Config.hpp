#pragma once

#include "../core/Config/Config.hpp"
#include <string>

namespace nlm {

/**
 * Unified Phase 6 Configuration System
 * 
 * This configuration class extends the base nlm::Config with
 * Phase 6-specific parameters while maintaining full backward
 * compatibility with the existing configuration system.
 * 
 * The unified approach eliminates configuration duplication
 * between nlm::Config and Phase6Config, providing a single
 * source of truth for all configuration needs.
 */
class Phase6Config : public Config {
public:
    Phase6Config() {
        // Set default Phase 6 values
        setDefaultPhase6Values();
    }
    
    // ========== PHASE 6 SPECIFIC PARAMETERS ==========
    
    // Core simulation parameters
    void setMaxSteps(uint64_t steps) {
        set("max_steps", steps, ConfigSource::Runtime);
    }
    
    uint64_t getMaxSteps() const {
        return getOr<uint64_t>("max_steps", 10000);
    }
    
    void setNeuronCount(size_t count) {
        set("neuron_count", count, ConfigSource::Runtime);
    }
    
    size_t getNeuronCount() const {
        return getOr<size_t>("neuron_count", 1000);
    }
    
    void setRegionCount(size_t count) {
        set("region_count", count, ConfigSource::Runtime);
    }
    
    size_t getRegionCount() const {
        return getOr<size_t>("region_count", 1);
    }
    
    void setConnectionProbability(float prob) {
        set("connection_probability", prob, ConfigSource::Runtime);
    }
    
    float getConnectionProbability() const {
        return getOr<float>("connection_probability", 0.1f);
    }
    
    // STDP parameters
    void setStdpLTPWeight(float weight) {
        set("stdp_ltp_weight", weight, ConfigSource::Runtime);
    }
    
    float getStdpLTPWeight() const {
        return getOr<float>("stdp_ltp_weight", 0.01f);
    }
    
    void setStdpLTDWeight(float weight) {
        set("stdp_ltd_weight", weight, ConfigSource::Runtime);
    }
    
    float getStdpLTDWeight() const {
        return getOr<float>("stdp_ltd_weight", 0.012f);
    }
    
    void setStdpTau(float tau) {
        set("stdp_tau", tau, ConfigSource::Runtime);
    }
    
    float getStdpTau() const {
        return getOr<float>("stdp_tau", 20.0f);
    }
    
    // Structural plasticity parameters
    void setSynaptogenesisRate(float rate) {
        set("synaptogenesis_rate", rate, ConfigSource::Runtime);
    }
    
    float getSynaptogenesisRate() const {
        return getOr<float>("synaptogenesis_rate", 0.0001f);
    }
    
    void setPruningRate(float rate) {
        set("pruning_rate", rate, ConfigSource::Runtime);
    }
    
    float getPruningRate() const {
        return getOr<float>("pruning_rate", 0.00001f);
    }
    
    // Experiment control parameters
    void setEnableCheckpointing(bool enable) {
        set("enable_checkpointing", enable, ConfigSource::Runtime);
    }
    
    bool getEnableCheckpointing() const {
        return getOr<bool>("enable_checkpointing", true);
    }
    
    void setEnableReplay(bool enable) {
        set("enable_replay", enable, ConfigSource::Runtime);
    }
    
    bool getEnableReplay() const {
        return getOr<bool>("enable_replay", true);
    }
    
    void setEnableDevelopment(bool enable) {
        set("enable_development", enable, ConfigSource::Runtime);
    }
    
    bool getEnableDevelopment() const {
        return getOr<bool>("enable_development", true);
    }
    
    // Checkpoint parameters
    void setCheckpointPath(const std::string& path) {
        set("checkpoint_path", path, ConfigSource::Runtime);
    }
    
    std::string getCheckpointPath() const {
        return getOr<std::string>("checkpoint_path", "./checkpoint_test.bin");
    }
    
    void setCheckpointInterval(size_t interval) {
        set("checkpoint_interval", interval, ConfigSource::Runtime);
    }
    
    size_t getCheckpointInterval() const {
        return getOr<size_t>("checkpoint_interval", 10000);
    }
    
    void setMaxCheckpoints(size_t maxCheckpoints) {
        set("max_checkpoints", maxCheckpoints, ConfigSource::Runtime);
    }
    
    size_t getMaxCheckpoints() const {
        return getOr<size_t>("max_checkpoints", 5);
    }
    
    void setEnableAutoCleanup(bool enable) {
        set("enable_auto_cleanup", enable, ConfigSource::Runtime);
    }
    
    bool getEnableAutoCleanup() const {
        return getOr<bool>("enable_auto_cleanup", true);
    }
    
    // Memory system parameters
    void setMemoryCapacityMultiplier(float multiplier) {
        set("memory_capacity_multiplier", multiplier, ConfigSource::Runtime);
    }
    
    float getMemoryCapacityMultiplier() const {
        return getOr<float>("memory_capacity_multiplier", 0.1f);
    }
    
    void setMaxEpisodes(size_t maxEpisodes) {
        set("max_episodes", maxEpisodes, ConfigSource::Runtime);
    }
    
    size_t getMaxEpisodes() const {
        return getOr<size_t>("max_episodes", 1000);
    }
    
    // Neuromodulation parameters
    void setDopamineBaseline(float baseline) {
        set("dopamine_baseline", baseline, ConfigSource::Runtime);
    }
    
    float getDopamineBaseline() const {
        return getOr<float>("dopamine_baseline", 0.5f);
    }
    
    void setCuriosityBaseline(float baseline) {
        set("curiosity_baseline", baseline, ConfigSource::Runtime);
    }
    
    float getCuriosityBaseline() const {
        return getOr<float>("curiosity_baseline", 0.5f);
    }
    
    // Development parameters
    void setDevelopmentSpeed(float speed) {
        set("development_speed", speed, ConfigSource::Runtime);
    }
    
    float getDevelopmentSpeed() const {
        return getOr<float>("development_speed", 1.0f);
    }
    
    void setMaturationRate(float rate) {
        set("maturation_rate", rate, ConfigSource::Runtime);
    }
    
    float getMaturationRate() const {
        return getOr<float>("maturation_rate", 0.01f);
    }
    
    // Cognitive parameters
    void setPlanningDepth(size_t depth) {
        set("planning_depth", depth, ConfigSource::Runtime);
    }
    
    size_t getPlanningDepth() const {
        return getOr<size_t>("planning_depth", 5);
    }
    
    void setAttentionInhibitionStrength(float strength) {
        set("attention_inhibition_strength", strength, ConfigSource::Runtime);
    }
    
    float getAttentionInhibitionStrength() const {
        return getOr<float>("attention_inhibition_strength", 0.5f);
    }
    
    void setAttentionExcitationStrength(float strength) {
        set("attention_excitation_strength", strength, ConfigSource::Runtime);
    }
    
    float getAttentionExcitationStrength() const {
        return getOr<float>("attention_excitation_strength", 1.5f);
    }
    
    // Replay and consolidation parameters
    void setReplayInterval(size_t interval) {
        set("replay_interval", interval, ConfigSource::Runtime);
    }
    
    size_t getReplayInterval() const {
        return getOr<size_t>("replay_interval", 100);
    }
    
    void setConsolidationInterval(size_t interval) {
        set("consolidation_interval", interval, ConfigSource::Runtime);
    }
    
    size_t getConsolidationInterval() const {
        return getOr<size_t>("consolidation_interval", 1000);
    }
    
    // Convenience methods for common configuration patterns
    
    /**
     * Configure for quick testing
     */
    void configureForQuickTesting() {
        setMaxSteps(1000);
        setNeuronCount(200);
        setRegionCount(1);
        setConnectionProbability(0.1f);
        setEnableCheckpointing(true);
        setEnableReplay(true);
        setEnableDevelopment(true);
        setCheckpointPath("./quick_test_checkpoint.bin");
    }
    
    /**
     * Configure for full integration testing
     */
    void configureForFullIntegration() {
        setMaxSteps(50000);
        setNeuronCount(2000);
        setRegionCount(2);
        setConnectionProbability(0.15f);
        setEnableCheckpointing(true);
        setEnableReplay(true);
        setEnableDevelopment(true);
        setCheckpointPath("./integration_checkpoint.bin");
        setMaxCheckpoints(10);
        setMemoryCapacityMultiplier(0.2f);
        setMaxEpisodes(5000);
    }
    
    /**
     * Configure for real-time simulation
     */
    void configureForRealTime() {
        setMaxSteps(10000000);  // 10 million steps
        setNeuronCount(500);
        setRegionCount(1);
        setConnectionProbability(0.05f);
        setEnableCheckpointing(true);
        setEnableReplay(true);
        setEnableDevelopment(true);
        setCheckpointInterval(100000);
        setMaxCheckpoints(20);
        setEnableAutoCleanup(true);
        setReplayInterval(1000);
        setConsolidationInterval(10000);
    }
    
    /**
     * Print configuration summary
     */
    void printConfiguration() const {
        std::cout << "=== Phase 6 Unified Configuration ===" << std::endl;
        std::cout << "Max steps: " << getMaxSteps() << std::endl;
        std::cout << "Neuron count: " << getNeuronCount() << std::endl;
        std::cout << "Region count: " << getRegionCount() << std::endl;
        std::cout << "Connection probability: " << getConnectionProbability() << std::endl;
        std::cout << "Enable checkpointing: " << (getEnableCheckpointing() ? "Yes" : "No") << std::endl;
        std::cout << "Enable replay: " << (getEnableReplay() ? "Yes" : "No") << std::endl;
        std::cout << "Enable development: " << (getEnableDevelopment() ? "Yes" : "No") << std::endl;
        std::cout << "Checkpoint path: " << getCheckpointPath() << std::endl;
        std::cout << "STDP LTP weight: " << getStdpLTPWeight() << std::endl;
        std::cout << "STDP LTD weight: " << getStdpLTDWeight() << std::endl;
        std::cout << "STDP tau: " << getStdpTau() << std::endl;
        std::cout << "Synaptogenesis rate: " << getSynaptogenesisRate() << std::endl;
        std::cout << "Pruning rate: " << getPruningRate() << std::endl;
        std::cout << "================================" << std::endl;
    }
    
private:
    void setDefaultPhase6Values() {
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
};

} // namespace nlm