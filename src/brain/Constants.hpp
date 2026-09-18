#pragma once

// Constants namespace to replace magic numbers
namespace nlm {
    namespace DefaultConstants {
        // Random seed
        constexpr uint64_t DEFAULT_RANDOM_SEED = 42;
        
        // STDP weights
        constexpr float DEFAULT_LTP_WEIGHT = 0.01f;
        constexpr float DEFAULT_LTD_WEIGHT = 0.012f;
        constexpr float DEFAULT_STDP_TAU = 20.0f;
        
        // Synaptogenesis and pruning rates
        constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;
        constexpr float DEFAULT_PRUNING_RATE = 0.00001f;
        
        // Simulation timestep
        constexpr double DEFAULT_TIMESTEP = 0.001;
        
        // Vision scaling factors
        constexpr float VISION_SCALE_FACTOR_EYE = 5.0f;
        constexpr float VISION_SCALE_FACTOR_PUPIL = 8.0f;
        constexpr float VISION_SCALE_FACTOR_LIGHT = 0.8f;
        
        // Novelty thresholds
        constexpr float NOVELTY_THRESHOLD_INITIAL = 0.5f;
        constexpr float NOVELTY_THRESHOLD_SUSTAINED = 0.3f;
        
        // Exploitation thresholds
        constexpr float EXPLOITATION_THRESHOLD_INITIAL = 0.7f;
        constexpr float EXPLOITATION_THRESHOLD_SUSTAINED = 0.6f;
        
        // Activation thresholds
        constexpr float ACTIVATION_THRESHOLD_REGULAR = 15.0f;
        constexpr float ACTIVATION_THRESHOLD_SPIKE = 100.0f;
        
        // Memory thresholds
        constexpr float MEMORY_THRESHOLD_ACTIVATION = 0.8f;
        constexpr float MEMORY_THRESHOLD_CONSOLIDATION = 0.9f;
        
        // Development parameters
        constexpr float DEVELOPMENT_RATE = 0.05f;
        constexpr float DEVELOPMENT_DECAY = 0.995f;
        
        // Learning parameters
        constexpr float LEARNING_RATE = 0.01f;
        constexpr float LEARNING_MOMENTUM = 0.9f;
        
        // Reward parameters
        constexpr float REWARD_RANGE_MIN = -1.0f;
        constexpr float REWARD_RANGE_MAX = 1.0f;
        
        // Attention parameters
        constexpr float ATTENTION_FOCUS_FACTOR = 0.9f;
        constexpr float ATTENTION_SPREAD_FACTOR = 0.1f;
        
        // Prediction parameters
        constexpr float PREDICTION_ERROR_THRESHOLD = 0.1f;
        constexpr float PREDICTION_LEARNING_RATE = 0.05f;
        
        // Planning parameters
        constexpr size_t PLANNING_HORIZON = 5;
        constexpr float PLANNING_CONFIDENCE_THRESHOLD = 0.8f;
        
        // Concept formation parameters
        constexpr float CONCEPT_THRESHOLD_SIMILARITY = 0.85f;
        constexpr float CONCEPT_THRESHOLD_EMERGENCE = 0.7f;
        
        // Structural plasticity parameters
        constexpr float STRUCTURAL_PLASTICITY_RATE = 0.001f;
        constexpr float STRUCTURAL_PLASTICITY_DECAY = 0.999f;
        
        // Replay parameters
        constexpr size_t REPLAY_INTERVAL_DEFAULT = 100;
        constexpr size_t CONSOLIDATION_INTERVAL_DEFAULT = 1000;
        
        // Checkpoint parameters
        constexpr size_t CHECKPOINT_INTERVAL = 1000;
        constexpr const char* CHECKPOINT_FILENAME_PREFIX = "checkpoint_";
    }
    
    // Helper functions for common operations
    inline double clamp(double value, double min, double max) {
        return (value < min) ? min : ((value > max) ? max : value);
    }
    
    inline float clamp(float value, float min, float max) {
        return (value < min) ? min : ((value > max) ? max : value);
    }
    
    inline float normalize(float value, float min, float max) {
        if (max == min) return 0.5f;
        return (value - min) / (max - min);
    }
    
    inline double normalize(double value, double min, double max) {
        if (max == min) return 0.5;
        return (value - min) / (max - min);
    }
    
    inline float movingAverage(float current, float previous, float weight) {
        return (weight * current) + ((1.0f - weight) * previous);
    }
    
    inline double movingAverage(double current, double previous, double weight) {
        return (weight * current) + ((1.0 - weight) * previous);
    }
    
    // Vector utility functions
    inline double vectorMagnitude(const std::vector<double>& v) {
        double sum = 0.0;
        for (double x : v) sum += x * x;
        return std::sqrt(sum);
    }
    
    inline float vectorMagnitude(const std::vector<float>& v) {
        float sum = 0.0f;
        for (float x : v) sum += x * x;
        return std::sqrt(sum);
    }
    
    inline double vectorDotProduct(const std::vector<double>& a, const std::vector<double>& b) {
        double result = 0.0;
        for (size_t i = 0; i < a.size() && i < b.size(); ++i) {
            result += a[i] * b[i];
        }
        return result;
    }
    
    inline float vectorDotProduct(const std::vector<float>& a, const std::vector<float>& b) {
        float result = 0.0f;
        for (size_t i = 0; i < a.size() && i < b.size(); ++i) {
            result += a[i] * b[i];
        }
        return result;
    }
}
