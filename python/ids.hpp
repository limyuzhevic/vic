// ID type definitions for Python bindings
#pragma once

#include <cstdint>
#include <string>
#include <format>
#include "../src/core/Types/Types.hpp"
#include "pybind11/pybind11.h"

namespace py = pybind11;

namespace nlm {

// Note: The actual ID types (NeuronId, SynapseId, RegionId, PopulationId) 
// are defined in ../src/core/Types/Types.hpp. This file provides Python bindings
// that eliminate boilerplate by using template-based configuration for each ID type.
// The template approach allows for consistent binding generation while keeping
// the Python interface clean and consistent.

// Template function to generate bindings for any ID type
// This eliminates the need to duplicate identical boilerplate for each ID type

template<typename IDType>
void bind_id_type(py::module_ &m, const std::string &name) {
    py::class_<IDType>(m, name.c_str(), name.c_str())
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &IDType::value)
        .def("index", &IDType::index)
        .def("__eq__", &IDType::operator==)
        .def("__ne__", &IDType::operator!=)
        .def("__hash__", [](const IDType &id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const IDType &id) {
            std::string className = name;
            // Convert first letter to lowercase for repr
            if (!className.empty()) {
                className[0] = std::tolower(className[0]);
            }
            return "<" + className + ": " + std::to_string(id.value) + ">";
        });
}

// Convenience function to bind all ID types
inline void bind_all_ids(py::module_ &m) {
    bind_id_type<NeuronId>(m, "NeuronId");
    bind_id_type<SynapseId>(m, "SynapseId");  
    bind_id_type<RegionId>(m, "RegionId");
    bind_id_type<PopulationId>(m, "PopulationId");
}

// Constants for invalid IDs
inline void bind_id_constants(py::module_ &m) {
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}
}