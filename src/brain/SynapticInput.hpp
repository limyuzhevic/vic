#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

class SynapticInput {
public:
    SynapticInput();
    
    // Incoming signals (post-synaptic potentials)
    void receiveExcitatoryInput(MembranePotential amplitude);
    void receiveInhibitoryInput(MembranePotential amplitude);
    void receiveModulatoryInput(MembranePotential amplitude);
    
    // Current injection (from external sources)
    void injectCurrent(MembranePotential current);
    
    // Access accumulated synaptic input
    MembranePotential getSynapticInput() const { return synapticInput_; }
    void setSynapticInput(MembranePotential input) { synapticInput_ = input; }
    void clearSynapticInput() { synapticInput_ = 0.0f; }
    
    // Get total current (for backward compatibility)
    MembranePotential getTotalCurrent() const { return synapticInput_; }
    
    // Synaptic input contribution with adaptation
    void applyAdaptation(float adaptationVariable, MembranePotential& V) const;
    
private:
    MembranePotential synapticInput_;  // Accumulated synaptic input
};

} // namespace nlm