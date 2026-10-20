#ifndef BRAIN_BINDINGS_HPP
#define BRAIN_BINDINGS_HPP

#include <pybind11/pybind11.h>

namespace nlm {
    // Forward declarations
    class Config;
    class Brain;
    class NeuronId;
    class SynapseId;
    class RegionId;
    class PopulationId;
    enum class NeuronType;
    enum class SynapseType;
    enum class DevelopmentalStage;
    enum class FiringState;
}

// Forward declare the module creation function
void bindBrainModule(pybind11::module_ &m);

#endif // BRAIN_BINDINGS_HPP
