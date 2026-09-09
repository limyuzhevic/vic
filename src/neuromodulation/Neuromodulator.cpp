#include "Neuromodulator.hpp"

namespace nlm {

struct SimpleNeuromodulator::Impl {
    const char* name;
    float level;
    
    Impl(const char* n) : name(n), level(0.0f) {}
};

SimpleNeuromodulator::SimpleNeuromodulator(const char* name) : pImpl(new Impl(name)) {}

SimpleNeuromodulator::~SimpleNeuromodulator() = default;

const char* SimpleNeuromodulator::getName() const {
    return pImpl->name;
}

float SimpleNeuromodulator::getLevel() const {
    return pImpl->level;
}

void SimpleNeuromodulator::setLevel(float level) {
    pImpl->level = level;
}

float SimpleNeuromodulator::getPlasticityFactor() const {
    return 1.0f;  // Default no modulation
}

void SimpleNeuromodulator::update(TimestepDuration dt) {
    // Default implementation: no dynamics
}

struct Acetylcholine::Impl {
    Acetylcholine::Impl() {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

float Acetylcholine::getPlasticityFactor() const {
    // ACh modulates attention and working memory
    return 1.2f;  // Slight enhancement
}

struct Norepinephrine::Impl {
    Norepinephrine::Impl() {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates arousal and vigilance
    return 1.1f;  // Mild enhancement
}

struct Serotonin::Impl {
    Serotonin::Impl() {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

float Serotonin::getPlasticityFactor() const {
    // 5-HT modulates mood and impulsivity
    return 1.0f;  // Neutral baseline
}

} // namespace nlm
