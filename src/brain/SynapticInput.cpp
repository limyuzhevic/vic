#include "SynapticInput.hpp"

namespace nlm {

SynapticInput::SynapticInput() : synapticInput_(0.0f) {}

void SynapticInput::receiveExcitatoryInput(MembranePotential amplitude) {
    synapticInput_ += amplitude;
}

void SynapticInput::receiveInhibitoryInput(MembranePotential amplitude) {
    synapticInput_ -= amplitude;
}

void SynapticInput::receiveModulatoryInput(MembranePotential amplitude) {
    // Modulatory input affects plasticity but not directly integrated
    // Used for neuromodulation (e.g., dopamine, acetylcholine)
    // Note: adaptation variable is handled by AdaptationSystem
}

void SynapticInput::injectCurrent(MembranePotential current) {
    synapticInput_ += current;
}

void SynapticInput::applyAdaptation(float adaptationVariable, MembranePotential& V) const {
    if (adaptationVariable > 0.0f) {
        V -= adaptationVariable * 0.01f;
    }
}

} // namespace nlm