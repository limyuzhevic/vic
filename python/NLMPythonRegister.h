#include "NLMPythonTypes.h"
#include "NLMPythonEnums.h"
#include "NLMPythonConfig.h"
#include "NLMPythonSensoryInput.h"
#include "NLMPythonMotor.h"
#include "NLMPythonWorld.h"
#include "NLMPythonAgent.h"
#include "NLMPythonBrain.h"
#include "NLMPythonConvenience.h"

void registerTypeDefinitions(py::module_& m) {
    // NeuronId type
    py::class_<NeuronId>(m, "NeuronId", "Unique identifier for a neuron")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        });

    // SynapseId type
    py::class_<SynapseId>(m, "SynapseId", "Unique identifier for a synapse")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    // RegionId type
    py::class_<RegionId>(m, "RegionId", "Unique identifier for a brain region")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    // PopulationId type
    py::class_<PopulationId>(m, "PopulationId", "Unique identifier for a neuron population")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);
}

void registerEnumDefinitions(py::module_& m);
void registerConfigBindings(py::module_& m);
void registerSensoryInputBindings(py::module_& m);
void registerMotorBindings(py::module_& m);
void registerWorldBindings(py::module_& m);
void registerAgentBindings(py::module_& m);
void registerBrainBindings(py::module_& m);
void registerConvenienceFunctions(py::module_& m);
