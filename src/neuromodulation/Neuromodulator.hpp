#pragma once

#include "../core/Types/Types.hpp"
#include <algorithm>
#include <string_view>
#include <memory>

namespace nlm {

/**
 * @class Neuromodulator
 * @brief Abstract base class for neuromodulatory signals with common functionality
 * 
 * This class provides a standardized interface for all neuromodulators in the brain,
 * along with common functionality for level management, plasticity factors, and
 * state updates. Derived classes implement specific neuromodulation mechanisms.
 * 
 * Neuromodulators are chemical signals that modulate neural activity and plasticity,
 * influencing learning, attention, motivation, and other cognitive processes.
 * 
 * @note This class uses the PImpl idiom for implementation hiding and forward
 *       declaration to avoid circular dependencies.
 */
class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;

    /**
     * @brief Get the neuromodulator name
     * @return Human-readable name of the neuromodulator
     */
    virtual const char* getName() const = 0;

    /**
     * @brief Get the current concentration/level of the neuromodulator
     * @return Current neuromodulator level (0.0 to 1.0 typical range)
     */
    virtual float getLevel() const = 0;
    
    /**
     * @brief Set the neuromodulator level
     * @param level New level value
     */
    virtual void setLevel(float level) = 0;

    /**
     * @brief Get the plasticity modulation factor for this neuromodulator
     * @return Multiplicative factor applied to plasticity (1.0 = baseline)
     * 
     * Neuromodulators can enhance or suppress synaptic plasticity.
     * Values > 1.0 increase plasticity, values < 1.0 decrease it.
     */
    virtual float getPlasticityFactor() const = 0;

    /**
     * @brief Update neuromodulator state based on elapsed time
     * @param dt Time elapsed since last update (in milliseconds)
     */
    virtual void update(TimestepDuration dt) = 0;

protected:
    Neuromodulator() = default;

    /**
     * @brief Apply clamping to ensure level stays within valid range
     * @param value Raw value to clamp
     * @param min Minimum allowed value
     * @param max Maximum allowed value
     * @return Clamped value
     */
    float clamp(float value, float min = 0.0f, float max = 1.0f) const {
        return std::max(min, std::min(max, value));
    }

    /**
     * @brief Smooth transition towards target level
     * @param current Current level
     * @param target Target level
     * @param rate Smoothing rate (0.0 to 1.0, higher = faster)
     * @return Smoothed level
     */
    float smooth(float current, float target, float rate) const {
        return current + (target - current) * rate;
    }
};

/**
 * @class BaseNeuromodulator
 * @brief Concrete implementation of common neuromodulator functionality
 * 
 * This template class provides implementations of common neuromodulator methods
 * for derived classes that need to store level, baseline, and decay dynamics.
 * It reduces code duplication across neuromodulator implementations.
 * 
 * @tparam T The concrete neuromodulator type (Curiosity, Novelty, etc.)
 */
template<typename T>
class BaseNeuromodulator : public Neuromodulator {
public:
    virtual ~BaseNeuromodulator() = default;

    const char* getName() const override {
        return T::getStaticName();
    }

    float getLevel() const override {
        return pImpl->level;
    }
    
    void setLevel(float level) override {
        pImpl->level = clamp(level);
    }

    float getPlasticityFactor() const override {
        return 1.0f;  // Default no modulation
    }

    void update(TimestepDuration dt) override {
        // Default decay towards baseline - override if needed
        pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    }

protected:
    struct Impl {
        float level;           ///< Current neuromodulator concentration
        float baseline;        ///< Baseline level (steady-state target)
        float peak;            ///< Peak level (maximum response)
        float decayRate;       ///< Rate of decay towards baseline
        float releaseRate;     ///< Rate of increase from stimuli
        
        Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
    };

    BaseNeuromodulator() : pImpl(std::make_unique<Impl>()) {}
    
    T* self() { return static_cast<T*>(this); }
    const T* self() const { return static_cast<const T*>(this); }
    
    Impl& getImpl() { return *pImpl; }
    const Impl& getImpl() const { return *pImpl; }

    // Allow derived classes to access implementation
    template<typename U>
    friend class BaseNeuromodulator;

private:
    std::unique_ptr<Impl> pImpl;
};

/**
 * @brief Macro to help implement derived neuromodulators with common patterns
 * 
 * This macro reduces boilerplate code for implementing new neuromodulators
 * by providing common constructor, destructor, and basic accessor implementations.
 * 
 * Usage:
 * class MyNeuromodulator : public BaseNeuromodulator<MyNeuromodulator> {
 *     IMPLEMENT_NEUROMODULATOR("MyNmod", 0.5f, 0.1f)
 * };
 */
#define IMPLEMENT_NEUROMODULATOR(name, baseline, decayRate) \
    static constexpr const char* getStaticName() { return name; } \
    MyNeuromodulator() = default; \
    ~MyNeuromodulator() = default;

// Concrete neuromodulators without PImpl redundancy
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
private:
    struct Impl;
    Impl* pImpl;
};

class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
};

class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
};

class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
};

} // namespace nlm
