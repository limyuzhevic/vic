#pragma once

#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../core/Types/Types.hpp"
#include "../dynamics/SpikeSystem.hpp"

namespace nlm {

// Basic neural connectivity test
void runBasicConnectivityTest(std::shared_ptr<Brain> brain);

// Plasticity learning experiment
void runPlasticityExperiment(std::shared_ptr<Brain> brain);

// STDP verification test
void runStdpVerification(std::shared_ptr<Brain> brain);

} // namespace nlm
