#pragma once

namespace nlm {

struct NeuronResetImpl {
    NeuronState state;
    MembranePotential synapticInput;
    std::vector<Timestamp> spikeHistory;
    
    NeuronResetImpl() : state(), synapticInput(0.0f) {}
};

}