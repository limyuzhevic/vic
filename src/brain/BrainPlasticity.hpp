// Brain plasticity header
// Declares functions extracted from Brain.cpp for better organization

#pragma once

#include "../core/Types/Types.hpp"
#include <cstdint>

namespace nlm {

class Brain;

// Forward declarations for types used in plasticity processing
class STDP;
class Hebbian;
class StructuralPlasticity;
class Synapse;
class NeuronId;
class SimulationStep;
class Timestamp;

// Function declarations for brain plasticity processing
void brainPlasticityUpdate(Brain* brain, SimulationStep currentStep, Timestamp currentTime);

} // namespace nlm
