// Core types and utilities

#include "bindings.h"

#include "../src/core/Types/Types.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Random/Random.hpp"
#include "../src/core/SimulationClock/SimulationClock.hpp"
#include "../src/core/Logger/Logger.hpp"

namespace nlm {

void bind_core_types(py::module_& m) {
    // Core types
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron.
        
        This class provides:
        - Safe neuron ID manipulation
        - Hash support for use in dictionaries and sets
        - Comparison operators for equality checks
        - Type safety for neuron references
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index, "Get the 0-based index of this neuron")
        .def("__eq__", &NeuronId::operator==, py::is_operator(), "Check equality with another NeuronId")
        .def("__ne__", &NeuronId::operator!=, py::is_operator(), "Check inequality with another NeuronId")
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        });

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse.
        
        This class provides:
        - Safe synapse ID manipulation
        - Hash support for use in dictionaries and sets
        - Comparison operators for equality checks
        - Type safety for synapse references
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index, "Get the 0-based index of this synapse")
        .def("__eq__", &SynapseId::operator==, py::is_operator(), "Check equality with another SynapseId")
        .def("__ne__", &SynapseId::operator!=, py::is_operator(), "Check inequality with another SynapseId")
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region.
        
        This class provides:
        - Safe region ID manipulation
        - Hash support for use in dictionaries and sets
        - Comparison operators for equality checks
        - Type safety for region references
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index, "Get the 0-based index of this region")
        .def("__eq__", &RegionId::operator==, py::is_operator(), "Check equality with another RegionId")
        .def("__ne__", &RegionId::operator!=, py::is_operator(), "Check inequality with another RegionId")
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population.
        
        This class provides:
        - Safe population ID manipulation
        - Hash support for use in dictionaries and sets
        - Comparison operators for equality checks
        - Type safety for population references
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index, "Get the 0-based index of this population")
        .def("__eq__", &PopulationId::operator==, py::is_operator(), "Check equality with another PopulationId")
        .def("__ne__", &PopulationId::operator!=, py::is_operator(), "Check inequality with another PopulationId")
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        });

    // Basic constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm