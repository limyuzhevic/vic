#pragma once

namespace nlm {

namespace NeuromodulationConstants {
    // Acetylcholine constants
    const float ACh_BASELINE_LEVEL = 0.1f;
    const float ACh_PEAK_LEVEL = 1.0f;
    const float ACh_DECAY_RATE = 0.05f;
    const float ACh_CONSOLIDATION_GAIN = 0.8f;
    const float ACh_ATTENTION_STRENGTH = 0.6f;
    
    // Synaptic consolidation constants
    const float SYNAPTIC_CONSOLIDATION_TIME = 30.0f;      // Minutes
    const float PROTEIN_SYNTHESIS_DELAY = 15.0f;        // Minutes
    const float PROTEIN_DECAY_RATE = 0.02f;            // Per minute
    const float SYNAPTIC_STRENGTH_MAX = 1.0f;
    const float SYNAPTIC_STRENGTH_MIN = 0.01f;
    
    // Systems consolidation constants
    const float SYSTEMS_CONSOLIDATION_RATE = 0.005f;   // Per minute
    const float HIPPOCAMPAL_REPLACEMENT = 0.9f;        // After 6-8 weeks
    const float NEURAL_INDEXING_RATE = 0.1f;           // Per minute
    
    // Memory integration constants
    const float BINDING_STRENGTH = 0.7f;
    const float TEMPORAL_BINDING_WEIGHT = 0.3f;
    const float CAUSAL_BINDING_WEIGHT = 0.5f;
    const float SCHEMA_INTEGRATION_THRESHOLD = 0.6f;
    
    // Sleep consolidation constants
    const float SHARP_WAVE_RIPPLE_FREQ = 0.15f;        // Hz
    const float REPLAY_PROBABILITY_BASE = 0.3f;
    const float REPLAY_ENHANCEMENT_REWARD = 0.4f;
    const float REPLAY_ENHANCEMENT_RELEVANCE = 0.3f;
    const float MEMORY_COMPETITION_FACTOR = 0.8f;
    
    // Active maintenance constants
    const float EPISODIC_BUFFER_SIZE = 50.0f;          // Number of items
    const float WORKING_EPISODIC_TRANSFER_RATE = 0.7f;
    const float CUE_DEPENDENCY_FACTOR = 0.6f;
    const float CONTEXT_MAINTENANCE_STRENGTH = 0.5f;
    
    // Forgetting constants
    const float FORGETTING_CURVE_RATE = 0.05f;         // Per minute
    const float INTERFERENCE_RATE = 0.03f;             // Per minute
    const float RETRIEVAL_INDUCED_FORGETTING = 0.02f; // Per recall
    const float MEMORY_RECONSTRUCTION_BIAS = 0.2f;     // Proportion
    
    // Development constants
    const float DEVEL_PLASTICITY_FACTOR_INITIAL = 1.0f;
    const float DEVEL_PLASTICITY_FACTOR_ADULT = 0.2f;
    const float DEVEL_DEVELOPMENT_DURATION = 1000.0f;   // Simulation steps
    const float DEVEL_CRITICAL_PERIOD = 200.0f;       // Simulation steps
    
    // Reward prediction constants
    const float REWARD_PREDICTION_GAMMA = 0.9f;
    const float REWARD_PREDICTION_LEARNING_RATE = 0.01f;
    const float REWARD_EXTRAPOLATION_FACTOR = 0.3f;
    
    // Hippocampal-cortical constants
    const float HIPPOCAMPAL_UPDATE_RATE = 0.2f;        // Per consolidation event
    const float CORTICAL_STABILIZATION_RATE = 0.02f;  // Per minute
    const float FFW_REACTIVATION_RATE = 0.3f;         // Per replay event
    
} // namespace NeuromodulationConstants

} // namespace nlm

#endif // NEUROMODULATION_CONSTANTS_HPP

