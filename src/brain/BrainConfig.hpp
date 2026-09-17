// BrainConfig.hpp - Updated to include prediction system parameters
#pragma once

namespace nlm {

// Config keys and default values for brain configuration
// This file should be included by Brain.hpp or Brain.cpp to provide centralized configuration constants

struct BrainIntegrationConfig {
    // Memory and episode management
    static constexpr size_t DEFAULT_EPISODE_STORAGE_INTERVAL = 10;
    
    // Connectivity and replay
    static constexpr size_t DEFAULT_REPLAY_INTERVAL = 100;
    static constexpr size_t DEFAULT_CONSOLIDATION_INTERVAL = 1000;
    
    // Plasticity and development
    static constexpr size_t DEFAULT_STRUCTURAL_PLASTICITY_INTERVAL = 100;
    static constexpr size_t DEFAULT_DEVELOPMENT_INTERVAL = 1000;
    
    // Synaptic weight initialization
    static constexpr float DEFAULT_SYNAPTIC_EXCITATION_WEIGHT = 0.2f;
    static constexpr float DEFAULT_SYNAPTIC_INHIBITION_WEIGHT = 0.1f;
    
    // Signal and current scaling factors
    static constexpr float DEFAULT_SYNAPTIC_CURRENT_SCALE = 10.0f;
    static constexpr float DEFAULT_POTENTIAL_NORMALIZATION_DIVISOR = 10.0f;
    static constexpr float DEFAULT_PLASTICITY_MODIFIER_SCALE = 0.001f;
    
    // Prediction system parameters
    static constexpr size_t DEFAULT_PREDICTION_SEQUENCE_MEMORY_SIZE = 10;
    static constexpr size_t DEFAULT_PREDICTION_HORIZON_STEPS = 1;
    static constexpr float DEFAULT_PREDICTION_NOVELTY_WEIGHT = 0.5f;
    static constexpr float DEFAULT_PREDICTION_ERROR_WEIGHT = 0.5f;
    static constexpr float DEFAULT_PREDICTION_SURPRISE_THRESHOLD = 0.3f;
};

// Type aliases for improved type safety
typedef size_t SimulationStepCount;
typedef double TimeDuration;
typedef float WeightValue;
typedef float CurrentValue;
typedef float ModulationFactor;

// Helper functions for common calculations
namespace BrainCalculations {
    // Calculate synaptic current from weight using configurable scale factor
    inline CurrentValue CalculateSynapticCurrent(WeightValue weight, 
                                                float scaleFactor = BrainIntegrationConfig::DEFAULT_SYNAPTIC_CURRENT_SCALE) {
        return weight * scaleFactor;
    }
    
    // Normalize membrane potential difference
    inline float NormalizePotentialDifference(float potential, 
                                           float restingPotential,
                                           float divisor = BrainIntegrationConfig::DEFAULT_POTENTIAL_NORMALIZATION_DIVISOR) {
        return std::abs(potential - restingPotential) / divisor;
    }
    
    // Apply plasticity modification with configurable scale
    inline WeightValue ApplyPlasticityModification(WeightValue weight,
                                                 ModulationFactor plasticityMod,
                                                 float scale = BrainIntegrationConfig::DEFAULT_PLASTICITY_MODIFIER_SCALE) {
        return weight + (weight > 0.0f ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * scale;
    }
}

} // namespace nlm