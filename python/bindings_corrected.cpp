#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0)
        .def(py::init<const NeuronId&>())
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__lt__", &NeuronId::operator<)
        .def("__gt__", &NeuronId::operator>)
        .def("__le__", &NeuronId::operator<=)
        .def("__ge__", &NeuronId::operator>=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        })
        .def_property("value", [](const NeuronId& self) { return self.value; }, 
                     [](NeuronId& self, uint64_t v) { self.value = v; })
        .def_property("index", [](const NeuronId& self) { return self.index(); },
                     nullptr)
        .def("__enter__", [](py::object& self) { return self; })
        .def("__exit__", [](py::object& self, py::args) {});

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0)
        .def(py::init<const SynapseId&>())
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__lt__", &SynapseId::operator<)
        .def("__gt__", &SynapseId::operator>)
        .def("__le__", &SynapseId::operator<=)
        .def("__ge__", &SynapseId::operator>=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        })
        .def_property("value", [](const SynapseId& self) { return self.value; }, 
                     [](SynapseId& self, uint64_t v) { self.value = v; })
        .def_property("index", [](const SynapseId& self) { return self.index(); },
                     nullptr)
        .def("__enter__", [](py::object& self) { return self; })
        .def("__exit__", [](py::object& self, py::args) {});

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0)
        .def(py::init<const RegionId&>())
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__lt__", &RegionId::operator<)
        .def("__gt__", &RegionId::operator>)
        .def("__le__", &RegionId::operator<=)
        .def("__ge__", &RegionId::operator>=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        })
        .def_property("value", [](const RegionId& self) { return self.value; }, 
                     [](RegionId& self, uint64_t v) { self.value = v; })
        .def_property("index", [](const RegionId& self) { return self.index(); },
                     nullptr)
        .def("__enter__", [](py::object& self) { return self; })
        .def("__exit__", [](py::object& self, py::args) {});

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0)
        .def(py::init<const PopulationId&>())
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__lt__", &PopulationId::operator<)
        .def("__gt__", &PopulationId::operator>)
        .def("__le__", &PopulationId::operator<=)
        .def("__ge__", &PopulationId::operator>=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const PopulationId& id) {
            return std::to_string(id.value);
        })
        .def_property("value", [](const PopulationId& self) { return self.value; }, 
                     [](PopulationId& self, uint64_t v) { self.value = v; })
        .def_property("index", [](const PopulationId& self) { return self.index(); },
                     nullptr);

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0)
        .def(py::init<const SynapseId&>())
        .def("__eq__", &SynapseId::operator==)
