#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <memory>
#include <string>

#include "../src/core/Types/Types.hpp"

namespace py = pybind11;
namespace nlm {

// id_types.cpp - NeuronId, SynapseId, RegionId, PopulationId
// Purpose: Defines Python bindings for unique identifier types used throughout the NLM system

// Register ID type bindings to the given module
void register_id_types(pybind11::module_& m) {

    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron.
        
        NeuronId represents a specific neuron in the brain with a unique integer identifier.
        Used throughout NLM to reference specific neurons in operations and lookups.
        
        Examples:
            >>> id1 = nlm.NeuronId()
            >>> id2 = nlm.NeuronId(42)
            >>> id1.value
            0
            >>> id2.index()
            42
            >>> id1 == id2
            False
            >>> id2 == nlm.NeuronId(42)
            True
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             R"pbdoc(
             Create a neuron identifier.
             
             Args:
                 value: Integer value for the neuron identifier. Defaults to 0.
             
             Examples:
                 >>> id = nlm.NeuronId(123)
                 >>> id.value
                 123
             )pbdoc")
        .def_readwrite("value", &NeuronId::value,
                      R"pbdoc(The integer value of the neuron identifier)pbdoc")
        .def("index", &NeuronId::index,
             R"pbdoc(Get the zero-based index of this neuron identifier)pbdoc")
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse.
        
        SynapseId represents a specific synaptic connection between neurons in the brain.
        Used to reference individual synapses for plasticity, transmission, and modification.
        
        Examples:
            >>> syn_id1 = nlm.SynapseId()
            >>> syn_id2 = nlm.SynapseId(42)
            >>> syn_id2.value
            42
            >>> syn_id2.index()
            42
            >>> syn_id1 != syn_id2
            True
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             R"pbdoc(
             Create a synapse identifier.
             
             Args:
                 value: Integer value for the synapse identifier. Defaults to 0.
             
             Examples:
                 >>> syn = nlm.SynapseId(123)
                 >>> syn.index()
                 123
             )pbdoc")
        .def_readwrite("value", &SynapseId::value,
                      R"pbdoc(The integer value of the synapse identifier)pbdoc")
        .def("index", &SynapseId::index,
             R"pbdoc(Get the zero-based index of this synapse identifier)pbdoc")
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region.
        
        RegionId represents a specific neural region or area in the brain. Each region
        contains populations of neurons and can have different characteristics. Regions
        are used to organize and manage different functional areas of the brain.
        
        Examples:
            >>> region1 = nlm.RegionId()
            >>> region2 = nlm.RegionId(42)
            >>> region2.index()
            42
            >>> region_ids = [nlm.RegionId(i) for i in range(3)]
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             R"pbdoc(
             Create a region identifier.
             
             Args:
                 value: Integer value for the region identifier. Defaults to 0.
             
             Examples:
                 >>> region = nlm.RegionId(123)
                 >>> region.index()
                 123
             )pbdoc")
        .def_readwrite("value", &RegionId::value,
                      R"pbdoc(The integer value of the region identifier)pbdoc")
        .def("index", &RegionId::index,
             R"pbdoc(Get the zero-based index of this region identifier)pbdoc")
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population.
        
        PopulationId represents a group of neurons that share similar properties or functions.
        Populations are collections of neurons that work together for specific computational
        tasks like processing sensory input, generating motor commands, or maintaining memory.
        
        Examples:
            >>> pop1 = nlm.PopulationId()
            >>> pop2 = nlm.PopulationId(42)
            >>> pop2.index()
            42
            >>> populations = [nlm.PopulationId(i) for i in range(5)]
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             R"pbdoc(
             Create a population identifier.
             
             Args:
                 value: Integer value for the population identifier. Defaults to 0.
             
             Examples:
                 >>> pop = nlm.PopulationId(123)
                 >>> pop.index()
                 123
             )pbdoc")
        .def_readwrite("value", &PopulationId::value,
                      R"pbdoc(The integer value of the population identifier)pbdoc")
        .def("index", &PopulationId::index,
             R"pbdoc(Get the zero-based index of this population identifier)pbdoc")
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);
}

} // namespace nlm