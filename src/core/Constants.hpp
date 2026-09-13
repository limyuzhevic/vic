#pragma once

#include <chrono>
#include <thread>
#include <map>
#include <set>
#include <limits>
#include <string_view>
#include "src/core/Types/Types.hpp"

namespace nlm {

// Neural system constants
namespace Constants {

// Vision constants
constexpr int VISION_WIDTH = 16;
constexpr int VISION_HEIGHT = 16;
constexpr int VISION_SIZE = VISION_WIDTH * VISION_HEIGHT;  // 256
constexpr float kVisionInputScale = 5.0f;

// Motor constants
constexpr int NUM_MOTOR_ACTIONS = 6;

// Sensory constants
constexpr int SENSE_TOUCH_SIZE = 8;
constexpr int SENSE_INTERNAL_SIZE = 4;
constexpr int SENSE_PROPRIOCEPTION_SIZE = 6;
constexpr float kTouchInputScale = 8.0f;
constexpr float kInternalInputScale = 5.0f;
constexpr float kProprioceptionInputScale = 3.0f;

// Development constants
constexpr double DEVELOPMENT_STAGE_DURATION_INITIAL = 60.0;   // seconds
constexpr double DEVELOPMENT_STAGE_DURATION_CRITICAL = 300.0; // seconds
constexpr double DEVELOPMENT_STAGE_DURATION_MATURATION = 900.0; // seconds
constexpr double DEVELOPMENT_STAGE_ADULT = 900.0;              // seconds

// Plasticity constants
constexpr float INITIAL_PLASTICITY_MODIFIER = 1.0f;
constexpr float CRITICAL_PERIOD_PLASTICITY_MODIFIER = 0.8f;
constexpr float MATURATION_PLASTICITY_MODIFIER = 0.5f;
constexpr float ADULT_PLASTICITY_MODIFIER = 0.2f;

// Neuromodulation constants
constexpr float MAX_DOPAMINE_LEVEL = 1.0f;
constexpr float MIN_DOPAMINE_LEVEL = -1.0f;
constexpr float DEFAULT_NOVELTY_DECAY = 0.99f;

// Learning constants
constexpr float DEFAULT_LTP_WEIGHT = 0.01f;
constexpr float DEFAULT_LTD_WEIGHT = 0.012f;
constexpr float SYNAPTOGENESIS_RATE_INITIAL = 0.0001f;
constexpr float PRUNING_RATE_INITIAL = 0.00001f;

// Memory constants
constexpr float WORKING_MEMORY_MAX_TRACES = 100.0f;

// Simulation constants
constexpr double DEFAULT_TIMESTEP = 0.001;  // seconds
constexpr uint64_t MAX_STEPS_PER_EPISODE = 10000;

// Exploration constants
constexpr float NOVELTY_THRESHOLD = 0.5f;
constexpr float HIGH_CURIOCITY_THRESHOLD = 0.3f;
constexpr float MAX_EXPLORATION_CHANCE = 0.3f;
constexpr float kCuriosityFromNovelty = 2.0f;
constexpr float kCuriosityFromPredictionError = 0.5f;

// Safety constants
constexpr size_t NULL_POINTER_CHECK = 1;  // Use 1 to enable checks
constexpr float MIN_ACTIVITY_THRESHOLD = 0.1f;
constexpr float MIN_REWARD_MAGNITUDE = 0.01f;
constexpr float kMinEligibilityThreshold = 0.001f;
constexpr float kEligibilityDecayRate = 0.1f;
constexpr float kBasePlasticityFactor = 0.5f;
constexpr float kDopaminePlasticitySlope = 0.5f;
constexpr float kMinPlasticityFactor = 0.1f;
constexpr float kMaxPlasticityFactor = 2.0f;
constexpr float kPruningBaseFactor = 2.0f;

// Deprecated constants - kept for backward compatibility
constexpr int DEPRECATED_VISION_INPUT_SCALE = 5;
constexpr int DEPRECATED_TOUCH_INPUT_SCALE = 8;
constexpr int DEPRECATED_INTERNAL_INPUT_SCALE = 5;
constexpr int DEPRECATED_PROPRIOCEPTION_INPUT_SCALE = 3;

} // namespace Constants

} // namespace nlm
