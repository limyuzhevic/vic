#ifndef PHASE_CONFIG_H
#define PHASE_CONFIG_H

#include "core/Config/NLMConfigUtils.hpp"
#include "core/Config/Config.hpp"
#include <memory>

namespace nlm {

/**
 * Centralized configuration for all NLM experiment phases
 * Eliminates duplication between experiment files
 */
class PhaseConfig {
public:
    static std::shared_ptr<Config> createConfig(int phase) {
        auto config = std::make_shared<Config>();
        
        switch (phase) {
            case 2:
                applyPhase2Config(config);
                break;
            case 3:
                NLMConfigUtils::applyPhase3Config(config);
                break;
            case 4:
                NLMConfigUtils::applyPhase4Config(config);
                break;
            case 5:
                applyPhase5Config(config);
                break;
            case 6:
                NLMConfigUtils::applyPhase6Config(config);
                break;
            default:
                NLMConfigUtils::applyDefaultConfig(config);
                break;
        }
        
        return config;
    }
    
    /**
     * Standard configuration for NLM experiments
     * Includes all common parameters used across phases
     */
    static void applyPhase2Config(std::shared_ptr<Config> config) {
        if (!config) return;
        
        NLMConfigUtils::applyDefaultConfig(config);
        
        // Phase 2 specific overrides
        config->set("neuron_count", 500LL);
        config->set("region_count", 1LL);
        config->set("connection_probability", 0.15f);
        
        // Phase 2 specific plasticity settings
        config->set("stdp_ltp_weight", 0.02f);
        config->set("stdp_ltd_weight", 0.015f);
        config->set("stdp_tau", 20.0f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
        
        // Phase 2 experimental settings
        config->set("experiment_name", "Phase2_NeuralComputation");
        config->set("enable_structural_plasticity", true);
    }
    
    static void applyPhase5Config(std::shared_ptr<Config> config) {
        if (!config) return;
        
        NLMConfigUtils::applyDefaultConfig(config);
        
        // Phase 5 performance optimization settings
        config->set("neuron_count", 2000LL);
        config->set("region_count", 3LL);
        config->set("enable_memory_pools", true);
        config->set("enable_parallel_processing", true);
        config->set("simd_vectorization", true);
        
        // Performance tuning
        config->set("min_check_point_interval", 1000);
        config->set("max_threads", 4);
    }
};

} // namespace nlm

#endif // PHASE_CONFIG_H
