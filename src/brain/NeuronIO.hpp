#pragma once

namespace nlm {

struct NeuronIOImpl {
    MembranePotential synapticInput;
    MembranePotential totalCurrent;
    float adaptationVariable;
    
    NeuronIOImpl() : synapticInput(0.0f), totalCurrent(0.0f), adaptationVariable(0.0f) {}
};

}