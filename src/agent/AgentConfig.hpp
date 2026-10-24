// Agent Configuration Constants
// Centralized configuration for agent behavior and parameters

#pragma once

namespace nlm {

namespace AgentConfig {
    // Sensory configuration
    constexpr size_t VISION_WIDTH = 16;
    constexpr size_t VISION_HEIGHT = 16;
    constexpr size_t VISION_SIZE = VISION_WIDTH * VISION_HEIGHT;
    
    constexpr size_t TOUCH_SIZE = 8;
    constexpr size_t INTERNAL_SIZE = 4;
    constexpr size_t PROPRIOCEPTION_SIZE = 6;
    constexpr size_t AUDIO_SIZE = 0;  // Disabled by default
    
    // Motor configuration
    constexpr size_t MOTOR_GROUP_COUNT = 6;
    
    // Thresholds and constants
    constexpr float NOVELTY_DECAY = 0.99f;
    constexpr float ACTIVATION_THRESHOLD = 0.5f;
    constexpr float EXPLORATION_THRESHOLD = 0.3f;
    constexpr float HIGH_EXPLORATION_THRESHOLD = 0.5f;
    
    // Neuromodulation parameters
    constexpr float DOPAMINE_CLAMP_MIN = -1.0f;
    constexpr float DOPAMINE_CLAMP_MAX = 1.0f;
    constexpr float ELIGIBILITY_THRESHOLD = 0.001f;
    constexpr float ELIGIBILITY_DECAY = 0.1f;
    constexpr float PLASTICITY_MIN = 0.1f;
    constexpr float PLASTICITY_MAX = 2.0f;
    constexpr float PLASTICITY_BASE = 0.5f;
    
    // Development stages
    constexpr double DEVELOPMENT_STAGE_INITIAL = 60.0;
    constexpr double DEVELOPMENT_STAGE_CRITICAL = 300.0;
    constexpr double DEVELOPMENT_STAGE_MATURATION = 900.0;
    
    // Default values
    constexpr float DEFAULT_AGE = 1.0f;
    constexpr float DEFAULT_HEALTH = 1.0f;
    constexpr float DEFAULT_ENERGY = 1.0f;
    constexpr float DEFAULT_PLATISTICS_MODIFIER = 1.0f;
    
    // Command configuration
    constexpr size_t MAX_COMMAND_TOKENS = 10;
    constexpr size_t MAX_COMMAND_LENGTH = 256;
    
    // Performance
    constexpr size_t MOTOR_GROUP_CACHE_SIZE = 8;
}

} // namespace nlm