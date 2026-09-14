#include "NeuronReset.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct NeuronReset::Impl {
    NeuronState state;
    MembranePotential synapticInput;
    std::vector<Timestamp> spikeHistory;
    
    Impl() : state(), synapticInput(0.0f) {}
};

}