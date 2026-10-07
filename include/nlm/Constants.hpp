#ifndef NLM_CONSTANTS_H
#define NLM_CONSTANTS_H

namespace nlm {

// **Working Memory Constants**
namespace WorkingMemoryConstants {
    constexpr size_t DEFAULT_CAPACITY = 100;
    constexpr float DEFAULT_DECAY_RATE = 0.01f;
}

// **Episodic Memory Constants**
namespace EpisodicMemoryConstants {
    constexpr size_t DEFAULT_MAX_EPISODES = 1000;
    constexpr float DEFAULT_CONSOLIDATION_THRESHOLD = 0.3f;
    constexpr float DEFAULT_RELEVANCE_THRESHOLD = 0.5f;
    constexpr size_t MIN_EPISODE_AGE_TO_REMOVE = 500;
    constexpr size_t MIN_EPISODE_AGE_TO_PREVENT_REMOVAL = 1000;
}

// **Semantic Memory Constants**
namespace SemanticMemoryConstants {
    // No constants needed for simple key-value store
}

// **Procedural Memory Constants**
namespace ProceduralMemoryConstants {
    constexpr float DEFAULT_INITIAL_PROFICIENCY = 0.3f;
    constexpr float DEFAULT_LEARNING_RATE = 0.1f;
    constexpr float DEFAULT_DECAY_RATE = 0.1f;
    constexpr float DEFAULT_PRACTICE_DECAY_THRESHOLD = 1000.0f;
}

// **Associative Memory Constants**
namespace AssociativeMemoryConstants {
    constexpr size_t DEFAULT_NEXT_ID_OFFSET = 1000;
    constexpr size_t DEFAULT_HISTORY_LIMIT = 10000;
}

// **Prediction System Constants**
namespace PredictionSystemConstants {
    constexpr size_t DEFAULT_CURRENT_SENSORY_SIZE = 50;
    constexpr size_t DEFAULT_VISION_PATTERN_SIZE = 16;
    constexpr float DEFAULT_VARIANCE_DIVISOR = 10.0f;
    constexpr size_t DEFAULT_ERROR_HISTORY_LIMIT = 100;
    constexpr float DEFAULT_SIMILARITY_THRESHOLD = 0.7f;
}

// **Reward System Constants**
namespace RewardConstants {
    constexpr float HEALTHY_ENERGY_MIN = 50.0f;
    constexpr float HEALTHY_ENERGY_MAX = 90.0f;
    constexpr float LOW_ENERGY_THRESHOLD = 20.0f;
    constexpr float RECOVERY_ENERGY_THRESHOLD = 10.0f;
    constexpr size_t HAZARD_PROXIMITY_INDEX = 10;
    constexpr float HAZARD_PROXIMITY_MULTIPLIER = 10.0f;
    constexpr size_t RESOURCE_PROXIMITY_INDEX = 11;
    constexpr float RESOURCE_PROXIMITY_MULTIPLIER = 15.0f;
    constexpr size_t NOVELTY_FEATURE_START_INDEX = 15;
    constexpr size_t NOVELTY_FEATURE_END_INDEX = 25;
    constexpr float REWARD_CLAMP_MIN = -50.0f;
    constexpr float REWARD_CLAMP_MAX = 50.0f;
    constexpr float ENERGY_BONUS_MULTIPLIER = 0.1f;
    constexpr float RECOVERY_BONUS = 1.0f;
    constexpr float TIME_BONUS = 0.01f;
    constexpr float NOVELTY_MULTIPLIER = 5.0f;
}

// **Development System Constants**
namespace DevelopmentConstants {
    constexpr float DEVELOPMENT_TIMESTEP = 0.001f;
    constexpr float INITIAL_STAGE_AGE_THRESHOLD = 60.0f;
    constexpr float CRITICAL_PERIOD_AGE_THRESHOLD = 360.0f;
    constexpr float MATURATION_STAGE_AGE_THRESHOLD = 960.0f;
    constexpr size_t CONSOLIDATION_INTERVAL = 1000;
}

// **Brain/Neural Constants**
namespace NeuralConstants {
    constexpr float DEFAULT_TIMESTEP = 0.001f;
    constexpr float DEFAULT_REPLAY_INTERVAL = 100;
    constexpr float DEFAULT_CONSOLIDATION_INTERVAL = 1000;
    constexpr float DEFAULT_SIMULATION_TIMESTEP = 0.001f;
    constexpr float DEFAULT_REPLAY_INTERVAL_DEFAULT = 100;
    constexpr float DEFAULT_CONSOLIDATION_INTERVAL_DEFAULT = 1000;
    constexpr size_t DEFAULT_NEURON_COUNT = 1000;
    constexpr float SYNAPTIC_CURRENT_SCALE_FACTOR = 10.0f;
    constexpr float NORMALIZATION_INPUT_MAX = 10.0f;
    constexpr size_t EPISODE_STORAGE_INTERVAL = 10;
    constexpr size_t EPISODE_AGE_THRESHOLD_FOR_REMOVAL = 500;
    constexpr float NORMALIZATION_VALUE_DIVISOR = 10.0f;
}

// **Plasticity Constants**
namespace PlasticityConstants {
    constexpr float DEFAULT_STDP_TAU = 20.0f;  // ms
    constexpr float DEFAULT_SPIKE_HISTORY_SIZE = 100;
    constexpr float DEFAULT_NEURON_TIME_CONSTANT = 20.0f;  // ms
    constexpr float DEFAULT_MEMBRANE_RESISTANCE = 10.0f;
    constexpr size_t DEFAULT_SPIKE_HISTORY_MAX_SIZE = 100;
    constexpr float DEFAULT_REFACTORY_PERIOD_MIN = 2.0f;
    constexpr float DEFAULT_REFACTORY_PERIOD_MAX = 10.0f;
}

// **Maturation Constants**
namespace MaturationConstants {
    constexpr float DEFAULT_MATURE_THRESHOLD = -55.0f;  // mV
    constexpr float DEFAULT_MATURE_RESTING_POTENTIAL = -70.0f;  // mV
    constexpr float DEFAULT_MATURE_TIME_CONSTANT = 20.0f;  // ms
}

// **General Constants**
namespace GeneralConstants {
    constexpr size_t DEFAULT_SEED = 42;
    constexpr size_t SIMULATION_STEPS_PER_SECOND = 1000;
    constexpr float DEFAULT_REWARD_RANGE_NORMALIZATION = 100.0f;
    constexpr float DEFAULT_VARIANCE_NORMALIZATION = 100.0f;
    constexpr size_t DEFAULT_STEP_RATE = 10;
    constexpr float DEFAULT_PROFICIENCY_CLAMP_MIN = 0.0f;
    constexpr float DEFAULT_PROFICIENCY_CLAMP_MAX = 1.0f;
    constexpr float DEFAULT_ASSOCIATION_STRENGTH_CLAMP_MIN = 0.0f;
    constexpr float DEFAULT_ASSOCIATION_STRENGTH_CLAMP_MAX = 1.0f;
}

} // namespace nlm

#endif // NLM_CONSTANTS_H