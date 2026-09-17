#pragma once

#include <memory>
#include <string>

namespace nlm {

class Brain;

// Forward declarations for region and neuron IDs
class RegionId;
class NeuronType;
class SimulationStep;
class Timestamp;
class Neuron;
class Synapse;
class SynapseHandle;
class NeuronId;

// Test function declarations
void runBasicConnectivityTest(std::shared_ptr<Brain> brain);
void runPlasticityExperiment(std::shared_ptr<Brain> brain);
void runStdpVerification(std::shared_ptr<Brain> brain);

} // namespace nlm