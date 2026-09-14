#include "Neuromodulator.hpp"
#include "NeuromodulatorBase.hpp"
#include "Dopamine.hpp"
#include "Curiosity.hpp"
#include "Novelty.hpp"
#include "PredictionError.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

// Neuromodulator type enumeration
enum class NeuromodulatorType {
    Dopamine,
    Curiosity,
    Novelty,
    PredictionError,
    Acetylcholine,
    Norepinephrine,
    Serotonin
};

// Base abstract class for neuromodulators
class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
    // Initialize with brain reference
    virtual void initialize(class Brain* brain) = 0;
    
    // Get type identifier
    virtual NeuromodulatorType getType() const = 0;
    
    // Load configuration parameters
    virtual void loadConfig(const std::string& key, float value) = 0;
    
protected:
    Neuromodulator() = default;
    Neuromodulator(const Neuromodulator&) = default;
    Neuromodulator& operator=(const Neuromodulator&) = default;
};

// Common base for all neuromodulators
class NeuromodulatorBase : public Neuromodulator {
protected:
    NeuromodulatorBase() = default;
    ~NeuromodulatorBase() override = default;
    
    // Common state
    float currentLevel = 0.0f;
    TimestepDuration lastUpdateTime = 0;
    float strength = 1.0f;
    
    // Common helper methods
    float computeDecay(float level, float decayRate, TimestepDuration dt) const {
        return std::max(0.0f, level - decayRate * static_cast<float>(dt));
    }
    
    float computeScale(float value, float min, float max) const {
        return std::clamp((value - min) / (max - min), 0.0f, 1.0f);
    }
    
    void applySmoothing(float& current, float target, float smoothingFactor) {
        current += (target - current) * smoothingFactor;
    }
};

// Dopamine implementation
struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
};

Dopamine::Dopamine() : NeuromodulatorBase(), pImpl(new Impl) {
    currentLevel = 0.0f;
    lastUpdateTime = 0;
    strength = 1.0f;
}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return currentLevel;
}

void Dopamine::setLevel(float level) {
    currentLevel = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    return 0.5f + 0.5f * currentLevel;
}

void Dopamine::update(TimestepDuration dt) {
    currentLevel = computeDecay(currentLevel, pImpl->decayRate, dt);
}

void Dopamine::signalReward(float reward) {
    currentLevel = std::min(pImpl->peak, currentLevel + reward * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    currentLevel = std::max(0.0f, currentLevel + error * pImpl->releaseRate);
}

void Dopamine::initialize(class Brain* brain) {}

NeuromodulatorType Dopamine::getType() const {
    return NeuromodulatorType::Dopamine;
}

void Dopamine::loadConfig(const std::string& key, float value) {
    if (key == "decayRate") pImpl->decayRate = value;
    else if (key == "releaseRate") pImpl->releaseRate = value;
    else if (key == "baseline") pImpl->baseline = value;
    else if (key == "peak") pImpl->peak = value;
}

// Acetylcholine implementation
struct Acetylcholine::Impl {
    float level;
    
    Impl() : level(0.0f) {}
};

Acetylcholine::Acetylcholine() : NeuromodulatorBase(), pImpl(new Impl) {
    currentLevel = 0.0f;
    lastUpdateTime = 0;
    strength = 1.0f;
}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return currentLevel;
}

void Acetylcholine::setLevel(float level) {
    currentLevel = level;
}

float Acetylcholine::getPlasticityFactor() const {
    return 1.0f;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Placeholder - no dynamics yet
}

void Acetylcholine::initialize(class Brain* brain) {}

NeuromodulatorType Acetylcholine::getType() const {
    return NeuromodulatorType::Acetylcholine;
}

void Acetylcholine::loadConfig(const std::string& key, float value) {
    // Configuration loading for Acetylcholine
}

// Norepinephrine implementation
struct Norepinephrine::Impl {
    float level;
    
    Impl() : level(0.0f) {}
};

Norepinephrine::Norepinephrine() : NeuromodulatorBase(), pImpl(new Impl) {
    currentLevel = 0.0f;
    lastUpdateTime = 0;
    strength = 1.0f;
}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return currentLevel;
}

void Norepinephrine::setLevel(float level) {
    currentLevel = level;
}

float Norepinephrine::getPlasticityFactor() const {
    return 1.0f;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Placeholder - no dynamics yet
}

void Norepinephrine::initialize(class Brain* brain) {}

NeuromodulatorType Norepinephrine::getType() const {
    return NeuromodulatorType::Norepinephrine;
}

void Norepinephrine::loadConfig(const std::string& key, float value) {
    // Configuration loading for Norepinephrine
}

// Serotonin implementation
struct Serotonin::Impl {
    float level;
    
    Impl() : level(0.0f) {}
};

Serotonin::Serotonin() : NeuromodulatorBase(), pImpl(new Impl) {
    currentLevel = 0.0f;
    lastUpdateTime = 0;
    strength = 1.0f;
}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return currentLevel;
}

void Serotonin::setLevel(float level) {
    currentLevel = level;
}

float Serotonin::getPlasticityFactor() const {
    return 1.0f;
}

void Serotonin::update(TimestepDuration dt) {
    // Placeholder - no dynamics yet
}

void Serotonin::initialize(class Brain* brain) {}

NeuromodulatorType Serotonin::getType() const {
    return NeuromodulatorType::Serotonin;
}

void Serotonin::loadConfig(const std::string& key, float value) {
    // Configuration loading for Serotonin
}

} // namespace nlm