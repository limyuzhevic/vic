#pragma once

#include "../core/Types/Types.hpp"
#include <memory>
#include <string_view>

namespace nlm {

/**
 * @brief Base class template for neuromodulators with common functionality
 * 
 * This template provides a standardized implementation for neuromodulators
 * that need level management, decay, and basic state tracking. It reduces
 * code duplication across different neuromodulator implementations.
 * 
 * @tparam T The concrete neuromodulator type (Curiosity, Novelty, etc.)
 * @tparam Config The configuration structure for the neuromodulator
 */
template<typename T, typename Config>
class BaseNeuromodulator {
public:
    BaseNeuromodulator() = default;
    virtual ~BaseNeuromodulator() = default;

    // Non-copyable, movable
    BaseNeuromodulator(const BaseNeuromodulator&) = delete;
    BaseNeuromodulator& operator=(const BaseNeuromodulator&) = delete;
    BaseNeuromodulator(BaseNeuromodulator&&) = default;
    BaseNeuromodulator& operator=(BaseNeuromodulator&&) = default;

    // Core neuromodulator interface
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    virtual float getPlasticityFactor() const { return 1.0f; }
    virtual void update(TimestepDuration dt) = 0;
    
    // Configuration access
    const Config& getConfig() const { return config; }
    void setConfig(const Config& cfg) { config = cfg; }
    
    // State management
    virtual void reset() = 0;
    virtual bool isInitialized() const { return initialized; }
    virtual void initialize(void* context = nullptr) { initialized = true; }

protected:
    Config config;
    bool initialized = false;
};

/**
 * @brief Simple level-based neuromodulator with decay
 * 
 * A basic neuromodulator implementation that manages a level with
 * configurable baseline, peak, decay, and release rates. Provides
 * common functionality for smooth transitions and clamping.
 */
class SimpleLevelNeuromodulator : public BaseNeuromodulator<SimpleLevelNeuromodulator, SimpleLevelNeuromodulator::Config> {
public:
    struct Config {
        float baseline = 0.0f;      ///< Baseline level (steady-state target)
        float peak = 1.0f;          ///< Peak level (maximum response)
        float decayRate = 0.1f;     ///< Rate of decay towards baseline
        float releaseRate = 1.0f;   ///< Rate of increase from stimuli
        float clampMin = 0.0f;      ///< Minimum allowed level
        float clampMax = 1.0f;      ///< Maximum allowed level
        float smoothingRate = 0.1f; ///< Rate of smooth transitions (0.0-1.0)
        
        Config() = default;
        Config(float baseline, float peak, float decayRate, 
               float releaseRate, float min = 0.0f, float max = 1.0f)
            : baseline(baseline), peak(peak), decayRate(decayRate), 
              releaseRate(releaseRate), clampMin(min), clampMax(max) {}
    };

    SimpleLevelNeuromodulator() = default;
    ~SimpleLevelNeuromodulator() override = default;

    float getLevel() const override {
        return currentLevel;
    }
    
    void setLevel(float level) override {
        currentLevel = clamp(level, config.clampMin, config.clampMax);
    }
    
    void setLevel(float level, float min, float max) {
        currentLevel = clamp(level, min, max);
    }

    void update(TimestepDuration dt) override {
        // Decay towards baseline with smooth transition
        float target = baseline;
        currentLevel = smooth(currentLevel, target, config.smoothingRate);
        
        // Apply decay if not at baseline
        if (currentLevel > baseline) {
            float decay = config.decayRate * static_cast<float>(dt);
            currentLevel = std::max(baseline, currentLevel - decay);
        }
    }

    void reset() override {
        currentLevel = baseline;
    }
    
    void setBaseline(float baseline) {
        this->baseline = baseline;
    }
    
    float getBaseline() const {
        return baseline;
    }
    
    void addToLevel(float delta) {
        currentLevel = clamp(currentLevel + delta, config.clampMin, config.clampMax);
    }
    
    void multiplyLevel(float factor) {
        currentLevel = clamp(currentLevel * factor, config.clampMin, config.clampMax);
    }

protected:
    float currentLevel = 0.0f;
    float baseline = 0.0f;
    
    float clamp(float value, float min, float max) const {
        return std::max(min, std::min(max, value));
    }
    
    float smooth(float current, float target, float rate) const {
        return current + (target - current) * rate;
    }
};

/**
 * @brief Factory for creating neuromodulators
 * 
 * Provides a registry-based factory for creating neuromodulators by name,
 * allowing for easier testing and plugin-like behavior.
 */
class NeuromodulatorFactory {
public:
    template<typename T, typename... Args>
    static T* create(Args&&... args) {
        return new T(std::forward<Args>(args)...);
    }
    
    template<typename T>
    static void destroy(T* neuromodulator) {
        delete neuromodulator;
    }
    
    template<typename T, typename Config>
    static BaseNeuromodulator<T, Config>* createWithConfig(const Config& config) {
        auto* instance = new T();
        instance->setConfig(config);
        return instance;
    }
};

} // namespace nlm
